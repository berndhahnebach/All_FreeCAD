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
# include <TopExp_Explorer.hxx>
# include <QMessageBox>
#endif

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>

#include <Mod/Part/App/Part2DObject.h>


using namespace std;

//===========================================================================
// Part_Pad
//===========================================================================

/* Sketch commands =======================================================*/
DEF_STD_CMD_A(CmdPartDesignNewSketch);

CmdPartDesignNewSketch::CmdPartDesignNewSketch()
	:Command("PartDesign_NewSketch")
{
    sAppModule      = "PartDesign";
    sGroup          = QT_TR_NOOP("PartDesign");
    sMenuText       = QT_TR_NOOP("Create sketch");
    sToolTipText    = QT_TR_NOOP("Create a new sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_NewSketch";
}


void CmdPartDesignNewSketch::activated(int iMsg)
{
    const char camstring[] = "#Inventor V2.1 ascii \\n OrthographicCamera { \\n viewportMapping ADJUST_CAMERA \\n position 0 0 87 \\n orientation 0 0 1  0 \\n nearDistance 37 \\n farDistance 137 \\n aspectRatio 1 \\n focalDistance 87 \\n height 119 }";

    std::string FeatName = getUniqueObjectName("Sketch");

    std::string cam(camstring);

    openCommand("Create a new Sketch");
    doCommand(Doc,"App.activeDocument().addObject('Sketcher::SketchObject','%s')",FeatName.c_str());
    doCommand(Gui,"Gui.activeDocument().activeView().setCamera('%s')",cam.c_str());
    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    
    //getDocument()->recompute();
}

bool CmdPartDesignNewSketch::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


//===========================================================================
// Part_Pad
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignPad);

CmdPartDesignPad::CmdPartDesignPad()
  : Command("PartDesign_Pad")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Pad");
    sToolTipText  = QT_TR_NOOP("Pad a selected sketch");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Pad";
    iAccel        = 0;
}

void CmdPartDesignPad::activated(int iMsg)
{
    unsigned int n = getSelection().countObjectsOfType(Part::Part2DObject::getClassTypeId());
    if (n != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select a sketch or 2D object."));
        return;
    }

    std::string FeatName = getUniqueObjectName("Pad");

    std::vector<App::DocumentObject*> Sel = getSelection().getObjectsOfType(Part::Part2DObject::getClassTypeId());
    Part::Part2DObject* part = static_cast<Part::Part2DObject*>(Sel.front());
    const TopoDS_Shape& shape = part->Shape.getValue();
    if (shape.IsNull()) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("The shape of the selected object is empty."));
        return;
    }

    // count free wires
    int ctWires=0;
    TopExp_Explorer ex;
    for (ex.Init(shape, TopAbs_WIRE, TopAbs_FACE); ex.More(); ex.Next()) {
        ctWires++;
    }
    if (ctWires == 0) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("The shape of the selected object is not a wire."));
        return;
    }

    openCommand("Make Pad");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::Pad\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = App.activeDocument().%s",FeatName.c_str(),part->getNameInDocument());
    doCommand(Doc,"App.activeDocument().%s.Dir = (0.0,0.0,-5.0)",FeatName.c_str());
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",part->getNameInDocument());
    updateActive();
    commitCommand();
}

bool CmdPartDesignPad::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// Part_Pad
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignFillet);

CmdPartDesignFillet::CmdPartDesignFillet()
  :Command("PartDesign_Fillet")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Fillet");
    sToolTipText  = QT_TR_NOOP("Make a fillet on an edge, face or body");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Fillet";
    iAccel        = 0;
}

void CmdPartDesignFillet::activated(int iMsg)
{
    //unsigned int n = getSelection().countObjectsOfType(Part::Feature::getClassTypeId());

	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edge, face or body. Only one body is allowed."));
        return;
    }

	if ( ! selection[0].isObjectTypeOf(Part::Feature::getClassTypeId())){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong object type"),
            QObject::tr("Fillet works only on parts"));
        return;
    }
	std::string SelString = selection[0].getAsPropertyLinkSubString();
    std::string FeatName = getUniqueObjectName("Fillet");

    openCommand("Make Fillet");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::Fillet\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = %s",FeatName.c_str(),SelString.c_str());
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",selection[0].getFeatName());
    updateActive();
    commitCommand();
}

bool CmdPartDesignFillet::isActive(void)
{
    return hasActiveDocument();
}


void CreatePartDesignCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdPartDesignPad());
    rcCmdMgr.addCommand(new CmdPartDesignFillet());
    rcCmdMgr.addCommand(new CmdPartDesignNewSketch());
 }
