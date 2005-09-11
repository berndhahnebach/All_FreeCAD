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


#ifndef MESHALGORITHM_H
#define MESHALGORITHM_H

#ifndef _PreComp_
# include <Wm3IntrSegment3Plane3.h>
# include <Wm3Vector3.h>
#endif

#include "MeshKernel.h"
#include "Elements.h"

// forward declarations
class TopoDS_Shape;
class gp_Pln;

namespace Base{
  class Vector2D;
  class Vector3D;
  class ViewProjMethod;
  class Polygon2D;
}

using Base::Vector2D;
using Base::Vector3D;
using Base::ViewProjMethod;
using Base::Polygon2D;

namespace MeshCore {

class MeshGeomFacet;
class MeshGeomEdge;
class MeshKernel;
class MeshFacetGrid;
class MeshFacetArray;

/**
 * The MeshAlgorithm class provides algorithms base on meshes.
 */
class AppMeshExport MeshAlgorithm
{
public:
  /// Construction
  MeshAlgorithm (MeshKernel &rclM) 
    : _rclMesh(rclM), _rclFAry(rclM._aclFacetArray), _rclPAry(rclM._aclPointArray) { }
  /// Destruction
  virtual ~MeshAlgorithm (void) { }

public:
  /**
   * Searches for the nearest facet to the ray defined by (\arclPt, \a rclDir). The point \a rclRes holds
   * the intersection point with the ray and the nearest facet with index \a rulFacet.
   * \note This method tests all facets so it should only be used occassionally.
   */
  bool NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes, 
                          unsigned long &rulFacet) const;
  /**
   * Searches for the nearest facet to the ray defined by (\arclPt, \a rclDir). The point \a rclRes holds
   * the intersection point with the ray and the nearest facet with index \a rulFacet.
   * \note This method is optimized by using a grid. So this method be used for a lot of tests..
   */
  bool NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, const MeshFacetGrid &rclGrid, 
                          Vector3D &rclRes, unsigned long &rulFacet) const;
  /**
   * Searches for the nearest facet to the ray defined by (\arclPt, \a rclDir). The point \a rclRes holds
   * the intersection point with the ray and the nearest facet with index \a rulFacet.
   * \note This method tests all facets taken from \a raulFacets so it should only be used occassionally.
   */
  bool NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, const std::vector<unsigned long> &raulFacets, 
                          Vector3D &rclRes, unsigned long &rulFacet) const;
  /**
   * Searches for the nearest facet to the ray defined by (\arclPt, \a rclDir). The point \a rclRes holds
   * the intersection point with the ray and the nearest facet with index \a rulFacet.
   * More a search radius around the ray of \a fMaxSearchArea is defined.
   * \note This method is optimized by using a grid. So this method be used for a lot of tests..
   */
  bool NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, float fMaxSearchArea, 
                          const MeshFacetGrid &rclGrid, Vector3D &rclRes, unsigned long &rulFacet) const;
  /**
   * Checks from the viewpoint \a rcView if the vertex \a rcVertex is visible or it is hidden by a facet. 
   * If the vertex is visible true is returned, false otherwise.
   */
  bool IsVertexVisible (const Vector3D &rcVertex, const Vector3D &rcView, const MeshFacetGrid &rclGrid ) const;
  /**
   * Calculates the average length of edges.
   */
  float GetAverageEdgeLength() const;
  /**
   * Returns all boundaries of the mesh.
   */
  void GetMeshBorders (std::list<std::vector<Vector3D> > &rclBorders);
  /**
   * Returns all boundaries of a subset the mesh defined by \a raulInd.
   */
  void GetFacetBorders (const std::vector<unsigned long> &raulInd, std::list<std::vector<Vector3D> > &rclBorders);
  /** Sets to all facets the flag \a tF. */
  virtual void SetFacetFlag (MeshFacet::TFlagType tF);
#ifdef Use_EdgeList
  /** Sets to all edges the flag \a tF. */
  virtual void SetEdgeFlag (MeshEdge::TFlagType tF);
#endif
  /** Sets to all points the flag \a tF. */
  virtual void SetPointFlag (MeshPoint::TFlagType tF);
  /** Resets of all facets the flag \a tF. */
  virtual void ResetFacetFlag (MeshFacet::TFlagType tF);
#ifdef Use_EdgeList
  /** Resets of all edges the flag \a tF. */
  virtual void ResetEdgeFlag (MeshEdge::TFlagType tF);
#endif
  /** Resets of all points the flag \a tF. */
  virtual void ResetPointFlag (MeshPoint::TFlagType tF);
  /** Sets to all facets in \a raulInds the flag \a tF. */
  void SetFacetsFlag (const std::vector<unsigned long> &raulInds, MeshFacet::TFlagType tF);
#ifdef Use_EdgeList
  /** Sets to all edges in \a raulInds the flag \a tF. */
  void SetEdgesFlag (const std::vector<unsigned long> &raulInds, MeshEdge::TFlagType tF);
#endif
  /** Sets to all points in \a raulInds the flag \a tF. */
  void SetPointsFlag (const std::vector<unsigned long> &raulInds, MeshPoint::TFlagType tF);
  /** Resets from all facets in \a raulInds the flag \a tF. */
  void ResetFacetsFlag (const std::vector<unsigned long> &raulInds, MeshFacet::TFlagType tF);
#ifdef Use_EdgeList
  /** Resets from all edges in \a raulInds the flag \a tF. */
  void ResetEdgesFlag (const std::vector<unsigned long> &raulInds, MeshEdge::TFlagType tF);
#endif
  /** Resets from all points in \a raulInds the flag \a tF. */
  void ResetPointsFlag (const std::vector<unsigned long> &raulInds, MeshPoint::TFlagType tF);
  /** Sets to all facets in \a raulInds the properties in raulProps. 
   * \note Both arrays must have the same size.
   */
  void SetFacetsProperty(const std::vector<unsigned long> &raulInds, const std::vector<unsigned long> &raulProps);
  /** Count all facets with the flag \a tF. */
  unsigned long CountFacetFlag (MeshFacet::TFlagType tF) const;
  /** Count all edges with the flag \a tF. */
  unsigned long CountEdgeFlag (MeshEdge::TFlagType tF) const;
  /** Count all points with the flag \a tF. */
  unsigned long CountPointFlag (MeshPoint::TFlagType tF) const;
  /** Returns all geometric points from the facets in \a rvecIndices. */
  void PointsFromFacetsIndices (const std::vector<unsigned long> &rvecIndices, std::vector<Vector3D> &rvecPoints) const;
  /**
   * CheckFacets() is invoked within this method and all found facets get deleted from the mesh structure. 
   * The facets to be deleted are returned with their geometric reprsentation.
   * @see CheckFacets().
   */
  void CutFacets (const MeshFacetGrid& rclGrid, const ViewProjMethod *pclP, const Polygon2D& rclPoly, 
                  bool bCutInner, std::vector<MeshGeomFacet> &raclFacets) const;
  /**
   * Does basically the same as method above unless that the facets to be deleted are returned with their
   * index number in the facet array of the mesh structure.
   */
  void CutFacets (const MeshFacetGrid& rclGrid, const ViewProjMethod* pclP, const Polygon2D& rclPoly, 
                  bool bCutInner, std::vector<unsigned long> &raclCutted) const;
  /**
   * Projects the determined facets through projection with \a pclProj into the 2D plane and checks for
   * intersection with the polygon.
   * If \a bInner is \a true than all facets with at least one corner inside the polygon get deleted. If \a
   * bInner is \a false then all facets with at least one corner outside the polygon get deleted.
   * This algorithm is optimized by using a grid.
   */
  void CheckFacets (const MeshFacetGrid &rclGrid, const ViewProjMethod* pclProj, const Polygon2D& rclPoly, 
                    bool bInner, std::vector<unsigned long> &rclRes) const;
  /**
   * Determines all facets of the given array \a raclFacetIndices that lie at the edge or that
   * have at least neighbour facet that is not inside the array. The resulting array \a raclResultIndices
   * is not be deleted before the algorithm starts. \a usLevel indicates how often the algorithm is 
   * repeated.
   */
  void CheckBorderFacets (const std::vector<unsigned long> &raclFacetIndices, 
                          std::vector<unsigned long> &raclResultIndices, unsigned short usLevel = 1);
  /**
   * Invokes CheckBorderFacets() to get all border facets of \a raclFacetIndices. Then the content of
   * \a raclFacetIndices is replaced by all facets that can be deleted.
   * \note The mesh structure is not modified by this method. This is in the responsibility of the user.
   */
  void CutBorderFacets (std::vector<unsigned long> &raclFacetIndices, unsigned short usLevel = 1);
  /**
   * Determines all border points as indices of the facets in \a raclFacetIndices. The points are unsorted.
   */
  void GetBorderPoints (const std::vector<unsigned long> &raclFacetIndices, std::set<unsigned long> &raclResultPointsIndices);
  /** Tessellates the shape \a aShape and replaces the mesh structure with the created facets. */
  bool MeshTopoShape(TopoDS_Shape aShape, float fAccuracy, float fAngle) const;
  /** Computes the surface of the mesh. */
  float Surface (void) const;
  /** Subsamples the mesh with point distance \a fDist and stores the points in \a rclPoints. */
  void SubSampleByDist  (float fDist, std::vector<Vector3D> &rclPoints) const;
  /**
   * Subsamples the mesh to produce around \a ulCtPoints. \a ulCtPoints should be greater
   * than 5 * number of facets.
   */
  void SubSampleByCount (unsigned long ulCtPoints, std::vector<Vector3D> &rclPoints) const;
  /** Returns only the points of the mesh without actually sampling the data. */
  void SubSampleAllPoints(std::vector<Vector3D> &rclPoints) const;
  /**
   * Searches for all facets that intersect the "search tube" with radius \a r around the polyline. 
   */
  void SearchFacetsFromPolyline (const std::vector<Vector3D> &rclPolyline, float fRadius,
                                 const MeshFacetGrid& rclGrid, std::vector<unsigned long> &rclResultFacetsIndices) const;
  /** Projects a point directly to the mesh (means nearest facet), the result is the facet index and
   * the formainate point, use second version with grid for more performance.
   */
  bool NearestPointFromPoint (const Vector3D &rclPt, unsigned long &rclResFacetIndex, Vector3D &rclResPoint) const;
  bool NearestPointFromPoint (const Vector3D &rclPt, const MeshFacetGrid& rclGrid, 
                              unsigned long &rclResFacetIndex, Vector3D &rclResPoint) const;
  bool NearestPointFromPoint (const Vector3D &rclPt, const MeshFacetGrid& rclGrid, float fMaxSearchArea, 
                              unsigned long &rclResFacetIndex, Vector3D &rclResPoint) const;
  /** Cuts the mesh with a plane. The result is a list of polylines. */
  bool CutWithPlane (const gp_Pln &rclPlane, const MeshFacetGrid &rclGrid, std::list<std::vector<Vector3D> > &rclResult, float fMinEps = 1.0e-2f); 
  bool CutWithPlane (const Vector3D &clBase, const Vector3D &clNormal, const MeshFacetGrid &rclGrid, 
                     std::list<std::vector<Vector3D> > &rclResult, float fMinEps = 1.0e-2f, bool bConnectPolygons = false); 
  /** Gets all facets that cut the plane and lying between the the two points. */
  void GetFacetsFromPlane (const MeshFacetGrid &rclGrid, const gp_Pln clPlane, const Vector3D &rclLeft, const Vector3D &rclRight, std::vector<unsigned long> &rclRes) const;
  /** Returns true if the distance from the \a rclPt to the facet \a ulFacetIdx is less than \a fMaxDistance.
   * If this restriction is met \a rfDistance is set to the actual distance, otherwise false is returned.
   */
  bool Distance (const Vector3D &rclPt, unsigned long ulFacetIdx, float fMaxDistance, float &rfDistance) const;
   
protected:
  /** Helper method to connect the intersection points to polylines. */
  bool ConnectLines (std::list<std::pair<Vector3D, Vector3D> > &rclLines, std::list<std::vector<Vector3D> >&rclPolylines, 
                    float fMinEps) const;
  bool ConnectPolygons(std::list<std::vector<Vector3D> > &clPolyList, std::list<std::pair<Vector3D, 
                       Vector3D> > &rclLines) const;
  /** Searches the nearest facet in \a raulFacets to the ray (\a rclPt, \a rclDir). */
  bool RayNearestField (const Vector3D &rclPt, const Vector3D &rclDir, const std::vector<unsigned long> &raulFacets, 
                        Vector3D &rclRes, unsigned long &rulFacet, float fMaxAngle = F_PI) const;

protected:
  MeshKernel      &_rclMesh; /**< The mesh kernel. */
  MeshFacetArray  &_rclFAry; /**< The facet array. */
  MeshPointArray  &_rclPAry; /**< The point array. */
};

/**
 * The MeshRefPointToFacets builds up a structure to have access to all facets indexing
 * a point.
 * \note If the underlying mesh kernel gets changed this structure becomes invalid and must
 * be rebuilt.
 */
class AppMeshExport MeshRefPointToFacets : public std::vector<std::set<MeshFacetArray::_TIterator> >
{
public:
  /// Construction
  MeshRefPointToFacets (MeshKernel &rclM) : _rclMesh(rclM) 
  { Rebuild(); }
  /// Destruction
  virtual ~MeshRefPointToFacets (void)
  {
    for (std::vector<std::set<MeshFacetArray::_TIterator> >::iterator it = begin(); it != end(); ++it)
      it->clear();
    clear();
    std::vector<std::set<MeshFacetArray::_TIterator> >().swap(*this);
  }

  /// Rebuilds up data structure
  void Rebuild (void);
  void Neighbours (unsigned long ulFacetInd, float fMaxDist, std::vector<MeshFacetArray::_TIterator> &rclNb);

protected:
  void SearchNeighbours(MeshFacetArray::_TIterator pFIter, const Vector3D &rclCenter, float fMaxDist, std::vector<MeshFacetArray::_TIterator> &rclNb);

protected:
  MeshKernel  &_rclMesh; /**< The mesh kernel. */
};

/**
 * The MeshRefFacetToFacets builds up a structure to have access to all facets sharing 
 * at least one same point.
 * \note If the underlying mesh kernel gets changed this structure becomes invalid and must
 * be rebuilt.
 */
class AppMeshExport MeshRefFacetToFacets : public std::vector<std::set<MeshFacetArray::_TIterator> >
{
public:
  /// Construction
  MeshRefFacetToFacets (MeshKernel &rclM) : _rclMesh(rclM)
  { Rebuild(); }
  /// Destruction
  virtual ~MeshRefFacetToFacets (void)
  {
    for (std::vector<std::set<MeshFacetArray::_TIterator> >::iterator it = begin(); it != end(); ++it)
      it->clear();
    clear();
    std::vector<std::set<MeshFacetArray::_TIterator> >().swap(*this);
  }
  /// Rebuilds up data structure
  void Rebuild (void);

  /// Returns a set of facets sharing one or more points with the facet with index \a ulFacetIndex.
  const std::set<MeshFacetArray::_TIterator>& Neighbours (unsigned long ulFacetIndex) const
  { return operator[](ulFacetIndex); }

protected:
  MeshKernel  &_rclMesh; /**< The mesh kernel. */
};

/**
 * The MeshRefPointToPoints builds up a structure to have access to all neighbour points  
 * of a point. Two points are neighbours if there is an edge indexing both points.
 * \note If the underlying mesh kernel gets changed this structure becomes invalid and must
 * be rebuilt.
 */
class AppMeshExport MeshRefPointToPoints : public std::vector<std::set<MeshPointArray::_TIterator> >
{
public:
  /// Construction
  MeshRefPointToPoints (MeshKernel &rclM) : _rclMesh(rclM) 
  { Rebuild(); }
  /// Destruction
  virtual ~MeshRefPointToPoints (void)
  {
    for (std::vector<std::set<MeshPointArray::_TIterator> >::iterator it = begin(); it != end(); ++it)
      it->clear();
    clear();
    std::vector<std::set<MeshPointArray::_TIterator> >().swap(*this);
  }

  /// Rebuilds up data structure
  void Rebuild (void);

protected:
  MeshKernel  &_rclMesh; /**< The mesh kernel. */
};


}; // namespace MeshCore 

#endif  // MESH_ALGORITHM_H 
