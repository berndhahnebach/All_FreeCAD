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
#include <Gui/FileDialog.h>

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
    sMenuText       = QT_TR_NOOP("Constraint Axle...");
    sToolTipText    = QT_TR_NOOP("Set an axle constraint between two objects");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_NewSketch";
}


void CmdSketcherNewSketch::activated(int iMsg)
{
    openCommand("Sketcher Create a new Sketch");
    doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObject\",\"Sketch\")");
    commitCommand();
      
}

bool CmdSketcherNewSketch::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

/* Sketch commands =======================================================*/
DEF_STD_CMD_A(CmdSketcherCreateArc);

CmdSketcherCreateArc::CmdSketcherCreateArc()
	:Command("Sketcher_CreateArc")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create arc");
    sToolTipText    = QT_TR_NOOP("Create arc an arc in the scetch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateArc";
}


void CmdSketcherCreateArc::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())->setSketchMode(1);
      
}

bool CmdSketcherCreateArc::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreateCircle);

CmdSketcherCreateCircle::CmdSketcherCreateCircle()
	:Command("Sketcher_CreateCircle")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create circle");
    sToolTipText    = QT_TR_NOOP("Create arc a circle in the scetch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateCircle";
}


void CmdSketcherCreateCircle::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObject\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateCircle::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreateLine);

CmdSketcherCreateLine::CmdSketcherCreateLine()
	:Command("Sketcher_CreateLine")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create line");
    sToolTipText    = QT_TR_NOOP("Create arc a line in the scetch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateLine";
}


void CmdSketcherCreateLine::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())->setSketchMode(ViewProviderSketch::STATUS_SKETCH_CreateLine);
      
}

bool CmdSketcherCreateLine::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreatePolyline);

CmdSketcherCreatePolyline::CmdSketcherCreatePolyline()
	:Command("Sketcher_CreatePolyline")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create arc");
    sToolTipText    = QT_TR_NOOP("Create arc an arc in the scetch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreatePolyline";
}


void CmdSketcherCreatePolyline::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObject\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreatePolyline::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreateRectangle);

CmdSketcherCreateRectangle::CmdSketcherCreateRectangle()
	:Command("Sketcher_CreateRectangle")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create rectagle");
    sToolTipText    = QT_TR_NOOP("Create arc a rectagle in the scetch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateRectangle";
}


void CmdSketcherCreateRectangle::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObject\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateRectangle::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreateText);

CmdSketcherCreateText::CmdSketcherCreateText()
	:Command("Sketcher_CreateText")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create text");
    sToolTipText    = QT_TR_NOOP("Create text in the scetch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateText";
}


void CmdSketcherCreateText::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObject\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateText::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
	//		return true;
	return false;
}


void CreateSketcherCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdSketcherNewSketch());

	rcCmdMgr.addCommand(new CmdSketcherCreateArc());
	rcCmdMgr.addCommand(new CmdSketcherCreateCircle());
	rcCmdMgr.addCommand(new CmdSketcherCreateLine());
	rcCmdMgr.addCommand(new CmdSketcherCreatePolyline());
	rcCmdMgr.addCommand(new CmdSketcherCreateRectangle());
	rcCmdMgr.addCommand(new CmdSketcherCreateText());
 }
