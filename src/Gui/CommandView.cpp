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
 
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif


#include "Application.h"
#include "../Base/Exception.h"
#include "../App/Document.h"

#include "Icons/images.cpp"

#include "Command.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_ViewBottom
//===========================================================================
DEF_STD_CMD(FCCmdViewBottom		,"Std_ViewBottom");

void FCCmdViewBottom::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Bottom";
	*sToolTipText = "Set to bottom view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_bottom);
	iAccel = 0;
}


void FCCmdViewBottom::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewBottom");
}

//===========================================================================
// Std_ViewFront
//===========================================================================
DEF_STD_CMD(FCCmdViewFront		,"Std_ViewFront");

void FCCmdViewFront::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Front";
	*sToolTipText = "Set to front view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_front);
	iAccel = 0;
}


void FCCmdViewFront::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewFront");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================
DEF_STD_CMD(FCCmdViewLeft		,"Std_ViewLeft");

void FCCmdViewLeft::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Left";
	*sToolTipText = "Set to left view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_left);
	iAccel = 0;
}


void FCCmdViewLeft::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewLeft");
}

//===========================================================================
// Std_ViewRear
//===========================================================================
DEF_STD_CMD(FCCmdViewRear		,"Std_ViewRear");

void FCCmdViewRear::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Rear";
	*sToolTipText = "Set to rear view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_back);
	iAccel = 0;
}


void FCCmdViewRear::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewRear");
}

//===========================================================================
// Std_ViewRight
//===========================================================================
DEF_STD_CMD(FCCmdViewRight		,"Std_ViewRight");

void FCCmdViewRight::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Right";
	*sToolTipText = "Set to right view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_right);
	iAccel = 0;
}


void FCCmdViewRight::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewRight");
}

//===========================================================================
// Std_ViewTop
//===========================================================================
DEF_STD_CMD(FCCmdViewTop		,"Std_ViewTop");

void FCCmdViewTop::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Top";
	*sToolTipText = "Set to top view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_top);
	iAccel = 0;
}


void FCCmdViewTop::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewTop");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================
DEF_STD_CMD(FCCmdViewAxo		,"Std_ViewAxo");

void FCCmdViewAxo::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Axometric";
	*sToolTipText = "Set to axometric view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_axo);
	iAccel = 0;
}


void FCCmdViewAxo::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewAxo");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================
DEF_STD_CMD(FCCmdViewFitAll		,"Std_ViewFitAll");

void FCCmdViewFitAll::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Fit all";
	*sToolTipText = "Fits the whole content on the screen";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_fitall);
	iAccel = 0;
}


void FCCmdViewFitAll::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewFit");
}


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

}


