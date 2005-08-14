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
#endif

#include "TopoAlgorithm.h"
#include "Iterator.h"
#include "MeshKernel.h"
#include "Algorithm.h"


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

void MeshTopoAlgorithm::InsertNode(unsigned long ulFacetPos, const Vector3D&  rclPoint)
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

#ifdef Use_EdgeList
void MeshTopoAlgorithm::OptimizeTopology()
{
  std::vector<std::set<unsigned long> > clNPoints;
  clNPoints.resize(_rclMesh._aclPointArray.size());
  unsigned long ulPt0, ulPt1;

  // get all current edges
  //
  for (MeshFacetArray::_TIterator pFIter = _rclMesh._aclFacetArray.begin(); pFIter != _rclMesh._aclFacetArray.end(); pFIter++)
  {
    unsigned long ulP0 = pFIter->_aulPoints[0];
    unsigned long ulP1 = pFIter->_aulPoints[1];
    unsigned long ulP2 = pFIter->_aulPoints[2];

    clNPoints[ulP0].insert(ulP1);
    clNPoints[ulP0].insert(ulP2);
    clNPoints[ulP1].insert(ulP0);
    clNPoints[ulP1].insert(ulP2);
    clNPoints[ulP2].insert(ulP0);
    clNPoints[ulP2].insert(ulP1);
  }

  MeshFacetIterator clFIter1(_rclMesh);
  MeshFacetIterator clFIter2(_rclMesh);
  // first reset VISITed flag
  MeshAlgorithm(_rclMesh).ResetFacetFlag(MeshFacet::VISIT);

  Vector3D p0, p1, q0, q1;
  MeshFacet clF, clN;
  for (MeshEdgeArray::_TIterator it = _rclMesh._aclEdgeArray.begin(); it != _rclMesh._aclEdgeArray.end(); ++it)
  {
    Base::Sequencer().next();

    clF = _rclMesh._aclFacetArray[it->Index()];
    if (clF._aulNeighbours[it->Side()] == ULONG_MAX)
      continue;

    clN = _rclMesh._aclFacetArray[clF._aulNeighbours[it->Side()]];

    if (clF.IsFlag(MeshFacet::VISIT) || clN.IsFlag(MeshFacet::VISIT))
      continue;

    clFIter1.Set(it->Index());
    const MeshGeomFacet& rclF = *clFIter1;
    clFIter2.Set(clF._aulNeighbours[it->Side()]);
    const MeshGeomFacet& rclN = *clFIter2;

    if (rclF.GetNormal() * rclN.GetNormal() < 0.95f)
      continue;

    // makes it sense to swap?
    //
    // Current length of the edge
    p0 = _rclMesh._aclPointArray[clF._aulPoints[it->Side()]];
    p1 = _rclMesh._aclPointArray[clF._aulPoints[(it->Side()+1)%3]];

    ulPt0 = clF._aulPoints[(it->Side()+2)%3];
    q0 = _rclMesh._aclPointArray[ulPt0];
    for (int i=0; i<3; ++i)
    {
      if (clN._aulNeighbours[i] == it->Index())
      {
        ulPt1 = clN._aulPoints[(i+2)%3];
        q1 = _rclMesh._aclPointArray[ulPt1];
        break;
      }
    }

    // get all neighbour points (to check the edges)
    if (clNPoints[ulPt0].find(ulPt1) != clNPoints[ulPt0].end())
      continue;
    if (clNPoints[ulPt1].find(ulPt0) != clNPoints[ulPt1].end())
      continue;

    if ((q1-q0).Length() < (p1-p0).Length())
    {
      // is polygon convex ?
      //
      float a0 = (q0-p0).GetAngle(p1-p0);
      float a1 = (p1-p0).GetAngle(q1-p0);

      float b0 = (q0-p1).GetAngle(p0-p1);
      float b1 = (p0-p1).GetAngle(q1-p1);

      if ((a0+a1<3.0f) && (b0+b1<3.0f))
      {
        if (((q1-p0)%(q0-p0))*rclF.GetNormal() < FLOAT_EPS)
        {
          continue;
        }
        if (((q1-p0)%(q0-p0))*rclN.GetNormal() < FLOAT_EPS)
        {
          continue;
        }

        SwapEdge(it->Index(), it->Side());
      
        clF.SetFlag(MeshFacet::VISIT);
        clN.SetFlag(MeshFacet::VISIT);

        // append the new edge to the point neighbourhood
        clNPoints[ulPt0].insert(ulPt1);
        clNPoints[ulPt1].insert(ulPt0);
      }
    }
  }
}
#endif

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
bool MeshTopoAlgorithm::MultiSplitOpenEdge(unsigned long ulFacetPos, int iSide, const Vector3D& rclPoint, float fTolerance)
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

void MeshTopoAlgorithm::SplitEdge(unsigned long ulFacetPos, int iSide, const Vector3D& rclPoint)
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

void MeshTopoAlgorithm::SplitFacet(unsigned long ulFacetPos, const Vector3D& rP1, const Vector3D& rP2)
{
  // search for the matching edges
  int iEdgeNo1=-1, iEdgeNo2=-1;
  const MeshFacet& rFace = _rclMesh._aclFacetArray[ulFacetPos];
  for ( int i=0; i<3; i++ )
  {
    Vector3D cBase(_rclMesh._aclPointArray[rFace._aulPoints[i]]);
    Vector3D cEnd (_rclMesh._aclPointArray[rFace._aulPoints[(i+1)%3]]);
    Vector3D cDir = cEnd - cBase;

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
  Vector3D cP1 = rP1;
  Vector3D cP2 = rP2;

  if ( (iEdgeNo2+1)%3 == iEdgeNo2 )
  {
    int tmp = iEdgeNo1;
    iEdgeNo1 = iEdgeNo2;
    iEdgeNo2 = tmp;
    cP1 = rP2;
    cP2 = rP1;
  }

  MeshFacet& rclF = _rclMesh._aclFacetArray[ulFacetPos];
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
  std::set<MeshFacetArray::_TIterator> aclNeighbours0 = (*_pclRefPt2Fac)[ulEdgeP0];
  std::set<MeshFacetArray::_TIterator> aclNeighbours1 = (*_pclRefPt2Fac)[ulEdgeP1];

  if (aclNeighbours0.size() < 3 || aclNeighbours1.size() < 3)
    return false; // this must be a border point => must not remove

  Vector3D clNew = 0.5f*(_rclMesh._aclPointArray[ulEdgeP0]+_rclMesh._aclPointArray[ulEdgeP1]);

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
  for (std::set<MeshFacetArray::_TIterator>::iterator it0 = aclNeighbours0.begin(); it0 != aclNeighbours0.end(); ++it0)
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
  for (std::set<MeshFacetArray::_TIterator>::iterator it1 = aclNeighbours1.begin(); it1 != aclNeighbours1.end(); ++it1)
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
  std::set<MeshFacetArray::_TIterator> aclNeighbours = (*_pclRefPt2Fac)[ulPointPos];
  if (aclNeighbours.size() == 3)
  {
    std::vector<unsigned long> aulPoly;
    if ( !GetPolygonOfNeighbours(ulPointPos, aulPoly) )
      return false; // Oops, this should never occur

    if (TriangulatePolygon(aulPoly, _aclNewFacets))
    {
      for (std::set<MeshFacetArray::_TIterator>::iterator it = aclNeighbours.begin(); it != aclNeighbours.end(); ++it)
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
  std::set<MeshFacetArray::_TIterator> aclNeighbours = (*_pclRefPt2Fac)[ulPointPos];
  if (aclNeighbours.size() < 3)
    return false; // this must be a border point => must not remove

  std::vector<unsigned long> clPoly;
  if ( !GetPolygonOfNeighbours(ulPointPos, clPoly) )
    return false;

  if ( !IsConvexPolygon(clPoly) )
    return false;

  if (TriangulatePolygon(clPoly, _aclNewFacets))
  {
    for (std::set<MeshFacetArray::_TIterator>::iterator it = aclNeighbours.begin(); it != aclNeighbours.end(); ++it)
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

  Vector3D a, b, c;
  Vector3D clNormal;

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
  std::set<MeshFacetArray::_TIterator> aclNeighbours = (*_pclRefPt2Fac)[ulPointPos];

  MeshFacetArray::_TIterator pFirst = *aclNeighbours.begin();
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
    for (std::set<MeshFacetArray::_TIterator>::iterator it = aclNeighbours.begin(); !found&&it!= aclNeighbours.end(); ++it)
    {
      MeshFacetArray::_TIterator pElement = *it;
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
#if 0
  RSlist<TVertexEval> alVertices;

  std::vector<unsigned long> aulPolygon = raulPoly;
  unsigned long ulCnt = aulPolygon.size();

  MeshGeomFacet clFacet;
  Vector3D u,v;
  for (unsigned long i=0; i<ulCnt; ++i)
  {
    TVertexEval vertex;
    unsigned long ulInd1 = aulPolygon[i];
    unsigned long ulInd2 = aulPolygon[(i+1)%ulCnt];
    unsigned long ulInd3 = aulPolygon[(i-1+ulCnt)%ulCnt];

    vertex.ulVertex = ulInd1;
    vertex.ulNext = ulInd2;
    vertex.ulPrev = ulInd3;

    clFacet._aclPoints[0] = _rclMesh._aclPointArray[ulInd1];
    clFacet._aclPoints[1] = _rclMesh._aclPointArray[ulInd2];
    clFacet._aclPoints[2] = _rclMesh._aclPointArray[ulInd3];
    clFacet.CalcNormal();

    u = clFacet._aclPoints[1] - clFacet._aclPoints[0];
    v = clFacet._aclPoints[2] - clFacet._aclPoints[0];
    float fAngle = u.GetAngle(v);
    if (rclNormal * clFacet.GetNormal() < 0)
      fAngle = 2*F_PI-fAngle;
    vertex.fEval = fAngle;

    alVertices.push_back(vertex);
  }

  alVertices.sort();

  TVertexEval vertex;
  while (alVertices.size() >= 3)
  {
    vertex = alVertices.front();
    alVertices.pop_front();

    clFacet._aclPoints[0] = _rclMesh._aclPointArray[vertex.ulVertex];
    clFacet._aclPoints[1] = _rclMesh._aclPointArray[vertex.ulNext];
    clFacet._aclPoints[2] = _rclMesh._aclPointArray[vertex.ulPrev];
    clFacet.CalcNormal();
    raclFacets.push_back(clFacet);

    // recalculate the angles of vertices prev and next
    for (RSlist<TVertexEval>::iterator it = alVertices.begin(); it != alVertices.end(); ++it)
    {
      bool found = false;
      if (it->ulVertex == vertex.ulPrev)
      {
        it->ulNext = vertex.ulNext;
        found = true;
      }
      else if (it->ulVertex == vertex.ulNext)
      {
        it->ulPrev = vertex.ulPrev;
        found = true;
      }

      if (found)
      {
        clFacet._aclPoints[0] = _rclMesh._aclPointArray[it->ulVertex];
        clFacet._aclPoints[1] = _rclMesh._aclPointArray[it->ulNext];
        clFacet._aclPoints[2] = _rclMesh._aclPointArray[it->ulPrev];
        clFacet.CalcNormal();

        u = clFacet._aclPoints[1] - clFacet._aclPoints[0];
        v = clFacet._aclPoints[2] - clFacet._aclPoints[0];
        float fAngle = u.GetAngle(v);
        if (rclNormal * clFacet.GetNormal() < 0)
          fAngle = 2*F_PI-fAngle;
        it->fEval = fAngle;
      }
    }

    alVertices.sort();
  }

#else

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
#endif

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
  unsigned long                       ulStartFacet, ulVisited;
  MeshHarmonizer              clHarmonizer;
  MeshFacetArray::_TIterator  clFIter;
  Vector3D                    clGravityPoint, clDir;  

  if (_rclMesh.CountFacets() == 0)
    return;

  // Flags zuruecksetzen
  MeshAlgorithm(_rclMesh).ResetFacetFlag(MeshFacet::VISIT);

  Base::Sequencer().start("harmonize normals...", _rclMesh.CountFacets() + 1);

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
    clFIter = std::find_if(_rclMesh._aclFacetArray.begin(),
                        _rclMesh._aclFacetArray.end(),
                        std::bind2nd(MeshIsNotFlag<MeshFacet>(), MeshFacet::VISIT));

    if (clFIter < _rclMesh._aclFacetArray.end())
    {
      ulStartFacet = clFIter - _rclMesh._aclFacetArray.begin();
      // suche naechstes Facet mit gesetzten VISIT-Flag und richte Normale danach aus
      
    }
    else
      ulStartFacet = ULONG_MAX;
  }
 
#ifdef Use_EdgeList
  _rclMesh.RebuildEdgeArray();
#endif
  Base::Sequencer().stop(); 
}

void MeshTopoAlgorithm::FlipNormals (void)
{
  for (MeshFacetArray::_TIterator i = _rclMesh._aclFacetArray.begin(); i < _rclMesh._aclFacetArray.end(); i++)
    i->FlipNormal();
#ifdef Use_EdgeList
  _rclMesh.RebuildEdgeArray();
#endif
}

//
// OBSOLETE
//

#ifdef Use_EdgeList
void MeshTopoAlgorithm::DeleteEdges(unsigned long ulFacetPos)
{
  unsigned long i, j, k, ulEdgeInd;

  if (ulFacetPos >= _rclMesh.CountFacets())
    return;
  
  for (i=0;i<3;i++)
  {
    ulEdgeInd = _rclMesh._aclEdgeArray.Find(ulFacetPos, i);
    if(ulEdgeInd != ULONG_MAX)
      _rclMesh._aclEdgeArray.erase(_rclMesh._aclEdgeArray.begin() + ulEdgeInd);

    k = _rclMesh._aclFacetArray[ulFacetPos]._aulNeighbours[i];
    if (k != ULONG_MAX)
    {
      for (j=0;j<3;j++)
      {
        if (_rclMesh._aclFacetArray[k]._aulNeighbours[j] == ulFacetPos)
        {
          ulEdgeInd = _rclMesh._aclEdgeArray.Find(k, j);
          
          if (ulEdgeInd != ULONG_MAX)
           _rclMesh._aclEdgeArray.erase(_rclMesh._aclEdgeArray.begin() + ulEdgeInd);
        }
      }
    }
  }

}

void MeshTopoAlgorithm::InsertEdges(unsigned long ulFacetPos)
{
  unsigned long i, j, k, ulEdgeInd;

  if (ulFacetPos >= _rclMesh.CountFacets())
    return;

  for (i=0;i<3;i++)
  {
    ulEdgeInd = _rclMesh._aclEdgeArray.Find(ulFacetPos, i);
    if (ulEdgeInd == ULONG_MAX){
      k = _rclMesh._aclFacetArray[ulFacetPos]._aulNeighbours[i];
      if (k!=ULONG_MAX){
        for (j=0;j<3;j++){
          if (_rclMesh._aclFacetArray[k]._aulNeighbours[j] == ulFacetPos)
          {
            ulEdgeInd = _rclMesh._aclEdgeArray.Find(k,j);
            if (ulEdgeInd == ULONG_MAX)
              _rclMesh._aclEdgeArray.Add(ulFacetPos,i);
          }
        }
      }
      else _rclMesh._aclEdgeArray.Add(ulFacetPos, i);
    }
  }
}
#endif


void MeshTopoAlgorithm::RotateFacet(unsigned long ulFacetPos, int iInd)
{
  unsigned long ulTmpInd;
  MeshFacet clFacet = _rclMesh._aclFacetArray[ulFacetPos];

  if (iInd == 1){
#ifdef Use_EdgeList
    DeleteEdges(ulFacetPos);
#endif
    ulTmpInd = clFacet._aulPoints[0];
    clFacet._aulPoints[0] = clFacet._aulPoints[1];
    clFacet._aulPoints[1] = clFacet._aulPoints[2];
    clFacet._aulPoints[2] = ulTmpInd;
    ulTmpInd = clFacet._aulNeighbours[0];
    clFacet._aulNeighbours[0] = clFacet._aulNeighbours[1];
    clFacet._aulNeighbours[1] = clFacet._aulNeighbours[2];
    clFacet._aulNeighbours[2] = ulTmpInd;
    _rclMesh._aclFacetArray[ulFacetPos] = clFacet;
#ifdef Use_EdgeList
    InsertEdges(ulFacetPos);
#endif
  }
  else if (iInd == 2)
  {
#ifdef Use_EdgeList
    DeleteEdges(ulFacetPos);
#endif
    ulTmpInd = clFacet._aulPoints[0];
    clFacet._aulPoints[0] = clFacet._aulPoints[2];
    clFacet._aulPoints[2] = clFacet._aulPoints[1];
    clFacet._aulPoints[1] = ulTmpInd;
    ulTmpInd = clFacet._aulNeighbours[0];
    clFacet._aulNeighbours[0] = clFacet._aulNeighbours[2];
    clFacet._aulNeighbours[2] = clFacet._aulNeighbours[1];
    clFacet._aulNeighbours[1] = ulTmpInd;
    _rclMesh._aclFacetArray[ulFacetPos] = clFacet;
#ifdef Use_EdgeList
    InsertEdges(ulFacetPos);
#endif
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

MeshComponents::MeshComponents( MeshKernel& rclMesh )
: _rclMesh(rclMesh)
{
}

MeshComponents::~MeshComponents()
{
}

void MeshComponents::SearchForComponents(TMode tMode, std::vector<std::vector<unsigned long> >& aclT) const
{
  // reset flag
  MeshAlgorithm(_rclMesh).ResetFacetFlag(MeshFacet::VISIT);

  // all facets
  std::vector<unsigned long> aulAllFacets(_rclMesh.CountFacets());
  unsigned long k = 0;
  for (std::vector<unsigned long>::iterator pI = aulAllFacets.begin(); pI != aulAllFacets.end(); pI++)
    *pI = k++;

  std::vector<std::vector<unsigned long> > aclConnectComp;
  while (aulAllFacets.size() > 0)
  {
    std::vector<unsigned long> aclComponent;
    MeshTopFacetVisitor clFVisitor( aclComponent );

    // collect all facets of a component
    if (tMode == OverEdge)
      _rclMesh.VisitNeighbourFacets(clFVisitor, aulAllFacets.front());
    else if (tMode == OverPoint)
      _rclMesh.VisitNeighbourFacetsOverCorners(clFVisitor, aulAllFacets.front());

    // get also start facet
    aclComponent.push_back(aulAllFacets.front());
    aclConnectComp.push_back(aclComponent);

    // search for all NOT YET visited facets
    std::vector<unsigned long> aclNotVisited;
    std::sort(aulAllFacets.begin(), aulAllFacets.end());
    std::sort(aclComponent.begin(), aclComponent.end());
    std::back_insert_iterator<std::vector<unsigned long> >  pBInd(aclNotVisited);
    std::set_difference(aulAllFacets.begin(), aulAllFacets.end(), aclComponent.begin(), aclComponent.end(), pBInd);
  
    aulAllFacets = aclNotVisited;
	}

  // sort components by size (descending order)
  std::sort(aclConnectComp.begin(), aclConnectComp.end(), CNofFacetsCompare());  
  aclT = aclConnectComp;
}
