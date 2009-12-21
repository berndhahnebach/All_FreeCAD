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
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>

#include <Mod/Sketcher/App/SketchFlatInterface.h>
#include <Mod/Sketcher/App/SketchObjectSF.h>


#include "ViewProviderSketch.h"


using namespace SketcherGui;
using namespace Sketcher;

const float fCurveColor[] =     {1.0f,1.0f,1.0f}; 
const float fCurveConstructionColor[] = {0.2f,1.0f,0.2f}; 
const float fPointColor[] =             {0.9f,0.9f,0.9f}; 
const float fPreselectColor[] = {0.8f,0.0f,0.0f}; 
const float fSelectColor[] =    {1.0f,0.0f,0.0f}; 
const float fDatumLineColor[] = {0.0f,0.8f,0.0f}; 

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProvider2DObject)


ViewProviderSketch::ViewProviderSketch()
  : Mode(STATUS_NONE),DragPoint(-1),EditRoot(0)
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

    sPixmap = "Sketcher_NewSketch";
}

ViewProviderSketch::~ViewProviderSketch()
{

}


// **********************************************************************************

void ViewProviderSketch::setSketchMode(int mode)
{
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

void ViewProviderSketch::getCoordsOnSketchPlane(double &u, double &v,const SbVec3f &point, const SbVec3f &normal)
{
	// Plane form
	Base::Vector3d R0(0,0,0),RN(0,0,1),RX(1,0,0),RY(0,1,0);

    // move to position of Sketch
    Base::Placement Plz = getSketchObjectSF()->Placement.getValue();
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
                        this->DragPoint = SketchFlat->getPoint(PreselectPoint);
						Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
					    SketchFlat->forcePoint(this->DragPoint,x,y);
					    Mode = STATUS_SKETCH_DragPoint;
                        return true;
                    } else
                        return false;

				case STATUS_SKETCH_CreateArc:
				case STATUS_SKETCH_CreateCircle:
				case STATUS_SKETCH_CreateRectangle:
				case STATUS_SKETCH_CreateText:
					return true;
				case STATUS_SKETCH_CreateLine:
					this->DragPoint = SketchFlat->addLine(x,y);
					SketchFlat->forcePoint(this->DragPoint,x,y);
					Mode = STATUS_SKETCH_DoLine;
					draw();
					return true;
				case STATUS_SKETCH_DoLine:
					SketchFlat->forcePoint(this->DragPoint,x,y);
					SketchFlat->solve();
					draw();
					Base::Console().Log("Finish line, point:%d\n",this->DragPoint);
                    this->DragPoint = -1;
					Mode = STATUS_NONE;
					return true;
				case STATUS_SKETCH_CreatePolyline:
					this->DragPoint = SketchFlat->addLine(x,y);
					SketchFlat->forcePoint(this->DragPoint,x,y);
					Mode = STATUS_SKETCH_DoPolyline;
					draw();
					return true;
				case STATUS_SKETCH_DoPolyline:
					SketchFlat->forcePoint(this->DragPoint,x,y);
					SketchFlat->solve();
					this->DragPoint = SketchFlat->addLine(x,y);
					SketchFlat->forcePoint(this->DragPoint,x,y);
					draw();
					return true;
                default:
                    return false;
					
			}
        }
        else {
			// Do things depending on the mode of the user interaction
			switch (Mode) {
				case STATUS_SKETCH_DragPoint:
					SketchFlat->forcePoint(this->DragPoint,x,y);
					SketchFlat->solve();
					draw();
                    this->DragPoint = -1;
					Mode = STATUS_NONE;
					return true;
                default:
                    return false;
            }
        }
	}
    // Right mouse button ****************************************************
    else if (Button == 2) {
        if (pressed) {
            switch(Mode){
                case STATUS_SKETCH_DoPolyline:
                    SketchFlat->forcePoint(this->DragPoint,x,y);
                    SketchFlat->solve();
                    draw();
                    Base::Console().Log("Finish polyline, point:%d\n",this->DragPoint);
                    this->DragPoint = -1;
                    Mode = STATUS_NONE;
                    return true;
                default:
                    return false;
            }
        }
    }

	return false;
}

bool ViewProviderSketch::mouseMove(const SbVec3f &point, const SbVec3f &normal, const SoPickedPoint* pp)
{
	double x,y;
	getCoordsOnSketchPlane(x,y,point,normal);

    handlePreselection(pp);

	switch (Mode) {
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
		case STATUS_SKETCH_DoPolyline:
		case STATUS_SKETCH_DragPoint:
			SketchFlat->forcePoint(this->DragPoint,x,y);
			SketchFlat->solve();
			draw();
			return true;
	}

	return false;
}

bool ViewProviderSketch::handlePreselection(const SoPickedPoint* Point)
{
    if (Point) {
        Base::Console().Log("Point pick\n");
        const SoDetail* point_detail = Point->getDetail(this->PointSet);
        if (point_detail && point_detail->getTypeId() == SoPointDetail::getClassTypeId()) {
            // get the index
            int idx = static_cast<const SoPointDetail*>(point_detail)->getCoordinateIndex();
            if (PreselectPoint != idx) {
                PointsMaterials->diffuseColor.set1Value(idx,fPreselectColor);
                if (PreselectPoint >= 0)
                    PointsMaterials->diffuseColor.set1Value(PreselectPoint,fPointColor);
                PreselectPoint = idx;
                if (PreselectCurve >= 0)
                    CurvesMaterials->diffuseColor.set1Value(PreselectCurve,PreselectOldColor);
                PreselectCurve = -1;
            }

            Base::Console().Log("Point pick%d\n",idx);
            return true;
        }

        const SoDetail* curve_detail = Point->getDetail(this->CurveSet);
        if (curve_detail && curve_detail->getTypeId() == SoLineDetail::getClassTypeId()) {
            // get the index
            int idx = static_cast<const SoLineDetail*>(curve_detail)->getLineIndex();
            if (PreselectCurve != idx) {
                if (PreselectCurve >= 0)
                    CurvesMaterials->diffuseColor.set1Value(PreselectCurve,PreselectOldColor);
				PreselectOldColor = CurvesMaterials->diffuseColor[idx];
                CurvesMaterials->diffuseColor.set1Value(idx,fPreselectColor);
                PreselectCurve = idx;
                if (PreselectPoint >= 0)
                    PointsMaterials->diffuseColor.set1Value(PreselectPoint,fPointColor);
                PreselectPoint = -1;
            }
            
            Base::Console().Log("Curve pick%d\n",idx);
            return true;
        }

        const SoDetail* datum_detail = Point->getDetail(this->LineSet);
        if (datum_detail && datum_detail->getTypeId() == SoLineDetail::getClassTypeId()) {
            // get the index
            unsigned long idx = static_cast<const SoLineDetail*>(datum_detail)->getPartIndex();
            Base::Console().Log("Datum pick%d\n",idx);
            return true;
        }
    }

    if (PreselectCurve >= 0)
        CurvesMaterials->diffuseColor.set1Value(PreselectCurve,PreselectOldColor);
    PreselectCurve = -1;
    if (PreselectPoint >= 0)
        PointsMaterials->diffuseColor.set1Value(PreselectPoint,fPointColor);
    PreselectPoint = -1;

    return false;
}

bool ViewProviderSketch::isPointOnSketch(const SoPickedPoint* pp) const
{
    // checks if we picked a point on the sketch or any other nodes like the grid
    SoPath* path = pp->getPath();
    return path->containsNode(EditRoot);
}

bool ViewProviderSketch::doubleClicked(void)
{

    Sketcher::SketchObjectSF *obj = static_cast<Sketcher::SketchObjectSF *>(getObject());

	Gui::Dialog::DlgEditFileIncludePropertyExternal dlg((obj->SketchFlatFile),Gui::getMainWindow());

	dlg.ProcName = QString::fromUtf8((App::Application::Config()["AppHomePath"] + "bin/sketchflat.exe").c_str());

    if(dlg.Do() == 1)
        App::GetApplication().getActiveDocument()->recompute();

	//Gui::Application::Instance->activeDocument()->setEdit(this);
	return true;
}

void ViewProviderSketch::draw(void)
{
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

    SketchFlat->getLineSet(Coords,Index,Color);

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
            SketchFlat->getPoint(i,x,y);
            verts[i].setValue(x,y,0.2f);
            color[i].setValue(fPointColor);
        }
        if (PreselectPoint >= 0 && PreselectPoint < NbrPts)
            color[PreselectPoint].setValue(fPreselectColor);
        PointsCoordinate->point.finishEditing();
        PointsMaterials->diffuseColor.finishEditing();
    }
}

void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProvider2DObject::updateData(prop);
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


 //   if (SketchFlatInterface::isAlive()) {
 //       QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Cannot edit sketch"),
 //           QObject::tr("Sketch cannot be edited because there is already another sketch in edit mode.\n"
 //           "Please leave the edit mode for the other sketch to enter edit mode for this sketch."));
 //       return false;
 //   }

 //   // interface to the solver
	//SketchFlat = new SketchFlatInterface();

 //   // insert the SketchFlat file
 //   SketchFlat->load(getSketchObjectSF()->SketchFlatFile.getValue());

 //   createEditInventorNodes();
 //   this->hide(); // avoid that the wires interfere with the edit lines

	//ShowGrid.setValue(true);

 //   SketchFlat->solve();
	//draw();

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
}

void ViewProviderSketch::unsetEdit(void)
{
	//ShowGrid.setValue(false);

 //   std::string file;

 //   // save the result of editing
 //   if (std::string(getSketchObjectSF()->SketchFlatFile.getValue())=="") {
 //       // make a meaningfull name
 //       Base::FileInfo temp(getSketchObjectSF()->SketchFlatFile.getDocTransientPath()
 //                           + "/" + getSketchObjectSF()->getNameInDocument() + ".skf");
 //       if (temp.exists())
 //           // save under save name
 //           file = Base::FileInfo::getTempFileName("Sketch.skf",getSketchObjectSF()
 //                           ->SketchFlatFile.getDocTransientPath().c_str());
 //       else
 //           file = temp.filePath();
 //   }
 //   else {
 //       // save under old name
 //       file = getSketchObjectSF()->SketchFlatFile.getExchangeTempFile();
 //   }

 //   // save the sketch and set the property
 //   SketchFlat->save(file.c_str());
 //   getSketchObjectSF()->SketchFlatFile.setValue(file.c_str());
 //   getSketchObjectSF()->touch();

	//// close the solver
	//delete(SketchFlat);

 //   // recompute the part
 //   getSketchObjectSF()->getDocument()->recompute();

	//// empty the nodes
	//EditRoot->removeAllChildren();
	//PointsMaterials = 0;
	//LinesMaterials = 0;
	//CurvesMaterials = 0;
	//PointsCoordinate = 0;
	//LinesCoordinate = 0;
	//CurvesCoordinate = 0;
	//LineSet = 0;
	//CurveSet = 0;
 //   PointSet = 0;

 //   PreselectCurve = -1;
 //   PreselectPoint = -1;
 //   this->show();
}

Sketcher::SketchObjectSF* ViewProviderSketch::getSketchObjectSF(void)
{
    return dynamic_cast<Sketcher::SketchObjectSF*>(pcObject);
}
