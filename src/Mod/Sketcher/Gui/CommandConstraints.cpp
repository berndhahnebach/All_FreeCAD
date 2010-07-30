/***************************************************************************
 *   Copyright (c) 2010 J�rgen Riegel (juergen.riegel@web.de)              *
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
	// get the selection 
	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

	// only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edges from the sketch."));
        return;
    }

	// get the needed lists and objects
	const std::vector<std::string> &SubNames = selection[0].getSubNames();
	Sketcher::SketchObject* Obj = dynamic_cast<Sketcher::SketchObject*>(selection[0].getObject());
	const std::vector< Sketcher::Constraint * > &vals = Obj->Constraints.getValues();

	// undo command open
    openCommand("add horizontal constraint");

	// go through the selected subelements
    for(std::vector<std::string>::const_iterator it=SubNames.begin();it!=SubNames.end();++it){
		// only handle edges
		if (it->size() > 4 && it->substr(0,4) == "Edge") {
			int index=std::atoi(&(*it)[4]);
			// check if the edge has already a Horizontal or Vertical constraint
			for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it){
				if((*it)->Type == Sketcher::Horizontal && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Double constraint"),
						QObject::tr("The selected edge has already a Horizontal constraint!"));
					return;
				}
				if((*it)->Type == Sketcher::Vertical && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Imposible constraint"),
						QObject::tr("The selected edge has already a Vertical constraint!"));
					return;
				}
			}
			// isue the actual commands to creat the Constraint
			doCommand(Doc,"App.ActiveDocument.ActiveObject.addConstraint(Sketcher.Constraint('Horizontal',%i)) ",index);
		}
    }
	// finish the transaction and update
    commitCommand();
    updateActive();

	// clear the selction (conviniance)
	getSelection().clearSelection();
	
      
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
	// get the selection 
	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

	// only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edges from the sketch."));
        return;
    }

	// get the needed lists and objects
	const std::vector<std::string> &SubNames = selection[0].getSubNames();
	Sketcher::SketchObject* Obj = dynamic_cast<Sketcher::SketchObject*>(selection[0].getObject());
	const std::vector< Sketcher::Constraint * > &vals = Obj->Constraints.getValues();

	// undo command open
    openCommand("add vertical constraint");

	// go through the selected subelements
    for(std::vector<std::string>::const_iterator it=SubNames.begin();it!=SubNames.end();++it){
		// only handle edges
		if (it->size() > 4 && it->substr(0,4) == "Edge") {
			int index=std::atoi(&(*it)[4]);
			// check if the edge has already a Horizontal or Vertical constraint
			for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it){
				if((*it)->Type == Sketcher::Horizontal && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Imposible constraint"),
						QObject::tr("The selected edge has already a Horizontal constraint!"));
					return;
				}
				if((*it)->Type == Sketcher::Vertical && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Double constraint"),
						QObject::tr("The selected edge has already a Vertical constraint!"));
					return;
				}
			}
			// isue the actual commands to creat the Constraint
			doCommand(Doc,"App.ActiveDocument.ActiveObject.addConstraint(Sketcher.Constraint('Vertical',%i)) ",index);
		}
    }
	// finish the transaction and update
    commitCommand();
    updateActive();

	// clear the selction (conviniance)
	getSelection().clearSelection();
	
      
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
