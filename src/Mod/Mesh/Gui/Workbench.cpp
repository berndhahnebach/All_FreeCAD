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

using namespace MeshGui;

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
  Gui::MenuItem* mesh = new Gui::MenuItem;
  root->insertItem( item, mesh );
  mesh->setCommand( "&Mesh" );
  *mesh << "Mesh_Import" << "Mesh_VertexCurvature" << "Separator" << "Mesh_ExMakeMesh" << "Mesh_ExMakeTool" << "Mesh_ExMakeUnion"; 
  return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
  Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
  Gui::ToolBarItem* mesh = new Gui::ToolBarItem(root);
  mesh->setCommand( "Mesh Tools" );
  *mesh << "Mesh_Import" << "Mesh_VertexCurvature" << "Separator" << "Mesh_ExMakeMesh" << "Mesh_ExMakeTool" << "Mesh_ExMakeUnion"; 
  return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
  // Mesh tools
  Gui::ToolBarItem* root = new Gui::ToolBarItem;
  Gui::ToolBarItem* mesh = new Gui::ToolBarItem( root );
  mesh->setCommand( "Mesh Tools" );
  *mesh << "Std_New" << "Mesh_Import" << "Separator" << "Mesh_ExMakeMesh" << "Mesh_ExMakeTool" << "Mesh_ExMakeUnion";
  return root;
}

