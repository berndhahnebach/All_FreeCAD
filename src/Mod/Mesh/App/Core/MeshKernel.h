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


#ifndef MESH_KERNEL_H
#define MESH_KERNEL_H

#include <assert.h>
#include <iostream>

#include "Elements.h"
#include "Helpers.h"

#include <Base/BoundBox.h>
#include <Base/Vector3D.h>

namespace Base{
  class Matrix4D;
  class Polygon2D;
  class ViewProjMethod;
}

namespace MeshCore {

// forward declarations
class MeshFacetIterator;
class MeshPointIterator;
class MeshGeomFacet;
class MeshFacet;
class MeshFacetVisitor;
class MeshPointVisitor;
class MeshFacetGrid;


/** 
 * The MeshKernel class is the basic class that holds the data points,
 * the edges and the facets describing a mesh object.
 * 
 * The bounding box is calculated during the buildup of the data
 * structure and gets only re-caclulated after insertion of new facets
 * but not after removal of facets.
 *
 * This class provides only some rudimental querying methods.
 */
class AppMeshExport MeshKernel
{
public:
  /// Construction
  MeshKernel (void);
  /// Construction
  MeshKernel (const MeshKernel &rclMesh);
  /// Destruction
  virtual ~MeshKernel (void)
  { Clear(); }

  /** @name I/O methods */
  //@{
  /// Binary streaming of data
  virtual void Write (std::ostream &rclOut) const;
  virtual void Read (std::istream &rclIn);
  //@}

  /** @name Querying */
  //@{
  /// Returns the number of facets
  unsigned long CountFacets (void) const
  { return (unsigned long)(_aclFacetArray.size()); }

  /// Returns the number of edge
  unsigned long CountEdges (void) const;

  // Returns the number of points
  unsigned long CountPoints (void) const
  { return (unsigned long)(_aclPointArray.size()); }

  unsigned int getMemSize (void) const
    { return _aclPointArray.size() * sizeof(MeshPoint) +
             _aclFacetArray.size() * sizeof(MeshFacet); }
  /// Determines the bounding box
  const Base::BoundBox3f& GetBoundBox (void) const
  { return _clBoundBox; }

  /** Forces a recalculation of the bounding box. This method should be called after
   * the removal of points.or after a transformation of the data structure.
   */ 
  void RecalcBoundBox (void);

  /** Returns the point at the given index. This method is rather slow and should be
   * called occassionally only.
   */
  inline MeshPoint GetPoint (unsigned long ulIndex) const;

  /** Returns an array of the vertex normals of the mesh. A vertex normal gets calculated
   * by summarizing the normals of the associated facets.
   */
  std::vector<Base::Vector3f> CalcVertexNormals() const;

  /** Returns the facet at the given index. This method is rather slow and should be
   * called occassionally only.
   */
  inline MeshGeomFacet GetFacet (unsigned long ulIndex) const;
  inline MeshGeomFacet GetFacet (const MeshFacet &rclFacet) const;

  /** Returns the point indices of the given facet index. */
  inline void GetFacetPoints ( unsigned long ulFaIndex, unsigned long &rclP0, 
                               unsigned long &rclP1, unsigned long &rclP2) const;
  /** Returns the indices of the neighbour facets of the given facet index. */
  inline void GetFacetNeighbours ( unsigned long ulIndex, unsigned long &rulNIdx0, 
                                   unsigned long &rulNIdx1, unsigned long &rulNIdx2) const;

  /** Determines all facets that are associated to this point. This method is rather
   * slow and should be called occassionally only.
   */
  std::vector<unsigned long> HasFacets (const MeshPointIterator &rclIter) const;

  /** Returns true if the data structure is valid. */
  virtual bool IsValid (void) const
  { return _bValid; }

  /** Returns the array of all data points. */
  const MeshPointArray& GetPoints (void) const { return _aclPointArray; }

  /** Returns the array of all facets */
  const MeshFacetArray& GetFacets (void) const { return _aclFacetArray; }

  /** Returns the array of all edges.
   *  Notice: The Edgelist will be temporary generated. Changes on the mesh structure does not affect the Edgelist
   */
  void GetEdges (std::vector<MeshGeomEdge>&) const;

  //@}

  /** @name Evaluation */
  //@{
  /** Calculates the surface area of the mesh object. */
  float GetSurface() const;
  /** Calculates the surface area of the segment defined by \a aSegment. */
  float GetSurface( const std::vector<unsigned long>& aSegment ) const;
  /** Calculates the volume of the mesh object. Therefore the mesh must be a solid, if not 0
   * is returned.
   */
  float GetVolume () const;
  /** Checks whether the mesh has open edges. */
  bool HasOpenEdges() const;
  /** Checks whether the mesh has non.manifold edges. An edge is regarded as non-manifolds if it
   * shares more than two facets.
   */
  bool HasNonManifolds() const;
  /** Checks whether the mesh intersects itself. */
  bool HasSelfIntersections() const;
  //@}

  /** Adds a single facet to the data structure. */
  MeshKernel& operator += (const MeshGeomFacet &rclSFacet);
  /** Adds an array of facet to the data structure. */
  MeshKernel& operator += (const std::vector<MeshGeomFacet> &rclVAry);

  /** @name Facet visitors
   * The MeshKernel class provides different methods to visit "topologic connected" facets 
   * to a given start facet. Two facets are regarded as "topologic connected" if they share 
   * a common edge or a common point.
   * All methods expect a MeshFacetVisitor as argument that can decide to continue or to stop. 
   * If there is no topologic neighbour facet any more being not marked as "VISIT" the algorithm
   * stops anyway.
   * @see MeshFacetVisitor, MeshOrientationVisitor, MeshSearchNeighbourFacetsVisitor
   * and MeshTopFacetVisitor.
   */
  //@{
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
  unsigned long VisitNeighbourFacets (MeshFacetVisitor &rclFVisitor, unsigned long ulStartFacet) const;
  /**
   * Does basically the same as the method above unless the facets that share just a common point
   * are regared as neighbours.
   */
  unsigned long VisitNeighbourFacetsOverCorners (MeshFacetVisitor &rclFVisitor, unsigned long ulStartFacet) const;
  //@}

  /** @name Point visitors
   * The MeshKernel class provides a method to visit neighbour points to a given start point.
   * Two points are regarded as neighbours if they share an edge.
   * The method expects a MeshPointVisitor as argument that can decide to continue or to stop. 
   * If there is no topologic neighbour point any more being not marked as "VISIT" the algorithm
   * stops anyway.
   */
  //@{
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
  unsigned long VisitNeighbourPoints (MeshPointVisitor &rclPVisitor, unsigned long ulStartPoint) const; 
  //@}

  /** @name Iterators 
   * The iterator methods are provided for convenience. They return an iterator object that
   * points to the first element in the appropriate list.
   * \code
   * MeshKernel mesh = ...
   * // iterate over all facets
   * for ( MeshFacetIterator it = mesh.FacetIterator(); it.More(); it.Next() )
   * ...
   * \endcode
   * An iterator can also be used in the following way
   * \code
   * MeshKernel mesh = ...
   * // iterate over all facets
   * MeshFacetIterator it(mesh);
   * for (  it.Init(); it.More(); it.Next() )
   * ...
   * \endcode
   */
  //@{
  /** Returns an iterator object to go over all facets. */
  MeshFacetIterator FacetIterator() const;
  /** Returns an iterator object to go over all points. */
  MeshPointIterator PointIterator() const;
  //@}

  /** @name Modification */
  //@{
  /** Adds a single facet to the data structure. */
  bool AddFacet(const MeshGeomFacet &rclSFacet);
  /** Adds an array of facets to the data structure. This method is very slow but keeps temporarily 
   * set properties and flags. If these flags are not important to keep then the += operator should
   * be used.
   */
  bool AddFacet(const std::vector<MeshGeomFacet> &rclVAry);
  /**
   * Adds an array of topologic facets to the data structure without inserting new points.
   * Facets which would create non-manifolds are not inserted.
   * The client programmer must make sure that the referenced point indices are correct and that
   * no geometric overlaps can be created. The method returns the total number of facets.
   * This method might be useful to close gaps or fill up holes in a mesh.
   * @note This method is quite expensive and should be rarely used.
   */
  unsigned long AddFacet(const std::vector<MeshFacet> &rclVAry);
  /**
   * Adds all facets and referenced points to the underlying mesh structure. The client programmer
   * must be sure that both meshes doesn't have geometric overlaps, otherwise the resulting mesh might
   * be invalid.
   * @note The method guarantees that the order of the arrays of the underlying mesh and of the given
   * array is kept.
   * @note Not all points of \a rPoints are necessarily appended to the underlying mesh but only these points which
   * are referenced by \a rFaces.
   */
  void Merge( const MeshPointArray& rPoints, const MeshFacetArray& rFaces );
  /** Deletes the facet the iterator points to. The deletion of a facet requires
   * the following steps:
   * \li Mark the neighbour index of all neighbour facets to the deleted facet as invalid
   * \li If the neighbour facet exists adjust the edge index if needed, if there is no neighbour
   *     remove the edges.
   * \li Adjust the indices of the neighbour facets of all facets.
   * \li Adjust the indices of the edges.
   * \li If there is no neighbour facet check if the points can be deleted.
   * True is returned if the facet could be deleted.
   */
  bool DeleteFacet (const MeshFacetIterator &rclIter);
  /**
   * Does basically the same as the method above unless that the index of the facet is given.
   */
  bool DeleteFacet (unsigned long ulInd);
  /** Removes several facets from the data structure. */
  virtual void DeleteFacets (const std::vector<unsigned long> &raulFacets);
  /** Deletes the point the iterator points to. The deletion of a point requires the following step:
   * \li Find all associated facets to this point.
   * \li Delete these facets.
   * True is returned if the point could be deleted.
   */
  bool DeletePoint (const MeshPointIterator &rclIter);
  /** Removes several points from the data structure. */
  virtual void DeletePoints (const std::vector<unsigned long> &raulPoints);
  /** Clears the whole data structure. */
  void Clear (void);
  /** Replaces the current data structure with the structure built up of the array 
   * of triangles given in \a rclVAry. \a rclVAry gets cleared automatically.
   */
  MeshKernel& operator = (std::vector<MeshGeomFacet> &rclVAry);
  /** Assignment operator. */
  MeshKernel& operator = (const MeshKernel &rclMesh);
  /** This allows to assign the mesh structure directly. The caller must make sure that the point indices are correctly set
   * but the neighbourhood gets checked and corrected if \a checkNeighbourHood is true.
   */
  void Assign(const MeshPointArray& rPoints, const MeshFacetArray& rFaces, bool checkNeighbourHood=false);
  /** This method does basically the same as Assign() unless that it adopts the content of both arrays.
   * These arrays are empty after assigning to the kernel. This method can be used for huge meshes to save memory
   * and increase speed.
   */
  void Adopt(MeshPointArray& rPoints, MeshFacetArray& rFaces, bool checkNeighbourHood=false);
  /// Transform the data structure with the given transformation matrix.
  virtual void operator *= (const Base::Matrix4D &rclMat);
  /// Transform the data structure with the given transformation matrix.
  void Transform (const Base::Matrix4D &rclMat);
  /** Moves the point at the given index along the vector \a rclTrans. */
  inline void MovePoint (unsigned long ulPtIndex, const Base::Vector3f &rclTrans);
  /** Sets the point at the given index to the new \a rPoint. */
  inline void SetPoint (unsigned long ulPtIndex, const Base::Vector3f &rPoint);
  /**
   * CheckFacets() is invoked within this method and all found facets get deleted from the mesh structure. 
   * The facets to be deleted are returned with their geometric reprsentation.
   * @see CheckFacets().
   */
  void CutFacets (const MeshFacetGrid& rclGrid, const Base::ViewProjMethod *pclP, const Base::Polygon2D& rclPoly, 
                  bool bCutInner, std::vector<MeshGeomFacet> &raclFacets);
  /**
   * Does basically the same as method above unless that the facets to be deleted are returned with their
   * index number in the facet array of the mesh structure.
   */
  void CutFacets (const MeshFacetGrid& rclGrid, const Base::ViewProjMethod* pclP, const Base::Polygon2D& rclPoly, 
                  bool bCutInner, std::vector<unsigned long> &raclCutted);
  //@}

  /// Rebuilds the neighbour indices for all facets.
  void RebuildNeighbours (void);

protected:
  /** Adds new edges and updates the available edges respectively. All edges
   * of the facets get checked if they have an associated MeshEdge. If needed 
   * a new edge gets added. The new edges get inserted into the array in accordance
   * to their index. The neighbour indices of the facets get resetted accordingly.
   */
  
  void RemoveInvalids (bool bWithEdgeCorrect = true, bool bWithEdgeDelete = false);
  /** Checks if this point is associated to no other facet and deletes if so.
   * The point indices of the facets get adjusted.
   * \a ulIndex is the index of the point to be deleted. \a ulFacetIndex is the index
   * of the quasi deleted facet and is ignored. If \a bOnlySetInvalid is true the point
   * doesn't get deleted but marked as invalid.
   */
  void ErasePoint (unsigned long ulIndex, unsigned long ulFacetIndex, bool bOnlySetInvalid = false);


  /** Adjusts the facet's orierntation to the given normal direction. */
  inline void AdjustNormal (MeshFacet &rclFacet, const Base::Vector3f &rclNormal);

  /** Calculates the normal to the given facet. */
  inline Base::Vector3f GetNormal (const MeshFacet &rclFacet) const;
  /** Calculates the gravity point to the given facet. */
  inline Base::Vector3f GetGravityPoint (const MeshFacet &rclFacet) const;

  MeshPointArray   _aclPointArray; /**< Holds the array of geometric points. */
  MeshFacetArray   _aclFacetArray; /**< Holds the array of facets. */
  Base::BoundBox3f _clBoundBox;    /**< The current calculated bounding box. */
  bool            _bValid; /**< Current state of validality. */

  // friends
  friend class MeshPointIterator;
  friend class MeshFacetIterator;
  friend class MeshFastFacetIterator;
  friend class MeshAlgorithm;
  friend class MeshTopoAlgorithm;
  friend class MeshFixNeighbourhood;
  friend class MeshFixDegenerations;
  friend class MeshFixSingleFacet;
  friend class MeshFixInvalids;
  friend class MeshFixDegeneratedFacets;
  friend class MeshBuilder;
};

inline MeshPoint MeshKernel::GetPoint (unsigned long ulIndex) const
{
  assert(ulIndex < _aclPointArray.size());
  return _aclPointArray[ulIndex];
}

inline MeshGeomFacet MeshKernel::GetFacet (unsigned long ulIndex) const
{
  assert(ulIndex < _aclFacetArray.size());

  const MeshFacet *pclF = &_aclFacetArray[ulIndex];
  MeshGeomFacet  clFacet;

  clFacet._aclPoints[0] = _aclPointArray[pclF->_aulPoints[0]];
  clFacet._aclPoints[1] = _aclPointArray[pclF->_aulPoints[1]];
  clFacet._aclPoints[2] = _aclPointArray[pclF->_aulPoints[2]];
  clFacet._ulProp       = pclF->_ulProp;
  clFacet._ucFlag       = pclF->_ucFlag;
  clFacet.CalcNormal();
  return clFacet;
}

inline MeshGeomFacet MeshKernel::GetFacet (const MeshFacet &rclFacet) const
{
  assert(rclFacet._aulPoints[0] < _aclPointArray.size());
  assert(rclFacet._aulPoints[1] < _aclPointArray.size());
  assert(rclFacet._aulPoints[2] < _aclPointArray.size());

  MeshGeomFacet  clFacet;
  clFacet._aclPoints[0] = _aclPointArray[rclFacet._aulPoints[0]];
  clFacet._aclPoints[1] = _aclPointArray[rclFacet._aulPoints[1]];
  clFacet._aclPoints[2] = _aclPointArray[rclFacet._aulPoints[2]];
  clFacet._ulProp       = rclFacet._ulProp;
  clFacet._ucFlag       = rclFacet._ucFlag;
  clFacet.CalcNormal();
  return  clFacet;
}

inline void MeshKernel::GetFacetNeighbours (unsigned long ulIndex, unsigned long &rulNIdx0, unsigned long &rulNIdx1, unsigned long &rulNIdx2) const 
{
  assert(ulIndex < _aclFacetArray.size());

  rulNIdx0 = _aclFacetArray[ulIndex]._aulNeighbours[0];
  rulNIdx1 = _aclFacetArray[ulIndex]._aulNeighbours[1];
  rulNIdx2 = _aclFacetArray[ulIndex]._aulNeighbours[2];
}

inline void MeshKernel::MovePoint (unsigned long ulPtIndex, const Base::Vector3f &rclTrans)
{
  _aclPointArray[ulPtIndex] += rclTrans;
}

inline void MeshKernel::SetPoint (unsigned long ulPtIndex, const Base::Vector3f &rPoint)
{
  _aclPointArray[ulPtIndex] = rPoint;
}

inline void MeshKernel::AdjustNormal (MeshFacet &rclFacet, const Base::Vector3f &rclNormal)
{
  Base::Vector3f clN = (_aclPointArray[rclFacet._aulPoints[1]] - _aclPointArray[rclFacet._aulPoints[0]]) %
                 (_aclPointArray[rclFacet._aulPoints[2]] - _aclPointArray[rclFacet._aulPoints[0]]);
  if ((clN * rclNormal) < 0.0f)
  {
    rclFacet.FlipNormal();
  }
}

inline Base::Vector3f MeshKernel::GetNormal (const MeshFacet &rclFacet) const
{
  Base::Vector3f clN = (_aclPointArray[rclFacet._aulPoints[1]] - _aclPointArray[rclFacet._aulPoints[0]]) %
                 (_aclPointArray[rclFacet._aulPoints[2]] - _aclPointArray[rclFacet._aulPoints[0]]);
  clN.Normalize();
  return clN;
}

inline Base::Vector3f MeshKernel::GetGravityPoint (const MeshFacet &rclFacet) const
{
  return (_aclPointArray[rclFacet._aulPoints[0]]+_aclPointArray[rclFacet._aulPoints[1]]+_aclPointArray[rclFacet._aulPoints[2]])/3.0f;
}

inline void MeshKernel::GetFacetPoints (unsigned long ulFaIndex, unsigned long &rclP0, unsigned long &rclP1, unsigned long &rclP2) const
{
  assert(ulFaIndex < _aclFacetArray.size());
  const MeshFacet& rclFacet = _aclFacetArray[ulFaIndex];
  rclP0 = rclFacet._aulPoints[0];  
  rclP1 = rclFacet._aulPoints[1];  
  rclP2 = rclFacet._aulPoints[2];  
}


} // namespace MeshCore

#endif // MESH_KERNEL_H 
