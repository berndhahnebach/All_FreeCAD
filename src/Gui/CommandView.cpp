/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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


#include "Command.h"
#include "Application.h"
#include "View.h"
#include "Document.h"

#include "../Base/Exception.h"
#include "../App/Document.h"

using namespace Gui;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_ViewBottom
//===========================================================================
DEF_3DV_CMD(StdCmdViewBottom)

StdCmdViewBottom::StdCmdViewBottom()
  :CppCommand("Std_ViewBottom")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Bottom";
  sToolTipText  = "Set to bottom view";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "view_bottom";
  iAccel        = 0;
}

void StdCmdViewBottom::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewBottom\")");
}

//===========================================================================
// Std_ViewFront
//===========================================================================
DEF_3DV_CMD(StdCmdViewFront);

StdCmdViewFront::StdCmdViewFront()
  :CppCommand("Std_ViewFront")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Front";
  sToolTipText  = "Set to front view";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "view_front";
  iAccel        = 0;
}

void StdCmdViewFront::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFront\")");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================
DEF_3DV_CMD(StdCmdViewLeft);

StdCmdViewLeft::StdCmdViewLeft()
  :CppCommand("Std_ViewLeft")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Left";
  sToolTipText  = "Set to left view";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "view_left";
  iAccel        = 0;
}

void StdCmdViewLeft::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewLeft\")");
}

//===========================================================================
// Std_ViewRear
//===========================================================================
DEF_3DV_CMD(StdCmdViewRear);

StdCmdViewRear::StdCmdViewRear()
  :CppCommand("Std_ViewRear")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Rear";
  sToolTipText  = "Set to rear view";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "view_back";
  iAccel        = 0;
}

void StdCmdViewRear::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRear\")");
}

//===========================================================================
// Std_ViewRight
//===========================================================================
DEF_3DV_CMD(StdCmdViewRight);

StdCmdViewRight::StdCmdViewRight()
  :CppCommand("Std_ViewRight")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Right";
  sToolTipText  = "Set to right view";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "view_right";
  iAccel        = 0;
}

void StdCmdViewRight::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRight\")");
}

//===========================================================================
// Std_ViewTop
//===========================================================================
DEF_3DV_CMD(StdCmdViewTop);

StdCmdViewTop::StdCmdViewTop()
  :CppCommand("Std_ViewTop")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Top";
  sToolTipText  = "Set to top view";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "view_top";
  iAccel        = 0;
}

void StdCmdViewTop::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewTop\")");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================
DEF_3DV_CMD(StdCmdViewAxo);

StdCmdViewAxo::StdCmdViewAxo()
  :CppCommand("Std_ViewAxo")
{
  sAppModule  = "";
  sGroup      = "Standard-View";
  sMenuText   = "Axometric";
  sToolTipText= "Set to axometric view";
  sWhatsThis  = sToolTipText;
  sStatusTip  = sToolTipText;
  sPixmap     = "view_axo";
  iAccel      = 0;
}

void StdCmdViewAxo::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewAxo\")");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================
DEF_STD_CMD_A(StdCmdViewFitAll);

StdCmdViewFitAll::StdCmdViewFitAll()
  :CppCommand("Std_ViewFitAll")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Fit all";
  sToolTipText  = "Fits the whole content on the screen";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "view_fitall";
  iAccel        = 0;
}

void StdCmdViewFitAll::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFit\")");
}

bool StdCmdViewFitAll::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("ViewFit");
}


//===========================================================================
// Std_ViewCreateOCC
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreateOCC);

StdCmdViewCreateOCC::StdCmdViewCreateOCC()
  :CppCommand("Std_ViewCreateOCC")
{
  sAppModule  = "";
  sGroup      = "Standard-View";
  sMenuText   = "Create new OCC View";
  sToolTipText= "Creates a new OpenCasCade view window for the active document";
  sWhatsThis  = sToolTipText;
  sStatusTip  = sToolTipText;
  sPixmap     = "view_fitall";
  iAccel      = 0;
}

void StdCmdViewCreateOCC::activated(int iMsg)
{
  getActiveDocument()->createView("View3DOcc");
}

bool StdCmdViewCreateOCC::isActive(void)
{
  return (getActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewCreateInventor
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreateInventor);

StdCmdViewCreateInventor::StdCmdViewCreateInventor()
  :CppCommand("Std_ViewCreateInventor")
{
  sAppModule  = "";
  sGroup      = "Standard-View";
  sMenuText   = "Create new Inventor View";
  sToolTipText= "Creates a new Inventor view window for the active document";
  sWhatsThis  = sToolTipText;
  sStatusTip  = sToolTipText;
  sPixmap     = "view_fitall";
  iAccel      = 0;
}

void StdCmdViewCreateInventor::activated(int iMsg)
{
  getActiveDocument()->createView("View3DIv");
}

bool StdCmdViewCreateInventor::isActive(void)
{
  return (getActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewExample1
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample1);

StdCmdViewExample1::StdCmdViewExample1()
  :CppCommand("Std_ViewExample1")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Inventor example #1";
  sToolTipText  = "Shows a 3D texture with manipulator";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void StdCmdViewExample1::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example1\")");
}

bool StdCmdViewExample1::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Example1");
}
//===========================================================================
// Std_ViewExample2
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample2);

StdCmdViewExample2::StdCmdViewExample2()
  :CppCommand("Std_ViewExample2")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Inventor example #2";
  sToolTipText  = "Shows spheres an dragglights";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool2";
  iAccel        = 0;
}

void StdCmdViewExample2::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example2\")");
}

bool StdCmdViewExample2::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Example2");
}

//===========================================================================
// Std_ViewExample3
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample3);

StdCmdViewExample3::StdCmdViewExample3()
  :CppCommand("Std_ViewExample3")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Inventor example #3";
  sToolTipText  = "Shows a animated texture";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool3";
  iAccel        = 0;
}

void StdCmdViewExample3::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example3\")");
}

bool StdCmdViewExample3::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Example3");
}

//===========================================================================
// Std_ViewIvDecorationOn
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvDecorationOn);

StdCmdViewIvDecorationOn::StdCmdViewIvDecorationOn()
  :CppCommand("Std_ViewIvDecorationOn")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Decoration on";
  sToolTipText  = "Examiner viewer decoration on";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool4";
  iAccel        = 0;
}

void StdCmdViewIvDecorationOn::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetDecorationOn\")");
}

bool StdCmdViewIvDecorationOn::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetDecorationOn");
}

//===========================================================================
// Std_ViewIvDecorationOff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvDecorationOff);

StdCmdViewIvDecorationOff::StdCmdViewIvDecorationOff()
  :CppCommand("Std_ViewIvDecorationOff")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Decoration Off";
  sToolTipText  = "Examiner viewer decoration Off";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool5";
  iAccel        = 0;
}

void StdCmdViewIvDecorationOff::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetDecorationOff\")");
}

bool StdCmdViewIvDecorationOff::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetDecorationOff");
}


//===========================================================================
// Std_ViewIvStereoOff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoOff);

StdCmdViewIvStereoOff::StdCmdViewIvStereoOff()
  :CppCommand("Std_ViewIvStereoOff")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Stereo Off";
  sToolTipText  = "switch stereo viewing off";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool6";
  iAccel        = 0;
}

void StdCmdViewIvStereoOff::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoOff\")");
}

bool StdCmdViewIvStereoOff::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetStereoOff");
}


//===========================================================================
// Std_ViewIvStereoOn
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoOn);

StdCmdViewIvStereoOn::StdCmdViewIvStereoOn()
  :CppCommand("Std_ViewIvStereoOn")
{
  sAppModule    = "";
  sGroup        = "Standard-View";
  sMenuText     = "Stereo On";
  sToolTipText  = "switch stereo viewing on";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool7";
  iAccel        = 0;
}

void StdCmdViewIvStereoOn::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoOn\")");
}

bool StdCmdViewIvStereoOn::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetStereoOn");
}





//===========================================================================
// Instanciation
//===========================================================================


namespace Gui {

void CreateViewStdCommands(void)
{
  CommandManager &rcCmdMgr = ApplicationWindow::Instance->commandManager();

  // views
  rcCmdMgr.addCommand(new StdCmdViewBottom());
  rcCmdMgr.addCommand(new StdCmdViewFront());
  rcCmdMgr.addCommand(new StdCmdViewLeft());
  rcCmdMgr.addCommand(new StdCmdViewRear());
  rcCmdMgr.addCommand(new StdCmdViewRight());
  rcCmdMgr.addCommand(new StdCmdViewTop());
  rcCmdMgr.addCommand(new StdCmdViewAxo());
  rcCmdMgr.addCommand(new StdCmdViewFitAll());

  rcCmdMgr.addCommand(new StdCmdViewExample1());
  rcCmdMgr.addCommand(new StdCmdViewExample2());
  rcCmdMgr.addCommand(new StdCmdViewExample3());

  rcCmdMgr.addCommand(new StdCmdViewIvDecorationOff());
  rcCmdMgr.addCommand(new StdCmdViewIvDecorationOn());

  rcCmdMgr.addCommand(new StdCmdViewIvStereoOn());
  rcCmdMgr.addCommand(new StdCmdViewIvStereoOff());

  rcCmdMgr.addCommand(new StdCmdViewCreateOCC());
  rcCmdMgr.addCommand(new StdCmdViewCreateInventor());
}

} // namespace Gui


