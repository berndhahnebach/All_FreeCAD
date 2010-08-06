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
# include <QMessageBox>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Base/Vector3D.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>

#include <Mod/Part/App/Geometry.h>
#include <Mod/Sketcher/App/SketchFlatInterface.h>
#include <Mod/Sketcher/App/SketchObject.h>
#include <Mod/Sketcher/App/Sketch.h>


#include "ViewProviderSketch.h"
#include "DrawSketchHandler.h"


using namespace SketcherGui;
using namespace Sketcher;

SbColor sCurveColor             (1.0f,1.0f,1.0f); 
SbColor sCurveConstructionColor (0.2f,1.0f,0.2f); 
SbColor sPointColor             (0.5f,0.5f,0.5f); 
SbColor sDatumLineColor         (0.0f,0.8f,0.0f); 

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
    // pointer to the Solver
    Sketcher::Sketch ActSketch;
    // container to track our own selected parts
    std::set<int> SelPointSet;
    std::set<int> SelCurvSet;

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
};


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProvider2DObject)


ViewProviderSketch::ViewProviderSketch()
  : 
    Mode(STATUS_NONE),
    //sketchHandler(0),
    //DragPoint(-1),
    //PreselectCurve(-1),
    //PreselectPoint(-1),
    //ActSketch(0),
    //EditRoot(0),
    //PointsMaterials(0),
    //CurvesMaterials(0),
    //LinesMaterials(0),
    //PointsCoordinate(0),
    //CurvesCoordinate(0),
    //LinesCoordinate(0),
    //CurveSet(0),
    //LineSet(0),
    //PointSet(0),
    edit(0)
{
    sPixmap = "Sketcher_NewSketch";
}

ViewProviderSketch::~ViewProviderSketch()
{}


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
    //else if (Button == 2) {
    //    if (pressed) {
    //        switch(Mode){
    //             default:
    //                return false;
    //        }
    //    }
    //}

    return false;
}

bool ViewProviderSketch::mouseMove(const SbVec3f &point, const SbVec3f &normal, const SoPickedPoint* pp)
{
    assert(edit);

    double x,y;
    getCoordsOnSketchPlane(x,y,point,normal);

    int PtIndex,CurvIndex;
    bool preselectChanged = detectPreselection(pp,PtIndex,CurvIndex);

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

            return true;
        case STATUS_SELECT_Edge:
            // drag a edge not implemented yet!
            Mode = STATUS_NONE;
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;

            return true;
        case STATUS_SKETCH_DragPoint:
            if(edit->DragPoint != -1){
                Base::Console().Log("Drag Point:%d\n",edit->DragPoint);
                int ret;
                if ((ret=edit->ActSketch.movePoint(edit->DragPoint/2,edit->DragPoint%2==0?start:end,Base::Vector3d(x,y,0))) == 0){
                    setPositionText(Base::Vector2D(x,y));
                    draw(true);
                }
                else
                    Base::Console().Log("Error solving:%d\n",ret);
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
            if ( edit->SelPointSet.size() > 0 || edit->SelCurvSet.size() > 0){
                // clear our selection and update the color of the viewed edges and points
                edit->SelPointSet.clear();
                edit->SelCurvSet.clear();
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


bool ViewProviderSketch::detectPreselection(const SoPickedPoint* Point, int &PtIndex,int &CurvIndex)
{
    assert(edit);

    PtIndex = -1;
    CurvIndex = -1;

    if (Point) {
        //Base::Console().Log("Point pick\n");
        const SoDetail* point_detail = Point->getDetail(edit->PointSet);
        if (point_detail && point_detail->getTypeId() == SoPointDetail::getClassTypeId()) {
            // get the index
            PtIndex = static_cast<const SoPointDetail*>(point_detail)->getCoordinateIndex();
        }

        const SoDetail* curve_detail = Point->getDetail(edit->CurveSet);
        if (curve_detail && curve_detail->getTypeId() == SoLineDetail::getClassTypeId()) {
            // get the index
            CurvIndex = static_cast<const SoLineDetail*>(curve_detail)->getLineIndex();
        }
    
        assert(PtIndex < 0 || CurvIndex < 0);
        if(PtIndex>=0){ // if a point is hit
            std::stringstream ss;
            ss << "Vertex" << edit->PreselectPoint;
            Gui::Selection().setPreselect(getSketchObject()->getDocument()->getName()
                                          ,getSketchObject()->getNameInDocument()
                                          ,ss.str().c_str()
                                          ,Point->getPoint()[0]
                                          ,Point->getPoint()[1]
                                          ,Point->getPoint()[2]);
                                                 
            if(PtIndex != edit->PreselectPoint){
                edit->PreselectPoint = PtIndex;
                edit->PreselectCurve = -1;
                return true;
            }else
                return false;
        }else if(CurvIndex>=0){  // if a curve is hit
            if(CurvIndex != edit->PreselectCurve){
                edit->PreselectCurve = CurvIndex;
                edit->PreselectPoint = -1;
                std::stringstream ss;
                ss << "Edge" << edit->PreselectCurve;
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
            return true;
        }

    }else if(edit->PreselectCurve>=0 || edit->PreselectPoint>=0 ){
        edit->PreselectCurve = -1;
        edit->PreselectPoint = -1;
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

    // color of the point set
    for(int  i=0;i<PtNum;i++)
        if(edit->SelPointSet.find(i) != edit->SelPointSet.end()){
            pcolor[i] = SelectColor;
        }else if(edit->PreselectPoint==i){
            pcolor[i] = PreselectColor;
        }else
             pcolor[i] = sPointColor;
    for(int  i=0;i<CurvNum;i++)
        if(edit->SelCurvSet.find(i) != edit->SelCurvSet.end()){
            color[i] = SelectColor;
        }else if(edit->PreselectCurve==i){
            color[i] = PreselectColor;
        }else
             color[i] = sCurveColor;
        //color[i]=PreselectCurve==i?PreselectColor:sCurveColor;

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
    if(temp){
        tempGeo = edit->ActSketch.getGeometry();
        geomlist = &tempGeo;
    }else
        geomlist = &getSketchObject()->Geometry.getValues();

    for(std::vector<Part::Geometry *>::const_iterator it=geomlist->begin();it!=geomlist->end();++it){
        if((*it)->getTypeId()== Part::GeomLineSegment::getClassTypeId()){ // add a line
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

    // delete the cloned objects 
    for(std::vector<Part::Geometry *>::iterator it=tempGeo.begin();it!=tempGeo.end();++it)
        if(*it)delete(*it);



}

void ViewProviderSketch::drawEdit(const std::vector<Base::Vector2D> &EditCurve)
{
    assert(edit);

    edit->EditCurveSet->numVertices.setNum(1);
    edit->EditCurvesCoordinate->point.setNum(EditCurve.size());
    SbVec3f* verts = edit->EditCurvesCoordinate->point.startEditing();
    int32_t* index = edit->EditCurveSet->numVertices.startEditing();

    int i=0; // setting up the line set
    for(std::vector<Base::Vector2D>::const_iterator it=EditCurve.begin();it!=EditCurve.end();++it,i++)
        verts[i].setValue(it->fX,it->fY,0.1f);

    index[0] = EditCurve.size();
    edit->EditCurvesCoordinate->point.finishEditing();
    edit->EditCurveSet->numVertices.finishEditing();

}


void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProvider2DObject::updateData(prop);

    if(edit && (prop == &(getSketchObject()->Geometry) || &(getSketchObject()->Constraints) )){
        edit->ActSketch.clear();
        edit->ActSketch.addGeometry(getSketchObject()->Geometry.getValues());
        edit->ActSketch.addConstraints(getSketchObject()->Constraints.getValues());
        draw(true);    
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

bool ViewProviderSketch::setEdit(int ModNum)
{
    // clear the selction (convenience)
    Gui::Selection().clearSelection();

    // create the container for the addtitional edit data
    assert(!edit);
    edit = new EditData();

    // creat temporary sketch to solve while edeting
    //edit->ActSketch = new Sketcher::Sketch();
    // fill up actuall constraints and geometry
    edit->ActSketch.addGeometry(getSketchObject()->Geometry.getValues());
    edit->ActSketch.addConstraints(getSketchObject()->Constraints.getValues());


    createEditInventorNodes();
    this->hide(); // avoid that the wires interfere with the edit lines

    ShowGrid.setValue(true);
    // create the container to track selection
    //edit->SelPointSet = new std::set<int>;
    //edit->SelCurvSet = new std::set<int>;

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


}

void ViewProviderSketch::unsetEdit(void)
{
    ShowGrid.setValue(false);
 
    // close the solver
    //delete edit->ActSketch;
    //ActSketch = 0;

    // delete the container to track selection
    //delete (SelPointSet);
    //delete (SelCurvSet);
    //SelPointSet = 0;
    //SelCurvSet = 0;

    // empty the nodes

    edit->EditRoot->removeAllChildren();
    pcRoot->removeChild(edit->EditRoot);

    delete edit;
    edit = 0;
    /*
    PointsMaterials = 0;
    LinesMaterials = 0;
    CurvesMaterials = 0;
    PointsCoordinate = 0;
    LinesCoordinate = 0;
    CurvesCoordinate = 0;
    LineSet = 0;
    CurveSet = 0;
    PointSet = 0;

    textPos = 0;
    textX = 0;

    PreselectCurve = -1;
    PreselectPoint = -1;
    */
    this->show();
}

void ViewProviderSketch::setPositionText(const Base::Vector2D &Pos)
{
    char buf[40];
    sprintf( buf, " (%.1f,%.1f)", Pos.fX,Pos.fY );
    edit->textX->string = buf;
    edit->textPos->translation = SbVec3f(Pos.fX,Pos.fY,0.2);

}
void ViewProviderSketch::resetPositionText(void)
{
    edit->textX->string = "";
}

int ViewProviderSketch::getPreselectPoint(void)const
{
    return edit->PreselectPoint;
}
int ViewProviderSketch::getPreselectCurve(void)const
{
    return edit->PreselectCurve;
}

Sketcher::SketchObject* ViewProviderSketch::getSketchObject(void) const
{
    return dynamic_cast<Sketcher::SketchObject*>(pcObject);
}
