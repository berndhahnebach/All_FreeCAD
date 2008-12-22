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
:Mode(0),EditRoot(0)
{
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
    SoMaterial * mat = new SoMaterial;
    mat->diffuseColor.set1Value(0,0, 0, 0);
    mat->diffuseColor.set1Value(1,1, 0, 0);
	EditRoot->addChild(mat);
	SoMaterialBinding *MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_VERTEX;
	EditRoot->addChild(MtlBind);
	SoCoordinate3* coordinate3 = new SoCoordinate3;

	coordinate3->point.set1Value(0,SbVec3f(10,10,0.01));
	coordinate3->point.set1Value(1,SbVec3f(0,0,0.01));

	EditRoot->addChild(coordinate3);

	SoDrawStyle *DrawStyle = new SoDrawStyle;
	DrawStyle->pointSize = 8;
	EditRoot->addChild( DrawStyle );
	EditRoot->addChild( new SoPointSet );

	// stuff for the lines
    mat = new SoMaterial;
    mat->diffuseColor.set1Value(0,0, 0, 0);
    mat->diffuseColor.set1Value(1,1, 0, 0);
	EditRoot->addChild(mat);
	MtlBind = new SoMaterialBinding;
	MtlBind->value = SoMaterialBinding::PER_PART;
	EditRoot->addChild(MtlBind);
	coordinate3 = new SoCoordinate3;

	coordinate3->point.set1Value(0,SbVec3f(10,0,0.01));
	coordinate3->point.set1Value(1,SbVec3f(0,10,0.01));
	coordinate3->point.set1Value(2,SbVec3f(10,5,0.01));
	coordinate3->point.set1Value(3,SbVec3f(0,15,0.01));

	EditRoot->addChild(coordinate3);

	DrawStyle = new SoDrawStyle;
	DrawStyle->pointSize = 8;
	EditRoot->addChild( DrawStyle );

	SoLineSet* lineSet = new SoLineSet;
    lineSet->numVertices.set1Value(0,2);
    lineSet->numVertices.set1Value(1,2);

	EditRoot->addChild( lineSet );


	return true;
}

void ViewProviderSketch::unsetEdit(void)
{
	EditRoot->removeAllChildren();

}

