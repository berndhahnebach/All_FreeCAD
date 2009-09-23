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

#include <App/Application.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>


using namespace std;

DEF_STD_CMD_A(CmdRobotConstraintAxle);

CmdRobotConstraintAxle::CmdRobotConstraintAxle()
	:Command("Robot_Create")
{
    sAppModule      = "Robot";
    sGroup          = QT_TR_NOOP("Robot");
    sMenuText       = QT_TR_NOOP("Place robot...");
    sToolTipText    = QT_TR_NOOP("Place a robot (experimental!)");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = 0;
}


void CmdRobotConstraintAxle::activated(int iMsg)
{
    std::string FeatName = getUniqueObjectName("Robot");
    std::string RobotPath = App::Application::getResourceDir() + "Mod/Robot/Lib/Kuka/kr500_1.wrl";

    openCommand("Place robot");
    doCommand(Doc,"App.activeDocument().addObject(\"Robot::RobotObject\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.RobotVrmlFile = \"%s\"",FeatName.c_str(),RobotPath.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis2 = -90",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Axis3 = 90",FeatName.c_str());
    updateActive();
    commitCommand();
      
}

bool CmdRobotConstraintAxle::isActive(void)
{
    return hasActiveDocument();
}



void CreateRobotCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdRobotConstraintAxle());
 }
