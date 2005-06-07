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
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <App/Application.h>
#include <Base/Sequencer.h>

#include "ViewProvider.h"

#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Core/Iterator.h>
# include <Inventor/nodes/SoLocateHighlight.h>

using namespace MeshGui;
using Mesh::MeshFeature;
using Mesh::MeshKernel;
using Mesh::MeshFacetIterator;
using Mesh::Vector3D;
       
ViewProviderInventorMesh::ViewProviderInventorMesh()
{
}

ViewProviderInventorMesh::~ViewProviderInventorMesh()
{
}

SoNode* ViewProviderInventorMesh::create(App::Feature *pcFeature)
{
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  if ( !meshFea ) return 0;

  // getting current setting values...
	bool computeNormals = /*true*/false;

  const MeshKernel* cMesh = meshFea->getMesh().getKernel();

  SoSeparator* tree = new SoSeparator();

  SoLocateHighlight* h = new SoLocateHighlight();
  h->color.setValue((float)0.2,(float)0.5,(float)0.2);

  SoCoordinate3* coord = new SoCoordinate3();
  SoNormal* norms = new SoNormal();
  int BaumIndex = 0;
  int AnzDreiecke = 0;

  int* numVertices = new int[cMesh->CountFacets()];

  Base::Sequencer().start( "Building Inventor node...", cMesh->CountFacets() );
  MeshFacetIterator cFIter(*cMesh);

  // get all facets and their points and normals
  for ( cFIter.Init(); cFIter.More(); cFIter.Next() )
  {
    const Vector3D& cNorm = cFIter->GetNormal();
    const Vector3D& cP0   = cFIter->_aclPoints[0];
    const Vector3D& cP1   = cFIter->_aclPoints[1];
    const Vector3D& cP2   = cFIter->_aclPoints[2];

    const int idxn=AnzDreiecke++;
		norms->vector.set1Value(idxn,cNorm.x,cNorm.y,cNorm.z);
		//p1
		const int idx1=BaumIndex++;
		coord->point.set1Value(idx1, cP0.x, cP0.y, cP0.z);
		//p2
		const int idx2=BaumIndex++;
		coord->point.set1Value(idx2, cP1.x, cP1.y, cP1.z);
		//p3
		const int idx3=BaumIndex++;
		coord->point.set1Value(idx3, cP2.x, cP2.y, cP2.z);
		numVertices[ cFIter.Position() ] = 3; //3Vertex pro Fläche.

    Base::Sequencer().next();
  }
  
	SoFaceSet* fSet = new SoFaceSet();
	fSet->numVertices.setNum(cMesh->CountFacets());
	fSet->numVertices.setValues(0,cMesh->CountFacets(),numVertices);
  delete [] numVertices;
  
	SoNormalBinding* nBinding=new SoNormalBinding();
	tree->addChild(nBinding);		
	if(!computeNormals){
		tree->addChild(norms);
		nBinding->value=SoNormalBinding::PER_FACE;
	}
	else{
		norms->ref();
		norms->unref();
		nBinding->value=SoNormalBinding::PER_VERTEX;
	}

  tree->addChild(h);
	h->addChild(coord);
	h->addChild(fSet);

  Base::Sequencer().stop();

	return tree;
}

