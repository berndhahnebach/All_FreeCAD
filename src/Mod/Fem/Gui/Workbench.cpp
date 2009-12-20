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


using namespace FemGui;

TYPESYSTEM_SOURCE(FemGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
}

Workbench::~Workbench()
{
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* part = new Gui::ToolBarItem(root);
    part->setCommand(QT_TR_NOOP("Fem"));
    //*part << "Fem_Create";
    //*part << "Separator";
    //*part << "Fem_CreateTrajectory";
    //*part << "Fem_InsertWaypoint";
    //*part << "Fem_InsertWaypointPreselect";
    //*part << "Separator";
    //*part << "Fem_Simulate";
    return root;
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* robot = new Gui::MenuItem;
    root->insertItem( item, robot );

    //// analyze
    //Gui::MenuItem* insertFems = new Gui::MenuItem;
    //insertFems->setCommand(QT_TR_NOOP("Insert Fems"));
    //*insertFems << "Fem_InsertKukaIR500" 
    //              << "Fem_InsertKukaIR16" 
    //              << "Separator"
    //              << "Fem_AddToolShape"
    //              ;

    //// boolean
    //Gui::MenuItem* exportM = new Gui::MenuItem;
    //exportM->setCommand(QT_TR_NOOP("Export trajectory"));
    //*exportM << "Fem_ExportKukaCompact" 
    //         << "Fem_ExportKukaFull"
    //         ;
 
    //robot->setCommand(QT_TR_NOOP("&Fem"));
    //*robot << insertFems 
    //       << "Fem_CreateTrajectory"
    //       << "Separator"
    //       << "Fem_CreateTrajectory"
    //       << "Fem_InsertWaypoint"
    //       << "Fem_InsertWaypointPreselect"
    //       << "Separator"
    //       << "Fem_Simulate"
    //       << exportM
    //       ;
    return root;
}
