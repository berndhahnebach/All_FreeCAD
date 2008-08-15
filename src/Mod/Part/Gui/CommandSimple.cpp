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
# include <qdir.h>
# include <qfileinfo.h>
# include <qlineedit.h>
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
#include "DlgPrimitives.h"


using Gui::FileDialog;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//===========================================================================
// Part_Cylinder
//===========================================================================
DEF_STD_CMD_A(CmdPartCylinder);

CmdPartCylinder::CmdPartCylinder()
  :Command("Part_Cylinder")
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
    PartGui::DlgPartCylinderImp cDlg(Gui::getMainWindow());
    if (cDlg.exec()== QDialog::Accepted)
    {
        this->
        openCommand("Create Part Cylinder");
        doCommand(Doc,"import Base,Part");
        doCommand(Doc,"g = Part.Cylinder()");
        doCommand(Doc,"g.Center = Base.Vector(%f,%f,%f)",cDlg.XPos->text().toFloat(),
                                                         cDlg.YPos->text().toFloat(),
                                                         cDlg.ZPos->text().toFloat());
        doCommand(Doc,"g.Radius = %f",cDlg.Radius->text().toFloat());
        doCommand(Doc,"g.Axis = Base.Vector(0,0,1)");      
        doCommand(Doc,"f = App.activeDocument().addObject(\"Part::Feature\",\"Cylinder\")");
        doCommand(Doc,"f.Shape = Part.Shape([g])");
        commitCommand();
  
        updateActive();
    }
}

bool CmdPartCylinder::isActive(void)
{
    if( getActiveGuiDocument() )
        return true;
    else
        return false;
}


//===========================================================================
// Part_Primitives
//===========================================================================
DEF_STD_CMD_A(CmdPartPrimitives);

CmdPartPrimitives::CmdPartPrimitives()
  :Command("Part_Primitives")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Create primitives...");
    sToolTipText  = QT_TR_NOOP("Creation of geometric primitives");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    //sPixmap       = "Part_Box";
    iAccel        = 0;
}

void CmdPartPrimitives::activated(int iMsg)
{
    static QPointer<QDialog> dlg = 0;
    if (!dlg)
        dlg = new PartGui::DlgPrimitives(Gui::getMainWindow());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

bool CmdPartPrimitives::isActive(void)
{
    return hasActiveDocument();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CreateSimplePartCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdPartCylinder());
    rcCmdMgr.addCommand(new CmdPartPrimitives());

} 


