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

#include <QMessageBox>
#include <QDebug>
#include <QUndoCommand>
#include <QDoubleValidator>
#include <QtGlobal>
#include <float.h>

#include "filevpzproject.h"
#include "ui_filevpzproject.h"

namespace vle {
namespace gvle2 {

FileVpzProject::FileVpzProject(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzProject), mTab(0), mId(0), mAuthor(0),
    mDate(0), mVersion(0), mName(0), mDuration(0),  mBegin(0), mVpm(0)
{

    ui->setupUi(this);

    mAuthor = ui->authorLineEdit;
    mDate  = ui->dateTimeEdit;
    mVersion = ui->versionEdit;
    mName = ui->nameEdit;
    mDuration = ui->durationEdit;

    QDoubleValidator *durationValidator = new QDoubleValidator(0., DBL_MAX, 3, this);
    durationValidator->setNotation(QDoubleValidator::ScientificNotation);
    mDuration->setValidator(durationValidator);

    mBegin = ui->beginEdit;

    QDoubleValidator *beginValidator = new QDoubleValidator(DBL_MIN, DBL_MAX, 3, this);
    beginValidator->setNotation(QDoubleValidator::ScientificNotation);
    mBegin->setValidator(beginValidator);

    QObject::connect(mAuthor, SIGNAL(editingFinished()),
                     this, SLOT(setAuthorToVpz()) );
    QObject::connect(mDate, SIGNAL(editingFinished()),
                     this, SLOT(setDateToVpz()) );
    QObject::connect(mVersion, SIGNAL(editingFinished()),
                     this, SLOT(setVersionToVpz()) );
    QObject::connect(mName, SIGNAL(editingFinished()),
                     this, SLOT(setExpNameToVpz()) );
    QObject::connect(mDuration, SIGNAL(editingFinished()),
                     this, SLOT(setExpDurationToVpz()) );
    QObject::connect(mBegin, SIGNAL(editingFinished()),
                     this, SLOT(setExpBeginToVpz()) );

}

FileVpzProject::~FileVpzProject()
{
    delete ui;
}

void
FileVpzProject::setVpm(vleVpm* vpm)
{
    mVpm = vpm;

    QObject::connect(mVpm,
            SIGNAL(undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)),
            this,
            SLOT(onUndoRedoVpm(QDomNode, QDomNode, QDomNode, QDomNode)));
    reload();
}

void FileVpzProject::reload()
{
    mAuthor->setText(mVpm->getAuthor());
    mDate->setDateTime(QDateTime::fromString(mVpm->getDate(), "dddd d MMMM yyyy hh:mm"));
    mVersion->setText(mVpm->getVersion());
    mName->setText(mVpm->getExpName());
    mDuration->setText(mVpm->getExpDuration());
    mBegin->setText(mVpm->getExpBegin());
}

void FileVpzProject::setAuthorToVpz()
{
    if (mAuthor->text() != mVpm->getAuthor()) {
        mVpm->setAuthor(mAuthor->text());
    }
}

void FileVpzProject::setDateToVpz()
{
    if (mDate->text() != mVpm->getDate()) {
        mVpm->setDate(mDate->text());
    }
}
void FileVpzProject::setVersionToVpz()
{
    if (mVersion->text() != mVpm->getVersion()) {
        mVpm->setVersion(mVersion->text());
    }
}

void FileVpzProject::setExpNameToVpz()
{
    if (mName->text() != mVpm->getExpName()) {
        mVpm->setExpName(mName->text());
    }
}

void FileVpzProject::setExpDurationToVpz()
{
    if (mDuration->text() != mVpm->getExpDuration()) {
        mVpm->setExpDuration(mDuration->text());
    }
}

void FileVpzProject::setExpBeginToVpz()
{
    if (mBegin->text() != mVpm->getExpBegin()) {
        mVpm->setExpBegin(mBegin->text());
    }
}

void
FileVpzProject::onUndoRedoVpm(QDomNode /*oldVpz*/, QDomNode /*newVpz*/,
        QDomNode /*oldVpm*/, QDomNode /*newVpm*/)
{
    reload();
}

}} //namespaces
