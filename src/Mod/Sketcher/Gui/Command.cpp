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
    sMenuText       = QT_TR_NOOP("Create sketch");
    sToolTipText    = QT_TR_NOOP("Create a new sketch");
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
    Gui::Document *doc = getActiveGuiDocument();
    doc->resetEdit();      
}

bool CmdSketcherLeaveSketch::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
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
    sToolTipText    = QT_TR_NOOP("Create an arc in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateArc";
}


void CmdSketcherCreateArc::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())->setSketchMode(ViewProviderSketch::STATUS_SKETCH_CreateArc);
      
}

bool CmdSketcherCreateArc::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	// checks if a Sketch Viewprovider is in Edit and is in no special mode
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
	//		if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
	//			->getSketchMode() == ViewProviderSketch::STATUS_NONE)
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
    sToolTipText    = QT_TR_NOOP("Create a circle in the sketch");
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


DEF_STD_CMD_A(CmdSketcherCreatePoint);

CmdSketcherCreatePoint::CmdSketcherCreatePoint()
	:Command("Sketcher_CreatePoint")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create point");
    sToolTipText    = QT_TR_NOOP("Create a point in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreatePoint";
}


void CmdSketcherCreatePoint::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObject\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreatePoint::isActive(void)
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
    sToolTipText    = QT_TR_NOOP("Create a line in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateLine";
}

void CmdSketcherCreateLine::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
			    ->setSketchMode(ViewProviderSketch::STATUS_SKETCH_CreateLine);
      
}

bool CmdSketcherCreateLine::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreatePolyline);

CmdSketcherCreatePolyline::CmdSketcherCreatePolyline()
	:Command("Sketcher_CreatePolyline")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create polyline");
    sToolTipText    = QT_TR_NOOP("Create a polyline in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreatePolyline";
}

void CmdSketcherCreatePolyline::activated(int iMsg)
{
    Gui::Document *doc = getActiveGuiDocument();
    if (doc) {
        Gui::ViewProvider* vp = doc->getInEdit();
        if (vp && vp->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
            static_cast<SketcherGui::ViewProviderSketch*>(vp)->setSketchMode
            (ViewProviderSketch::STATUS_SKETCH_CreatePolyline);
    }
}

bool CmdSketcherCreatePolyline::isActive(void)
{
    Gui::Document *doc = getActiveGuiDocument();
    if (doc) {
        // checks if a sketch is in edit mode and is in no special mode
        Gui::ViewProvider* vp = doc->getInEdit();
        if (vp && vp->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
            if (static_cast<SketcherGui::ViewProviderSketch*>(vp)->getSketchMode() ==
                ViewProviderSketch::STATUS_NONE)
                return true;
    }

    return false;
}


DEF_STD_CMD_A(CmdSketcherCreateRectangle);

CmdSketcherCreateRectangle::CmdSketcherCreateRectangle()
	:Command("Sketcher_CreateRectangle")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create rectangle");
    sToolTipText    = QT_TR_NOOP("Create a rectangle in the sketch");
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
    sToolTipText    = QT_TR_NOOP("Create text in the sketch");
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


DEF_STD_CMD_A(CmdSketcherCreateDraftLine);

CmdSketcherCreateDraftLine::CmdSketcherCreateDraftLine()
	:Command("Sketcher_CreateDraftLine")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create draft line");
    sToolTipText    = QT_TR_NOOP("Create a draft line in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_DraftLine";
}


void CmdSketcherCreateDraftLine::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObject\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateDraftLine::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
	//		return true;
	return false;
}

/* Constrain commands =======================================================*/
DEF_STD_CMD_A(CmdSketcherConstrainHorizontal);

CmdSketcherConstrainHorizontal::CmdSketcherConstrainHorizontal()
	:Command("Sketcher_ConstrainHorizontal")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain orizontal");
    sToolTipText    = QT_TR_NOOP("Create a horizontal constrain on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_ConstrainHorizontal";
}


void CmdSketcherConstrainHorizontal::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())->setConstrainOnSelected(ViewProviderSketch::CONSTRAIN_HORIZONTAL);
      
}

bool CmdSketcherConstrainHorizontal::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
	return false;
}

DEF_STD_CMD_A(CmdSketcherConstrainVertical);

CmdSketcherConstrainVertical::CmdSketcherConstrainVertical()
	:Command("Sketcher_ConstrainVertical")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain vertical");
    sToolTipText    = QT_TR_NOOP("Create a vertical constrain on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_ConstrainVertical";
}


void CmdSketcherConstrainVertical::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())->setConstrainOnSelected(ViewProviderSketch::CONSTRAIN_VERTICAL);
      
}

bool CmdSketcherConstrainVertical::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherConstrainLock);

CmdSketcherConstrainLock::CmdSketcherConstrainLock()
	:Command("Sketcher_ConstrainLock")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain Lock");
    sToolTipText    = QT_TR_NOOP("Create a lock constrain on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_ConstrainLock";
}


void CmdSketcherConstrainLock::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())->setConstrainOnSelected(ViewProviderSketch::CONSTRAIN_LOCK);
      
}

bool CmdSketcherConstrainLock::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
	return false;
}



void CreateSketcherCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdSketcherNewSketch());
    rcCmdMgr.addCommand(new CmdSketcherLeaveSketch());

	rcCmdMgr.addCommand(new CmdSketcherCreatePoint());
	rcCmdMgr.addCommand(new CmdSketcherCreateArc());
	rcCmdMgr.addCommand(new CmdSketcherCreateCircle());
	rcCmdMgr.addCommand(new CmdSketcherCreateLine());
	rcCmdMgr.addCommand(new CmdSketcherCreatePolyline());
	rcCmdMgr.addCommand(new CmdSketcherCreateRectangle());
	rcCmdMgr.addCommand(new CmdSketcherCreateText());
	rcCmdMgr.addCommand(new CmdSketcherCreateDraftLine());

	rcCmdMgr.addCommand(new CmdSketcherConstrainHorizontal());
	rcCmdMgr.addCommand(new CmdSketcherConstrainVertical());
	rcCmdMgr.addCommand(new CmdSketcherConstrainLock());
 }
