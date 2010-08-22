/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
#include <Gui/CombiView.h>
#include <Gui/DockWindowManager.h>

using namespace PartDesignGui;

TYPESYSTEM_SOURCE(PartDesignGui::Workbench, Gui::StdWorkbench)

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
        "PartDesign_NewSketch",
        "Part_Box",
        "Part_Cylinder",
        0};

    std::vector<Gui::TaskView::TaskWatcher*> Watcher;

    //Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
    //    "FROM Robot SELECT TrajectoryObject COUNT 1"
    //    "FROM Robot SELECT RobotObject COUNT 1",
    //    RobotAndTrac,
    //    "Trajectory tools",
    //    "Robot_InsertWaypoint"
    //));

    //Watcher.push_back(new TaskWatcherRobot);

    //Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
    //    "FROM Robot SELECT RobotObject COUNT 1",
    //    Robot,
    //    "Robot tools",
    //    "Robot_CreateRobot"
    //));

   Watcher.push_back(new Gui::TaskView::TaskWatcherCommandsEmptyDoc(
         Empty,
        "Create Geometry",
        "Part_Box"
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
    part->setCommand(QT_TR_NOOP("PartDesign"));
    *part << "PartDesign_NewSketch" 
          << "Separator" 
          << "PartDesign_Pad" 
          << "PartDesign_Fillet" ;
       
    part = new Gui::ToolBarItem(root);
    part->setCommand(QT_TR_NOOP("Sketcher geoms"));
    *part << "Sketcher_CreatePoint" 
		  << "Sketcher_CreateArc"
		  << "Sketcher_CreateCircle"
		  << "Sketcher_CreateLine"
		  << "Sketcher_CreatePolyline"
		  << "Sketcher_CreateBox"
		  << "Sketcher_CreateText"
		  << "Sketcher_CreateDraftLine";

    part = new Gui::ToolBarItem(root);
    part->setCommand(QT_TR_NOOP("Sketcher constrains"));
    *part << "Sketcher_ConstrainLock"
		  << "Sketcher_ConstrainCoincident"
		  << "Sketcher_ConstrainVertical"
		  << "Sketcher_ConstrainHorizontal"
		  << "Sketcher_ConstrainDistance"
		  << "Sketcher_ConstrainParallel";

     return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    // Part tools
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}

