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
# include <Wm3Vector3.h>
# include <Wm3DistVector3Segment3.h>
# include <algorithm>
#endif

#include "TopoAlgorithm.h"
#include "Iterator.h"
#include "MeshKernel.h"
#include "Algorithm.h"
#include "Evaluation.h"
#include "triangle.h"


using namespace MeshCore;
using namespace Wm3;

MeshTopoAlgorithm::MeshTopoAlgorithm (MeshKernel &rclM)
: _rclMesh(rclM), _pclRefPt2Fac(0L)
{
}

MeshTopoAlgorithm::~MeshTopoAlgorithm (void)
{
  delete _pclRefPt2Fac;
  _pclRefPt2Fac = NULL;
  _aclNewFacets.clear();
  _aclMultiSplitted.clear();
}

void MeshTopoAlgorithm::Commit()
{
  // get all not to be deleted facets
  //
  if (_aclNewFacets.size() > 0 || _aclMultiSplitted.size() > 0)
  {
    for (std::map<unsigned long, std::vector<MeshGeomFacet> >::iterator it = _aclMultiSplitted.begin(); 
      it!=_aclMultiSplitted.end(); ++it)
    {
      for (std::vector<MeshGeomFacet>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
      {
        _aclNewFacets.push_back(*it2);
      }
    }
    _aclMultiSplitted.clear();

    for (MeshFacetIterator cFIter(_rclMesh, 0); cFIter.More(); cFIter.Next())
    {
      if ( !cFIter->IsFlag(MeshFacet::INVALID) )
        _aclNewFacets.push_back(*cFIter);
    }

    Base::Sequencer().setLocked( true );
    _rclMesh = _aclNewFacets;
    if (_pclRefPt2Fac) _pclRefPt2Fac->Rebuild();
    Base::Sequencer().setLocked( false );
    _aclNewFacets.clear();
  }
}

void MeshTopoAlgorithm::Discard()
{
  _aclNewFacets.clear();
  _aclMultiSplitted.clear();
  ClearFlag();
}

void MeshTopoAlgorithm::InsertNode(unsigned long ulFacetPos, const Base::Vector3f&  rclPoint)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  if (IsFlag(ulFacetPos)) return; // already marked as INVALID
  SetFlag(ulFacetPos); // mark for deletion

  MeshGeomFacet clFacet;

  // create three new facets
  //
  clFacet._aclPoints[0] = _rclMesh._aclPointArray[rclF._aulPoints[0]];
  clFacet._aclPoints[1] = _rclMesh._aclPointArray[rclF._aulPoints[1]];
  clFacet._aclPoints[2] = rclPoint;
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);

  clFacet._aclPoints[0] = rclPoint;
  clFacet._aclPoints[1] = _rclMesh._aclPointArray[rclF._aulPoints[1]];
  clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclF._aulPoints[2]];
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);

  clFacet._aclPoints[0] = _rclMesh._aclPointArray[rclF._aulPoints[0]];
  clFacet._aclPoints[1] = rclPoint;
  clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclF._aulPoints[2]];
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);
}

void MeshTopoAlgorithm::OptimizeTopology()
{
  //TODO: Implement without EdgeList
  throw "sorry, not yet implemented";

  //std::vector<std::set<unsigned long> > clNPoints;
  //clNPoints.resize(_rclMesh._aclPointArray.size());
  //unsigned long ulPt0, ulPt1;

  //// get all current edges
  ////
  //for (MeshFacetArray::_TIterator pFIter = _rclMesh._aclFacetArray.begin(); pFIter != _rclMesh._aclFacetArray.end(); pFIter++)
  //{
  //  unsigned long ulP0 = pFIter->_aulPoints[0];
  //  unsigned long ulP1 = pFIter->_aulPoints[1];
  //  unsigned long ulP2 = pFIter->_aulPoints[2];

  //  clNPoints[ulP0].insert(ulP1);
  //  clNPoints[ulP0].insert(ulP2);
  //  clNPoints[ulP1].insert(ulP0);
  //  clNPoints[ulP1].insert(ulP2);
  //  clNPoints[ulP2].insert(ulP0);
  //  clNPoints[ulP2].insert(ulP1);
  //}

  //MeshFacetIterator clFIter1(_rclMesh);
  //MeshFacetIterator clFIter2(_rclMesh);
  //// first reset VISITed flag
  //MeshAlgorithm(_rclMesh).ResetFacetFlag(MeshFacet::VISIT);

  //Base::Vector3f p0, p1, q0, q1;
  //MeshFacet clF, clN;
  //for (MeshEdgeArray::_TIterator it = _rclMesh._aclEdgeArray.begin(); it != _rclMesh._aclEdgeArray.end(); ++it)
  //{
  //  Base::Sequencer().next();

  //  clF = _rclMesh._aclFacetArray[it->Index()];
  //  if (clF._aulNeighbours[it->Side()] == ULONG_MAX)
  //    continue;

  //  clN = _rclMesh._aclFacetArray[clF._aulNeighbours[it->Side()]];

  //  if (clF.IsFlag(MeshFacet::VISIT) || clN.IsFlag(MeshFacet::VISIT))
  //    continue;

  //  clFIter1.Set(it->Index());
  //  const MeshGeomFacet& rclF = *clFIter1;
  //  clFIter2.Set(clF._aulNeighbours[it->Side()]);
  //  const MeshGeomFacet& rclN = *clFIter2;

  //  if (rclF.GetNormal() * rclN.GetNormal() < 0.95f)
  //    continue;

  //  // makes it sense to swap?
  //  //
  //  // Current length of the edge
  //  p0 = _rclMesh._aclPointArray[clF._aulPoints[it->Side()]];
  //  p1 = _rclMesh._aclPointArray[clF._aulPoints[(it->Side()+1)%3]];

  //  ulPt0 = clF._aulPoints[(it->Side()+2)%3];
  //  q0 = _rclMesh._aclPointArray[ulPt0];
  //  for (int i=0; i<3; ++i)
  //  {
  //    if (clN._aulNeighbours[i] == it->Index())
  //    {
  //      ulPt1 = clN._aulPoints[(i+2)%3];
  //      q1 = _rclMesh._aclPointArray[ulPt1];
  //      break;
  //    }
  //  }

  //  // get all neighbour points (to check the edges)
  //  if (clNPoints[ulPt0].find(ulPt1) != clNPoints[ulPt0].end())
  //    continue;
  //  if (clNPoints[ulPt1].find(ulPt0) != clNPoints[ulPt1].end())
  //    continue;

  //  if ((q1-q0).Length() < (p1-p0).Length())
  //  {
  //    // is polygon convex ?
  //    //
  //    float a0 = (q0-p0).GetAngle(p1-p0);
  //    float a1 = (p1-p0).GetAngle(q1-p0);

  //    float b0 = (q0-p1).GetAngle(p0-p1);
  //    float b1 = (p0-p1).GetAngle(q1-p1);

  //    if ((a0+a1<3.0f) && (b0+b1<3.0f))
  //    {
  //      if (((q1-p0)%(q0-p0))*rclF.GetNormal() < FLOAT_EPS)
  //      {
  //        continue;
  //      }
  //      if (((q1-p0)%(q0-p0))*rclN.GetNormal() < FLOAT_EPS)
  //      {
  //        continue;
  //      }

  //      SwapEdge(it->Index(), it->Side());
  //    
  //      clF.SetFlag(MeshFacet::VISIT);
  //      clN.SetFlag(MeshFacet::VISIT);

  //      // append the new edge to the point neighbourhood
  //      clNPoints[ulPt0].insert(ulPt1);
  //      clNPoints[ulPt1].insert(ulPt0);
  //    }
  //  }
  //}
}


void MeshTopoAlgorithm::SwapEdge(unsigned long ulFacetPos, int iSide)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  if (IsFlag(ulFacetPos)) return; // already marked as INVALID
  unsigned long ulNeighbour = rclF._aulNeighbours[iSide];
  if (ulNeighbour == ULONG_MAX) return;
  MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];
  if (IsFlag(ulNeighbour)) return; // already marked as INVALID

  SetFlag(ulFacetPos); // mark for deletion
  SetFlag(ulNeighbour); // mark for deletion

  MeshGeomFacet clFacet1, clFacet2;
  clFacet1._aclPoints[0] = _rclMesh._aclPointArray[rclF._aulPoints[iSide]];
  clFacet1._aclPoints[2] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+2)%3]];

  clFacet2._aclPoints[0] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+1)%3]];
  clFacet2._aclPoints[1] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+2)%3]];

  for (int i=0; i<3; ++i)
  {
    if (rclN._aulNeighbours[i] == ulFacetPos)
    {
      clFacet1._aclPoints[1] = _rclMesh._aclPointArray[rclN._aulPoints[(i+2)%3]];
      clFacet2._aclPoints[2] = _rclMesh._aclPointArray[rclN._aulPoints[(i+2)%3]];
      break;
    }
  }

  _aclNewFacets.push_back(clFacet1);
  _aclNewFacets.push_back(clFacet2);
}
/// @todo
bool MeshTopoAlgorithm::MultiSplitOpenEdge(unsigned long ulFacetPos, int iSide, const Base::Vector3f& rclPoint, float fTolerance)
{
  assert(ulFacetPos < _rclMesh._aclFacetArray.size());
  MeshFacetIterator cFIter(_rclMesh, ulFacetPos);
  for (int ii=0; ii<3; ii++)
  {
    if (cFIter->_aclPoints[ii] == rclPoint)
    {
      return false;
    }
  }

  SetFlag(ulFacetPos); // mark for deletion
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  // store also the side index
  unsigned long ulIndex = ((ulFacetPos << 2) | (iSide & 3)); 

  // check if this facet is already splitted at another side
  for (int i=0; i<3; ++i)
  {
    unsigned long ulTestInd = ((ulFacetPos << 2) | (i & 3));
    if (ulTestInd != ulIndex)
    {
      if (_aclMultiSplitted.find(ulTestInd) != _aclMultiSplitted.end())
        return false; // not allowed to split
    }
  }

  // not yet splitted => just append facet
  if (_aclMultiSplitted.find(ulIndex) == _aclMultiSplitted.end())
  {
    MeshGeomFacet clFacet;

    // first facet
    clFacet._aclPoints[0] = _rclMesh._aclPointArray[rclF._aulPoints[iSide]];
    clFacet._aclPoints[1] = rclPoint;
    clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+2)%3]];
    clFacet.CalcNormal();
    _aclMultiSplitted[ulIndex].push_back(clFacet);

    // second facet
    clFacet._aclPoints[0] = rclPoint;
    clFacet._aclPoints[1] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+1)%3]];
    clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+2)%3]];
    clFacet.CalcNormal();
    _aclMultiSplitted[ulIndex].push_back(clFacet);

    return true;
  }
  // modify the already buffered facets
  else
  {
    // search for the already inserted facet containing this point
    for (std::vector<MeshGeomFacet>::iterator it = _aclMultiSplitted[ulIndex].begin(); 
      it != _aclMultiSplitted[ulIndex].end(); ++it)
    {
      MeshGeomFacet testFacet;
      testFacet._aclPoints[0] = it->_aclPoints[0];
      testFacet._aclPoints[1] = it->_aclPoints[1];
      testFacet._aclPoints[2] = it->_aclPoints[2];

      if ( (rclPoint == it->_aclPoints[0]) || (rclPoint == it->_aclPoints[1]))
        return false; // do not split with the same point twice or a facet point!!!

      Vector3<float> B(rclPoint.x, rclPoint.y, rclPoint.z);
      Vector3<float> C(it->_aclPoints[0].x, it->_aclPoints[0].y, it->_aclPoints[0].z);
      Vector3<float> D(it->_aclPoints[1].x, it->_aclPoints[1].y, it->_aclPoints[1].z);
      Vector3<float> P(0.5f*(C+D));
      Vector3<float> dCD(D-C);
      float len = dCD.Length(); dCD.Normalize();
      Segment3<float> akCD(P, dCD, len);

      DistVector3Segment3<float> akDist(B, akCD);
      float l= akDist.Get();

      if (l < fTolerance)
      {
        MeshGeomFacet clFacet;

        // first facet
        clFacet._aclPoints[0] = it->_aclPoints[0];
        clFacet._aclPoints[1] = rclPoint;
        clFacet._aclPoints[2] = it->_aclPoints[2];
        clFacet.CalcNormal();

        // second facet (change the existing facet)
        it->_aclPoints[0] = rclPoint;
        it->CalcNormal();

        // insert the new facet
        _aclMultiSplitted[ulIndex].push_back(clFacet);

        return true;
      }
    }
  }

  // ignored
  return false;
}

void MeshTopoAlgorithm::SplitEdge(unsigned long ulFacetPos, int iSide, const Base::Vector3f& rclPoint)
{
  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
  if (IsFlag(ulFacetPos)) return; // already marked as INVALID
  SetFlag(ulFacetPos); // mark for deletion

  MeshGeomFacet clFacet;

  // create the new facets
  //
  //
  unsigned long ulNeighbour = rclF._aulNeighbours[iSide];
  if (ulNeighbour != ULONG_MAX)
  {
    MeshFacet& rclN = _rclMesh._aclFacetArray[ulNeighbour];
    if (IsFlag(ulNeighbour)) return; // already marked as INVALID
    SetFlag(ulNeighbour); // mark for deletion
    for (int i=0; i<3; ++i)
    {
      if (rclN._aulNeighbours[i] == ulFacetPos)
      {
        // third facet
        clFacet._aclPoints[0] = _rclMesh._aclPointArray[rclN._aulPoints[i]];
        clFacet._aclPoints[1] = rclPoint;
        clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclN._aulPoints[(i+2)%3]];
        clFacet.CalcNormal();
        _aclNewFacets.push_back(clFacet);

        // fourth facet
        clFacet._aclPoints[0] = rclPoint;
        clFacet._aclPoints[1] = _rclMesh._aclPointArray[rclN._aulPoints[(i+1)%3]];
        clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclN._aulPoints[(i+2)%3]];
        clFacet.CalcNormal();
        _aclNewFacets.push_back(clFacet);

        break;
      }
    }
  }

  // first facet
  clFacet._aclPoints[0] = _rclMesh._aclPointArray[rclF._aulPoints[iSide]];
  clFacet._aclPoints[1] = rclPoint;
  clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+2)%3]];
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);

  // second facet
  clFacet._aclPoints[0] = rclPoint;
  clFacet._aclPoints[1] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+1)%3]];
  clFacet._aclPoints[2] = _rclMesh._aclPointArray[rclF._aulPoints[(iSide+2)%3]];
  clFacet.CalcNormal();
  _aclNewFacets.push_back(clFacet);
}

void MeshTopoAlgorithm::SplitFacet(unsigned long ulFacetPos, const Base::Vector3f& rP1, const Base::Vector3f& rP2)
{
  // search for the matching edges
  int iEdgeNo1=-1, iEdgeNo2=-1;
  const MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];
  for ( int i=0; i<3; i++ )
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

  if ( iEdgeNo1 == -1 || iEdgeNo2 == -1 )
    return; // no two different edge

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
}

bool MeshTopoAlgorithm::CollapseEdge(unsigned long ulEdgeP0, unsigned long ulEdgeP1)
{
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

  return true;
}

bool MeshTopoAlgorithm::InverseInsertNode(unsigned long ulPointPos)
{
  RefPointToFacet();
  std::set<MeshFacetArray::_TConstIterator> aclNeighbours = (*_pclRefPt2Fac)[ulPointPos];
  if (aclNeighbours.size() == 3)
  {
    std::vector<unsigned long> aulPoly;
    if ( !GetPolygonOfNeighbours(ulPointPos, aulPoly) )
      return false; // Oops, this should never occur

    if (TriangulatePolygon(aulPoly, _aclNewFacets))
    {
      for (std::set<MeshFacetArray::_TConstIterator>::iterator it = aclNeighbours.begin(); it != aclNeighbours.end(); ++it)
      {
        (*it)->SetFlag(MeshFacet::INVALID); // mark for deletion
      }
    }
    else
      return false;

    return true;
  }

  return false;
}

bool MeshTopoAlgorithm::CollapseNode(unsigned long ulPointPos)
{
  RefPointToFacet();
  std::set<MeshFacetArray::_TConstIterator> aclNeighbours = (*_pclRefPt2Fac)[ulPointPos];
  if (aclNeighbours.size() < 3)
    return false; // this must be a border point => must not remove

  std::vector<unsigned long> clPoly;
  if ( !GetPolygonOfNeighbours(ulPointPos, clPoly) )
    return false;

  if ( !IsConvexPolygon(clPoly) )
    return false;

  if (TriangulatePolygon(clPoly, _aclNewFacets))
  {
    for (std::set<MeshFacetArray::_TConstIterator>::iterator it = aclNeighbours.begin(); it != aclNeighbours.end(); ++it)
    {
      (*it)->SetFlag(MeshFacet::INVALID); // mark for deletion
    }
  }
  else
    return false;

  return true;
}

bool MeshTopoAlgorithm::IsConvexPolygon(const std::vector<unsigned long>& raulPoly)
{
  unsigned long ulCt = raulPoly.size();
  std::vector<unsigned long>::const_iterator pBegin = raulPoly.begin();

  Base::Vector3f a, b, c;
  Base::Vector3f clNormal;

  // get reference normal first
  //
  a = _rclMesh._aclPointArray[*(pBegin + 1     )]  - _rclMesh._aclPointArray[*(pBegin)];
  b = _rclMesh._aclPointArray[*(pBegin + ulCt-1)]  - _rclMesh._aclPointArray[*(pBegin)];
  clNormal = a % b;

  // compare with each calculated vector
  //
  for (unsigned long i=0; i<ulCt; ++i)
  {
    unsigned long ulCurr = i;
    unsigned long ulNext = (i+1)%ulCt;
    unsigned long ulPrev = (i-1+ulCt)%ulCt;

    a = _rclMesh._aclPointArray[*(pBegin + ulNext)]  - _rclMesh._aclPointArray[*(pBegin + ulCurr)];
    b = _rclMesh._aclPointArray[*(pBegin + ulPrev)]  - _rclMesh._aclPointArray[*(pBegin + ulCurr)];
    c = a % b;
    if (c * clNormal < FLOAT_EPS)
      return false; // not convex
  }

  return true;
}

bool MeshTopoAlgorithm::GetPolygonOfNeighbours(unsigned long ulPointPos, std::vector<unsigned long>& raulPoly)
{
  RefPointToFacet();
  raulPoly.clear();
  std::set<MeshFacetArray::_TConstIterator> aclNeighbours = (*_pclRefPt2Fac)[ulPointPos];

  MeshFacetArray::_TConstIterator pFirst = *aclNeighbours.begin();
  aclNeighbours.erase(aclNeighbours.begin());

  unsigned long ulStartPoint;
  for (int i=0; i<3; ++i)
  {
    if (pFirst->_aulPoints[i] == ulPointPos)
    {
      ulStartPoint = pFirst->_aulPoints[(i+2)%3];
      raulPoly.push_back(ulStartPoint);
      break;
    }
  }

  while (aclNeighbours.size() > 0)
  {
    bool found=false;
    for (std::set<MeshFacetArray::_TConstIterator>::iterator it = aclNeighbours.begin(); !found&&it!= aclNeighbours.end(); ++it)
    {
      MeshFacetArray::_TConstIterator pElement = *it;
      for (int i=0; i<3; ++i)
      {
        if (pElement->_aulPoints[i] == ulPointPos && pElement->_aulPoints[(i+1)%3] == ulStartPoint)
        {
          ulStartPoint = pElement->_aulPoints[(i+2)%3];
          raulPoly.push_back(ulStartPoint);
          aclNeighbours.erase(it);
          found = true;
          break;
        }
      }
    }

    if (found == false)
      return false;
  }

  return true;
}

struct TVertexEval
{
  unsigned long ulNext;
  unsigned long ulPrev;
  unsigned long ulVertex;
  float fEval;

  bool operator < (const TVertexEval& v) const
  {
    return fEval < v.fEval;
  }
};

bool MeshTopoAlgorithm::TriangulatePolygon(const std::vector<unsigned long>& raulPoly, std::vector<MeshGeomFacet>& raclFacets)
{
  std::vector<MeshGeomFacet> aclFacets;
  std::vector<unsigned long> aulPolygon = raulPoly;
  unsigned long ulCnt = aulPolygon.size();

  MeshGeomFacet clFacet;
  for (unsigned long i=0; i<ulCnt-2; ++i)
  {
    clFacet._aclPoints[0] = _rclMesh._aclPointArray[aulPolygon[0]];
    clFacet._aclPoints[1] = _rclMesh._aclPointArray[aulPolygon[i+1]];
    clFacet._aclPoints[2] = _rclMesh._aclPointArray[aulPolygon[i+2]];
    clFacet.CalcNormal();

    if (clFacet.Area() < FLOAT_EPS)
    {
      return false; // degenerated facet
    }
    raclFacets.push_back(clFacet);
  }
  return true;
}

void MeshTopoAlgorithm::InsertFacets(const std::vector<MeshGeomFacet>& raclFacets)
{
  for (std::vector<MeshGeomFacet>::const_iterator it = raclFacets.begin(); it != raclFacets.end(); ++it)
    _aclNewFacets.push_back(*it);
}

void MeshTopoAlgorithm::RefPointToFacet()
{
  if (_pclRefPt2Fac==0L)
  {
    _pclRefPt2Fac = new MeshRefPointToFacets(_rclMesh);
  }
}

void MeshTopoAlgorithm::HarmonizeNormals (void)
{
#if 0
  unsigned long               ulStartFacet, ulVisited;
  std::vector<unsigned long>  uIndices;
  MeshHarmonizer              clHarmonizer(uIndices);
  MeshFacetArray::_TIterator  clFIter;
  Base::Vector3f              clGravityPoint, clDir;

  if (_rclMesh.CountFacets() == 0)
    return;

  // Flags zuruecksetzen
  MeshAlgorithm(_rclMesh).ResetFacetFlag(MeshFacet::VISIT);

  Base::Sequencer().start("Harmonize normals...", _rclMesh.CountFacets() + 1);

  // gemeinsame Schwerpunkt aller Facets(-eckpunkte) bestimmen
  clGravityPoint.Set(0.0f, 0.0f, 0.0f);
  for (MeshPointIterator clIter(_rclMesh); clIter.EndReached() == false; ++clIter)
    clGravityPoint += *clIter; 
  clGravityPoint *= 1.0f / float(_rclMesh.CountPoints());

  ulVisited = 0;
  ulStartFacet = 0;
  while (ulStartFacet != ULONG_MAX) // solange noch freie Facets vorhanden
  {
    // Normale des Start-Facet so ausrichten das es vom Schwerpunkte wegzeigt
    clDir = _rclMesh.GetFacet(ulStartFacet).GetGravityPoint() - clGravityPoint;
    if ((_rclMesh.GetNormal(_rclMesh._aclFacetArray[ulStartFacet]) * clDir) < 0.0f)
      _rclMesh._aclFacetArray[ulStartFacet].FlipNormal();

    ulVisited += _rclMesh.VisitNeighbourFacets(clHarmonizer, ulStartFacet);
    // nach noch freien Facets (auf Inseln) suchen
    clFIter = std::find_if(_rclMesh._aclFacetArray.begin(), _rclMesh._aclFacetArray.end(), std::bind2nd(MeshIsNotFlag<MeshFacet>(), MeshFacet::VISIT));

    if (clFIter < _rclMesh._aclFacetArray.end())
    {
      ulStartFacet = clFIter - _rclMesh._aclFacetArray.begin();
      // suche naechstes Facet mit gesetzten VISIT-Flag und richte Normale danach aus
      
    }
    else
      ulStartFacet = ULONG_MAX;
  }

  for ( std::vector<unsigned long>::iterator it = uIndices.begin(); it != uIndices.end(); ++it )
    _rclMesh._aclFacetArray[*it].FlipNormal();
 
  Base::Sequencer().stop(); 
#else
  std::vector<unsigned long> uIndices = MeshEvalOrientation(_rclMesh).GetIndices();
  for ( std::vector<unsigned long>::iterator it = uIndices.begin(); it != uIndices.end(); ++it )
    _rclMesh._aclFacetArray[*it].FlipNormal();
#endif
}

void MeshTopoAlgorithm::FlipNormals (void)
{
  for (MeshFacetArray::_TIterator i = _rclMesh._aclFacetArray.begin(); i < _rclMesh._aclFacetArray.end(); i++)
    i->FlipNormal();
}

void MeshTopoAlgorithm::DirectSplitFacet(unsigned long ulFacetPos, unsigned short i, const Base::Vector3f& rP)
{
  if (ulFacetPos >= _rclMesh._aclFacetArray.size() ) return;
  MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];

  const MeshPoint& rE1 = _rclMesh._aclPointArray[rFace._aulPoints[i]];
  const MeshPoint& rE2 = _rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]];
  if ( rP == rE1 || rP == rE2 ) return;

  unsigned long uNeighbour = rFace._aulNeighbours[i];

  // open edge
  if ( uNeighbour == ULONG_MAX )
  {
    DirectSplitFacetWithOpenEdge(ulFacetPos, i, rP);
  }
  else
  {
    //MeshFacet& rFace2 = _rclMesh._aclFacetArray[uNeighbour];
    unsigned long ulSize = _rclMesh.CountFacets();
    unsigned long uPtInd = _rclMesh._aclPointArray.GetOrAddIndex(rP);

    // first new face
    MeshFacet cNewFace1;
    cNewFace1._aulPoints[0] = uPtInd;
    cNewFace1._aulPoints[1] = rFace._aulPoints[(i+2)%3];
    cNewFace1._aulPoints[2] = rFace._aulPoints[i];
    cNewFace1._aulNeighbours[0] = ulFacetPos;
    cNewFace1._aulNeighbours[1] = rFace._aulNeighbours[(i+2)%3];
    cNewFace1._aulNeighbours[2] = ulSize+1;
    _rclMesh._aclFacetArray.push_back(cNewFace1);

    // second new face
    MeshFacet cNewFace2;
    cNewFace2._aulPoints[0] = uPtInd;
    cNewFace2._aulPoints[1] = rFace._aulPoints[(i+2)%3];
    cNewFace2._aulPoints[2] = rFace._aulPoints[i];
    cNewFace2._aulNeighbours[0] = ulFacetPos;
    cNewFace2._aulNeighbours[1] = rFace._aulNeighbours[(i+2)%3];
    cNewFace2._aulNeighbours[2] = ulSize+1;
    _rclMesh._aclFacetArray.push_back(cNewFace2);
  }
}

void MeshTopoAlgorithm::DirectSplitFacetWithOpenEdge(unsigned long ulFacetPos, unsigned short i, const Base::Vector3f& rP)
{
  if (ulFacetPos >= _rclMesh._aclFacetArray.size() ) return;
  MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];

  const MeshPoint& rE1 = _rclMesh._aclPointArray[rFace._aulPoints[i]];
  const MeshPoint& rE2 = _rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]];
  if ( rP == rE1 || rP == rE2 ) return;

  unsigned long ulSize = _rclMesh.CountFacets();
  unsigned long uPtInd = _rclMesh._aclPointArray.GetOrAddIndex(rP);

  // insert one new facet
  MeshFacet cNewFace;
  cNewFace._aulPoints[0] = uPtInd;
  cNewFace._aulPoints[1] = rFace._aulPoints[(i+2)%3];
  cNewFace._aulPoints[2] = rFace._aulPoints[i];
  cNewFace._aulNeighbours[0] = ulFacetPos;
  cNewFace._aulNeighbours[1] = rFace._aulNeighbours[(i+2)%3];
  cNewFace._aulNeighbours[2] = ULONG_MAX;
  _rclMesh._aclFacetArray.push_back(cNewFace);

  // adjust the original face
  rFace._aulPoints[i] = uPtInd;
  rFace._aulNeighbours[(i+2)%3] = ulSize;

  // Adjust neighbour indices
  unsigned long uN = rFace._aulNeighbours[(i+2)%3];
  if ( uN != ULONG_MAX )
    _rclMesh._aclFacetArray[uN].ReplaceNeighbour(ulFacetPos, ulSize);
}

void MeshTopoAlgorithm::DirectRemoveDegenerated(unsigned long index)
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

void MeshTopoAlgorithm::DirectRemoveCorrupted(unsigned long index)
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

bool MeshTopoAlgorithm::Snap(unsigned long ulFacetPos, const Base::Vector3f& rP)
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
        DirectSplitFacetWithOpenEdge(ulFacetPos, i, rP);
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

void MeshTopoAlgorithm::FillupHoles(unsigned long length)
{
  // get the mesh boundaries as an array of point indices
  std::list<std::vector<unsigned long> > aBorders;
  MeshAlgorithm cAlgo(_rclMesh);
  cAlgo.GetMeshBorders(aBorders);

  // get the facets to a point
  MeshRefPointToFacets cPt2Fac(_rclMesh);

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

      bool ok = true;
      std::vector<Base::Vector3f> polygon;
      for ( std::vector<unsigned long>::iterator jt = it->begin(); jt != it->end(); ++jt )
      {
        // two (ore more) boundaries meet in one non-manifold point
        //FIXME: Split up into several independant loops and retry again.
        if ( openPointDegree[*jt] > 2) {
          ok = false;
          break;
        }

        polygon.push_back( _rclMesh._aclPointArray[*jt] );
      }

      if (!ok)
        continue;

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

//
// OBSOLETE
//


void MeshTopoAlgorithm::RotateFacet(unsigned long ulFacetPos, int iInd)
{
  unsigned long ulTmpInd;
  MeshFacet clFacet = _rclMesh._aclFacetArray[ulFacetPos];

  if (iInd == 1){
    ulTmpInd = clFacet._aulPoints[0];
    clFacet._aulPoints[0] = clFacet._aulPoints[1];
    clFacet._aulPoints[1] = clFacet._aulPoints[2];
    clFacet._aulPoints[2] = ulTmpInd;
    ulTmpInd = clFacet._aulNeighbours[0];
    clFacet._aulNeighbours[0] = clFacet._aulNeighbours[1];
    clFacet._aulNeighbours[1] = clFacet._aulNeighbours[2];
    clFacet._aulNeighbours[2] = ulTmpInd;
    _rclMesh._aclFacetArray[ulFacetPos] = clFacet;
  }
  else if (iInd == 2)
  {
    ulTmpInd = clFacet._aulPoints[0];
    clFacet._aulPoints[0] = clFacet._aulPoints[2];
    clFacet._aulPoints[2] = clFacet._aulPoints[1];
    clFacet._aulPoints[1] = ulTmpInd;
    ulTmpInd = clFacet._aulNeighbours[0];
    clFacet._aulNeighbours[0] = clFacet._aulNeighbours[2];
    clFacet._aulNeighbours[2] = clFacet._aulNeighbours[1];
    clFacet._aulNeighbours[1] = ulTmpInd;
    _rclMesh._aclFacetArray[ulFacetPos] = clFacet;
  }
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
