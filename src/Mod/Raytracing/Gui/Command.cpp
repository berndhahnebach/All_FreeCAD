/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
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
# include <BRepPrimAPI_MakeBox.hxx>
# include <TopoDS_Shape.hxx>
# include <TNaming_Builder.hxx>
# include <AIS_InteractiveContext.hxx>
#endif

#include "../../../Base/Exception.h"
#include "../../../App/Document.h"
#include "../../../Gui/Application.h"
#include "../../../Gui/Document.h"
#include "../../../Gui/Command.h"
#include "../../../Gui/FileDialog.h"
#include "../../../Gui/View.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdRaytracingTest THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingWriteCamera);

CmdRaytracingWriteCamera::CmdRaytracingWriteCamera()
  :CppCommand("Raytracing_WriteCamera")
{
  sAppModule    = "Raytracing";
  sGroup        = "Raytracing";
  sMenuText     = "Write camera position";
  sToolTipText  = "Write the camera positon of the active 3D view in PovRay format to a file";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingWriteCamera::activated(int iMsg)
{
  QString fn = Gui::FileDialog::getSaveFileName( QString::null, "POV include (*.inc);;All Files (*.*)", 
                                                 Gui::ApplicationWindow::Instance );
	if (! fn.isEmpty() )
	{
    Base::Console().Log("File name: %s",fn.latin1());
	}

}

bool CmdRaytracingWriteCamera::isActive(void)
{
  Gui::Document *Doc = getActiveDocument();

  if(!Doc) return false;

  Gui::MDIView* View = Doc->getActiveView();

  if( strcmp(View->getName(),"View3DInventorEx") == 0) return true;

  return false;

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void CreateCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdRaytracingWriteCamera());
}
