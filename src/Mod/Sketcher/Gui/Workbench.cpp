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
# include <qobject.h>
#endif

#include "Workbench.h"
#include <Gui/ToolBarManager.h>

using namespace SketcherGui;

TYPESYSTEM_SOURCE(SketcherGui::Workbench, Gui::StdWorkbench)

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
    part->setCommand(QT_TR_NOOP("Sketcher"));
    *part << "Sketcher_NewSketch"<< "Sketcher_LeavSketch";

    part = new Gui::ToolBarItem(root);
    part->setCommand(QT_TR_NOOP("Sketcher geoms"));
    *part << "Sketcher_CreatePoint" 
		  << "Sketcher_CreateArc"
		  << "Sketcher_CreateCircle"
		  << "Sketcher_CreateLine"
		  << "Sketcher_CreatePolyline"
		  << "Sketcher_CreateRectangle"
		  << "Sketcher_CreateText"
		  << "Sketcher_CreateDraftLine";

    part = new Gui::ToolBarItem(root);
    part->setCommand(QT_TR_NOOP("Sketcher constrains"));
    *part << "Sketcher_ConstrainLock"
		  << "Sketcher_ConstrainVertical"
		  << "Sketcher_ConstrainHorizontal";
     return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    // Part tools
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}

