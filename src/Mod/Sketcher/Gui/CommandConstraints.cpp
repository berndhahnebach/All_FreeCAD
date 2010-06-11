/***************************************************************************
 *   Copyright (c) 2010 Jürgen Riegel (juergen.riegel@web.de)              *
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

#include "ViewProviderSketchSF.h"

using namespace std;
using namespace SketcherGui;


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
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())->setConstrainOnSelected(ViewProviderSketchSF::CONSTRAIN_HORIZONTAL);
      
}

bool CmdSketcherConstrainHorizontal::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketchSF::STATUS_NONE)
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
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())->setConstrainOnSelected(ViewProviderSketchSF::CONSTRAIN_VERTICAL);
      
}

bool CmdSketcherConstrainVertical::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketchSF::STATUS_NONE)
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
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())->setConstrainOnSelected(ViewProviderSketchSF::CONSTRAIN_LOCK);
      
}

bool CmdSketcherConstrainLock::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketchSF::STATUS_NONE)
			return true;
	return false;
}

DEF_STD_CMD_A(CmdSketcherConstrainCoincident);

CmdSketcherConstrainCoincident::CmdSketcherConstrainCoincident()
	:Command("Sketcher_ConstrainCoincident")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain Lock");
    sToolTipText    = QT_TR_NOOP("Create a lock constrain on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_ConstrainCoincident";
}


void CmdSketcherConstrainCoincident::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())->setConstrainOnSelected(ViewProviderSketchSF::CONSTRAIN_LOCK);
      
}

bool CmdSketcherConstrainCoincident::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketchSF::STATUS_NONE)
			return true;
	return false;
}



void CreateSketcherCommandsConstraints(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	rcCmdMgr.addCommand(new CmdSketcherConstrainHorizontal());
	rcCmdMgr.addCommand(new CmdSketcherConstrainVertical());
	rcCmdMgr.addCommand(new CmdSketcherConstrainLock());
	rcCmdMgr.addCommand(new CmdSketcherConstrainCoincident());
 }
