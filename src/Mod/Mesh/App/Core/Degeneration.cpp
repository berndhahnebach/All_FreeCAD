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
# include <map>
#endif

#include "Degeneration.h"
#include "Definitions.h"
#include "Iterator.h"
#include "Helpers.h"
#include "MeshKernel.h"
#include "Algorithm.h"
#include "Info.h"
#include "Grid.h"
#include "TopoAlgorithm.h"

#include <Base/Sequencer.h>

using namespace MeshCore;

bool MeshEvalInvalids::Evaluate()
{
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it )
  {
    if ( !it->IsValid() )
      return false;
  }

  const MeshPointArray& rPoints = _rclMesh.GetPoints();
  for ( MeshPointArray::_TConstIterator jt = rPoints.begin(); jt != rPoints.end(); ++jt )
  {
    if ( !jt->IsValid() )
      return false;
  }

  return true;
}

std::vector<unsigned long> MeshEvalInvalids::GetIndices() const
{
  std::vector<unsigned long> aInds;
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  const MeshPointArray& rPoints = _rclMesh.GetPoints();
  unsigned long ind=0;
  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it, ind++ )
  {
    if ( !it->IsValid() )
      aInds.push_back(ind);
    else if ( !rPoints[it->_aulPoints[0]].IsValid() )
      aInds.push_back(ind);
    else if ( !rPoints[it->_aulPoints[1]].IsValid() )
      aInds.push_back(ind);
    else if ( !rPoints[it->_aulPoints[2]].IsValid() )
      aInds.push_back(ind);
  }

  return aInds;
}

bool MeshFixInvalids::Fixup()
{
  _rclMesh.RemoveInvalids();
  return true;
}

// ----------------------------------------------------------------------

/*
 * When building up a mesh then usually the class MeshBuilder is used. This class uses internally a std::set<MeshPoint> 
 * which uses the '<' operator of MeshPoint to sort the points. Thus to be consistent (and avoid using the '==' operator of MeshPoint) 
 * we use the same operator when comparing the points in the function object.
 */
struct MeshPoint_EqualTo  : public std::binary_function<const MeshPoint&, const MeshPoint&, bool>
{
  bool operator()(const MeshPoint& x, const MeshPoint& y) const
  {
    if ( x < y )
      return false;
    else if ( y < x )
      return false;
    return true;
  }
};

bool MeshEvalDuplicatePoints::Evaluate()
{
  // make a copy of the point list
  MeshPointArray aPoints = _rclMesh.GetPoints();

  // sort the points ascending x,y or z coordinates
  std::sort(aPoints.begin(), aPoints.end());
  // if there are two adjacent points whose distance is less then an epsilon
  if (std::adjacent_find(aPoints.begin(), aPoints.end(), MeshPoint_EqualTo()) < aPoints.end() )
    return false;
  return true;
}

std::vector<unsigned long> MeshEvalDuplicatePoints::GetIndices() const
{
  std::vector<unsigned long> aInds;
  const MeshPointArray& rPoints = _rclMesh.GetPoints();
  unsigned long ind=0;

  // get all points
  std::map<MeshPoint, std::vector<unsigned long> > aFIndsMap;
  for ( MeshPointArray::_TConstIterator it = rPoints.begin(); it != rPoints.end(); ++it, ind++ )
    aFIndsMap[*it].push_back(ind);

  // get all duplicated points
  for (std::map<MeshPoint, std::vector<unsigned long> >::iterator it2 = aFIndsMap.begin(); it2 != aFIndsMap.end(); ++it2) {
    const std::vector<unsigned long>& idx = it2->second;
    if (idx.size() > 1) {
      for (std::vector<unsigned long>::const_iterator it3 = idx.begin(); it3 != idx.end(); ++it3)
        aInds.push_back(*it3);
    }
  }

  return aInds;
}

bool MeshFixDuplicatePoints::Fixup()
{/*
  MeshPointArray &rclPAry = _rclMesh._aclPointArray;
  rclPAry.ResetFlag(MeshPoint::MARKED);

  // merge only points in open edges =>
  // mark all other points to skip them
  if ( open )
  {
    // set all points as MARKED
    rclPAry.SetFlag(MeshPoint::MARKED);
    MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;
    for (MeshFacetArray::_TIterator it = rclFAry.begin(); it != rclFAry.end(); ++it)
    {
      for (int i=0; i<3; i++)
      {
        // open edge
        if (it->_aulNeighbours[i] == ULONG_MAX)
        {
          // and reset the open edge points
          rclPAry[it->_aulPoints[i]].ResetFlag(MeshPoint::MARKED);
          rclPAry[it->_aulPoints[(i+1)%3]].ResetFlag(MeshPoint::MARKED);
        }
      }
    }
  }

  MeshPointGrid clGrid(_rclMesh);

  // take square distance
  fMinDistance = fMinDistance * fMinDistance;
  std::vector<std::pair<unsigned long, std::vector<unsigned long> > > aulMergePts;
  
  MeshGridIterator clGridIter(clGrid);
  std::vector<unsigned long> aulPoints;
  for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
  {
    aulPoints.clear();
    clGridIter.GetElements(aulPoints);

    // compare all points in each grid
    for (std::vector<unsigned long>::iterator it1 = aulPoints.begin(); it1 != aulPoints.end(); ++it1)
    {
      MeshPoint& rclPt1 = rclPAry[*it1];
      if (rclPt1.IsFlag(MeshPoint::MARKED))
        continue; // already merged with another point

      rclPt1.SetFlag(MeshPoint::MARKED);
      std::pair<unsigned long, std::vector<unsigned long> > aMergePt;
      aMergePt.first = *it1;

      for (std::vector<unsigned long>::iterator it2 = it1; it2 != aulPoints.end(); ++it2)
      {
        if (it2 == it1 || (*it2) == (*it1))
          continue;

        MeshPoint& rclPt2 = rclPAry[*it2];
        if (rclPt2.IsFlag(MeshPoint::MARKED))
          continue; // already merged with another point

        if (Base::DistanceP2(rclPt1, rclPt2) < fMinDistance)
        {
          rclPt2.SetFlag(MeshPoint::MARKED);
          aMergePt.second.push_back(*it2);
        }
      }

      // point cluster to merge
      if (aMergePt.second.size() > 0)
        aulMergePts.push_back(aMergePt);
    }
  }

  // adjust the corresponding facets
  MeshRefPointToFacets  clPt2Facets(_rclMesh);
  unsigned long ulCt = 0;
  for (std::vector<std::pair<unsigned long, std::vector<unsigned long> > >::iterator it = aulMergePts.begin(); it != aulMergePts.end(); ++it)
  {
    unsigned long ulPos = it->first;
    std::vector<unsigned long>& aMergePt = it->second;
    for (std::vector<unsigned long>::iterator it2 = aMergePt.begin(); it2 != aMergePt.end(); ++it2)
    {
      ulCt++;
      rclPAry[*it2].SetInvalid();
      std::set<MeshFacetArray::_TConstIterator>& aulFacs = clPt2Facets[*it2];
      for (std::set<MeshFacetArray::_TConstIterator>::iterator itF = aulFacs.begin(); itF != aulFacs.end(); ++itF)
      {
        for (int i=0; i<3; i++)
        {
          if ((*itF)->_aulPoints[i] == *it2)
          {
//            (*itF)->_aulPoints[i] = ulPos;
          }
        }
      }
    }
  }

  unsigned long ulCtPts = _rclMesh.CountPoints();
  _rclMesh.RemoveInvalids();
  unsigned long ulDiff = ulCtPts - _rclMesh.CountPoints();

  // falls Dreiecke zusammenklappen => entfernen
  RemoveDegeneratedFacets();

  return ulDiff;*/
  return false;
}

// ----------------------------------------------------------------------

/*
 * The facet with the lowset index is regarded as 'less'.
 */
struct MeshFacet_Less  : public std::binary_function<const MeshFacet&, const MeshFacet&, bool>
{
  bool operator()(const MeshFacet& x, const MeshFacet& y) const
  {
    unsigned long tmp;
    unsigned long x0 = x._aulPoints[0];
    unsigned long x1 = x._aulPoints[1];
    unsigned long x2 = x._aulPoints[2];
    unsigned long y0 = y._aulPoints[0];
    unsigned long y1 = y._aulPoints[1];
    unsigned long y2 = y._aulPoints[2];

    if (x0 > x1)
    { tmp = x0; x0 = x1; x1 = tmp; }
    if (x0 > x2)
    { tmp = x0; x0 = x2; x2 = tmp; }
    if (x1 > x2)
    { tmp = x1; x1 = x2; x2 = tmp; }
    if (y0 > y1)
    { tmp = y0; y0 = y1; y1 = tmp; }
    if (y0 > y2)
    { tmp = y0; y0 = y2; y2 = tmp; }
    if (y1 > y2)
    { tmp = y1; y1 = y2; y2 = tmp; }

    if      (x0 < y0)  return true;
    else if (x0 > y0)  return false;
    else if (x1 < y1)  return true;
    else if (x1 > y1)  return false;
    else if (x2 < y2)  return true;
    else               return false;
  }
};

/*
 * Two facets are equal if all its three point indices refer to the same location in the point array of
 * the mesh kernel they belong to.
 */
struct MeshFacet_EqualTo  : public std::binary_function<const MeshFacet&, const MeshFacet&, bool>
{
  bool operator()(const MeshFacet& x, const MeshFacet& y) const
  {
    std::vector<unsigned long> xx;
    std::vector<unsigned long> yy;
    for ( int i=0; i<3; i++ )
    {
      xx.push_back( x._aulPoints[i] );
      yy.push_back( y._aulPoints[i] );
    }

    std::sort(xx.begin(), xx.end());
    std::sort(yy.begin(), yy.end());

    return ( (xx[0] == yy[0]) && (xx[1] == yy[1]) && (xx[2] == yy[2]) );
  }
};

bool MeshEvalDuplicateFacets::Evaluate()
{
  std::set<MeshFacet, MeshFacet_Less> aFaces;
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it )
    aFaces.insert( *it );

  return (aFaces.size() == rFaces.size());
}

std::vector<unsigned long> MeshEvalDuplicateFacets::GetIndices() const
{
  std::vector<unsigned long> aInds;
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  unsigned long ind=0;

  // get all facets
  std::map<MeshFacet, std::vector<unsigned long>, MeshFacet_Less > aFIndsMap;
  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it, ind++ )
    aFIndsMap[*it].push_back(ind);

  // get all duplicated facets
  for (std::map<MeshFacet, std::vector<unsigned long>, MeshFacet_Less >::iterator it2 = aFIndsMap.begin(); it2 != aFIndsMap.end(); ++it2) {
    const std::vector<unsigned long>& idx = it2->second;
    if (idx.size() > 1) {
      for (std::vector<unsigned long>::const_iterator it3 = idx.begin(); it3 != idx.end(); ++it3)
        aInds.push_back(*it3);
    }
  }

  return aInds;
}

bool MeshFixDuplicateFacets::Fixup()
{
  return false;
}

// ----------------------------------------------------------------------

bool MeshEvalDegeneratedFacets::Evaluate()
{
  MeshFacetIterator it(_rclMesh);
  for ( it.Init(); it.More(); it.Next() )
  {
    if ( it->Area() < FLOAT_EPS )
      return false;
  }

  return true;
}

unsigned long MeshEvalDegeneratedFacets::CountEdgeTooSmall (float fMinEdgeLength) const
{
  MeshFacetIterator  clFIter(_rclMesh);   
  unsigned long k = 0;

  while (clFIter.EndReached() == false)
  {
    for ( int i = 0; i < 3; i++)
    {
      if (Base::Distance(clFIter->_aclPoints[i], clFIter->_aclPoints[(i+1)%3]) < fMinEdgeLength)
        k++;
    }
    ++clFIter;
  }

  return k;
}

std::vector<unsigned long> MeshEvalDegeneratedFacets::DefacedFacets() const
{
  std::vector<unsigned long> aulDeg;
  MeshFacetIterator cIter(_rclMesh);

  Base::Vector3f u,v;
  float fAngle;
  for (cIter.Init(); cIter.More(); cIter.Next())
  {
    const MeshGeomFacet& rclF = *cIter;
    for (int i=0; i<3; i++)
    {
      u = rclF._aclPoints[(i+1)%3]-rclF._aclPoints[i];
      v = rclF._aclPoints[(i+2)%3]-rclF._aclPoints[i];
      u.Normalize();
      v.Normalize();

      fAngle = u * v;
      if (fAngle > 0.86f || fAngle < -0.5f)
      {
        aulDeg.push_back(cIter.Position());
        break;
      }
    }
  }

  return aulDeg;
}

std::vector<unsigned long> MeshEvalDegeneratedFacets::GetIndices() const
{
  std::vector<unsigned long> aInds;
  MeshFacetIterator it(_rclMesh);
  for ( it.Init(); it.More(); it.Next() )
  {
    if ( it->Area() < FLOAT_EPS )
      aInds.push_back(it.Position());
  }

  return aInds;
}

bool MeshFixDegeneratedFacets::Fixup()
{
  MeshTopoAlgorithm cTopAlg(_rclMesh);

  MeshFacetIterator it(_rclMesh);
  for ( it.Init(); it.More(); it.Next() )
  {
    if ( it->Area() <= FLOAT_EPS )
    {
      unsigned long uCt = _rclMesh.CountFacets();
      unsigned long uId = it.Position();
      cTopAlg.DirectRemoveDegenerated(uId);
      if ( uCt != _rclMesh.CountFacets() )
      {
        // due to a modification of the array the iterator became invalid
        it.Set(uId-1);
      }
    }
  }

  return true;
}

unsigned long MeshFixDegeneratedFacets::RemoveEdgeTooSmall (float fMinEdgeLength, float fMinEdgeAngle)
{
  unsigned long ulCtLastLoop, ulCtFacets = _rclMesh.CountFacets();

  MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;
  MeshPointArray &rclPAry = _rclMesh._aclPointArray;

  // wiederhole solange, bis keine Facets mehr entfernt werden
  do
  {
    MeshRefPointToFacets  clPt2Facets(_rclMesh);

    rclFAry.ResetInvalid();
    rclPAry.ResetInvalid();
    rclPAry.ResetFlag(MeshPoint::VISIT);

    std::set<std::pair<unsigned long, unsigned long> > aclPtDelList;

    MeshFacetIterator clFIter(_rclMesh), clFN0(_rclMesh), clFN1(_rclMesh), clFN2(_rclMesh);
    for (clFIter.Init(); clFIter.More(); clFIter.Next())
    {
      MeshGeomFacet clSFacet = *clFIter;
      Base::Vector3f clP0  = clSFacet._aclPoints[0];
      Base::Vector3f clP1  = clSFacet._aclPoints[1];
      Base::Vector3f clP2  = clSFacet._aclPoints[2];
      Base::Vector3f clE01 = clP1 - clP0;
      Base::Vector3f clE12 = clP2 - clP1;
      Base::Vector3f clE20 = clP2 - clP0;
      MeshFacet clFacet = clFIter.GetIndicies();
      unsigned long    ulP0 = clFacet._aulPoints[0];
      unsigned long    ulP1 = clFacet._aulPoints[1];
      unsigned long    ulP2 = clFacet._aulPoints[2];

      if ((Base::Distance(clP0, clP1) < fMinEdgeLength) || (clE20.GetAngle(-clE12) < fMinEdgeAngle))
      {  // loesche Punkt P1 auf P0
        aclPtDelList.insert(std::make_pair(std::min<unsigned long>(ulP1, ulP0), std::max<unsigned long>(ulP1, ulP0)));
      }
      else if ((Base::Distance(clP1, clP2) < fMinEdgeLength) || (clE01.GetAngle(-clE20) < fMinEdgeAngle))
      {  // loesche Punkt P2 auf P1
        aclPtDelList.insert(std::make_pair(std::min<unsigned long>(ulP2, ulP1), std::max<unsigned long>(ulP2, ulP1)));
      }
      else if ((Base::Distance(clP2, clP0) < fMinEdgeLength) || (clE12.GetAngle(-clE01) < fMinEdgeAngle))
      {  // loesche Punkt P0 auf P2
        aclPtDelList.insert(std::make_pair(std::min<unsigned long>(ulP0, ulP2), std::max<unsigned long>(ulP0, ulP2)));
      }
    }

    // Punkte entfernen, Indizes korrigieren
    for (std::set<std::pair<unsigned long, unsigned long> >::iterator pI = aclPtDelList.begin(); pI != aclPtDelList.end(); pI++)
    {
      // einer der beiden Punkte des Punktpaars bereits bearbeitet
      if ((rclPAry[pI->first].IsFlag(MeshPoint::VISIT) == true) || (rclPAry[pI->second].IsFlag(MeshPoint::VISIT) == true))
        continue;

      rclPAry[pI->first].SetFlag(MeshPoint::VISIT);
      rclPAry[pI->second].SetFlag(MeshPoint::VISIT);
      rclPAry[pI->second].SetInvalid();

      // alle Eckpunkt-Indizies der Dreiecke die den geloeschten Punkte indizieren umbiegen auf neuen (Nachbar-)Punkt
      for (std::set<MeshFacetArray::_TConstIterator>::iterator pF = clPt2Facets[pI->second].begin(); pF != clPt2Facets[pI->second].end(); pF++)
      {
        const MeshFacet &rclF = *(*pF);

        for (int i = 0; i < 3; i++)
        {
//          if (rclF._aulPoints[i] == pI->second)
//            rclF._aulPoints[i] = pI->first;
        }

        // Facets mit 2 indentischen Eckpunkten loeschen
        if ((rclF._aulPoints[0] == rclF._aulPoints[1]) ||
            (rclF._aulPoints[0] == rclF._aulPoints[2]) ||
            (rclF._aulPoints[1] == rclF._aulPoints[2]))
        {
          rclF.SetInvalid();
        }
      }
    }

    ulCtLastLoop = _rclMesh.CountFacets();

    // Datenstruktur updaten (d.h. Punkte, Facets (und nicht Kanten) loeschen)
    _rclMesh.RemoveInvalids(false, false);
  }
  while (ulCtLastLoop > _rclMesh.CountFacets());

  _rclMesh.RebuildNeighbours();

  return ulCtFacets - _rclMesh.CountFacets();
}

// ----------------------------------------------------------------------

bool MeshEvalRangeFacet::Evaluate()
{
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  unsigned long ulCtFacets = rFaces.size();

  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it ) {
    for ( int i = 0; i < 3; i++ ) {
      if ((it->_aulNeighbours[i] >= ulCtFacets) && (it->_aulNeighbours[i] < ULONG_MAX)) {
        return false;
      }
    }
  }

  return true;
}

std::vector<unsigned long> MeshEvalRangeFacet::GetIndices() const
{
  std::vector<unsigned long> aInds;
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  unsigned long ulCtFacets = rFaces.size();

  unsigned long ind=0;
  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it, ind++ )
  {
    for ( int i = 0; i < 3; i++ ) {
      if ((it->_aulNeighbours[i] >= ulCtFacets) && (it->_aulNeighbours[i] < ULONG_MAX)) {
        aInds.push_back(ind);
        break;
      }
    }
  }

  return aInds;
}

bool MeshFixRangeFacet::Fixup()
{
  return false;
}

// ----------------------------------------------------------------------

bool MeshEvalRangePoint::Evaluate()
{
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  unsigned long ulCtPoints = _rclMesh.CountPoints();

  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it ) {
    if (std::find_if(it->_aulPoints, it->_aulPoints + 3, std::bind2nd(std::greater_equal<unsigned long>(), ulCtPoints)) < it->_aulPoints + 3)
      return false;
  }

  return true;
}

std::vector<unsigned long> MeshEvalRangePoint::GetIndices() const
{
  std::vector<unsigned long> aInds;
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  unsigned long ulCtPoints = _rclMesh.CountPoints();

  unsigned long ind=0;
  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it, ind++ )
  {
    if (std::find_if(it->_aulPoints, it->_aulPoints + 3, std::bind2nd(std::greater_equal<unsigned long>(), ulCtPoints)) < it->_aulPoints + 3)
      aInds.push_back(ind);
  }

  return aInds;
}

bool MeshFixRangePoint::Fixup()
{
  return false;
}

// ----------------------------------------------------------------------

bool MeshEvalCorruptedFacets::Evaluate()
{
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();

  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it ) {
    // dupicated point indices
    if ((it->_aulPoints[0] == it->_aulPoints[1]) || (it->_aulPoints[0] == it->_aulPoints[2]) || (it->_aulPoints[1] == it->_aulPoints[2]))
      return false;
  }

  return true;
}

std::vector<unsigned long> MeshEvalCorruptedFacets::GetIndices() const
{
  std::vector<unsigned long> aInds;
  const MeshFacetArray& rFaces = _rclMesh.GetFacets();
  unsigned long ind=0;

  for ( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it, ind++ ) {
    if ((it->_aulPoints[0] == it->_aulPoints[1]) || (it->_aulPoints[0] == it->_aulPoints[2]) || (it->_aulPoints[1] == it->_aulPoints[2]))
      aInds.push_back(ind);
  }

  return aInds;
}

bool MeshFixCorruptedFacets::Fixup()
{
  MeshTopoAlgorithm cTopAlg(_rclMesh);

  MeshFacetIterator it(_rclMesh);
  for ( it.Init(); it.More(); it.Next() )
  {
    if ( it->Area() <= FLOAT_EPS )
    {
      unsigned long uId = it.Position();
      cTopAlg.DirectRemoveCorrupted(uId);
      // due to a modification of the array the iterator became invalid
      it.Set(uId-1);
    }
  }

  return true;
}

