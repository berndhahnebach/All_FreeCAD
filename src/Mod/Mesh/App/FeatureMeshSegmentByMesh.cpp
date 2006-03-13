/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <fcntl.h>
# include <TFunction_Logbook.hxx>
# include <ios>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>

#include "Core/Algorithm.h"
#include "Core/Evaluation.h"
#include "Core/Iterator.h"
#include "Core/Visitor.h"

#include "FeatureMeshSegmentByMesh.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::SegmentByMesh, Mesh::Feature)


SegmentByMesh::SegmentByMesh(void)
{
  ADD_PROPERTY(Source  ,(0));
  ADD_PROPERTY(Tool    ,(0));
  ADD_PROPERTY(Base    ,(0.0,0.0,0.0));
  ADD_PROPERTY(Normal  ,(0.0,0.0,1.0));

}

int SegmentByMesh::execute(void)
{
  Feature *pcMesh  = dynamic_cast<Feature*>(Source.getValue());
  Feature *pcTool  = dynamic_cast<Feature*>(Tool.getValue());

  // the clipping plane
  Vector3D cBase, cNormal;
  cBase =   Base.getValue();
  cNormal = Normal.getValue();

  if (!pcMesh) {
    setError("No mesh specified.\n");
    return 1;
  } else if (pcMesh->getStatus() != Valid) {
    setError("'%s' isn't a valid mesh.\n", pcMesh->name.getValue());
    return 1;
  } else if (!pcTool) {
    setError("No toolmesh specified.\n");
    return 1;
  } else if (pcTool->getStatus() != Valid) {
    setError("'%s' isn't a valid toolmesh.\n", pcTool->name.getValue());
    return 1;
  }

  /*const*/ MeshKernel& rMeshKernel = pcMesh->getMesh();
  /*const*/ MeshKernel& rToolMesh   = pcTool->getMesh();

  // check if the toolmesh is a solid
  if ( MeshEvalSolid(rToolMesh).Validate(false) != MeshEvalSolid::Valid )
  {
    setError("'%s' isn't a solid.\n", pcTool->name.getValue());
    return 1; // no solid
  }

  std::vector<unsigned long> faces;
  std::vector<MeshGeomFacet> aFaces;

  MeshAlgorithm cAlg(rMeshKernel);
  if ( cNormal.Length() > 0.1f ) // not a null vector
    cAlg.GetFacetsFromToolMesh(rToolMesh, cNormal, faces);
  else
    cAlg.GetFacetsFromToolMesh(rToolMesh, Vector3D(0.0, 1.0f, 0.0f), faces);

  // if the clipping plane was set then we want only the visible facets
  if ( cNormal.Length() > 0.1f ) // not a null vector
  {
    // now we have too many facets since we have (invisible) facets near to the back clipping plane, 
    // so we need the nearest facet to the front clipping plane
    //
    float fDist = FLOAT_MAX;
    unsigned long uIdx=ULONG_MAX;
    MeshFacetIterator cFIt(rMeshKernel);

    // get the nearest facet to the user (front clipping plane)
    for ( std::vector<unsigned long>::iterator it = faces.begin(); it != faces.end(); ++it )
    {
      cFIt.Set(*it);
      float dist = fabs(cFIt->GetGravityPoint().DistanceToPlane( cBase, cNormal ));
      if ( dist < fDist )
      {
        fDist = dist;
        uIdx = *it;
      }
    }

    // succeeded
    if ( uIdx != ULONG_MAX )
    {
      // set VISIT-Flag to all outer facets
      cAlg.SetFacetFlag( MeshFacet::VISIT );
      cAlg.ResetFacetsFlag(faces, MeshFacet::VISIT);

      faces.clear();
      MeshTopFacetVisitor clVisitor(faces);
      rMeshKernel.VisitNeighbourFacets(clVisitor, uIdx);
    
      // append also the start facet
      faces.push_back(uIdx);
    }
  }

  for ( std::vector<unsigned long>::iterator it = faces.begin(); it != faces.end(); ++it )
    aFaces.push_back( rMeshKernel.GetFacet(*it) );
  Mesh.getValue() = ( aFaces );

  return 0;
}

