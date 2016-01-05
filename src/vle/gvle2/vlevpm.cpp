/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vlevpm.h"
#include "plugin_cond.h"
#include "plugin_output.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>

namespace vle {
namespace gvle2 {

vleDomVpm::vleDomVpm(QDomDocument* doc): vleDomObject(doc)

{

}
vleDomVpm::~vleDomVpm()
{

}
QString
vleDomVpm::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    //element uniq in children
    if ((name == "condPlugins") or
        (name == "outputGUIplugins")) {
        return getXQuery(node.parentNode())+"/"+name;
    }
    //element identified by attribute name
    if ((name == "condPlugin") or
        (name == "outputGUIplugin")){
        return getXQuery(node.parentNode())+"/"+name+"[@name=\""
                +attributeValue(node,"name")+"\"]";
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomVpm::getNodeFromXQuery(const QString& query,
        QDomNode d)
{
    //handle last
    if (query.isEmpty()) {
        return d;
    }

    QStringList queryList = query.split("/");
    QString curr = "";
    QString rest = "";
    int j=0;
    if (queryList.at(0) == ".") {
        curr = queryList.at(1);
        j=2;
    } else {
        curr = queryList.at(0);
        j=1;
    }
    for (int i=j; i<queryList.size(); i++) {
        rest = rest + queryList.at(i);
        if (i < queryList.size()-1) {
            rest = rest + "/";
        }
    }
    //handle first
    if (d.isNull()) {
        QDomNode rootNode = mDoc->documentElement();
        if (curr != "vle_project_metadata" or queryList.at(0) != ".") {
            return QDomNode();
        }
        return(getNodeFromXQuery(rest,rootNode));
    }

    //handle recursion with uniq node
    if ((curr == "condPlugins") or
        (curr == "outputGUIplugins")){
        return getNodeFromXQuery(rest, obtainChild(d, curr, true));
    }
    //handle recursion with nodes identified by name
    std::vector<QString> nodeByNames;
    nodeByNames.push_back(QString("condPlugin"));
    nodeByNames.push_back(QString("outputGUIplugin"));
    QString selNodeByName ="";
    for (unsigned int i=0; i< nodeByNames.size(); i++) {
        if (curr.startsWith(nodeByNames[i])) {
            selNodeByName = nodeByNames[i];
        }
    }
    if (not selNodeByName.isEmpty()) {
        QStringList currSplit = curr.split("\"");
        QDomNode selMod = childWhithNameAttr(d, selNodeByName, currSplit.at(1));
        return getNodeFromXQuery(rest,selMod);
    }
    return QDomNode();
}

/************************************************************************/

vleVpm::vleVpm(const QString& vpzpath, const QString& vpmpath):
        vleVpz(vpzpath), mDocVpm(0), mFileNameVpm(vpmpath), mVdoVpm(0),
        undoStackVpm(0), waitUndoRedoVpz(false), waitUndoRedoVpm(false),
        oldValVpz(), newValVpz(), oldValVpm(), newValVpm(), plugins()
{
    QFile file(mFileNameVpm);
    if (file.exists()) {
        mDocVpm = new QDomDocument("vle_project_metadata");
        QXmlInputSource source(&file);
        QXmlSimpleReader reader;
        mDocVpm->setContent(&source, &reader);
    } else {
        xCreateDom();
    }

    plugins.loadPlugins();
    mVdoVpm = new vleDomVpm(mDocVpm);
    undoStackVpm = new vleDomDiffStack(mVdoVpm);
    undoStackVpm->init(*mDocVpm);


    QObject::connect(vleVpz::undoStack,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoVpz(QDomNode, QDomNode)));
    QObject::connect(vleVpz::undoStack,
                SIGNAL(snapshotVdo(QDomNode, bool)),
                this, SLOT(onSnapshotVpz(QDomNode, bool)));
    QObject::connect(undoStackVpm,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoVpm(QDomNode, QDomNode)));
    QObject::connect(undoStackVpm,
                SIGNAL(snapshotVdo(QDomNode, bool)),
                this, SLOT(onSnapshotVpm(QDomNode, bool)));

}

QString
vleVpm::toQString(const QDomNode& node) const
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
}

void
vleVpm::xCreateDom()
{
    if (not mDocVpm) {
        mDocVpm = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDocVpm->createProcessingInstruction("xml",
                "version=\"1.0\" encoding=\"UTF-8\" ");
        mDocVpm->appendChild(pi);

        QDomElement vpmRoot = mDocVpm->createElement("vle_project_metadata");
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "me");
        QDomElement xCondPlug = mDocVpm->createElement("condPlugins");
        vpmRoot.appendChild(xCondPlug);
        QDomElement xOutPlug = mDocVpm->createElement("outputGUIplugins");
        vpmRoot.appendChild(xOutPlug);
        mDocVpm->appendChild(vpmRoot);
    }
}

void
vleVpm::xReadDom()
{
    if (mDocVpm) {
        QFile file(mFileNameVpm);
        mDocVpm->setContent(&file);
    }
}

void
vleVpm::setCondGUIplugin(const QString& condName, const QString& name)
{
    xCreateDom();

    QDomElement docElem = mDocVpm->documentElement();

    QDomNode condsPlugins =
            mDocVpm->elementsByTagName("condPlugins").item(0);
    undoStackVpm->snapshot(condsPlugins);
    QDomNodeList plugins =
            condsPlugins.toElement().elementsByTagName("condPlugin");
    for (unsigned int i =0; i< plugins.length(); i++) {
        QDomNode plug = plugins.at(i);
        for (int j=0; j< plug.attributes().size(); j++) {
            if ((plug.attributes().item(j).nodeName() == "cond") and
                    (plug.attributes().item(j).nodeValue() == condName))  {
                plug.toElement().setAttribute("plugin", name);
                return;
            }
        }
    }
    QDomElement el = mDocVpm->createElement("condPlugin");
    el.setAttribute("cond", condName);
    el.setAttribute("plugin", name);
    condsPlugins.appendChild(el);


}

void
vleVpm::setOutputGUIplugin(const QString& viewName, const QString& pluginName)
{
    xCreateDom();

    QDomElement docElem = mDocVpm->documentElement();
    QDomNode outGUIplugs =
            mDocVpm->elementsByTagName("outputGUIplugins").item(0);
    if (outGUIplugs.isNull() ) {
        QDomNode metaData = mDocVpm->elementsByTagName(
                "vle_project_metadata").item(0);
        undoStackVpm->snapshot(metaData);
        metaData.appendChild(mDocVpm->createElement("outputGUIplugins"));
        outGUIplugs =  mDocVpm->elementsByTagName("outputGUIplugins").item(0);
    } else {
        undoStackVpm->snapshot(outGUIplugs);
    }
    QDomNodeList plugins =
            outGUIplugs.toElement().elementsByTagName("outputGUIplugin");
    for (unsigned int i =0; i< plugins.length(); i++) {
        QDomNode plug = plugins.at(i);
        for (int j=0; j< plug.attributes().size(); j++) {
            if ((plug.attributes().item(j).nodeName() == "view") and
                    (plug.attributes().item(j).nodeValue() == viewName))  {
                plug.toElement().setAttribute("GUIplugin", pluginName);
                return;
            }
        }
    }
    QDomElement el = mDocVpm->createElement("outputGUIplugin");
    el.setAttribute("view", viewName);
    el.setAttribute("GUIplugin", pluginName);
    outGUIplugs.appendChild(el);
}

void
vleVpm::addConditionFromPluginToDoc(const QString& condName,
        const QString& pluginName)
{
    //update vpm with snapshot but without signal
    bool oldBlock = undoStackVpm->blockSignals(true);
    setCondGUIplugin(condName, pluginName);
    undoStackVpm->blockSignals(oldBlock);

    //update vpz with snapshot but without signal
    oldBlock = vleVpz::undoStack->blockSignals(true);
    vleVpz::addConditionToDoc(condName);
    provideCondGUIplugin(condName);
    vleVpz::undoStack->blockSignals(oldBlock);
}

void
vleVpm::renameConditionToDoc(const QString& oldName, const QString& newName)
{
    QString condPlugin = getCondGUIplugin(oldName);
    if (condPlugin == "") {
        vleVpz::renameConditionToDoc(oldName, newName);
    } else {
        //update vpz with snapshot bu without signal
        bool oldBlock = vleVpz::undoStack->blockSignals(true);
        vleVpz::renameConditionToDoc(oldName, newName);
        vleVpz::undoStack->blockSignals(oldBlock);
        //update vpm with snapshot but without signal
        oldBlock = undoStackVpm->blockSignals(true);
        setCondGUIplugin(oldName, "");
        setCondGUIplugin(newName, condPlugin);
        undoStackVpm->blockSignals(oldBlock);
    }
}

bool
vleVpm::setOutputPluginToDoc(const QString& viewName,
        const QString& outputPlugin)
{
    //TODO, ne devrait pas etre en dur
    QString guiPluginName("");
    if (outputPlugin == "vle.output/storage") {
        guiPluginName = "gvle2.output/storage";
    }
    if (outputPlugin == "vle.output/file") {
        guiPluginName = "gvle2.output/file";
    }
    //update vpm with snapshot but without signal
    bool oldBlock = undoStackVpm->blockSignals(true);
    setOutputGUIplugin(viewName, guiPluginName);
    undoStackVpm->blockSignals(oldBlock);

    //update in vpz with snapshot but without signal
    oldBlock = vleVpz::undoStack->blockSignals(true);
    bool res = vleVpz::setOutputPluginToDoc(viewName, outputPlugin);
    provideOutputGUIplugin(viewName);
    vleVpz::undoStack->blockSignals(oldBlock);

    return res;
}

PluginOutput*
vleVpm::provideOutputGUIplugin(const QString& viewName)
{
    QString guiPluginName = getOutputGUIplugin(viewName);
    if (guiPluginName == "") {
        return 0;
    } else {
        QString libName = plugins.getOutputPluginPath(guiPluginName);
        QPluginLoader loader(libName);
        PluginOutput* plugin = qobject_cast<PluginOutput*>(loader.instance());
        bool enableSnapshot = vleVpz::undoStack->enableSnapshot(false);
        bool oldBlock = undoStackVpm->blockSignals(true);
        bool oldBlock2 = vleVpz::undoStack->blockSignals(true);
        plugin->init(this, viewName);
        vleVpz::undoStack->blockSignals(oldBlock2);
        undoStackVpm->blockSignals(oldBlock);
        vleVpz::undoStack->enableSnapshot(enableSnapshot);
        return plugin;
    }
}

PluginExpCond*
vleVpm::provideCondGUIplugin(const QString& condName)
{
    QString guiPluginName = getCondGUIplugin(condName);
    if (guiPluginName == "") {
        return 0;
    } else {
        QString libName = plugins.getCondPluginPath(guiPluginName);
        QPluginLoader loader(libName);
        PluginExpCond* plugin = qobject_cast<PluginExpCond*>(loader.instance());
        bool enableSnapshot = vleVpz::undoStack->enableSnapshot(false);
        bool oldBlock = undoStackVpm->blockSignals(true);
        bool oldBlock2 = vleVpz::undoStack->blockSignals(true);
        plugin->init(this, condName);
        vleVpz::undoStack->blockSignals(oldBlock2);
        undoStackVpm->blockSignals(oldBlock);
        vleVpz::undoStack->enableSnapshot(enableSnapshot);
        return plugin;
    }
}

void
vleVpm::addViewToDoc(const QString& viewName)
{
    //update vpz with snapshot but without signal
    bool oldBlock = vleVpz::undoStack->blockSignals(true);
    vleVpz::addViewToDoc(viewName);
    vleVpz::undoStack->blockSignals(oldBlock);

    //update vpm with snapshot but without signal
    QString outputPlugin = vleVpz::getOutputPluginFromDoc(viewName);
    QString guiPluginName("");
    if (outputPlugin == "vle.output/storage") {
        guiPluginName = "gvle2.output/storage";
    }
    if (outputPlugin == "vle.output/file") {
        guiPluginName = "gvle2.output/file";
    }
    oldBlock = undoStackVpm->blockSignals(true);
    setOutputGUIplugin(viewName, guiPluginName);
    undoStackVpm->blockSignals(oldBlock);

    //build plugin and initialize it with the view
    provideOutputGUIplugin(viewName);
}

void
vleVpm::renameViewToDoc(const QString& oldName, const QString& newName)
{
    //update in vpz with snapshot without signal
    bool oldBlock = vleVpz::undoStack->blockSignals(true);
    vleVpz::renameViewToDoc(oldName, newName);
    vleVpz::undoStack->blockSignals(oldBlock);

    //update vpm with snapshot but without signal
    oldBlock = undoStackVpm->blockSignals(true);
    QString outputGUIplugin = getOutputGUIplugin(oldName);
    setOutputGUIplugin(oldName, "");
    setOutputGUIplugin(newName, outputGUIplugin);
    undoStackVpm->blockSignals(oldBlock);
}

void
vleVpm::rmConditionToDoc(const QString& condName)
{
    QString condPlugin = getCondGUIplugin(condName);
    if (condPlugin == "") {
        vleVpz::rmConditionToDoc(condName);
    } else {
        //update vpz with snapshot but without signal
        bool oldBlock = vleVpz::undoStack->blockSignals(true);
        vleVpz::rmConditionToDoc(condName);
        vleVpz::undoStack->blockSignals(oldBlock);
        //update vpm with snapshot but without signal
        oldBlock = undoStackVpm->blockSignals(true);
        setCondGUIplugin(condName, "");
        undoStackVpm->blockSignals(oldBlock);
    }
}

QString
vleVpm::getCondGUIplugin(const QString& condName)
{

    QStringList res;
    if (mDocVpm) {
        QDomElement docElem = mDocVpm->documentElement();
        QDomNode condsPlugins =
                mDocVpm->elementsByTagName("condPlugins").item(0);
        QDomNodeList plugins =
                condsPlugins.toElement().elementsByTagName("condPlugin");
        for (unsigned int i =0; i< plugins.length(); i++) {
            QDomNode plug = plugins.item(i);
            if (plug.attributes().contains("cond") and
                (plug.attributes().namedItem("cond").nodeValue() == condName)) {
                return plug.attributes().namedItem("plugin").nodeValue();
            }
        }
    }
    return "";
}

QString
vleVpm::getOutputGUIplugin(const QString& viewName)
{

    QStringList res;
    if (mDocVpm) {
        QDomElement docElem = mDocVpm->documentElement();
        QDomNode condsPlugins =
                mDocVpm->elementsByTagName("outputGUIplugins").item(0);
        QDomNodeList plugins =
                condsPlugins.toElement().elementsByTagName("outputGUIplugin");
        for (unsigned int i =0; i< plugins.length(); i++) {
            QDomNode plug = plugins.item(i);
            if (plug.attributes().contains("view") and
                (plug.attributes().namedItem("view").nodeValue() == viewName)) {
                return plug.attributes().namedItem("GUIplugin").nodeValue();
            }
        }
    }
    return "";
}

void
vleVpm::setCurrentTab(QString tabName)
{
    undoStackVpm->current_source = tabName;
    vleVpz::undoStack->current_source = tabName;
}

void
vleVpm::save()
{
    //save vpz
    vleVpz::save();
    //save vpm
    QFile file(mFileNameVpm);
    QFileInfo fileInfo(file);
    if (not fileInfo.dir().exists()) {
        if (not QDir().mkpath(fileInfo.dir().path())) {
            qDebug() << "Cannot create dir " << fileInfo.dir().path() ;
            return;
        }
    }
    if (not file.exists()) {
        if (not file.open(QIODevice::WriteOnly)) {
            qDebug() << "VPM File (" << mFileNameVpm << ")can't be opened for write !";
            return;
        }
        file.close();
    }
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QByteArray xml = mDocVpm->toByteArray();
    file.write(xml);
    file.close();
}

void
vleVpm::undo()
{
    waitUndoRedoVpz = true;
    waitUndoRedoVpm = true;
    undoStackVpm->undo();
    vleVpz::undoStack->undo();

}
void
vleVpm::redo()
{
    waitUndoRedoVpz = true;
    waitUndoRedoVpm = true;
    undoStackVpm->redo();
    vleVpz::undoStack->redo();
}

void
vleVpm::onSnapshotVpz(QDomNode /*snapVpz*/, bool isMerged)
{
    if (not isMerged) {
        bool oldBlock = undoStackVpm->blockSignals(true);
        undoStackVpm->snapshot(QDomNode());
        undoStackVpm->blockSignals(oldBlock);
    }

}

void
vleVpm::onSnapshotVpm(QDomNode /*snapVpm*/, bool isMerged)
{
    if (not isMerged) {
        bool oldBlock = vleVpz::undoStack->blockSignals(true);
        vleVpz::undoStack->snapshot(QDomNode());
        vleVpz::undoStack->blockSignals(oldBlock);
    }

}
void
vleVpm::onUndoRedoVpm(QDomNode oldVpm, QDomNode newVpm)
{
    oldValVpm = oldVpm;
    newValVpm = newVpm;
    waitUndoRedoVpm = false;
    if (not waitUndoRedoVpz ) {
        emit undoRedo(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}


void
vleVpm::onUndoRedoVpz(QDomNode oldVpz, QDomNode newVpz)
{
    oldValVpz = oldVpz;
    newValVpz = newVpz;
    waitUndoRedoVpz = false;
    if (not waitUndoRedoVpm ) {
        emit undoRedo(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}


}}//namespaces
