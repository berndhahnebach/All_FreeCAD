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
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
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
#include <App/Document.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>
#include <Base/Sequencer.h>

#include "ViewProvider.h"

#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/Core/Iterator.h>

using namespace std;
using namespace MeshGui;
using Mesh::MeshFeature;

using MeshCore::MeshKernel;
using MeshCore::MeshFacetIterator;
using MeshCore::MeshGeomFacet;
using MeshCore::MeshFacet;

using Base::Vector3D;
       
ViewProviderInventorMesh::ViewProviderInventorMesh()
{
//  pcSwitch = new SoSwitch;
}

ViewProviderInventorMesh::~ViewProviderInventorMesh()
{
}


void ViewProviderInventorMesh::createMesh(Mesh::MeshWithProperty *pcMesh)
{
  MeshKernel *cMesh = pcMesh->getKernel();
  SbVec3f* vertices = new SbVec3f[cMesh->CountPoints()];
  int* faces = new int [4*cMesh->CountFacets()];

  Base::Sequencer().start( "Building View node...", cMesh->CountFacets() );

  unsigned long j=0;
  MeshFacetIterator cFIt(*cMesh);
  for( cFIt.Init(); cFIt.More(); cFIt.Next(), j++ )
  {
    const MeshGeomFacet& rFace = *cFIt;
    MeshFacet aFace = cFIt.GetIndicies();

    for ( int i=0; i<3; i++ )
    {
      vertices[aFace._aulPoints[i]].setValue(rFace._aclPoints[i].x,
                                             rFace._aclPoints[i].y,
                                             rFace._aclPoints[i].z);
      faces[4*j+i] = aFace._aulPoints[i];
    }

    faces[4*j+3] = SO_END_FACE_INDEX;
    Base::Sequencer().next( false ); // don't allow to cancel
  }

	pcMeshCoord->point.setValues(0,cMesh->CountPoints(), vertices);
	pcMeshFaces->coordIndex.setValues(0,4*cMesh->CountFacets(),(const int*) faces);

  delete [] vertices;
  delete [] faces;

  Base::Sequencer().stop();
}


void ViewProviderInventorMesh::attache(App::Feature *pcFeat)
{
  // call father (set material and feature pointer)
  ViewProviderInventorFeature::attache(pcFeat);

  // get and save the feature
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  if ( !meshFea )
    throw "ViewProviderInventorMesh::attache(): wrong feature attached!";

  // creat the mesh core nodes
  pcMeshCoord     = new SoCoordinate3();
  //  pcMeshNormal    = new SoNormal();
  pcMeshFaces     = new SoIndexedFaceSet();
  // and set them
  createMesh(&(meshFea->getMesh()));


  // some helper Separators
  SoGroup* pcFlatRoot = new SoGroup();
  SoGroup* pcFlatNormRoot = new SoGroup();
  SoGroup* pcWireRoot = new SoGroup();
  SoGroup* pcPointRoot = new SoGroup();
  SoGroup* pcFlatWireRoot = new SoGroup();
  SoGroup* pcColorShadedRoot = new SoGroup();

  // only one selection node for the mesh
  pcHighlight = new Gui::SoFCSelection();
  pcHighlight->featureName = pcFeature->getName();
  pcHighlight->documentName = pcFeature->getDocument().getName();
  pcHighlight->subElementName = "Main";
  pcHighlight->addChild(pcMeshCoord);
  pcHighlight->addChild(pcMeshFaces);

  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;

  pcFlatRoot->addChild(pcFlatStyle);
  pcFlatRoot->addChild(pcShadedMaterial);
  pcFlatRoot->addChild(pcBinding);
  pcFlatRoot->addChild(pcHighlight);


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
  pcWireStyle->lineWidth = fLineSize;
  SoLightModel *pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  pcWireRoot->addChild(pcWireStyle);
  pcWireRoot->addChild(pcLightModel);
  pcWireRoot->addChild(pcLineMaterial);
  pcWireRoot->addChild(pcHighlight);

  // points part ---------------------------------------------
  SoDrawStyle *pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = fPointSize;
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
  pcPointRoot->addChild(pcHighlight);

  // wire shaded  ------------------------------------------
  pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;
  pcFlatWireRoot->addChild(pcBinding);
  pcFlatWireRoot->addChild(pcFlatStyle);
  pcFlatWireRoot->addChild(pcShadedMaterial);
  pcFlatWireRoot->addChild(pcHighlight);
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
  pcModeSwitch->addChild(pcFlatRoot);
//  pcSwitch->addChild(pcFlatNormRoot);
  pcModeSwitch->addChild(pcWireRoot);
  pcModeSwitch->addChild(pcPointRoot);
  pcModeSwitch->addChild(pcFlatWireRoot);

  // standard viewing (flat)
  pcModeSwitch->whichChild = 0; 

}

void ViewProviderInventorMesh::updateData(void)
{
  // get the mesh
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  createMesh(&(meshFea->getMesh()));
}


vector<string> ViewProviderInventorMesh::getModes(void)
{
  // get the modes of the father
  vector<string> StrList = ViewProviderInventorFeature::getModes();

  // add your own modes
  StrList.push_back("Flat");
  StrList.push_back("Wire");
  StrList.push_back("Point");
  StrList.push_back("FlatWire");

  return StrList;
}


