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


#ifndef VISITOR_H
#define VISITOR_H

namespace Mesh {

class MeshFacet;
class MeshKernel;
class MeshFacetVisitor;

/**
 * Abstract base class for facet visitors. 
 * The MeshFacetVisitor class can be used for the so called
 * "Region growing" algorithms.
 */
class AppMeshExport MeshFacetVisitor
{
public:
  /// Construction 
  MeshFacetVisitor(void) { }
  /// Denstruction 
  virtual ~MeshFacetVisitor(void) { }
  /** Needs to be implemented in sub-classes.
   * \a rclFacet is the currently visited facet with the index \a ulFInd, \a rclFrom is the last visited facet
   * and \a ulLevel indicates the ring number around the start facet. 
   * If \a true is returned the next iteration is done if there are still facets to visit. If \a false is 
   * returned the calling method stops immediately visiting further facets.
   */
  virtual bool Visit (MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long ulLevel) = 0;
};

/**
 * The MeshVisitFacets class provides different methods to visit "topologic connected" facets 
 * to a given start facet.
 * Two facets are regarded as "topologic connected" if they share a common edge or a common 
 * point.
 * All methods expect a MeshFacetVisitor as argument that can decide to continue or to stop. 
 * If there is no topologic neighbour facet any more being not marked as "VISIT" the algorithm
 * stops anyway.
 */
class AppMeshExport MeshVisitFacets
{
public:
  /// Construction 
  MeshVisitFacets (MeshKernel &rclM) : _rclMesh(rclM) { }
  /// Denstruction 
  virtual ~MeshVisitFacets(void) { }
  /**
   * This method visits all neighbour facets, i.e facets that share a common edge 
   * starting from the facet associated to index \a ulStartFacet. All facets having set the VISIT 
   * flag are ignored. Therefore the user have to set or unset this flag if needed.
   * All facets that get visited during this algorithm are marked as VISIT and the Visit() method
   * of the given MeshFacetVisitor gets invoked. 
   * If there are no unvisited neighbours any more the algorithms returns immediately and returns 
   * the number of visited facets.
   * \note For the start facet \a ulStartFacet MeshFacetVisitor::Visit() does not get invoked though
   * the facet gets marked as VISIT.
   */
  virtual unsigned long VisitNeighbours (MeshFacetVisitor &rclFVisitor, unsigned long ulStartFacet);
  /**
   * Does basically the same as the method above unless the facets that share just a common point
   * are regared as neighbours.
   */
  virtual unsigned long VisitNeighboursOverCorners (MeshFacetVisitor &rclFVisitor, unsigned long ulStartFacet);
  /// Not yet implemented.
  virtual unsigned long VisitTopologicalNeighbours (MeshFacetVisitor &rclFVisitor, unsigned long ulStartFacet);

protected:
  MeshKernel &_rclMesh; /**< The mesh kernel. */
};

/**
 * Special mesh visitor that searches for facets within a given search radius.
 */
class AppMeshExport MeshSearchNeighbourFacetsVisitor : public MeshFacetVisitor
{
public:
  MeshSearchNeighbourFacetsVisitor (MeshKernel &rclMesh, float fRadius, unsigned long ulStartFacetIdx);
  virtual ~MeshSearchNeighbourFacetsVisitor () {}
  /** Checks the facet if it lies inside the search radius. */
  inline bool Visit (MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long ulLevel);
  /** Resets the VISIT flag of already visited facets. */
  inline std::vector<unsigned long> GetAndReset (void);

protected:
  MeshKernel& _rclMeshBase; /**< The mesh kernel. */
  Vector3D    _clCenter; /**< Center. */
  float  _fRadius; /**< Search radius. */
  unsigned long _ulCurrentLevel;
  bool _bFacetsFoundInCurrentLevel;
  std::vector<unsigned long>  _vecFacets; /**< Found facets. */
};

inline bool MeshSearchNeighbourFacetsVisitor::Visit (MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long ulLevel)
{
  if (ulLevel > _ulCurrentLevel)
  {
    if (_bFacetsFoundInCurrentLevel == false)
      return false;
    _ulCurrentLevel = ulLevel;
    _bFacetsFoundInCurrentLevel = false;
  }

  MeshPointArray &rclPtAry = _rclMeshBase._aclPointArray;

  for (int i = 0; i < 3; i++)
  {
    if ( Mesh::Distance(_clCenter, rclPtAry[rclFacet._aulPoints[i]]) < _fRadius)
    {
      _vecFacets.push_back(ulFInd);
      _bFacetsFoundInCurrentLevel = true;
      return true;
    }
  }

  return true;
}

/**
 * The MeshTopFacetVisitor just collects the indices of all visited facets.
 */
class AppMeshExport MeshTopFacetVisitor : public MeshFacetVisitor
{
public:
  MeshTopFacetVisitor (std::vector<unsigned long> &raulNB) : _raulNeighbours(raulNB) {}
  virtual ~MeshTopFacetVisitor () {}
  /** Collects the facet indices. */
  virtual bool Visit (MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long)
  {
    _raulNeighbours.push_back(ulFInd);
    return true;
  }

protected:
  std::vector<unsigned long>  &_raulNeighbours; /**< Indices of all visited facets. */
};

// -------------------------------------------------------------------------

/**
 * Abstract base class for point visitors. 
 */
class AppMeshExport MeshPointVisitor
{
public:
  /// Construction 
  MeshPointVisitor(void) { }
  /// Denstruction 
  virtual ~MeshPointVisitor(void) { }
  /** Needs to be implemented in sub-classes.
   * \a rclPoint is the currently visited point with the index \a ulPInd, \a rclFrom is the last visited point
   * and \a ulLevel indicates the ring number around the start point. 
   * If \a true is returned the next iteration is done if there are still point to visit. If \a false is 
   * returned the calling method stops immediately visiting further points.
   */
  virtual bool Visit (MeshPoint &rclPoint, const MeshPoint &rclFrom, unsigned long ulPInd, unsigned long ulLevel) = 0;
};

/**
 * The MeshVisitPoints class provides a method to visit neighbour points to a given start point.
 * Two points are regarded as neighbours if the mesh has an edge with these points.
 */
class AppMeshExport MeshVisitPoints
{
public:
  /// Construction 
  MeshVisitPoints (MeshKernel &rclM) : _rclMesh(rclM)
  {
  }

  /// Denstruction 
  virtual ~MeshVisitPoints(void)
  {
  }
 
  /**
   * This method visits all neighbour points starting from the point associated to index \a ulStartPoint. 
   * All points having set the VISIT flag are ignored. Therefore the user have to set or unset this flag 
   * if needed before the algorithm starts.
   * All points that get visited during this algorithm are marked as VISIT and the Visit() method
   * of the given MeshPointVisitor gets invoked. 
   * If there are no unvisited neighbours any more the algorithms returns immediately and returns 
   * the number of visited points.
   * \note For the start facet \a ulStartPoint MeshPointVisitor::Visit() does not get invoked though
   * the point gets marked as VISIT.
   */
  virtual unsigned long VisitNeighbours (MeshPointVisitor &rclPVisitor, unsigned long ulStartPoint); 

protected:
  MeshKernel &_rclMesh;
};

} // namespace Mesh

#endif // VISITOR_H 

