/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   J�rgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>


using namespace std;

DEF_STD_CMD(CmdCompleteConstraintAxle);

CmdCompleteConstraintAxle::CmdCompleteConstraintAxle()
	:Command("Complete_ConstraintAxle")
{
    sAppModule      = "Complete";
    sGroup          = QT_TR_NOOP("Complete");
    sMenuText       = QT_TR_NOOP("Constraint Axle...");
    sToolTipText    = QT_TR_NOOP("set a axle constraint between two objects");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/document-new";
}


void CmdCompleteConstraintAxle::activated(int iMsg)
{
    // load the file with the module
    //Command::doCommand(Command::Gui, "import Complete, CompleteGui");
      
}



void CreateCompleteCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdCompleteConstraintAxle());
 }
