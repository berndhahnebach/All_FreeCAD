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
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoNormal.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
# include <Inventor/manips/SoTransformerManip.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <App/Application.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>
#include <Base/Sequencer.h>


#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/Core/Iterator.h>

using Mesh::MeshFeature;
using MeshCore::MeshKernel;
using MeshCore::MeshFacetIterator;
using Base::Vector3D;

#include "ViewProviderTransformDemolding.h"
using namespace MeshGui;



ViewProviderInventorMeshTransformDemolding::ViewProviderInventorMeshTransformDemolding()
{
  pcTransformerDragger = new SoTransformerManip;
  pcTransformerDragger->ref();
}

ViewProviderInventorMeshTransformDemolding::~ViewProviderInventorMeshTransformDemolding()
{
  pcTransformerDragger->unref();
}


void ViewProviderInventorMeshTransformDemolding::attache(App::Feature *pcFeat)
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
  SoMaterial* pcColorMat = new SoMaterial;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
//  pcColorShadedRoot->addChild(pcBinding);  
  pcColorShadedRoot->addChild(pcHighlight);

  // adding to the switch
  pcModeSwitch->addChild(pcColorShadedRoot);

}




std::vector<std::string> ViewProviderInventorMeshTransformDemolding::getModes(void)
{
  std::vector<std::string> StrList = ViewProviderInventorMesh::getModes();

  StrList.push_back("Demold");

  return StrList;
}



/*
void ViewProviderInventorMeshTransformDemolding::updateData(void)
{
  ViewProviderInventorMesh::updateData();
}

void ViewProviderInventorMeshTransformDemolding::setMode(const char* ModeName)
{
  ViewProviderInventorMesh::setMode(ModeName);

  int i = getMode();
}
*/


