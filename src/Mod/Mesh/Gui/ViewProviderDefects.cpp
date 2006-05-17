/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoShapeHints.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Feature.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>

#include <Mod/Mesh/App/Core/Degeneration.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include "ViewProvider.h"
#include "ViewProviderDefects.h"

using namespace Mesh;
using namespace MeshGui;


PROPERTY_SOURCE_ABSTRACT(MeshGui::ViewProviderMeshDefects, Gui::ViewProviderDocumentObject)
PROPERTY_SOURCE(MeshGui::ViewProviderMeshOrientation, MeshGui::ViewProviderMeshDefects)
PROPERTY_SOURCE(MeshGui::ViewProviderMeshManifolds, MeshGui::ViewProviderMeshDefects)
PROPERTY_SOURCE(MeshGui::ViewProviderMeshDuplicatedFaces, MeshGui::ViewProviderMeshDefects)
PROPERTY_SOURCE(MeshGui::ViewProviderMeshDuplicatedPoints, MeshGui::ViewProviderMeshDefects)
PROPERTY_SOURCE(MeshGui::ViewProviderMeshDegenerations, MeshGui::ViewProviderMeshDefects)
PROPERTY_SOURCE(MeshGui::ViewProviderMeshIndices, MeshGui::ViewProviderMeshDefects)

ViewProviderMeshDefects::ViewProviderMeshDefects()
{
  pcCoords = new SoCoordinate3();
  pcCoords->ref();
}

ViewProviderMeshDefects::~ViewProviderMeshDefects()
{
  pcCoords->unref();
}

// ----------------------------------------------------------------------

ViewProviderMeshOrientation::ViewProviderMeshOrientation()
{
  pcFaces = new SoFaceSet;
  pcFaces->ref();
}

ViewProviderMeshOrientation::~ViewProviderMeshOrientation()
{
  pcFaces->unref();
}

void ViewProviderMeshOrientation::attach(App::DocumentObject* pcFeat)
{
  ViewProviderDocumentObject::attach( pcFeat );

  SoGroup* pcFaceRoot = new SoGroup();

  SoDrawStyle* pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcFaceRoot->addChild(pcFlatStyle);
  
  SoShapeHints * flathints = new SoShapeHints;
  flathints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
  flathints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  pcFaceRoot->addChild(flathints);

  // Draw lines
  SoSeparator* linesep = new SoSeparator;
  SoBaseColor * basecol = new SoBaseColor;
  basecol->rgb.setValue( 1.0f, 0.5f, 0.0f );
  linesep->addChild(basecol);
  linesep->addChild(pcCoords);
  linesep->addChild(pcFaces);
  pcFaceRoot->addChild(linesep);

  addDisplayMode(pcFaceRoot, "Face");
}

void ViewProviderMeshOrientation::showDefects()
{
  Mesh::Feature* f = dynamic_cast<Mesh::Feature*>(pcObject);
  const MeshCore::MeshKernel & rMesh = f->getMesh();
  MeshCore::MeshEvalNormals eval(rMesh);
  
  std::vector<unsigned long> inds = eval.GetIndices();

  pcCoords->point.deleteValues(0);
  pcCoords->point.setNum(3*inds.size());
  MeshCore::MeshFacetIterator cF(rMesh);
  unsigned long i=0;
  unsigned long j=0;
  for ( std::vector<unsigned long>::const_iterator it = inds.begin(); it != inds.end(); ++it ) {
    cF.Set(*it);
    for ( int k=0; k<3; k++ ) {
      Base::Vector3f cP = cF->_aclPoints[k];
      // move a bit in opposite normal direction to overlay the original faces
      cP -= 0.001f * cF->GetNormal();
      pcCoords->point.set1Value(i++,cP.x,cP.y,cP.z);
    }
    pcFaces->numVertices.set1Value(j++, 3);
  }

  setDisplayMode("Face");
}

// ----------------------------------------------------------------------

ViewProviderMeshManifolds::ViewProviderMeshManifolds()
{
  pcLines = new SoLineSet;
  pcLines->ref();
}

ViewProviderMeshManifolds::~ViewProviderMeshManifolds()
{
  pcLines->unref();
}

void ViewProviderMeshManifolds::attach(App::DocumentObject* pcFeat)
{
  ViewProviderDocumentObject::attach( pcFeat );

  SoGroup* pcLineRoot = new SoGroup();
  pcLineStyle->lineWidth = 3;
  pcLineRoot->addChild(pcLineStyle);

  // Draw lines
  SoSeparator* linesep = new SoSeparator;
  SoBaseColor * basecol = new SoBaseColor;
  basecol->rgb.setValue( 1.0f, 0.0f, 0.0f );
  linesep->addChild(basecol);
  linesep->addChild(pcCoords);
  linesep->addChild(pcLines);
  pcLineRoot->addChild(linesep);

  addDisplayMode(pcLineRoot, "Line");
}

void ViewProviderMeshManifolds::showDefects()
{
  Mesh::Feature* f = dynamic_cast<Mesh::Feature*>(pcObject);
  const MeshCore::MeshKernel & rMesh = f->getMesh();
  MeshCore::MeshEvalTopology eval(rMesh);
  eval.Evaluate();
  
  const std::vector<std::pair<unsigned long, unsigned long> >& inds = eval.GetIndices();

  pcCoords->point.deleteValues(0);
  pcCoords->point.setNum(2*inds.size());
  MeshCore::MeshPointIterator cP(rMesh);
  unsigned long i=0;
  unsigned long j=0;
  for ( std::vector<std::pair<unsigned long, unsigned long> >::const_iterator it = inds.begin(); it != inds.end(); ++it ) {
    cP.Set(it->first);
    pcCoords->point.set1Value(i++,cP->x,cP->y,cP->z);
    cP.Set(it->second);
    pcCoords->point.set1Value(i++,cP->x,cP->y,cP->z);
    pcLines->numVertices.set1Value(j++, 2);
  }

  setDisplayMode("Line");
}

// ----------------------------------------------------------------------

ViewProviderMeshDuplicatedFaces::ViewProviderMeshDuplicatedFaces()
{
  pcFaces = new SoFaceSet;
  pcFaces->ref();
}

ViewProviderMeshDuplicatedFaces::~ViewProviderMeshDuplicatedFaces()
{
  pcFaces->unref();
}

void ViewProviderMeshDuplicatedFaces::attach(App::DocumentObject* pcFeat)
{
  ViewProviderDocumentObject::attach( pcFeat );

  SoGroup* pcFaceRoot = new SoGroup();

  SoDrawStyle* pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcFaceRoot->addChild(pcFlatStyle);
  
  SoShapeHints * flathints = new SoShapeHints;
  flathints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
  flathints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  pcFaceRoot->addChild(flathints);

  // Draw lines
  SoSeparator* linesep = new SoSeparator;
  SoBaseColor * basecol = new SoBaseColor;
  basecol->rgb.setValue( 1.0f, 0.0f, 0.0f );
  linesep->addChild(basecol);
  linesep->addChild(pcCoords);
  linesep->addChild(pcFaces);
  pcFaceRoot->addChild(linesep);

  addDisplayMode(pcFaceRoot, "Face");
}

void ViewProviderMeshDuplicatedFaces::showDefects()
{
  Mesh::Feature* f = dynamic_cast<Mesh::Feature*>(pcObject);
  const MeshCore::MeshKernel & rMesh = f->getMesh();
  MeshCore::MeshEvalDuplicateFacets eval(rMesh);
  
  std::vector<unsigned long> inds = eval.GetIndices();

  pcCoords->point.deleteValues(0);
  pcCoords->point.setNum(3*inds.size());
  MeshCore::MeshFacetIterator cF(rMesh);
  unsigned long i=0;
  unsigned long j=0;
  for ( std::vector<unsigned long>::const_iterator it = inds.begin(); it != inds.end(); ++it ) {
    cF.Set(*it);
    for ( int k=0; k<3; k++ ) {
      Base::Vector3f cP = cF->_aclPoints[k];
      // move a bit in normal direction to overlay the original faces
      cP += 0.001f * cF->GetNormal();
      pcCoords->point.set1Value(i++,cP.x,cP.y,cP.z);
    }
    pcFaces->numVertices.set1Value(j++, 3);
  }

  setDisplayMode("Face");
}

// ----------------------------------------------------------------------

ViewProviderMeshDuplicatedPoints::ViewProviderMeshDuplicatedPoints()
{
}

ViewProviderMeshDuplicatedPoints::~ViewProviderMeshDuplicatedPoints()
{
}

void ViewProviderMeshDuplicatedPoints::attach(App::DocumentObject* pcFeat)
{
}

void ViewProviderMeshDuplicatedPoints::showDefects()
{
}

// ----------------------------------------------------------------------

ViewProviderMeshDegenerations::ViewProviderMeshDegenerations()
{
  pcLines = new SoLineSet;
  pcLines->ref();
}

ViewProviderMeshDegenerations::~ViewProviderMeshDegenerations()
{
  pcLines->unref();
}

void ViewProviderMeshDegenerations::attach(App::DocumentObject* pcFeat)
{
  ViewProviderDocumentObject::attach( pcFeat );

  SoGroup* pcLineRoot = new SoGroup();
  pcLineStyle->lineWidth = 3;
  pcLineRoot->addChild(pcLineStyle);

  // Draw lines
  SoSeparator* linesep = new SoSeparator;
  SoBaseColor * basecol = new SoBaseColor;
  basecol->rgb.setValue( 1.0f, 0.5f, 0.0f );
  linesep->addChild(basecol);
  linesep->addChild(pcCoords);
  linesep->addChild(pcLines);
  pcLineRoot->addChild(linesep);

  addDisplayMode(pcLineRoot, "Line");
}

void ViewProviderMeshDegenerations::showDefects()
{
  Mesh::Feature* f = dynamic_cast<Mesh::Feature*>(pcObject);
  const MeshCore::MeshKernel & rMesh = f->getMesh();
  MeshCore::MeshEvalDegeneratedFacets eval(rMesh);
  
  std::vector<unsigned long> inds = eval.GetIndices();

  pcCoords->point.deleteValues(0);
  pcCoords->point.setNum(2*inds.size());
  MeshCore::MeshFacetIterator cF(rMesh);
  unsigned long i=0;
  unsigned long j=0;
  for ( std::vector<unsigned long>::iterator it = inds.begin(); it != inds.end(); ++it ) {
    cF.Set(*it);
    const MeshCore::MeshPoint& rE0 = cF->_aclPoints[0]; 
    const MeshCore::MeshPoint& rE1 = cF->_aclPoints[1];
    const MeshCore::MeshPoint& rE2 = cF->_aclPoints[2];

    // check if the points are coincident
    if ( rE0 == rE1 && rE0 == rE2 ) {
      // not implemented yet
      continue;
    }
    else if ( rE0 == rE1 ) {
      pcCoords->point.set1Value(i++,rE1.x,rE1.y,rE1.z);
      pcCoords->point.set1Value(i++,rE2.x,rE2.y,rE2.z);
    }
    else if ( rE1 == rE2 ) {
      pcCoords->point.set1Value(i++,rE2.x,rE2.y,rE2.z);
      pcCoords->point.set1Value(i++,rE0.x,rE0.y,rE0.z);
    }
    else if ( rE2 == rE0 ) {
      pcCoords->point.set1Value(i++,rE0.x,rE0.y,rE0.z);
      pcCoords->point.set1Value(i++,rE1.x,rE1.y,rE1.z);
    }
    else {
      for ( int j=0; j<3; j++ ) {
        Base::Vector3f cVec1 = cF->_aclPoints[(j+1)%3] - cF->_aclPoints[j];
        Base::Vector3f cVec2 = cF->_aclPoints[(j+2)%3] - cF->_aclPoints[j];

        // adjust the neighbourhoods and point indices
        if ( cVec1 * cVec2 < 0.0f ) {
          pcCoords->point.set1Value(i++,cF->_aclPoints[(j+1)%3].x,cF->_aclPoints[(j+1)%3].y,cF->_aclPoints[(j+1)%3].z);
          pcCoords->point.set1Value(i++,cF->_aclPoints[(j+2)%3].x,cF->_aclPoints[(j+2)%3].y,cF->_aclPoints[(j+2)%3].z);
          break;
        }
      }
    }

    pcLines->numVertices.set1Value(j++, 2);
  }

  setDisplayMode("Line");
}

// ----------------------------------------------------------------------

ViewProviderMeshIndices::ViewProviderMeshIndices()
{
}

ViewProviderMeshIndices::~ViewProviderMeshIndices()
{
}

void ViewProviderMeshIndices::attach(App::DocumentObject* pcFeat)
{
}

void ViewProviderMeshIndices::showDefects()
{
}
