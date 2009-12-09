/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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

#include "TrajectorySimulate.h"

using namespace std;

DEF_STD_CMD_A(CmdRobotConstraintAxle);

CmdRobotConstraintAxle::CmdRobotConstraintAxle()
	:Command("Robot_Create")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Place robot...");
    sToolTipText    = QT_TR_NOOP("Place a robot (experimental!)");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_CreateRobot";
}


void CmdRobotConstraintAxle::activated(int iMsg)
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

bool CmdRobotConstraintAxle::isActive(void)
{
    return hasActiveDocument();
}

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotCreateTrajectory);

CmdRobotCreateTrajectory::CmdRobotCreateTrajectory()
	:Command("Robot_CreateTrajectory")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Creat trajectory");
    sToolTipText    = QT_TR_NOOP("Creat a new empty trajecotry ");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_CreateTrajectory";
}


void CmdRobotCreateTrajectory::activated(int iMsg)
{
    std::string FeatName = getUniqueObjectName("Trajectory");
 
    openCommand("Create trajectory");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::TrajectoryObject\",\"%s\")",FeatName.c_str());
    updateActive();
    commitCommand();
      
}

bool CmdRobotCreateTrajectory::isActive(void)
{
    return hasActiveDocument();
}

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotInsertWaypoint);

CmdRobotInsertWaypoint::CmdRobotInsertWaypoint()
	:Command("Robot_InsertWaypoint")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Insert in trajectory");
    sToolTipText    = QT_TR_NOOP("Insert robot TCP location into trajectory");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_InsertWaypoint";
}


void CmdRobotInsertWaypoint::activated(int iMsg)
{
    unsigned int n1 = getSelection().countObjectsOfType(Robot::RobotObject::getClassTypeId());
    unsigned int n2 = getSelection().countObjectsOfType(Robot::TrajectoryObject::getClassTypeId());
 
    if (n1 != 1 || n2 != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one Robot and one Trajectory object."));
        return;
    }

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    Robot::RobotObject *pcRobotObject;
    if(Sel[0].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[1].pObject);
    std::string RoboName = pcRobotObject->getNameInDocument();

    Robot::TrajectoryObject *pcTrajectoryObject;
    if(Sel[0].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[1].pObject);
    std::string TrakName = pcTrajectoryObject->getNameInDocument();

    openCommand("Insert waypoint");
    doCommand(Doc,"App.activeDocument().%s.Trajectory = App.activeDocument().%s.Trajectory.insertWaypoints(App.activeDocument().%s.Tcp)",TrakName.c_str(),TrakName.c_str(),RoboName.c_str());
    updateActive();
    commitCommand();
      
}

bool CmdRobotInsertWaypoint::isActive(void)
{
    return hasActiveDocument();
}

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotInsertWaypointPreselect);

CmdRobotInsertWaypointPreselect::CmdRobotInsertWaypointPreselect()
	:Command("Robot_InsertWaypointPreselect")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Insert in trajectory");
    sToolTipText    = QT_TR_NOOP("Insert robot TCP location into trajectory");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_InsertWaypointPre";
    iAccel          = Qt::Key_W;

}


void CmdRobotInsertWaypointPreselect::activated(int iMsg)
{
    unsigned int n1 = getSelection().countObjectsOfType(Robot::RobotObject::getClassTypeId());
    unsigned int n2 = getSelection().countObjectsOfType(Robot::TrajectoryObject::getClassTypeId());
 
    if (n1 != 1 || n2 != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one Robot and one Trajectory object."));
        return;
    }

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    const Gui::SelectionChanges & PreSel = getSelection().getPreselection();
    float x = PreSel.x;
    float y = PreSel.y;
    float z = PreSel.z;


    Robot::RobotObject *pcRobotObject;
    if(Sel[0].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[1].pObject);
    std::string RoboName = pcRobotObject->getNameInDocument();

    Robot::TrajectoryObject *pcTrajectoryObject;
    if(Sel[0].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[1].pObject);
    std::string TrakName = pcTrajectoryObject->getNameInDocument();

    if(PreSel.pDocName == 0){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No preselection"),
            QObject::tr("You have to hover above a geometry (Preselection) with the mouse to use this command. See documentation for details."));
        return;
    }

    openCommand("Insert waypoint");
    doCommand(Doc,"App.activeDocument().%s.Trajectory = App.activeDocument().%s.Trajectory.insertWaypoints(Robot.Waypoint(FreeCAD.Placement(FreeCAD.Vector(%f,%f,%f),FreeCAD.Vector(1,0,0),180),type='LIN',name='Pt',vel=1000.0,cont=True,tool=1))",TrakName.c_str(),TrakName.c_str(),x,y,z);
    updateActive();
    commitCommand();
      
}

bool CmdRobotInsertWaypointPreselect::isActive(void)
{
    return hasActiveDocument();
}

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotSimulate);

CmdRobotSimulate::CmdRobotSimulate()
	:Command("Robot_Simulate")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Simulate a trajectory");
    sToolTipText    = QT_TR_NOOP("Run a simulation on a trajectory");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_Simulate";
}


void CmdRobotSimulate::activated(int iMsg)
{
 
    unsigned int n1 = getSelection().countObjectsOfType(Robot::RobotObject::getClassTypeId());
    unsigned int n2 = getSelection().countObjectsOfType(Robot::TrajectoryObject::getClassTypeId());
 
    if (n1 != 1 || n2 != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one Robot and one Trajectory object."));
        return;
    }

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    Robot::RobotObject *pcRobotObject;
    if(Sel[0].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[1].pObject);
    std::string RoboName = pcRobotObject->getNameInDocument();

    Robot::TrajectoryObject *pcTrajectoryObject;
    if(Sel[0].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[1].pObject);
    std::string TrakName = pcTrajectoryObject->getNameInDocument();

    RobotGui::TrajectorySimulate dlg(pcRobotObject,pcTrajectoryObject,Gui::getMainWindow());
    dlg.exec();

/*   openCommand("Place robot");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::RobotObject\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotVrmlFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),RobotPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis2 = -90",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis3 = 90",FeatName.c_str());
    updateActive();
    commitCommand();*/
      
}

bool CmdRobotSimulate::isActive(void)
{
    return hasActiveDocument();
}



// #####################################################################################################

DEF_STD_CMD_A(CmdRobotExport);

CmdRobotExport::CmdRobotExport()
	:Command("Robot_Export")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Export a trajectory");
    sToolTipText    = QT_TR_NOOP("Export a trajectory");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_Export";
}


void CmdRobotExport::activated(int iMsg)
{
 
    unsigned int n1 = getSelection().countObjectsOfType(Robot::RobotObject::getClassTypeId());
    unsigned int n2 = getSelection().countObjectsOfType(Robot::TrajectoryObject::getClassTypeId());
 
    if (n1 != 1 || n2 != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one Robot and one Trajectory object."));
        return;
    }

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();


    Robot::RobotObject *pcRobotObject;
    if(Sel[0].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::RobotObject::getClassTypeId())
        pcRobotObject = dynamic_cast<Robot::RobotObject*>(Sel[1].pObject);
    std::string RoboName = pcRobotObject->getNameInDocument();

    Robot::TrajectoryObject *pcTrajectoryObject;
    if(Sel[0].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[0].pObject);
    else if(Sel[1].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[1].pObject);
    std::string TrakName = pcTrajectoryObject->getNameInDocument();

    //RobotGui::TrajectorySimulate dlg(pcRobotObject,pcTrajectoryObject,Gui::getMainWindow());
    //dlg.exec();

/*   openCommand("Place robot");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::RobotObject\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotVrmlFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),RobotPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis2 = -90",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis3 = 90",FeatName.c_str());
    updateActive();
    commitCommand();*/
      
}

bool CmdRobotExport::isActive(void)
{
    return hasActiveDocument();
}



void CreateRobotCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdRobotConstraintAxle());
    rcCmdMgr.addCommand(new CmdRobotCreateTrajectory());
    rcCmdMgr.addCommand(new CmdRobotInsertWaypoint());
    rcCmdMgr.addCommand(new CmdRobotInsertWaypointPreselect());
    rcCmdMgr.addCommand(new CmdRobotSimulate());
    rcCmdMgr.addCommand(new CmdRobotExport());
 }
