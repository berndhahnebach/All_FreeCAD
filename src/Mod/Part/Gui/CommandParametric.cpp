/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <QDir>
# include <QFileInfo>
# include <QLineEdit>
#endif

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>

//===========================================================================
// Part_Cylinder
//===========================================================================
DEF_STD_CMD_A(CmdPartCylinder);

CmdPartCylinder::CmdPartCylinder()
  : Command("Part_Cylinder")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Cylinder");
    sToolTipText  = QT_TR_NOOP("Create a Cylinder");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Cylinder";
    iAccel        = 0;
}

void CmdPartCylinder::activated(int iMsg)
{
    openCommand("Create cylinder");
    doCommand(Doc,"App.ActiveDocument.addObject(\"Part::Cylinder\",\"Cylinder\")");
    commitCommand();
    updateActive();
}

bool CmdPartCylinder::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Part_Box
//===========================================================================
DEF_STD_CMD_A(CmdPartBox);

CmdPartBox::CmdPartBox()
  : Command("Part_Box")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Box");
    sToolTipText  = QT_TR_NOOP("Create a box solid");
    sWhatsThis    = "Part_Box";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Box";
    iAccel        = 0;
}

void CmdPartBox::activated(int iMsg)
{
    openCommand("Create box");
    doCommand(Doc,"App.ActiveDocument.addObject(\"Part::Box\",\"Box\")");
    commitCommand();
    updateActive();
}

bool CmdPartBox::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Part_Sphere
//===========================================================================
DEF_STD_CMD_A(CmdPartSphere);

CmdPartSphere::CmdPartSphere()
  : Command("Part_Sphere")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Sphere");
    sToolTipText  = QT_TR_NOOP("Create a sphere solid");
    sWhatsThis    = "Part_Sphere";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Sphere";
    iAccel        = 0;
}

void CmdPartSphere::activated(int iMsg)
{
    openCommand("Create box");
    doCommand(Doc,"App.ActiveDocument.addObject(\"Part::Sphere\",\"Sphere\")");
    commitCommand();
    updateActive();
}

bool CmdPartSphere::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Part_Cone
//===========================================================================
DEF_STD_CMD_A(CmdPartCone);

CmdPartCone::CmdPartCone()
  : Command("Part_Cone")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Cone");
    sToolTipText  = QT_TR_NOOP("Create a cone solid");
    sWhatsThis    = "Part_Cone";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Cone";
    iAccel        = 0;
}

void CmdPartCone::activated(int iMsg)
{
    openCommand("Create cone");
    doCommand(Doc,"App.ActiveDocument.addObject(\"Part::Cone\",\"Cone\")");
    commitCommand();
    updateActive();
}

bool CmdPartCone::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Part_Torus
//===========================================================================
DEF_STD_CMD_A(CmdPartTorus);

CmdPartTorus::CmdPartTorus()
  : Command("Part_Torus")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Torus");
    sToolTipText  = QT_TR_NOOP("Create a torus solid");
    sWhatsThis    = "Part_Torus";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Torus";
    iAccel        = 0;
}

void CmdPartTorus::activated(int iMsg)
{
    openCommand("Create cone");
    doCommand(Doc,"App.ActiveDocument.addObject(\"Part::Torus\",\"Torus\")");
    commitCommand();
    updateActive();
}

bool CmdPartTorus::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CreateParamPartCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdPartCylinder());
    rcCmdMgr.addCommand(new CmdPartBox());
    rcCmdMgr.addCommand(new CmdPartSphere());
    rcCmdMgr.addCommand(new CmdPartCone());
    rcCmdMgr.addCommand(new CmdPartTorus());
}
