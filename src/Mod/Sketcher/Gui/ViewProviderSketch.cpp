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
#include <Gui/Application.h>
#include <Gui/Document.h>

#include "ViewProviderSketch.h"


//#include "Tree.h"



using namespace SketcherGui;
using namespace std;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProvider2DObject)

       
ViewProviderSketch::ViewProviderSketch()
:Mode(STATUS_NONE),EditRoot(0)
{
	PointsMaterials = 0;
	LinesMaterials = 0;
	PointsCoordinate = 0;
	LinesCoordinate = 0;
	LineSet = 0;

 /*   ADD_PROPERTY(ShowGrid,(true));


    GridRoot = new SoSeparator();
    GridRoot->ref();

    pcRoot->addChild(GridRoot);*/
 
    sPixmap = "Sketcher_NewSketch";
}

ViewProviderSketch::~ViewProviderSketch()
{
  //GridRoot->unref();
}



// **********************************************************************************

void ViewProviderSketch::setSketchMode(int mode)
{
	Mode = mode;
}



bool ViewProviderSketch::mouseMove(const Base::Vector3f &pos, const Base::Vector3f &norm)
{
	return true;
}

bool ViewProviderSketch::keyPressed(int key)
{
	return true;
}


bool ViewProviderSketch::mouseButtonPressed(int Button, bool pressed, const Base::Vector3f &pos, const Base::Vector3f &norm)
{
	return true;
}


bool ViewProviderSketch::doubleClicked(void)
{
	Gui::Application::Instance->activeDocument()->setEdit((ViewProvider*)this);
	return true;
}

void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProviderPart::updateData(prop);

 
}

void ViewProviderSketch::onChanged(const App::Property* prop)
{
    // call father
    ViewProviderPart::onChanged(prop);

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

    createGrid();


} 

bool ViewProviderSketch::setEdit(int ModNum)
{
	if(!EditRoot){
		EditRoot = new SoSeparator;
		pcRoot->addChild(EditRoot);
	}

	// stuff for the points
    SoMaterial * PointsMaterials = new SoMaterial;
    PointsMaterials->diffuseColor.set1Value(0,0, 0, 0);
    PointsMaterials->diffuseColor.set1Value(1,1, 0, 0);
	EditRoot->addChild(PointsMaterials);

	SoMaterialBinding *MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_VERTEX;
	EditRoot->addChild(MtlBind);

	SoCoordinate3* PointsCoordinate = new SoCoordinate3;
	PointsCoordinate->point.set1Value(0,SbVec3f(10,10,0.01));
	PointsCoordinate->point.set1Value(1,SbVec3f(0,0,0.01));
	EditRoot->addChild(PointsCoordinate);

	SoDrawStyle *DrawStyle = new SoDrawStyle;
	DrawStyle->pointSize = 8;
	EditRoot->addChild( DrawStyle );
	EditRoot->addChild( new SoPointSet );

	// stuff for the lines
    LinesMaterials = new SoMaterial;
    LinesMaterials->diffuseColor.set1Value(0,0, 0, 0);
    LinesMaterials->diffuseColor.set1Value(1,1, 0, 0);
	EditRoot->addChild(LinesMaterials);

	MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_PART;
	EditRoot->addChild(MtlBind);

	LinesCoordinate = new SoCoordinate3;
	LinesCoordinate->point.set1Value(0,SbVec3f(10,0,0.01));
	LinesCoordinate->point.set1Value(1,SbVec3f(0,10,0.01));
	LinesCoordinate->point.set1Value(2,SbVec3f(10,5,0.01));
	LinesCoordinate->point.set1Value(3,SbVec3f(0,15,0.01));
	EditRoot->addChild(LinesCoordinate);

	DrawStyle = new SoDrawStyle;
	DrawStyle->pointSize = 8;
	EditRoot->addChild( DrawStyle );

	LineSet = new SoLineSet;
    LineSet->numVertices.set1Value(0,2);
    LineSet->numVertices.set1Value(1,2);

	EditRoot->addChild( LineSet );


	return true;
}

void ViewProviderSketch::unsetEdit(void)
{
	EditRoot->removeAllChildren();
	PointsMaterials = 0;
	LinesMaterials = 0;
	PointsCoordinate = 0;
	LinesCoordinate = 0;
	LineSet = 0;
}

