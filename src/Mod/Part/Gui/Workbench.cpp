/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

using namespace PartGui;

TYPESYSTEM_SOURCE(PartGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
}

Workbench::~Workbench()
{
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
  Gui::MenuItem* root = StdWorkbench::setupMenuBar();
  Gui::MenuItem* item = root->findItem( "&Windows" );
  Gui::MenuItem* part = new Gui::MenuItem;
  root->insertItem( item, part );
  part->setCommand( "&Part" );
  *part << "Part_ImportStep" << "Part_ImportIges" << "Part_ImportBrep" << "Separator" << "Part_Box" << "Part_Box2" << "Part_Box3" << "Part_Cut"
        << "Separator" << "Part_Test1" << "Part_Test2";
  return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
  Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
  Gui::ToolBarItem* part = new Gui::ToolBarItem( root );
  part->setCommand( "PartTools" );
  *part << "Part_ImportStep" << "Part_ImportIges" << "Part_ImportBrep" << "Separator" << "Part_Box" << "Part_Box2" << "Part_Box3" << "Part_Cut"
        << "Separator" << "Part_Test1" << "Part_Test2";
  return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
  // Part tools
  Gui::ToolBarItem* root = new Gui::ToolBarItem;

  Gui::ToolBarItem* imp = new Gui::ToolBarItem( root );
  imp->setCommand( "Import" );
  *imp << "Part_NewDoc" << "Part_ImportStep" << "Part_ImportIges" << "Part_ImportBrep" << "Part_ImportCurveNet";

  Gui::ToolBarItem* bol = new Gui::ToolBarItem( root );
  bol->setCommand( "Boolean" );
  *bol << "Part_NewDoc" << "Part_Box" << "Part_Box2" << "Part_Box3" << "Part_Cut";

  Gui::ToolBarItem* test = new Gui::ToolBarItem( root );
  test->setCommand( "Testing" );
  *test << "Part_Test1" << "Part_Test2";
  
  return root;
}

