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
#include <Gui/Placement.h>
#include <Gui/Control.h>


#include <Mod/Robot/App/RobotObject.h>
#include <Mod/Robot/App/TrajectoryObject.h>
#include <Mod/Robot/App/Edge2TracObject.h>
#include "TaskDlgEdge2Trac.h"

#include "TrajectorySimulate.h"

using namespace std;
using namespace RobotGui;

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotCreateTrajectory);

CmdRobotCreateTrajectory::CmdRobotCreateTrajectory()
	:Command("Robot_CreateTrajectory")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Create trajectory");
    sToolTipText    = QT_TR_NOOP("Create a new empty trajectory ");
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
    sToolTipText    = QT_TR_NOOP("Insert robot Tool location into trajectory");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_InsertWaypoint";
    iAccel          = Qt::Key_A;
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
    doCommand(Doc,"App.activeDocument().%s.Trajectory = App.activeDocument().%s.Trajectory.insertWaypoints(Robot.Waypoint(App.activeDocument().%s.Tcp.multiply(App.activeDocument().%s.Tool),type='LIN',name='Pt',vel=_DefSpeed,cont=_DefCont,acc=_DefAccelaration,tool=1))",TrakName.c_str(),TrakName.c_str(),RoboName.c_str(),RoboName.c_str());
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
    sToolTipText    = QT_TR_NOOP("Insert preselection position into trajectory (W)");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_InsertWaypointPre";
    iAccel          = Qt::Key_W;

}


void CmdRobotInsertWaypointPreselect::activated(int iMsg)
{
    
    if (getSelection().size() != 1 ) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one Trajectory object."));
        return;
    }

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    const Gui::SelectionChanges & PreSel = getSelection().getPreselection();
    float x = PreSel.x;
    float y = PreSel.y;
    float z = PreSel.z;


    Robot::TrajectoryObject *pcTrajectoryObject;
    if(Sel[0].pObject->getTypeId() == Robot::TrajectoryObject::getClassTypeId())
        pcTrajectoryObject = dynamic_cast<Robot::TrajectoryObject*>(Sel[0].pObject);
    else  {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one Trajectory object."));
        return;
    }
    std::string TrakName = pcTrajectoryObject->getNameInDocument();

    if(PreSel.pDocName == 0){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No preselection"),
            QObject::tr("You have to hover above a geometry (Preselection) with the mouse to use this command. See documentation for details."));
        return;
    }

    openCommand("Insert waypoint");
    doCommand(Doc,"App.activeDocument().%s.Trajectory = App.activeDocument().%s.Trajectory.insertWaypoints(Robot.Waypoint(FreeCAD.Placement(FreeCAD.Vector(%f,%f,%f)+_DefDisplacement,_DefOrientation),type='LIN',name='Pt',vel=_DefSpeed,cont=_DefCont,acc=_DefAccelaration,tool=1))",TrakName.c_str(),TrakName.c_str(),x,y,z);
    updateActive();
    commitCommand();
      
}

bool CmdRobotInsertWaypointPreselect::isActive(void)
{
    return hasActiveDocument();
}

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotSetDefaultOrientation);

CmdRobotSetDefaultOrientation::CmdRobotSetDefaultOrientation()
	:Command("Robot_SetDefaultOrientation")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Set default orientation");
    sToolTipText    = QT_TR_NOOP("set the default orientation for subsequent commands for waypoint creation");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = 0;
    iAccel          = 0;

}


void CmdRobotSetDefaultOrientation::activated(int iMsg)
{
    // create placement dialog 
    Gui::Dialog::Placement *Dlg = new Gui::Dialog::Placement();
    Base::Placement place;
    Dlg->setPlacement(place);
    if(Dlg->exec() == QDialog::Accepted ){
        place = Dlg->getPlacement();
        Base::Rotation rot = place.getRotation();
        Base::Vector3d disp = place.getPosition();
        doCommand(Doc,"_DefOrientation = FreeCAD.Rotation(%f,%f,%f,%f)",rot[0],rot[1],rot[2],rot[3]);
        doCommand(Doc,"_DefDisplacement = FreeCAD.Vector(%f,%f,%f)",disp[0],disp[1],disp[2]);
    }
      
}

bool CmdRobotSetDefaultOrientation::isActive(void)
{
    return true;
}

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotSetDefaultValues);

CmdRobotSetDefaultValues::CmdRobotSetDefaultValues()
	:Command("Robot_SetDefaultValues")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Set default values");
    sToolTipText    = QT_TR_NOOP("set the default values for speed, acceleration and continuity for subsequent commands of waypoint creation");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = 0;
    iAccel          = 0;

}


void CmdRobotSetDefaultValues::activated(int iMsg)
{

    bool ok;
    QString text = QInputDialog::getText(0, QObject::tr("set default speed"),
                                          QObject::tr("speed: (e.g. 1 m/s or 3 cm/s)"), QLineEdit::Normal,
                                          QString::fromAscii("1 m/s"), &ok);
    if ( ok && !text.isEmpty() ) {
        doCommand(Doc,"_DefSpeed = '%s'",text.toAscii().constData());
    } 

    QStringList items;
    items  << QString::fromAscii("False") << QString::fromAscii("True");

    QString item = QInputDialog::getItem(0, QObject::tr("set default continuity"),
                                          QObject::tr("continuous ?"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
        doCommand(Doc,"_DefCont = %s",item.toAscii().constData());

    text.clear();

    text = QInputDialog::getText(0, QObject::tr("set default acceleration"),
                                          QObject::tr("acceleration: (e.g. 1 m/s^2 or 3 cm/s^2)"), QLineEdit::Normal,
                                          QString::fromAscii("1 m/s^2"), &ok);
    if ( ok && !text.isEmpty() ) {
        doCommand(Doc,"_DefAccelaration = '%s'",text.toAscii().constData());
    } 


    // create placement dialog 
    //Gui::Dialog::Placement *Dlg = new Gui::Dialog::Placement();
    //Base::Placement place;
    //Dlg->setPlacement(place);
    //if(Dlg->exec() == QDialog::Accepted ){
    //    place = Dlg->getPlacement();
    //    Base::Rotation rot = place.getRotation();
    //    Base::Vector3d disp = place.getPosition();
    //    doCommand(Doc,"_DefOrientation = FreeCAD.Rotation(%f,%f,%f,%f)",rot[0],rot[1],rot[2],rot[3]);
    //    doCommand(Doc,"_DefDisplacement = FreeCAD.Vector(%f,%f,%f)",disp[0],disp[1],disp[2]);
    //}
      
}

bool CmdRobotSetDefaultValues::isActive(void)
{
    return true;
}

// #####################################################################################################

DEF_STD_CMD_A(CmdRobotEdge2Trac);

CmdRobotEdge2Trac::CmdRobotEdge2Trac()
	:Command("Robot_Edge2Trac")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Edge to Trajectory...");
    sToolTipText    = QT_TR_NOOP("Generate a Trajectory from a set of edges");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Robot_Edge2Trac";
    iAccel          = 0;

}


void CmdRobotEdge2Trac::activated(int iMsg)
{
    std::string FeatName = getUniqueObjectName("EdgeTrajectory");
 
    openCommand("Create trajectory");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::Edge2TracObject\",\"%s\")",FeatName.c_str());
    commitCommand();
    
    App::DocumentObject *obj = this->getDocument()->getObject(FeatName.c_str());
    App::Property *prop = &(dynamic_cast<Robot::Edge2TracObject *>(obj)->Source); 

    Gui::TaskView::TaskDialog* dlg = new TaskDlgEdge2Trac(dynamic_cast<Robot::Edge2TracObject *>(obj));
    Gui::Control().showDialog(dlg);


}

bool CmdRobotEdge2Trac::isActive(void)
{
    return true;
}


// #####################################################################################################



void CreateRobotCommandsTrajectory(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdRobotCreateTrajectory());
    rcCmdMgr.addCommand(new CmdRobotInsertWaypoint());
    rcCmdMgr.addCommand(new CmdRobotInsertWaypointPreselect());
    rcCmdMgr.addCommand(new CmdRobotSetDefaultOrientation());
    rcCmdMgr.addCommand(new CmdRobotSetDefaultValues());
    rcCmdMgr.addCommand(new CmdRobotEdge2Trac());
 }
