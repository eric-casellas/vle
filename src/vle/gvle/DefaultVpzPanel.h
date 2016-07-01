/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
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

#ifndef GVLE_DEFAULT_VPZ_PANEL_H
#define GVLE_DEFAULT_VPZ_PANEL_H

#include <QWidget>
#include <QObject>
#include <QDebug>

#ifndef Q_MOC_RUN
#include "vlevpm.h"
#include "filevpzview.h"
#include "ui_filevpzview.h"
#include "filevpzrtool.h"
#include "plugin_mainpanel.h"
#include "plugin_simpanel.h"
#endif

namespace vle {
namespace gvle {


class DefaultVpzPanel : public PluginMainPanel
{
    Q_OBJECT
public:
    DefaultVpzPanel();
    virtual ~DefaultVpzPanel();
    QString  getname() override;
    QWidget* leftWidget() override;
    QWidget* rightWidget() override;
    void undo() override;
    void redo() override;
    void init(QString& file, utils::Package* pkg, Logger*, gvle_plugins* plugs,
              const utils::ContextPtr& ctx) override;
    QString canBeClosed() override;
    void save() override;
    void setSimLeftWidget(QWidget*);
    PluginMainPanel* newInstance() override {return 0;}

public slots:
    void onCurrentChanged(int index);
    void onUndoAvailable(bool);
    void onRightSimWidgetChanged();

public:
    fileVpzView*  m_vpzview;
    FileVpzRtool* m_rtool;
    gvle_plugins*  mGvlePlugins;
};

}} //namespaces


#endif
