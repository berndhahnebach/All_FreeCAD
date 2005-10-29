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
# include <Inventor/draggers/SoTrackballDragger.h>
# include <Inventor/nodes/SoAntiSquish.h>
# include <Inventor/nodes/SoSurroundScale.h>
# include <Inventor/nodes/SoTranslation.h>
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
using MeshCore::MeshGeomFacet;
using Base::Vector3D;

#include "ViewProviderTransformDemolding.h"
using namespace MeshGui;



ViewProviderInventorMeshTransformDemolding::ViewProviderInventorMeshTransformDemolding()
{
  pcTrackballDragger = new SoTrackballDragger;
  pcTrackballDragger->ref();
}

ViewProviderInventorMeshTransformDemolding::~ViewProviderInventorMeshTransformDemolding()
{
  pcTrackballDragger->unref();
}


void ViewProviderInventorMeshTransformDemolding::attach(App::Feature *pcFeat)
{
  // creats the satandard viewing modes
  ViewProviderInventorMesh::attach(pcFeat);


  SoGroup* pcColorShadedRoot = new SoGroup();


  // color shaded  ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcColorShadedRoot->addChild(pcFlatStyle);

  // dragger
  SoSeparator * surroundsep = new SoSeparator;

  SoSurroundScale * ss = new SoSurroundScale;
  ss->numNodesUpToReset = 1;
  ss->numNodesUpToContainer = 2;
  surroundsep->addChild(ss);

  SoAntiSquish * antisquish = new SoAntiSquish;
  antisquish->sizing = SoAntiSquish::AVERAGE_DIMENSION ;
  surroundsep->addChild(antisquish);

  pcTrackballDragger->addValueChangedCallback(sValueChangedCallback,this); 
  surroundsep->addChild(pcTrackballDragger);

  pcTransformDrag = new SoTransform();


  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  //pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcMatBinding->value = SoMaterialBinding::PER_FACE_INDEXED;
  pcColorMat = new SoMaterial;
  pcColorMat->diffuseColor.set1Value(0, 1,1,0);
  pcColorMat->diffuseColor.set1Value(1, 1,0,0);
  pcColorMat->diffuseColor.set1Value(2, 0,1,0);
  calcNormalVector();
  calcMaterialIndex(SbRotation());
  
  pcColorShadedRoot->addChild(surroundsep);
  pcColorShadedRoot->addChild(pcTransformDrag);
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
  pcColorShadedRoot->addChild(pcHighlight);

  // adding to the switch
  pcModeSwitch->addChild(pcColorShadedRoot);

  // geting center point
  center = dynamic_cast<MeshFeature*>(pcFeature)->getMesh().getKernel()->GetBoundBox().CalcCenter();

  //SoGetBoundingBoxAction  boxAction;
  //pcHighlight->getBoundingBox(&boxAction);
  //SbVector3f Center = boxAction->getCenter();

}

void ViewProviderInventorMeshTransformDemolding::calcNormalVector(void)
{
  MeshKernel *cMesh = dynamic_cast<MeshFeature*>(pcFeature)->getMesh().getKernel();

  MeshFacetIterator cFIt(*cMesh);
  for( cFIt.Init(); cFIt.More(); cFIt.Next())
  {
    const MeshGeomFacet& rFace = *cFIt;

    Vector3D norm(rFace.GetNormal());
    normalVector.push_back(SbVec3f(norm.x,norm.y,norm.z));
  }

}
void ViewProviderInventorMeshTransformDemolding::calcMaterialIndex(const SbRotation &rot)
{
  // 3.1415926535897932384626433832795
  SbVec3f Up(0,0,1),result;

  unsigned long i=0;
  for( std::vector<SbVec3f>::const_iterator it=normalVector.begin();it != normalVector.end(); ++it,i++)
  {
    rot.multVec(*it,result);

    float Angle = acos( (result.dot(Up)) / (result.length() * Up.length()) ) * (180/3.1415926535);

    if(Angle < 87.0){
      pcMeshFaces->materialIndex .set1Value(i, 2);
    }else if(Angle > 90.0){
      pcMeshFaces->materialIndex .set1Value(i, 1 );
    }else{
      pcMeshFaces->materialIndex .set1Value(i, 0 );
    }

  }

}

void ViewProviderInventorMeshTransformDemolding::sValueChangedCallback(void *This, SoDragger *)
{
  static_cast<ViewProviderInventorMeshTransformDemolding*>(This)->valueChangedCallback();
}

void ViewProviderInventorMeshTransformDemolding::valueChangedCallback(void)
{
  //Base::Console().Log("Value change Callback\n");
  //setTransformation(pcTrackballDragger->getMotionMatrix());
  //pcTransform->rotation = pcTrackballDragger->rotation;
  SbMatrix temp;
  SbRotation rot = pcTrackballDragger->rotation.getValue();

  calcMaterialIndex(rot);

  temp.setTransform( SbVec3f(0,0,0),    // no transformation
                     rot,               // rotation from the dragger
                     SbVec3f(1,1,1),    // no scaling
                     SbRotation() ,     // no scaling oriantation
                     SbVec3f(center.x,center.y,center.z)); // center of rotaion
  pcTransformDrag->setMatrix( temp );
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


