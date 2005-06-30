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

#ifndef _PreComp_
# include <assert.h>
#endif

#include "BoundBox.h"
#include "Elements.h"
#include "Helpers.h"

namespace Mesh {

// forward declarations
class MeshFacetIterator;
class MeshEdgeIterator;
class MeshPointIterator;
class MeshGeomFacet;
class Matrix4D;
class DataStream;
class Vector3D;
class MeshFacet;
class MeshHelpEdge;
class MeshFacetFunc;
class MeshSTL;

#undef Use_EdgeList

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
  virtual ~MeshKernel (void) {}

  /** @name I/O methods */
  //@{
  /// Binary streaming of data
  virtual void SaveStream (DataStream &rclOut);
  /// Binary streaming of data
  virtual void RestoreStream (DataStream &rclIn);
  //@}

  /** @name Querying */
  //@{
  /// Returns the number of facets
  unsigned long CountFacets (void) const
  { return (unsigned long)(_aclFacetArray.size()); }

#ifdef Use_EdgeList
  /// Returns the number of edges
  unsigned long CountEdges (void) const
  { return (unsigned long)(_aclEdgeArray.size()); }
#endif

  // Returns the number of points
  unsigned long CountPoints (void) const
  { return (unsigned long)(_aclPointArray.size()); }

  /// Determines the bounding box
  BoundBox3D GetBoundBox (void) const
  { return const_cast<BoundBox3D&>(_clBoundBox); }

  /** Forces a recalculation of the bounding box. This method should be called after
   * the removal of points.or after a transformation of the data structure.
   */ 
  void RecalcBoundBox (void);

  /** Returns the point at the given index. This method is rather slow and should be
   * called occassionally only.
   */
  inline MeshPoint GetPoint (unsigned long ulIndex) const;

#ifdef Use_EdgeList
  /** Returns the edge at the given index. This method is rather slow and should be
   * called occassionally only.
   */
  inline MeshGeomEdge GetEdge (unsigned long ulPos) const;
#endif

  /** Returns the facet at the given index. This method is rather slow and should be
   * called occassionally only.
   */
  inline MeshGeomFacet GetFacet (unsigned long ulIndex) const;

  /** Returns the point indices of the given facet index. */
  inline void GetFacetPoints ( unsigned long ulFaIndex, unsigned long &rclP0, 
                               unsigned long &rclP1, unsigned long &rclP2) const;
  /** Returns the indices of the neighbour facets of the given facet index. */
  inline void GetFacetNeighbours ( unsigned long ulIndex, unsigned long &rulNIdx0, 
                                   unsigned long &rulNIdx1, unsigned long &rulNIdx2);

#ifdef Use_EdgeList
  /** Returns true if the edge the iterator points to is a border edge. */
  inline bool IsBorder (const MeshEdgeArray::_TIterator pPEdge) const;
#endif

  /** Determines all facets that are associated to this point. This method is rather
   * slow and should be called occassionally only.
   */
  std::vector<unsigned long> HasFacets (const MeshPointIterator &rclIter) const;

  /** Returns true if the data structure is valid. */
  virtual bool IsValid (void) const
  { return _bValid; }

  /** Returns the array of all data points. */
  std::vector<MeshPoint>& GetPoints (void) { return _aclPointArray; }

  /** Returns the array of all facets */
  std::vector<MeshFacet>& GetFacets (void) { return _aclFacetArray; }
  //@}

  /** Adds a single facet to the data structure. */
  MeshKernel& operator += (const MeshGeomFacet &rclSFacet);
  /** Adds an array of facet to the data structure. */
  MeshKernel& operator += (const std::vector<MeshGeomFacet> &rclVAry);

  /** @name Modification */
  //@{
  /** Adds a single facet to the data structure. */
  bool AddFacet(const MeshGeomFacet &rclSFacet);
  /** Adds an array of facet to the data structure. */
  bool AddFacet(const std::vector<MeshGeomFacet> &rclVAry);
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
  /** Removes several facets from the data structure. */
  virtual void DeleteFacets (const std::vector<unsigned long> &raulFacets);
  /** Deletes the edge the iterator points to. The deletion of an edge requires the following step:
   * \li Delete the facet(s) associated to this edge.
   * True is returned if the edge could be deleted.
   */
  bool DeleteEdge (const MeshEdgeIterator &rclIter);
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
  /// Transform the data structure with the given transformation matrix.
  virtual void operator *= (const Matrix4D &rclMat);
  /** Moves the point at the given index along the vector \a rclTrans. */
  inline void MovePoint (unsigned long ulPtIndex, const Vector3D &rclTrans);
  //@}

protected:
  /** Adds new edges and updates the available edges respectively. All edges
   * of the facets get checked if they have an associated MeshEdge. If needed 
   * a new edge gets added. The new edges get inserted into the array in accordance
   * to their index. The neighbour indices of the facets get resetted accordingly.
   */
#ifdef Use_EdgeList
  void AddEdge (MeshFacet &rclFacet, unsigned long ulFacetIndex);
  /** Deletes and resets respectively all edges indexing the given facet.
   * If there is no neighbour facet then the edge gets deleted.
   * Resetting in this case means to make edges that index this facet indexing
   * the neighbour facet instead.
   */
  MeshEdgeArray::_TConstIterator FindEdge (unsigned long ulFacet, unsigned short usSide) const ;
  inline unsigned long FindEdge (const MeshHelpEdge &rclEdge) const;

  void CheckAndCorrectEdge (unsigned long ulFacetIndex);
  /** Deletes all points and facets that are marked as invalid and corrects the point and
   * neighbour indices at the same time.
   */
#endif
  
  void RemoveInvalids (bool bWithEdgeCorrect = true, bool bWithEdgeDelete = false);
  /** Checks if this point is associated to no other facet and deletes if so.
   * The point indices of the facets get adjusted.
   * \a ulIndex is the index of the point to be deleted. \a ulFacetIndex is the index
   * of the quasi deleted facet and is ignored. If \a bOnlySetInvalid is true the point
   * doesn't get deleted but marked as invalid.
   */
  void ErasePoint (unsigned long ulIndex, unsigned long ulFacetIndex, bool bOnlySetInvalid = false);

#ifdef Use_EdgeList
  /// Rebuilds the edge array.
  void RebuildEdgeArray (void);
#endif

  /// Rebuilds the neighbour indices for all facets.
  void RebuildNeighbours (void);

  /** Builds up the mesh data-structure. MeshPointBuilder is already created and gets deleted then.
   * The Map is not sorted.
   */
  void Assign (MeshPointBuilder &rclMap);

  /** Adjusts the facet's orierntation to the given normal direction. */
  inline void AdjustNormal (MeshFacet &rclFacet, const Vector3D &rclNormal);

  /** Calculates the normal to the given facet. */
  inline Vector3D GetNormal (const MeshFacet &rclFacet) const;

  MeshPointArray  _aclPointArray; /**< Holds the array of geometric points. */
#ifdef Use_EdgeList
  MeshEdgeArray   _aclEdgeArray; /**< Holds the array of edges. */
#endif
  MeshFacetArray  _aclFacetArray; /**< Holds the array of facets. */
  BoundBox3D      _clBoundBox; /**< The current calculated bounding box. */
  bool             _bValid; /**< Current state of validality. */

  // friends
  friend class MeshPointIterator;
  friend class MeshEdgeIterator;
  friend class MeshFacetIterator;
  friend class MeshFastFacetIterator;
  friend class MeshInventor;
  friend class MeshSTL;
  friend class MeshFacetFunc;
  friend class MeshRefPointToFacets;
  friend class MeshRefFacetToFacets;
  friend class MeshRefPointToPoints;
  friend class MeshFacetTools;
  friend class MeshSearchNeighbours;
  friend class MeshAlgorithm;
  friend class MeshSearchNeighbourFacetsVisitor;
  friend class MeshVisitFacets;
  friend class MeshVisitPoints;
  friend class MeshTopoAlgorithm;
  friend class MeshInfo;
};

#ifdef Use_EdgeList
inline unsigned long MeshKernel::FindEdge (const MeshHelpEdge &rclEdge) const
{
  unsigned long i, ulCt  = _aclEdgeArray.size();
  MeshHelpEdge    clEdge;
  const MeshEdge *pclEdge;

  for (i = 0; i < ulCt; i++)
  {
    pclEdge = &_aclEdgeArray[i];
    _aclFacetArray[pclEdge->Index()].GetEdge((unsigned short)(pclEdge->Side()), clEdge);
    if (clEdge == rclEdge)
      return i;
  }

  return ULONG_MAX;
}
#endif

inline MeshPoint MeshKernel::GetPoint (unsigned long ulIndex) const
{
  assert(ulIndex < _aclPointArray.size());
  return _aclPointArray[ulIndex];
}

#ifdef Use_EdgeList
inline MeshGeomEdge MeshKernel::GetEdge (unsigned long ulPos) const
{
  assert(ulPos < _aclEdgeArray.size());

  MeshHelpEdge   clH;
  MeshGeomEdge clEdge;
  unsigned short usSide  = (unsigned short)(_aclEdgeArray[ulPos].Side());
  unsigned long ulIndex = _aclEdgeArray[ulPos].Index();

  _aclFacetArray[ulIndex].GetEdge(usSide, clH);
  clEdge._aclPoints[0] = _aclPointArray[clH._ulIndex[0]];
  clEdge._aclPoints[1] = _aclPointArray[clH._ulIndex[1]];
  clEdge._bBorder = _aclFacetArray[ulIndex]._aulNeighbours[usSide] == ULONG_MAX;
  return clEdge;
}
#endif

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

inline void MeshKernel::GetFacetNeighbours (unsigned long ulIndex, unsigned long &rulNIdx0, unsigned long &rulNIdx1, unsigned long &rulNIdx2)
{
  assert(ulIndex < _aclFacetArray.size());

  rulNIdx0 = _aclFacetArray[ulIndex]._aulNeighbours[0];
  rulNIdx1 = _aclFacetArray[ulIndex]._aulNeighbours[1];
  rulNIdx2 = _aclFacetArray[ulIndex]._aulNeighbours[2];
}

inline void MeshKernel::MovePoint (unsigned long ulPtIndex, const Vector3D &rclTrans)
{
  _aclPointArray[ulPtIndex] += rclTrans;
}

inline void MeshKernel::AdjustNormal (MeshFacet &rclFacet, const Vector3D &rclNormal)
{
  Vector3D clN = (_aclPointArray[rclFacet._aulPoints[1]] - _aclPointArray[rclFacet._aulPoints[0]]) %
                 (_aclPointArray[rclFacet._aulPoints[2]] - _aclPointArray[rclFacet._aulPoints[0]]);
  if ((clN * rclNormal) < 0.0f)
  {
    rclFacet.FlipNormal();
  }
}

inline Vector3D MeshKernel::GetNormal (const MeshFacet &rclFacet) const
{
  Vector3D clN = (_aclPointArray[rclFacet._aulPoints[1]] - _aclPointArray[rclFacet._aulPoints[0]]) %
                 (_aclPointArray[rclFacet._aulPoints[2]] - _aclPointArray[rclFacet._aulPoints[0]]);
  clN.Normalize();
  return clN;
}

inline void MeshKernel::GetFacetPoints (unsigned long ulFaIndex, unsigned long &rclP0, unsigned long &rclP1, unsigned long &rclP2) const
{
  assert(ulFaIndex < _aclFacetArray.size());
  const MeshFacet& rclFacet = _aclFacetArray[ulFaIndex];
  rclP0 = rclFacet._aulPoints[0];  
  rclP1 = rclFacet._aulPoints[1];  
  rclP2 = rclFacet._aulPoints[2];  
}

#ifdef Use_EdgeList
inline bool MeshKernel::IsBorder (const MeshEdgeArray::_TIterator pPEdge) const
{
  return _aclFacetArray[pPEdge->Index()]._aulNeighbours[pPEdge->Side()] == ULONG_MAX;
}
#endif


} // namespace Mesh

#endif // MESH_KERNEL_H 
