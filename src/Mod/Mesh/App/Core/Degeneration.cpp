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

#include <Base/Sequencer.h>

using namespace MeshCore;


struct TMeshFacetInds
{
  unsigned long p0, p1, p2;
  unsigned long ulFIndex;

  TMeshFacetInds(unsigned long p0, unsigned long p1, unsigned long p2)
  {
    unsigned long tmp;
    if (p0 > p1)
    {
      tmp = p0;
      p0 = p1;
      p1 = tmp;
    }
    if (p0 > p2)
    {
      tmp = p0;
      p0 = p2;
      p2 = tmp;
    }
    if (p1 > p2)
    {
      tmp = p1;
      p1 = p2;
      p2 = tmp;
    }

    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
  }

  bool operator < (const TMeshFacetInds& m) const
  {
    if (p0 < m.p0)
      return true;
    else if (p0 > m.p0)
      return false;
    else if (p1 < m.p1)
      return true;
    else if (p1 > m.p1)
      return false;
    else if (p2 < m.p2)
      return true;
    else 
      return false;
  }

  bool operator == (const TMeshFacetInds& m) const
  {
    if (p0 == m.p0 && p1 == m.p1 && p2 == m.p2)
      return true;
    return false;
  }
};

unsigned long MeshDegenerations::MergePoints (bool open, float fMinDistance)
{
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
      std::set<MeshFacetArray::_TIterator>& aulFacs = clPt2Facets[*it2];
      for (std::set<MeshFacetArray::_TIterator>::iterator itF = aulFacs.begin(); itF != aulFacs.end(); ++itF)
      {
        for (int i=0; i<3; i++)
        {
          if ((*itF)->_aulPoints[i] == *it2)
          {
            (*itF)->_aulPoints[i] = ulPos;
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

  return ulDiff;
}

unsigned long MeshDegenerations::RemoveDegeneratedFacets ()
{
  MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;
  unsigned long ulPos = 0;
  for (MeshFacetArray::_TIterator pIt = rclFAry.begin(); pIt != rclFAry.end(); ++pIt, ulPos++)
  {
    // all points are equal
    if (pIt->_aulPoints[0] == pIt->_aulPoints[1] &&
        pIt->_aulPoints[0] == pIt->_aulPoints[2])
    {
      // just set the facet invalid
      // (if there are neighbours they will be also removed)
      pIt->SetInvalid();
    }
    else
    {
      for (int i=0; i<3; i++)
      {
        // two same points
        if ( pIt->_aulPoints[i] == pIt->_aulPoints[(i+1)%3] )
        {
          pIt->SetInvalid();

          // adjust neighbourhood
          //
          // 1st neighbour
          if (pIt->_aulNeighbours[(i+1)%3] != ULONG_MAX)
          {
            MeshFacet& rclF = rclFAry[pIt->_aulNeighbours[(i+1)%3]];
            unsigned short usSide = rclF.Side(ulPos);
            if (usSide != USHRT_MAX)
            {
              rclF._aulNeighbours[usSide] = pIt->_aulNeighbours[(i+2)%3];
            }
          }

          // 2nd neighbour
          if (pIt->_aulNeighbours[(i+2)%3] != ULONG_MAX)
          {
            MeshFacet& rclF = rclFAry[pIt->_aulNeighbours[(i+2)%3]];
            unsigned short usSide = rclF.Side(ulPos);
            if (usSide != USHRT_MAX)
            {
              rclF._aulNeighbours[usSide] = pIt->_aulNeighbours[(i+1)%3];
            }
          }
          break;
        }
      }
    }
  }

#if 1
  unsigned long ulCtFts = _rclMesh.CountFacets();
  _rclMesh.RemoveInvalids();
  unsigned long ulDiff = ulCtFts - _rclMesh.CountFacets();

  std::vector<MeshGeomFacet> aclFacets;
  MeshFacetIterator cFIter(_rclMesh);
  for (cFIter.Init(); cFIter.More(); cFIter.Next())
  {
    aclFacets.push_back(*cFIter);
  }
  Base::Sequencer().setLocked(true);
  _rclMesh = aclFacets;
  Base::Sequencer().setLocked(false);
#else
  unsigned long ulCtFts = _rclMesh.CountFacets();
  _rclMesh.RemoveInvalids(true, true);
  unsigned long ulDiff = ulCtFts - _rclMesh.CountFacets();
#endif

  return ulDiff;
}

unsigned long MeshDegenerations::CountEdgeTooSmall (float fMinEdgeLength) const
{
  MeshFacetIterator  clFIter(_rclMesh);   
  unsigned long              i, k;

  k = 0;
  while (clFIter.EndReached() == false)
  {
    for (i = 0; i < 3; i++)
    {
      if (Base::Distance(clFIter->_aclPoints[i], clFIter->_aclPoints[(i+1)%3]) < fMinEdgeLength)
        k++;
    }
    ++clFIter;
  }

  return k;
}

std::vector<unsigned long> MeshDegenerations::DefacedFacets() const
{
  std::vector<unsigned long> aulDeg;
  MeshFacetIterator cIter(_rclMesh);

  Vector3D u,v;
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

unsigned long MeshDegenerations::RemoveEdgeTooSmall (float fMinEdgeLength, float fMinEdgeAngle)
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
      Vector3D clP0  = clSFacet._aclPoints[0];
      Vector3D clP1  = clSFacet._aclPoints[1];
      Vector3D clP2  = clSFacet._aclPoints[2];
      Vector3D clE01 = clP1 - clP0;
      Vector3D clE12 = clP2 - clP1;
      Vector3D clE20 = clP2 - clP0;
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
      for (std::set<MeshFacetArray::_TIterator>::iterator pF = clPt2Facets[pI->second].begin(); pF != clPt2Facets[pI->second].end(); pF++)
      {
        MeshFacet &rclF = *(*pF);

        for (int i = 0; i < 3; i++)
        {
          if (rclF._aulPoints[i] == pI->second)
            rclF._aulPoints[i] = pI->first;
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

  _rclMesh.RebuildEdgeArray();
  _rclMesh.RebuildNeighbours();

  return ulCtFacets - _rclMesh.CountFacets();
}

bool MeshDegenerations::Evaluate ()
{
  // first reset all error modes
  ResetErrorModes();

  MeshFacetArray::_TIterator  pFIter;
  MeshEdgeArray::_TIterator   pEIter;
  MeshPointArray::_TIterator  pPIter;
  unsigned long                       ulCtPoints, ulCtFacets, i, j, k, l;

  ulCtPoints = _rclMesh.CountPoints();
  ulCtFacets = _rclMesh.CountFacets();

  // Ueberpruefung auf ungueltige Facets
  for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++)
  {
    if (pFIter->IsValid() == false)
    {
      // 1 oder mehrere ungueltige Facets
      AddErrorMode( InvalidFacet );
    }

    if (std::find_if(pFIter->_aulPoints, pFIter->_aulPoints + 3,
      std::bind2nd(std::greater_equal<unsigned long>(), ulCtPoints)) < pFIter->_aulPoints + 3)
    {
      // Punkt-Indizies > max. Punkte
      AddErrorMode( OutOfRangePoint );
    }

    // doppelte Punktbelegung
    if ((pFIter->_aulPoints[0] == pFIter->_aulPoints[1]) ||
        (pFIter->_aulPoints[0] == pFIter->_aulPoints[2]) ||
        (pFIter->_aulPoints[1] == pFIter->_aulPoints[2]))
    {
      AddErrorMode( CorruptedFacets );
    }

    for (i = 0; i < 3; i++)
    {
      if ((pFIter->_aulNeighbours[i] >= ulCtFacets) && (pFIter->_aulNeighbours[i] < ULONG_MAX))
      {
        // Nachbar-Indizies ungueltig
        AddErrorMode( OutOfRangeFacet );
      }
    }
  }

  // mind. ein zu einer Kante (oder Punkt) degeneriertes Dreieck (geometrisch, nicht topologisch)
  MeshFacetIterator cFIter(_rclMesh);
  for (cFIter.Init(); cFIter.More(); cFIter.Next())
  {
    if ((cFIter->_aclPoints[0] == cFIter->_aclPoints[1]) ||
        (cFIter->_aclPoints[0] == cFIter->_aclPoints[2]) ||
        (cFIter->_aclPoints[1] == cFIter->_aclPoints[2]))
    {
      AddErrorMode( DegeneratedFacets );
      break;
    }
  }

  // Ueberpruefung auf ungueltige Punkte
  for (pPIter = _rclMesh._aclPointArray.begin(); pPIter < _rclMesh._aclPointArray.end(); pPIter++)
  {
    if (pPIter->IsValid() == false)
    {
      // 1 oder mehrere ungueltige Punkte
      AddErrorMode( InvalidPoint );
      break;
    }
  }

  // Kanten-Array muss sortiert sein
  // @todo gcc macht Probleme
  /**if (std::is_sorted(_rclMesh._aclEdgeArray.begin(), _rclMesh._aclEdgeArray.end()) == false)
  {
    // Kanten-Array nicht sortiert
    AddErrorMode( Undefined );
  }*/

  if (std::adjacent_find(_rclMesh._aclEdgeArray.begin(), _rclMesh._aclEdgeArray.end()) < _rclMesh._aclEdgeArray.end())
  {
    // zwei gleiche Kanten (gleiches Dreieck & gleiche Seite) s.u.
    AddErrorMode( DuplicatedEdges );
  }

  // Ueberpruefung auf doppelte Dreiecke
  std::set<TMeshFacetInds> FIndsSet;
  for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++)
  {
    TMeshFacetInds FInds(pFIter->_aulPoints[0], pFIter->_aulPoints[1], pFIter->_aulPoints[2]);
    FIndsSet.insert(FInds);
  }
  if (FIndsSet.size() != _rclMesh._aclFacetArray.size())
  {
    // zwei gleiche Dreiecke
    AddErrorMode( DuplicatedFacets );
  }

  // Ueberpruefung auf doppelte Kanten und ungueltige Indizies
  for (pEIter = _rclMesh._aclEdgeArray.begin(); pEIter < _rclMesh._aclEdgeArray.end(); pEIter++)
  {
    i = pEIter->Index();
    j = pEIter->Side();
    if ((i >= ulCtFacets) || (j > 2))
    {
      // ungueltige Indizies
      AddErrorMode( OutOfRangeEdge );
    }

    k = _rclMesh._aclFacetArray[i]._aulNeighbours[j];
    l = _rclMesh._aclFacetArray[k].Side(i);
    if (_rclMesh._aclEdgeArray.Find(k, l) != ULONG_MAX)
    {
      // doppelte Kante (beide Dreiecke besitzen jeweils die gleiche Kante) s.o
      AddErrorMode( DuplicatedEdges );
    }

    for (pEIter = _rclMesh._aclEdgeArray.begin(); pEIter < _rclMesh._aclEdgeArray.end(); pEIter++)
    {
      if (pEIter->IsValid() == false)
      {
        AddErrorMode( InvalidEdge );
      }
    }
  }

  // keine doppelte Punkte
  {
    std::vector<MeshHelpPoint>             aclPoints(_rclMesh.CountPoints());
    std::vector<MeshHelpPoint>::iterator   pI = aclPoints.begin();
    for (pPIter = _rclMesh._aclPointArray.begin(); pPIter < _rclMesh._aclPointArray.end(); pPIter++, pI++)
      pI->Set(0, 0, *pPIter);
    std::sort(aclPoints.begin(), aclPoints.end());
    if (std::adjacent_find(aclPoints.begin(), aclPoints.end()) < aclPoints.end())
    {
      // doppelter Punkt
      AddErrorMode( DuplicatedPoints );
    }
  }

  return !HasDegenerations();
}

bool MeshDegenerations::Fixup()
{
  return false;
}

std::vector<unsigned long> MeshDegenerations::GetIndices(MeshDegenerations::TErrorTable error)
{
  unsigned long ulCtFacets = _rclMesh.CountFacets();
  unsigned long ulCtPoints = _rclMesh.CountPoints();

  unsigned long i=0;
  std::vector<unsigned long> ulInds;
  MeshFacetArray::_TIterator  pFIter;
  MeshEdgeArray::_TIterator   pEIter;
  MeshPointArray::_TIterator  pPIter;

  switch (error)
  {
    case InvalidFacet:
    {
      for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++, i++)
      {
        if (pFIter->IsValid() == false)
        {
          ulInds.push_back(i);
        }
      } break;
    }
    case InvalidEdge:
    {
      for (pEIter = _rclMesh._aclEdgeArray.begin(); pEIter < _rclMesh._aclEdgeArray.end(); pEIter++, i++)
      {
        if (pEIter->IsValid() == false)
        {
          ulInds.push_back(pEIter->Index());
        }
      } break;
    }
    case InvalidPoint:
    {
      std::map<unsigned long, std::vector<unsigned long> > aulPoint2Facet;
      for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++, i++)
      {
        for (int i=0; i<3; i++)
        {
          aulPoint2Facet[pFIter->_aulPoints[i]].push_back(i);
        }
      }

      i = 0;
      MeshPointArray::_TIterator  pPIter;
      std::vector<unsigned long> aulTmp;
      for (pPIter = _rclMesh._aclPointArray.begin(); pPIter < _rclMesh._aclPointArray.end(); pPIter++, i++)
      {
        if (pPIter->IsValid() == false)
        {
          aulTmp.push_back(i);
        }
      }

      for (std::vector<unsigned long>::iterator it = aulTmp.begin(); it != aulTmp.end(); ++it)
      {
        for (std::vector<unsigned long>::iterator it2 = aulPoint2Facet[*it].begin(); it2 != aulPoint2Facet[*it].end(); it2++)
        {
          ulInds.push_back(*it2);
        }
      }

      std::sort(ulInds.begin(), ulInds.end());
      ulInds.erase(std::unique(ulInds.begin(), ulInds.end()), ulInds.end());
      break;
    }
    case DuplicatedEdges:
    {
      unsigned long i,j,k,l;
      for (pEIter = _rclMesh._aclEdgeArray.begin(); pEIter < _rclMesh._aclEdgeArray.end(); pEIter++)
      {
        i = pEIter->Index();
        j = pEIter->Side();
        k = _rclMesh._aclFacetArray[i]._aulNeighbours[j];
        l = _rclMesh._aclFacetArray[k].Side(i);
        if (_rclMesh._aclEdgeArray.Find(k, l) != ULONG_MAX)
        {
          ulInds.push_back(i);
          ulInds.push_back(k);
        }
      } break;
    }
    case DuplicatedFacets:
    {
      // get all facets
      std::map<TMeshFacetInds, std::vector<unsigned long> > aFIndsMap;
      for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++, i++)
      {
        TMeshFacetInds FInds(pFIter->_aulPoints[0], pFIter->_aulPoints[1], pFIter->_aulPoints[2]);
        aFIndsMap[FInds].push_back(i);
      }

      // get all double facets
      std::vector<unsigned long> idx;
      for (std::map<TMeshFacetInds, std::vector<unsigned long> >::iterator it = aFIndsMap.begin(); it != aFIndsMap.end(); ++it)
      {
        idx = it->second;
        if (idx.size() > 1)
        {
          for (std::vector<unsigned long>::iterator it2 = idx.begin(); it2 != idx.end(); ++it2)
            ulInds.push_back(*it2);
        }
      } break;
    }
    case DuplicatedPoints:
    {
      std::vector<MeshHelpPoint> aclPoints(_rclMesh.CountPoints());
      std::vector<MeshHelpPoint>::iterator   pI = aclPoints.begin();
      for (pPIter = _rclMesh._aclPointArray.begin(); pPIter < _rclMesh._aclPointArray.end(); pPIter++, pI++)
        pI->Set(0, 0, *pPIter);
      std::sort(aclPoints.begin(), aclPoints.end());
      if (std::adjacent_find(aclPoints.begin(), aclPoints.end()) < aclPoints.end())
      {
        std::map<unsigned long, std::vector<unsigned long> > aulPoint2Facet;
        i = 0;
        for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++, i++)
        {
          for (int i=0; i<3; i++)
          {
            aulPoint2Facet[pFIter->_aulPoints[i]].push_back(i);
          }
        }

        i = 0;

        // get all multiple points
        std::vector<MeshHelpPoint>::iterator it  = aclPoints.begin();
        std::vector<MeshHelpPoint>::iterator it2 = aclPoints.begin();
        std::vector<unsigned long> aclMultiple;
        for (++it2; it2 != aclPoints.end() && it != aclPoints.end(); ++it2, ++it)
        {
          if ((*it2) == (*it))
            aclMultiple.push_back(_rclMesh._aclPointArray.GetOrAddIndex(it2->_clPt));
        }

        for (std::vector<unsigned long>::iterator it3 = aclMultiple.begin(); it3 != aclMultiple.end(); ++it3)
        {
          for (std::vector<unsigned long>::iterator it4 = aulPoint2Facet[*it3].begin(); it4 != aulPoint2Facet[*it3].end(); it4++)
          {
            ulInds.push_back(*it4);
          }
        }

        std::sort(ulInds.begin(), ulInds.end());
        ulInds.erase(std::unique(ulInds.begin(), ulInds.end()), ulInds.end());
      } break;
    }
    case OutOfRangeEdge:
    {
      unsigned long ii, jj;
      for (pEIter = _rclMesh._aclEdgeArray.begin(); pEIter < _rclMesh._aclEdgeArray.end(); pEIter++, i++)
      {
        ii = pEIter->Index();
        jj = pEIter->Side();
        if ((ii >= ulCtFacets) || (jj > 2))
        {
          ulInds.push_back(ii);
        }
      } break;
    }
    case OutOfRangePoint:
    {
      MeshFacetArray::_TIterator  pFIter;
      for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++, i++)
      {
        if (std::find_if(pFIter->_aulPoints, pFIter->_aulPoints + 3,
          std::bind2nd(std::greater_equal<unsigned long>(), ulCtPoints)) < pFIter->_aulPoints + 3)
        {
          ulInds.push_back(i);
        }
      } break;
    }
    case OutOfRangeFacet:
    {
      MeshFacetArray::_TIterator  pFIter;
      for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++, i++)
      {
        for (i = 0; i < 3; i++)
        {
          if ((pFIter->_aulNeighbours[i] >= ulCtFacets) && (pFIter->_aulNeighbours[i] < ULONG_MAX))
          {
            ulInds.push_back(i);
            break;
          }
        }
      } break;
    }
    case CorruptedFacets:
    {
      MeshFacetArray::_TIterator  pFIter;
      for (pFIter = _rclMesh._aclFacetArray.begin(); pFIter < _rclMesh._aclFacetArray.end(); pFIter++, i++)
      {
        if ((pFIter->_aulPoints[0] == pFIter->_aulPoints[1]) ||
            (pFIter->_aulPoints[0] == pFIter->_aulPoints[2]) ||
            (pFIter->_aulPoints[1] == pFIter->_aulPoints[2]))
        {
          ulInds.push_back(i);
        }
      } break;
    }
    case DegeneratedFacets:
    case Undefined:
      break;
  }

  return ulInds;
}
