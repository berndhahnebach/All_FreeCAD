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

#include "TopoAlgorithm.h"
#include "Iterator.h"
#include "MeshKernel.h"
#include "Algorithm.h"
#include "Evaluation.h"
#include "triangle.h"


using namespace MeshCore;

MeshTopoAlgorithm::MeshTopoAlgorithm (MeshKernel &rclM)
: _rclMesh(rclM)
{
}

MeshTopoAlgorithm::~MeshTopoAlgorithm (void)
{
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
  for (pE = aEdge2Face.begin(); pE != aEdge2Face.end(); ++pE)
    aEdgeList.push_back(pE->first);

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

/// FIXME: Implement
bool MeshTopoAlgorithm::CollapseEdge(unsigned long ulEdgeP0, unsigned long ulEdgeP1)
{
#if 0
  RefPointToFacet();
  std::set<MeshFacetArray::_TConstIterator> aclNeighbours0 = (*_pclRefPt2Fac)[ulEdgeP0];
  std::set<MeshFacetArray::_TConstIterator> aclNeighbours1 = (*_pclRefPt2Fac)[ulEdgeP1];

  if (aclNeighbours0.size() < 3 || aclNeighbours1.size() < 3)
    return false; // this must be a border point => must not remove

  Base::Vector3f clNew = 0.5f*(_rclMesh._aclPointArray[ulEdgeP0]+_rclMesh._aclPointArray[ulEdgeP1]);

  // check point 0
  std::vector<unsigned long> clPoly;
  if ( !GetPolygonOfNeighbours(ulEdgeP0, clPoly) )
    return false;
  if ( !IsConvexPolygon(clPoly) )
    return false;

  // check point 1
  clPoly.clear();
  if ( !GetPolygonOfNeighbours(ulEdgeP1, clPoly) )
    return false;
  if ( !IsConvexPolygon(clPoly) )
    return false;

  MeshGeomFacet clFacet;
  for (std::set<MeshFacetArray::_TConstIterator>::iterator it0 = aclNeighbours0.begin(); it0 != aclNeighbours0.end(); ++it0)
  {
    (*it0)->SetFlag(MeshFacet::INVALID); // mark for deletion

    bool remove = false;
    for (int i=0; i<3; ++i)
    {
      if ((*it0)->_aulPoints[i] == ulEdgeP1)
      {
        remove = true;
        break;
      }

      if ((*it0)->_aulPoints[i] == ulEdgeP0)
        clFacet._aclPoints[i] = clNew;
      else
        clFacet._aclPoints[i] = _rclMesh._aclPointArray[(*it0)->_aulPoints[i]];
    }

    if (!remove)
    {
      clFacet.CalcNormal();
      _aclNewFacets.push_back(clFacet);
    }
  }
  for (std::set<MeshFacetArray::_TConstIterator>::iterator it1 = aclNeighbours1.begin(); it1 != aclNeighbours1.end(); ++it1)
  {
    (*it1)->SetFlag(MeshFacet::INVALID); // mark for deletion

    bool remove = false;
    for (int i=0; i<3; ++i)
    {
      if ((*it1)->_aulPoints[i] == ulEdgeP0)
      {
        remove = true;
        break;
      }

      if ((*it1)->_aulPoints[i] == ulEdgeP1)
        clFacet._aclPoints[i] = clNew;
      else
        clFacet._aclPoints[i] = _rclMesh._aclPointArray[(*it1)->_aulPoints[i]];
    }

    if (!remove)
    {
      clFacet.CalcNormal();
      _aclNewFacets.push_back(clFacet);
    }
  }
#endif
  return true;
}
///FIXME: Creates non-manifolds
bool MeshTopoAlgorithm::CollapseFacet(unsigned long ulFacetPos)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  if (rclF.CountOpenEdges() != 0)
    return false;
  
  // set the neighbourhood of the circumjacent facets
  for ( int i=0; i<3; i++ )
  {
    MeshFacet& rclN = _rclMesh._aclFacetArray[rclF._aulNeighbours[i]];
    unsigned short uNSide = rclN.Side(rclF);

    if ( rclN._aulNeighbours[(uNSide+1)%3] != ULONG_MAX )
    {
      _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+1)%3]].ReplaceNeighbour(rclF._aulNeighbours[i],rclN._aulNeighbours[(uNSide+2)%3]);
      _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+1)%3]].Transpose(rclN._aulPoints[(uNSide+1)%3],rclF._aulPoints[0]);
    }
    if ( rclN._aulNeighbours[(uNSide+2)%3] != ULONG_MAX )
    {
      _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+2)%3]].ReplaceNeighbour(rclF._aulNeighbours[i],rclN._aulNeighbours[(uNSide+1)%3]);
      _rclMesh._aclFacetArray[rclN._aulNeighbours[(uNSide+2)%3]].Transpose(rclN._aulPoints[uNSide],rclF._aulPoints[0]);
    }
  }

  // FIXME: quite ineffective
  //
  Base::Vector3f cCenter = _rclMesh.GetGravityPoint(rclF);
  _rclMesh._aclPointArray[rclF._aulPoints[0]] = cCenter;

  std::vector<unsigned long> remPoints;
  remPoints.push_back(rclF._aulPoints[1]);
  remPoints.push_back(rclF._aulPoints[2]);

  _rclMesh._aclFacetArray.TransposeIndices(rclF._aulPoints[1], rclF._aulPoints[0]);
  _rclMesh._aclFacetArray.TransposeIndices(rclF._aulPoints[2], rclF._aulPoints[0]);

  rclF._aulPoints[0] = remPoints[0];
  _rclMesh._aclFacetArray[rclF._aulNeighbours[0]]._aulPoints[0] = remPoints[0];
  _rclMesh._aclFacetArray[rclF._aulNeighbours[1]]._aulPoints[0] = remPoints[0];
  _rclMesh._aclFacetArray[rclF._aulNeighbours[0]]._aulPoints[2] = remPoints[0];
  _rclMesh.DeletePoints(remPoints);

  return true;
}

/// FIXME: Implement
void MeshTopoAlgorithm::SplitFacet(unsigned long ulFacetPos, const Base::Vector3f& rP1, const Base::Vector3f& rP2)
{
  // search for the matching edges
  unsigned short iEdgeNo1=USHRT_MAX, iEdgeNo2=USHRT_MAX;
  const MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];
  for ( unsigned short i=0; i<3; i++ )
  {
    Base::Vector3f cBase(_rclMesh._aclPointArray[rFace._aulPoints[i]]);
    Base::Vector3f cEnd (_rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]]);
    Base::Vector3f cDir = cEnd - cBase;

    if ( rP1.DistanceToLine(cBase, cDir) < /*MESH_MIN_PT_DIST*/0.05f )
    {
      iEdgeNo1 = i;
    }
    else if ( rP2.DistanceToLine(cBase, cDir) < /*MESH_MIN_PT_DIST*/0.05f )
    {
      iEdgeNo2 = i;
    }
  }

  if ( iEdgeNo1 == USHRT_MAX || iEdgeNo2 == USHRT_MAX )
    return; // no two different edge

#if 0
  // rP1 should lie at the edge with the previous index
  Base::Vector3f cP1 = rP1;
  Base::Vector3f cP2 = rP2;

  if ( (iEdgeNo2+1)%3 == iEdgeNo2 )
  {
    int tmp = iEdgeNo1;
    iEdgeNo1 = iEdgeNo2;
    iEdgeNo2 = tmp;
    cP1 = rP2;
    cP2 = rP1;
  }

  if (IsFlag(ulFacetPos)) return; // already marked as INVALID
  SetFlag(ulFacetPos); // mark for deletion

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
}

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

void MeshTopoAlgorithm::SplitBoundaryLoops( std::list<std::vector<unsigned long> >& aBorders )
{
  // Count the number of open edges for each point
  std::map<unsigned long, int> openPointDegree;
  for ( MeshFacetArray::_TConstIterator jt = _rclMesh._aclFacetArray.begin(); jt != _rclMesh._aclFacetArray.end(); ++jt ) 
  {
    for ( int i=0; i<3; i++ ) {
      if ( jt->_aulNeighbours[i] == ULONG_MAX ) {
        openPointDegree[jt->_aulPoints[i]]++;
        openPointDegree[jt->_aulPoints[(i+1)%3]]++;
      }
    }
  }

  // go through all boundaries and split them if needed
  std::list<std::vector<unsigned long> > aSplitBorders;
  for ( std::list<std::vector<unsigned long> >::iterator it = aBorders.begin(); it != aBorders.end(); ++it )
  {
    bool split=false;
    for ( std::vector<unsigned long>::iterator jt = it->begin(); jt != it->end(); ++jt )
    {
      // two (ore more) boundaries meet in one non-manifold point
      if ( openPointDegree[*jt] > 2) {
        split = true;
        break;
      }
    }

    if ( !split )
      aSplitBorders.push_back( *it );
    else
      SplitBoundaryLoops( *it, aSplitBorders );
  }

  aBorders = aSplitBorders;
}

void MeshTopoAlgorithm::SplitBoundaryLoops( const std::vector<unsigned long>& rBound, std::list<std::vector<unsigned long> >& aBorders )
{
  std::map<unsigned long, int> aPtDegree;
  std::vector<unsigned long> cBound;
  for ( std::vector<unsigned long>::const_iterator it = rBound.begin(); it != rBound.end(); ++it )
  {
    int deg = (aPtDegree[*it]++);
    if ( deg > 0 ) {
      for ( std::vector<unsigned long>::iterator jt = cBound.begin(); jt != cBound.end(); ++jt ) {
        if ( *jt == *it ) {
          std::vector<unsigned long> cBoundLoop;
          cBoundLoop.insert(cBoundLoop.end(), jt, cBound.end());
          cBoundLoop.push_back( *it );
          cBound.erase(jt, cBound.end());
          aBorders.push_back( cBoundLoop );
          (aPtDegree[*it]--);
          break;
        }
      }
    }

    cBound.push_back( *it );
  }
}

void MeshTopoAlgorithm::FillupHoles(unsigned long length)
{
  // get the mesh boundaries as an array of point indices
  std::list<std::vector<unsigned long> > aBorders;
  MeshAlgorithm cAlgo(_rclMesh);
  cAlgo.GetMeshBorders(aBorders);

  // split boundaries if needed
  SplitBoundaryLoops(aBorders);

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
      Base::Vector3f cPlaneNormal = cTria.TransformToFitPlane();
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
            // do not any of these triangles
            if ( triangle.GetNormal() * cPlaneNormal <= 0.0f )
              break;
            // Special case handling for a hole with tree edges: the resulting facet might be coincident with the 
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

  _rclMesh.AddFacet(newFacets);
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
