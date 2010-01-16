/***************************************************************************
 *   Copyright (c) 2008 J�rgen Riegel (juergen.riegel@web.de)              *
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
#endif

#include <App/Application.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>
#include <Gui/Document.h>

#include <Mod/Robot/App/RobotObject.h>
#include <Mod/Robot/App/TrajectoryObject.h>

#include <Mod/Part/App/PartFeature.h>


#include "TrajectorySimulate.h"

using namespace std;

DEF_STD_CMD_A(CmdRobotInsertKukaIR500);

CmdRobotInsertKukaIR500::CmdRobotInsertKukaIR500()
	:Command("Robot_InsertKukaIR500")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Kuka IR500");
    sToolTipText    = QT_TR_NOOP("Insert a Kuka IR500 into the document.");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_CreateRobot";
}


void CmdRobotInsertKukaIR500::activated(int iMsg)
{
    std::string FeatName = getUniqueObjectName("Robot");
    std::string RobotPath = "Mod/Robot/Lib/Kuka/kr500_1.wrl";
    std::string KinematicPath = "Mod/Robot/Lib/Kuka/kr500_1.csv";

    openCommand("Place robot");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::RobotObject\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotVrmlFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),RobotPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotKinematicFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),KinematicPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis2 = -90",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis3 = 90",FeatName.c_str());
    updateActive();
    commitCommand();
      
}

bool CmdRobotInsertKukaIR500::isActive(void)
{
    return hasActiveDocument();
}

// #####################################################################################################


DEF_STD_CMD_A(CmdRobotInsertKukaIR16);

CmdRobotInsertKukaIR16::CmdRobotInsertKukaIR16()
	:Command("Robot_InsertKukaIR16")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Kuka IR16");
    sToolTipText    = QT_TR_NOOP("Insert a Kuka IR16 into the document.");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_CreateRobot";
}


void CmdRobotInsertKukaIR16::activated(int iMsg)
{
    std::string FeatName = getUniqueObjectName("Robot");
    std::string RobotPath = "Mod/Robot/Lib/Kuka/kr16.wrl";
    std::string KinematicPath = "Mod/Robot/Lib/Kuka/kr_16.csv";

    openCommand("Place robot");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::RobotObject\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotVrmlFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),RobotPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotKinematicFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),KinematicPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis2 = -90",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis3 = 90",FeatName.c_str());
    updateActive();
    commitCommand();
      
}

bool CmdRobotInsertKukaIR16::isActive(void)
{
    return hasActiveDocument();
}

// #####################################################################################################


DEF_STD_CMD_A(CmdRobotInsertKukaIR210);

CmdRobotInsertKukaIR210::CmdRobotInsertKukaIR210()
	:Command("Robot_InsertKukaIR210")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Kuka IR210");
    sToolTipText    = QT_TR_NOOP("Insert a Kuka IR210 into the document.");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_CreateRobot";
}


void CmdRobotInsertKukaIR210::activated(int iMsg)
{
    std::string FeatName = getUniqueObjectName("Robot");
    std::string RobotPath = "Mod/Robot/Lib/Kuka/kr210.WRL";
    std::string KinematicPath = "Mod/Robot/Lib/Kuka/kr_210_2.csv";

    openCommand("Place robot");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::RobotObject\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotVrmlFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),RobotPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotKinematicFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),KinematicPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis2 = -90",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis3 = 90",FeatName.c_str());
    updateActive();
    commitCommand();
      
}

bool CmdRobotInsertKukaIR210::isActive(void)
{
    return hasActiveDocument();
}


// #####################################################################################################

DEF_STD_CMD_A(CmdRobotAddToolShape);

CmdRobotAddToolShape::CmdRobotAddToolShape()
	:Command("Robot_AddToolShape")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Add tool");
    sToolTipText    = QT_TR_NOOP("Add a tool shape to the robot");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    //sPixmap         = "Robot_AddToolShape";
}


void CmdRobotAddToolShape::activated(int iMsg)
{
    std::vector<App::DocumentObject*> robots = getSelection()
        .getObjectsOfType(Robot::RobotObject::getClassTypeId());
    std::vector<App::DocumentObject*> shapes = getSelection()
        .getObjectsOfType(Part::Feature::getClassTypeId());
 
    if (robots.size() != 1 || shapes.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one robot and one shape object."));
        return;
    }

    Robot::RobotObject *pcRobotObject = static_cast<Robot::RobotObject*>(robots.front());
    std::string RoboName = pcRobotObject->getNameInDocument();

    Part::Feature *pcShapeObject = static_cast<Part::Feature*>(shapes.front());
    std::string ShapeName = pcShapeObject->getNameInDocument();

    openCommand("Add tool to robot");
    doCommand(Doc,"App.activeDocument().%s.ToolShape = App.activeDocument().%s",RoboName.c_str(),ShapeName.c_str());
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",ShapeName.c_str());
    updateActive();
    commitCommand();
}

bool CmdRobotAddToolShape::isActive(void)
{
    return false; // not yet implemetned thus not active
    //return hasActiveDocument();
}

void CreateRobotCommandsInsertRobots(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdRobotInsertKukaIR16());
    rcCmdMgr.addCommand(new CmdRobotInsertKukaIR500());
    rcCmdMgr.addCommand(new CmdRobotInsertKukaIR210());
    rcCmdMgr.addCommand(new CmdRobotAddToolShape());
 }
