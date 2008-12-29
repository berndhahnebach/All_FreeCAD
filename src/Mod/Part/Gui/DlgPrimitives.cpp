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
            name = QString::fromAscii(doc->getUniqueObjectName("Plane").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Plane\",\"%1\")\n"
                "App.ActiveDocument.%1.Length=%2\n"
                "App.ActiveDocument.%1.Width=%3\n")
                .arg(name).arg(planeLength->value(),0,'f',2)
                .arg(planeWidth->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 1) {         // box
            name = QString::fromAscii(doc->getUniqueObjectName("Box").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Box\",\"%1\")\n"
                "App.ActiveDocument.%1.Length=%2\n"
                "App.ActiveDocument.%1.Width=%3\n"
                "App.ActiveDocument.%1.Height=%4\n")
                .arg(name).arg(boxLength->value(),0,'f',2)
                .arg(boxWidth->value(),0,'f',2)
                .arg(boxHeight->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 2) {  // cylinder
            name = QString::fromAscii(doc->getUniqueObjectName("Cylinder").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Cylinder\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius=%2\n"
                "App.ActiveDocument.%1.Height=%3\n"
                "App.ActiveDocument.%1.Angle=%4\n")
                .arg(name).arg(cylinderRadius->value(),0,'f',2)
                .arg(cylinderHeight->value(),0,'f',2)
                .arg(cylinderAngle->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 3) {  // cone
            name = QString::fromAscii(doc->getUniqueObjectName("Cone").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Cone\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Height=%4\n"
                "App.ActiveDocument.%1.Angle=%5\n")
                .arg(name).arg(coneRadius1->value(),0,'f',2)
                .arg(coneRadius2->value(),0,'f',2)
                .arg(coneHeight->value(),0,'f',2)
                .arg(coneAngle->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 4) {  // sphere
            name = QString::fromAscii(doc->getUniqueObjectName("Sphere").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Sphere\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius=%2\n"
                "App.ActiveDocument.%1.Angle1=%3\n"
                "App.ActiveDocument.%1.Angle2=%4\n"
                "App.ActiveDocument.%1.Angle3=%5\n")
                .arg(name).arg(sphereRadius->value(),0,'f',2)
                .arg(sphereAngle1->value(),0,'f',2)
                .arg(sphereAngle2->value(),0,'f',2)
                .arg(sphereAngle3->value(),0,'f',2);
        }
        else if (comboBox1->currentIndex() == 5) {  // ellipsoid
            QMessageBox::warning(this, tr("Ellipsoid"), tr("Not implemented yet"));
        }
        else if (comboBox1->currentIndex() == 6) {  // torus
            name = QString::fromAscii(doc->getUniqueObjectName("Torus").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Torus\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Angle1=%4\n"
                "App.ActiveDocument.%1.Angle2=%5\n"
                "App.ActiveDocument.%1.Angle3=%6\n")
                .arg(name).arg(torusRadius1->value(),0,'f',2)
                .arg(torusRadius2->value(),0,'f',2)
                .arg(torusAngle1->value(),0,'f',2)
                .arg(torusAngle2->value(),0,'f',2)
                .arg(torusAngle3->value(),0,'f',2);
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
        QMessageBox::warning(this, tr("Create %1").arg(comboBox1->currentText()), QString::fromLatin1(e.what()));
    }
}
