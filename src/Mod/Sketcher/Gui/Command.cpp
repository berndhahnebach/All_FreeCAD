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
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>

#include <Mod/Sketcher/App/SketchObjectSF.h>

#include "ViewProviderSketch.h"

using namespace std;
using namespace SketcherGui;

/* Sketch commands =======================================================*/
DEF_STD_CMD_A(CmdSketcherNewSketch);

CmdSketcherNewSketch::CmdSketcherNewSketch()
	:Command("Sketcher_NewSketch")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create sketch");
    sToolTipText    = QT_TR_NOOP("Create a new sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_NewSketch";
}


void CmdSketcherNewSketch::activated(int iMsg)
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

bool CmdSketcherNewSketch::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}



DEF_STD_CMD_A(CmdSketcherLeaveSketch);

CmdSketcherLeaveSketch::CmdSketcherLeaveSketch()
	:Command("Sketcher_LeaveSketch")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Leave sketch");
    sToolTipText    = QT_TR_NOOP("Close the editing of the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_LeaveSketch";
}

void CmdSketcherLeaveSketch::activated(int iMsg)
{
    openCommand("Sketch changed");
    doCommand(Gui,"Gui.activeDocument().resetEdit()");
    doCommand(Doc,"App.ActiveDocument.recompute()");
    commitCommand();

}

bool CmdSketcherLeaveSketch::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
	return false;
}

// Sketchflat integration ++++++++++++++++++++++++++++++++++++++++++++++++

DEF_STD_CMD_A(CmdSketcherNewSketchSF);

CmdSketcherNewSketchSF::CmdSketcherNewSketchSF()
	:Command("Sketcher_NewSketchSF")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Sketchflat sketch");
    sToolTipText    = QT_TR_NOOP("Create a new sketchflat sketch by starting externel editor");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_NewSketch";
}

void CmdSketcherNewSketchSF::activated(int iMsg)
{

    std::string FeatName = getUniqueObjectName("Sketch");

    openCommand("Create a new Sketch");
    doCommand(Doc,"App.activeDocument().addObject('Sketcher::SketchObjectSF','%s')",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.SketchFlatFile = App.getResourceDir()+'Mod/Sketcher/Templates/Sketch.skf'",FeatName.c_str());
  
    Sketcher::SketchObjectSF *obj = static_cast<Sketcher::SketchObjectSF *>(getDocument()->getObject( FeatName.c_str() ));

    Gui::Dialog::DlgEditFileIncludePropertyExternal dlg((obj->SketchFlatFile),Gui::getMainWindow());
    dlg.ProcName = QString::fromUtf8((App::Application::Config()["AppHomePath"] + "bin/sketchflat.exe").c_str());
    dlg.Do();

    commitCommand();
    getDocument()->recompute();
}

bool CmdSketcherNewSketchSF::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


void CreateSketcherCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdSketcherNewSketch());
    rcCmdMgr.addCommand(new CmdSketcherLeaveSketch());
    rcCmdMgr.addCommand(new CmdSketcherNewSketchSF());

 }
