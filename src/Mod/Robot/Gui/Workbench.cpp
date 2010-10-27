/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qobject.h>
#endif

#include "Workbench.h"
#include <Gui/ToolBarManager.h>
#include <Gui/MenuManager.h>
#include <Gui/MainWindow.h>
#include <Gui/CombiView.h>
#include <Gui/DockWindowManager.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/TaskView/TaskWatcher.h>

#include "TaskWatcher.h"


using namespace RobotGui;

#if 0 // needed for Qt's lupdate utility
    qApp->translate("Workbench", "Robot");
    qApp->translate("Workbench", "Insert Robots");
    qApp->translate("Workbench", "&Robot");
    qApp->translate("Workbench", "Export trajectory");
#endif

/// @namespace RobotGui @class Workbench
TYPESYSTEM_SOURCE(RobotGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
}

Workbench::~Workbench()
{
}

void Workbench::activated()
{
    Gui::Workbench::activated();

    const char* RobotAndTrac[] = {
        "Robot_InsertWaypoint",
        "Robot_InsertWaypointPreselect",
        0};

    const char* Robot[] = {
        "Robot_AddToolShape",
        "Robot_SetHomePos",
        "Robot_RestoreHomePos",
        0};

    const char* Empty[] = {
        "Robot_InsertKukaIR500",
        "Robot_InsertKukaIR16",
        "Robot_InsertKukaIR210",
        "Robot_InsertKukaIR125",
        0};

    std::vector<Gui::TaskView::TaskWatcher*> Watcher;

    Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
        "FROM Robot SELECT TrajectoryObject COUNT 1"
        "FROM Robot SELECT RobotObject COUNT 1",
        RobotAndTrac,
        "Trajectory tools",
        "Robot_InsertWaypoint"
    ));

    Watcher.push_back(new TaskWatcherRobot);

    Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
        "FROM Robot SELECT RobotObject COUNT 1",
        Robot,
        "Robot tools",
        "Robot_CreateRobot"
    ));

   Watcher.push_back(new Gui::TaskView::TaskWatcherCommandsEmptyDoc(
         Empty,
        "Insert Robot",
        "Robot_CreateRobot"
    ));

    
    addTaskWatcher(Watcher);

    Gui::DockWnd::CombiView* pcCombiView = qobject_cast<Gui::DockWnd::CombiView*>
        (Gui::DockWindowManager::instance()->getDockWindow("Combo View"));
    // should return the pointer to combo view
    assert(pcCombiView);
    pcCombiView->showTaskView();
 
}


void Workbench::deactivated()
{
    Gui::Workbench::deactivated();
    removeTaskWatcher();

}


Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* part = new Gui::ToolBarItem(root);
    part->setCommand("Robot");
    *part << "Robot_Create";
    *part << "Separator";
    *part << "Robot_CreateTrajectory";
    *part << "Robot_InsertWaypoint";
    *part << "Robot_InsertWaypointPreselect";
    *part << "Separator";
    *part << "Robot_Simulate";
    return root;
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* robot = new Gui::MenuItem;
    root->insertItem( item, robot );

    // analyze
    Gui::MenuItem* insertRobots = new Gui::MenuItem;
    insertRobots->setCommand("Insert Robots");
    *insertRobots << "Robot_InsertKukaIR500" 
                  << "Robot_InsertKukaIR210" 
                  << "Robot_InsertKukaIR125" 
                  << "Robot_InsertKukaIR16" 
                  << "Separator"
                  << "Robot_AddToolShape"
                  ;

    // boolean
    Gui::MenuItem* exportM = new Gui::MenuItem;
    exportM->setCommand("Export trajectory");
    *exportM << "Robot_ExportKukaCompact" 
             << "Robot_ExportKukaFull"
             ;
 
    robot->setCommand("&Robot");
    *robot << insertRobots 
           << "Robot_CreateTrajectory"
           << "Separator"
           << "Robot_CreateTrajectory"
           << "Robot_InsertWaypoint"
           << "Robot_InsertWaypointPreselect"
           << "Robot_SetDefaultOrientation"
           << "Robot_SetDefaultValues"
           << "Separator"
           << "Robot_Simulate"
           << exportM
           ;
    return root;
}
