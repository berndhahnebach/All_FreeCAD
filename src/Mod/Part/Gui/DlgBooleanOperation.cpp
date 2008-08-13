/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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
#include <Gui/Qt4All.h>

#include "DlgBooleanOperation.h"
#include "../App/PartFeature.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Command.h>

using namespace PartGui;

DlgBooleanOperation::DlgBooleanOperation(QWidget* parent, Qt::WFlags fl)
  : QDialog( parent, fl )
{
    this->setupUi(this);
    okButton->setDisabled(true);
    findShapes();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgBooleanOperation::~DlgBooleanOperation()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgBooleanOperation::findShapes()
{
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) return;

    std::vector<App::DocumentObject*> objs = activeDoc->getObjectsOfType
        (Part::Feature::getClassTypeId());
    int index = 1;
    for (std::vector<App::DocumentObject*>::iterator it = objs.begin(); it!=objs.end(); ++it, ++index) {
        firstShape->addItem(QString::fromUtf8((*it)->Label.getValue()));
        firstShape->setItemData(index, QString::fromAscii((*it)->getNameInDocument()));
        secondShape->addItem(QString::fromUtf8((*it)->Label.getValue()));
        secondShape->setItemData(index, QString::fromAscii((*it)->getNameInDocument()));
    }
}

void DlgBooleanOperation::accept()
{
    if (firstShape->currentIndex() == secondShape->currentIndex()) {
        QMessageBox::critical(this, windowTitle(), 
            tr("Cannot perform a boolean operation with the same shape"));
        return;
    }

    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    std::string type, objName;
    if (unionButton->isChecked()) {
        type = "Part::Fuse";
        objName = activeDoc->getUniqueObjectName("Fusion");
    }
    else if (interButton->isChecked()) {
        type = "Part::Common";
        objName = activeDoc->getUniqueObjectName("Common");
    }
    else if (diffButton->isChecked()) {
        type = "Part::Cut";
        objName = activeDoc->getUniqueObjectName("Cut");
    }
    else if (sectionButton->isChecked()) {
        type = "Part::Section";
        objName = activeDoc->getUniqueObjectName("Section");
    }

    std::string shapeOne, shapeTwo;
    shapeOne = (const char*)firstShape->itemData(firstShape->currentIndex()).toByteArray();
    shapeTwo = (const char*)secondShape->itemData(secondShape->currentIndex()).toByteArray();

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
    
    QDialog::accept();
}

void DlgBooleanOperation::on_firstShape_activated(int index)
{
    okButton->setEnabled(index > 0 && secondShape->currentIndex() > 0);
}

void DlgBooleanOperation::on_secondShape_activated(int index)
{
    okButton->setEnabled(index > 0 && firstShape->currentIndex() > 0);
}

#include "moc_DlgBooleanOperation.cpp"
