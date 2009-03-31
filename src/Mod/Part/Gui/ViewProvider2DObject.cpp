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
# include <Inventor/nodes/SoPickStyle.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoVertexProperty.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>

#include "ViewProvider2DObject.h"
#include <Mod/Part/App/PartFeature.h>


//#include "Tree.h"



using namespace PartGui;
using namespace std;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProvider2DObject, PartGui::ViewProviderPart)

       
ViewProvider2DObject::ViewProvider2DObject()
{
    ADD_PROPERTY_TYPE(ShowGrid,(false),"Grid",(App::PropertyType)(App::Prop_None),"Switch the grid on/off");
    ADD_PROPERTY_TYPE(GridSize,(10),"Grid",(App::PropertyType)(App::Prop_None),"Gap size of the grid");

    GridRoot = new SoSeparator();
    GridRoot->ref();
	MinX = MinY = -100;
	MaxX = MaxY = 100;

    pcRoot->addChild(GridRoot);
 
    sPixmap = "PartFeatureImport";
}

ViewProvider2DObject::~ViewProvider2DObject()
{
  GridRoot->unref();
}



// **********************************************************************************

SoSeparator* ViewProvider2DObject::createGrid(void) {
  //double dx = 10 * GridSize.getValue();                       // carpet size
  //double dy = 10 * GridSize.getValue();
	float Size = (MaxX-MinX > MaxY-MinY)?MaxX-MinX: MaxY-MinY;
	float Step = pow(10,floor(log10(Size/5.0)));
	float MiX = MinX - (MaxX-MinX)*0.5;
	float MaX = MaxX + (MaxX-MinX)*0.5;
	float MiY = MinY - (MaxY-MinY)*0.5;
	float MaY = MaxY + (MaxY-MinY)*0.5;
	//float Step = 10.0;
  double dz = 0.0;                     // carpet-grid separation
  int gridsize = 20;                    // grid size

  SoSeparator *parent = GridRoot;
  GridRoot->removeAllChildren();
  SoBaseColor *mycolor;
  SoVertexProperty *vts;

  // carpet
 /* mycolor = new SoBaseColor;
  mycolor->rgb.setValue(0.2f, 0.7f, 0.7f);
  parent->addChild(mycolor);

  vts = new SoVertexProperty;
  vts->vertex.set1Value(0, -0.5*dx, -1.5*dy,  0.5*dz);
  vts->vertex.set1Value(1, -0.5*dx, -1.5*dy, -0.5*dz);
  vts->vertex.set1Value(2,  0.5*dx, -1.5*dy,  0.5*dz);
  vts->vertex.set1Value(3,  0.5*dx, -1.5*dy, -0.5*dz);

  SoQuadMesh *carpet = new SoQuadMesh;
  carpet->verticesPerColumn = 2;
  carpet->verticesPerRow = 2;
  carpet->vertexProperty = vts;
  parent->addChild(carpet);*/

  // gridlines
  mycolor = new SoBaseColor;

  mycolor->rgb.setValue(0.7f, 0.7f ,0.7f);
  parent->addChild(mycolor);

  SoDrawStyle* DrawStyle = new SoDrawStyle;
  DrawStyle->lineWidth = 1;
  DrawStyle->linePattern = 0x0f0f;
  parent->addChild(DrawStyle);

  SoPickStyle* PickStyle = new SoPickStyle;
  PickStyle->style = SoPickStyle::UNPICKABLE;
  parent->addChild(PickStyle);

  SoLineSet *grid = new SoLineSet;
  vts = new SoVertexProperty;
  grid->vertexProperty = vts;

  int vi=0, l=0;

  // vertical lines
  float i;
  for (i=MiX; i<MaX; i+=Step) {
    /*float h=-0.5*dx + float(i) / gridsize * dx;*/
    vts->vertex.set1Value(vi++, i, MiY, dz);
    vts->vertex.set1Value(vi++, i,  MaY, dz);
    grid->numVertices.set1Value(l++, 2);
  }

  // horizontal lines
  for (i=MiY; i<MaY; i+=Step) {
    //float v=-0.5*dy + float(i) / gridsize * dy;
    vts->vertex.set1Value(vi++, MiX, i, dz );
    vts->vertex.set1Value(vi++,  MaX, i, dz );
    grid->numVertices.set1Value(l++, 2);
  }
  parent->addChild(vts);

  parent->addChild(grid);

  return parent;

}

void ViewProvider2DObject::updateData(const App::Property* prop)
{
    ViewProviderPart::updateData(prop);

    if (prop->getTypeId() == Part::PropertyPartShape::getClassTypeId()) {
        Base::BoundBox3d Bnd = static_cast<const Part::PropertyPartShape*>(prop)->getBoundingBox();
		GridRoot->removeAllChildren();
		MinX = Bnd.MinX;
		MaxX = Bnd.MaxX;
		MinY = Bnd.MinY;
		MaxY = Bnd.MaxY;
		if(ShowGrid.getValue()){
			GridRoot->removeAllChildren();
			createGrid();
		}
	}
 
}

void ViewProvider2DObject::onChanged(const App::Property* prop)
{
    // call father
    ViewProviderPart::onChanged(prop);

    if (prop == &ShowGrid) {
        if(ShowGrid.getValue())
            createGrid();
        else
            GridRoot->removeAllChildren();
    }
    if (prop == &GridSize) {
        if(ShowGrid.getValue()){
            GridRoot->removeAllChildren();
            createGrid();
        }
    }
}

void ViewProvider2DObject::attach(App::DocumentObject *pcFeat)
{
    ViewProviderPart::attach(pcFeat);

    if(ShowGrid.getValue())
        createGrid();


}

void ViewProvider2DObject::setEdit(void)
{

}

void ViewProvider2DObject::unsetEdit(void)
{

}

