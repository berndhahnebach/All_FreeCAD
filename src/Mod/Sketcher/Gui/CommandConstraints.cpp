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

#include <Mod/Part/App/Geometry.h>
#include <Mod/Sketcher/App/SketchObject.h>

#include "ViewProviderSketch.h"

using namespace std;
using namespace SketcherGui;

bool isCreateConstraintActive(Gui::Document *doc)
{
    if (doc)
        // checks if a Sketch Viewprovider is in Edit and is in no special mode
        if (doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
            if (dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
                ->getSketchMode() == ViewProviderSketch::STATUS_NONE)
                if (Gui::Selection().countObjectsOfType(Sketcher::SketchObject::getClassTypeId()) > 0)
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
    sAccel          = "H";
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
            for (std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();
                 it != vals.end(); ++it) {
                if ((*it)->Type == Sketcher::Horizontal && (*it)->First == index){
                    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Double constraint"),
                        QObject::tr("The selected edge has already a horizontal constraint!"));
                    return;
                }
                if ((*it)->Type == Sketcher::Vertical && (*it)->First == index) {
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
    sAccel          = "V";
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
            for (std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();
                 it != vals.end(); ++it) {
                if ((*it)->Type == Sketcher::Horizontal && (*it)->First == index) {
                    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Impossible constraint"),
                        QObject::tr("The selected edge has already a horizontal constraint!"));
                    return;
                }
                if ((*it)->Type == Sketcher::Vertical && (*it)->First == index) {
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
    sMenuText       = QT_TR_NOOP("Constrain lock");
    sToolTipText    = QT_TR_NOOP("Create a lock constraint on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_ConstrainLock";
    eType           = ForEdit;
}

void CmdSketcherConstrainLock::activated(int iMsg)
{
    // get the selection 
    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

    // only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select entities from the sketch."));
        return;
    }

    // get the needed lists and objects
    const std::vector<std::string> &SubNames = selection[0].getSubNames();
    Sketcher::SketchObject* Obj = dynamic_cast<Sketcher::SketchObject*>(selection[0].getObject());

    if (SubNames.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly one entity from the sketch."));
        return;
    }

    int GeoId;
    Sketcher::PointPos PosId=Sketcher::none;
    if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex") {
        int VtId = std::atoi(SubNames[0].substr(6,4000).c_str());
        Obj->getGeoVertexIndex(VtId,GeoId,PosId);
    }
    else if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge")
        GeoId = std::atoi(SubNames[0].substr(4,4000).c_str());
    else {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly one entity from the sketch."));
        return;
    }

    Base::Vector3d pnt = Obj->getPoint(GeoId,PosId);

    // undo command open
    openCommand("add fixed constraint");
    Gui::Command::doCommand(
        Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceX',%i,%i,%f)) ",
        selection[0].getFeatName(),GeoId,PosId,pnt.x);
    Gui::Command::doCommand(
        Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceY',%i,%i,%f)) ",
        selection[0].getFeatName(),GeoId,PosId,pnt.y);

    // finish the transaction and update
    commitCommand();
    updateActive();

    // clear the selction (convenience)
    getSelection().clearSelection();
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
    sMenuText       = QT_TR_NOOP("Constrain coincident");
    sToolTipText    = QT_TR_NOOP("Create a coincident constraint on the selected item");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_PointOnPoint";
    sAccel          = "C";
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
    else {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two vertexes from the sketch."));
        return;
    }
        
    // get second vertex index
    if (SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex") 
        index2 = std::atoi(SubNames[1].substr(6,4000).c_str());
    else {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two vertexes from the sketch."));
        return;
    }
    int GeoId1,GeoId2;
    Sketcher::PointPos Pt1,Pt2;
    Obj->getGeoVertexIndex(index1,GeoId1,Pt1);
    Obj->getGeoVertexIndex(index2,GeoId2,Pt2);

    // undo command open
    openCommand("add coinsident constraint");
    Gui::Command::doCommand(
        Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,%i,%i,%i)) ",
        selection[0].getFeatName(),GeoId1,Pt1,GeoId2,Pt2);

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
    sMenuText       = QT_TR_NOOP("Constrain distance");
    sToolTipText    = QT_TR_NOOP("Fix a length of a line or the distance between a line and a vertex");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Length";
    sAccel          = "D";
    eType           = ForEdit;
}

void CmdSketcherConstrainDistance::activated(int iMsg)
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
    const std::vector<Part::Geometry *> &geo = Obj->Geometry.getValues();

    if (SubNames.size() < 1 || SubNames.size() > 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly one line or one point and one line or two points from the sketch."));
        return;
    }

    int GeoId1=-1, VtId1=-1, GeoId2=-1, VtId2=-1;
    if (SubNames.size() >= 1) {
        if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge") 
            GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
        else if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex") 
            VtId1 = std::atoi(SubNames[0].substr(6,4000).c_str());
    }
    if (SubNames.size() == 2) {
        if (SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge") 
            GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());
        else if (SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex") 
            VtId2 = std::atoi(SubNames[1].substr(6,4000).c_str());
    }

    if (VtId1 >= 0 && VtId2 >= 0) { // point to point distance
        Sketcher::PointPos PosId1,PosId2;
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        Obj->getGeoVertexIndex(VtId2,GeoId2,PosId2);
        Base::Vector3d pnt1 = Obj->getPoint(GeoId1,PosId1);
        Base::Vector3d pnt2 = Obj->getPoint(GeoId2,PosId2);
 
        openCommand("add Point to Line Distance constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Distance',%d,%d,%d,%d,%f)) ",
            selection[0].getFeatName(),GeoId1,PosId1,GeoId2,PosId2,(pnt2-pnt1).Length());
        commitCommand();
        //updateActive();
        getSelection().clearSelection();
        return;
    }
    else if ((VtId1 >= 0 && GeoId2 >= 0) || (VtId2 >= 0 && GeoId1 >= 0))  { // point to line distance
        if (VtId2 >= 0 && GeoId1 >= 0) {
            std::swap(VtId1,VtId2);
            std::swap(GeoId1,GeoId2);
        }
        Sketcher::PointPos PosId1;
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        Base::Vector3d pnt = Obj->getPoint(GeoId1,PosId1);
        const Part::Geometry *geom = geo[GeoId2];
        if (geom->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
            const Part::GeomLineSegment *lineSeg;
            lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geom);
            Base::Vector3d pnt1 = lineSeg->getStartPoint();
            Base::Vector3d pnt2 = lineSeg->getEndPoint();
            Base::Vector3d d = pnt2-pnt1;
            double ActDist = (-pnt.x*d.y+pnt.y*d.x+pnt1.x*pnt2.y-pnt2.x*pnt1.y)
                            / (2*d.Length());

            openCommand("add Point to Line Distance constraint");
            Gui::Command::doCommand(
                Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Distance',%d,%d,%d,%f)) ",
                selection[0].getFeatName(),GeoId1,PosId1,GeoId2,ActDist);
            commitCommand();
            //updateActive();
            getSelection().clearSelection();
            return;
        }
    }
    else if (GeoId1 >= 0) { // line length
        const Part::Geometry *geom = geo[GeoId1];
        if (geom->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
            const Part::GeomLineSegment *lineSeg;
            lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geom);
            double ActLength = (lineSeg->getEndPoint()-lineSeg->getStartPoint()).Length();

            openCommand("add Length constraint");
            Gui::Command::doCommand(
                Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Distance',%d,%f)) ",
                selection[0].getFeatName(),GeoId1,ActLength);
            commitCommand();
            //updateActive();
            getSelection().clearSelection();
            return;
        }
    }

    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
        QObject::tr("Select exactly one line or one point and one line or two points from the sketch."));
    return;
}

bool CmdSketcherConstrainDistance::isActive(void)
{
    return isCreateConstraintActive( getActiveGuiDocument() );
}


DEF_STD_CMD_A(CmdSketcherConstrainDistanceX);

CmdSketcherConstrainDistanceX::CmdSketcherConstrainDistanceX()
    :Command("Sketcher_ConstrainDistanceX")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain horizontal distance");
    sToolTipText    = QT_TR_NOOP("Fix the horizontal distance between two points or line ends");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_HorizontalDistance";
    sAccel          = "D";
    eType           = ForEdit;
}

void CmdSketcherConstrainDistanceX::activated(int iMsg)
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
    const std::vector<Part::Geometry *> &geo = Obj->Geometry.getValues();

    if (SubNames.size() < 1 || SubNames.size() > 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly one line or up to two points from the sketch."));
        return;
    }

    int GeoId1=-1, VtId1=-1, GeoId2=-1, VtId2=-1;
    if (SubNames.size() >= 1) {
        if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge") 
            GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
        else if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex") 
            VtId1 = std::atoi(SubNames[0].substr(6,4000).c_str());
    }
    if (SubNames.size() == 2) {
        if (SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge") 
            GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());
        else if (SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex") 
            VtId2 = std::atoi(SubNames[1].substr(6,4000).c_str());
    }

    if (VtId1 >= 0 && VtId2 >= 0) { // point to point horizontal distance
        Sketcher::PointPos PosId1,PosId2;
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        Obj->getGeoVertexIndex(VtId2,GeoId2,PosId2);
        Base::Vector3d pnt1 = Obj->getPoint(GeoId1,PosId1);
        Base::Vector3d pnt2 = Obj->getPoint(GeoId2,PosId2);
        double ActLength = pnt2.x-pnt1.x;
 
        openCommand("add point to point horizontal distance constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceX',%d,%d,%d,%d,%f)) ",
            selection[0].getFeatName(),GeoId1,PosId1,GeoId2,PosId2,ActLength);
        commitCommand();
        //updateActive();
        getSelection().clearSelection();
        return;
    }
    else if (GeoId1 >= 0 && GeoId2 < 0 && VtId2 < 0)  { // horizontal length of a line
        const Part::Geometry *geom = geo[GeoId1];
        if (geom->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
            const Part::GeomLineSegment *lineSeg;
            lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geom);
            double ActLength = lineSeg->getEndPoint().x-lineSeg->getStartPoint().x;

            openCommand("add horizontal length constraint");
            Gui::Command::doCommand(
                Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceX',%d,%f)) ",
                selection[0].getFeatName(),GeoId1,ActLength);
            commitCommand();
            //updateActive();
            getSelection().clearSelection();
            return;
        }
    }
    else if (VtId1 >= 0) { // point on fixed x-coordinate
        Sketcher::PointPos PosId1;
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        Base::Vector3d pnt = Obj->getPoint(GeoId1,PosId1);
        double ActX = pnt.x;

        openCommand("add fixed x-coordinate constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceX',%d,%d,%f)) ",
            selection[0].getFeatName(),GeoId1,PosId1,ActX);
        commitCommand();
        //updateActive();
        getSelection().clearSelection();
        return;
    }

    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
        QObject::tr("Select exactly one line or up to two points from the sketch."));
    return;
}

bool CmdSketcherConstrainDistanceX::isActive(void)
{
    return isCreateConstraintActive( getActiveGuiDocument() );
}


DEF_STD_CMD_A(CmdSketcherConstrainDistanceY);

CmdSketcherConstrainDistanceY::CmdSketcherConstrainDistanceY()
    :Command("Sketcher_ConstrainDistanceY")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain horizontal distance");
    sToolTipText    = QT_TR_NOOP("Fix the vertical distance between two points or line ends");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_VerticalDistance";
    sAccel          = "D";
    eType           = ForEdit;
}

void CmdSketcherConstrainDistanceY::activated(int iMsg)
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
    const std::vector<Part::Geometry *> &geo = Obj->Geometry.getValues();

    if (SubNames.size() < 1 || SubNames.size() > 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly one line or up to two points from the sketch."));
        return;
    }

    int GeoId1=-1, VtId1=-1, GeoId2=-1, VtId2=-1;
    if (SubNames.size() >= 1) {
        if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge") 
            GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
        else if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex") 
            VtId1 = std::atoi(SubNames[0].substr(6,4000).c_str());
    }
    if (SubNames.size() == 2) {
        if (SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge") 
            GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());
        else if (SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex") 
            VtId2 = std::atoi(SubNames[1].substr(6,4000).c_str());
    }

    if (VtId1 >= 0 && VtId2 >= 0) { // point to point horizontal distance
        Sketcher::PointPos PosId1,PosId2;
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        Obj->getGeoVertexIndex(VtId2,GeoId2,PosId2);
        Base::Vector3d pnt1 = Obj->getPoint(GeoId1,PosId1);
        Base::Vector3d pnt2 = Obj->getPoint(GeoId2,PosId2);
        double ActLength = pnt2.y-pnt1.y;
 
        openCommand("add point to point vertical distance constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceY',%d,%d,%d,%d,%f)) ",
            selection[0].getFeatName(),GeoId1,PosId1,GeoId2,PosId2,ActLength);
        commitCommand();
        //updateActive();
        getSelection().clearSelection();
        return;
    }
    else if (GeoId1 >= 0 && GeoId2 < 0 && VtId2 < 0)  { // horizontal length of a line
        const Part::Geometry *geom = geo[GeoId1];
        if (geom->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
            const Part::GeomLineSegment *lineSeg;
            lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geom);
            double ActLength = lineSeg->getEndPoint().y-lineSeg->getStartPoint().y;

            openCommand("add vertical length constraint");
            Gui::Command::doCommand(
                Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceY',%d,%f)) ",
                selection[0].getFeatName(),GeoId1,ActLength);
            commitCommand();
            //updateActive();
            getSelection().clearSelection();
            return;
        }
    }
    else if (VtId1 >= 0) { // point on fixed y-coordinate
        Sketcher::PointPos PosId1;
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        Base::Vector3d pnt = Obj->getPoint(GeoId1,PosId1);
        double ActY = pnt.y;

        openCommand("add fixed y-coordinate constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('DistanceY',%d,%d,%f)) ",
            selection[0].getFeatName(),GeoId1,PosId1,ActY);
        commitCommand();
        //updateActive();
        getSelection().clearSelection();
        return;
    }

    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
        QObject::tr("Select exactly one line or up to two points from the sketch."));
    return;
}

bool CmdSketcherConstrainDistanceY::isActive(void)
{
    return isCreateConstraintActive( getActiveGuiDocument() );
}


DEF_STD_CMD_A(CmdSketcherConstrainParallel);

CmdSketcherConstrainParallel::CmdSketcherConstrainParallel()
    :Command("Sketcher_ConstrainParallel")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain parallel");
    sToolTipText    = QT_TR_NOOP("Create a parallel constraint between two lines");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Parallel";
    sAccel          = "P";
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

    // only one sketch with its subelements are allowed to be selected
    if (SubNames.size() != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    int GeoId1,GeoId2;
    if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge") 
        GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
    else {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    if (SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge") 
        GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());
    else {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    // undo command open
    openCommand("add parallel constraint");
    Gui::Command::doCommand(
        Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Parallel',%i,%i)) ",
        selection[0].getFeatName(),GeoId1,GeoId2);

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


DEF_STD_CMD_A(CmdSketcherConstrainPerpendicular);

CmdSketcherConstrainPerpendicular::CmdSketcherConstrainPerpendicular()
    :Command("Sketcher_ConstrainPerpendicular")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain perpendicular");
    sToolTipText    = QT_TR_NOOP("Create a Perpendicular constraint between two lines");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Perpendicular";
    sAccel          = "N";
    eType           = ForEdit;
}

void CmdSketcherConstrainPerpendicular::activated(int iMsg)
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

    // only one sketch with its subelements are allowed to be selected
    if (SubNames.size() != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    int GeoId1,GeoId2;
    if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge") 
        GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
    else {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    if (SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge") 
        GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());
    else {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two lines from the sketch."));
        return;
    }

    // undo command open
    openCommand("add perpendicular constraint");
    Gui::Command::doCommand(
        Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Perpendicular',%i,%i)) ",
        selection[0].getFeatName(),GeoId1,GeoId2);

    // finish the transaction and update
    commitCommand();
    updateActive();

    // clear the selction (convenience)
    getSelection().clearSelection();
}

bool CmdSketcherConstrainPerpendicular::isActive(void)
{
    return isCreateConstraintActive( getActiveGuiDocument() );
}

DEF_STD_CMD_A(CmdSketcherConstrainTangent);

CmdSketcherConstrainTangent::CmdSketcherConstrainTangent()
    :Command("Sketcher_ConstrainTangent")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Constrain tangent");
    sToolTipText    = QT_TR_NOOP("Create a tangent constraint between two entities");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Constraint_Tangent";
    sAccel          = "T";
    eType           = ForEdit;
}

void CmdSketcherConstrainTangent::activated(int iMsg)
{
    // get the selection 
    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

    // only one sketch with its subelements are allowed to be selected
    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select two entities from the sketch."));
        return;
    }

    // get the needed lists and objects
    const std::vector<std::string> &SubNames = selection[0].getSubNames();
    Sketcher::SketchObject* Obj = dynamic_cast<Sketcher::SketchObject*>(selection[0].getObject());

    // only one sketch with its subelements are allowed to be selected
    if (SubNames.size() != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select exactly two entities from the sketch."));
        return;
    }

    int GeoId1=-1, VtId1=-1, GeoId2=-1, VtId2=-1;
    if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge")
        GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
    else if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex")
        VtId1 = std::atoi(SubNames[0].substr(6,4000).c_str());

    if (SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge")
        GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());
    else if (SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex")
        VtId2 = std::atoi(SubNames[1].substr(6,4000).c_str());

    Sketcher::PointPos PosId1,PosId2;
    if (VtId1 >= 0 && VtId2 >= 0) { // tangency at common point
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        Obj->getGeoVertexIndex(VtId2,GeoId2,PosId2);
        openCommand("add tangent constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Tangent',%i,%i,%i,%i)) ",
            selection[0].getFeatName(),GeoId1,PosId1,GeoId2,PosId2);
        commitCommand();
        updateActive();
        getSelection().clearSelection();
        return;
    }
    else if ((VtId1 >= 0 && GeoId2 >= 0) || (VtId2 >= 0 && GeoId1 >= 0)) { // VtId1 is a tangency point
        if (VtId2 >= 0 && GeoId1 >= 0) {
            VtId1 = VtId2;
            VtId2 = -1;
            GeoId2 = GeoId1;
            GeoId1 = -1;
        }
        Obj->getGeoVertexIndex(VtId1,GeoId1,PosId1);
        openCommand("add tangent constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Tangent',%i,%i,%i)) ",
            selection[0].getFeatName(),GeoId1,PosId1,GeoId2);
        commitCommand();
        updateActive();
        getSelection().clearSelection();
        return;
    }
    else if (GeoId1 >= 0 && GeoId2 >= 0) { // simple tangency between GeoId1 and GeoId2
        openCommand("add tangent constraint");
        Gui::Command::doCommand(
            Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Tangent',%i,%i)) ",
            selection[0].getFeatName(),GeoId1,GeoId2);
        commitCommand();
        updateActive();
        getSelection().clearSelection();
        return;
    }
    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
        QObject::tr("Select exactly two entities from the sketch."));
    return;
}

bool CmdSketcherConstrainTangent::isActive(void)
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
    rcCmdMgr.addCommand(new CmdSketcherConstrainPerpendicular());
    rcCmdMgr.addCommand(new CmdSketcherConstrainTangent());
    rcCmdMgr.addCommand(new CmdSketcherConstrainDistance());
    rcCmdMgr.addCommand(new CmdSketcherConstrainDistanceX());
    rcCmdMgr.addCommand(new CmdSketcherConstrainDistanceY());
 }
