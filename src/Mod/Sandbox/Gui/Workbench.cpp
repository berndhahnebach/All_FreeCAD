/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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
#endif

#include "Workbench.h"
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

using namespace SandboxGui;

TYPESYSTEM_SOURCE(SandboxGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
}

Workbench::~Workbench()
{
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* test = new Gui::MenuItem;
    root->insertItem(item, test);
    Gui::MenuItem* threads = new Gui::MenuItem;
    threads->setCommand("Python Threads");
    *threads << "Sandbox_PythonLockThread" << "Sandbox_NolockPython"
             << "Sandbox_PyQtThread" << "Sandbox_PythonThread" << "Sandbox_PythonMainThread";
    test->setCommand("Threads");
    *test << "Sandbox_Thread" << "Sandbox_TestThread" << "Sandbox_WorkerThread" << "Sandbox_SeqThread"
          << "Sandbox_BlockThread" << "Sandbox_NoThread" << threads << "Separator"
          << "Sandbox_Dialog" << "Sandbox_FileDialog";
    Gui::MenuItem* misc = new Gui::MenuItem;
    root->insertItem(item, misc);
    misc->setCommand("Misc");
    *misc << "Sandbox_EventLoop" << "Sandbox_MeshLoad"
          << "Sandbox_MeshLoaderBoost"
          << "Sandbox_MeshLoaderFuture";

    Gui::MenuItem* widg = new Gui::MenuItem;
    root->insertItem(item, widg);
    widg->setCommand("Widgets");
    *widg << "Std_GrabWidget" << "Std_ImageNode";
    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* test = new Gui::ToolBarItem(root);
    test->setCommand( "Sandbox Tools" );
    *test << "Sandbox_Thread" << "Sandbox_WorkerThread" << "Sandbox_SeqThread"
          << "Sandbox_BlockThread" << "Sandbox_NoThread"
          << "Sandbox_Dialog" << "Sandbox_FileDialog"; 
    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    return 0;
}

