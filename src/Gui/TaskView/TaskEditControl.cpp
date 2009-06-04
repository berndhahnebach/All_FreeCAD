/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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
#endif

#include "ui_TaskEditControl.h"
#include "TaskEditControl.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Console.h>

using namespace Gui::TaskView;

TaskEditControl::TaskEditControl(QWidget *parent)
    : TaskWidget(parent)
{
    ui = new Ui_TaskEditControl();
    ui->setupUi(this);
    QMetaObject::connectSlotsByName(this);

    Gui::Selection().Attach(this);
}

TaskEditControl::~TaskEditControl()
{
    delete ui;
    Gui::Selection().Detach(this);
}

void TaskEditControl::changeEvent(QEvent *e)
{
    TaskWidget::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void TaskEditControl::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                               Gui::SelectionSingleton::MessageType Reason)
{
    if (Reason.Type == SelectionChanges::AddSelection ||
        Reason.Type == SelectionChanges::RmvSelection ||
        Reason.Type == SelectionChanges::ClrSelection) {
        //std::vector<Gui::ViewProvider*> views = getSelection();
    }
}



#include "moc_TaskEditControl.cpp"
