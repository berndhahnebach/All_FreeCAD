/***************************************************************************
 *   Copyright (c) 2011 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
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

#include "ui_TaskPadParameters.h"
#include "TaskPadParameters.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Console.h>
#include <Gui/Selection.h>
#include <Gui/Command.h>
#include <Mod/PartDesign/App/FeaturePad.h>


using namespace PartDesignGui;
using namespace Gui;

/* TRANSLATOR PartDesignGui::TaskPadParameters */

TaskPadParameters::TaskPadParameters(ViewProviderPad *PadView,QWidget *parent)
    : TaskBox(Gui::BitmapFactory().pixmap("PartDesign_Pad"),tr("Pad parameters"),true, parent),PadView(PadView)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskPadParameters();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);

    PartDesign::Pad* pad = static_cast<PartDesign::Pad*>(PadView->getObject());
    double l = pad->Length.getValue();
    bool mirrored = pad->MirroredExtent.getValue();
    bool reversed = pad->Reversed.getValue();

    ui->doubleSpinBox->setValue(l);
    ui->doubleSpinBox->selectAll();
    ui->checkBoxMirrored->setChecked(mirrored);
    ui->checkBoxReversed->setChecked(reversed);

    setFocus ();
}

double TaskPadParameters::getLength(void) const
{
    return ui->doubleSpinBox->value();
}

bool   TaskPadParameters::getReversed(void) const
{
    return ui->checkBoxReversed->isChecked();
}

bool   TaskPadParameters::getMirroredExtent(void) const
{
    return ui->checkBoxMirrored->isChecked();
}

TaskPadParameters::~TaskPadParameters()
{
    delete ui;
}

void TaskPadParameters::changeEvent(QEvent *e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgPadParameters::TaskDlgPadParameters(ViewProviderPad *PadView)
    : TaskDialog(),PadView(PadView)
{
    assert(PadView);
    parameter  = new TaskPadParameters(PadView);

    Content.push_back(parameter);
}

TaskDlgPadParameters::~TaskDlgPadParameters()
{

}

//==== calls from the TaskView ===============================================================


void TaskDlgPadParameters::open()
{
    
}

void TaskDlgPadParameters::clicked(int)
{
    
}

bool TaskDlgPadParameters::accept()
{
    std::string name = PadView->getObject()->getNameInDocument();

    Gui::Command::openCommand("Pad changed");
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.Length = %f",name.c_str(),parameter->getLength());
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.Reversed = %i",name.c_str(),parameter->getReversed()?1:0);
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.MirroredExtent = %i",name.c_str(),parameter->getMirroredExtent()?1:0);
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.recompute()");
    Gui::Command::doCommand(Gui::Command::Gui,"Gui.activeDocument().resetEdit()");
    Gui::Command::commitCommand();

    return true;
}

bool TaskDlgPadParameters::reject()
{
    //Gui::Command::openCommand("Pad changed");
    Gui::Command::doCommand(Gui::Command::Gui,"Gui.activeDocument().resetEdit()");
    //Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.recompute()");
    //Gui::Command::commitCommand();

    return true;
}



#include "moc_TaskPadParameters.cpp"
