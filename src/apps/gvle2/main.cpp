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

#include "vle/gvle2/gvle2win.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bool result;

    std::string vlePrefixDir = vle::utils::Path::path().getPrefixDir();
    std::string localesPath =
        vle::utils::Path::path().buildDirname(vlePrefixDir,
                                              VLE_SHARE_DIRS,"translations");
    std::string manPath =
        vle::utils::Path::path().buildDirname(vlePrefixDir,
                                              VLE_SHARE_DIRS,"man");

    a.setProperty("localesPath", QString(localesPath.c_str()));
    a.setProperty("manPath",     QString(manPath.c_str()));

    QTranslator qtTranslator;
    result = qtTranslator.load("gvle2_" + QLocale::system().name() + ".qm", localesPath.c_str());
    if (result == false)
    {
        qDebug() << "Load Translator : " <<
            QLocale::system().name() << " not found " <<
            "here : " << localesPath.c_str();
    }
    a.installTranslator(&qtTranslator);

    vle::gvle2::GVLE2Win w;

    w.show();

    result = a.exec();

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
