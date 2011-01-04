/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <Standard_math.hxx>
# include <Inventor/SoPath.h>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/details/SoLineDetail.h>
# include <Inventor/details/SoPointDetail.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoPointSet.h>
# include <Inventor/nodes/SoMarkerSet.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoVertexProperty.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoText2.h>
# include <Inventor/nodes/SoFont.h>
# include <QAction>
# include <QMenu>
# include <QMessageBox>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Base/Vector3D.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>

#include <Mod/Part/App/Geometry.h>
#include <Mod/Sketcher/App/SketchFlatInterface.h>
#include <Mod/Sketcher/App/SketchObject.h>
#include <Mod/Sketcher/App/Sketch.h>


#include "ViewProviderSketch.h"
#include "DrawSketchHandler.h"
#include "TaskDlgEditSketch.h"


using namespace SketcherGui;
using namespace Sketcher;

SbColor sCurveColor             (1.0f,1.0f,1.0f); 
SbColor sCurveConstructionColor (0.2f,1.0f,0.2f); 
SbColor sPointColor             (0.5f,0.5f,0.5f); 
SbColor sConstraintColor        (0.0f,0.8f,0.0f); 

SbColor ViewProviderSketch::PreselectColor(0.1f, 0.1f, 0.8f); 
SbColor ViewProviderSketch::SelectColor   (0.1f, 0.1f, 0.8f); 

//**************************************************************************
// Edit data structure

/// Data structure while edit the sketch 
struct EditData {
    EditData():
    sketchHandler(0),
    DragPoint(-1),
    PreselectCurve(-1),
    PreselectPoint(-1),
    //ActSketch(0),
    EditRoot(0),
    PointsMaterials(0),
    CurvesMaterials(0),
    LinesMaterials(0),
    PointsCoordinate(0),
    CurvesCoordinate(0),
    LinesCoordinate(0),
    CurveSet(0),
    LineSet(0),
    PointSet(0)
    {}

    // pointer to the active handler for new sketch objects
    DrawSketchHandler *sketchHandler;

    // dragged point
    int DragPoint;

    int PreselectCurve;
    SbColor PreselectOldColor;
    int PreselectPoint;
    int PreselectConstraint;
    // pointer to the Solver
    Sketcher::Sketch ActSketch;
    // container to track our own selected parts
    std::set<int> SelPointSet;
    std::set<int> SelCurvSet;
    std::set<int> SelConstraintSet;

    // helper data structure for the constraint rendering
    std::vector<ConstraintType> vConstrType;

    // nodes for the visuals 
    SoSeparator   *EditRoot;
    SoMaterial    *PointsMaterials;
    SoMaterial    *CurvesMaterials;
    SoMaterial    *EditCurvesMaterials;
    SoMaterial    *LinesMaterials;
    SoCoordinate3 *PointsCoordinate;
    SoCoordinate3 *CurvesCoordinate;
    SoCoordinate3 *EditCurvesCoordinate;
    SoCoordinate3 *LinesCoordinate;
    SoLineSet     *CurveSet;
    SoLineSet     *LineSet;
    SoLineSet     *EditCurveSet;
    SoMarkerSet   *PointSet;

    SoText2       * textX;
    SoTranslation * textPos;

    SoGroup       * constrGroup;
};


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProvider2DObject)


ViewProviderSketch::ViewProviderSketch()
  : edit(0),
    Mode(STATUS_NONE)
{
    sPixmap = "Sketcher_NewSketch";
    LineColor.setValue(1,1,1);
    PointColor.setValue(1,1,1);
    PointSize.setValue(4);
}

ViewProviderSketch::~ViewProviderSketch()
{
}

// handler management ***************************************************************
void ViewProviderSketch::activateHandler(DrawSketchHandler *newHandler)
{
    assert(edit);

    assert(edit->sketchHandler == 0);
    edit->sketchHandler = newHandler;
    Mode = STATUS_SKETCH_UseHandler;
    edit->sketchHandler->sketchgui = this;
    edit->sketchHandler->activated(this);
}

/// removes the active handler
void ViewProviderSketch::purgeHandler(void)
{
    assert(edit);
    assert(edit->sketchHandler != 0);
    delete(edit->sketchHandler);  
    edit->sketchHandler = 0;
    Mode = STATUS_NONE;
}

// **********************************************************************************

bool ViewProviderSketch::keyPressed(int key)
{
    return true;
}

void ViewProviderSketch::getCoordsOnSketchPlane(double &u, double &v,const SbVec3f &point, const SbVec3f &normal)
{
    // Plane form
    Base::Vector3d R0(0,0,0),RN(0,0,1),RX(1,0,0),RY(0,1,0);

    // move to position of Sketch
    Base::Placement Plz = getSketchObject()->Placement.getValue();
    R0 = Plz.getPosition() ; 
    Base::Rotation tmp(Plz.getRotation());
    tmp.multVec(RN,RN);
    tmp.multVec(RX,RX);
    tmp.multVec(RY,RY);
    Plz.setRotation(tmp);

    // line 
    Base::Vector3d R1(point[0],point[1],point[2]),RA(normal[0],normal[1],normal[2]);
    // intersection point on plane
    Base::Vector3d S = R1 + ((RN * (R0-R1))/(RN*RA))*RA;

    // distance to x Axle of the sketch
    S.TransformToCoordinateSystem(R0,RX,RY);

    u = S.x;
    v = S.y;
}

bool ViewProviderSketch::mouseButtonPressed(int Button, bool pressed, const SbVec3f &point,
                                            const SbVec3f &normal, const SoPickedPoint* pp)
{
    assert(edit);

    double x,y;
    SbVec3f pos = point;
    if (pp) {
        const SoDetail* detail = pp->getDetail();
        if (detail && detail->getTypeId() == SoPointDetail::getClassTypeId()) {
            pos = pp->getPoint();
        }
    }

    getCoordsOnSketchPlane(x,y,pos,normal);

    // Left Mouse button ****************************************************
    if (Button == 1) {
        if (pressed) {
            // Do things depending on the mode of the user interaction
            switch(Mode){
                case STATUS_NONE:
                    if (edit->PreselectPoint >=0) {
                        //Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
                        Mode = STATUS_SELECT_Point;
                        return true;
                    } else if (edit->PreselectCurve >=0) {
                        //Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
                        Mode = STATUS_SELECT_Edge;
                        return true;
                    } else if (edit->PreselectConstraint >=0) {
                        //Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
                        Mode = STATUS_SELECT_Constraint;
                        return true;
                    } else
                        return false;

                case STATUS_SKETCH_UseHandler:
                    return edit->sketchHandler->pressButton(Base::Vector2D(x,y));
                default:
                    return false;
            }
        }
        else {
            // Do things depending on the mode of the user interaction
            switch (Mode) {
                
                case STATUS_SELECT_Point:{
                    //Base::Console().Log("Select Point:%d\n",this->DragPoint);
                    // Do selection
                    std::stringstream ss;
                    ss << "Vertex" << edit->PreselectPoint;
                    Gui::Selection().addSelection(getSketchObject()->getDocument()->getName()
                                                 ,getSketchObject()->getNameInDocument()
                                                 , ss.str().c_str()
                                                 ,pp->getPoint()[0]
                                                 ,pp->getPoint()[1]
                                                 ,pp->getPoint()[2]);

                    edit->DragPoint = -1;
                    Mode = STATUS_NONE;
                    return true;
                                         }
                case STATUS_SELECT_Edge:{
                    //Base::Console().Log("Select Point:%d\n",this->DragPoint);
                    // Do selection
                    std::stringstream ss;
                    ss << "Edge" << edit->PreselectCurve;
                                        Gui::Selection().addSelection(getSketchObject()->getDocument()->getName()
                                                 ,getSketchObject()->getNameInDocument()
                                                 ,ss.str().c_str()
                                                 ,pp->getPoint()[0]
                                                 ,pp->getPoint()[1]
                                                 ,pp->getPoint()[2]);

                    this->edit->DragPoint = -1;
                    Mode = STATUS_NONE;
                    return true;}
                case STATUS_SELECT_Constraint:{
                     // Do selection
                    std::stringstream ss;
                    ss << "Constraint" << edit->PreselectConstraint;
                                        Gui::Selection().addSelection(getSketchObject()->getDocument()->getName()
                                                 ,getSketchObject()->getNameInDocument()
                                                 ,ss.str().c_str()
                                                 ,pp->getPoint()[0]
                                                 ,pp->getPoint()[1]
                                                 ,pp->getPoint()[2]);

                    this->edit->DragPoint = -1;
                    Mode = STATUS_NONE;
                    return true;}
                case STATUS_SKETCH_DragPoint:
                    if(edit->DragPoint != -1 && pp){
                          Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.movePoint(%i,%i,App.Vector(%f,%f,0)) "
                          ,getObject()->getNameInDocument()
                          ,edit->DragPoint/2,edit->DragPoint%2+1
                          ,pp->getPoint()[0],pp->getPoint()[1]
                          );

                        edit->PreselectPoint = edit->DragPoint;
                        edit->DragPoint = -1;
                        //updateColor();
                    }
                    resetPositionText();
                    Mode = STATUS_NONE;
                    return true;
                case STATUS_SKETCH_UseHandler:
                    return edit->sketchHandler->releaseButton(Base::Vector2D(x,y));
                
                case STATUS_NONE:
                default:
                    return false;
            }
        }
    }
    // Right mouse button ****************************************************
    else if (Button == 2) {
        if (pressed) {
            switch(Mode){
                case STATUS_SKETCH_UseHandler:
                    // make the handler quit
                    edit->sketchHandler->quit();
                    return true;
                case STATUS_NONE:
                case STATUS_SELECT_Point:
                case STATUS_SELECT_Edge:
                case STATUS_SELECT_Constraint:
                case STATUS_SKETCH_DragPoint:
                    break;
            }
        }
    }

    return false;
}

bool ViewProviderSketch::mouseMove(const SbVec3f &point, const SbVec3f &normal, const SoPickedPoint* pp)
{
    if(!edit)
        return false;
    assert(edit);

    double x,y;
    getCoordsOnSketchPlane(x,y,point,normal);

    int PtIndex,CurvIndex,ConstrIndex;
    bool preselectChanged = detectPreselection(pp,PtIndex,CurvIndex,ConstrIndex);

    switch (Mode) {
        case STATUS_NONE:
            if(preselectChanged)
                updateColor();
            return false;
        case STATUS_SELECT_Point:
            Mode = STATUS_SKETCH_DragPoint;
            edit->DragPoint = edit->PreselectPoint;
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;
            edit->PreselectConstraint = -1;

            return true;
        case STATUS_SELECT_Edge:
        case STATUS_SELECT_Constraint:
            // drag a edge not implemented yet!
            Mode = STATUS_NONE;
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;
            edit->PreselectConstraint = -1;

            return true;
        case STATUS_SKETCH_DragPoint:
            if(edit->DragPoint != -1){
                //Base::Console().Log("Drag Point:%d\n",edit->DragPoint);
                int ret;
                if ((ret=edit->ActSketch.movePoint(edit->DragPoint/2,edit->DragPoint%2==0?start:end,Base::Vector3d(x,y,0))) == 0){
                    setPositionText(Base::Vector2D(x,y));
                    draw(true);
                    signalSolved(0,edit->ActSketch.SolveTime);
                }
                else{
                    signalSolved(1,edit->ActSketch.SolveTime);
                    //Base::Console().Log("Error solving:%d\n",ret);
                }
            }
            return true;
        case STATUS_SKETCH_UseHandler:
            if(preselectChanged)
                updateColor();
            edit->sketchHandler->mouseMove(Base::Vector2D(x,y));
            return true;
        default:
            return false;
    }

    return false;
}

void ViewProviderSketch::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    // are we in edit?
    if(edit){
        std::string temp;
        if (msg.Type == Gui::SelectionChanges::ClrSelection) {
            // if something selected in this object?
            if ( edit->SelPointSet.size() > 0 || edit->SelCurvSet.size() > 0 || edit->SelConstraintSet.size() > 0){
                // clear our selection and update the color of the viewed edges and points
                edit->SelPointSet.clear();
                edit->SelCurvSet.clear();
                edit->SelConstraintSet.clear();
                updateColor();
            }
        }
        else if (msg.Type == Gui::SelectionChanges::AddSelection) {
            // is it this object??
            if(strcmp(msg.pDocName,getSketchObject()->getDocument()->getName())==0
                &&strcmp(msg.pObjectName,getSketchObject()->getNameInDocument())== 0) {
                    if(msg.pSubName){
                        std::string shapetype(msg.pSubName);
                        if (shapetype.size() > 4 && shapetype.substr(0,4) == "Edge") {
                            int index=std::atoi(&shapetype[4]);
                            edit->SelCurvSet.insert(index);
                            updateColor();
                        }
                        else if (shapetype.size() > 6 && shapetype.substr(0,6) == "Vertex") {
                            int index=std::atoi(&shapetype[6]);
                            edit->SelPointSet.insert(index);
                            updateColor();
                        }
                        else if (shapetype.size() > 10 && shapetype.substr(0,10) == "Constraint") {
                            int index=std::atoi(&shapetype[10]);
                            edit->SelConstraintSet.insert(index);
                            updateColor();
                        }

                    }
            }        
        }
        else if (msg.Type == Gui::SelectionChanges::RmvSelection) {
            //// build name
            //temp = Reason.pDocName;
            //temp += ".";
            //temp += Reason.pObjectName;
            //if (Reason.pSubName[0] != 0) {
            //    temp += ".";
            //    temp += Reason.pSubName;
            //}

            //// remove all items
            //QList<QListWidgetItem *> l = selectionView->findItems(QLatin1String(temp.c_str()),Qt::MatchExactly);
            //if (l.size() == 1)
            //    delete l[0];

        }
        else if (msg.Type == Gui::SelectionChanges::SetSelection) {
            // remove all items
            //selectionView->clear();
            //std::vector<SelectionSingleton::SelObj> objs = Gui::Selection().getSelection(Reason.pDocName);
            //for (std::vector<SelectionSingleton::SelObj>::iterator it = objs.begin(); it != objs.end(); ++it) {
            //    // build name
            //    temp = it->DocName;
            //    temp += ".";
            //    temp += it->FeatName;
            //    if (it->SubName && it->SubName[0] != '\0') {
            //        temp += ".";
            //        temp += it->SubName;
            //    }
            //    new QListWidgetItem(QString::fromAscii(temp.c_str()), selectionView);
            //}
        }
        
    }
}

bool ViewProviderSketch::detectPreselection(const SoPickedPoint* Point, int &PtIndex,int &CurvIndex, int &ConstrIndex)
{
    assert(edit);

    PtIndex = -1;
    CurvIndex = -1;
    ConstrIndex = -1;

    if (Point) {
        //Base::Console().Log("Point pick\n");
        SoPath * path = Point->getPath();
        SoNode * tail = path->getTail();
        SoNode * tailFather = path->getNode(path->getLength()-2);
        SoNode * tailFather2 = path->getNode(path->getLength()-3);

        // cecking for a hit in the points
        if(tail == edit->PointSet){
            const SoDetail* point_detail = Point->getDetail(edit->PointSet);
            if (point_detail && point_detail->getTypeId() == SoPointDetail::getClassTypeId()) {
                // get the index
                PtIndex = static_cast<const SoPointDetail*>(point_detail)->getCoordinateIndex();
            }
        } else {
            // checking for a hit in the Curves
            if(tail == edit->CurveSet){ 
                const SoDetail* curve_detail = Point->getDetail(edit->CurveSet);
                if (curve_detail && curve_detail->getTypeId() == SoLineDetail::getClassTypeId()) {
                    // get the index
                    CurvIndex = static_cast<const SoLineDetail*>(curve_detail)->getLineIndex();
                }
            }else {
                // checking if a constraint is hit
                if(tailFather2 == edit->constrGroup)
                    for(int i=0; i< edit->constrGroup->getNumChildren();i++)
                        if(edit->constrGroup->getChild(i) == tailFather){
                            ConstrIndex = i;
                            //Base::Console().Log("Constr %d pick\n",i);
                            break;
                        }

            }
            
        }
        
    
        if(ConstrIndex>=0){ // if a constraint is hit
            std::stringstream ss;
            ss << "Constraint" << ConstrIndex;
            Gui::Selection().setPreselect(getSketchObject()->getDocument()->getName()
                                          ,getSketchObject()->getNameInDocument()
                                          ,ss.str().c_str()
                                          ,Point->getPoint()[0]
                                          ,Point->getPoint()[1]
                                          ,Point->getPoint()[2]);
                                                 
            if(ConstrIndex != edit->PreselectConstraint){
                edit->PreselectConstraint = ConstrIndex;
                edit->PreselectPoint = -1;
                edit->PreselectCurve = -1;
                return true;
            }else
                return false;
        }else if(PtIndex>=0){ // if a point is hit
            std::stringstream ss;
            ss << "Vertex" << PtIndex;
            Gui::Selection().setPreselect(getSketchObject()->getDocument()->getName()
                                          ,getSketchObject()->getNameInDocument()
                                          ,ss.str().c_str()
                                          ,Point->getPoint()[0]
                                          ,Point->getPoint()[1]
                                          ,Point->getPoint()[2]);
                                                 
            if(PtIndex != edit->PreselectPoint){
                edit->PreselectPoint = PtIndex;
                edit->PreselectCurve = -1;
                edit->PreselectConstraint = -1;
                return true;
            }else
                return false;
        }else if(CurvIndex>=0){  // if a curve is hit
            if(CurvIndex != edit->PreselectCurve){
                edit->PreselectCurve = CurvIndex;
                edit->PreselectPoint = -1;
                edit->PreselectConstraint = -1;
                std::stringstream ss;
                ss << "Edge" << CurvIndex;
                Gui::Selection().setPreselect(getSketchObject()->getDocument()->getName()
                                             ,getSketchObject()->getNameInDocument()
                                             ,ss.str().c_str()
                                             ,Point->getPoint()[0]
                                             ,Point->getPoint()[1]
                                             ,Point->getPoint()[2]);

                return true;
            }else{
                Gui::Selection().setPreselectCoord(Point->getPoint()[0]
                                                  ,Point->getPoint()[1]
                                                  ,Point->getPoint()[2]);
            }
        }else if((CurvIndex<0 && PtIndex<0) && (edit->PreselectCurve>=0 || edit->PreselectPoint>=0) ){
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;
            edit->PreselectConstraint = -1;
           return true;
        }

    }else if(edit->PreselectCurve>=0 || edit->PreselectPoint>=0 || edit->PreselectConstraint>=0){
        edit->PreselectCurve = -1;
        edit->PreselectPoint = -1;
        edit->PreselectConstraint = -1;
        return true;
    }


    return false;
}

void ViewProviderSketch::updateColor(void)
{
    assert(edit);
    //Base::Console().Log("Draw preseletion\n");

    int PtNum = edit->PointsMaterials->diffuseColor.getNum();
    SbColor* pcolor = edit->PointsMaterials->diffuseColor.startEditing();
    int CurvNum = edit->CurvesMaterials->diffuseColor.getNum();
    SbColor* color = edit->CurvesMaterials->diffuseColor.startEditing();

    // colors of the point set
    for (int  i=0;i<PtNum;i++) {
        if (edit->SelPointSet.find(i) != edit->SelPointSet.end())
            pcolor[i] = SelectColor;
        else if(edit->PreselectPoint==i)
            pcolor[i] = PreselectColor;
        else
            pcolor[i] = sPointColor;
    }

    // colors or the curves
    for (int  i=0;i<CurvNum;i++) {
        if (edit->SelCurvSet.find(i) != edit->SelCurvSet.end())
            color[i] = SelectColor;
        else if(edit->PreselectCurve==i)
            color[i] = PreselectColor;
        else
            color[i] = sCurveColor;
    }
    // colors of the constraints
    for (int i=0; i< edit->constrGroup->getNumChildren();i++) {
        SoSeparator *s = dynamic_cast<SoSeparator *>(edit->constrGroup->getChild(i));
        SoMaterial *m = dynamic_cast<SoMaterial *>(s->getChild(0));
        if (edit->SelConstraintSet.find(i) != edit->SelConstraintSet.end())
            m->diffuseColor = SelectColor;
        else if(edit->PreselectConstraint==i)
            m->diffuseColor = PreselectColor;
        else
            m->diffuseColor = sConstraintColor;
    }

    // end edeting
    edit->CurvesMaterials->diffuseColor.finishEditing();
    edit->PointsMaterials->diffuseColor.finishEditing();
}

bool ViewProviderSketch::isPointOnSketch(const SoPickedPoint* pp) const
{
    // checks if we picked a point on the sketch or any other nodes like the grid
    SoPath* path = pp->getPath();
    return path->containsNode(edit->EditRoot);
}

bool ViewProviderSketch::doubleClicked(void)
{
    Gui::Application::Instance->activeDocument()->setEdit(this);
    return true;
}

void ViewProviderSketch::draw(bool temp)
{
    assert(edit);

    // Render Geometry ===================================================
    std::vector<Base::Vector3d> Coords;
    std::vector<Base::Vector3d> Points;
    std::vector<unsigned int> Index;
    std::vector<unsigned int> Color;
    std::vector<unsigned int> PtColor;

    const std::vector<Part::Geometry *> *geomlist;
    std::vector<Part::Geometry *> tempGeo;
    if (temp) {
        tempGeo = edit->ActSketch.getGeometry();
        geomlist = &tempGeo;
    }
    else
        geomlist = &getSketchObject()->Geometry.getValues();

    for (std::vector<Part::Geometry *>::const_iterator it=geomlist->begin();it!=geomlist->end();++it) {
        if ((*it)->getTypeId()== Part::GeomLineSegment::getClassTypeId()){ // add a line
            const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(*it);
            // create the definition struct for that geom
            Coords.push_back(lineSeg->getStartPoint());
            Coords.push_back(lineSeg->getEndPoint());
            Points.push_back(lineSeg->getStartPoint());
            Points.push_back(lineSeg->getEndPoint());
            Index.push_back(2);
            Color.push_back(0);
            PtColor.push_back(0);
            PtColor.push_back(0);
        } else {
            ; 
        }
    }

    edit->CurveSet->numVertices.setNum(Index.size());
    edit->CurvesCoordinate->point.setNum(Coords.size());
    edit->CurvesMaterials->diffuseColor.setNum(Color.size());
    edit->PointsCoordinate->point.setNum(Points.size());
    edit->PointsMaterials->diffuseColor.setNum(PtColor.size());

    SbVec3f* verts = edit->CurvesCoordinate->point.startEditing();
    int32_t* index = edit->CurveSet->numVertices.startEditing();
    SbColor* color = edit->CurvesMaterials->diffuseColor.startEditing();
    SbVec3f* pverts = edit->PointsCoordinate->point.startEditing();
    SbColor* pcolor = edit->PointsMaterials->diffuseColor.startEditing();

    int i=0; // setting up the line set
    for(std::vector<Base::Vector3d>::const_iterator it=Coords.begin();it!=Coords.end();++it,i++)
        verts[i].setValue(it->x,it->y,0.1f);
    
    i=0; // setting up the indexes of the line set
    for(std::vector<unsigned int>::const_iterator it=Index.begin();it!=Index.end();++it,i++)
        index[i]=*it;
    
    i=0; // color of the line set
    for(std::vector<unsigned int>::const_iterator it=Color.begin();it!=Color.end();++it,i++)
        color[i]=(*it==1?sCurveConstructionColor:sCurveColor);
    
    i=0; // setting up the point set
    for(std::vector<Base::Vector3d>::const_iterator it=Points.begin();it!=Points.end();++it,i++)
        pverts[i].setValue(it->x,it->y,0.1f);
    
    i=0; // color of the point set
    for(std::vector<unsigned int>::const_iterator it=PtColor.begin();it!=PtColor.end();++it,i++)
        pcolor[i]=(*it==1?sCurveConstructionColor:sPointColor);

    if (edit->PreselectPoint >= 0 && edit->PreselectPoint < (int) Points.size())
        pcolor[edit->PreselectPoint]=PreselectColor;

    edit->CurvesCoordinate->point.finishEditing();
    edit->CurveSet->numVertices.finishEditing();
    edit->CurvesMaterials->diffuseColor.finishEditing();
    edit->PointsCoordinate->point.finishEditing();
    edit->PointsMaterials->diffuseColor.finishEditing();


    // Render Constraints ===================================================
    const std::vector<Sketcher::Constraint*> &ConStr = getSketchObject()->Constraints.getValues();
    // reset point if the constraint type has changed
Restart:
    // check if a new constraint arrived
    if (ConStr.size() != edit->vConstrType.size())
        rebuildConstraintsVisual();
    assert((int)ConStr.size()==edit->constrGroup->getNumChildren());
    assert((int)edit->vConstrType.size()==edit->constrGroup->getNumChildren());
    // go through the constraints and update the position 
    i = 0;
    for(std::vector<Sketcher::Constraint*>::const_iterator it=ConStr.begin();it!=ConStr.end();++it,i++){
        // check if the type has changed
        if((*it)->Type != edit->vConstrType[i]){
            // clearing the type vector will force a rebuild of the visual nodes
            edit->vConstrType.clear();
            goto Restart;
        }
        // root separator for this constraint
        SoSeparator *sep = dynamic_cast<SoSeparator *>(edit->constrGroup->getChild(i));
        const Constraint *Constr = *it;
        // distinquish different constraint types to build up
        switch(Constr->Type) {
            case Horizontal: // write the new position of the Horizontal constraint
                {
                    assert(Constr->First < (int)geomlist->size());
                    // get the geometry
                    const Part::Geometry *geo = (*geomlist)[Constr->First];
                    // horizontal can only be a GeomLineSegment
                    assert(geo->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                    const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);
                    // calculate the half distance between the start and endpoint
                    Base::Vector3d pos = lineSeg->getStartPoint() + ((lineSeg->getEndPoint()-lineSeg->getStartPoint())/2);
                    dynamic_cast<SoTranslation *>(sep->getChild(1))->translation = SbVec3f(pos.x,pos.y,0.0f);
                }   break;
            case Vertical: // write the new position of the Vertical constraint
                {
                    assert(Constr->First < (int)geomlist->size());
                    // get the geometry
                    const Part::Geometry *geo = (*geomlist)[Constr->First];
                    // Vertical can only be a GeomLineSegment
                    assert(geo->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                    const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);
                    // calculate the half distance between the start and endpoint
                    Base::Vector3d pos = lineSeg->getStartPoint() + ((lineSeg->getEndPoint()-lineSeg->getStartPoint())/2);
                    dynamic_cast<SoTranslation *>(sep->getChild(1))->translation = SbVec3f(pos.x,pos.y,0.0f);
                }   break;
            case Parallel:
                {
                    assert(Constr->First < (int)geomlist->size());
                    assert(Constr->Second < (int)geomlist->size());
                    // get the geometry
                    const Part::Geometry *geo1 = (*geomlist)[Constr->First];
                    const Part::Geometry *geo2 = (*geomlist)[Constr->Second];
                    // Vertical can only be a GeomLineSegment
                    assert(geo1->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                    assert(geo2->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                    const Part::GeomLineSegment *lineSeg1 = dynamic_cast<const Part::GeomLineSegment*>(geo1);
                    const Part::GeomLineSegment *lineSeg2 = dynamic_cast<const Part::GeomLineSegment*>(geo2);
                    // calculate the half-distance between the start and endpoint
                    Base::Vector3d pos1 = lineSeg1->getStartPoint() + ((lineSeg1->getEndPoint()-lineSeg1->getStartPoint())/2);
                    Base::Vector3d pos2 = lineSeg2->getStartPoint() + ((lineSeg2->getEndPoint()-lineSeg2->getStartPoint())/2);
                    // move the second point because of two translations in a row. 
                    pos2 = pos2 - pos1;
                    dynamic_cast<SoTranslation *>(sep->getChild(1))->translation = SbVec3f(pos1.x,pos1.y,0.0f);
                    dynamic_cast<SoTranslation *>(sep->getChild(3))->translation = SbVec3f(pos2.x,pos2.y,0.0f);
                }   break;
            case Distance:
                {
                    assert(Constr->First < (int)geomlist->size());
                    // get the geometry
                    const Part::Geometry *geo = (*geomlist)[Constr->First];
                    // Vertical can only be a GeomLineSegment
                    assert(geo->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                    const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);
                    // calculate the half distance between the start and endpoint
                    SbVec3f p1(lineSeg->getStartPoint().x,lineSeg->getStartPoint().y,0);
                    SbVec3f p2(lineSeg->getEndPoint().x,lineSeg->getEndPoint().y,0);
                    SbVec3f dir = p2 - p1;
                    SbVec3f pos = p1 + dir/2;
                    dir.normalize();
                    // rotate direction perpendicular
                    dir = SbVec3f (-dir[1],dir[0],0); 
                    // set the line coordinates:
                    dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(0,p1);
                    dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(1,p1+dir*12);
                    dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(2,p2);
                    dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(3,p2+dir*12);
                    dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(4,p1+dir*10);
                    dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(5,p2+dir*10);
                    // set position of datum
                    dynamic_cast<SoTranslation *>(sep->getChild(3))->translation = pos + dir*10;
                    dynamic_cast<SoText2 *>(sep->getChild(4))->string = SbString().sprintf("%.2f",Constr->Value);
                }   break;
            case Angle:
            case Coincident: // nothing to do for coincident
            case None:
                break;
        }
    }

    //edit->ActSketch.Cons

    // delete the cloned objects 
    for(std::vector<Part::Geometry *>::iterator it=tempGeo.begin();it!=tempGeo.end();++it)
        if(*it)delete(*it);

    Gui::MDIView* mdi = Gui::Application::Instance->activeDocument()->getActiveView();
    if (mdi && mdi->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        static_cast<Gui::View3DInventor*>(mdi)->getViewer()->render();
    }


}

void ViewProviderSketch::rebuildConstraintsVisual(void)
{
    const std::vector<Sketcher::Constraint*> &ConStr = getSketchObject()->Constraints.getValues();
    // clean up 
    edit->constrGroup->removeAllChildren();
    edit->vConstrType.clear();

    for (std::vector<Sketcher::Constraint*>::const_iterator it=ConStr.begin();it!=ConStr.end();++it){
        // root separator for one constraint
        SoSeparator *sep = new SoSeparator();
        // no caching for fluctuand data structures
        sep->renderCaching = SoSeparator::OFF;
        // every constrained visual node gets its own material for preselection and selection
        SoMaterial *Material = new SoMaterial;
        Material->diffuseColor = sConstraintColor;
        sep->addChild(Material);

        // destiquish different constraint types to build up
        switch((*it)->Type) {
            case Horizontal: // add a Text node with the "H" for that constraint
                {
                    sep->addChild(new SoTranslation());
                    SoText2 *text = new SoText2();
                    text->justification = SoText2::LEFT;
                    text->string = "H";
                    sep->addChild(text); 
                    // remeber the type of this constraint node
                    edit->vConstrType.push_back(Horizontal);
                }
                break;
            case Vertical: // add a Text node with the "V" for that constraint
                {
                    sep->addChild(new SoTranslation());
                    SoText2 *text = new SoText2();
                    text->justification = SoText2::LEFT;
                    text->string = "V";
                    sep->addChild(text); 
                    // remeber the type of this constraint node
                    edit->vConstrType.push_back(Vertical);
                 }
                break;
            case Coincident: // no visual for coincident so far
                edit->vConstrType.push_back(Coincident);
                break;
            case Parallel: 
                {
                    sep->addChild(new SoTranslation());
                    SoText2 *text = new SoText2();
                    text->justification = SoText2::LEFT;
                    text->string = "P";
                    sep->addChild(text); 
                    sep->addChild(new SoTranslation());
                    text = new SoText2();
                    text->justification = SoText2::LEFT;
                    text->string = "P";
                    sep->addChild(text); 
                    // remeber the type of this constraint node
                    edit->vConstrType.push_back(Parallel);
                }
                break;
            case Distance: 
                {
                    // nodes for the datum lines 
                    sep->addChild(new SoCoordinate3);
                    SoLineSet *lineSet = new SoLineSet;
                    lineSet->numVertices.set1Value(0,2);
                    lineSet->numVertices.set1Value(1,2);
                    lineSet->numVertices.set1Value(2,2);
                    sep->addChild(lineSet);

                    // text node for Distance Value
                    sep->addChild(new SoTranslation());
                    SoText2 *text = new SoText2();
                    text->justification = SoText2::LEFT;
                    text->string = "";
                    sep->addChild(text); 
                    edit->vConstrType.push_back(Distance);
                }
                break;
            default:
                edit->vConstrType.push_back(None);
                
        }

        edit->constrGroup->addChild(sep);
    }
}

void ViewProviderSketch::drawEdit(const std::vector<Base::Vector2D> &EditCurve)
{
    assert(edit);

    edit->EditCurveSet->numVertices.setNum(1);
    edit->EditCurvesCoordinate->point.setNum(EditCurve.size());
    SbVec3f* verts = edit->EditCurvesCoordinate->point.startEditing();
    int32_t* index = edit->EditCurveSet->numVertices.startEditing();

    int i=0; // setting up the line set
    for (std::vector<Base::Vector2D>::const_iterator it=EditCurve.begin();it!=EditCurve.end();++it,i++)
        verts[i].setValue(it->fX,it->fY,0.1f);

    index[0] = EditCurve.size();
    edit->EditCurvesCoordinate->point.finishEditing();
    edit->EditCurveSet->numVertices.finishEditing();
}

void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProvider2DObject::updateData(prop);

    if(edit && (prop == &(getSketchObject()->Geometry) || &(getSketchObject()->Constraints) )){
        edit->ActSketch.setUpSketch(getSketchObject()->Geometry.getValues(),getSketchObject()->Constraints.getValues());
 /*       double * fixed[2]={0,0};
        if(edit->ActSketch.solve(fixed) == 0)
            signalSolved(0,edit->ActSketch.SolveTime);
        else
            signalSolved(1,edit->ActSketch.SolveTime);*/
        draw(true);    
    }
    if (edit && &(getSketchObject()->Constraints) ){
        // send the signal for the TaskDlg.
        signalConstraintsChanged();
    }
}

void ViewProviderSketch::onChanged(const App::Property* prop)
{
    // call father
    PartGui::ViewProvider2DObject::onChanged(prop);
}

void ViewProviderSketch::attach(App::DocumentObject *pcFeat)
{
    ViewProviderPart::attach(pcFeat);
}

void ViewProviderSketch::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act = menu->addAction(QObject::tr("Edit sketch"), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));
}

bool ViewProviderSketch::setEdit(int ModNum)
{
    // When double-clicking on the item for this sketch the
    // object unsets and sets its edit mode without closing
    // the task panel
    Gui::TaskView::TaskDialog* dlg = Gui::Control().activeDialog();
    TaskDlgEditSketch* sketchDlg = qobject_cast<TaskDlgEditSketch*>(dlg);
    if (sketchDlg && sketchDlg->getSketchView() != this)
        sketchDlg = 0; // another sketch left open its task panel
    if (dlg && !sketchDlg) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("A dialog is already open in the task panel"));
        msgBox.setInformativeText(QObject::tr("Do you want to close this dialog?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
            Gui::Control().closeDialog();
        else
            return false;
    }

    // clear the selction (convenience)
    Gui::Selection().clearSelection();

    // create the container for the addtitional edit data
    assert(!edit);
    edit = new EditData();

    // fill up actuall constraints and geometry
    edit->ActSketch.setUpSketch(getSketchObject()->Geometry.getValues(),getSketchObject()->Constraints.getValues());


    createEditInventorNodes();
    this->hide(); // avoid that the wires interfere with the edit lines

    ShowGrid.setValue(true);
 
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/View");
    float transparency;
    // set the highlight color
    unsigned long highlight = (unsigned long)(PreselectColor.getPackedValue());
    highlight = hGrp->GetUnsigned("HighlightColor", highlight);
    PreselectColor.setPackedValue((uint32_t)highlight, transparency);
    // set the selection color
    highlight = (unsigned long)(SelectColor.getPackedValue());
    highlight = hGrp->GetUnsigned("SelectionColor", highlight);
    SelectColor.setPackedValue((uint32_t)highlight, transparency);

    draw();

    // start the edit dialog
    if (sketchDlg)
        Gui::Control().showDialog(sketchDlg);
    else
        Gui::Control().showDialog(new TaskDlgEditSketch(this));

    return true;
}

void ViewProviderSketch::createEditInventorNodes(void)
{
    assert(edit);

    edit->EditRoot = new SoSeparator;
    pcRoot->addChild(edit->EditRoot);
    edit->EditRoot->renderCaching = SoSeparator::OFF ;
    
    // stuff for the points ++++++++++++++++++++++++++++++++++++++
    edit->PointsMaterials = new SoMaterial;
    edit->EditRoot->addChild(edit->PointsMaterials);

    SoMaterialBinding *MtlBind = new SoMaterialBinding;
    MtlBind->value = SoMaterialBinding::PER_VERTEX;
    edit->EditRoot->addChild(MtlBind);

    edit->PointsCoordinate = new SoCoordinate3;
    edit->EditRoot->addChild(edit->PointsCoordinate);

    SoDrawStyle *DrawStyle = new SoDrawStyle;
    DrawStyle->pointSize = 8;
    edit->EditRoot->addChild( DrawStyle );
    edit->PointSet = new SoMarkerSet;
    edit->PointSet->markerIndex = SoMarkerSet::CIRCLE_FILLED_7_7;
    edit->EditRoot->addChild( edit->PointSet );

    // stuff for the lines +++++++++++++++++++++++++++++++++++++++
    edit->LinesMaterials = new SoMaterial;
    edit->EditRoot->addChild(edit->LinesMaterials);

    MtlBind = new SoMaterialBinding;
    MtlBind->value = SoMaterialBinding::PER_PART;
    edit->EditRoot->addChild(MtlBind);

    edit->LinesCoordinate = new SoCoordinate3;
    edit->EditRoot->addChild(edit->LinesCoordinate);

    DrawStyle = new SoDrawStyle;
    DrawStyle->lineWidth = 3;
    DrawStyle->linePattern = 0x0fff;
    edit->EditRoot->addChild(DrawStyle);

    edit->LineSet = new SoLineSet;

    edit->EditRoot->addChild(edit->LineSet);

    // stuff for the Curves +++++++++++++++++++++++++++++++++++++++
    edit->CurvesMaterials = new SoMaterial;
    edit->EditRoot->addChild(edit->CurvesMaterials);

    MtlBind = new SoMaterialBinding;
    MtlBind->value = SoMaterialBinding::PER_FACE;
    edit->EditRoot->addChild(MtlBind);

    edit->CurvesCoordinate = new SoCoordinate3;
    edit->EditRoot->addChild(edit->CurvesCoordinate);

    DrawStyle = new SoDrawStyle;
    DrawStyle->lineWidth = 3;
    edit->EditRoot->addChild( DrawStyle );

    edit->CurveSet = new SoLineSet;

    edit->EditRoot->addChild( edit->CurveSet );

    // stuff for the EditCurves +++++++++++++++++++++++++++++++++++++++
    edit->EditCurvesMaterials = new SoMaterial;
    edit->EditRoot->addChild(edit->EditCurvesMaterials);

    edit->EditCurvesCoordinate = new SoCoordinate3;
    edit->EditRoot->addChild(edit->EditCurvesCoordinate);

    DrawStyle = new SoDrawStyle;
    DrawStyle->lineWidth = 3;
    edit->EditRoot->addChild( DrawStyle );

    edit->EditCurveSet = new SoLineSet;
    edit->EditRoot->addChild( edit->EditCurveSet );

    // stuff for the edit coordinates ++++++++++++++++++++++++++++++++++++++
    SoMaterial *EditMaterials = new SoMaterial;
    EditMaterials->diffuseColor = SbColor(0,0,1);
    edit->EditRoot->addChild(EditMaterials);

    SoSeparator * Coordsep = new SoSeparator();
    // no caching for fluctuand data structures
    Coordsep->renderCaching = SoSeparator::OFF;

    SoFont * font = new SoFont();
    font->size = 15.0;
    Coordsep->addChild(font);

    edit->textPos = new SoTranslation();
    Coordsep->addChild(edit->textPos);

    edit->textX = new SoText2();
    edit->textX->justification = SoText2::LEFT;
    edit->textX->string = "";
    Coordsep->addChild(edit->textX);
    edit->EditRoot->addChild(Coordsep);

    // group node for the Constraint visual +++++++++++++++++++++++++++++++++++
    MtlBind = new SoMaterialBinding;
    MtlBind->value = SoMaterialBinding::OVERALL ;
    edit->EditRoot->addChild(MtlBind);
    
    // add font for the text shown constraints
    font = new SoFont();
    font->size = 15.0;
    edit->EditRoot->addChild(font);

    // use small line with for the Constraints
    DrawStyle = new SoDrawStyle;
    DrawStyle->lineWidth = 1;
    edit->EditRoot->addChild( DrawStyle );


    // add the group where all the constraints has its SoSeparator
    edit->constrGroup = new SoGroup();
    edit->EditRoot->addChild(edit->constrGroup);
}

void ViewProviderSketch::unsetEdit(int ModNum)
{
    ShowGrid.setValue(false);
 
    edit->EditRoot->removeAllChildren();
    pcRoot->removeChild(edit->EditRoot);

    if (edit->sketchHandler) {
        edit->sketchHandler->unsetCursor();
        purgeHandler();
    }
    delete edit;
    edit = 0;
 
    this->show();

    // when pressing ESC make sure to close the dialog
    // and update the sketch
    getSketchObject()->getDocument()->recompute();

    // clear the selction and set the new/edited sketch(convenience)
    Gui::Selection().clearSelection();
    std::string ObjName = getSketchObject()->getNameInDocument();
    std::string DocName = getSketchObject()->getDocument()->getName();
    Gui::Selection().addSelection(DocName.c_str(),ObjName.c_str());


    Gui::Control().closeDialog();
}

void ViewProviderSketch::setPositionText(const Base::Vector2D &Pos)
{
    char buf[40];
    sprintf( buf, " (%.1f,%.1f)", Pos.fX,Pos.fY );
    edit->textX->string = buf;
    edit->textPos->translation = SbVec3f(Pos.fX,Pos.fY,0.2f);
}

void ViewProviderSketch::resetPositionText(void)
{
    edit->textX->string = "";
}

int ViewProviderSketch::getPreselectPoint(void)const
{
    if (edit)
        return edit->PreselectPoint;
    return -1;
}
int ViewProviderSketch::getPreselectCurve(void)const
{
    if (edit)
        return edit->PreselectCurve;
    return -1;
}

int ViewProviderSketch::getPreselectConstraint(void)const
{
    if (edit)
        return edit->PreselectConstraint;
    return -1;
}

void ViewProviderSketch::setGridSnap(int Type)
{
    assert(edit);
}

Sketcher::SketchObject* ViewProviderSketch::getSketchObject(void) const
{
    return dynamic_cast<Sketcher::SketchObject*>(pcObject);
}

