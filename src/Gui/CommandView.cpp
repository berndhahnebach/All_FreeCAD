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




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_ViewBottom
//===========================================================================
DEF_3DV_CMD(StdCmdViewBottom)

StdCmdViewBottom::StdCmdViewBottom()
  :FCCppCommand("Std_ViewBottom")
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

void StdCmdViewBottom::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewBottom\")");
}

//===========================================================================
// Std_ViewFront
//===========================================================================
DEF_3DV_CMD(StdCmdViewFront);

StdCmdViewFront::StdCmdViewFront()
  :FCCppCommand("Std_ViewFront")
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

void StdCmdViewFront::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFront\")");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================
DEF_3DV_CMD(StdCmdViewLeft);

StdCmdViewLeft::StdCmdViewLeft()
  :FCCppCommand("Std_ViewLeft")
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

void StdCmdViewLeft::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewLeft\")");
}

//===========================================================================
// Std_ViewRear
//===========================================================================
DEF_3DV_CMD(StdCmdViewRear);

StdCmdViewRear::StdCmdViewRear()
  :FCCppCommand("Std_ViewRear")
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

void StdCmdViewRear::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRear\")");
}

//===========================================================================
// Std_ViewRight
//===========================================================================
DEF_3DV_CMD(StdCmdViewRight);

StdCmdViewRight::StdCmdViewRight()
  :FCCppCommand("Std_ViewRight")
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

void StdCmdViewRight::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRight\")");
}

//===========================================================================
// Std_ViewTop
//===========================================================================
DEF_3DV_CMD(StdCmdViewTop);

StdCmdViewTop::StdCmdViewTop()
  :FCCppCommand("Std_ViewTop")
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

void StdCmdViewTop::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewTop\")");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================
DEF_3DV_CMD(StdCmdViewAxo);

StdCmdViewAxo::StdCmdViewAxo()
  :FCCppCommand("Std_ViewAxo")
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

void StdCmdViewAxo::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewAxo\")");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================
DEF_STD_CMD_A(StdCmdViewFitAll);

StdCmdViewFitAll::StdCmdViewFitAll()
  :FCCppCommand("Std_ViewFitAll")
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

void StdCmdViewFitAll::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFit\")");
}

bool StdCmdViewFitAll::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("ViewFit");
}


//===========================================================================
// Std_ViewCreateOCC
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreateOCC);

StdCmdViewCreateOCC::StdCmdViewCreateOCC()
  :FCCppCommand("Std_ViewCreateOCC")
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

void StdCmdViewCreateOCC::Activated(int iMsg)
{
  GetActiveDocument()->CreateView("View3DOcc");
}

bool StdCmdViewCreateOCC::IsActive(void)
{
  return (GetActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewCreateInventor
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreateInventor);

StdCmdViewCreateInventor::StdCmdViewCreateInventor()
  :FCCppCommand("Std_ViewCreateInventor")
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

void StdCmdViewCreateInventor::Activated(int iMsg)
{
  GetActiveDocument()->CreateView("View3DIv");
}

bool StdCmdViewCreateInventor::IsActive(void)
{
  return (GetActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewExample1
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample1);

StdCmdViewExample1::StdCmdViewExample1()
  :FCCppCommand("Std_ViewExample1")
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

void StdCmdViewExample1::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"Example1\")");
}

bool StdCmdViewExample1::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Example1");
}
//===========================================================================
// Std_ViewExample2
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample2);

StdCmdViewExample2::StdCmdViewExample2()
  :FCCppCommand("Std_ViewExample2")
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

void StdCmdViewExample2::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"Example2\")");
}

bool StdCmdViewExample2::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Example2");
}

//===========================================================================
// Std_ViewExample3
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample3);

StdCmdViewExample3::StdCmdViewExample3()
  :FCCppCommand("Std_ViewExample3")
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

void StdCmdViewExample3::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"Example3\")");
}

bool StdCmdViewExample3::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Example3");
}

//===========================================================================
// Std_ViewIvDecorationOn
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvDecorationOn);

StdCmdViewIvDecorationOn::StdCmdViewIvDecorationOn()
  :FCCppCommand("Std_ViewIvDecorationOn")
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

void StdCmdViewIvDecorationOn::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"SetDecorationOn\")");
}

bool StdCmdViewIvDecorationOn::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("SetDecorationOn");
}

//===========================================================================
// Std_ViewIvDecorationOff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvDecorationOff);

StdCmdViewIvDecorationOff::StdCmdViewIvDecorationOff()
  :FCCppCommand("Std_ViewIvDecorationOff")
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

void StdCmdViewIvDecorationOff::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"SetDecorationOff\")");
}

bool StdCmdViewIvDecorationOff::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("SetDecorationOff");
}


//===========================================================================
// Std_ViewIvStereoOff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoOff);

StdCmdViewIvStereoOff::StdCmdViewIvStereoOff()
  :FCCppCommand("Std_ViewIvStereoOff")
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

void StdCmdViewIvStereoOff::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoOff\")");
}

bool StdCmdViewIvStereoOff::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("SetStereoOff");
}


//===========================================================================
// Std_ViewIvStereoOn
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoOn);

StdCmdViewIvStereoOn::StdCmdViewIvStereoOn()
  :FCCppCommand("Std_ViewIvStereoOn")
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

void StdCmdViewIvStereoOn::Activated(int iMsg)
{
  DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoOn\")");
}

bool StdCmdViewIvStereoOn::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("SetStereoOn");
}





//===========================================================================
// Instanciation
//===========================================================================


void CreateViewStdCommands(void)
{
  FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  // views
  rcCmdMgr.AddCommand(new StdCmdViewBottom());
  rcCmdMgr.AddCommand(new StdCmdViewFront());
  rcCmdMgr.AddCommand(new StdCmdViewLeft());
  rcCmdMgr.AddCommand(new StdCmdViewRear());
  rcCmdMgr.AddCommand(new StdCmdViewRight());
  rcCmdMgr.AddCommand(new StdCmdViewTop());
  rcCmdMgr.AddCommand(new StdCmdViewAxo());
  rcCmdMgr.AddCommand(new StdCmdViewFitAll());

  rcCmdMgr.AddCommand(new StdCmdViewExample1());
  rcCmdMgr.AddCommand(new StdCmdViewExample2());
  rcCmdMgr.AddCommand(new StdCmdViewExample3());

  rcCmdMgr.AddCommand(new StdCmdViewIvDecorationOff());
  rcCmdMgr.AddCommand(new StdCmdViewIvDecorationOn());

  rcCmdMgr.AddCommand(new StdCmdViewIvStereoOn());
  rcCmdMgr.AddCommand(new StdCmdViewIvStereoOff());

  rcCmdMgr.AddCommand(new StdCmdViewCreateOCC());
  rcCmdMgr.AddCommand(new StdCmdViewCreateInventor());
}


