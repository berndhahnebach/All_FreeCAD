/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
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
# include <algorithm>
#endif

#include <Mod/Mesh/App/WildMagic4/Wm4MeshCurvature.h>
#include <Mod/Mesh/App/WildMagic4/Wm4Vector3.h>

#include "TopoAlgorithm.h"
#include "Iterator.h"
#include "MeshKernel.h"
#include "Algorithm.h"
#include "Evaluation.h"
#include "Triangulation.h"
#include <Base/Console.h>

using namespace MeshCore;

MeshTopoAlgorithm::MeshTopoAlgorithm (MeshKernel &rclM)
: _rclMesh(rclM), _needsCleanup(false)
{
}

MeshTopoAlgorithm::~MeshTopoAlgorithm (void)
{
  if ( _needsCleanup )
    Cleanup();
}

bool MeshTopoAlgorithm::InsertVertex(unsigned long ulFacetPos, const Base::Vector3f&  rclPoint)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  MeshFacet  clNewFacet1, clNewFacet2;

  // insert new point
  unsigned long ulPtCnt = _rclMesh._aclPointArray.size();
  unsigned long ulPtInd = _rclMesh._aclPointArray.GetOrAddIndex(rclPoint);
  unsigned long ulSize  = _rclMesh._aclFacetArray.size();

  if ( ulPtInd < ulPtCnt )
    return false; // the given point is already part of the mesh => creating new facets would be an illegal operation

  // adjust the facets
  //
  // first new facet
  clNewFacet1._aulPoints[0] = rclF._aulPoints[1];
  clNewFacet1._aulPoints[1] = rclF._aulPoints[2];
  clNewFacet1._aulPoints[2] = ulPtInd;
  clNewFacet1._aulNeighbours[0] = rclF._aulNeighbours[1];
  clNewFacet1._aulNeighbours[1] = ulSize+1;
  clNewFacet1._aulNeighbours[2] = ulFacetPos;
  // second new facet
  clNewFacet2._aulPoints[0] = rclF._aulPoints[2];
  clNewFacet2._aulPoints[1] = rclF._aulPoints[0];
  clNewFacet2._aulPoints[2] = ulPtInd;
  clNewFacet2._aulNeighbours[0] = rclF._aulNeighbours[2];
  clNewFacet2._aulNeighbours[1] = ulFacetPos;
  clNewFacet2._aulNeighbours[2] = ulSize;
  // adjust the neighbour facet
  if (rclF._aulNeighbours[1] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclF._aulNeighbours[1]].ReplaceNeighbour(ulFacetPos, ulSize);
  if (rclF._aulNeighbours[2] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclF._aulNeighbours[2]].ReplaceNeighbour(ulFacetPos, ulSize+1);
  // original facet
  rclF._aulPoints[2] = ulPtInd;
  rclF._aulNeighbours[1] = ulSize;
  rclF._aulNeighbours[2] = ulSize+1;

  // insert new facets
  _rclMesh._aclFacetArray.push_back(clNewFacet1);
  _rclMesh._aclFacetArray.push_back(clNewFacet2);

  return true;
}

bool MeshTopoAlgorithm::SnapVertex(unsigned long ulFacetPos, const Base::Vector3f& rP)
{
  MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];
  if (!rFace.HasOpenEdge())
    return false;
  Base::Vector3f cNo1 = _rclMesh.GetNormal(rFace);
  for (short i=0; i<3; i++)
  {
    if (rFace._aulNeighbours[i]==ULONG_MAX)
    {
      const Base::Vector3f& rPt1 = _rclMesh._aclPointArray[rFace._aulPoints[i]];
      const Base::Vector3f& rPt2 = _rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]];
      Base::Vector3f cNo2 = (rPt2 - rPt1) % cNo1;
      Base::Vector3f cNo3 = (rP - rPt1) % (rPt2 - rPt1);
      float fD2 = Base::DistanceP2(rPt1, rPt2);
      float fTV = (rP-rPt1) * (rPt2-rPt1);

      // Point is on the edge
      if ( cNo3.Length() < FLOAT_EPS )
      {
        unsigned long uCt = _rclMesh.CountFacets();
        SplitOpenEdge(ulFacetPos, rFace._aulNeighbours[i], rP);
        return uCt < _rclMesh.CountFacets();
      }
      else if ( (rP - rPt1)*cNo2 > 0.0f && fD2 >= fTV && fTV >= 0.0f )
      {
        MeshFacet cTria;
        cTria._aulPoints[0] = _rclMesh._aclPointArray.GetOrAddIndex(rP);
        cTria._aulPoints[1] = rFace._aulPoints[(i+1)%3];
        cTria._aulPoints[2] = rFace._aulPoints[i];
        cTria._aulNeighbours[1] = ulFacetPos;
        rFace._aulNeighbours[i] = _rclMesh.CountFacets();
        _rclMesh._aclFacetArray.push_back(cTria);
        return true;
      }
    }
  }

  return false;
}

void MeshTopoAlgorithm::OptimizeTopology(float fMaxAngle)
{
  // For each internal edge get the adjacent facets. When doing an edge swap we must update
  // this structure.
  std::map<std::pair<unsigned long, unsigned long>, std::vector<unsigned long> > aEdge2Face;
  for (MeshFacetArray::_TIterator pI = _rclMesh._aclFacetArray.begin(); pI != _rclMesh._aclFacetArray.end(); pI++)
  {
    for (int i = 0; i < 3; i++)
    {
      // ignore open edges
      if ( pI->_aulNeighbours[i] != ULONG_MAX ) {
        unsigned long ulPt0 = std::min<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
        unsigned long ulPt1 = std::max<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
        aEdge2Face[std::pair<unsigned long, unsigned long>(ulPt0, ulPt1)].push_back(pI - _rclMesh._aclFacetArray.begin());
      }
    }
  }

  // fill up this list with all internal edges and perform swap edges until this list is empty
  std::list<std::pair<unsigned long, unsigned long> > aEdgeList;
  std::map<std::pair<unsigned long, unsigned long>, std::vector<unsigned long> >::iterator pE;
  for (pE = aEdge2Face.begin(); pE != aEdge2Face.end(); ++pE) {
    if (pE->second.size() == 2) // make sure that we really have an internal edge
      aEdgeList.push_back(pE->first);
  }

  // to be sure to avoid an endless loop
  unsigned long uMaxIter = 5 * aEdge2Face.size();

  // Perform a swap edge where needed
  while ( !aEdgeList.empty() && uMaxIter > 0 ) {
    // get the first edge and remove it from the list
    std::pair<unsigned long, unsigned long> aEdge = aEdgeList.front();
    aEdgeList.pop_front();
    uMaxIter--;

    // get the adjacent facets to this edge
    pE = aEdge2Face.find( aEdge );

    // this edge has been removed some iterations before
    if ( pE == aEdge2Face.end() )
      continue;

    // Is swap edge allowed and sensible?
    if ( !ShouldSwapEdge(pE->second[0], pE->second[1], fMaxAngle) )
      continue;

    // ok, here we should perform a swap edge to minimize the maximum angle
    if ( /*fMax12 > fMax34*/true ) {
      // swap the edge
      SwapEdge(pE->second[0], pE->second[1]);
      
      MeshFacet& rF1 = _rclMesh._aclFacetArray[pE->second[0]];
      MeshFacet& rF2 = _rclMesh._aclFacetArray[pE->second[1]];
      unsigned short side1 = rF1.Side(aEdge.first, aEdge.second);
      unsigned short side2 = rF2.Side(aEdge.first, aEdge.second);

      // adjust the edge list
      for ( int i=0; i<3; i++ ) {
        std::map<std::pair<unsigned long, unsigned long>, std::vector<unsigned long> >::iterator it;
        // first facet
        unsigned long ulPt0 = std::min<unsigned long>(rF1._aulPoints[i],  rF1._aulPoints[(i+1)%3]);
        unsigned long ulPt1 = std::max<unsigned long>(rF1._aulPoints[i],  rF1._aulPoints[(i+1)%3]);
        it = aEdge2Face.find( std::make_pair(ulPt0, ulPt1) );
        if ( it != aEdge2Face.end() ) {
          if ( it->second[0] == pE->second[1] )
            it->second[0] = pE->second[0];
          else if ( it->second[1] == pE->second[1] )
            it->second[1] = pE->second[0];
          aEdgeList.push_back( it->first );
        }

        // second facet
        ulPt0 = std::min<unsigned long>(rF2._aulPoints[i],  rF2._aulPoints[(i+1)%3]);
        ulPt1 = std::max<unsigned long>(rF2._aulPoints[i],  rF2._aulPoints[(i+1)%3]);
        it = aEdge2Face.find( std::make_pair(ulPt0, ulPt1) );
        if ( it != aEdge2Face.end() ) {
          if ( it->second[0] == pE->second[0] )
            it->second[0] = pE->second[1];
          else if ( it->second[1] == pE->second[0] )
            it->second[1] = pE->second[1];
          aEdgeList.push_back( it->first );
        }
      }

      // Now we must remove the edge and replace it through the new edge
      unsigned long ulPt0 = std::min<unsigned long>(rF1._aulPoints[(side1+1)%3], rF2._aulPoints[(side2+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(rF1._aulPoints[(side1+1)%3], rF2._aulPoints[(side2+1)%3]);
      std::pair<unsigned long, unsigned long> aNewEdge = std::make_pair(ulPt0, ulPt1);
      aEdge2Face[aNewEdge] = pE->second;
      aEdge2Face.erase(pE);
    }
  }
}

void MeshTopoAlgorithm::AdjustEdgesToCurvatureDirection()
{
  std::vector< Wm4::Vector3<float> > aPnts;
  MeshPointIterator cPIt( _rclMesh );
  aPnts.reserve(_rclMesh.CountPoints());
  for ( cPIt.Init(); cPIt.More(); cPIt.Next() )
    aPnts.push_back( Wm4::Vector3<float>( cPIt->x, cPIt->y, cPIt->z ) );

  // get all point connections
  std::vector<int> aIdx;
  const MeshFacetArray& raFts = _rclMesh.GetFacets();
  aIdx.reserve( 3*raFts.size() );

  // Build map of edges to the referencing facets
  unsigned long k = 0;
  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> > aclEdgeMap;
  for ( std::vector<MeshFacet>::const_iterator jt = raFts.begin(); jt != raFts.end(); ++jt, k++ )
  {
    for (int i=0; i<3; i++)
    {
      unsigned long ulT0 = jt->_aulPoints[i];
      unsigned long ulT1 = jt->_aulPoints[(i+1)%3];
      unsigned long ulP0 = std::min<unsigned long>(ulT0, ulT1);
      unsigned long ulP1 = std::max<unsigned long>(ulT0, ulT1);
      aclEdgeMap[std::make_pair<unsigned long, unsigned long>(ulP0, ulP1)].push_front(k);
      aIdx.push_back( (int)jt->_aulPoints[i] );
    }
  }

  // compute vertex based curvatures
  Wm4::MeshCurvature<float> meshCurv(_rclMesh.CountPoints(), &(aPnts[0]), _rclMesh.CountFacets(), &(aIdx[0]));

  // get curvature information now
  const Wm4::Vector3<float>* aMaxCurvDir = meshCurv.GetMaxDirections();
  const Wm4::Vector3<float>* aMinCurvDir = meshCurv.GetMinDirections();
  const float* aMaxCurv = meshCurv.GetMaxCurvatures();
  const float* aMinCurv = meshCurv.GetMinCurvatures();

  raFts.ResetFlag(MeshFacet::VISIT);
  const MeshPointArray& raPts = _rclMesh.GetPoints();
  for ( std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >::iterator kt = aclEdgeMap.begin(); kt != aclEdgeMap.end(); ++kt )
  {
    if ( kt->second.size() == 2 ) {
      unsigned long uPt1 = kt->first.first;
      unsigned long uPt2 = kt->first.second;
      unsigned long uFt1 = kt->second.front();
      unsigned long uFt2 = kt->second.back();

      const MeshFacet& rFace1 = raFts[uFt1];
      const MeshFacet& rFace2 = raFts[uFt2];
      if ( rFace1.IsFlag(MeshFacet::VISIT) || rFace2.IsFlag(MeshFacet::VISIT) )
        continue;

      unsigned long uPt3, uPt4;
      unsigned short side = rFace1.Side(uPt1, uPt2);
      uPt3 = rFace1._aulPoints[(side+2)%3];
      side = rFace2.Side(uPt1, uPt2);
      uPt4 = rFace2._aulPoints[(side+2)%3];
      
      Wm4::Vector3<float> dir;
      float fActCurvature;
      if ( fabs(aMinCurv[uPt1]) > fabs(aMaxCurv[uPt1]) ) {
        fActCurvature = aMinCurv[uPt1];
        dir = aMaxCurvDir[uPt1];
      } else {
        fActCurvature = aMaxCurv[uPt1];
        dir = aMinCurvDir[uPt1];
      }

      Base::Vector3f cMinDir(dir.X(), dir.Y(), dir.Z());
      Base::Vector3f cEdgeDir1 = raPts[uPt1] - raPts[uPt2];
      Base::Vector3f cEdgeDir2 = raPts[uPt3] - raPts[uPt4];
      cMinDir.Normalize(); cEdgeDir1.Normalize(); cEdgeDir2.Normalize();
    
      // get the plane and calculate the distance to the fourth point
      MeshGeomFacet cPlane(raPts[uPt1], raPts[uPt2], raPts[uPt3]);
      // positive or negative distance
      float fDist = raPts[uPt4].DistanceToPlane(cPlane._aclPoints[0], cPlane.GetNormal());

      float fLength12 = Base::Distance(raPts[uPt1], raPts[uPt2]);
      float fLength34 = Base::Distance(raPts[uPt3], raPts[uPt4]);
      if ( fabs(cEdgeDir1*cMinDir) < fabs(cEdgeDir2*cMinDir) )
      {
        if ( IsSwapEdgeLegal(uFt1, uFt2) && fLength34 < 1.05f*fLength12 && fActCurvature*fDist > 0.0f) {
          SwapEdge(uFt1, uFt2);
          rFace1.SetFlag(MeshFacet::VISIT);
          rFace2.SetFlag(MeshFacet::VISIT);
        }
      }
    }
  }
}

bool MeshTopoAlgorithm::InsertVertexAndSwapEdge(unsigned long ulFacetPos, const Base::Vector3f&  rclPoint, float fMaxAngle)
{
  if ( !InsertVertex(ulFacetPos, rclPoint) )
    return false;

  // get the created elements
  unsigned long ulF1Ind = _rclMesh._aclFacetArray.size()-2;
  unsigned long ulF2Ind = _rclMesh._aclFacetArray.size()-1;
  MeshFacet& rclF1 = _rclMesh._aclFacetArray[ulFacetPos];
  MeshFacet& rclF2 = _rclMesh._aclFacetArray[ulF1Ind];
  MeshFacet& rclF3 = _rclMesh._aclFacetArray[ulF2Ind];

  // first facet
  int i;
  for ( i=0; i<3; i++ )
  {
    unsigned long uNeighbour = rclF1._aulNeighbours[i];
    if ( uNeighbour!=ULONG_MAX && uNeighbour!=ulF1Ind && uNeighbour!=ulF2Ind )
    {
      if ( ShouldSwapEdge(ulFacetPos, uNeighbour, fMaxAngle) ) {
        SwapEdge(ulFacetPos, uNeighbour);
        break;
      }
    }
  }
  for ( i=0; i<3; i++ )
  {
    // second facet
    unsigned long uNeighbour = rclF2._aulNeighbours[i];
    if ( uNeighbour!=ULONG_MAX && uNeighbour!=ulFacetPos && uNeighbour!=ulF2Ind )
    {
      if ( ShouldSwapEdge(ulF1Ind, uNeighbour, fMaxAngle) ) {
        SwapEdge(ulF1Ind, uNeighbour);
        break;
      }
    }
  }

  // third facet
  for ( i=0; i<3; i++ )
  {
    unsigned long uNeighbour = rclF3._aulNeighbours[i];
    if ( uNeighbour!=ULONG_MAX && uNeighbour!=ulFacetPos && uNeighbour!=ulF1Ind )
    {
      if ( ShouldSwapEdge(ulF2Ind, uNeighbour, fMaxAngle) ) {
        SwapEdge(ulF2Ind, uNeighbour);
        break;
      }
    }
  }

  return true;
}

bool MeshTopoAlgorithm::IsSwapEdgeLegal(unsigned long ulFacetPos, unsigned long ulNeighbour) const
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];

  unsigned short uFSide = rclF.Side(rclN);
  unsigned short uNSide = rclN.Side(rclF);

  if (uFSide == USHRT_MAX || uNSide == USHRT_MAX) 
    return false; // not neighbours

  Base::Vector3f cP1 = _rclMesh._aclPointArray[rclF._aulPoints[uFSide]];
  Base::Vector3f cP2 = _rclMesh._aclPointArray[rclF._aulPoints[(uFSide+1)%3]];
  Base::Vector3f cP3 = _rclMesh._aclPointArray[rclF._aulPoints[(uFSide+2)%3]];
  Base::Vector3f cP4 = _rclMesh._aclPointArray[rclN._aulPoints[(uNSide+2)%3]];

  // do not allow to create degenerated triangles
  MeshGeomFacet cT3(cP4, cP3, cP1);
  if ( cT3.IsDegenerated() )
    return false;
  MeshGeomFacet cT4(cP3, cP4, cP2);
  if ( cT4.IsDegenerated() )
    return false;

  // We must make sure that the two adjacent triangles builds a convex polygon, otherwise 
  // the swap edge operation is illegal
  Base::Vector3f cU = cP2-cP1;
  Base::Vector3f cV = cP4-cP3;
  // build a helper plane through cP1 that must separate cP3 and cP4
  Base::Vector3f cN1 = (cU % cV) % cU;
  if ( ((cP3-cP1)*cN1)*((cP4-cP1)*cN1) >= 0.0f )
    return false; // not convex
  // build a helper plane through cP3 that must separate cP1 and cP2
  Base::Vector3f cN2 = (cU % cV) % cV;
  if ( ((cP1-cP3)*cN2)*((cP2-cP3)*cN2) >= 0.0f )
    return false; // not convex

  return true;
}

bool MeshTopoAlgorithm::ShouldSwapEdge(unsigned long ulFacetPos, unsigned long ulNeighbour, float fMaxAngle) const
{
  if ( !IsSwapEdgeLegal(ulFacetPos, ulNeighbour) )
    return false;

  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];

  unsigned short uFSide = rclF.Side(rclN);
  unsigned short uNSide = rclN.Side(rclF);

  Base::Vector3f cP1 = _rclMesh._aclPointArray[rclF._aulPoints[uFSide]];
  Base::Vector3f cP2 = _rclMesh._aclPointArray[rclF._aulPoints[(uFSide+1)%3]];
  Base::Vector3f cP3 = _rclMesh._aclPointArray[rclF._aulPoints[(uFSide+2)%3]];
  Base::Vector3f cP4 = _rclMesh._aclPointArray[rclN._aulPoints[(uNSide+2)%3]];

  MeshGeomFacet cT1(cP1, cP2, cP3); float fMax1 = cT1.MaximumAngle();
  MeshGeomFacet cT2(cP2, cP1, cP4); float fMax2 = cT2.MaximumAngle();
  MeshGeomFacet cT3(cP4, cP3, cP1); float fMax3 = cT3.MaximumAngle();
  MeshGeomFacet cT4(cP3, cP4, cP2); float fMax4 = cT4.MaximumAngle();

  // get the angle between the triangles
  Base::Vector3f cN1 = cT1.GetNormal();
  Base::Vector3f cN2 = cT2.GetNormal();
  if ( cN1.GetAngle(cN2) > fMaxAngle )
    return false;

  float fMax12 = std::max<float>(fMax1, fMax2);
  float fMax34 = std::max<float>(fMax3, fMax4);

  return  fMax12 > fMax34;
}

void MeshTopoAlgorithm::SwapEdge(unsigned long ulFacetPos, unsigned long ulNeighbour)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];

  unsigned short uFSide = rclF.Side(rclN);
  unsigned short uNSide = rclN.Side(rclF);

  if (uFSide == USHRT_MAX || uNSide == USHRT_MAX) 
    return; // not neighbours

  // adjust the neighbourhood
  if (rclF._aulNeighbours[(uFSide+1)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclF._aulNeighbours[(uFSide+1)%3]].ReplaceNeighbour(ulFacetPos, ulNeighbour);
  if (rclN._aulNeighbours[(uNSide+1)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+1)%3]].ReplaceNeighbour(ulNeighbour, ulFacetPos);

  // swap the point and neighbour indices
  rclF._aulPoints[(uFSide+1)%3] = rclN._aulPoints[(uNSide+2)%3];
  rclN._aulPoints[(uNSide+1)%3] = rclF._aulPoints[(uFSide+2)%3];
  rclF._aulNeighbours[uFSide] = rclN._aulNeighbours[(uNSide+1)%3];
  rclN._aulNeighbours[uNSide] = rclF._aulNeighbours[(uFSide+1)%3];
  rclF._aulNeighbours[(uFSide+1)%3] = ulNeighbour;
  rclN._aulNeighbours[(uNSide+1)%3] = ulFacetPos;
}

void MeshTopoAlgorithm::SplitEdge(unsigned long ulFacetPos, unsigned long ulNeighbour, const Base::Vector3f& rP)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];

  unsigned short uFSide = rclF.Side(rclN);
  unsigned short uNSide = rclN.Side(rclF);

  if (uFSide == USHRT_MAX || uNSide == USHRT_MAX) 
    return; // not neighbours

  unsigned long uPtCnt = _rclMesh._aclPointArray.size();
  unsigned long uPtInd = _rclMesh._aclPointArray.GetOrAddIndex(rP);
  unsigned long ulSize = _rclMesh._aclFacetArray.size();

  if ( uPtInd < uPtCnt )
    return; // the given point is already part of the mesh => creating new facets would be an illegal operation

  // adjust the neighbourhood
  if (rclF._aulNeighbours[(uFSide+1)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclF._aulNeighbours[(uFSide+1)%3]].ReplaceNeighbour(ulFacetPos, ulSize);
  if (rclN._aulNeighbours[(uNSide+2)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+2)%3]].ReplaceNeighbour(ulNeighbour, ulSize+1);

  MeshFacet cNew1, cNew2;
  cNew1._aulPoints[0] = uPtInd;
  cNew1._aulPoints[1] = rclF._aulPoints[(uFSide+1)%3];
  cNew1._aulPoints[2] = rclF._aulPoints[(uFSide+2)%3];
  cNew1._aulNeighbours[0] = ulSize+1;
  cNew1._aulNeighbours[1] = rclF._aulNeighbours[(uFSide+1)%3];
  cNew1._aulNeighbours[2] = ulFacetPos;

  cNew2._aulPoints[0] = rclN._aulPoints[uNSide];
  cNew2._aulPoints[1] = uPtInd;
  cNew2._aulPoints[2] = rclN._aulPoints[(uNSide+2)%3];
  cNew2._aulNeighbours[0] = ulSize;
  cNew2._aulNeighbours[1] = ulNeighbour;
  cNew2._aulNeighbours[2] = rclN._aulNeighbours[(uNSide+2)%3];

  // adjust the facets
  rclF._aulPoints[(uFSide+1)%3] = uPtInd;
  rclF._aulNeighbours[(uFSide+1)%3] = ulSize;
  rclN._aulPoints[uNSide] = uPtInd;
  rclN._aulNeighbours[(uNSide+2)%3] = ulSize+1;

  // insert new facets
  _rclMesh._aclFacetArray.push_back(cNew1);
  _rclMesh._aclFacetArray.push_back(cNew2);
}

void MeshTopoAlgorithm::SplitOpenEdge(unsigned long ulFacetPos, unsigned short uSide, const Base::Vector3f& rP)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  if (rclF._aulNeighbours[uSide] != ULONG_MAX) 
    return; // not open

  unsigned long uPtCnt = _rclMesh._aclPointArray.size();
  unsigned long uPtInd = _rclMesh._aclPointArray.GetOrAddIndex(rP);
  unsigned long ulSize = _rclMesh._aclFacetArray.size();

  if ( uPtInd < uPtCnt )
    return; // the given point is already part of the mesh => creating new facets would be an illegal operation

  // adjust the neighbourhood
  if (rclF._aulNeighbours[(uSide+1)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclF._aulNeighbours[(uSide+1)%3]].ReplaceNeighbour(ulFacetPos, ulSize);

  MeshFacet cNew;
  cNew._aulPoints[0] = uPtInd;
  cNew._aulPoints[1] = rclF._aulPoints[(uSide+1)%3];
  cNew._aulPoints[2] = rclF._aulPoints[(uSide+2)%3];
  cNew._aulNeighbours[0] = ULONG_MAX;
  cNew._aulNeighbours[1] = rclF._aulNeighbours[(uSide+1)%3];
  cNew._aulNeighbours[2] = ulFacetPos;

  // adjust the facets
  rclF._aulPoints[(uSide+1)%3] = uPtInd;
  rclF._aulNeighbours[(uSide+1)%3] = ulSize;

  // insert new facets
  _rclMesh._aclFacetArray.push_back(cNew);
}

std::vector<unsigned long> MeshTopoAlgorithm::GetFacetsToPoint(unsigned long uFacetPos, unsigned long uPointPos) const
{
  // get all facets this point is referenced by
  std::list<unsigned long> aReference;
  aReference.push_back(uFacetPos);
  std::set<unsigned long> aRefFacet;
  while ( !aReference.empty() )
  {
    unsigned long uIndex = aReference.front();
    aReference.pop_front();
    aRefFacet.insert(uIndex);
    MeshFacet& rFace = _rclMesh._aclFacetArray[uIndex];
    for ( int i=0; i<3; i++ ) {
      if ( rFace._aulPoints[i] == uPointPos ) {
        if ( rFace._aulNeighbours[i] != ULONG_MAX )
        {
          if ( aRefFacet.find(rFace._aulNeighbours[i]) == aRefFacet.end() )
            aReference.push_back( rFace._aulNeighbours[i] );
        }
        if ( rFace._aulNeighbours[(i+2)%3] != ULONG_MAX )
        {
          if ( aRefFacet.find(rFace._aulNeighbours[(i+2)%3]) == aRefFacet.end() )
            aReference.push_back( rFace._aulNeighbours[(i+2)%3] );
        }
        break;
      }
    }
  }

  //copy the items
  std::vector<unsigned long> aRefs;
  aRefs.insert(aRefs.end(), aRefFacet.begin(), aRefFacet.end());
  return aRefs;
}

void MeshTopoAlgorithm::Cleanup()
{
  _rclMesh.RemoveInvalids();
  _needsCleanup = false;
}

bool MeshTopoAlgorithm::CollapseEdge(unsigned long ulFacetPos, unsigned long ulNeighbour)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];

  unsigned short uFSide = rclF.Side(rclN);
  unsigned short uNSide = rclN.Side(rclF);

  if (uFSide == USHRT_MAX || uNSide == USHRT_MAX) 
    return false; // not neighbours

  if (!rclF.IsValid() || !rclN.IsValid())
    return false; // the facets are marked invalid from a previous run

  // get the point index we want to remove
  unsigned long ulPointPos = rclF._aulPoints[uFSide];
  unsigned long ulPointNew = rclN._aulPoints[uNSide];

  // get all facets this point is referenced by
  std::vector<unsigned long> aRefs = GetFacetsToPoint(ulFacetPos, ulPointPos);
  for ( std::vector<unsigned long>::iterator it = aRefs.begin(); it != aRefs.end(); ++it )
  {
    MeshFacet& rFace = _rclMesh._aclFacetArray[*it];
    rFace.Transpose( ulPointPos, ulPointNew );
  }

  // set the new neighbourhood
  if (rclF._aulNeighbours[(uFSide+1)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclF._aulNeighbours[(uFSide+1)%3]].ReplaceNeighbour(ulFacetPos, rclF._aulNeighbours[(uFSide+2)%3]);
  if (rclF._aulNeighbours[(uFSide+2)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclF._aulNeighbours[(uFSide+2)%3]].ReplaceNeighbour(ulFacetPos, rclF._aulNeighbours[(uFSide+1)%3]);
  if (rclN._aulNeighbours[(uNSide+1)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+1)%3]].ReplaceNeighbour(ulNeighbour, rclN._aulNeighbours[(uNSide+2)%3]);
  if (rclN._aulNeighbours[(uNSide+2)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+2)%3]].ReplaceNeighbour(ulNeighbour, rclN._aulNeighbours[(uNSide+1)%3]);

  // isolate the both facets and the point
  rclF._aulNeighbours[0] = ULONG_MAX;
  rclF._aulNeighbours[1] = ULONG_MAX;
  rclF._aulNeighbours[2] = ULONG_MAX;
  rclF.SetInvalid();
  rclN._aulNeighbours[0] = ULONG_MAX;
  rclN._aulNeighbours[1] = ULONG_MAX;
  rclN._aulNeighbours[2] = ULONG_MAX;
  rclN.SetInvalid();
  _rclMesh._aclPointArray[ulPointPos].SetInvalid();

  _needsCleanup = true;

  return true;
}
#if 0
bool MeshTopoAlgorithm::CollapseFacet(unsigned long ulFacetPos)
{
    MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
    //if (rclF.CountOpenEdges() != 0)
    //    return false;

    if (!rclF.IsValid() )
        return false; // the facets are marked invalid from a previous run

    // set the neighbourhood of the circumjacent facets
    for ( int i=0; i<3; i++ )
    {
        if (rclF._aulNeighbours[i] == ULONG_MAX)
            continue;
        MeshFacet& rclN = _rclMesh._aclFacetArray[rclF._aulNeighbours[i]];
        unsigned short uNSide = rclN.Side(rclF);
        if (!rclN.IsValid() || uNSide == USHRT_MAX)
            continue; // the facets are marked invalid from a previous run

        if ( rclN._aulNeighbours[(uNSide+1)%3] != ULONG_MAX )
          _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+1)%3]].ReplaceNeighbour(rclF._aulNeighbours[i],rclN._aulNeighbours[(uNSide+2)%3]);
        if ( rclN._aulNeighbours[(uNSide+2)%3] != ULONG_MAX )
          _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+2)%3]].ReplaceNeighbour(rclF._aulNeighbours[i],rclN._aulNeighbours[(uNSide+1)%3]);

        // isolate the face and the point
        rclN._aulNeighbours[0] = rclN._aulNeighbours[1] = rclN._aulNeighbours[2] = ULONG_MAX;
        rclN.SetInvalid();

  }
  // move one point to gravity as replacement for the deleted face
  Base::Vector3f cCenter = _rclMesh.GetGravityPoint(rclF);
  unsigned long ptIdx0 = rclF._aulPoints[0];
  unsigned long ptIdx1 = rclF._aulPoints[1];
  unsigned long ptIdx2 = rclF._aulPoints[2];
  _rclMesh._aclPointArray[ptIdx0] = cCenter;
  // invalidate the rest of the points
  _rclMesh._aclPointArray[rclF._aulPoints[1]].SetInvalid();
  _rclMesh._aclPointArray[rclF._aulPoints[2]].SetInvalid();
  // go through all faces and replace the deleted points with the point moved to the gravity point
  for(MeshFacetArray::iterator It = _rclMesh._aclFacetArray.begin();It != _rclMesh._aclFacetArray.end(); ++ It){
      if(rclF.IsValid()){
        It->Transpose(ptIdx1,ptIdx0);
        It->Transpose(ptIdx2,ptIdx0);
      }
  }

  // isolate the face and the point
  rclF._aulNeighbours[0] = rclF._aulNeighbours[1] = rclF._aulNeighbours[2] = ULONG_MAX;
  rclF.SetInvalid();

  return true;
}
#else
bool MeshTopoAlgorithm::CollapseFacet(unsigned long ulFacetPos)
{
    MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
    if (!rclF.IsValid())
        return false; // the facet is marked invalid from a previous run

    // get the point index we want to remove
    unsigned long ulPointInd0 = rclF._aulPoints[0];
    unsigned long ulPointInd1 = rclF._aulPoints[1];
    unsigned long ulPointInd2 = rclF._aulPoints[2];

    // move the vertex to the gravity center
    Base::Vector3f cCenter = _rclMesh.GetGravityPoint(rclF);
    _rclMesh._aclPointArray[ulPointInd0] = cCenter;

    // set the new point indices for all facets that share one of the points to be deleted
    std::vector<unsigned long> aRefs = GetFacetsToPoint(ulFacetPos, ulPointInd1);
    for (std::vector<unsigned long>::iterator it = aRefs.begin(); it != aRefs.end(); ++it) {
        MeshFacet& rFace = _rclMesh._aclFacetArray[*it];
        rFace.Transpose(ulPointInd1, ulPointInd0);
    }
    
    aRefs = GetFacetsToPoint(ulFacetPos, ulPointInd2);
    for (std::vector<unsigned long>::iterator it = aRefs.begin(); it != aRefs.end(); ++it) {
        MeshFacet& rFace = _rclMesh._aclFacetArray[*it];
        rFace.Transpose(ulPointInd2, ulPointInd0);
    }

    // set the neighbourhood of the circumjacent facets
    for (int i=0; i<3; i++) {
        if (rclF._aulNeighbours[i] == ULONG_MAX)
            continue;
        MeshFacet& rclN = _rclMesh._aclFacetArray[rclF._aulNeighbours[i]];
        unsigned short uNSide = rclN.Side(rclF);

        if (rclN._aulNeighbours[(uNSide+1)%3] != ULONG_MAX) {
            _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+1)%3]]
                    .ReplaceNeighbour(rclF._aulNeighbours[i],rclN._aulNeighbours[(uNSide+2)%3]);
        }
        if (rclN._aulNeighbours[(uNSide+2)%3] != ULONG_MAX) {
            _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+2)%3]]
                    .ReplaceNeighbour(rclF._aulNeighbours[i],rclN._aulNeighbours[(uNSide+1)%3]);
        }

        // Isolate the neighbours from the topology
        rclN._aulNeighbours[0] = ULONG_MAX;
        rclN._aulNeighbours[1] = ULONG_MAX;
        rclN._aulNeighbours[2] = ULONG_MAX;
        rclN.SetInvalid();
    }

    // Isolate this facet and make two of its points invalid
    rclF._aulNeighbours[0] = ULONG_MAX;
    rclF._aulNeighbours[1] = ULONG_MAX;
    rclF._aulNeighbours[2] = ULONG_MAX;
    rclF.SetInvalid();
    _rclMesh._aclPointArray[ulPointInd1].SetInvalid();
    _rclMesh._aclPointArray[ulPointInd2].SetInvalid();

    _needsCleanup = true;

    return true;
}
#endif
/// FIXME: Implement
void MeshTopoAlgorithm::SplitFacet(unsigned long ulFacetPos, const Base::Vector3f& rP1, const Base::Vector3f& rP2)
{
  float fEps = MESH_MIN_EDGE_LEN;
  MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];
  MeshPoint& rVertex0 = _rclMesh._aclPointArray[rFace._aulPoints[0]];
  MeshPoint& rVertex1 = _rclMesh._aclPointArray[rFace._aulPoints[1]];
  MeshPoint& rVertex2 = _rclMesh._aclPointArray[rFace._aulPoints[2]];

  unsigned short equalP1=USHRT_MAX, equalP2=USHRT_MAX;
  if ( Base::Distance(rVertex0, rP1) < fEps )
    equalP1=0;
  else if ( Base::Distance(rVertex1, rP1) < fEps )
    equalP1=1;
  else if ( Base::Distance(rVertex2, rP1) < fEps )
    equalP1=2;
  if ( Base::Distance(rVertex0, rP2) < fEps )
    equalP2=0;
  else if ( Base::Distance(rVertex1, rP2) < fEps )
    equalP2=1;
  else if ( Base::Distance(rVertex2, rP2) < fEps )
    equalP2=2;

  // both points are coincident with the corner points
  if ( equalP1 != USHRT_MAX && equalP2 != USHRT_MAX )
    return; // must not split the facet

  if ( equalP1 != USHRT_MAX )
  {
    // get the edge to the second given point and perform a split edge operation
    float fMinDist = FLOAT_MAX;
    unsigned short iEdgeNo=USHRT_MAX;
    for ( unsigned short i=0; i<3; i++ )
    {
      Base::Vector3f cBase(_rclMesh._aclPointArray[rFace._aulPoints[i]]);
      Base::Vector3f cEnd (_rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]]);
      Base::Vector3f cDir = cEnd - cBase;

      float fDist = rP2.DistanceToLine(cBase, cDir);
      if ( fMinDist < fDist )
      {
        fMinDist = fDist;
        iEdgeNo = i;
      }
    }
    if ( fMinDist < 0.05f )
    {
      if ( rFace._aulNeighbours[iEdgeNo] != ULONG_MAX )
        SplitEdge(ulFacetPos, rFace._aulNeighbours[iEdgeNo], rP2);
      else
        SplitOpenEdge(ulFacetPos, iEdgeNo, rP2);
    }
  }
  else if ( equalP2 != USHRT_MAX )
  {
    // get the edge to the first given point and perform a split edge operation
    float fMinDist = FLOAT_MAX;
    unsigned short iEdgeNo=USHRT_MAX;
    for ( unsigned short i=0; i<3; i++ )
    {
      Base::Vector3f cBase(_rclMesh._aclPointArray[rFace._aulPoints[i]]);
      Base::Vector3f cEnd (_rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]]);
      Base::Vector3f cDir = cEnd - cBase;

      float fDist = rP1.DistanceToLine(cBase, cDir);
      if ( fMinDist < fDist )
      {
        fMinDist = fDist;
        iEdgeNo = i;
      }
    }
    if ( fMinDist < 0.05f )
    {
      if ( rFace._aulNeighbours[iEdgeNo] != ULONG_MAX )
        SplitEdge(ulFacetPos, rFace._aulNeighbours[iEdgeNo], rP1);
      else
        SplitOpenEdge(ulFacetPos, iEdgeNo, rP1);
    }
  }
  else
  {
    // search for the matching edges
    unsigned short iEdgeNo1=USHRT_MAX, iEdgeNo2=USHRT_MAX;
    float fMinDist1 = FLOAT_MAX, fMinDist2 = FLOAT_MAX;
    const MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];
    for ( unsigned short i=0; i<3; i++ )
    {
      Base::Vector3f cBase(_rclMesh._aclPointArray[rFace._aulPoints[i]]);
      Base::Vector3f cEnd (_rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]]);
      Base::Vector3f cDir = cEnd - cBase;

      float fDist = rP1.DistanceToLine(cBase, cDir);
      if ( fMinDist1 < fDist )
      {
        fMinDist1 = fDist;
        iEdgeNo1 = i;
      }
      fDist = rP2.DistanceToLine(cBase, cDir);
      if ( fMinDist2 < fDist )
      {
        fMinDist2 = fDist;
        iEdgeNo2 = i;
      }
    }

    if ( iEdgeNo1 == iEdgeNo2 || fMinDist1 >= 0.05f || fMinDist2 >= 0.05f ) 
      return; // no valid configuration

    // make first point lying on the previous edge
    Base::Vector3f cP1 = rP1;
    Base::Vector3f cP2 = rP2;
    if ( (iEdgeNo2+1)%3 == iEdgeNo1 )
    {
      unsigned short tmp = iEdgeNo1;
      iEdgeNo1 = iEdgeNo2;
      iEdgeNo2 = tmp;
      cP1 = rP2;
      cP2 = rP1;
    }

    // split up the facet now
    if ( rFace._aulNeighbours[iEdgeNo1] != ULONG_MAX )
      SplitNeighbourFacet(ulFacetPos, iEdgeNo1, cP1);
    if ( rFace._aulNeighbours[iEdgeNo2] != ULONG_MAX )
      SplitNeighbourFacet(ulFacetPos, iEdgeNo2, cP1);
  }
}

void MeshTopoAlgorithm::SplitNeighbourFacet(unsigned long ulFacetPos, unsigned short uFSide, const Base::Vector3f rPoint)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];

  unsigned long ulNeighbour = rclF._aulNeighbours[uFSide];
  MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];

  unsigned short uNSide = rclN.Side(rclF);

  //unsigned long uPtCnt = _rclMesh._aclPointArray.size();
  unsigned long uPtInd = _rclMesh._aclPointArray.GetOrAddIndex(rPoint);
  unsigned long ulSize = _rclMesh._aclFacetArray.size();

  // adjust the neighbourhood
  if (rclN._aulNeighbours[(uNSide+1)%3] != ULONG_MAX)
    _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+1)%3]].ReplaceNeighbour(ulNeighbour, ulSize);

  MeshFacet cNew;
  cNew._aulPoints[0] = uPtInd;
  cNew._aulPoints[1] = rclN._aulPoints[(uNSide+1)%3];
  cNew._aulPoints[2] = rclN._aulPoints[(uNSide+2)%3];
  cNew._aulNeighbours[0] = ulFacetPos;
  cNew._aulNeighbours[1] = rclN._aulNeighbours[(uNSide+1)%3];
  cNew._aulNeighbours[2] = ulNeighbour;

  // adjust the facet
  rclN._aulPoints[(uNSide+1)%3] = uPtInd;
  rclN._aulNeighbours[(uNSide+1)%3] = ulSize;

  // insert new facet
  _rclMesh._aclFacetArray.push_back(cNew);
}

#if 0
  // create 3 new facets
  MeshGeomFacet clFacet;

  // facet [P1, Ei+1, P2]
  clFacet._aclPoints[0] = cP1;
  clFacet._aclPoints[1] = _rclMesh._aclPointArray[rFace._aulPoints[(iEdgeNo1+1)%3]];
  clFacet._aclPoints[2] = cP2;
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);
  // facet [P2, Ei+2, Ei]
  clFacet._aclPoints[0] = cP2;
  clFacet._aclPoints[1] = _rclMesh._aclPointArray[rFace._aulPoints[(iEdgeNo1+2)%3]];
  clFacet._aclPoints[2] = _rclMesh._aclPointArray[rFace._aulPoints[iEdgeNo1]];
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);
  // facet [P2, Ei, P1]
  clFacet._aclPoints[0] = cP2;
  clFacet._aclPoints[1] = _rclMesh._aclPointArray[rFace._aulPoints[iEdgeNo1]];
  clFacet._aclPoints[2] = cP1;
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);
#endif

void MeshTopoAlgorithm::RemoveDegeneratedFacet(unsigned long index)
{
  if (index >= _rclMesh._aclFacetArray.size() ) return;
  MeshFacet& rFace = _rclMesh._aclFacetArray[index];

  // coincident corners (either topological or geometrical)
  for ( int i=0; i<3; i++ ) {
    const MeshPoint& rE0 = _rclMesh._aclPointArray[rFace._aulPoints[i]]; 
    const MeshPoint& rE1 = _rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]]; 
    if ( rE0 == rE1 ) {
      unsigned long uN1 = rFace._aulNeighbours[(i+1)%3];
      unsigned long uN2 = rFace._aulNeighbours[(i+2)%3];
      if ( uN2 != ULONG_MAX )
        _rclMesh._aclFacetArray[uN2].ReplaceNeighbour(index, uN1);
      if ( uN1 != ULONG_MAX )
        _rclMesh._aclFacetArray[uN1].ReplaceNeighbour(index, uN2);

      // isolate the face and remove it
      rFace._aulNeighbours[0] = ULONG_MAX;
      rFace._aulNeighbours[1] = ULONG_MAX;
      rFace._aulNeighbours[2] = ULONG_MAX;
      _rclMesh.DeleteFacet( index );
      return;
    }
  }

  // We have a facet of the form
  // P0 +----+------+P2
  //         P1
  for ( int j=0; j<3; j++ ) {
    Base::Vector3f cVec1 = _rclMesh._aclPointArray[rFace._aulPoints[(j+1)%3]] - _rclMesh._aclPointArray[rFace._aulPoints[j]];
    Base::Vector3f cVec2 = _rclMesh._aclPointArray[rFace._aulPoints[(j+2)%3]] - _rclMesh._aclPointArray[rFace._aulPoints[j]];

    // adjust the neighbourhoods and point indices
    if ( cVec1 * cVec2 < 0.0f ) {
      unsigned long uN1 = rFace._aulNeighbours[(j+1)%3];
      if ( uN1 != ULONG_MAX ) {
        // get the neighbour and common edge side
        MeshFacet& rNb = _rclMesh._aclFacetArray[uN1];
        unsigned short side = rNb.Side(index);

        // bend the point indices
        rFace._aulPoints[(j+2)%3] = rNb._aulPoints[(side+2)%3];
        rNb._aulPoints[(side+1)%3] = rFace._aulPoints[j];

        // set correct neighbourhood
        unsigned long uN2 = rFace._aulNeighbours[(j+2)%3];
        rNb._aulNeighbours[side] = uN2;
        if ( uN2 != ULONG_MAX ) {
          _rclMesh._aclFacetArray[uN2].ReplaceNeighbour(index, uN1);
        }
        unsigned long uN3 = rNb._aulNeighbours[(side+1)%3];
        rFace._aulNeighbours[(j+1)%3] = uN3;
        if ( uN3 != ULONG_MAX ) {
          _rclMesh._aclFacetArray[uN3].ReplaceNeighbour(uN1, index);
        }
        rNb._aulNeighbours[(side+1)%3] = index;
        rFace._aulNeighbours[(j+2)%3] = uN1;
      }
      else
        _rclMesh.DeleteFacet(index);

      return;
    }
  }
}

void MeshTopoAlgorithm::RemoveCorruptedFacet(unsigned long index)
{
  if (index >= _rclMesh._aclFacetArray.size() ) return;
  MeshFacet& rFace = _rclMesh._aclFacetArray[index];

  // coincident corners (topological)
  for ( int i=0; i<3; i++ ) {
    if ( rFace._aulPoints[i] == rFace._aulPoints[(i+1)%3] ) {
      unsigned long uN1 = rFace._aulNeighbours[(i+1)%3];
      unsigned long uN2 = rFace._aulNeighbours[(i+2)%3];
      if ( uN2 != ULONG_MAX )
        _rclMesh._aclFacetArray[uN2].ReplaceNeighbour(index, uN1);
      if ( uN1 != ULONG_MAX )
        _rclMesh._aclFacetArray[uN1].ReplaceNeighbour(index, uN2);

      // isolate the face and remove it
      rFace._aulNeighbours[0] = ULONG_MAX;
      rFace._aulNeighbours[1] = ULONG_MAX;
      rFace._aulNeighbours[2] = ULONG_MAX;
      _rclMesh.DeleteFacet( index );
      return;
    }
  }
}

void MeshTopoAlgorithm::FillupHoles(unsigned long length)
{
  // get the mesh boundaries as an array of point indices
  std::list<std::vector<unsigned long> > aBorders;
  MeshAlgorithm cAlgo(_rclMesh);
  cAlgo.GetMeshBorders(aBorders);

  // split boundaries if needed
  cAlgo.SplitBoundaryLoops(aBorders);

  // get the facets to a point
  MeshRefPointToFacets cPt2Fac(_rclMesh);

  MeshFacetArray newFacets;
  for ( std::list<std::vector<unsigned long> >::iterator it = aBorders.begin(); it != aBorders.end(); ++it )
  {
    // first and last vertex are identical
    if ( it->size() < 4 )
      continue; // something strange
    if ( it->size()-1 <= length )
    {
      // Get a facet as reference coordinate system
      const MeshFacet& rFace = **cPt2Fac[it->front()].begin();
      MeshGeomFacet rTriangle = _rclMesh.GetFacet(rFace);

      std::vector<Base::Vector3f> polygon;
      for ( std::vector<unsigned long>::iterator jt = it->begin(); jt != it->end(); ++jt )
        polygon.push_back( _rclMesh._aclPointArray[*jt] );

      // There is no easy way to check whether the boundary is a hole or a silhoutte before performing triangulation.
      // Afterwards we can compare the normals of the created triangles with the z-direction of our local coordinate system.
      // If the scalar product is positive it was a hole, otherwise not.
      MeshPolygonTriangulation cTria;
      cTria.SetPolygon( polygon );
      // Get the plane normal as result of the fit. The normal might be flipped so we adjust it to
      // a reference triangle (which might have quite the same normal)
      Base::Matrix4D matrix;
      Base::Vector3f cPlaneNormal = cTria.TransformToFitPlane(matrix);
      if ( rTriangle.GetNormal() * cPlaneNormal < 0.0f )
        cPlaneNormal *= -1.0f;
      if ( cTria.ComputeQuasiDelaunay() )
      {
        std::vector<MeshFacet> faces = cTria.GetFacets();
        for ( std::vector<MeshCore::MeshFacet>::iterator kt = faces.begin(); kt != faces.end(); ++kt )
        {
          if (kt == faces.begin())
          {
            MeshGeomFacet triangle;
            triangle._aclPoints[0] = polygon[kt->_aulPoints[0]];
            triangle._aclPoints[1] = polygon[kt->_aulPoints[1]];
            triangle._aclPoints[2] = polygon[kt->_aulPoints[2]];
            // do not use any of these triangles
            if ( triangle.GetNormal() * cPlaneNormal <= 0.0f )
              break;
            // Special case handling for a hole with three edges: the resulting facet might be coincident with the 
            // reference facet
            else if (faces.size()==1){
              MeshFacet first;
              first._aulPoints[0] = (*it)[kt->_aulPoints[0]];
              first._aulPoints[1] = (*it)[kt->_aulPoints[1]];
              first._aulPoints[2] = (*it)[kt->_aulPoints[2]];
              if ( first.IsEqual(rFace) )
                break;
            }
          }

          kt->_aulPoints[0] = (*it)[kt->_aulPoints[0]];
          kt->_aulPoints[1] = (*it)[kt->_aulPoints[1]];
          kt->_aulPoints[2] = (*it)[kt->_aulPoints[2]];
          newFacets.push_back(*kt);
        }
      }
    }
  }

  _rclMesh.AddFacets(newFacets);
}

void MeshTopoAlgorithm::FillupHoles(unsigned long length, float fMaxArea)
{
    // get the mesh boundaries as an array of point indices
    std::list<std::vector<unsigned long> > aBorders;
    MeshAlgorithm cAlgo(_rclMesh);
    cAlgo.GetMeshBorders(aBorders);

    // split boundary loops if needed
    cAlgo.SplitBoundaryLoops(aBorders);

    // get the facets to a point
    MeshRefPointToFacets cPt2Fac(_rclMesh);

    MeshFacetArray newFacets;
    MeshPointArray newPoints;
    unsigned long numberOfOldPoints = _rclMesh._aclPointArray.size();
    for ( std::list<std::vector<unsigned long> >::iterator it = aBorders.begin(); it != aBorders.end(); ++it ) {
        if ( it->size()-1 > length )
            continue; // boundary with too many edges
        MeshFacetArray cFacets;
        MeshPointArray cPoints;
        if (cAlgo.FillupHole(*it, fMaxArea, cFacets, cPoints, &cPt2Fac)) {
            if (it->front() == it->back())
                it->pop_back();
            // the triangulation may produce additional points which we must take into account when appending to the mesh
            unsigned long countBoundaryPoints = it->size();
            unsigned long countDifference = cPoints.size() - countBoundaryPoints;
            if (countDifference > 0) {
                MeshPointArray::_TIterator pt = cPoints.begin() + countBoundaryPoints;
                for (unsigned long i=0; i<countDifference; i++, pt++) {
                    it->push_back(numberOfOldPoints++);
                    newPoints.push_back(*pt);
                }
            }
            for (MeshFacetArray::_TIterator kt = cFacets.begin(); kt != cFacets.end(); ++kt ) {
                kt->_aulPoints[0] = (*it)[kt->_aulPoints[0]];
                kt->_aulPoints[1] = (*it)[kt->_aulPoints[1]];
                kt->_aulPoints[2] = (*it)[kt->_aulPoints[2]];
                newFacets.push_back(*kt);
            }
        }
    }

    // insert new points and faces into the mesh structure
    _rclMesh._aclPointArray.insert(_rclMesh._aclPointArray.end(), newPoints.begin(), newPoints.end());
    _rclMesh.AddFacets(newFacets);
}

void MeshTopoAlgorithm::RemoveComponents(unsigned long count)
{
  std::vector<std::vector<unsigned long> > segments;
  MeshComponents comp(_rclMesh);
  comp.SearchForComponents(MeshComponents::OverEdge,segments);

  std::vector<unsigned long> removeFacets;
  for ( std::vector<std::vector<unsigned long> >::iterator it = segments.begin(); it != segments.end(); ++it ) {
    if ( it->size() <= (unsigned long)count )
      removeFacets.insert( removeFacets.end(), it->begin(), it->end() );
    }

  if ( !removeFacets.empty() )
    _rclMesh.DeleteFacets( removeFacets );
}

void MeshTopoAlgorithm::HarmonizeNormals (void)
{
  std::vector<unsigned long> uIndices = MeshEvalOrientation(_rclMesh).GetIndices();
  for ( std::vector<unsigned long>::iterator it = uIndices.begin(); it != uIndices.end(); ++it )
    _rclMesh._aclFacetArray[*it].FlipNormal();
}

void MeshTopoAlgorithm::FlipNormals (void)
{
  for (MeshFacetArray::_TIterator i = _rclMesh._aclFacetArray.begin(); i < _rclMesh._aclFacetArray.end(); i++)
    i->FlipNormal();
}

// ---------------------------------------------------------------------------

/**
 * Some important formulas:
 *
 * Ne = 3Nv - Nb + 3B + 6(G-R)
 * Nt = 2Nv - Nb + 2B + 4(G-R)
 *
 * Ne <= 3Nv + 6(G-R)
 * Nt <= 2Nv + 4(G-R)
 *
 * Ne ~ 3Nv, Nv >> G, Nv >> R
 * Nt ~ 2Nv, Nv >> G, Nv >> R
 *
 * Ne = #Edges
 * Nt = #Facets
 * Nv = #Vertices
 * Nb = #Boundary vertices
 * B  = #Boundaries
 * G  = Genus (Number of holes)
 * R  = #components
 */

MeshComponents::MeshComponents( const MeshKernel& rclMesh )
: _rclMesh(rclMesh)
{
}

MeshComponents::~MeshComponents()
{
}

void MeshComponents::SearchForComponents(TMode tMode, std::vector<std::vector<unsigned long> >& aclT) const
{
  // all facets
  std::vector<unsigned long> aulAllFacets(_rclMesh.CountFacets());
  unsigned long k = 0;
  for (std::vector<unsigned long>::iterator pI = aulAllFacets.begin(); pI != aulAllFacets.end(); pI++)
    *pI = k++;

  SearchForComponents( tMode, aulAllFacets, aclT );
}

void MeshComponents::SearchForComponents(TMode tMode, const std::vector<unsigned long>& aSegment, std::vector<std::vector<unsigned long> >& aclT) const
{
  unsigned long ulStartFacet, ulVisited;

  if (_rclMesh.CountFacets() == 0)
    return;

  // reset VISIT flags
  MeshAlgorithm cAlgo(_rclMesh);
  cAlgo.SetFacetFlag(MeshFacet::VISIT);
  cAlgo.ResetFacetsFlag(aSegment, MeshFacet::VISIT);
  
  const MeshFacetArray& rFAry = _rclMesh.GetFacets();
  MeshFacetArray::_TConstIterator iTri = rFAry.begin();
  MeshFacetArray::_TConstIterator iBeg = rFAry.begin();
  MeshFacetArray::_TConstIterator iEnd = rFAry.end();

  // start from the first not visited facet
  ulVisited = cAlgo.CountFacetFlag(MeshFacet::VISIT);
  iTri = std::find_if(iTri, iEnd, std::bind2nd(MeshIsNotFlag<MeshFacet>(), MeshFacet::VISIT));
  ulStartFacet = iTri - iBeg;

  // visitor
  std::vector<unsigned long> aclComponent;
  std::vector<std::vector<unsigned long> > aclConnectComp;
  MeshTopFacetVisitor clFVisitor( aclComponent );

  while ( ulStartFacet !=  ULONG_MAX )
  {
    // collect all facets of a component
    aclComponent.clear();
    if (tMode == OverEdge)
      ulVisited += _rclMesh.VisitNeighbourFacets(clFVisitor, ulStartFacet);
    else if (tMode == OverPoint)
      ulVisited += _rclMesh.VisitNeighbourFacetsOverCorners(clFVisitor, ulStartFacet);

    // get also start facet
    aclComponent.push_back(ulStartFacet);
    aclConnectComp.push_back(aclComponent);

    // if the mesh consists of several topologic independent components
    // We can search from position 'iTri' on because all elements _before_ are already visited
    // what we know from the previous iteration.
    iTri = std::find_if(iTri, iEnd, std::bind2nd(MeshIsNotFlag<MeshFacet>(), MeshFacet::VISIT));

    if (iTri < iEnd)
      ulStartFacet = iTri - iBeg;
    else
      ulStartFacet = ULONG_MAX;
  }

  // sort components by size (descending order)
  std::sort(aclConnectComp.begin(), aclConnectComp.end(), CNofFacetsCompare());  
  aclT = aclConnectComp;
}
