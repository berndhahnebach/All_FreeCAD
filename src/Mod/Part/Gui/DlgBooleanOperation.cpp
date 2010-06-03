/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QMessageBox>
# include <TopoDS_Shape.hxx>
#endif

#include "DlgBooleanOperation.h"
#include "ui_DlgBooleanOperation.h"
#include "../App/PartFeature.h"
#include <Base/Exception.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Command.h>
#include <Gui/Selection.h>
#include <Gui/WaitCursor.h>

using namespace PartGui;

/* TRANSLATOR PartGui::DlgBooleanOperation */

DlgBooleanOperation::DlgBooleanOperation(QWidget* parent)
  : QWidget(parent), ui(new Ui_DlgBooleanOperation)
{
    ui->setupUi(this);
    findShapes();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgBooleanOperation::~DlgBooleanOperation()
{
    // no need to delete child widgets, Qt does it all for us
    delete ui;
}

void DlgBooleanOperation::findShapes()
{
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) return;

    std::vector<App::DocumentObject*> objs = activeDoc->getObjectsOfType
        (Part::Feature::getClassTypeId());
    int index = 1; // the first item marks an invalid entry
    int index_sel1=0, index_sel2=0;
    for (std::vector<App::DocumentObject*>::iterator it = objs.begin(); it!=objs.end(); ++it) {
        const TopoDS_Shape& shape = static_cast<Part::Feature*>(*it)->Shape.getValue();
        //if (!shape.IsNull()) {
        //    TopAbs_ShapeEnum type = shape.ShapeType();
        //    if (type == TopAbs_SOLID) {
                if (index_sel1 == 0 && Gui::Selection().isSelected(*it))
                    index_sel1 = index;
                else if (index_sel1 > 0 && index_sel2 == 0 && Gui::Selection().isSelected(*it))
                    index_sel2 = index;
                ui->firstShape->addItem(QString::fromUtf8((*it)->Label.getValue()));
                ui->firstShape->setItemData(index, QString::fromAscii((*it)->getNameInDocument()));
                ui->secondShape->addItem(QString::fromUtf8((*it)->Label.getValue()));
                ui->secondShape->setItemData(index, QString::fromAscii((*it)->getNameInDocument()));
                index++;
        //    }
        //}
    }

    ui->firstShape->setCurrentIndex(index_sel1);
    ui->secondShape->setCurrentIndex(index_sel2);
}

void DlgBooleanOperation::on_swapButton_clicked()
{
    int index_sel1 = ui->firstShape->currentIndex();
    int index_sel2 = ui->secondShape->currentIndex();
    ui->firstShape->setCurrentIndex(index_sel2);
    ui->secondShape->setCurrentIndex(index_sel1);
}

void DlgBooleanOperation::accept()
{
    if (ui->firstShape->currentIndex() == 0 ||
        ui->secondShape->currentIndex() == 0) {
        QMessageBox::critical(this, windowTitle(), 
            tr("Select a shape first"));
        return;
    }
    if (ui->firstShape->currentIndex() == ui->secondShape->currentIndex()) {
        QMessageBox::critical(this, windowTitle(), 
            tr("Cannot perform a boolean operation with the same shape"));
        return;
    }

    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    std::string type, objName;
    if (ui->unionButton->isChecked()) {
        type = "Part::Fuse";
        objName = activeDoc->getUniqueObjectName("Fusion");
    }
    else if (ui->interButton->isChecked()) {
        type = "Part::Common";
        objName = activeDoc->getUniqueObjectName("Common");
    }
    else if (ui->diffButton->isChecked()) {
        type = "Part::Cut";
        objName = activeDoc->getUniqueObjectName("Cut");
    }
    else if (ui->sectionButton->isChecked()) {
        type = "Part::Section";
        objName = activeDoc->getUniqueObjectName("Section");
    }

    std::string shapeOne, shapeTwo;
    shapeOne = (const char*)ui->firstShape->itemData(ui->firstShape->currentIndex()).toByteArray();
    shapeTwo = (const char*)ui->secondShape->itemData(ui->secondShape->currentIndex()).toByteArray();

    try {
        Gui::WaitCursor wc;
        activeDoc->openTransaction("Boolean operation");
        Gui::Command::doCommand(Gui::Command::Doc,
            "App.activeDocument().addObject(\"%s\",\"%s\")",
            type.c_str(), objName.c_str());
        Gui::Command::doCommand(Gui::Command::Doc,
            "App.activeDocument().%s.Base = App.activeDocument().%s",
            objName.c_str(),shapeOne.c_str());
        Gui::Command::doCommand(Gui::Command::Doc,
            "App.activeDocument().%s.Tool = App.activeDocument().%s",
            objName.c_str(),shapeTwo.c_str());
        Gui::Command::doCommand(Gui::Command::Gui,
            "Gui.activeDocument().hide(\"%s\")",shapeOne.c_str());
        Gui::Command::doCommand(Gui::Command::Gui,
            "Gui.activeDocument().hide(\"%s\")",shapeTwo.c_str());
        activeDoc->commitTransaction();
        activeDoc->recompute();
    }
    catch (const Base::Exception& e) {
        e.ReportException();
    }
}

// ---------------------------------------

TaskBooleanOperation::TaskBooleanOperation()
{
    widget = new DlgBooleanOperation();
    taskbox = new Gui::TaskView::TaskBox(QPixmap(), widget->windowTitle(), true, 0);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
}

TaskBooleanOperation::~TaskBooleanOperation()
{
    // automatically deleted in the sub-class
}

void TaskBooleanOperation::clicked(int id)
{
    if (id == QDialogButtonBox::Apply) {
        widget->accept();
    }
}

#include "moc_DlgBooleanOperation.cpp"
