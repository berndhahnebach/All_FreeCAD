/** \file CommandStd.h
 *  \brief The implementation of the standrd Commands
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
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
//DEF_STD_CMD(FCCmdViewBottom);

class FCCmdViewBottom : public FCCppCommand 
{
public:
	FCCmdViewBottom();
	virtual void Activated(int iMsg);
	virtual bool IsActive(void); 
};



FCCmdViewBottom::FCCmdViewBottom()
	:FCCppCommand("Std_ViewBottom")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Bottom";
	sToolTipText	= "Set to bottom view";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_bottom";
	iAccel			= 0;
}

bool FCCmdViewBottom::IsActive(void)
{
	return GetAppWnd()->GetActiveView() && strcmp( GetAppWnd()->GetActiveView()->GetName(), "View3D" ) == 0?true:false;
} 

void FCCmdViewBottom::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewBottom\")");
}

//===========================================================================
// Std_ViewFront
//===========================================================================
DEF_3DV_CMD(FCCmdViewFront);

FCCmdViewFront::FCCmdViewFront()
	:FCCppCommand("Std_ViewFront")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Front";
	sToolTipText	= "Set to front view";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_front";
	iAccel			= 0;
}


void FCCmdViewFront::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFront\")");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================
DEF_3DV_CMD(FCCmdViewLeft);

FCCmdViewLeft::FCCmdViewLeft()
	:FCCppCommand("Std_ViewLeft")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Left";
	sToolTipText	= "Set to left view";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_left";
	iAccel			= 0;
}


void FCCmdViewLeft::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewLeft\")");
}

//===========================================================================
// Std_ViewRear
//===========================================================================
DEF_3DV_CMD(FCCmdViewRear);

FCCmdViewRear::FCCmdViewRear()
	:FCCppCommand("Std_ViewRear")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Rear";
	sToolTipText	= "Set to rear view";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_back";
	iAccel			= 0;
}


void FCCmdViewRear::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRear\")");
}

//===========================================================================
// Std_ViewRight
//===========================================================================
DEF_3DV_CMD(FCCmdViewRight);

FCCmdViewRight::FCCmdViewRight()
	:FCCppCommand("Std_ViewRight")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Right";
	sToolTipText	= "Set to right view";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_right";
	iAccel			= 0;
}


void FCCmdViewRight::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRight\")");
}

//===========================================================================
// Std_ViewTop
//===========================================================================
DEF_3DV_CMD(FCCmdViewTop);

FCCmdViewTop::FCCmdViewTop()
	:FCCppCommand("Std_ViewTop")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Top";
	sToolTipText	= "Set to top view";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_top";
	iAccel			= 0;
}


void FCCmdViewTop::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewTop\")");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================
DEF_3DV_CMD(FCCmdViewAxo);

FCCmdViewAxo::FCCmdViewAxo()
	:FCCppCommand("Std_ViewAxo")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Axometric";
	sToolTipText	= "Set to axometric view";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_axo";
	iAccel			= 0;
}


void FCCmdViewAxo::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewAxo\")");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================
DEF_3DV_CMD(FCCmdViewFitAll);

FCCmdViewFitAll::FCCmdViewFitAll()
	:FCCppCommand("Std_ViewFitAll")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Fit all";
	sToolTipText	= "Fits the whole content on the screen";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_fitall";
	iAccel			= 0;
}


void FCCmdViewFitAll::Activated(int iMsg)
{
	DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFit\")");
}


//===========================================================================
// Std_ViewCreateOCC
//===========================================================================
DEF_STD_CMD_A(FCCmdViewCreateOCC);

FCCmdViewCreateOCC::FCCmdViewCreateOCC()
	:FCCppCommand("Std_ViewCreateOCC")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Create new OCC View";
	sToolTipText	= "Creates a new OpenCasCade view window for the active document";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_fitall";
	iAccel			= 0;
}


void FCCmdViewCreateOCC::Activated(int iMsg)
{
	GetActiveDocument()->CreateView("View3DOcc");
}

bool FCCmdViewCreateOCC::IsActive(void)
{
  return (GetActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewCreateInventor
//===========================================================================
DEF_STD_CMD_A(FCCmdViewCreateInventor);

FCCmdViewCreateInventor::FCCmdViewCreateInventor()
	:FCCppCommand("Std_ViewCreateInventor")
{
	sAppModule		= "";
	sGroup			= "Standard-View";
	sMenuText		= "Create new Inventor View";
	sToolTipText	= "Creates a new Inventor view window for the active document";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "view_fitall";
	iAccel			= 0;
}


void FCCmdViewCreateInventor::Activated(int iMsg)
{
	GetActiveDocument()->CreateView("View3DIv");
}

bool FCCmdViewCreateInventor::IsActive(void)
{
  return (GetActiveDocument()!=NULL);
}


//===========================================================================
// Instanciation
//===========================================================================


void CreateViewStdCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	// views
	rcCmdMgr.AddCommand(new FCCmdViewBottom());
	rcCmdMgr.AddCommand(new FCCmdViewFront());
	rcCmdMgr.AddCommand(new FCCmdViewLeft());
	rcCmdMgr.AddCommand(new FCCmdViewRear());
	rcCmdMgr.AddCommand(new FCCmdViewRight());
	rcCmdMgr.AddCommand(new FCCmdViewTop());
	rcCmdMgr.AddCommand(new FCCmdViewAxo());
	rcCmdMgr.AddCommand(new FCCmdViewFitAll());

	rcCmdMgr.AddCommand(new FCCmdViewCreateOCC());
	rcCmdMgr.AddCommand(new FCCmdViewCreateInventor());
}


