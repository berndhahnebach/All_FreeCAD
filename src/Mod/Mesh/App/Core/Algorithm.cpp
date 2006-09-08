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
# include <Wm3Matrix3.h>
# include <Wm3Vector3.h>
#endif

#include "Algorithm.h"
#include "Elements.h"
#include "Iterator.h"
#include "Grid.h"

#include <Base/Sequencer.h>

using namespace MeshCore;
using Base::BoundBox3f;
using Base::BoundBox2D;
using Base::Polygon2D;


bool MeshAlgorithm::IsVertexVisible (const Base::Vector3f &rcVertex, const Base::Vector3f &rcView, const MeshFacetGrid &rclGrid ) const
{
  Base::Vector3f cDirection = rcVertex-rcView;
  float fDistance = cDirection.Length();
  Base::Vector3f cIntsct; unsigned long uInd;

  // search for the nearest facet to rcView in direction to rcVertex
  if ( NearestFacetOnRay( rcView, cDirection, /*1.2f*fDistance,*/ rclGrid, cIntsct, uInd) )
  {
    // now check if the facet overlays the point
    float fLen = Base::Distance( rcView, cIntsct );
    if ( fLen < fDistance )
    {
      // is it the same point?
      if ( Base::Distance(rcVertex, cIntsct) > 0.001f )
      {
        // ok facet overlays the vertex
        return false;
      }
    }
  }

  return true; // no facet between the two points
}

bool MeshAlgorithm::NearestFacetOnRay (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, Base::Vector3f &rclRes,
                                       unsigned long &rulFacet) const
{
  Base::Vector3f clProj, clRes;
  bool bSol = false;
  unsigned long ulInd = 0;

  // langsame Ausfuehrung ohne Grid
  MeshFacetIterator  clFIter(_rclMesh);
  for (clFIter.Init(); clFIter.More(); clFIter.Next())
  {
    if (clFIter->Foraminate( rclPt, rclDir, clRes ) == true)
    {
      if (bSol == false) // erste Loesung
      {
        bSol   = true;
        clProj = clRes;
        ulInd  = clFIter.Position();
      }
      else
      {  // liegt Punkt naeher
        if ((clRes - rclPt).Length() < (clProj - rclPt).Length())
        {
          clProj = clRes;
          ulInd  = clFIter.Position();
        }
      }
    }
  }

  rclRes   = clProj;
  rulFacet = ulInd;
  return bSol;
}

bool MeshAlgorithm::NearestFacetOnRay (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, const MeshFacetGrid &rclGrid,
                                       Base::Vector3f &rclRes, unsigned long &rulFacet) const
{
  std::vector<unsigned long> aulFacets;
  MeshGridIterator clGridIter(rclGrid);

  if (clGridIter.InitOnRay(rclPt, rclDir, aulFacets) == true)
  {
    if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet) == false)
    {
      aulFacets.clear();
      while (clGridIter.NextOnRay(aulFacets) == true)
      {
        if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet) == true)
          return true;
      }
    }
    else
      return true;
  } 

  return false;
}

bool MeshAlgorithm::NearestFacetOnRay (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, float fMaxSearchArea,
                                       const MeshFacetGrid &rclGrid, Base::Vector3f &rclRes, unsigned long &rulFacet) const
{
  std::vector<unsigned long> aulFacets;
  MeshGridIterator  clGridIter(rclGrid);

  if (clGridIter.InitOnRay(rclPt, rclDir, fMaxSearchArea, aulFacets) == true)
  {
    if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet, 1.75f) == false)
    {
      aulFacets.clear();
      while (clGridIter.NextOnRay(aulFacets) == true)
      {
        if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet, 1.75f) == true)
          return true;
      }
    }
    else
      return true;
  } 

  return false;
}

bool MeshAlgorithm::NearestFacetOnRay (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, const std::vector<unsigned long> &raulFacets,
                                       Base::Vector3f &rclRes, unsigned long &rulFacet) const
{
  Base::Vector3f  clProj, clRes;
  bool bSol = false;
  unsigned long ulInd = 0;
 
  for (std::vector<unsigned long>::const_iterator pI = raulFacets.begin(); pI != raulFacets.end(); pI++)
  {
    MeshGeomFacet rclSFacet = _rclMesh.GetFacet(*pI);

    if (rclSFacet.Foraminate(rclPt, rclDir, clRes) == true)
    {
      if (bSol == false) // erste Loesung
      {
        bSol   = true;
        clProj = clRes;
        ulInd  = *pI;
      }
      else
      {  // liegt Punkt naeher
        if ((clRes - rclPt).Length() < (clProj - rclPt).Length())
        {
          clProj = clRes;
          ulInd  = *pI;
        }
      }
    }
  }

  rclRes   = clProj;
  rulFacet = ulInd;
  return bSol;
}

bool MeshAlgorithm::RayNearestField (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, const std::vector<unsigned long> &raulFacets,
                                     Base::Vector3f &rclRes, unsigned long &rulFacet, float fMaxAngle) const
{
  Base::Vector3f  clProj, clRes;
  bool bSol = false;
  unsigned long ulInd = 0;

  for (std::vector<unsigned long>::const_iterator pF = raulFacets.begin(); pF != raulFacets.end(); pF++)
  {
    if (_rclMesh.GetFacet(*pF).Foraminate(rclPt, rclDir, clRes/*, fMaxAngle*/) == true)
    {
      if (bSol == false) // erste Loesung
      {
        bSol   = true;
        clProj = clRes;
        ulInd  = *pF;
      }
      else
      {  // liegt Punkt naeher
        if ((clRes - rclPt).Length() < (clProj - rclPt).Length())
        {
          clProj = clRes;
          ulInd  = *pF;
        }
      }
    }
  }

  rclRes   = clProj;
  rulFacet = ulInd;

  return bSol;
}

float MeshAlgorithm::GetAverageEdgeLength() const
{
  float fLen = 0.0f;
  MeshFacetIterator cF(_rclMesh);
  for ( cF.Init(); cF.More(); cF.Next() )
  {
    for ( int i=0; i<3; i++ )
      fLen += Base::Distance( cF->_aclPoints[i], cF->_aclPoints[(i+1)%3] );
  }

  fLen = fLen / (3.0f * _rclMesh.CountFacets() );
  return fLen;
}

void MeshAlgorithm::GetMeshBorders (std::list<std::vector<Base::Vector3f> > &rclBorders) const
{
  std::vector<unsigned long> aulAllFacets(_rclMesh.CountFacets());
  unsigned long k = 0;
  for (std::vector<unsigned long>::iterator pI = aulAllFacets.begin(); pI != aulAllFacets.end(); pI++)
    *pI = k++;

  GetFacetBorders(aulAllFacets, rclBorders);
}

void MeshAlgorithm::GetMeshBorders (std::list<std::vector<unsigned long> > &rclBorders) const
{
  std::vector<unsigned long> aulAllFacets(_rclMesh.CountFacets());
  unsigned long k = 0;
  for (std::vector<unsigned long>::iterator pI = aulAllFacets.begin(); pI != aulAllFacets.end(); pI++)
    *pI = k++;

  GetFacetBorders(aulAllFacets, rclBorders);
}

void MeshAlgorithm::GetFacetBorders (const std::vector<unsigned long> &raulInd, std::list<std::vector<Base::Vector3f> > &rclBorders) const
{
#if 0
  //FIXME: Test this code!
  const MeshPointArray &rclPAry = _rclMesh._aclPointArray;
  std::list<std::vector<unsigned long> > aulBorders;

  GetFacetBorders ( raulInd, aulBorders );
  for ( std::list<std::vector<unsigned long> >::iterator it = aulBorders.begin(); it != aulBorders.end(); ++it )
  {
    std::vector<Base::Vector3f> boundary;
    boundary.reserve( it->size() );

    for ( std::vector<unsigned long>::iterator jt = it->begin(); jt != it->end(); ++jt )
      boundary.push_back(rclPAry[*jt]);

    rclBorders.push_back( boundary );
  }

#else
  const MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;

  // alle Facets markieren die in der Indizie-Liste vorkommen
  ResetFacetFlag(MeshFacet::VISIT);
  for (std::vector<unsigned long>::const_iterator pIter = raulInd.begin(); pIter != raulInd.end(); pIter++)
    rclFAry[*pIter].SetFlag(MeshFacet::VISIT);

  std::list<std::pair<unsigned long, unsigned long> >  aclEdges;
  // alle Randkanten suchen und ablegen (unsortiert)
  for (std::vector<unsigned long>::const_iterator pIter2 = raulInd.begin(); pIter2 != raulInd.end(); pIter2++)
  {
    const MeshFacet  &rclFacet = rclFAry[*pIter2];
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulNB = rclFacet._aulNeighbours[i];
      if (ulNB != ULONG_MAX)
      {
        if (rclFAry[ulNB].IsFlag(MeshFacet::VISIT) == true)
          continue;
      }

      aclEdges.push_back(rclFacet.GetEdge(i));
    }
  }

  if (aclEdges.size() == 0)
    return; // no borders found (=> solid)

  // Kanten aus der unsortieren Kantenliste suchen
  const MeshPointArray &rclPAry = _rclMesh._aclPointArray;
  unsigned long              ulFirst, ulLast;
  std::list<Base::Vector3f>   clBorder;
  ulFirst = aclEdges.begin()->first;
  ulLast  = aclEdges.begin()->second;

  aclEdges.erase(aclEdges.begin());
  clBorder.push_back(rclPAry[ulFirst]);
  clBorder.push_back(rclPAry[ulLast]);

  while (aclEdges.size() > 0)
  {
    // naechste anliegende Kante suchen
    std::list<std::pair<unsigned long, unsigned long> >::iterator pEI;
    for (pEI = aclEdges.begin(); pEI != aclEdges.end(); pEI++)
    {
      if (pEI->first == ulLast)
      {
        ulLast = pEI->second;
        clBorder.push_back(rclPAry[ulLast]);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->second == ulLast)
      {
        ulLast = pEI->first;
        clBorder.push_back(rclPAry[ulLast]);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->first == ulFirst)
      {
        ulFirst = pEI->second;
        clBorder.push_front(rclPAry[ulFirst]);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->second == ulFirst)
      {
        ulFirst = pEI->first;
        clBorder.push_front(rclPAry[ulFirst]);
        aclEdges.erase(pEI);
        break;
      }
    }
    if ((pEI == aclEdges.end()) || (ulLast == ulFirst))
    {  // keine weitere Kante gefunden bzw. Polylinie geschlossen
      rclBorders.push_back(std::vector<Base::Vector3f>(clBorder.begin(), clBorder.end()));
      clBorder.clear();

      if (aclEdges.size() > 0)
      {  // neue Border anfangen
        ulFirst = aclEdges.begin()->first;
        ulLast  = aclEdges.begin()->second;
        aclEdges.erase(aclEdges.begin());
        clBorder.push_back(rclPAry[ulFirst]);
        clBorder.push_back(rclPAry[ulLast]);
      }
    }
  }
#endif
}

void MeshAlgorithm::GetFacetBorders (const std::vector<unsigned long> &raulInd, std::list<std::vector<unsigned long> > &rclBorders) const
{
  const MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;

  // alle Facets markieren die in der Indizie-Liste vorkommen
  ResetFacetFlag(MeshFacet::VISIT);
  for (std::vector<unsigned long>::const_iterator pIter = raulInd.begin(); pIter != raulInd.end(); pIter++)
    rclFAry[*pIter].SetFlag(MeshFacet::VISIT);

  std::list<std::pair<unsigned long, unsigned long> >  aclEdges;
  // alle Randkanten suchen und ablegen (unsortiert)
  for (std::vector<unsigned long>::const_iterator pIter2 = raulInd.begin(); pIter2 != raulInd.end(); pIter2++)
  {
    const MeshFacet  &rclFacet = rclFAry[*pIter2];
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulNB = rclFacet._aulNeighbours[i];
      if (ulNB != ULONG_MAX)
      {
        if (rclFAry[ulNB].IsFlag(MeshFacet::VISIT) == true)
          continue;
      }

      aclEdges.push_back(rclFacet.GetEdge(i));
    }
  }

  if (aclEdges.size() == 0)
    return; // no borders found (=> solid)

  // Kanten aus der unsortieren Kantenliste suchen
  unsigned long              ulFirst, ulLast;
  std::list<unsigned long>   clBorder;
  ulFirst = aclEdges.begin()->first;
  ulLast  = aclEdges.begin()->second;

  aclEdges.erase(aclEdges.begin());
  clBorder.push_back(ulFirst);
  clBorder.push_back(ulLast);

  while (aclEdges.size() > 0)
  {
    // naechste anliegende Kante suchen
    std::list<std::pair<unsigned long, unsigned long> >::iterator pEI;
    for (pEI = aclEdges.begin(); pEI != aclEdges.end(); pEI++)
    {
      if (pEI->first == ulLast)
      {
        ulLast = pEI->second;
        clBorder.push_back(ulLast);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->second == ulLast)
      {
        ulLast = pEI->first;
        clBorder.push_back(ulLast);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->first == ulFirst)
      {
        ulFirst = pEI->second;
        clBorder.push_front(ulFirst);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->second == ulFirst)
      {
        ulFirst = pEI->first;
        clBorder.push_front(ulFirst);
        aclEdges.erase(pEI);
        break;
      }
    }
    if ((pEI == aclEdges.end()) || (ulLast == ulFirst))
    {  // keine weitere Kante gefunden bzw. Polylinie geschlossen
      rclBorders.push_back(std::vector<unsigned long>(clBorder.begin(), clBorder.end()));
      clBorder.clear();

      if (aclEdges.size() > 0)
      {  // neue Border anfangen
        ulFirst = aclEdges.begin()->first;
        ulLast  = aclEdges.begin()->second;
        aclEdges.erase(aclEdges.begin());
        clBorder.push_back(ulFirst);
        clBorder.push_back(ulLast);
      }
    }
  }
}

void MeshAlgorithm::SetFacetsProperty(const std::vector<unsigned long> &raulInds, const std::vector<unsigned long> &raulProps) const
{
  if (raulInds.size() != raulProps.size()) return;

  std::vector<unsigned long>::const_iterator iP = raulProps.begin();
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++, iP++)
    _rclMesh._aclFacetArray[*i].SetProperty(*iP);
}

void MeshAlgorithm::SetFacetsFlag (const std::vector<unsigned long> &raulInds, MeshFacet::TFlagType tF) const
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclFacetArray[*i].SetFlag(tF);
}

void MeshAlgorithm::SetPointsFlag (const std::vector<unsigned long> &raulInds, MeshPoint::TFlagType tF) const 
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclPointArray[*i].SetFlag(tF);
}

void MeshAlgorithm::ResetFacetsFlag (const std::vector<unsigned long> &raulInds, MeshFacet::TFlagType tF) const
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclFacetArray[*i].ResetFlag(tF);
}

void MeshAlgorithm::ResetPointsFlag (const std::vector<unsigned long> &raulInds, MeshPoint::TFlagType tF) const
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclPointArray[*i].ResetFlag(tF);
}

void MeshAlgorithm::SetFacetFlag (MeshFacet::TFlagType tF) const
{
  _rclMesh._aclFacetArray.SetFlag(tF);
}

void MeshAlgorithm::SetPointFlag (MeshPoint::TFlagType tF) const
{
  _rclMesh._aclPointArray.SetFlag(tF);
}

void MeshAlgorithm::ResetFacetFlag (MeshFacet::TFlagType tF) const
{
  _rclMesh._aclFacetArray.ResetFlag(tF);
}

void MeshAlgorithm::ResetPointFlag (MeshPoint::TFlagType tF) const
{
  _rclMesh._aclPointArray.ResetFlag(tF);
}

unsigned long MeshAlgorithm::CountFacetFlag (MeshFacet::TFlagType tF) const
{
  return std::count_if(_rclMesh._aclFacetArray.begin(), _rclMesh._aclFacetArray.end(),
                    std::bind2nd(MeshIsFlag<MeshFacet>(), tF));
}

unsigned long MeshAlgorithm::CountPointFlag (MeshPoint::TFlagType tF) const
{
  return std::count_if(_rclMesh._aclPointArray.begin(), _rclMesh._aclPointArray.end(),
                    std::bind2nd(MeshIsFlag<MeshPoint>(), tF));
}

void MeshAlgorithm::GetFacetsFromToolMesh( const MeshKernel& rToolMesh, const Base::Vector3f& rcDir, std::vector<unsigned long> &raclCutted ) const
{
  MeshFacetIterator cFIt(_rclMesh);
  MeshFacetIterator cTIt(rToolMesh);

  BoundBox3f cBB = rToolMesh.GetBoundBox();

  Base::SequencerLauncher seq("Check facets...", _rclMesh.CountFacets());

  // check all facets
  Base::Vector3f tmp;
  for ( cFIt.Init(); cFIt.More(); cFIt.Next() )
  {
    // check each point of each facet
    for ( int i=0; i<3; i++ )
    {
      // at least the point must be inside the bounding box of the tool mesh
      if ( cBB.IsInBox( cFIt->_aclPoints[i] ) )
      {
        // should not cause performance problems since the tool mesh is usually rather lightweight
        int ct=0;
        for ( cTIt.Init(); cTIt.More(); cTIt.Next() )
        {
          if ( cTIt->IsPointOfFace( cFIt->_aclPoints[i], FLOAT_EPS ) )
          {
            ct=1;
            break; // the point lies on the tool mesh
          }
          else if ( cTIt->Foraminate( cFIt->_aclPoints[i], rcDir, tmp ) )
          {
            // check if the intersection point lies in direction rcDir of the considered point
            if ( (tmp - cFIt->_aclPoints[i]) * rcDir > 0 )
              ct++;
          }
        }

        // odd number => point is inside the tool mesh
        if ( ct % 2 == 1 )
        {
          raclCutted.push_back( cFIt.Position() );
          break;
        }
      }
    }

    Base::Sequencer().next();
  }
}

void MeshAlgorithm::GetFacetsFromToolMesh( const MeshKernel& rToolMesh, const Base::Vector3f& rcDir, const MeshFacetGrid& rGrid, std::vector<unsigned long> &raclCutted ) const
{
  // iterator over grid structure
  MeshGridIterator clGridIter(rGrid);
  BoundBox3f cBB = rToolMesh.GetBoundBox();
  Base::Vector3f tmp;

  MeshFacetIterator cFIt(_rclMesh);
  MeshFacetIterator cTIt(rToolMesh);
  MeshAlgorithm cToolAlg(rToolMesh);

  // To speed up the algorithm we use the grid built up from the associated mesh. For each grid element we check whether it lies completely inside 
  // or outside the toolmesh or even intersect with the toolmesh. So we can reduce the number of facets with further tests dramatically.
  // If the grid box is outside the toolmesh all the facets inside can be skipped. If the grid box is inside the toolmesh all facets are stored with
  // no further tests because they must also lie inside the toolmesh. Finally, if the grid box intersect with the toolmesh we must also check for each
  // whether it intersect we the toolmesh as well.
  std::vector<unsigned long> aulInds;
  for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
  {
    int ret = cToolAlg.Surround( clGridIter.GetBoundBox(), rcDir );

    // the box is completely inside the toolmesh
    if ( ret == 1 ) {
      // these facets can be removed without more checks
      clGridIter.GetElements(raclCutted);
    } 
    // the box intersect with toolmesh
    else if ( ret == 0 ) {
      // these facets must be tested for intersectons with the toolmesh
      clGridIter.GetElements(aulInds);
    }
  }

  // remove duplicates
  std::sort(aulInds.begin(), aulInds.end());
  aulInds.erase(std::unique(aulInds.begin(), aulInds.end()), aulInds.end());
  std::sort(raclCutted.begin(), raclCutted.end());
  raclCutted.erase(std::unique(raclCutted.begin(), raclCutted.end()), raclCutted.end());

  Base::SequencerLauncher seq("Check facets...", aulInds.size());

  // check all facets
  for ( std::vector<unsigned long>::iterator it = aulInds.begin(); it != aulInds.end(); ++it )
  {
    cFIt.Set( *it );

    // check each point of each facet
    for ( int i=0; i<3; i++ )
    {
      // at least the point must be inside the bounding box of the tool mesh
      if ( cBB.IsInBox( cFIt->_aclPoints[i] ) )
      {
        // should not cause performance problems since the tool mesh is usually rather lightweight
        int ct=0;
        for ( cTIt.Init(); cTIt.More(); cTIt.Next() )
        {
          if ( cTIt->IsPointOfFace( cFIt->_aclPoints[i], FLOAT_EPS ) )
          {
            ct=1;
            break; // the point lies on the tool mesh
          }
          else if ( cTIt->Foraminate( cFIt->_aclPoints[i], rcDir, tmp ) )
          {
            // check if the intersection point lies in direction rcDir of the considered point
            if ( (tmp - cFIt->_aclPoints[i]) * rcDir > 0 )
              ct++;
          }
        }

        // odd number => point is inside the tool mesh
        if ( ct % 2 == 1 )
        {
          raclCutted.push_back( cFIt.Position() );
          break;
        }
      }
    }

    Base::Sequencer().next();
  }

  // remove duplicates
  std::sort(raclCutted.begin(), raclCutted.end());
  raclCutted.erase(std::unique(raclCutted.begin(), raclCutted.end()), raclCutted.end());
}

int MeshAlgorithm::Surround( const Base::BoundBox3f& rBox, const Base::Vector3f& rcDir )
{
  Base::Vector3f pt1, pt2, tmp;
  const BoundBox3f& cBB = _rclMesh.GetBoundBox();

  // at least both boxes intersect
  if ( cBB && rBox )
  {
    // check for intersections with the actual mesh
    Base::Vector3f cCorner[8] = {
      Base::Vector3f(rBox.MinX,rBox.MinY,rBox.MinZ), Base::Vector3f(rBox.MaxX,rBox.MinY,rBox.MinZ), 
      Base::Vector3f(rBox.MaxX,rBox.MaxY,rBox.MinZ), Base::Vector3f(rBox.MinX,rBox.MaxY,rBox.MinZ),
      Base::Vector3f(rBox.MinX,rBox.MinY,rBox.MaxZ), Base::Vector3f(rBox.MaxX,rBox.MinY,rBox.MaxZ), 
      Base::Vector3f(rBox.MaxX,rBox.MaxY,rBox.MaxZ), Base::Vector3f(rBox.MinX,rBox.MaxY,rBox.MaxZ)
    };

    MeshFacetIterator cTIt(_rclMesh);

    // triangulation of the box
    int triangles[36] = {
      0,1,2,0,2,3,
      0,1,5,0,5,4,
      0,4,7,0,7,3,
      6,7,4,6,4,5,
      6,2,3,6,3,7,
      6,1,2,6,5,1
    };

    std::vector<MeshGeomFacet> cFacet(12);
    int id=0;
    for ( int ii=0; ii<12; ii++ )
    {
      cFacet[ii]._aclPoints[0]=cCorner[triangles[id++]]; 
      cFacet[ii]._aclPoints[1]=cCorner[triangles[id++]]; 
      cFacet[ii]._aclPoints[2]=cCorner[triangles[id++]];
    }

    // check for intersections of the box with the mesh
    for ( std::vector<MeshGeomFacet>::iterator it = cFacet.begin(); it != cFacet.end(); ++it )
    {
      for ( cTIt.Init(); cTIt.More(); cTIt.Next() )
      {
        int ret = cTIt->IntersectWithFacet1(*it,pt1, pt2);

        // the box intersects the mesh?
        if ( ret != 0)
          return 0; // => no more investigations required
      }
    }

    // Now we know that the box doesn't intersect with the mesh. This means that either the box is completely inside
    // or outside the mesh. To check this we test one point of the box whether it is inside or outside.
    int ct=0;
    for ( cTIt.Init(); cTIt.More(); cTIt.Next() )
    {
      if ( cTIt->IsPointOfFace( cCorner[0], FLOAT_EPS ) )
      {
        ct=1;
        break; // the point lies on the tool mesh
      }
      else if ( cTIt->Foraminate( cCorner[0], rcDir, tmp ) )
      {
        // check if the intersection point lies in direction rcDir of the considered point
        if ( (tmp - cCorner[0]) * rcDir > 0 )
          ct++;
      }
    }

    // odd number => point (i.e. the box) is inside the mesh, even number => point is outside the mesh
    return (ct % 2 == 1) ? 1 : -1;
  }

  // no intersection the box is outside the mesh
  return -1;
}

void MeshAlgorithm::CheckFacets(const MeshFacetGrid& rclGrid, const Base::ViewProjMethod* pclProj, const Base::Polygon2D& rclPoly,
                                bool bInner, std::vector<unsigned long> &raulFacets) const
{
    std::vector<unsigned long>::iterator it;
    MeshFacetIterator clIter(_rclMesh, 0);
    Base::Vector3f clPt2d;
    Base::Vector3f clGravityOfFacet;
    bool bNoPointInside;

    // Falls true, verwende Grid auf Mesh, um Suche zu beschleunigen
    if (bInner)
    {
        BoundBox3f clBBox3d;
        BoundBox2D clViewBBox, clPolyBBox;
        std::vector<unsigned long> aulAllElements;

        //B-Box des Polygons
        clPolyBBox = rclPoly.CalcBoundBox();
        // Iterator fuer die zu durchsuchenden B-Boxen des Grids
        MeshGridIterator clGridIter(rclGrid);
        // alle B-Boxen durchlaufen und die Facets speichern
        for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
        {
            clBBox3d = clGridIter.GetBoundBox();
            clViewBBox = clBBox3d.ProjectBox(pclProj);
            if (clViewBBox || clPolyBBox)
            {
                // alle Elemente in AllElements sammeln
                clGridIter.GetElements(aulAllElements);
            }
        }
        // doppelte Elemente wieder entfernen
        std::sort(aulAllElements.begin(), aulAllElements.end());
        aulAllElements.erase(std::unique(aulAllElements.begin(), aulAllElements.end()), aulAllElements.end());

        Base::Sequencer().start( "Check facets", aulAllElements.size() );

        for (it = aulAllElements.begin(); it != aulAllElements.end(); it++)
        {
            bNoPointInside = true;
            clGravityOfFacet.Set(0.0f, 0.0f, 0.0f);
            MeshGeomFacet rclFacet = _rclMesh.GetFacet(*it);
            for (int j=0; j<3; j++)
            {
                clPt2d = pclProj->operator()(rclFacet._aclPoints[j]);
                clGravityOfFacet += clPt2d;
                if (rclPoly.Contains(Base::Vector2D(clPt2d.x, clPt2d.y)) == bInner)
                {
                    raulFacets.push_back(*it);
                    bNoPointInside = false;
                    break;
                }
            }

            // if no facet point is inside the polygon then check also the gravity
            if (bNoPointInside == true)
            {
              clGravityOfFacet *= 1.0f/3.0f;

              if (rclPoly.Contains(Base::Vector2D(clGravityOfFacet.x, clGravityOfFacet.y)) == bInner)
                 raulFacets.push_back(*it);
            }

            Base::Sequencer().next();
        }
        Base::Sequencer().stop();
    }
    // Dreiecke ausserhalb schneiden, dann alles durchsuchen
    else
    {
      Base::Sequencer().start("Check facets", _rclMesh.CountFacets());
      for (clIter.Init(); clIter.More(); clIter.Next())
      {
          for (int j=0; j<3; j++)
          {
              clPt2d = pclProj->operator()(clIter->_aclPoints[j]);
              if (rclPoly.Contains(Base::Vector2D(clPt2d.x, clPt2d.y)) == bInner)
              {
                  raulFacets.push_back(clIter.Position());
                  break;
              }
          }
          Base::Sequencer().next();
      }
      Base::Sequencer().stop();
    }
}

#if 0
bool MeshAlgorithm::MeshTopoShape(TopoDS_Shape aShape, float fAccuracy, float fAngle) const
{
  Standard_Integer e1,e2,e3,i1,i2,i3;
  Standard_Boolean b1,b2,b3;
  std::vector<MeshGeomFacet> vFacets;
  MeshGeomFacet cSimpleFacet;

  try{
    BRepMesh_Discret mesh(fAccuracy,aShape,fAngle);
    //BRepMesh_Discret mesh(fAccuracy,aShape,fAngle);
    int NbTri = mesh.NbTriangles() ;
    for ( int nbt = 1, i = 1 ;nbt <= NbTri;nbt++, i += 3 ) {
      BRepMesh_Triangle tri = mesh.Triangle(nbt);
      tri.Edges(e1,e2,e3,b1,b2,b3);
        if (b1) {
          i1 = mesh.Edge(e1).FirstNode() ;
          i2 = mesh.Edge(e1).LastNode() ;
        }
        else {
          i1 = mesh.Edge(e1).LastNode() ;
          i2 = mesh.Edge(e1).FirstNode() ;
        }

        if (b2) {
          i3 = mesh.Edge(e2).LastNode();
        }
        else {
          i3 = mesh.Edge(e2).FirstNode() ;
        }
        cSimpleFacet._aclPoints[0].x = float(mesh.Pnt( i1 ).X());
        cSimpleFacet._aclPoints[0].y = float(mesh.Pnt( i1 ).Y());
        cSimpleFacet._aclPoints[0].z = float(mesh.Pnt( i1 ).Z());
        cSimpleFacet._aclPoints[1].x = float(mesh.Pnt( i2 ).X());
        cSimpleFacet._aclPoints[1].y = float(mesh.Pnt( i2 ).Y());
        cSimpleFacet._aclPoints[1].z = float(mesh.Pnt( i2 ).Z());
        cSimpleFacet._aclPoints[2].x = float(mesh.Pnt( i3 ).X());
        cSimpleFacet._aclPoints[2].y = float(mesh.Pnt( i3 ).Y());
        cSimpleFacet._aclPoints[2].z = float(mesh.Pnt( i3 ).Z());

        vFacets.push_back(cSimpleFacet);
   }

    _rclMesh.Clear();
    _rclMesh = vFacets;

  } catch(...){
      return false;
  }

  return true;
}
#endif
float MeshAlgorithm::Surface (void) const
{
  float              fTotal = 0.0f;
  MeshFacetIterator clFIter(_rclMesh);

  for (clFIter.Init(); clFIter.More(); clFIter.Next())
    fTotal +=  clFIter->Area();
  
  return fTotal;
}

void MeshAlgorithm::SubSampleByDist (float fDist, std::vector<Base::Vector3f> &rclPoints) const
{ 
  rclPoints.clear();
  MeshFacetIterator clFIter(_rclMesh);
  for (clFIter.Init(); clFIter.More(); clFIter.Next())
  {
    unsigned long k = rclPoints.size();
    clFIter->SubSample(fDist, rclPoints);
    if (rclPoints.size() == k)
      rclPoints.push_back(clFIter->GetGravityPoint()); // min. add middle point
  }
}

void MeshAlgorithm::SubSampleAllPoints (std::vector<Base::Vector3f> &rclPoints) const
{ 
  rclPoints.clear();

  // Add all Points
  //
  MeshPointIterator clPIter(_rclMesh);
  for (clPIter.Init(); clPIter.More(); clPIter.Next())
  {
      rclPoints.push_back(*clPIter);
  }
}

void MeshAlgorithm::SubSampleByCount (unsigned long ulCtPoints, std::vector<Base::Vector3f> &rclPoints) const
{
  float fDist = float(sqrt(Surface() / float(ulCtPoints)));
  SubSampleByDist(fDist, rclPoints);
}

void MeshAlgorithm::SearchFacetsFromPolyline (const std::vector<Base::Vector3f> &rclPolyline, float fRadius,
                                              const MeshFacetGrid& rclGrid, std::vector<unsigned long> &rclResultFacetsIndices) const
{
  rclResultFacetsIndices.clear();
  if ( rclPolyline.size() < 3 )
    return; // no polygon defined

  std::set<unsigned long>  aclFacets;
  for (std::vector<Base::Vector3f>::const_iterator pV = rclPolyline.begin(); pV < (rclPolyline.end() - 1); pV++)
  {
    const Base::Vector3f &rclP0 = *pV, &rclP1 = *(pV + 1);
   
    // BB eines Polyline-Segments
    BoundBox3f clSegmBB(rclP0.x, rclP0.y, rclP0.z, rclP0.x, rclP0.y, rclP0.z);
    clSegmBB &= rclP1;
    clSegmBB.Enlarge(fRadius);  // BB um Suchradius vergroessern

    std::vector<unsigned long> aclBBFacets;
    unsigned long k = rclGrid.Inside(clSegmBB, aclBBFacets, false);
    for (unsigned long i = 0; i < k; i++)
    {
      if (_rclMesh.GetFacet(aclBBFacets[i]).DistanceToLineSegment(rclP0, rclP1) < fRadius)
        aclFacets.insert(aclBBFacets[i]);
    }
  }

  rclResultFacetsIndices.insert(rclResultFacetsIndices.begin(), aclFacets.begin(), aclFacets.end());
}

void MeshAlgorithm::CutBorderFacets (std::vector<unsigned long> &raclFacetIndices, unsigned short usLevel) const
{
  std::vector<unsigned long> aclToDelete;

  CheckBorderFacets(raclFacetIndices, aclToDelete, usLevel);

  // alle gefunden "Rand"-Facetsindizes" aus dem Array loeschen
  std::vector<unsigned long>  aclResult;
  std::set<unsigned long>     aclTmp(aclToDelete.begin(), aclToDelete.end());

  for (std::vector<unsigned long>::iterator pI = raclFacetIndices.begin(); pI != raclFacetIndices.end(); pI++)
  {
    if (aclTmp.find(*pI) == aclTmp.end())
      aclResult.push_back(*pI);
  }

  raclFacetIndices = aclResult;
}

void MeshAlgorithm::CheckBorderFacets (const std::vector<unsigned long> &raclFacetIndices, std::vector<unsigned long> &raclResultIndices, unsigned short usLevel) const
{
  ResetFacetFlag(MeshFacet::TMP0);
  SetFacetsFlag(raclFacetIndices, MeshFacet::TMP0);

  const MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;

  for (unsigned short usL = 0; usL < usLevel; usL++)
  {
    for (std::vector<unsigned long>::const_iterator pF = raclFacetIndices.begin(); pF != raclFacetIndices.end(); pF++)
    {
      for (int i = 0; i < 3; i++)
      {
        unsigned long ulNB = rclFAry[*pF]._aulNeighbours[i];
        if (ulNB == ULONG_MAX)
        {
          raclResultIndices.push_back(*pF);
          rclFAry[*pF].ResetFlag(MeshFacet::TMP0);
          continue;
        }
        if (rclFAry[ulNB].IsFlag(MeshFacet::TMP0) == false)
        {
          raclResultIndices.push_back(*pF);
          rclFAry[*pF].ResetFlag(MeshFacet::TMP0);
          continue;
        }
      }
    }
  }
}

void MeshAlgorithm::GetBorderPoints (const std::vector<unsigned long> &raclFacetIndices, std::set<unsigned long> &raclResultPointsIndices) const
{
  ResetFacetFlag(MeshFacet::TMP0);
  SetFacetsFlag(raclFacetIndices, MeshFacet::TMP0);

  const MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;

  for (std::vector<unsigned long>::const_iterator pF = raclFacetIndices.begin(); pF != raclFacetIndices.end(); pF++)
  {
    for (int i = 0; i < 3; i++)
    {
      const MeshFacet &rclFacet = rclFAry[*pF];
      unsigned long      ulNB     = rclFacet._aulNeighbours[i];
      if (ulNB == ULONG_MAX)
      {
        raclResultPointsIndices.insert(rclFacet._aulPoints[i]);
        raclResultPointsIndices.insert(rclFacet._aulPoints[(i+1)%3]);
        continue;
      }
      if (rclFAry[ulNB].IsFlag(MeshFacet::TMP0) == false)
      {
        raclResultPointsIndices.insert(rclFacet._aulPoints[i]);
        raclResultPointsIndices.insert(rclFacet._aulPoints[(i+1)%3]);
        continue;
      }
    }
  }
}

bool MeshAlgorithm::NearestPointFromPoint (const Base::Vector3f &rclPt, unsigned long &rclResFacetIndex, Base::Vector3f &rclResPoint) const
{
  if (_rclMesh.CountFacets() == 0)
    return false;

  // calc each facet
  float fMinDist = FLOAT_MAX;
  unsigned long ulInd   = ULONG_MAX;
  MeshFacetIterator pF(_rclMesh);
  for (pF.Init(); pF.More(); pF.Next())
  {
    float fDist = pF->DistanceToPoint(rclPt);
    if (fDist < fMinDist)
    {
      fMinDist = fDist;
      ulInd    = pF.Position();
    }
  }

  MeshGeomFacet rclSFacet = _rclMesh.GetFacet(ulInd);
  rclSFacet.DistanceToPoint(rclPt, rclResPoint);
  rclResFacetIndex = ulInd;

  return true;
}

bool MeshAlgorithm::NearestPointFromPoint (const Base::Vector3f &rclPt, const MeshFacetGrid& rclGrid, unsigned long &rclResFacetIndex, Base::Vector3f &rclResPoint) const
{
  unsigned long ulInd = rclGrid.SearchNearestFromPoint(rclPt);

  if (ulInd == ULONG_MAX)
  {
    return false;
  }

  MeshGeomFacet rclSFacet = _rclMesh.GetFacet(ulInd);
  rclSFacet.DistanceToPoint(rclPt, rclResPoint);
  rclResFacetIndex = ulInd;

  return true;
}

bool MeshAlgorithm::NearestPointFromPoint (const Base::Vector3f &rclPt, const MeshFacetGrid& rclGrid, float fMaxSearchArea,
                                           unsigned long &rclResFacetIndex, Base::Vector3f &rclResPoint) const
{
  unsigned long ulInd = rclGrid.SearchNearestFromPoint(rclPt, fMaxSearchArea);

  if (ulInd == ULONG_MAX)
    return false;  // no facets inside BoundingBox

  MeshGeomFacet rclSFacet = _rclMesh.GetFacet(ulInd);
  rclSFacet.DistanceToPoint(rclPt, rclResPoint);
  rclResFacetIndex = ulInd;

  return true;
}

bool MeshAlgorithm::CutWithPlane (const Wm3::Plane3<float> &rclPlane, const MeshFacetGrid &rclGrid,
                                  std::list<std::vector<Base::Vector3f> > &rclResult, float fMinEps) const
{
  Base::Vector3f  clBase, clNormal; // Schnittebene

  Wm3::Vector3<float> clPos = rclPlane.Normal * rclPlane.Constant;
  Wm3::Vector3<float> clDir = rclPlane.Normal;
  clBase.Set(clPos.X(), clPos.Y(), clPos.Z());
  clNormal.Set(clDir.X(), clDir.Y(), clDir.Z());

  return CutWithPlane (clBase, clNormal, rclGrid, rclResult, fMinEps, false);
}

bool MeshAlgorithm::CutWithPlane (const Base::Vector3f &clBase, const Base::Vector3f &clNormal, const MeshFacetGrid &rclGrid,
                                  std::list<std::vector<Base::Vector3f> > &rclResult, float fMinEps, bool bConnectPolygons) const
{
  std::vector<unsigned long>  aulFacets;  

  // Grid durschsuchen
  MeshGridIterator clGridIter(rclGrid);
  for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
  {  
    // wenn Gridvoxel Ebene schneidet: alle Facets aufnehmen zum Schneiden
    if (clGridIter.GetBoundBox().IsCutPlane(clBase, clNormal) == true)
      clGridIter.GetElements(aulFacets);
  }

  // mehrfach vorhanden Dreiecke entfernen
  std::sort(aulFacets.begin(), aulFacets.end());
  aulFacets.erase(std::unique(aulFacets.begin(), aulFacets.end()), aulFacets.end());  

  // alle Facets mit Ebene schneiden
  std::list<std::pair<Base::Vector3f, Base::Vector3f> > clTempPoly;  // Feld mit Schnittlinien (unsortiert, nicht verkettet)

  for (std::vector<unsigned long>::iterator pF = aulFacets.begin(); pF != aulFacets.end(); pF++)
  {
    Base::Vector3f  clE1, clE2;
    const MeshGeomFacet clF(_rclMesh.GetFacet(*pF));

    // Facet schneiden und Schnittstrecke ablegen
    if (clF.IntersectWithPlane(clBase, clNormal, clE1, clE2) == true)
      clTempPoly.push_back(std::pair<Base::Vector3f, Base::Vector3f>(clE1, clE2));
  }

  if(bConnectPolygons)
  {
      //std::list<std::pair<Base::Vector3f, Base::Vector3f> > rclTempLines;
      std::list<std::pair<Base::Vector3f, Base::Vector3f> > rclResultLines(clTempPoly.begin(),clTempPoly.end());
      std::list<std::vector<Base::Vector3f> > tempList;
      ConnectLines(clTempPoly, tempList, fMinEps);
      ConnectPolygons(tempList, clTempPoly);

      for(std::list<std::pair<Base::Vector3f, Base::Vector3f> >::iterator iter = clTempPoly.begin(); iter != clTempPoly.end(); iter++)
      {
        rclResultLines.push_front(*iter);
      }

      return ConnectLines(rclResultLines, rclResult, fMinEps);
  }

  return ConnectLines(clTempPoly, rclResult, fMinEps);
}

bool MeshAlgorithm::ConnectLines (std::list<std::pair<Base::Vector3f, Base::Vector3f> > &rclLines,
                                  std::list<std::vector<Base::Vector3f> > &rclPolylines, float fMinEps) const
{
  typedef std::list<std::pair<Base::Vector3f, Base::Vector3f> >::iterator  TCIter;

  // square search radius
  // const float fMinEps = 1.0e-2f; // := 10 mirometer distance
  fMinEps = fMinEps * fMinEps;  

  // remove all lines which distance is smaller than epsilon
  std::list<TCIter> _clToDelete;
  float fToDelDist = fMinEps / 10.0f;
  for (TCIter pF = rclLines.begin(); pF != rclLines.end(); pF++)
  {
    if (Base::DistanceP2(pF->first, pF->second) < fToDelDist)
      _clToDelete.push_back(pF);
  }
  for (std::list<TCIter>::iterator pI = _clToDelete.begin(); pI != _clToDelete.end(); pI++)
    rclLines.erase(*pI);


  while (rclLines.size() > 0)
  {
    TCIter pF;
    
    // new polyline
    std::list<Base::Vector3f> clPoly;

    // add first line and delete from the list
    Base::Vector3f clFront = rclLines.begin()->first;  // current start point of the polyline
    Base::Vector3f clEnd   = rclLines.begin()->second; // current end point of the polyline
    clPoly.push_back(clFront);
    clPoly.push_back(clEnd);
    rclLines.erase(rclLines.begin());

    // search for the next line on the begin/end of the polyline and add it
    TCIter pFront, pEnd;
    do
    {
      float  fFrontMin = fMinEps, fEndMin = fMinEps;
      bool   bFrontFirst=false, bEndFirst=false;

      pFront = rclLines.end();
      pEnd   = rclLines.end();
      for (pF = rclLines.begin(); pF != rclLines.end(); pF++)
      {
        if (Base::DistanceP2(clFront, pF->first) < fFrontMin) 
        {
          fFrontMin   = Base::DistanceP2(clFront, pF->first);
          pFront      = pF;
          bFrontFirst = true;
        }
        else if (Base::DistanceP2(clEnd, pF->first) < fEndMin)
        {
          fEndMin     = Base::DistanceP2(clEnd, pF->first);
          pEnd        = pF;
          bEndFirst   = true;
        }
        else if (Base::DistanceP2(clFront, pF->second) < fFrontMin)
        {
          fFrontMin   = Base::DistanceP2(clFront, pF->second);
          pFront      = pF;
          bFrontFirst = false;
        }
        else if (Base::DistanceP2(clEnd, pF->second) < fEndMin)
        {
          fEndMin     = Base::DistanceP2(clEnd, pF->second);
          pEnd        = pF;
          bEndFirst   = false;
        }        
      }

      if (pFront != rclLines.end())
      {
        if (bFrontFirst == true)
        {
          clPoly.push_front(pFront->second);
          clFront = pFront->second;
        }
        else
        {
          clPoly.push_front(pFront->first);
          clFront = pFront->first;
        }
        rclLines.erase(pFront);
      }

      if (pEnd != rclLines.end())
      {
        if (bEndFirst == true)
        {
          clPoly.push_back(pEnd->second);
          clEnd = pEnd->second;
        }
        else
        {
          clPoly.push_back(pEnd->first);
          clEnd = pEnd->first;
        }
        rclLines.erase(pEnd);
      }
    }
    while ((pFront != rclLines.end()) || (pEnd != rclLines.end()));

    rclPolylines.push_back(std::vector<Base::Vector3f>(clPoly.begin(), clPoly.end()));
  }  
  
  // remove all polylines with too few length
  typedef std::list<std::vector<Base::Vector3f> >::iterator TPIter;
  std::list<TPIter> _clPolyToDelete;
  for (TPIter pJ = rclPolylines.begin(); pJ != rclPolylines.end(); pJ++)
  {
    if (pJ->size() == 2) // only one line segment
    {
      if (Base::DistanceP2(*pJ->begin(), *(pJ->begin() + 1)) <= fMinEps)
        _clPolyToDelete.push_back(pJ);
    }
  }
  for (std::list<TPIter>::iterator pK = _clPolyToDelete.begin(); pK != _clPolyToDelete.end(); pK++)
    rclPolylines.erase(*pK);

  return true;
}

bool MeshAlgorithm::ConnectPolygons(std::list<std::vector<Base::Vector3f> > &clPolyList,
                                    std::list<std::pair<Base::Vector3f, Base::Vector3f> > &rclLines) const
{

  for(std::list< std::vector<Base::Vector3f> >::iterator OutIter = clPolyList.begin(); OutIter != clPolyList.end() ; OutIter++)
  {
    std::pair<Base::Vector3f,Base::Vector3f> currentSort;
    float fDist = Base::Distance(OutIter->front(),OutIter->back());
    currentSort.first = OutIter->front();
    currentSort.second = OutIter->back();

    for(std::list< std::vector<Base::Vector3f> >::iterator InnerIter = clPolyList.begin(); InnerIter != clPolyList.end(); InnerIter++)
    {
      if(OutIter == InnerIter) continue;

      if(Base::Distance(OutIter->front(),InnerIter->front()) < fDist)
      {
        currentSort.second = InnerIter->front();
        fDist = Base::Distance(OutIter->front(),InnerIter->front());
      }
      if(Base::Distance(OutIter->front(),InnerIter->back()) < fDist)
      {
        currentSort.second = InnerIter->back();
        fDist = Base::Distance(OutIter->front(),InnerIter->back());
      }
    }

    rclLines.push_front(currentSort);

  }

  return true;
}

void MeshAlgorithm::GetFacetsFromPlane (const MeshFacetGrid &rclGrid, const Wm3::Plane3<float>& clPlane, const Base::Vector3f &rclLeft,
                                        const Base::Vector3f &rclRight, std::vector<unsigned long> &rclRes) const
{
  std::vector<unsigned long> aulFacets;
 
  Wm3::Vector3<float> clPos = clPlane.Normal * clPlane.Constant;
  Wm3::Vector3<float> clDir = clPlane.Normal;

  Base::Vector3f clBase(float(clPos.X()), float(clPos.Y()), float(clPos.Z()));
  Base::Vector3f clNormal(float(clDir.X()), float(clDir.Y()), float(clDir.Z()));
  clNormal.Normalize();

  Base::Vector3f clPtNormal(rclLeft - rclRight);
  clPtNormal.Normalize();

  // search grid 
  MeshGridIterator clGridIter(rclGrid);
  for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
  {  
    // add facets from grid if the plane if cut the grid-voxel
    if (clGridIter.GetBoundBox().IsCutPlane(clBase, clNormal) == true)
      clGridIter.GetElements(aulFacets);
  }

  // testing facet against planes
  for (std::vector<unsigned long>::iterator pI = aulFacets.begin(); pI != aulFacets.end(); pI++)
  {
    MeshGeomFacet clSFacet = _rclMesh.GetFacet(*pI);
    if (clSFacet.IntersectWithPlane(clBase, clNormal) == true)
    {
      bool bInner = false;
      for (int i = 0; (i < 3) && (bInner == false); i++)
      {
        Base::Vector3f clPt = clSFacet._aclPoints[i];
        if ((clPt.DistanceToPlane(rclLeft, clPtNormal) <= 0.0f) && (clPt.DistanceToPlane(rclRight, clPtNormal) >= 0.0f))
          bInner = true;
      }

      if (bInner == true)
        rclRes.push_back(*pI);
    }
  }
}

void MeshAlgorithm::PointsFromFacetsIndices (const std::vector<unsigned long> &rvecIndices, std::vector<Base::Vector3f> &rvecPoints) const
{
  const MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;
  const MeshPointArray &rclPAry = _rclMesh._aclPointArray;

  std::set<unsigned long> setPoints;

  for (std::vector<unsigned long>::const_iterator itI = rvecIndices.begin(); itI != rvecIndices.end(); itI++)
  {
    for (int i = 0; i < 3; i++)
      setPoints.insert(rclFAry[*itI]._aulPoints[i]);
  }

  rvecPoints.clear();
  for (std::set<unsigned long>::iterator itP = setPoints.begin(); itP != setPoints.end(); itP++)
    rvecPoints.push_back(rclPAry[*itP]);
}

bool MeshAlgorithm::Distance (const Base::Vector3f &rclPt, unsigned long ulFacetIdx, float fMaxDistance, float &rfDistance) const
{
  const MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;
  const MeshPointArray &rclPAry = _rclMesh._aclPointArray;
  const unsigned long *pulIdx = rclFAry[ulFacetIdx]._aulPoints;

  BoundBox3f clBB;
  clBB &= rclPAry[*(pulIdx++)];
  clBB &= rclPAry[*(pulIdx++)];
  clBB &= rclPAry[*pulIdx];
  clBB.Enlarge(fMaxDistance);

  if (clBB.IsInBox(rclPt) == false)
    return false;

  rfDistance = _rclMesh.GetFacet(ulFacetIdx).DistanceToPoint(rclPt);

  return rfDistance < fMaxDistance;
}

// ----------------------------------------------------

void MeshRefPointToFacets::Rebuild (void)
{
  clear();

  const MeshPointArray& rPoints = _rclMesh.GetPoints();
  resize(rPoints.size());

  const MeshFacetArray& rFacets = _rclMesh.GetFacets();
  for (MeshFacetArray::_TConstIterator pFIter = rFacets.begin(); pFIter != rFacets.end(); pFIter++)
  {
    operator[](pFIter->_aulPoints[0]).insert(pFIter);
    operator[](pFIter->_aulPoints[1]).insert(pFIter);
    operator[](pFIter->_aulPoints[2]).insert(pFIter);
  }
}

// ermittelt alle Nachbarn zum Facet deren Schwerpunkt unterhalb der mpx. Distanz befindet. 
// Facet deren VISIT-Flag gesetzt ist werden nicht beruecksichtig. 
/// @todo
void MeshRefPointToFacets::Neighbours (unsigned long ulFacetInd, float fMpxDist, std::vector<MeshFacetArray::_TConstIterator> &rclNb)
{
  rclNb.clear();
  Base::Vector3f  clCenter = _rclMesh.GetFacet(ulFacetInd).GetGravityPoint();

  const MeshFacetArray& rFacets = _rclMesh.GetFacets();
  SearchNeighbours(rFacets.begin() + ulFacetInd, clCenter, fMpxDist * fMpxDist, rclNb);

  for (std::vector<MeshFacetArray::_TConstIterator>::iterator i = rclNb.begin(); i != rclNb.end(); i++)
    (*i)->ResetFlag(MeshFacet::VISIT);  
}

/// @todo
void MeshRefPointToFacets::SearchNeighbours(MeshFacetArray::_TConstIterator pFIter, const Base::Vector3f &rclCenter, float fMpxDist, std::vector<MeshFacetArray::_TConstIterator> &rclNb)
{
  if (pFIter->IsFlag(MeshFacet::VISIT) == true)
    return;

  if (Base::DistanceP2(rclCenter, _rclMesh.GetFacet(*pFIter).GetGravityPoint()) > fMpxDist)
    return;

  rclNb.push_back(pFIter);
  pFIter->SetFlag(MeshFacet::VISIT);

  const MeshPointArray& rPoints = _rclMesh.GetPoints();
  MeshPointArray::_TConstIterator pPBegin = rPoints.begin();
   
  for (int i = 0; i < 3; i++)
  {
    MeshPointArray::_TConstIterator pPIter = rPoints.begin() + pFIter->_aulPoints[i];
    const std::set<MeshFacetArray::_TConstIterator> &rclFacets = operator[](pPIter - pPBegin);

    for (std::set<MeshFacetArray::_TConstIterator>::const_iterator j = rclFacets.begin(); j != rclFacets.end(); j++)
    {
      SearchNeighbours(*j, rclCenter, fMpxDist, rclNb);
    }
  }
}

//----------------------------------------------------------------------------

void MeshRefFacetToFacets::Rebuild (void)
{
  const MeshFacetArray& rFacets = _rclMesh.GetFacets();
  MeshFacetArray::_TConstIterator pFBegin = rFacets.begin();
  std::set<MeshFacetArray::_TConstIterator> clEmptySet;

  clear();

  MeshRefPointToFacets  clRPF(_rclMesh);
  
  for (MeshFacetArray::_TConstIterator pFIter = pFBegin; pFIter != rFacets.end(); pFIter++)
  {
    for (int i = 0; i < 3; i++)
    {
      const std::set<MeshFacetArray::_TConstIterator> &rclNBSet = clRPF[pFIter->_aulPoints[i]];
      operator[](pFIter - pFBegin).insert(rclNBSet.begin(), rclNBSet.end());
    }
  }
}

//----------------------------------------------------------------------------

void MeshRefPointToPoints::Rebuild (void)
{
  clear();

  const MeshPointArray& rPoints = _rclMesh.GetPoints();
  resize(rPoints.size());

  const MeshFacetArray& rFacets = _rclMesh.GetFacets();
  MeshPointArray::_TConstIterator  pBegin = rPoints.begin();
  for (MeshFacetArray::_TConstIterator pFIter = rFacets.begin(); pFIter != rFacets.end(); pFIter++)
  {
    unsigned long ulP0 = pFIter->_aulPoints[0];
    unsigned long ulP1 = pFIter->_aulPoints[1];
    unsigned long ulP2 = pFIter->_aulPoints[2];

    operator[](ulP0).insert(pBegin + ulP1);
    operator[](ulP0).insert(pBegin + ulP2);
    operator[](ulP1).insert(pBegin + ulP0);
    operator[](ulP1).insert(pBegin + ulP2);
    operator[](ulP2).insert(pBegin + ulP0);
    operator[](ulP2).insert(pBegin + ulP1);
  }
}

//----------------------------------------------------------------------------

float MeshPolygonTriangulation::Triangulate::Area(const std::vector<Base::Vector3f> &contour)
{
  int n = contour.size();

  float A=0.0f;

  for(int p=n-1,q=0; q<n; p=q++)
  {
    A+= contour[p].x*contour[q].y - contour[q].x*contour[p].y;
  }
  return A*0.5f;
}

/*
  InsideTriangle decides if a point P is Inside of the triangle
  defined by A, B, C.
*/
bool MeshPolygonTriangulation::Triangulate::InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py)
{
  float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
  float cCROSSap, bCROSScp, aCROSSbp;

  ax = Cx - Bx;  ay = Cy - By;
  bx = Ax - Cx;  by = Ay - Cy;
  cx = Bx - Ax;  cy = By - Ay;
  apx= Px - Ax;  apy= Py - Ay;
  bpx= Px - Bx;  bpy= Py - By;
  cpx= Px - Cx;  cpy= Py - Cy;

  aCROSSbp = ax*bpy - ay*bpx;
  cCROSSap = cx*apy - cy*apx;
  bCROSScp = bx*cpy - by*cpx;

  return ((aCROSSbp >= FLOAT_EPS) && (bCROSScp >= FLOAT_EPS) && (cCROSSap >= FLOAT_EPS));
}

bool MeshPolygonTriangulation::Triangulate::Snip(const std::vector<Base::Vector3f> &contour,int u,int v,int w,int n,int *V)
{
  int p;
  float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

  Ax = contour[V[u]].x;
  Ay = contour[V[u]].y;

  Bx = contour[V[v]].x;
  By = contour[V[v]].y;

  Cx = contour[V[w]].x;
  Cy = contour[V[w]].y;

  if ( FLOAT_EPS > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

  for (p=0;p<n;p++)
  {
    if( (p == u) || (p == v) || (p == w) ) continue;
    Px = contour[V[p]].x;
    Py = contour[V[p]].y;
    if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
  }

  return true;
}

bool MeshPolygonTriangulation::Triangulate::_invert = false;

bool MeshPolygonTriangulation::Triangulate::Process(const std::vector<Base::Vector3f> &contour, std::vector<unsigned long> &result)
{
  /* allocate and initialize list of Vertices in polygon */

  int n = contour.size();
  if ( n < 3 ) return false;

  int *V = new int[n];

  /* we want a counter-clockwise polygon in V */

  if ( 0.0f < Area(contour) )
  {
    for (int v=0; v<n; v++) V[v] = v;
    _invert = true;
  }
//    for(int v=0; v<n; v++) V[v] = (n-1)-v;
  else
  {
    for(int v=0; v<n; v++) V[v] = (n-1)-v;
    _invert = false;
  }

  int nv = n;

  /*  remove nv-2 Vertices, creating 1 triangle every time */
  int count = 2*nv;   /* error detection */

  for(int m=0, v=nv-1; nv>2; )
  {
    /* if we loop, it is probably a non-simple polygon */
    if (0 >= (count--))
    {
      //** Triangulate: ERROR - probable bad polygon!
      return false;
    }

    /* three consecutive vertices in current polygon, <u,v,w> */
    int u = v  ; if (nv <= u) u = 0;     /* previous */
    v = u+1; if (nv <= v) v = 0;     /* new v    */
    int w = v+1; if (nv <= w) w = 0;     /* next     */

    if ( Snip(contour,u,v,w,nv,V) )
    {
      int a,b,c,s,t;

      /* true names of the vertices */
      a = V[u]; b = V[v]; c = V[w];

      /* output Triangle */
      result.push_back( a );
      result.push_back( b );
      result.push_back( c );

      m++;

      /* remove v from remaining polygon */
      for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

      /* resest error detection counter */
      count = 2*nv;
    }
  }

  delete V;

  return true;
}

MeshPolygonTriangulation::MeshPolygonTriangulation()
{
}

MeshPolygonTriangulation::MeshPolygonTriangulation(const std::vector<Base::Vector3f>& raclPoints)
	: _aclPoints(raclPoints)
{
}

MeshPolygonTriangulation::~MeshPolygonTriangulation()
{
}

bool MeshPolygonTriangulation::compute()
{
	_aclFacets.clear();
  _aclTopology.clear();
/*
  float sxx,sxy,sxz,syy,syz,szz,mx,my,mz;
  sxx=sxy=sxz=syy=syz=szz=mx=my=mz=0.0f;

  for ( std::vector<Base::Vector3f>::iterator it = _aclPoints.begin(); it!=_aclPoints.end(); ++it)
  {
    sxx += it->x * it->x; sxy += it->x * it->y;
    sxz += it->x * it->z; syy += it->y * it->y;
    syz += it->y * it->z; szz += it->z * it->z;
    mx += it->x; my += it->y; mz += it->z;
  }

  unsigned nSize = _aclPoints.size();
  sxx = sxx - mx*mx/((float)nSize);
  sxy = sxy - mx*my/((float)nSize);
  sxz = sxz - mx*mz/((float)nSize);
  syy = syy - my*my/((float)nSize);
  syz = syz - my*mz/((float)nSize);
  szz = szz - mz*mz/((float)nSize);

  // Kovarianzmatrix
  Wm3::Matrix3<float> akMat(sxx,sxy,sxz,sxy,syy,syz,sxz,syz,szz);

  Wm3::Matrix3<float> rkRot, rkDiag;
  akMat.EigenDecomposition(rkRot, rkDiag);

  Wm3::Vector3<float> U = rkRot.GetColumn(0);
  Wm3::Vector3<float> V = rkRot.GetColumn(1);
  Wm3::Vector3<float> W = rkRot.GetColumn(2);

  Matrix4D clTMat;
  clTMat[0][0] = U.X(); clTMat[0][1] = U.Y(); clTMat[0][2] = U.Z(); clTMat[0][3] = mx/(float)nSize;
  clTMat[1][0] = V.X(); clTMat[1][1] = V.Y(); clTMat[1][2] = V.Z(); clTMat[1][3] = my/(float)nSize;
  clTMat[2][0] = W.X(); clTMat[2][1] = W.Y(); clTMat[2][2] = W.Z(); clTMat[2][3] = mz/(float)nSize;
  clTMat[3][0] = 0.0f;  clTMat[3][1] = 0.0f;  clTMat[3][2] = 0.0f;  clTMat[3][3] = 1.0f;
*/
  std::vector<Base::Vector3f> pts;
  std::vector<unsigned long> result;
	for ( std::vector<Base::Vector3f>::iterator it = _aclPoints.begin(); it != _aclPoints.end(); ++it)
	{
		pts.push_back(/*clTMat **/ (*it));
	}

  //  Invoke the triangulator to triangulate this polygon.
  Triangulate::Process(pts,result);

  // print out the results.
  unsigned long tcount = result.size()/3;

  bool ok = tcount+2 == _aclPoints.size();
  if  ( tcount > _aclPoints.size() )
		return false; // no valid triangulation

	MeshGeomFacet clFacet;
	MeshFacet clTopFacet;
  for (unsigned long i=0; i<tcount; i++)
  {
    if ( Triangulate::_invert )
    {
      clFacet._aclPoints[0] = _aclPoints[result[i*3+0]];
      clFacet._aclPoints[2] = _aclPoints[result[i*3+1]];
      clFacet._aclPoints[1] = _aclPoints[result[i*3+2]];
      clTopFacet._aulPoints[0] = result[i*3+0];
      clTopFacet._aulPoints[2] = result[i*3+1];
      clTopFacet._aulPoints[1] = result[i*3+2];
    }
    else
    {
      clFacet._aclPoints[0] = _aclPoints[result[i*3+0]];
      clFacet._aclPoints[1] = _aclPoints[result[i*3+1]];
      clFacet._aclPoints[2] = _aclPoints[result[i*3+2]];
      clTopFacet._aulPoints[0] = result[i*3+0];
      clTopFacet._aulPoints[1] = result[i*3+1];
      clTopFacet._aulPoints[2] = result[i*3+2];
    }

		_aclFacets.push_back(clFacet);
		_aclTopology.push_back(clTopFacet);
  }

	return ok;
}

void MeshPolygonTriangulation::setPolygon(const std::vector<Base::Vector3f>& raclPoints)
{
	_aclPoints = raclPoints;
	if (_aclPoints.size() > 0)
	{
		if (_aclPoints.front() == _aclPoints.back())
			_aclPoints.pop_back();
	}
}
