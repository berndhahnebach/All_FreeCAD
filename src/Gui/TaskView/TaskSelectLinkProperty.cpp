/***************************************************************************
 *   Copyright (c) 2010 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <algorithm>
# include <boost/bind.hpp>
#endif

#include "ui_TaskSelectLinkProperty.h"
#include "TaskSelectLinkProperty.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Console.h>

using namespace Gui::TaskView;

TaskSelectLinkProperty::TaskSelectLinkProperty(QWidget *parent)
    : TaskBox(Gui::BitmapFactory().pixmap("mouse_pointer"),tr("edit selection"),true, parent)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskSelectLinkProperty();
    ui->setupUi(proxy);

    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
    Gui::Selection().Attach(this);

    ui->Remove->setIcon(Gui::BitmapFactory().pixmap("delete"));
    ui->Add->setIcon(Gui::BitmapFactory().pixmap("add"));
    ui->Invert->setIcon(Gui::BitmapFactory().pixmap("edit_remove"));
    ui->Help->setIcon(Gui::BitmapFactory().pixmap("help-browser"));

}

TaskSelectLinkProperty::~TaskSelectLinkProperty()
{
    delete ui;
    Gui::Selection().Detach(this);
}

/// @cond DOXERR
void TaskSelectLinkProperty::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                              Gui::SelectionSingleton::MessageType Reason)
{
    if (Reason.Type == SelectionChanges::AddSelection ||
        Reason.Type == SelectionChanges::RmvSelection ||
        Reason.Type == SelectionChanges::SetSelection ||
        Reason.Type == SelectionChanges::ClrSelection) {
        //std::vector<Gui::ViewProvider*> views = getSelection();
    }
}
/// @endcond


void TaskSelectLinkProperty::on_Remove_clicked(bool)
{
}

void TaskSelectLinkProperty::on_Add_clicked(bool)
{
}

void TaskSelectLinkProperty::on_Invert_clicked(bool)
{
}

void TaskSelectLinkProperty::on_Help_clicked(bool)
{
}



#include "moc_TaskSelectLinkProperty.cpp"
