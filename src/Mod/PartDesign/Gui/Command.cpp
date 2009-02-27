/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>


using namespace std;

//===========================================================================
// Part_Pad
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignPad);

CmdPartDesignPad::CmdPartDesignPad()
  :Command("PartDesign_Pad")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Pad...");
    sToolTipText  = QT_TR_NOOP("Pad a selected sketch");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    iAccel        = 0;
}

void CmdPartDesignPad::activated(int iMsg)
{
    openCommand("Exrud");
    doCommand(Doc,"f = App.activeDocument().addObject(\"PartDesign::Pad\",\"Pad\")");
    doCommand(Doc,"f.Base = Simple");
    doCommand(Doc,"f.Dir = (0.0,0.0,1.0)");
    commitCommand();
    updateActive();
}

bool CmdPartDesignPad::isActive(void)
{
    return hasActiveDocument();
}

void CreatePartDesignCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdPartDesignPad());
 }
