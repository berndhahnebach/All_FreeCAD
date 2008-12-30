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
# include <Inventor/events/SoMouseButtonEvent.h>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>

#include "../App/PartFeature.h"
#include "DlgPartCylinderImp.h"


//===========================================================================
// Part_SimpleCylinder
//===========================================================================
DEF_STD_CMD_A(CmdPartSimpleCylinder);

CmdPartSimpleCylinder::CmdPartSimpleCylinder()
  :Command("Part_SimpleCylinder")
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

void CmdPartSimpleCylinder::activated(int iMsg)
{
    PartGui::DlgPartCylinderImp dlg(Gui::getMainWindow());
    if (dlg.exec()== QDialog::Accepted) {
        Base::Vector3f dir = dlg.getDirection();
        openCommand("Create Part Cylinder");
        doCommand(Doc,"import Base,Part");
        doCommand(Doc,"App.ActiveDocument.addObject(\"Part::Feature\",\"Cylinder\")"
                      ".Shape=Part.makeCylinder(%f,%f,"
                      "Base.Vector(%f,%f,%f),"
                      "Base.Vector(%f,%f,%f))"
                     ,dlg.radius->value()
                     ,dlg.length->value()
                     ,dlg.xPos->value()
                     ,dlg.yPos->value()
                     ,dlg.zPos->value()
                     ,dir.x,dir.y,dir.z);
        commitCommand();
        updateActive();
    }
}

bool CmdPartSimpleCylinder::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CreateSimplePartCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdPartSimpleCylinder());
} 
