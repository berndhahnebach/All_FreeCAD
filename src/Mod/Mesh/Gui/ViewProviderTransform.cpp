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
#include "ViewProviderTransform.h"

#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/draggers/SoTransformerDragger.h>
#include <Inventor/manips/SoTransformerManip.h> 

using namespace MeshGui;
using Mesh::MeshFeature;
using MeshCore::MeshKernel;
using MeshCore::MeshFacetIterator;
using Base::Vector3D;
    

 
ViewProviderInventorMeshTransform::ViewProviderInventorMeshTransform()
{
}

ViewProviderInventorMeshTransform::~ViewProviderInventorMeshTransform()
{
}


void ViewProviderInventorMeshTransform::selected(Gui::View3DInventorViewer *, SoPath *)
{
   Base::Console().Log("Select viewprovider Mesh  %p\n",this);
   Gui::Selection().addFeature(pcFeature);

   pcHighlight->mode = SoLocateHighlight::ON;
//   pcHighlight->style = SoLocateHighlight::EMISSIVE_DIFFUSE;
   pcHighlight->colorHighlight.setValue((float)0.0,(float)0.3,(float)0.0);

   // switch on dragger
//   pcManipSwitch->whichChild = 0;

 
}
void ViewProviderInventorMeshTransform::unselected(Gui::View3DInventorViewer *, SoPath *)
{
   Base::Console().Log("Unselect viewprovider Mesh  %p\n",this);
   Gui::Selection().removeFeature(pcFeature);

   pcHighlight->mode = SoLocateHighlight::AUTO;
//   pcHighlight->style = SoLocateHighlight::EMISSIVE;
   pcHighlight->colorHighlight.setValue((float)0.1,(float)0.3,(float)0.7);
   
   // switch off dragger
//   pcManipSwitch->whichChild = -1;

}



void ViewProviderInventorMeshTransform::attache(App::Feature *pcFeat)
{
  // creats the satandard viewing modes
  ViewProviderInventorMesh::attache(pcFeat);

//  SoTransformerDragger *pcTransformerDragger = new SoTransformerDragger();
  pcTransformerDragger = new SoTransformerManip();


  SoSeparator* pcEditRoot = new SoSeparator();


  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;

  pcEditRoot->addChild(pcTransformerDragger);
  pcEditRoot->addChild(pcFlatStyle);
  pcEditRoot->addChild(pcShadedMaterial);
  pcEditRoot->addChild(pcBinding);
  pcEditRoot->addChild(pcHighlight);



  // adding to the switch
  pcModeSwitch->addChild(pcEditRoot);

//  setMode(pcFeat->getShowMode());
}

void ViewProviderInventorMeshTransform::updateData(void)
{
  ViewProviderInventorMesh::updateData();
}


void ViewProviderInventorMeshTransform::setMode(const char* ModeName)
{
  ViewProviderInventorMesh::setMode(ModeName);

  int i = getMode();
}


std::vector<std::string> ViewProviderInventorMeshTransform::getModes(void)
{
  std::vector<std::string> StrList = ViewProviderInventorMesh::getModes();

  StrList.push_back("Transform");

  return StrList;
//  return ViewProviderInventorMesh::getModes().push_back("Transform");
}


