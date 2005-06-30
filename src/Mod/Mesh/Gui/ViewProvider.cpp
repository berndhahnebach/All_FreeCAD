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
#include <Base/Sequencer.h>

#include "ViewProvider.h"

#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>

using namespace MeshGui;
using Mesh::MeshFeature;
using Mesh::MeshKernel;
using Mesh::MeshFacetIterator;
using Mesh::Vector3D;
       
ViewProviderInventorMesh::ViewProviderInventorMesh()
{
  pcSwitch = new SoSwitch;
}

ViewProviderInventorMesh::~ViewProviderInventorMesh()
{
}

void ViewProviderInventorMesh::createMesh(Mesh::MeshWithProperty *pcMesh)
{
  MeshKernel *cMesh = pcMesh->getKernel();

  int TreeIndex = 0;
//  int FaceCount = 0;

  int* numVertices = new int[cMesh->CountFacets()];

  Base::Sequencer().start( "Building View node...", cMesh->CountFacets() );
  MeshFacetIterator cFIter(*cMesh);

  // get all facets and their points and normals
  for ( cFIter.Init(); cFIter.More(); cFIter.Next() )
  {
    const Vector3D& cNorm = cFIter->GetNormal();
    const Vector3D& cP0   = cFIter->_aclPoints[0];
    const Vector3D& cP1   = cFIter->_aclPoints[1];
    const Vector3D& cP2   = cFIter->_aclPoints[2];

//    const int idxn=FaceCount++;
//    pcMeshNormal->vector.set1Value(idxn,cNorm.x,cNorm.y,cNorm.z);
		//p1
		const int idx1=TreeIndex++;
		pcMeshCoord->point.set1Value(idx1, cP0.x, cP0.y, cP0.z);
		//p2
		const int idx2=TreeIndex++;
		pcMeshCoord->point.set1Value(idx2, cP1.x, cP1.y, cP1.z);
		//p3
		const int idx3=TreeIndex++;
		pcMeshCoord->point.set1Value(idx3, cP2.x, cP2.y, cP2.z);
		numVertices[ cFIter.Position() ] = 3; //3Vertexes per face

    Base::Sequencer().next();
  }
  
	pcMeshFaces->numVertices.setNum(cMesh->CountFacets());
	pcMeshFaces->numVertices.setValues(0,cMesh->CountFacets(),numVertices);
  delete [] numVertices;
  
  Base::Sequencer().stop();

}


void ViewProviderInventorMesh::attache(App::Feature *pcFeat)
{
  pcFeature = pcFeat;
  // get and save the feature
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  if ( !meshFea ) 
    throw "ViewProviderInventorMesh::attache(): wrong feature attached!";

  // copy the material properties of the feature
  setMatFromFeature();

  // creat the mesh core nodes
  pcMeshCoord     = new SoCoordinate3();
//  pcMeshNormal    = new SoNormal();
  pcMeshFaces     = new SoFaceSet();
  // and set them
  createMesh(&(meshFea->getMesh()));



  SoSeparator* pcFlatRoot = new SoSeparator();
  SoSeparator* pcFlatNormRoot = new SoSeparator();
  SoSeparator* pcWireRoot = new SoSeparator();
  SoSeparator* pcPointRoot = new SoSeparator();
  SoSeparator* pcFlatWireRoot = new SoSeparator();


  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;
  SoLocateHighlight* h = new SoLocateHighlight();
  h->color.setValue((float)0.2,(float)0.5,(float)0.2);
  h->addChild(pcBinding);
  h->addChild(pcMeshCoord);
  h->addChild(pcMeshFaces);
  pcFlatRoot->addChild(pcFlatStyle);
  pcFlatRoot->addChild(pcShadedMaterial);
  pcFlatRoot->addChild(h);

/*  // flat shaded (per Vertex normales) ------------------------------------------
  pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_VERTEX;
  h = new SoLocateHighlight();
  h->color.setValue((float)0.2,(float)0.5,(float)0.2);
  h->addChild(pcBinding);
  h->addChild(pcMeshCoord);
  h->addChild(pcMeshNormal);
  h->addChild(pcMeshFaces);
  pcFlatNormRoot->addChild(pcFlatStyle);
  pcFlatNormRoot->addChild(pcShadedMaterial);
  pcFlatNormRoot->addChild(h); */

  // wire part ----------------------------------------------
  SoDrawStyle *pcWireStyle = new SoDrawStyle();
  pcWireStyle->style = SoDrawStyle::LINES;
  pcWireStyle->lineWidth = 2.0;
  SoLightModel *pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  h = new SoLocateHighlight();
  h->color.setValue((float)0.2,(float)0.5,(float)0.2);
  h->addChild(pcWireStyle);
  h->addChild(pcLightModel);
  h->addChild(pcLineMaterial);
  h->addChild(pcMeshCoord);
  h->addChild(pcMeshFaces);
  pcWireRoot->addChild(h);

  // points part ---------------------------------------------
  SoDrawStyle *pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = 4.0;
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
  pcPointRoot->addChild(pcMeshCoord);
  pcPointRoot->addChild(pcMeshFaces);

  // wire shaded  ------------------------------------------
  pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;
  h = new SoLocateHighlight();
  h->color.setValue((float)0.2,(float)0.5,(float)0.2);
  h->addChild(pcBinding);
  h->addChild(pcMeshCoord);
  h->addChild(pcMeshFaces);
  pcFlatWireRoot->addChild(pcFlatStyle);
  pcFlatWireRoot->addChild(pcShadedMaterial);
  pcFlatWireRoot->addChild(h);
  pcWireStyle = new SoDrawStyle();
  pcWireStyle->style = SoDrawStyle::LINES;
  pcWireStyle->lineWidth = 2.0;
  pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  pcFlatWireRoot->addChild(pcWireStyle);
  pcFlatWireRoot->addChild(pcLightModel);
  pcFlatWireRoot->addChild(pcLineMaterial);
  pcFlatWireRoot->addChild(pcMeshCoord);
  pcFlatWireRoot->addChild(pcMeshFaces);
 

  // puting all togetern with a switch
  pcSwitch->addChild(pcFlatRoot);
//  pcSwitch->addChild(pcFlatNormRoot);
  pcSwitch->addChild(pcWireRoot);
  pcSwitch->addChild(pcPointRoot);
  pcSwitch->addChild(pcFlatWireRoot);
  pcSwitch->whichChild = 0; 

  pcRoot->addChild(pcSwitch);

  setMode(pcFeat->_showMode.c_str());
}

void ViewProviderInventorMesh::update(const ChangeType& Reason)
{
  Reason;
  // set new view modes
  setMode(pcFeature->_showMode.c_str());
  // copy the material properties of the feature
  setMatFromFeature();

}

void ViewProviderInventorMesh::setMode(const char* ModeName)
{
  if(stricmp("Flat",ModeName)==0)
    pcSwitch->whichChild = 0; 
  else if(stricmp("Wire",ModeName)==0)
    pcSwitch->whichChild = 1; 
  else if(stricmp("Point",ModeName)==0)
    pcSwitch->whichChild = 2; 
  else if(stricmp("FlatWire",ModeName)==0)
    pcSwitch->whichChild = 3; 
  else 
    Base::Console().Warning("Unknown mode in ViewProviderInventorMesh::setMode(), ignored");
}

std::vector<std::string> ViewProviderInventorMesh::getModes(void)
{
  std::vector<std::string> StrList;

  StrList.push_back("Flat");
  StrList.push_back("Wire");
  StrList.push_back("Point");
  StrList.push_back("FlatWire");

  return StrList;
}
