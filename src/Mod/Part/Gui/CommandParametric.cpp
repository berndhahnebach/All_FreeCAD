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
#include "DlgPartCylinderImp.h"
#include "DlgPartBoxImp.h"

//===========================================================================
// Part_Cylinder
//===========================================================================
DEF_STD_CMD_A(CmdPartCylinder);

CmdPartCylinder::CmdPartCylinder()
  : Command("Part_Cylinder")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Create Cylinder...");
    sToolTipText  = QT_TR_NOOP("Create a Cylinder");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Cylinder";
    iAccel        = 0;
}

void CmdPartCylinder::activated(int iMsg)
{
    PartGui::DlgPartCylinderImp dlg(Gui::getMainWindow());
    if (dlg.exec()== QDialog::Accepted) {
        Base::Vector3f dir = dlg.getDirection();
        openCommand("Create Part Cylinder");
        doCommand(Doc,"from FreeCAD import Base");
        doCommand(Doc,"import Part");
        doCommand(Doc,"__cf__ = App.ActiveDocument.addObject(\"Part::Cylinder\",\"Cylinder\")");
        doCommand(Doc,"__cf__.Location = Base.Vector(%f,%f,%f)",
                        dlg.xPos->value(),
                        dlg.yPos->value(),
                        dlg.zPos->value());
        doCommand(Doc,"__cf__.Axis = Base.Vector(%f,%f,%f)",
                        dir.x, dir.y, dir.z);
        doCommand(Doc,"__cf__.Radius = %f", dlg.radius->value());
        doCommand(Doc,"__cf__.Height = %f", dlg.length->value());
        doCommand(Doc,"del __cf__");
        commitCommand();
        updateActive();
    }
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
  :Command("Part_Box")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Create box...");
    sToolTipText  = QT_TR_NOOP("Create a Box feature");
    sWhatsThis    = "Part_Box";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Box";
    iAccel        = 0;
}

void CmdPartBox::activated(int iMsg)
{
    PartGui::DlgPartBoxImp dlg(Gui::getMainWindow());
    if (dlg.exec()== QDialog::Accepted) {
        Base::Vector3f dir = dlg.getDirection();
        openCommand("Part Box Create");
        doCommand(Doc,"from FreeCAD import Base");
        doCommand(Doc,"import Part");
        doCommand(Doc,"__fb__ = App.ActiveDocument.addObject(\"Part::Box\",\"PartBox\")");
        doCommand(Doc,"__fb__.Location = Base.Vector(%f,%f,%f)",
                                 dlg.xPos->value(),
                                 dlg.yPos->value(),
                                 dlg.zPos->value());
        doCommand(Doc,"__fb__.Axis = Base.Vector(%f,%f,%f)",
                        dir.x, dir.y, dir.z);
        doCommand(Doc,"__fb__.Length = %f",dlg.uLength->value());
        doCommand(Doc,"__fb__.Width = %f" ,dlg.vLength->value());
        doCommand(Doc,"__fb__.Height = %f",dlg.wLength->value());
        doCommand(Doc,"del __fb__");
        commitCommand();
        updateActive();
    }
}

bool CmdPartBox::isActive(void)
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
}
