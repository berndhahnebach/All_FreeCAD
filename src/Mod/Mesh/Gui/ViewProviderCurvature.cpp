/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qlistview.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoNormal.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <App/Application.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>
#include <Base/Sequencer.h>

#include "ViewProvider.h"
#include "ViewProviderCurvature.h"

#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/Core/Iterator.h>

using namespace MeshGui;
using namespace std;
using Mesh::MeshFeature;
using MeshCore::MeshKernel;
using MeshCore::MeshFacetIterator;
using Base::Vector3D;
    

 
ViewProviderInventorMeshCurvature::ViewProviderInventorMeshCurvature()
{
}

ViewProviderInventorMeshCurvature::~ViewProviderInventorMeshCurvature()
{
}


void ViewProviderInventorMeshCurvature::attache(App::Feature *pcFeat)
{
  // creats the satandard viewing modes
  ViewProviderInventorMesh::attache(pcFeat);


  SoGroup* pcColorShadedRoot = new SoGroup();


  // color shaded  ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcColorShadedRoot->addChild(pcFlatStyle);

  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcColorMat = new SoMaterial;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
//  pcColorShadedRoot->addChild(pcBinding);  
  pcColorShadedRoot->addChild(pcHighlight);

  Mesh::MeshWithProperty &rcMesh = dynamic_cast<MeshFeature*>(pcFeature)->getMesh();
  list<App::PropertyBag*> s = rcMesh.GetAllOfType("VertexColor");
  
  for(unsigned int i = 0 ;i<s.size();i++)
    pcModeSwitch->addChild(pcColorShadedRoot);

}

void ViewProviderInventorMeshCurvature::updateData(void)
{
  ViewProviderInventorMesh::updateData();
}


void ViewProviderInventorMeshCurvature::setMode(const char* ModeName)
{
  ViewProviderInventorMesh::setMode(ModeName);


  Mesh::MeshWithProperty &rcMesh = dynamic_cast<MeshFeature*>(pcFeature)->getMesh();
  App::PropertyBag *pcProp = 0;

  pcProp = rcMesh.Get(ModeName);
  if ( pcProp && stricmp("VertexColor",pcProp->GetType())==0 )
  {
    SetVertexColorMode(dynamic_cast<Mesh::MeshPropertyColor*>(pcProp));
    pcModeSwitch->whichChild = 4;

  } 

}


std::vector<std::string> ViewProviderInventorMeshCurvature::getModes(void)
{
  std::vector<std::string> StrList = ViewProviderInventorMesh::getModes();

  // add the dynamic modes (depends on which properties are added to the mesh..)
  Mesh::MeshWithProperty &rcMesh = dynamic_cast<MeshFeature*>(pcFeature)->getMesh();
  list<string> List = rcMesh.GetAllNamesOfType("VertexColor");

  for(list<string>::iterator It=List.begin();It!=List.end();It++)
    StrList.push_back(*It);

  return StrList;
//  return ViewProviderInventorMesh::getModes().push_back("Transform");
}


void ViewProviderInventorMeshCurvature::SetVertexColorMode(Mesh::MeshPropertyColor* pcProp)
{
  vector<Mesh::MeshPropertyColor::fColor> color = pcProp->Color;
  for (unsigned long i=0; i<color.size();i++)
  {
    Mesh::MeshPropertyColor::fColor& col = color[i];
    pcColorMat->diffuseColor.set1Value(i, SbColor(col.r, col.g, col.b));
  }
}
