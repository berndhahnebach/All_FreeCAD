/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qaction.h>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/FileDialog.h>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdMeshImportSTL THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD_A(CmdMeshImportSTL);

CmdMeshImportSTL::CmdMeshImportSTL()
  :CppCommand("Mesh_Test")
{
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = "Import Mesh";
  sToolTipText  = "Create or change an Import Mesh feature";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Open";
  iAccel        = Qt::CTRL+Qt::Key_T;
}

void CmdMeshImportSTL::activated(int iMsg)
{
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "STL (*.stl *.ast);;All Files (*.*)", 
                                                 Gui::ApplicationWindow::Instance );
	if (! fn.isEmpty() )
  {
    openCommand("Mesh ImportSTL Create");
	  doCommand(Doc,"f = App.DocGet().AddFeature(\"MeshImportSTL\")");
	  doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();
 
    updateActive();
  }
}

bool CmdMeshImportSTL::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;
}

void CreateCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdMeshImportSTL());
}
