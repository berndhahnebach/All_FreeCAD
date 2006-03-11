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
using Mesh::Feature;
using MeshCore::MeshKernel;
using MeshCore::MeshFacetIterator;
using Base::Vector3D;
    

PROPERTY_SOURCE(MeshGui::ViewProviderMeshTransform, MeshGui::ViewProviderMesh)

 
ViewProviderMeshTransform::ViewProviderMeshTransform()
{
  pcTransformerDragger = new SoTransformerManip();
  pcTransformerDragger->ref();
}

ViewProviderMeshTransform::~ViewProviderMeshTransform()
{
  pcTransformerDragger->unref();
}

void ViewProviderMeshTransform::attach(App::AbstractFeature *pcFeat)
{
  SoSeparator* pcEditRoot = new SoSeparator();

  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;

  pcEditRoot->addChild(pcTransformerDragger);
  pcEditRoot->addChild(pcFlatStyle);
  pcEditRoot->addChild(pcSolidMaterial);
  pcEditRoot->addChild(pcBinding);
  pcEditRoot->addChild(pcHighlight);

  // adding to the switch
  addDisplayMode(pcEditRoot, "Edit");

  // creats the standard viewing modes
  ViewProviderMesh::attach(pcFeat);
}

void ViewProviderMeshTransform::updateData(void)
{
  ViewProviderMesh::updateData();
}

void ViewProviderMeshTransform::setMode(const char* ModeName)
{
  if ( strcmp("Transform",ModeName) == 0 )
    setDisplayMode("Edit");
  ViewProviderMesh::setMode(ModeName);
}

std::vector<std::string> ViewProviderMeshTransform::getModes(void)
{
  std::vector<std::string> StrList = ViewProviderMesh::getModes();
  StrList.push_back("Transform");
  return StrList;
}


