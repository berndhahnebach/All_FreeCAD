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

#include <Base/Interpreter.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>

#include "DlgPrimitives.h"

using namespace PartGui;

DlgPrimitives::DlgPrimitives(QWidget* parent, Qt::WFlags fl)
  : QDialog( parent, fl )
{
    this->setupUi(this);
    Gui::Command::doCommand(Gui::Command::Doc, "import Part,PartGui");
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgPrimitives::~DlgPrimitives()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgPrimitives::accept()
{
    try {
        QString cmd; QString name;
        App::Document* doc = App::GetApplication().getActiveDocument();
        if (!doc) {
            QMessageBox::warning(this, tr("Create %1").arg(comboBox1->currentText()), tr("No active document"));
            return;
        }
        if (comboBox1->currentIndex() == 0) {         // plane
            QMessageBox::warning(this, "Plane", "Not implemented yet");
        }
        else if (comboBox1->currentIndex() == 1) {         // box
            name = doc->getUniqueObjectName("Box").c_str();
            cmd = QString(
                "App.ActiveDocument.addObject(\"Part::Box\",\"%1\")\n"
                "App.ActiveDocument.%2.l=%3\n"
                "App.ActiveDocument.%4.w=%5\n"
                "App.ActiveDocument.%6.h=%7\n")
                .arg(name).arg(name).arg(boxLength->value(),0,'f',2)
                .arg(name).arg(boxWidth->value(),0,'f',2)
                .arg(name).arg(boxHeight->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 2) {  // cylinder
            name = doc->getUniqueObjectName("Cylinder").c_str();
            cmd = QString(
                "App.ActiveDocument.addObject(\"Part::Cylinder\",\"%1\")\n"
                "App.ActiveDocument.%2.Radius=%3\n"
                "App.ActiveDocument.%4.Height=%5\n"
                "App.ActiveDocument.%6.Angle=%7\n")
                .arg(name).arg(name).arg(cylinderRadius->value(),0,'f',2)
                .arg(name).arg(cylinderHeight->value(),0,'f',2)
                .arg(name).arg(cylinderAngle->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 3) {  // cone
            name = doc->getUniqueObjectName("Cone").c_str();
            cmd = QString(
                "App.ActiveDocument.addObject(\"Part::Cone\",\"%1\")\n"
                "App.ActiveDocument.%2.Radius1=%3\n"
                "App.ActiveDocument.%4.Radius2=%5\n"
                "App.ActiveDocument.%6.Height=%7\n"
                "App.ActiveDocument.%8.Angle=%9\n")
                .arg(name).arg(name).arg(coneRadius1->value(),0,'f',2)
                .arg(name).arg(coneRadius2->value(),0,'f',2)
                .arg(name).arg(coneHeight->value(),0,'f',2)
                .arg(name).arg(coneAngle->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 4) {  // sphere
            name = doc->getUniqueObjectName("Sphere").c_str();
            cmd = QString(
                "App.ActiveDocument.addObject(\"Part::Sphere\",\"%1\")\n"
                "App.ActiveDocument.%2.Radius=%3\n"
                "App.ActiveDocument.%4.Angle1=%5\n"
                "App.ActiveDocument.%6.Angle2=%7\n"
                "App.ActiveDocument.%8.Angle3=%9\n")
                .arg(name).arg(name).arg(sphereRadius->value(),0,'f',2)
                .arg(name).arg(sphereAngle1->value(),0,'f',2)
                .arg(name).arg(sphereAngle2->value(),0,'f',2)
                .arg(name).arg(sphereAngle3->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 5) {  // ellipsoid
            QMessageBox::warning(this, "Ellipsoid", "Not implemented yet");
        }
        else if (comboBox1->currentIndex() == 6) {  // torus
            name = doc->getUniqueObjectName("Torus").c_str();
            cmd = QString(
                "App.ActiveDocument.addObject(\"Part::Torus\",\"%1\")\n"
                "App.ActiveDocument.%2.Radius1=%3\n"
                "App.ActiveDocument.%4.Radius2=%5\n"
                "App.ActiveDocument.%6.Angle1=%7\n"
                "App.ActiveDocument.%8.Angle2=%9\n"
                "App.ActiveDocument.%10.Angle3=%11\n")
                .arg(name).arg(name).arg(torusRadius1->value(),0,'f',2)
                .arg(name).arg(torusRadius2->value(),0,'f',2)
                .arg(name).arg(torusAngle1->value(),0,'f',2)
                .arg(name).arg(torusAngle2->value(),0,'f',2)
                .arg(name).arg(torusAngle3->value(),0,'f',2);
        }

        // Execute the Python block
        QString prim = tr("Create %1").arg(comboBox1->currentText());
        Gui::Application::Instance->activeDocument()->openCommand(prim.toUtf8());
        Gui::Command::doCommand(Gui::Command::Doc, (const char*)cmd.toAscii());
        Gui::Application::Instance->activeDocument()->commitCommand();
        Gui::Command::doCommand(Gui::Command::Doc, "App.ActiveDocument.recompute()");
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
    }
    catch (const Base::PyException& e) {
        QMessageBox::warning(this, tr("Create %1").arg(comboBox1->currentText()), e.what());
    }
}
