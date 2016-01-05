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
#ifndef GVLE2_VLEPACKAGE_H
#define GVLE2_VLEPACKAGE_H

#include <QDir>
#include <QMap>
#include <QPluginLoader>
#include <QString>

#include "plugin_modeler.h"
#include "plugin_output.h"
#include "plugin_cond.h"
#include "plugin_modeler.h"
#include "sourcecpp.h"

#ifndef Q_MOC_RUN
#include <vle/utils/Package.hpp>
#endif

namespace vle {
namespace gvle2 {

class vlePackage
{
public:
    vlePackage();
    vlePackage(QString path);
    QString getName();
    void    setStdPackage(vle::utils::Package* pkg);
    vle::utils::Package* getStdPackage();
    QString getSrcFilePath(QString name);

    // Functions used to manage cpp source files
public:
    sourceCpp *openSourceCpp(QString filename);
    void       closeSourceCpp(sourceCpp *src);
private slots:
    void       sourceDestroyed(QObject *obj);

public: // Functions used for modeler plugins
    QStringList    getListOfCondPlugins();
    void           addModeler(QString filename);
    PluginModeler* getModelerByPlugin(QString pluginName);
    PluginModeler* getModelerByClass(QString className);
    int            getModelerCount();
    QString        getModelerName(int pos);
    int            getModelerClassCount();
    QString        getModelerClass(int pos);
    QString        getModelerClassPlugin(QString className);

public: // Functions used by Experimental Conditions plugins
    void    addExpPlugin(QString name, QString fileName);
    PluginExpCond* getCondPlugin(QString name);
    QString getExpPluginName(int index);
    int     expPluginCount();
    void addOutputPlugin(QString name, QString fileName);
    PluginOutput *getOutputPlugin(QString name);

protected:
    void    refreshModelerClass();
private:
    QDir mDir;
    QMap<QString,sourceCpp*>      mSourceCpp;
    QMap<sourceCpp*,int>          mSourceCppUsage;
    vle::utils::Package*          mStdPackage;
    QMap <QString,QString>        mModelers;
    QMap <QString,QString>        mModelerClass;
    QMap <QString,QPluginLoader*> mModelerLoader;
    QMap <QString,QString>        mCondPlugins;
    QMap <QString,QString>        mOutputPlugins;
};

}} // namespaces

#endif // VLEPACKAGE_H
