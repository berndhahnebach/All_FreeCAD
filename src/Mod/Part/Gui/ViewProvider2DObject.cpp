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

#include "ViewProvider2DObject.h"


//#include "Tree.h"



using namespace PartGui;
using namespace std;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProvider2DObject, PartGui::ViewProviderPart)

       
ViewProvider2DObject::ViewProvider2DObject()
{
    ADD_PROPERTY(ShowGrid,(true));


    GridRoot = new SoSeparator();
    GridRoot->ref();

    pcRoot->addChild(GridRoot);
 
    sPixmap = "PartFeatureImport";
}

ViewProvider2DObject::~ViewProvider2DObject()
{
  GridRoot->unref();
}



// **********************************************************************************

SoSeparator* ViewProvider2DObject::createGrid(float size, int density) {
  double dx = 10;                       // carpet size
  double dz = 10;
  double dy = 0.02;                     // carpet-grid separation
  int gridsize = 50;                    // grid size

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
        SoLineSet *grid = new SoLineSet;
  vts = new SoVertexProperty;
  grid->vertexProperty = vts;

  int vi=0, l=0;

  // vertical lines
  int i;
  for (i=1; i<gridsize; i++) {
    float h=-0.5*dx + float(i) / gridsize * dx;
    vts->vertex.set1Value(vi++, h, -dy, -0.5*dz);
    vts->vertex.set1Value(vi++, h, -dy,  0.5*dz);
    grid->numVertices.set1Value(l++, 2);
  }

  // horizontal lines
  for (i=1; i<gridsize; i++) {
    float v=-0.5*dz + float(i) / gridsize * dz;
    vts->vertex.set1Value(vi++, -0.5*dx, -dy, v );
    vts->vertex.set1Value(vi++,  0.5*dx, -dy, v );
    grid->numVertices.set1Value(l++, 2);
  }
  parent->addChild(vts);

  parent->addChild(grid);

  return parent;

}

void ViewProvider2DObject::updateData(const App::Property* prop)
{
    ViewProviderPart::updateData(prop);

 
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
}

void ViewProvider2DObject::attach(App::DocumentObject *pcFeat)
{
    ViewProviderPart::attach(pcFeat);

    createGrid();


}

void ViewProvider2DObject::setEdit(void)
{

}

void ViewProvider2DObject::unsetEdit(void)
{

}

