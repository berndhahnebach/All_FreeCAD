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
#include <Base/Sequencer.h>

#include "ViewProvider.h"

#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>

using namespace MeshGui;
using Mesh::MeshFeature;
using Mesh::MeshKernel;
using Mesh::MeshFacetIterator;
using Base::Vector3D;
       
ViewProviderInventorMesh::ViewProviderInventorMesh()
{
  pcSwitch = new SoSwitch;
}

ViewProviderInventorMesh::~ViewProviderInventorMesh()
{
}


void ViewProviderInventorMesh::selected(Gui::View3DInventorViewer *, SoPath *)
{
   Base::Console().Log("Select viewprovider Mesh  %p\n",this);
   Gui::Selection().addFeature(pcFeature);

   pcHighlight->mode = SoLocateHighlight::ON;
//   pcHighlight->style = SoLocateHighlight::EMISSIVE_DIFFUSE;
   pcHighlight->color.setValue((float)0.0,(float)0.3,(float)0.0);

 
}
void ViewProviderInventorMesh::unselected(Gui::View3DInventorViewer *, SoPath *)
{
   Base::Console().Log("Unselect viewprovider Mesh  %p\n",this);
   Gui::Selection().removeFeature(pcFeature);

   pcHighlight->mode = SoLocateHighlight::AUTO;
//   pcHighlight->style = SoLocateHighlight::EMISSIVE;
   pcHighlight->color.setValue((float)0.1,(float)0.3,(float)0.7);

}


void ViewProviderInventorMesh::createMesh(Mesh::MeshWithProperty *pcMesh)
{/*
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
*/
  MeshKernel *cMesh = pcMesh->getKernel();
  SbVec3f* vertices = new SbVec3f[cMesh->CountPoints()];
  int* faces = new int [4*cMesh->CountFacets()];

  Base::Sequencer().start( "Building View node...", cMesh->CountFacets() );

  unsigned long j=0;
  Mesh::MeshFacetIterator cFIt(*cMesh);
  for( cFIt.Init(); cFIt.More(); cFIt.Next(), j++ )
  {
    const Mesh::MeshGeomFacet& rFace = *cFIt;
    Mesh::MeshFacet aFace = cFIt.GetIndicies();

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
  pcMeshFaces     = new SoIndexedFaceSet();
//  pcMeshFaces     = new SoFaceSet();
  // and set them
  createMesh(&(meshFea->getMesh()));



  SoSeparator* pcFlatRoot = new SoSeparator();
  SoSeparator* pcFlatNormRoot = new SoSeparator();
  SoSeparator* pcWireRoot = new SoSeparator();
  SoSeparator* pcPointRoot = new SoSeparator();
  SoSeparator* pcFlatWireRoot = new SoSeparator();
  SoSeparator* pcColorShadedRoot = new SoSeparator();


  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;

  pcFlatRoot->addChild(pcFlatStyle);
  pcFlatRoot->addChild(pcShadedMaterial);
  // Hilight for selection
  pcHighlight = new SoLocateHighlight();
  pcHighlight->color.setValue((float)0.1,(float)0.3,(float)0.7);
//  pcHighlight->style = SoLocateHighlight::EMISSIVE_DIFFUSE;
  pcHighlight->addChild(pcBinding);
  pcHighlight->addChild(pcMeshCoord);
  pcHighlight->addChild(pcMeshFaces);
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
  SoLocateHighlight *pcHighlight2 = new SoLocateHighlight();
  pcHighlight2->color.setValue((float)0.1,(float)0.3,(float)0.7);
  pcHighlight2->addChild(pcWireStyle);
  pcHighlight2->addChild(pcLightModel);
  pcHighlight2->addChild(pcLineMaterial);
  pcHighlight2->addChild(pcMeshCoord);
  pcHighlight2->addChild(pcMeshFaces);
  pcWireRoot->addChild(pcHighlight2);

  // points part ---------------------------------------------
  SoDrawStyle *pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = fPointSize;
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
  pcPointRoot->addChild(pcMeshCoord);
  pcPointRoot->addChild(pcMeshFaces);

  // wire shaded  ------------------------------------------
  pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;
  SoLocateHighlight *pcHighlight3 = new SoLocateHighlight();
  pcHighlight3->color.setValue((float)0.1,(float)0.3,(float)0.7);
  pcHighlight3->addChild(pcBinding);
  pcHighlight3->addChild(pcMeshCoord);
  pcHighlight3->addChild(pcMeshFaces);
  pcFlatWireRoot->addChild(pcFlatStyle);
  pcFlatWireRoot->addChild(pcShadedMaterial);
  pcFlatWireRoot->addChild(pcHighlight3);
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

  // color shaded  ------------------------------------------
  pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcFlatRoot->addChild(pcFlatStyle);

  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcColorMat = new SoMaterial;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
  // Hilight for selection
  /*SoLocateHighlight *pcHighlight4 = new SoLocateHighlight();*/
  /*pcHighlight4->color.setValue((float)0.1,(float)0.3,(float)0.7);*/
//  pcHighlight->style = SoLocateHighlight::EMISSIVE_DIFFUSE;
  pcColorShadedRoot->addChild(pcBinding);   /*pcHighlight4->addChild(pcBinding);*/
  pcColorShadedRoot->addChild(pcMeshCoord); /*pcHighlight4->addChild(pcMeshCoord);*/
  pcColorShadedRoot->addChild(pcMeshFaces); /*pcHighlight4->addChild(pcMeshFaces);*/
  /*pcColorShadedRoot->addChild(pcHighlight4);*/

  // puting all togetern with a switch
  pcSwitch->addChild(pcFlatRoot);
//  pcSwitch->addChild(pcFlatNormRoot);
  pcSwitch->addChild(pcWireRoot);
  pcSwitch->addChild(pcPointRoot);
  pcSwitch->addChild(pcFlatWireRoot);
  pcSwitch->addChild(pcColorShadedRoot);

  pcSwitch->whichChild = 0; 

  pcRoot->addChild(pcSwitch);

  setMode(pcFeat->getShowMode());
}

void ViewProviderInventorMesh::update(const ChangeType& Reason)
{
  Reason;
  // set new view modes
  setMode(pcFeature->getShowMode());
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
  {
    Mesh::MeshWithProperty &rcMesh = dynamic_cast<MeshFeature*>(pcFeature)->getMesh();
    App::PropertyBag *pcProp = 0;
    pcProp = rcMesh.Get(ModeName);
    if ( pcProp && stricmp("VertexColor",pcProp->GetType())==0 )
    {
      SetVertexColorMode(dynamic_cast<Mesh::MeshPropertyColor*>(pcProp));
      pcSwitch->whichChild = 4;

    }else 
      Base::Console().Warning("Unknown mode '%s' in ViewProviderInventorMesh::setMode(), ignored\n", ModeName);
  }
}

std::vector<std::string> ViewProviderInventorMesh::getModes(void)
{
  std::vector<std::string> StrList;

  StrList.push_back("Flat");
  StrList.push_back("Wire");
  StrList.push_back("Point");
  StrList.push_back("FlatWire");

  Mesh::MeshWithProperty &rcMesh = dynamic_cast<MeshFeature*>(pcFeature)->getMesh();
  std::list<std::string> list = rcMesh.GetAllNamesOfType("VertexColor");

  for(std::list<std::string>::iterator It=list.begin();It!=list.end();It++)
    StrList.push_back(*It);

  return StrList;
}


void ViewProviderInventorMesh::SetVertexColorMode(Mesh::MeshPropertyColor* pcProp)
{
  std::vector<Mesh::MeshPropertyColor::fColor> color = pcProp->Color;
  for (unsigned long i=0; i<color.size();i++)
  {
    Mesh::MeshPropertyColor::fColor& col = color[i];
    pcColorMat->diffuseColor.set1Value(i, SbColor(col.r, col.g, col.b));
  }
}
