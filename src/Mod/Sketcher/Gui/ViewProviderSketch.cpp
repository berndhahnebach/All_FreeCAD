/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoVertexProperty.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/Document.h>

#include <Mod/Sketcher/App/SketchFlatInterface.h>
#include <Mod/Sketcher/App/SketchObject.h>

#include "ViewProviderSketch.h"


//#include "Tree.h"



using namespace SketcherGui;
using namespace Sketcher;
using namespace std;

const float fCurveColor[] =     {1.0f,1.0f,1.0f}; 
const float fPointColor[] =     {0.9f,0.9f,0.9f}; 
const float fPreselectColor[] = {0.8f,0.0f,0.0f}; 
const float fSelectColor[] =    {1.0f,0.0f,0.0f}; 
const float fDatumLineColor[] = {0.0f,0.8f,0.0f}; 

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProvider2DObject)

       
ViewProviderSketch::ViewProviderSketch()
:Mode(STATUS_NONE),EditRoot(0),DraggPoint(-1)
{
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

 /*   ADD_PROPERTY(ShowGrid,(true));
*/
 
    sPixmap = "Sketcher_NewSketch";
}

ViewProviderSketch::~ViewProviderSketch()
{

}



// **********************************************************************************

void ViewProviderSketch::setSketchMode(int mode)
{
	//ShowGrid.setValue(false);
	Mode = mode;
}

bool ViewProviderSketch::setConstrainOnSelected(int Constrain)
{
	return false;
}

bool ViewProviderSketch::keyPressed(int key)
{
	return true;
}



void ViewProviderSketch::CoordsOnSketchPlane(double &u, double &v,const Base::Vector3f &pNear, const Base::Vector3f &pFar)
{
	// Plane form
	Base::Vector3d R0(0,0,0),RN(0,0,1),RX(1,0,0),RY(0,1,0);

    // move to position of Sketch
    const Base::Placement Plz = getSketchObject()->Placement.getValue();
    R0 = Plz._pos ; 
    Plz._rot.multVec(RN,RN);
    Plz._rot.multVec(RX,RX);
    Plz._rot.multVec(RY,RY);

	// line 
	Base::Vector3f r2 = pNear - pFar;
	Base::Vector3d R1(pNear .x,pNear .y,pNear .z),RA(r2.x,r2.y,r2.z);
	// intersection point on plane
	Base::Vector3d S = R1 + ((RN * (R0-R1))/(RN*RA))*RA;

	// distance to x Axle of the sketch
	S.TransformToCoordinateSystem(R0,RX,RY);

	u = S.x;
	v = S.y;
}

bool ViewProviderSketch::mouseButtonPressed(int Button, bool pressed, const Base::Vector3f &pNear, const Base::Vector3f &pFar, SoPickedPoint* Point)
{
	double x,y;
	CoordsOnSketchPlane(x,y,pNear,pFar);

	// Left Mouse button ****************************************************
	if(Button == 1){
		if(pressed){
			// Do things depending on the mode of the user interaktion
			switch(Mode){
				case STATUS_NONE:
                    if(PreselectPoint >=0){
                        DraggPoint = SketchFlat->getPoint(PreselectPoint);
						Base::Console().Log("start dragging, point:%d\n",DraggPoint);
					    SketchFlat->forcePoint(DraggPoint,x,y);
					    Mode = STATUS_SKETCH_DraggPoint;
                        return true;
                    }else
                        return false;

				case STATUS_SKETCH_CreateArc:
				case STATUS_SKETCH_CreateCircle:
				case STATUS_SKETCH_CreatePolyline:
				case STATUS_SKETCH_CreateRectangle:
				case STATUS_SKETCH_CreateText:
					return true;
				case STATUS_SKETCH_CreateLine:
					DraggPoint = SketchFlat->addLine(x,y);
					SketchFlat->forcePoint(DraggPoint,x,y);
					Mode = STATUS_SKETCH_DoLine;
					draw();
					return true;
				case STATUS_SKETCH_DoLine:
					SketchFlat->forcePoint(DraggPoint,x,y);
					SketchFlat->solve();
					draw();
					Base::Console().Log("Finish line, point:%d\n",DraggPoint);
                    DraggPoint = -1;
					Mode = STATUS_NONE;
					return true;
                default:
                    return false;
					
			}
        }else{
			// Do things depending on the mode of the user interaktion
			switch(Mode){
				case STATUS_SKETCH_DraggPoint:
					SketchFlat->forcePoint(DraggPoint,x,y);
					SketchFlat->solve();
					draw();
                    DraggPoint = -1;
					Mode = STATUS_NONE;
					return true;
                default:
                    return false;
            }

        }


	}
	return false;
}

bool ViewProviderSketch::mouseMove(const Base::Vector3f &pNear, const Base::Vector3f &pFar, SoPickedPoint* Point)
{
	double x,y;
	CoordsOnSketchPlane(x,y,pNear,pFar);

    HandlePreselection(Point);

	switch(Mode){
		case STATUS_NONE:
			return false;
		case STATUS_SKETCH_CreateArc:
		case STATUS_SKETCH_CreateCircle:
		case STATUS_SKETCH_CreatePolyline:
		case STATUS_SKETCH_CreateRectangle:
		case STATUS_SKETCH_CreateText:
		case STATUS_SKETCH_CreateLine:
			return true;
		case STATUS_SKETCH_DoLine:
		case STATUS_SKETCH_DraggPoint:
			SketchFlat->forcePoint(DraggPoint,x,y);
			SketchFlat->solve();
			draw();
			return true;
			
	}

	return false;
}

bool ViewProviderSketch::HandlePreselection(SoPickedPoint* Point)
{
    if(Point){
        //Base::Console().Log("Point pick\n");
        const SoDetail* detail = Point->getDetail(PointSet);
        if ( detail && detail->getTypeId() == SoPointDetail::getClassTypeId() ) {
            // get the index
            unsigned long idx = ((SoPointDetail*)detail)->getCoordinateIndex();
            if(PreselectPoint != idx){
                PointsMaterials->diffuseColor.set1Value(idx,fPreselectColor);
                if(PreselectPoint >= 0)
                    PointsMaterials->diffuseColor.set1Value(PreselectPoint,fPointColor);
                PreselectPoint = idx;
                if(PreselectCurve >= 0)
                    CurvesMaterials->diffuseColor.set1Value(PreselectCurve,fCurveColor);
                PreselectCurve = -1;
            }
            //Base::Console().Log("Point pick%d\n",idx);
            return true;
        }else {
            // details from the Curves
            const SoDetail* detail = Point->getDetail(CurveSet);
            if ( detail && detail->getTypeId() == SoLineDetail::getClassTypeId() ) {
                // get the index
                unsigned long idx = ((SoLineDetail*)detail)->getPartIndex();
                if(PreselectCurve != idx){
                    CurvesMaterials->diffuseColor.set1Value(idx,fPreselectColor);
                    if(PreselectCurve >= 0)
                        CurvesMaterials->diffuseColor.set1Value(PreselectCurve,fCurveColor);
                    PreselectCurve = idx;
                    if(PreselectPoint >= 0)
                        PointsMaterials->diffuseColor.set1Value(PreselectPoint,fPointColor);
                    PreselectPoint = -1;
                }
                
                //Base::Console().Log("Curve pick%d\n",idx);
                return true;
            }else {
                // details from the Datum lines
                const SoDetail* detail = Point->getDetail(LineSet);
                if ( detail && detail->getTypeId() == SoLineDetail::getClassTypeId() ) {
                    // get the index
                    unsigned long idx = ((SoLineDetail*)detail)->getPartIndex();
                    Base::Console().Log("Datum pick%d\n",idx);
                    return true;
                }
            }
        }
    }
    if(PreselectCurve >= 0)
       CurvesMaterials->diffuseColor.set1Value(PreselectCurve,fCurveColor);
    PreselectCurve = -1;
    if(PreselectPoint >= 0)
       PointsMaterials->diffuseColor.set1Value(PreselectPoint,fPointColor);
    PreselectPoint = -1;

    return false;
}

bool ViewProviderSketch::doubleClicked(void)
{
	Gui::Application::Instance->activeDocument()->setEdit(this);
	return true;
}

void ViewProviderSketch::draw(void)
{
	std::vector<Base::Vector3d> coords;
	double x,y;
	double x0, y0, dx, dy;
    int i=0,l=0,r=0;
	// sketchflat generate curves out of enteties:
	SketchFlat->setUpRendering();

	// go throug the curves and collect the points
	int Nbr = SketchFlat->nbrOfCurves();
	for( i=0 ; i<Nbr;++i){
		SketchFlat->getCurvePoints(coords,i);
        std::vector<Base::Vector3d>::const_iterator it=coords.begin();
	    while(it!=coords.end()){
		    CurvesCoordinate->point.set1Value(r,SbVec3f(it->x,it->y,0.0f));
		    ++it;r++;
		    CurvesCoordinate->point.set1Value(r,SbVec3f(it->x,it->y,0.0f));
            ++it;r++;
	    }
		CurveSet->numVertices.set1Value(i,coords.size());
		CurvesMaterials->diffuseColor.set1Value(i,fCurveColor);
        coords.clear();
	}

    // set up datum lines
	for(i=0 ; i<SketchFlat->nbrOfLines();++i){
		LinesMaterials->diffuseColor.set1Value(i,fDatumLineColor);
		SketchFlat->getLine(i, x0, y0, dx, dy);
		LinesCoordinate->point.set1Value(i*2  ,SbVec3f(x0-dx*5,y0-dy*5,0.0f));
		LinesCoordinate->point.set1Value(i*2+1,SbVec3f(x0+dx*5,y0+dy*5,0.0f));
		LineSet->numVertices.set1Value(i,2);
	}


	// set up the points
	for(i=0 ; i<SketchFlat->nbrOfPoints();++i){
		PointsMaterials->diffuseColor.set1Value(i,fPointColor);
		SketchFlat->getPoint(i,x,y);
		PointsCoordinate->point.set1Value(i  ,SbVec3f(x,y,0.0f));
	}
}
void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProviderPart::updateData(prop);

 
}

void ViewProviderSketch::onChanged(const App::Property* prop)
{
    // call father
    PartGui::ViewProvider2DObject::onChanged(prop);

 /*   if (prop == &ShowGrid) {
        if(ShowGrid.getValue())
            createGrid();
        else
            GridRoot->removeAllChildren();
    }*/
}

void ViewProviderSketch::attach(App::DocumentObject *pcFeat)
{
    ViewProviderPart::attach(pcFeat);



} 

bool ViewProviderSketch::setEdit(int ModNum)
{
    // interface to the solver
	SketchFlat = new SketchFlatInterface();

    // insert the SketchFlat file
    SketchFlat->load(getSketchObject()->SketchFlatFile.getValue());

    createEditInventorNodes();

	ShowGrid.setValue(true);

    SketchFlat->solve();
	draw();

	return true;
}

void ViewProviderSketch::createEditInventorNodes(void)
{
	if(!EditRoot){
		EditRoot = new SoSeparator;
		pcRoot->addChild(EditRoot);
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
	DrawStyle->pointSize = 6;
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
	EditRoot->addChild( DrawStyle );

	LineSet = new SoLineSet;

	EditRoot->addChild( LineSet );

	// stuff for the Curves +++++++++++++++++++++++++++++++++++++++
    CurvesMaterials = new SoMaterial;
	EditRoot->addChild(CurvesMaterials);

	MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_PART;
	EditRoot->addChild(MtlBind);

	CurvesCoordinate = new SoCoordinate3;
	EditRoot->addChild(CurvesCoordinate);

	DrawStyle = new SoDrawStyle;
	DrawStyle->lineWidth = 3;
	EditRoot->addChild( DrawStyle );

	CurveSet = new SoLineSet;

	EditRoot->addChild( CurveSet );

}

void ViewProviderSketch::unsetEdit(void)
{
	ShowGrid.setValue(false);

    std::string file;

    // save the result of editing
    if(std::string(getSketchObject()->SketchFlatFile.getValue())==""){
        // make a meaningfull name
        Base::FileInfo temp(getSketchObject()->SketchFlatFile.getDocTransientPath() + "/" + getSketchObject()->getNameInDocument() + ".skf");
        if(temp.exists())
            // save under save name
            file = Base::FileInfo::getTempFileName("Sketch.skf",getSketchObject()->SketchFlatFile.getDocTransientPath().c_str());
        else
            file = temp.filePath();
    }else{
        // save under old name
        file = getSketchObject()->SketchFlatFile.getExchangeTempFile();
    }

    // save the sketch and set the property
    SketchFlat->save(file.c_str());
    getSketchObject()->SketchFlatFile.setValue(file.c_str());

	// close the solver
	delete(SketchFlat);

    // recompute the part
    getSketchObject()->getDocument()->recompute();


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

}

Sketcher::SketchObject* ViewProviderSketch::getSketchObject(void)
{
    return dynamic_cast<Sketcher::SketchObject*>(pcObject);
}
