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
#	include <qaction.h>
#	include <BRepPrimAPI_MakeBox.hxx>
#	include <TopoDS_Shape.hxx>
#	include <TNaming_Builder.hxx>
#endif

#include "../../../Base/Exception.h"
#include "../../../App/Document.h"
#include "../../../Gui/Application.h"
#include "../../../Gui/Command.h"


#include "../../../Gui/Icons/images.cpp"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// FCCmd_TEMPLATE_Test THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD(FCCmd_TEMPLATE_Test);

FCCmd_TEMPLATE_Test::FCCmd_TEMPLATE_Test()
	:FCCppCommand("_TEMPLATE__Test")
{
	sAppModule		= "_TEMPLATE_";
	sGroup			= "_TEMPLATE_";
	sMenuText		= "Test1";
	sToolTipText	= "_TEMPLATE_ Test function";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Test1";
	iAccel			= Qt::CTRL+Qt::Key_T;
}


void FCCmd_TEMPLATE_Test::Activated(int iMsg)
{
  GetConsole().Message("Hello, World!\n");
}

void CreateCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.AddCommand(new FCCmd_TEMPLATE_Test());
}
