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
# include <Wm3IntrSegment3Plane3.h>
# include <Wm3Matrix3.h>
# include <Wm3Vector3.h>
# include <algorithm>
# include <vector>
#endif

#include "Evaluation.h"
#include "Iterator.h"
#include "Algorithm.h"
#include "Visitor.h"
#include "MeshIO.h"
#include "Helpers.h"
#include "Grid.h"
#include "TopoAlgorithm.h"
#include <Base/Matrix.h>

#include <Base/Sequencer.h>

using namespace MeshCore;


/**
 * This class searches for inconsistent orientation of neighboured facets. 
 */
class MeshWrongOrientation : public MeshFacetVisitor
{
public:
  MeshWrongOrientation(std::vector<unsigned long>& aulVisited)
    : _raulVisited(aulVisited), _wrongOrientation(false)
  {
  }

  bool Visit (const MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long ulLevel)
  {
    _raulVisited.push_back(ulFInd);
    unsigned long i, j;
  
    // check the orientation of two neighboured facets
    for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
      {
        // sharing two same points?
        if (rclFrom._aulPoints[i] == rclFacet._aulPoints[j])
        { 
          if ((rclFrom._aulPoints[(i+1)%3] == rclFacet._aulPoints[(j+1)%3]))
          {
            _wrongOrientation = true;
            return false;
          } 
        }
      }
    }

    return true;
  }

  bool HasWrongOrientatedFacet() const
  {
    return _wrongOrientation;
  }

private:
  std::vector<unsigned long>& _raulVisited;
  bool _wrongOrientation;
};

/**
 * This class searches for inconsistent orientation of neighboured facets. 
 * Note: The 'TMP0' flag for facets must be restted before using this class.
 */
class MeshWrongOrientationCollect : public MeshWrongOrientation
{
public:
  MeshWrongOrientationCollect(std::vector<unsigned long>& aulVisited, std::vector<unsigned long>& aulIndices)
    : MeshWrongOrientation(aulVisited), _aulIndices(aulIndices)
  {
  }

  bool Visit (const MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long ulLevel)
  {
    // different orientation of rclFacet and rclFrom
    if ( !MeshWrongOrientation::Visit(rclFacet, rclFrom, ulFInd, ulLevel) )
    {
      // is not marked as false oriented
      if ( !rclFrom.IsFlag(MeshFacet::TMP0) )
      {
        // mark this facet as false oriented
        rclFacet.SetFlag(MeshFacet::TMP0);
        _aulIndices.push_back( ulFInd );
      }
    }
    else
    {
      // same orientation but if the neighbour rclFrom is false oriented then rclFrom is also false oriented
      if ( rclFrom.IsFlag(MeshFacet::TMP0) )
      {
        // mark this facet as false oriented
        rclFacet.SetFlag(MeshFacet::TMP0);
        _aulIndices.push_back( ulFInd );
      }
    }

    return true;
  }

private:
  std::vector<unsigned long>& _aulIndices;
};

// ----------------------------------------------------

MeshEvalNormals::MeshEvalNormals (const MeshKernel& rclM)
  :MeshEvaluation( rclM )
{
}

MeshEvalNormals::~MeshEvalNormals()
{
}

bool MeshEvalNormals::Evaluate ()
{
  unsigned long ulStartFacet, ulVisited;

  if (_rclMesh.CountFacets() == 0)
    return true;

  // reset VISIT flags
  MeshAlgorithm(_rclMesh).ResetFacetFlag(MeshFacet::VISIT);

  ulVisited = 0;
  ulStartFacet = 0;

  // all facet indices
  std::vector<unsigned long> aAllFacets(_rclMesh.CountFacets());
  unsigned long k = 0;
  for (std::vector<unsigned long>::iterator pI = aAllFacets.begin(); pI != aAllFacets.end(); pI++)
    *pI = k++;

  // if the mesh consists of several topologic independent components
  while ( true ) // while unvisited facets are available
  {
    if ( aAllFacets.size() > 0 )
      ulStartFacet = aAllFacets.front();
    else
      return true; // no more facets to visit

    std::vector<unsigned long> aComponent;

    // get start facet first
    aComponent.push_back(ulStartFacet);
    MeshWrongOrientation clOrientation(aComponent);
    ulVisited += _rclMesh.VisitNeighbourFacets(clOrientation, ulStartFacet);

    // return immediately after having found one "wrong" facet
    if ( clOrientation.HasWrongOrientatedFacet() )
      return false;

    // search for all NOT YET visited facets
    std::vector<unsigned long> aNotVisited;
    std::sort(aAllFacets.begin(), aAllFacets.end());
    std::sort(aComponent.begin(), aComponent.end());
    std::back_insert_iterator<std::vector<unsigned long> >  pBInd(aNotVisited);
    std::set_difference(aAllFacets.begin(), aAllFacets.end(), aComponent.begin(), aComponent.end(), pBInd);
  
    aAllFacets = aNotVisited;
  }

  return true;
}

std::vector<unsigned long> MeshEvalNormals::GetIndices() const
{
  // reset VISIT flags
  MeshAlgorithm cAlg(_rclMesh);
  cAlg.ResetFacetFlag(MeshFacet::VISIT);
  cAlg.ResetFacetFlag(MeshFacet::TMP0);

  // all facet indices
  std::vector<unsigned long> aAllFacets(_rclMesh.CountFacets());
  unsigned long k = 0;
  for (std::vector<unsigned long>::iterator pI = aAllFacets.begin(); pI != aAllFacets.end(); pI++)
    *pI = k++;

  std::vector<unsigned long> inds;
  unsigned long ulStartFacet=0, ulVisited=0;

  // if the mesh consists of several topologic independent components
  while ( true ) // while unvisited facets are available
  {
    if ( aAllFacets.size() > 0 )
      ulStartFacet = aAllFacets.front();
    else
      break; // no more facets to visit

    std::vector<unsigned long> aComponent;

    // get start facet first
    aComponent.push_back(ulStartFacet);
    MeshWrongOrientationCollect clOrientation(aComponent, inds);
    ulVisited += _rclMesh.VisitNeighbourFacets(clOrientation, ulStartFacet);

    // search for all NOT YET visited facets
    std::vector<unsigned long> aNotVisited;
    std::sort(aAllFacets.begin(), aAllFacets.end());
    std::sort(aComponent.begin(), aComponent.end());
    std::back_insert_iterator<std::vector<unsigned long> >  pBInd(aNotVisited);
    std::set_difference(aAllFacets.begin(), aAllFacets.end(), aComponent.begin(), aComponent.end(), pBInd);
  
    aAllFacets = aNotVisited;
  }

  // As we cannot determine for sure whether the start facet has correct orientation
  unsigned long diff = _rclMesh.CountFacets()-inds.size();
  if ( inds.size() > diff )
  {
    // free memory
    { inds.clear(); std::vector<unsigned long>().swap(inds);}
    inds.reserve( diff );

    const MeshFacetArray& rclFAry = _rclMesh.GetFacets();
    for (MeshFacetArray::_TConstIterator pI = rclFAry.begin(); pI != rclFAry.end(); pI++)
    {
      if ( !pI->IsFlag(MeshFacet::TMP0) )
        inds.push_back( pI-rclFAry.begin() );
    }
  }

  return inds;
}

MeshFixNormals::MeshFixNormals (MeshKernel& rclM)
  :MeshValidation( rclM )
{
}

MeshFixNormals::~MeshFixNormals()
{
}

bool MeshFixNormals::Fixup ()
{
  MeshTopoAlgorithm(_rclMesh).HarmonizeNormals();
  return true;
}

// ----------------------------------------------------

MeshEvalSolid::MeshEvalSolid (const MeshKernel& rclM)
  :MeshEvaluation( rclM )
{
}

MeshEvalSolid::~MeshEvalSolid()
{
}

bool MeshEvalSolid::Evaluate ()
{
  std::vector<MeshGeomEdge> edges;
  _rclMesh.GetEdges( edges );
  for (std::vector<MeshGeomEdge>::iterator it = edges.begin(); it != edges.end(); it++)
  {
    if (it->_bBorder)
      return false;
  }

  return true;
}

// ----------------------------------------------------

bool MeshEvalTopology::Evaluate ()
{
  const MeshFacetArray& rclFAry = _rclMesh.GetFacets();
  MeshFacetArray::_TConstIterator pI;

  _aclManifoldList.clear();

  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> > aclHits;
  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >::iterator pEdge;

  // facet to edge
  for (pI = rclFAry.begin(); pI != rclFAry.end(); pI++)
  {
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulPt0 = std::min<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      aclHits[std::pair<unsigned long, unsigned long>(ulPt0, ulPt1)].push_front(pI - rclFAry.begin());
    }
  }

  // search for non-manifold edges
  for (pEdge = aclHits.begin(); pEdge != aclHits.end(); pEdge++)
  {
    // Edge that is shared by more than 2 facets
    if (pEdge->second.size() > 2)
    {  // found non-manifold edge, add to list
      _aclManifoldList.push_back( std::make_pair<unsigned long, unsigned long>(pEdge->first.first, pEdge->first.second) );
    }
  }

  if (_aclManifoldList.size() > 0)
    return false;
  else
    return true;
}

// generate indexed edge list which tangents non-manifolds
void MeshEvalTopology::GetFacetManifolds (std::vector<unsigned long> &raclFacetIndList) const
{
  raclFacetIndList.clear();
  const MeshFacetArray& rclFAry = _rclMesh.GetFacets();
  MeshFacetArray::_TConstIterator pI;

  for (pI = rclFAry.begin(); pI != rclFAry.end(); pI++)
  {
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulPt0 = std::min<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      std::pair<unsigned long, unsigned long> edge = std::make_pair<unsigned long, unsigned long>(ulPt0, ulPt1);

      if ( std::find(_aclManifoldList.begin(), _aclManifoldList.end(), edge) != _aclManifoldList.end() )
        raclFacetIndList.push_back( pI - rclFAry.begin() );
    }
  }
}

unsigned long MeshEvalTopology::CountManifolds() const
{
  return _aclManifoldList.size();
}

// ---------------------------------------------------------

bool MeshEvalSingleFacet::Evaluate ()
{
  // get all non-manifolds
  MeshEvalTopology::Evaluate();
/*
  // for each (multiple) single linked facet there should
  // exist two valid facets sharing the same edge 
  // so make facet 1 neighbour of facet 2 and vice versa
  const std::vector<MeshFacet>& rclFAry = _rclMesh.GetFacets();
  std::vector<MeshFacet>::const_iterator pI;

  std::vector<std::list<unsigned long> > aclMf = _aclManifoldList;
  _aclManifoldList.clear();

  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> > aclHits;
  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >::iterator pEdge;

  // search for single links (a non-manifold edge and two open edges)
  //
  //
  // build edge <=> facet map
  for (pI = rclFAry.begin(); pI != rclFAry.end(); pI++)
  {
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulPt0 = std::min<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      aclHits[std::pair<unsigned long, unsigned long>(ulPt0, ulPt1)].push_front(pI - rclFAry.begin());
    }
  }

  // now search for single links
  for (std::vector<std::list<unsigned long> >::const_iterator pMF = aclMf.begin(); pMF != aclMf.end(); pMF++)
  {
    std::list<unsigned long> aulManifolds;
    for (std::list<unsigned long>::const_iterator pF = pMF->begin(); pF != pMF->end(); ++pF)
    {
      const MeshFacet& rclF = rclFAry[*pF];

      unsigned long ulCtNeighbours=0;
      for (int i = 0; i < 3; i++)
      {
        unsigned long ulPt0 = std::min<unsigned long>(rclF._aulPoints[i],  rclF._aulPoints[(i+1)%3]);
        unsigned long ulPt1 = std::max<unsigned long>(rclF._aulPoints[i],  rclF._aulPoints[(i+1)%3]);
        std::pair<unsigned long, unsigned long> clEdge(ulPt0, ulPt1); 

        // number of facets sharing this edge
        ulCtNeighbours += aclHits[clEdge].size();
      }

      // single linked found
      if (ulCtNeighbours == pMF->size() + 2)
        aulManifolds.push_front(*pF);
    }

    if ( aulManifolds.size() > 0 )
      _aclManifoldList.push_back(aulManifolds);
  }
*/
  return (_aclManifoldList.size() == 0);
}

bool MeshFixSingleFacet::Fixup ()
{
  std::vector<unsigned long> aulInvalids;
//  MeshFacetArray& raFacets = _rclMesh._aclFacetArray;
  for ( std::vector<std::list<unsigned long> >::const_iterator it=_raclManifoldList.begin();it!=_raclManifoldList.end();++it )
  {
    unsigned long uFInd1, uFInd2;
    uFInd1 = uFInd2 = ULONG_MAX;
    for ( std::list<unsigned long>::const_iterator it2 = it->begin(); it2 != it->end(); ++it2 )
    {
      aulInvalids.push_back(*it2);
//      MeshFacet& rF = raFacets[*it2];
    }
  }
  
  _rclMesh.DeleteFacets(aulInvalids);
  return true;
}

// ----------------------------------------------------------------

bool MeshEvalSelfIntersection::Evaluate ()
{
  std::set<unsigned long> ulsSelfIntersectFacets;
/*
  //Contains bounding boxes for every facet 
  std::vector<BoundBox3D> cvBB;

  //Set of neighbour facet indicies for every facet
  std::vector<std::vector<unsigned long> > ulvFacetNeighbours;

  MeshFacetIterator cMFI(_rclMesh);

  //Splits the mesh using grid for speeding up the calculation
  MeshFacetGrid cMeshFacetGrid(_rclMesh);
  MeshGridIterator clGridIter(cMeshFacetGrid);
  unsigned long ulGridX, ulGridY, ulGridZ;
  cMeshFacetGrid.GetCtGrids(ulGridX, ulGridY, ulGridZ);

  MeshAlgorithm cMeshAlgo(_rclMesh);
  for(cMFI.Begin(); cMFI.More(); cMFI.Next())
  {
    cvBB.push_back((*cMFI).GetBoundBox());
    std::set<unsigned long> ulsNeighbourFacets;
    MeshFacet cFacet = cMFI.GetIndicies();
    for (int i=0; i<3; i++)
    {
      std::set<unsigned long>ulsNeighbourFacetsForPoint;
      cMeshBaseInterface.GetFacetNeighboursOverPoints(cFacet._aulPoints[i], 
                                                      ulsNeighbourFacetsForPoint);
      std::merge(ulsNeighbourFacets.begin(), ulsNeighbourFacets.end(),
            ulsNeighbourFacetsForPoint.begin(), ulsNeighbourFacetsForPoint.end(),
            std::inserter(ulsNeighbourFacets, ulsNeighbourFacets.begin()));
    }
    std::vector<unsigned long> ulvNeighbourFacets(ulsNeighbourFacets.begin(),
                                        ulsNeighbourFacets.end());
    ulvFacetNeighbours.push_back(ulvNeighbourFacets);
  }
  
  //Calculates the intersections
  Base::SequencerLauncher seq("calculating self intersections...", ulGridX*ulGridY*ulGridZ );
  for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
  {
    //Get the facet indicies, belonging to the current grid unit
    std::vector<unsigned long> aulGridElements;
    clGridIter.GetElements(aulGridElements);
    if (aulGridElements.size()==0)
    {
      Base::Sequencer().next();
      continue;
    }
    std::vector<unsigned long>::iterator ExtIt;
    std::vector<unsigned long>::iterator IntIt;
    std::vector<unsigned long>::iterator NextExtIt;
    std::vector<unsigned long>::iterator ExtItBeforeEnd = aulGridElements.end();
    ExtItBeforeEnd--;
    //external loop
    for (ExtIt = aulGridElements.begin(); ExtIt != ExtItBeforeEnd; ExtIt++)
    {
      //Marks the neighbours to pass them during calculation
      cMeshAlgo.ResetFacetFlag(MeshFacet::TMP0);
      cMeshAlgo.SetFacetsFlag(ulvFacetNeighbours[*ExtIt], MeshFacet::TMP0);
      MeshGeomFacet cCurrentFacet = _rclMesh.GetFacet(*ExtIt);;
      //internal loop
      NextExtIt = ExtIt;
      NextExtIt++;
      for (IntIt = NextExtIt; IntIt != aulGridElements.end(); IntIt++)
      {      
        //Tests BB of two facets for intersection
        BoundBox3D cBB3DExt(cvBB[*ExtIt]);
        BoundBox3D cBB3DInt(cvBB[*IntIt]);
        if ((cBB3DExt.MinX>cBB3DInt.MaxX)||
            (cBB3DInt.MinX>cBB3DExt.MaxX)||
            (cBB3DExt.MinY>cBB3DInt.MaxY)||
            (cBB3DInt.MinY>cBB3DExt.MaxY)||
            (cBB3DExt.MinZ>cBB3DInt.MaxZ)||
            (cBB3DInt.MinZ>cBB3DExt.MaxZ))
          continue;
  
        //Tests two facets for intersection
        MeshGeomFacet cNeighbourFacet = _rclMesh.GetFacet(*IntIt);
        if(cNeighbourFacet.IsFlag(MeshFacet::TMP0) == false)
        {
          //Tests two triangles for intersection (Magic Softwate Library)
          if ( MeshFacetFunc::IntersectWithFacet(cCurrentFacet, cNeighbourFacet) )
          {
            ulsSelfIntersectFacets.insert(*ExtIt);
            ulsSelfIntersectFacets.insert(*IntIt);
          }
        }
      }
    }
    Base::Sequencer().next();
  }

*/
  return ulsSelfIntersectFacets.size() == 0;
}

// ----------------------------------------------------------------

bool MeshEvalNeighbourhood::Evaluate ()
{
  const MeshFacetArray& rclFAry = _rclMesh.GetFacets();
  MeshFacetArray::_TConstIterator pI;

  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> > cEdgeList;
  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >::iterator pEdge;

  // facet to edge
  for (pI = rclFAry.begin(); pI != rclFAry.end(); pI++)
  {
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulPt0 = std::min<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      cEdgeList[std::pair<unsigned long, unsigned long>(ulPt0, ulPt1)].push_front(pI - rclFAry.begin());
    }
  }

  // search for the attached faces to an edge
  for (pEdge = cEdgeList.begin(); pEdge != cEdgeList.end(); ++pEdge)
  {
    // open edge
    if (pEdge->second.size() == 1) {
      unsigned long p0 = pEdge->first.first;
      unsigned long p1 = pEdge->first.second;
      const MeshFacet& rFace = rclFAry[pEdge->second.front()];
      unsigned short side = rFace.Side(p0,p1);
      // should be "open edge" but isn't marked as such
      if ( rFace._aulNeighbours[side] != ULONG_MAX )
        return false;
    }
    // two-manifolds
    else if (pEdge->second.size() == 2) {
      unsigned long p0 = pEdge->first.first;
      unsigned long p1 = pEdge->first.second;
      unsigned long f0 = pEdge->second.front();
      unsigned long f1 = pEdge->second.back();

      const MeshFacet& rFace0 = rclFAry[f0];
      const MeshFacet& rFace1 = rclFAry[f1];
      unsigned short side0 = rFace0.Side(p0,p1);
      unsigned short side1 = rFace1.Side(p0,p1);

      // Check wether rFace0 and rFace1 reference each other as neighbour
      if ( rFace0._aulNeighbours[side0]!=f1 || rFace1._aulNeighbours[side1]!=f0 )
        return false;
    }

    // Note: If more than two facets are attached to the edge then we have a non-manifold edge here. 
    // This means that the neighbourhood cannot be valid, fore sure. But we just want to check whether
    // the neighbourhood is valid for topologic correctly edges and thus we ignore this case.
    // Non-manifolds are an own category of errors and are handled by the class MeshEvalTopology.
  }

  return true;
}

bool MeshFixNeighbourhood::Fixup()
{
  _rclMesh.RebuildNeighbours();
  return true;
}

// ----------------------------------------------------------------

MeshEigensystem::MeshEigensystem (const MeshKernel &rclB)
 : MeshEvaluation(rclB), _cU(1.0f, 0.0f, 0.0f), _cV(0.0f, 1.0f, 0.0f), _cW(0.0f, 0.0f, 1.0f)
{
  // use the values of world coordinates as default
  Base::BoundBox3f box = _rclMesh.GetBoundBox();
  _fU = box.LengthX();
  _fV = box.LengthY();
  _fW = box.LengthZ();
}

Base::Matrix4D MeshEigensystem::Transform() const
{
  // x,y,c ... vectors
  // R,Q   ... matrices (R is orthonormal so its transposed(=inverse) is equal to Q)
  //
  // from local (x) to world (y,c) coordinates we have the equation
  // y = R * x  + c
  //     <==> 
  // x = Q * y - Q * c
  Base::Matrix4D clTMat;
  // rotation part
  clTMat[0][0] = _cU.x; clTMat[0][1] = _cU.y; clTMat[0][2] = _cU.z; clTMat[0][3] = 0.0f;
  clTMat[1][0] = _cV.x; clTMat[1][1] = _cV.y; clTMat[1][2] = _cV.z; clTMat[1][3] = 0.0f;
  clTMat[2][0] = _cW.x; clTMat[2][1] = _cW.y; clTMat[2][2] = _cW.z; clTMat[2][3] = 0.0f;
  clTMat[3][0] =  0.0f; clTMat[3][1] =  0.0f; clTMat[3][2] =  0.0f; clTMat[3][3] = 1.0f;

  Base::Vector3f c(_cC);
  c = clTMat * c;

  // translation part
  clTMat[0][3] = -c.x; clTMat[1][3] = -c.y; clTMat[2][3] = -c.z;

  return clTMat;
}

bool MeshEigensystem::Evaluate()
{
  CalculateLocalSystem();

  float xmin=0.0f, xmax=0.0f, ymin=0.0f, ymax=0.0f, zmin=0.0f, zmax=0.0f;

  Base::Vector3f clVect, clProj;
  float fH;

  const MeshPointArray& aclPoints = _rclMesh.GetPoints ();
  for (MeshPointArray::_TConstIterator it = aclPoints.begin(); it!=aclPoints.end(); ++it)
  {
    // u-Richtung
    clVect = *it - _cC;
    clProj.ProjToLine(clVect, _cU);
    clVect = clVect + clProj;
    fH = clVect.Length();
  
    // zeigen Vektoren in die gleiche Richtung ?
    if ( (clVect * _cU) < 0.0f)
      fH = -fH;

    xmax = std::max<float>(xmax, fH);
    xmin = std::min<float>(xmin, fH);

    // v-Richtung
    clVect = *it - _cC;
    clProj.ProjToLine(clVect, _cV);
    clVect = clVect + clProj;
    fH = clVect.Length();
  
    // zeigen Vektoren in die gleiche Richtung ?
    if ( (clVect * _cV) < 0.0f)
      fH = -fH;

    ymax = std::max<float>(ymax, fH);
    ymin = std::min<float>(ymin, fH);

    // w-Richtung
    clVect = *it - _cC;
    clProj.ProjToLine(clVect, _cW);
    clVect = clVect + clProj;
    fH = clVect.Length();
  
    // zeigen Vektoren in die gleiche Richtung ?
    if ( (clVect * _cW) < 0.0f)
      fH = -fH;

    zmax = std::max<float>(zmax, fH);
    zmin = std::min<float>(zmin, fH);
  }

  _fU = xmax - xmin;
  _fV = ymax - ymin;
  _fW = zmax - zmin;

  return false; // to call Fixup() if needed
}

Base::Vector3f MeshEigensystem::GetBoundings() const
{
  return Base::Vector3f ( _fU, _fV, _fW );
}

void MeshEigensystem::CalculateLocalSystem()
{
  // at least one facet is needed
  if ( _rclMesh.CountFacets() < 1 )
    return; // cannot continue calculation
#if 0
/*
  std::vector<MeshPoint>& aclPoints = _rclMesh.GetPoints ();
  std::vector<MeshPoint>::iterator it;

  float m00, m01, m02, m10, m11, m12, m20, m21, m22;
  m00 = m01 = m02 = 0.0f;
  m10 = m11 = m12 = 0.0f;
  m20 = m21 = m22 = 0.0f;

  float fSumArea=0.0f;
  Vector3f cGravity;

  // get the center of mass for the mesh
  MeshFacetIterator cIter( _rclMesh );
  for ( cIter.Init(); cIter.More(); cIter.Next() )
  {
    float fArea = MeshFacetFunc::Area( *cIter );
    fSumArea += fArea;
    cGravity += (fArea * cIter->GetGravityPoint());
  }

  cGravity = cGravity / fSumArea;

  for ( cIter.Init(); cIter.More(); cIter.Next() )
  {
    Vector3f d = ( cIter->GetGravityPoint() - cGravity );
    float fArea = MeshFacetFunc::Area( *cIter );
    m00 += fArea * d.x * d.x; m01 += fArea * d.x * d.y; m02 += fArea * d.x * d.z;
    m10 += fArea * d.y * d.x; m11 += fArea * d.y * d.y; m12 += fArea * d.y * d.z;
    m20 += fArea * d.z * d.x; m21 += fArea * d.z * d.y; m22 += fArea * d.z * d.z;
  }

  Matrix3<float> akMat(m00,m01,m02,m10,m11,m12,m20,m21,m22);

  unsigned nSize = 1;
  float mx = cGravity.x;
  float my = cGravity.y;
  float mz = cGravity.z;

#elif 1

  float m00, m01, m02, m10, m11, m12, m20, m21, m22;
  m00 = m01 = m02 = 0.0f;
  m10 = m11 = m12 = 0.0f;
  m20 = m21 = m22 = 0.0f;

  float fSumArea=0.0f;
  Vector3f cGravity;
  MeshRefPointToFacets cPt2Fac( _rclMesh );
  MeshPointIterator cP (_rclMesh);
  std::vector<MeshPoint>& aclPoints = _rclMesh.GetPoints ();
  std::vector<MeshPoint>::iterator it;

  unsigned long pos=0;
  for ( MeshRefPointToFacets::iterator it2 = cPt2Fac.begin(); it2 != cPt2Fac.end(); it2++ )
  {
    float fArea = 0.0f;
    const std::set<MeshFacetArray::_TIterator>& facs = *it2;
    for ( std::set<MeshFacetArray::_TIterator>::const_iterator cI = facs.begin(); cI != facs.end(); cI++ )
    {
      fArea += MeshFacetFunc::Area( MeshFacetFunc::ToGeomFacet(_rclMesh, **cI) );
    }

    cP.Set( pos++ );
    cGravity += fArea * (*cP);
  }

  MeshFacetIterator cIter(_rclMesh);
  for ( cIter.Init(); cIter.More(); cIter.Next() )
  {
    float fArea = MeshFacetFunc::Area( *cIter );
    fSumArea += fArea;
  }

  cGravity = cGravity / (3.0f*fSumArea);

  pos=0;
  for ( it2 = cPt2Fac.begin(); it2 != cPt2Fac.end(); it2++ )
  {
    cP.Set( pos++ );
    Vector3f d = ( *cP - cGravity );
    float fArea = 0.0f;
    const std::set<MeshFacetArray::_TIterator>& facs = *it2;
    for ( std::set<MeshFacetArray::_TIterator>::const_iterator cI = facs.begin(); cI != facs.end(); cI++ )
    {
      fArea += MeshFacetFunc::Area( MeshFacetFunc::ToGeomFacet(_rclMesh, **cI) );
    }
    m00 += fArea * d.x * d.x; m01 += fArea * d.x * d.y; m02 += fArea * d.x * d.z;
    m10 += fArea * d.y * d.x; m11 += fArea * d.y * d.y; m12 += fArea * d.y * d.z;
    m20 += fArea * d.z * d.x; m21 += fArea * d.z * d.y; m22 += fArea * d.z * d.z;
  }

  Matrix3<float> akMat(m00,m01,m02,m10,m11,m12,m20,m21,m22);

  unsigned nSize = 1;
  float mx = cGravity.x;
  float my = cGravity.y;
  float mz = cGravity.z;
*/
#else

  float sxx,sxy,sxz,syy,syz,szz,mx,my,mz;
  sxx=sxy=sxz=syy=syz=szz=mx=my=mz=0.0f;

  const MeshPointArray& aclPoints = _rclMesh.GetPoints ();
  MeshPointArray::_TConstIterator it;
  for ( it = aclPoints.begin(); it!=aclPoints.end(); ++it)
  {
    sxx += it->x * it->x; sxy += it->x * it->y;
    sxz += it->x * it->z; syy += it->y * it->y;
    syz += it->y * it->z; szz += it->z * it->z;
    mx += it->x; my += it->y; mz += it->z;
  }

  unsigned nSize = aclPoints.size();
  sxx = sxx - mx*mx/((float)nSize);
  sxy = sxy - mx*my/((float)nSize);
  sxz = sxz - mx*mz/((float)nSize);
  syy = syy - my*my/((float)nSize);
  syz = syz - my*mz/((float)nSize);
  szz = szz - mz*mz/((float)nSize);

  // Kovarianzmatrix
  Wm3::Matrix3<float> akMat(sxx,sxy,sxz,sxy,syy,syz,sxz,syz,szz);
#endif

  Wm3::Matrix3<float> rkRot, rkDiag;
  akMat.EigenDecomposition(rkRot, rkDiag);

  Wm3::Vector3<float> U = rkRot.GetColumn(0);
  Wm3::Vector3<float> V = rkRot.GetColumn(1);
  Wm3::Vector3<float> W = rkRot.GetColumn(2);

  _cC.Set(mx/(float)nSize, my/(float)nSize, mz/(float)nSize);
  _cU.Set(U.X(), U.Y(), U.Z());
  _cV.Set(V.X(), V.Y(), V.Z());
  _cW.Set(W.X(), W.Y(), W.Z());

  // set the sign for the vectors
  float fSumU, fSumV, fSumW;
  fSumU = fSumV = fSumW = 0.0f;
  for ( it = aclPoints.begin(); it!=aclPoints.end(); ++it )
  {
    float fU = _cU * ( *it - _cC );
    float fV = _cV * ( *it - _cC );
    float fW = _cW * ( *it - _cC );
    fSumU += (fU > 0 ? fU * fU : -fU * fU);
    fSumV += (fV > 0 ? fV * fV : -fV * fV);
    fSumW += (fW > 0 ? fW * fW : -fW * fW);
  }

  // avoid ambiguities concerning directions
  if ( fSumU < 0.0f )
    _cU *= -1.0f;
  if ( fSumV < 0.0f )
    _cV *= -1.0f;
  if ( fSumW < 0.0f )
    _cW *= -1.0f;

  if ((_cU%_cV)*_cW < 0.0f)
    _cW = -_cW; // make a right-handed system
}
