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

#include <Base/Interpreter.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>

#include "DlgPrimitives.h"

using namespace PartGui;

/* TRANSLATOR PartGui::DlgPrimitives */

DlgPrimitives::DlgPrimitives(QWidget* parent, Qt::WFlags fl)
  : Gui::LocationDialogComp<Ui_DlgPrimitives>(parent, fl)
{
    Gui::Command::doCommand(Gui::Command::Doc, "from FreeCAD import Base");
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
            QMessageBox::warning(this, tr("Create %1")
                .arg(ui.comboBox1->currentText()), tr("No active document"));
            return;
        }
        Base::Vector3f dir = ui.getDirection();
        if (ui.comboBox1->currentIndex() == 0) {         // plane
            name = QString::fromAscii(doc->getUniqueObjectName("Plane").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Plane\",\"%1\")\n"
                "App.ActiveDocument.%1.Length=%2\n"
                "App.ActiveDocument.%1.Width=%3\n"
                "App.ActiveDocument.%1.Location=Base.Vector(%4,%5,%6)\n"
                "App.ActiveDocument.%1.Axis=Base.Vector(%7,%8,%9)\n")
                .arg(name)
                .arg(ui.planeLength->value(),0,'f',2)
                .arg(ui.planeWidth->value(),0,'f',2)
                .arg(ui.xPos->value(),0,'f',2)
                .arg(ui.yPos->value(),0,'f',2)
                .arg(ui.zPos->value(),0,'f',2)
                .arg(dir.x,0,'f',2)
                .arg(dir.y,0,'f',2)
                .arg(dir.z,0,'f',2);
        }
        else if (ui.comboBox1->currentIndex() == 1) {         // box
            name = QString::fromAscii(doc->getUniqueObjectName("Box").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Box\",\"%1\")\n"
                "App.ActiveDocument.%1.Length=%2\n"
                "App.ActiveDocument.%1.Width=%3\n"
                "App.ActiveDocument.%1.Height=%4\n"
                "App.ActiveDocument.%1.Location=Base.Vector(%5,%6,%7)\n"
                "App.ActiveDocument.%1.Axis=Base.Vector(%8,%9,%10)\n")
                .arg(name)
                .arg(ui.boxLength->value(),0,'f',2)
                .arg(ui.boxWidth->value(),0,'f',2)
                .arg(ui.boxHeight->value(),0,'f',2)
                .arg(ui.xPos->value(),0,'f',2)
                .arg(ui.yPos->value(),0,'f',2)
                .arg(ui.zPos->value(),0,'f',2)
                .arg(dir.x,0,'f',2)
                .arg(dir.y,0,'f',2)
                .arg(dir.z,0,'f',2);
        }
        else if (ui.comboBox1->currentIndex() == 2) {  // cylinder
            name = QString::fromAscii(doc->getUniqueObjectName("Cylinder").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Cylinder\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius=%2\n"
                "App.ActiveDocument.%1.Height=%3\n"
                "App.ActiveDocument.%1.Angle=%4\n"
                "App.ActiveDocument.%1.Location=Base.Vector(%5,%6,%7)\n"
                "App.ActiveDocument.%1.Axis=Base.Vector(%8,%9,%10)\n")
                .arg(name)
                .arg(ui.cylinderRadius->value(),0,'f',2)
                .arg(ui.cylinderHeight->value(),0,'f',2)
                .arg(ui.cylinderAngle->value(),0,'f',2)
                .arg(ui.xPos->value(),0,'f',2)
                .arg(ui.yPos->value(),0,'f',2)
                .arg(ui.zPos->value(),0,'f',2)
                .arg(dir.x,0,'f',2)
                .arg(dir.y,0,'f',2)
                .arg(dir.z,0,'f',2);
        }
        else if (ui.comboBox1->currentIndex() == 3) {  // cone
            name = QString::fromAscii(doc->getUniqueObjectName("Cone").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Cone\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Height=%4\n"
                "App.ActiveDocument.%1.Angle=%5\n"
                "App.ActiveDocument.%1.Location=Base.Vector(%6,%7,%8)\n"
                "App.ActiveDocument.%1.Axis=Base.Vector(%9,%10,%11)\n")
                .arg(name)
                .arg(ui.coneRadius1->value(),0,'f',2)
                .arg(ui.coneRadius2->value(),0,'f',2)
                .arg(ui.coneHeight->value(),0,'f',2)
                .arg(ui.coneAngle->value(),0,'f',2)
                .arg(ui.xPos->value(),0,'f',2)
                .arg(ui.yPos->value(),0,'f',2)
                .arg(ui.zPos->value(),0,'f',2)
                .arg(dir.x,0,'f',2)
                .arg(dir.y,0,'f',2)
                .arg(dir.z,0,'f',2);
        }
        else if (ui.comboBox1->currentIndex() == 4) {  // sphere
            name = QString::fromAscii(doc->getUniqueObjectName("Sphere").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Sphere\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius=%2\n"
                "App.ActiveDocument.%1.Angle1=%3\n"
                "App.ActiveDocument.%1.Angle2=%4\n"
                "App.ActiveDocument.%1.Angle3=%5\n"
                "App.ActiveDocument.%1.Location=Base.Vector(%6,%7,%8)\n"
                "App.ActiveDocument.%1.Axis=Base.Vector(%9,%10,%11)\n")
                .arg(name)
                .arg(ui.sphereRadius->value(),0,'f',2)
                .arg(ui.sphereAngle1->value(),0,'f',2)
                .arg(ui.sphereAngle2->value(),0,'f',2)
                .arg(ui.sphereAngle3->value(),0,'f',2)
                .arg(ui.xPos->value(),0,'f',2)
                .arg(ui.yPos->value(),0,'f',2)
                .arg(ui.zPos->value(),0,'f',2)
                .arg(dir.x,0,'f',2)
                .arg(dir.y,0,'f',2)
                .arg(dir.z,0,'f',2);
        }
        else if (ui.comboBox1->currentIndex() == 5) {  // ellipsoid
            name = QString::fromAscii(doc->getUniqueObjectName("Ellipsoid").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Ellipsoid\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Angle1=%4\n"
                "App.ActiveDocument.%1.Angle2=%5\n"
                "App.ActiveDocument.%1.Angle3=%6\n"
                "App.ActiveDocument.%1.Location=Base.Vector(%7,%8,%9)\n"
                "App.ActiveDocument.%1.Axis=Base.Vector(%10,%11,%12)\n")
                .arg(name)
                .arg(ui.ellipsoidRadius1->value(),0,'f',2)
                .arg(ui.ellipsoidRadius2->value(),0,'f',2)
                .arg(ui.ellipsoidAngle1->value(),0,'f',2)
                .arg(ui.ellipsoidAngle2->value(),0,'f',2)
                .arg(ui.ellipsoidAngle3->value(),0,'f',2)
                .arg(ui.xPos->value(),0,'f',2)
                .arg(ui.yPos->value(),0,'f',2)
                .arg(ui.zPos->value(),0,'f',2)
                .arg(dir.x,0,'f',2)
                .arg(dir.y,0,'f',2)
                .arg(dir.z,0,'f',2);
        }
        else if (ui.comboBox1->currentIndex() == 6) {  // torus
            name = QString::fromAscii(doc->getUniqueObjectName("Torus").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Torus\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Angle1=%4\n"
                "App.ActiveDocument.%1.Angle2=%5\n"
                "App.ActiveDocument.%1.Angle3=%6\n"
                "App.ActiveDocument.%1.Location=Base.Vector(%7,%8,%9)\n"
                "App.ActiveDocument.%1.Axis=Base.Vector(%10,%11,%12)\n")
                .arg(name)
                .arg(ui.torusRadius1->value(),0,'f',2)
                .arg(ui.torusRadius2->value(),0,'f',2)
                .arg(ui.torusAngle1->value(),0,'f',2)
                .arg(ui.torusAngle2->value(),0,'f',2)
                .arg(ui.torusAngle3->value(),0,'f',2)
                .arg(ui.xPos->value(),0,'f',2)
                .arg(ui.yPos->value(),0,'f',2)
                .arg(ui.zPos->value(),0,'f',2)
                .arg(dir.x,0,'f',2)
                .arg(dir.y,0,'f',2)
                .arg(dir.z,0,'f',2);
        }

        // Execute the Python block
        QString prim = tr("Create %1").arg(ui.comboBox1->currentText());
        Gui::Application::Instance->activeDocument()->openCommand(prim.toUtf8());
        Gui::Command::doCommand(Gui::Command::Doc, (const char*)cmd.toAscii());
        Gui::Application::Instance->activeDocument()->commitCommand();
        Gui::Command::doCommand(Gui::Command::Doc, "App.ActiveDocument.recompute()");
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
    }
    catch (const Base::PyException& e) {
        QMessageBox::warning(this, tr("Create %1")
            .arg(ui.comboBox1->currentText()), QString::fromLatin1(e.what()));
    }
}

#include "moc_DlgPrimitives.cpp"
