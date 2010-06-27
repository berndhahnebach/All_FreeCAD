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
# include <QMessageBox>
#endif

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Selection.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>

#include <Mod/Sketcher/App/SketchObject.h>

#include "ViewProviderSketch.h"

using namespace std;
using namespace SketcherGui;

bool isCreateConstraintActive(Gui::Document *doc)
{
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
                if( Gui::Selection().countObjectsOfType(Sketcher::SketchObject::getClassTypeId()) > 0)
			return true;
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
   //unsigned int n = getSelection().countObjectsOfType(Part::Feature::getClassTypeId());

	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edges from the sketch."));
        return;
    }

	const std::vector<std::string> &SubNames = selection[0].getSubNames();

    for(std::vector<std::string>::const_iterator it=SubNames.begin();it!=SubNames.end();++it){
        
    }

      
}

bool CmdSketcherConstrainHorizontal::isActive(void)
{
	return isCreateConstraintActive( getActiveGuiDocument() );
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
      
}

bool CmdSketcherConstrainVertical::isActive(void)
{
	return isCreateConstraintActive( getActiveGuiDocument() );
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
      
}

bool CmdSketcherConstrainLock::isActive(void)
{
	return isCreateConstraintActive( getActiveGuiDocument() );
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
      
}

bool CmdSketcherConstrainCoincident::isActive(void)
{
	return isCreateConstraintActive( getActiveGuiDocument() );
}



void CreateSketcherCommandsConstraints(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	rcCmdMgr.addCommand(new CmdSketcherConstrainHorizontal());
	rcCmdMgr.addCommand(new CmdSketcherConstrainVertical());
	rcCmdMgr.addCommand(new CmdSketcherConstrainLock());
	rcCmdMgr.addCommand(new CmdSketcherConstrainCoincident());
 }
