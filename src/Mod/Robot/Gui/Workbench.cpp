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


using namespace RobotGui;

TYPESYSTEM_SOURCE(RobotGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
}

Workbench::~Workbench()
{
}

void Workbench::activated()
{
    const char* test[] = {"hh","jj"};

    std::vector<Gui::TaskView::TaskWatcher*> Watcher;
    Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
        "",
        test,
        "Trajectory tools",
        ""
    ));
    
    addTaskWatcher(Watcher);
 
}


void Workbench::deactivated()
{
   removeTaskWatcher();

}


Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* part = new Gui::ToolBarItem(root);
    part->setCommand(QT_TR_NOOP("Robot"));
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
    insertRobots->setCommand(QT_TR_NOOP("Insert Robots"));
    *insertRobots << "Robot_InsertKukaIR500" 
                  << "Robot_InsertKukaIR210" 
                  << "Robot_InsertKukaIR16" 
                  << "Separator"
                  << "Robot_AddToolShape"
                  ;

    // boolean
    Gui::MenuItem* exportM = new Gui::MenuItem;
    exportM->setCommand(QT_TR_NOOP("Export trajectory"));
    *exportM << "Robot_ExportKukaCompact" 
             << "Robot_ExportKukaFull"
             ;
 
    robot->setCommand(QT_TR_NOOP("&Robot"));
    *robot << insertRobots 
           << "Robot_CreateTrajectory"
           << "Separator"
           << "Robot_CreateTrajectory"
           << "Robot_InsertWaypoint"
           << "Robot_InsertWaypointPreselect"
           << "Separator"
           << "Robot_Simulate"
           << exportM
           ;
    return root;
}
