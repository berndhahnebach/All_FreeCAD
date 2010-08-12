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
    sMenuText       = QT_TR_NOOP("Constrain horizontally");
    sToolTipText    = QT_TR_NOOP("Create a horizontal constraint on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Horizontal";
    iAccel          = Qt::Key_H;
    eType           = ForEdit;
}


void CmdSketcherConstrainHorizontal::activated(int iMsg)
{
	// get the selection 
	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

	// only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edge from the sketch."));
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
			int index=std::atoi(it->substr(4,4000).c_str());
			// check if the edge has already a Horizontal or Vertical constraint
			for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it){
				if((*it)->Type == Sketcher::Horizontal && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Double constraint"),
						QObject::tr("The selected edge has already a horizontal constraint!"));
					return;
				}
				if((*it)->Type == Sketcher::Vertical && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Impossible constraint"),
						QObject::tr("The selected edge has already a vertical constraint!"));
					return;
				}
			}
			// issue the actual commands to create the constraint
			doCommand(Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Horizontal',%i)) "
                         ,selection[0].getFeatName(),index);
		}
    }
	// finish the transaction and update
    commitCommand();
    updateActive();

	// clear the selction (convenience)
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
    sMenuText       = QT_TR_NOOP("Constrain vertically");
    sToolTipText    = QT_TR_NOOP("Create a vertical constraint on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Vertical";
    iAccel          = Qt::Key_V;
    eType           = ForEdit;

}


void CmdSketcherConstrainVertical::activated(int iMsg)
{
	// get the selection 
	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

	// only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edge from the sketch."));
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
			int index=std::atoi(it->substr(4,4000).c_str());
			// check if the edge has already a Horizontal or Vertical constraint
			for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it){
				if((*it)->Type == Sketcher::Horizontal && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Impossible constraint"),
						QObject::tr("The selected edge has already a horizontal constraint!"));
					return;
				}
				if((*it)->Type == Sketcher::Vertical && (*it)->First == index){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Double constraint"),
						QObject::tr("The selected edge has already a vertical constraint!"));
					return;
				}
			}
			// issue the actual command to create the constraint
			doCommand(Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Vertical',%d))"
                         ,selection[0].getFeatName(),index);
		}
    }
	// finish the transaction and update
    commitCommand();
    updateActive();

	// clear the selection (convenience)
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
    eType           = ForEdit;
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
    sPixmap         = "Constraint_PointOnPoint";
    iAccel          = Qt::Key_C;
    eType           = ForEdit;

}


void CmdSketcherConstrainCoincident::activated(int iMsg)
{
	// get the selection 
	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

	// only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select vertexes from the sketch."));
        return;
    }

	// get the needed lists and objects
	const std::vector<std::string> &SubNames = selection[0].getSubNames();
	Sketcher::SketchObject* Obj = dynamic_cast<Sketcher::SketchObject*>(selection[0].getObject());
	const std::vector< Sketcher::Constraint * > &vals = Obj->Constraints.getValues();

	// only one sketch with its subelements are allowed to be selected
    if (SubNames.size() != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two vertexes from the sketch."));
        return;
    }

    int index1,index2;
    // get first vertex index
	if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex") 
        index1 = std::atoi(SubNames[0].substr(6,4000).c_str());
    else{
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two vertexes from the sketch."));
        return;
    }
        
    // get second vertex index
	if (SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex") 
        index2 = std::atoi(SubNames[1].substr(6,4000).c_str());
    else{
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two vertexes from the sketch."));
        return;
    }
    int GeoId1,GeoId2,Pt1,Pt2;
    Obj->getGeoVertexIndex(index1,GeoId1,Pt1);
    Obj->getGeoVertexIndex(index2,GeoId2,Pt2);

    // undo command open
    openCommand("add coinsident constraint");
    Gui::Command::doCommand(Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,%i,%i,%i)) "
                 ,selection[0].getFeatName(),GeoId1,Pt1,GeoId2,Pt2);

    // finish the transaction and update
    commitCommand();
    updateActive();

    // clear the selction (convenience)
    getSelection().clearSelection();
}

bool CmdSketcherConstrainCoincident::isActive(void)
{
	return isCreateConstraintActive( getActiveGuiDocument() );
}

DEF_STD_CMD_A(CmdSketcherConstrainDistance);

CmdSketcherConstrainDistance::CmdSketcherConstrainDistance()
	:Command("Sketcher_ConstrainDistance")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain Distance");
    sToolTipText    = QT_TR_NOOP("fix a length of a line or the distance between a line and a vertex");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Length";
    iAccel          = Qt::Key_C;
    eType           = ForEdit;

}


void CmdSketcherConstrainDistance::activated(int iMsg)
{
	//// get the selection 
	//std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

	//// only one sketch with its subelements are allowed to be selected
 //   if (selection.size() != 1) {
 //       QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
 //           QObject::tr("Select vertexes from the sketch."));
 //       return;
 //   }

	//// get the needed lists and objects
	//const std::vector<std::string> &SubNames = selection[0].getSubNames();
	//Sketcher::SketchObject* Obj = dynamic_cast<Sketcher::SketchObject*>(selection[0].getObject());
	//const std::vector< Sketcher::Constraint * > &vals = Obj->Constraints.getValues();

	//// only one sketch with its subelements are allowed to be selected
 //   if (SubNames.size() != 2) {
 //       QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
 //           QObject::tr("Select exactly two vertexes from the sketch."));
 //       return;
 //   }

 //   int index1,index2;
 //   // get first vertex index
	//if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex") 
 //       index1 = std::atoi(SubNames[0].substr(6,4000).c_str());
 //   else{
 //       QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
 //           QObject::tr("Select exactly two vertexes from the sketch."));
 //       return;
 //   }
 //       
 //   // get second vertex index
	//if (SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex") 
 //       index2 = std::atoi(SubNames[1].substr(6,4000).c_str());
 //   else{
 //       QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
 //           QObject::tr("Select exactly two vertexes from the sketch."));
 //       return;
 //   }
 //   int GeoId1,GeoId2,Pt1,Pt2;
 //   Obj->getGeoVertexIndex(index1,GeoId1,Pt1);
 //   Obj->getGeoVertexIndex(index2,GeoId2,Pt2);

 //   // undo command open
 //   openCommand("add coinsident constraint");
 //   Gui::Command::doCommand(Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,%i,%i,%i)) "
 //                ,selection[0].getFeatName(),GeoId1,Pt1,GeoId2,Pt2);

 //   // finish the transaction and update
 //   commitCommand();
 //   updateActive();

 //   // clear the selction (convenience)
 //   getSelection().clearSelection();
}

bool CmdSketcherConstrainDistance::isActive(void)
{
	return isCreateConstraintActive( getActiveGuiDocument() );
}


DEF_STD_CMD_A(CmdSketcherConstrainParallel);

CmdSketcherConstrainParallel::CmdSketcherConstrainParallel()
	:Command("Sketcher_ConstrainParallel")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain Parallel");
    sToolTipText    = QT_TR_NOOP("create a parallel constraint between two lines");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Parallel";
    iAccel          = Qt::Key_P;
    eType           = ForEdit;

}


void CmdSketcherConstrainParallel::activated(int iMsg)
{
	// get the selection 
	std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

	// only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select two lines from the sketch."));
        return;
    }

	// get the needed lists and objects
	const std::vector<std::string> &SubNames = selection[0].getSubNames();
	Sketcher::SketchObject* Obj = dynamic_cast<Sketcher::SketchObject*>(selection[0].getObject());
	const std::vector< Sketcher::Constraint * > &vals = Obj->Constraints.getValues();

	// only one sketch with its subelements are allowed to be selected
    if (SubNames.size() != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    int GeoId1,GeoId2;
    // get first vertex index
	if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge") 
        GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
    else{
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }
        
    // get second vertex index
	if (SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge") 
        GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());
    else{
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    // undo command open
    openCommand("add parallel constraint");
    Gui::Command::doCommand(Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Parallel',%i,%i)) "
                 ,selection[0].getFeatName(),GeoId1,GeoId2);

    // finish the transaction and update
    commitCommand();
    updateActive();

    // clear the selction (convenience)
    getSelection().clearSelection();
}

bool CmdSketcherConstrainParallel::isActive(void)
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
	rcCmdMgr.addCommand(new CmdSketcherConstrainParallel());
	rcCmdMgr.addCommand(new CmdSketcherConstrainDistance());
 }
