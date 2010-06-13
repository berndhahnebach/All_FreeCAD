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
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoVertexProperty.h>
# include <QMessageBox>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Base/Vector3D.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
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

const float fCurveColor[] =     {1.0f,1.0f,1.0f}; 
const float fCurveConstructionColor[] = {0.2f,1.0f,0.2f}; 
const float fPointColor[] =             {0.5f,0.5f,0.5f}; 
const float fPreselectColor[] = {0.8f,0.0f,0.0f}; 
const float fSelectColor[] =    {1.0f,0.0f,0.0f}; 
const float fDatumLineColor[] = {0.0f,0.8f,0.0f}; 

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProvider2DObject)


ViewProviderSketch::ViewProviderSketch()
  : Mode(STATUS_NONE),
    DragPoint(-1),
    EditRoot(0),

	PointsMaterials(0),
	LinesMaterials(0),
	CurvesMaterials(0),
	PointsCoordinate(0),
	LinesCoordinate(0),
	CurvesCoordinate(0),
	LineSet(0),
	CurveSet(0),
    PointSet(0),

    PreselectCurve(-1),
    PreselectPoint(-1),

    sketchHandler(0),
    ActSketch(0)

    
{
    sPixmap = "Sketcher_NewSketch";
}

ViewProviderSketch::~ViewProviderSketch()
{}


// handler management ***************************************************************
void ViewProviderSketch::activateHandler(DrawSketchHandler *newHandler)
{
    assert(sketchHandler == 0);
    sketchHandler = newHandler;
    Mode = STATUS_SKETCH_UseHandler;
    sketchHandler->sketchgui = this;
    sketchHandler->activated(this);

}


/// removes the active handler
void ViewProviderSketch::purgeHandler(void)
{
    assert(sketchHandler != 0);
    delete(sketchHandler);  
    sketchHandler = 0;
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
                    if (PreselectPoint >=0) {
 						//Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
					    Mode = STATUS_SELECT_Point;
                        return true;
                    } else
                        return false;

				case STATUS_SKETCH_UseHandler:
                    return sketchHandler->pressButton(Base::Vector2D(x,y));
                default:
                    return false;					
			}
        }
        else {
			// Do things depending on the mode of the user interaction
			switch (Mode) {
				case STATUS_SELECT_Point:
                    //Base::Console().Log("Select Point:%d\n",this->DragPoint);
					// Do selection
					//Gui::Selection().addSelection(documentName.getValue().getString()
     //                                            ,objectName.getValue().getString()
     //                                            ,subElementName.getValue().getString()
     //                                            ,pp->getPoint()[0]
     //                                            ,pp->getPoint()[1]
     //                                            ,pp->getPoint()[2]);

                    this->DragPoint = -1;
					Mode = STATUS_NONE;
					return true;
				case STATUS_SKETCH_DragPoint:
                    PreselectPoint = DragPoint;
                    this->DragPoint = -1;
                    drawPreselection();
					Mode = STATUS_NONE;
					return true;
				case STATUS_SKETCH_UseHandler:
                    return sketchHandler->releaseButton(Base::Vector2D(x,y));
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
	double x,y;
	getCoordsOnSketchPlane(x,y,point,normal);

    int PtIndex,CurvIndex;
    bool preselectChanged = detectPreselection(pp,PtIndex,CurvIndex);

	switch (Mode) {
		case STATUS_NONE:
            if(preselectChanged)
                drawPreselection();
            return false;
		case STATUS_SELECT_Point:
			Mode = STATUS_SKETCH_DragPoint;
            this->DragPoint = PreselectPoint;
            PreselectCurve = -1;
            PreselectPoint = -1;

            return true;
		case STATUS_SKETCH_DragPoint:
            Base::Console().Log("Drag Point:%d\n",this->DragPoint);
            int ret;
            if(ret=ActSketch->movePoint(DragPoint/2,DragPoint%2==0?start:end,Base::Vector3d(x,y,0)) == 0)
			    draw(true);
            else
                Base::Console().Log("Error solving:%d\n",ret);
			return true;
		case STATUS_SKETCH_UseHandler:
            if(preselectChanged)
                drawPreselection();
            sketchHandler->mouseMove(Base::Vector2D(x,y));
            return true;
        default:
            return false;					
	}

	return false;
}


bool ViewProviderSketch::detectPreselection(const SoPickedPoint* Point, int &PtIndex,int &CurvIndex)
{
    PtIndex = -1;
    CurvIndex = -1;

    if (Point) {
        //Base::Console().Log("Point pick\n");
        const SoDetail* point_detail = Point->getDetail(this->PointSet);
        if (point_detail && point_detail->getTypeId() == SoPointDetail::getClassTypeId()) {
            // get the index
            PtIndex = static_cast<const SoPointDetail*>(point_detail)->getCoordinateIndex();
        }

        const SoDetail* curve_detail = Point->getDetail(this->CurveSet);
        if (curve_detail && curve_detail->getTypeId() == SoLineDetail::getClassTypeId()) {
            // get the index
            CurvIndex = static_cast<const SoLineDetail*>(curve_detail)->getLineIndex();
        }
    
        assert(PtIndex < 0 || CurvIndex < 0);
        if(PtIndex>=0 && PtIndex != PreselectPoint){
            PreselectPoint = PtIndex;
            PreselectCurve = -1;
            return true;
        }else if(CurvIndex>=0 && CurvIndex != PreselectCurve){
            PreselectCurve = CurvIndex;
            PreselectPoint = -1;
            return true;
        }else if((CurvIndex<0 && PtIndex<0) && (PreselectCurve>=0 || PreselectPoint>=0) ){
            PreselectCurve = -1;
            PreselectPoint = -1;
            return true;
        }

    }else if(PreselectCurve>=0 || PreselectPoint>=0 ){
        PreselectCurve = -1;
        PreselectPoint = -1;
        return true;
    }


    return false;
}

void ViewProviderSketch::drawPreselection(void)
{
    //Base::Console().Log("Draw preseletion\n");

    int PtNum = PointsMaterials->diffuseColor.getNum();
    SbColor* pcolor = PointsMaterials->diffuseColor.startEditing();
    // color of the point set
    for(int  i=0;i<PtNum;i++)
        pcolor[i].setValue(PreselectPoint==i?fPreselectColor:fPointColor);
    PointsMaterials->diffuseColor.finishEditing();

    int CurvNum = CurvesMaterials->diffuseColor.getNum();
    SbColor* color = CurvesMaterials->diffuseColor.startEditing();
    for(int  i=0;i<CurvNum;i++)
        color[i].setValue(PreselectCurve==i?fPreselectColor:fCurveColor);
    CurvesMaterials->diffuseColor.finishEditing();

}

bool ViewProviderSketch::isPointOnSketch(const SoPickedPoint* pp) const
{
    // checks if we picked a point on the sketch or any other nodes like the grid
    SoPath* path = pp->getPath();
    return path->containsNode(EditRoot);
}

bool ViewProviderSketch::doubleClicked(void)
{

	Gui::Application::Instance->activeDocument()->setEdit(this);
	return true;
}

void ViewProviderSketch::solve(void)
{

}

void ViewProviderSketch::draw(bool temp)
{
    // Render Geometry ===================================================
    std::vector<Base::Vector3d> Coords;
    std::vector<Base::Vector3d> Points;
    std::vector<unsigned int> Index;
    std::vector<unsigned int> Color;
    std::vector<unsigned int> PtColor;

    const std::vector<Part::Geometry *> *geomlist;
    std::vector<Part::Geometry *> tempGeo;
    if(temp){
        tempGeo = ActSketch->getGeometry();
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
    CurveSet->numVertices.setNum(Index.size());
    CurvesCoordinate->point.setNum(Coords.size());
    CurvesMaterials->diffuseColor.setNum(Color.size());
    PointsCoordinate->point.setNum(Points.size());
    PointsMaterials->diffuseColor.setNum(PtColor.size());

    SbVec3f* verts = CurvesCoordinate->point.startEditing();
    int32_t* index = CurveSet->numVertices.startEditing();
    SbColor* color = CurvesMaterials->diffuseColor.startEditing();
    SbVec3f* pverts = PointsCoordinate->point.startEditing();
    SbColor* pcolor = PointsMaterials->diffuseColor.startEditing();

    int i=0; // setting up the line set
    for(std::vector<Base::Vector3d>::const_iterator it=Coords.begin();it!=Coords.end();++it,i++)
        verts[i].setValue(it->x,it->y,0.1f);
    
    i=0; // setting up the indexes of the line set
    for(std::vector<unsigned int>::const_iterator it=Index.begin();it!=Index.end();++it,i++)
        index[i]=*it;
    
    i=0; // color of the line set
    for(std::vector<unsigned int>::const_iterator it=Color.begin();it!=Color.end();++it,i++)
        color[i].setValue((*it==1?fCurveConstructionColor:fCurveColor));
    
    i=0; // setting up the point set
    for(std::vector<Base::Vector3d>::const_iterator it=Points.begin();it!=Points.end();++it,i++)
        pverts[i].setValue(it->x,it->y,0.1f);
    
    i=0; // color of the point set
    for(std::vector<unsigned int>::const_iterator it=PtColor.begin();it!=PtColor.end();++it,i++)
        pcolor[i].setValue((*it==1?fCurveConstructionColor:fPointColor));

    if (PreselectPoint >= 0 && PreselectPoint < (int) Points.size())
        color[PreselectPoint].setValue(fPreselectColor);

    CurvesCoordinate->point.finishEditing();
    CurveSet->numVertices.finishEditing();
    CurvesMaterials->diffuseColor.finishEditing();
    PointsCoordinate->point.finishEditing();
    PointsMaterials->diffuseColor.finishEditing();

    // delete the cloned objects 
    for(std::vector<Part::Geometry *>::iterator it=tempGeo.begin();it!=tempGeo.end();++it)
        if(*it)delete(*it);


#if 0
    double x,y;
    //double x0, y0, dx, dy;
    int i=0;
    //bool Construction;
    // sketchflat generate curves out of entities:
    //SketchFlat->setUpRendering();

    // go through the curves and collect the points
    //int NbrCrv = SketchFlat->nbrOfCurves();
    //int totalPts=0;
    //std::vector<std::vector<Base::Vector3d> > coords(NbrCrv);
    //for (i=0 ; i<NbrCrv;++i) {
    //    SketchFlat->getCurvePoints(coords[i],Construction,i);
    //    totalPts += coords[i].size();
    //}

    std::vector<Base::Vector3d> Coords;
    std::vector<unsigned int> Index;
    std::vector<unsigned int> Color;

    //SketchFlat->getLineSet(Coords,Index,Color);

    CurveSet->numVertices.setNum(Index.size());
    CurvesCoordinate->point.setNum(Coords.size());
    CurvesMaterials->diffuseColor.setNum(Color.size());

    SbVec3f* verts = CurvesCoordinate->point.startEditing();
    int32_t* index = CurveSet->numVertices.startEditing();
    SbColor* color = CurvesMaterials->diffuseColor.startEditing();

    i=0;
    for(std::vector<Base::Vector3d>::const_iterator it=Coords.begin();it!=Coords.end();++it,i++){
        verts[i].setValue(it->x,it->y,0.1f);
    }
    i=0;
    for(std::vector<unsigned int>::const_iterator it=Index.begin();it!=Index.end();++it,i++){
        index[i]=*it;
    }
    i=0;
    for(std::vector<unsigned int>::const_iterator it=Color.begin();it!=Color.end();++it,i++){
        color[i].setValue((*it==1?fCurveConstructionColor:fCurveColor));
    }

    CurvesCoordinate->point.finishEditing();
    CurveSet->numVertices.finishEditing();
    CurvesMaterials->diffuseColor.finishEditing();

    //if (NbrCrv > 0) {
    //    // use the start/finish editing facility for optimization
    //    SbVec3f* verts = CurvesCoordinate->point.startEditing();
    //    int32_t* index = CurveSet->numVertices.startEditing();
    //    SbColor* color = CurvesMaterials->diffuseColor.startEditing();

    //    for (i=0 ; i<NbrCrv;++i) {
    //        const std::vector<Base::Vector3d>& c = coords[i];
    //        for (std::vector<Base::Vector3d>::const_iterator it = c.begin(); it != c.end(); ++it)
    //            verts[r++].setValue(it->x,it->y,0.1f);
    //        index[i] = c.size();
    //        color[i].setValue((Construction?fCurveConstructionColor:fCurveColor));
    //    }
    //    CurvesCoordinate->point.finishEditing();
    //    CurveSet->numVertices.finishEditing();
    //    CurvesMaterials->diffuseColor.finishEditing();
    //}

    //// set up datum lines
    //int NbrLns = SketchFlat->nbrOfLines();
    //LinesCoordinate->point.setNum(2*NbrLns);
    //LinesMaterials->diffuseColor.setNum(NbrLns);
    //LineSet->numVertices.setNum(NbrLns);
    //if (NbrLns > 0) {
    //    // use the start/finish editing facility for optimization
    //    SbVec3f* verts = LinesCoordinate->point.startEditing();
    //    int32_t* index = LineSet->numVertices.startEditing();
    //    SbColor* color = LinesMaterials->diffuseColor.startEditing();
    //    for (i=0; i<NbrLns; ++i) {
    //        SketchFlat->getLine(i, x0, y0, dx, dy);
    //        verts[i*2  ].setValue(x0-dx*50,y0-dy*50,0.1f);
    //        verts[i*2+1].setValue(x0+dx*50,y0+dy*50,0.1f);
    //        index[i] = 2;
    //        color[i].setValue(fDatumLineColor);
    //    }
    //    LinesCoordinate->point.finishEditing();
    //    LineSet->numVertices.finishEditing();
    //    LinesMaterials->diffuseColor.finishEditing();
    //}

    // set up the points
    int NbrPts = SketchFlat->nbrOfPoints();
    PointsCoordinate->point.setNum(NbrPts);
    PointsMaterials->diffuseColor.setNum(NbrPts);
    if (NbrPts > 0) {
        // use the start/finish editing facility for optimization
        SbVec3f* verts = PointsCoordinate->point.startEditing();
        SbColor* color = PointsMaterials->diffuseColor.startEditing();
        for (i=0; i<NbrPts; ++i) {
            //SketchFlat->getPoint(i,x,y);
            verts[i].setValue(x,y,0.2f);
            color[i].setValue(fPointColor);
        }
        if (PreselectPoint >= 0 && PreselectPoint < NbrPts)
            color[PreselectPoint].setValue(fPreselectColor);
        PointsCoordinate->point.finishEditing();
        PointsMaterials->diffuseColor.finishEditing();
    }
#endif
}

void ViewProviderSketch::drawEdit(const std::vector<Base::Vector2D> &EditCurve)
{
    EditCurveSet->numVertices.setNum(1);
    EditCurvesCoordinate->point.setNum(EditCurve.size());
    SbVec3f* verts = EditCurvesCoordinate->point.startEditing();
    int32_t* index = EditCurveSet->numVertices.startEditing();

    int i=0; // setting up the line set
    for(std::vector<Base::Vector2D>::const_iterator it=EditCurve.begin();it!=EditCurve.end();++it,i++)
        verts[i].setValue(it->fX,it->fY,0.1f);

    index[0] = EditCurve.size();
    EditCurvesCoordinate->point.finishEditing();
    EditCurveSet->numVertices.finishEditing();

}


void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProvider2DObject::updateData(prop);

    if(ActSketch && (prop == &(getSketchObject()->Geometry) || &(getSketchObject()->Constraints) )){
        ActSketch->clear();
        ActSketch->addGeometry(getSketchObject()->Geometry.getValues());
        ActSketch->addConstraints(getSketchObject()->Constraints.getValues());
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
    // creat temporary sketch to solve while edeting
    ActSketch = new Sketcher::Sketch();
    // fill up actuall constraints and geometry
    ActSketch->addGeometry(getSketchObject()->Geometry.getValues());
    ActSketch->addConstraints(getSketchObject()->Constraints.getValues());


    createEditInventorNodes();
    this->hide(); // avoid that the wires interfere with the edit lines

	ShowGrid.setValue(true);

    solve();
	draw();

	return true;
}

void ViewProviderSketch::createEditInventorNodes(void)
{
	if (!EditRoot) {
		EditRoot = new SoSeparator;
		pcRoot->addChild(EditRoot);
        EditRoot->renderCaching = SoSeparator::OFF ;
	}

	// stuff for the points ++++++++++++++++++++++++++++++++++++++
    PointsMaterials = new SoMaterial;
	EditRoot->addChild(PointsMaterials);

	SoMaterialBinding *MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_VERTEX;
	EditRoot->addChild(MtlBind);

	PointsCoordinate = new SoCoordinate3;
	EditRoot->addChild(PointsCoordinate);

	SoDrawStyle *DrawStyle = new SoDrawStyle;
	DrawStyle->pointSize = 8;
	EditRoot->addChild( DrawStyle );
    PointSet = new SoPointSet;
	EditRoot->addChild( PointSet );

	// stuff for the lines +++++++++++++++++++++++++++++++++++++++
    LinesMaterials = new SoMaterial;
	EditRoot->addChild(LinesMaterials);

	MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_PART;
	EditRoot->addChild(MtlBind);

	LinesCoordinate = new SoCoordinate3;
	EditRoot->addChild(LinesCoordinate);

	DrawStyle = new SoDrawStyle;
	DrawStyle->lineWidth = 3;
	DrawStyle->linePattern = 0x0fff;
	EditRoot->addChild(DrawStyle);

	LineSet = new SoLineSet;

	EditRoot->addChild(LineSet);

	// stuff for the Curves +++++++++++++++++++++++++++++++++++++++
    CurvesMaterials = new SoMaterial;
	EditRoot->addChild(CurvesMaterials);

	MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_FACE;
	EditRoot->addChild(MtlBind);

	CurvesCoordinate = new SoCoordinate3;
	EditRoot->addChild(CurvesCoordinate);

	DrawStyle = new SoDrawStyle;
	DrawStyle->lineWidth = 3;
	EditRoot->addChild( DrawStyle );

	CurveSet = new SoLineSet;

	EditRoot->addChild( CurveSet );

    // stuff for the EditCurves +++++++++++++++++++++++++++++++++++++++
    EditCurvesMaterials = new SoMaterial;
	EditRoot->addChild(EditCurvesMaterials);

	EditCurvesCoordinate = new SoCoordinate3;
	EditRoot->addChild(EditCurvesCoordinate);

	DrawStyle = new SoDrawStyle;
	DrawStyle->lineWidth = 3;
	EditRoot->addChild( DrawStyle );

	EditCurveSet = new SoLineSet;

	EditRoot->addChild( EditCurveSet );

}

void ViewProviderSketch::unsetEdit(void)
{
	ShowGrid.setValue(false);
 
	// close the solver
	delete(ActSketch);

 //   // recompute the part
 //   getSketchObject()->getDocument()->recompute();

	// empty the nodes
	EditRoot->removeAllChildren();
	PointsMaterials = 0;
	LinesMaterials = 0;
	CurvesMaterials = 0;
	PointsCoordinate = 0;
	LinesCoordinate = 0;
	CurvesCoordinate = 0;
	LineSet = 0;
	CurveSet = 0;
    PointSet = 0;

    PreselectCurve = -1;
    PreselectPoint = -1;
    this->show();
}

Sketcher::SketchObject* ViewProviderSketch::getSketchObject(void)
{
    return dynamic_cast<Sketcher::SketchObject*>(pcObject);
}
