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


#ifndef MESH_DEGENERATION_H
#define MESH_DEGENERATION_H

#ifndef _PreComp_
# include <bitset>
# include <string>
# include <vector>
#endif

#include "Evaluation.h"
#include "Definitions.h"

namespace Mesh {

class MeshKernel;
class MeshGeomFacet;
class MeshFacetIterator;

/**
 * The MeshDegenerations class allows to examine the mesh for degenerated elements,
 * to invalid set elememts, duplicated elements, etc.
 * Moreover the class provides methods to fixup some of the mentioned degenrations.
 */
class AppMeshExport MeshDegenerations : public MeshEvaluation
{
public:
  enum TErrorTable 
  {
    InvalidFacet       =  0,    /**< as invalid marked facet */
    InvalidEdge        =  1,    /**< as invalid marked edge */
    InvalidPoint       =  2,    /**< as invalid marked point */
    DuplicatedFacets   =  3,    /**< duplicated facets */
    DuplicatedEdges    =  4,    /**< duplicated edges */
    DuplicatedPoints   =  5,    /**< duplicated points */
    OutOfRangeFacet    =  6,    /**< */
    OutOfRangeEdge     =  7,    /**< */
    OutOfRangePoint    =  8,    /**< */
    CorruptedFacets    =  9,    /**< */
    DegeneratedFacets  = 10,    /**< */
    Undefined          = 11,    /**< unknown error type */
  };

public:
  /** 
   * Construction.
   */
  MeshDegenerations (MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshDegenerations () { }
  /**
   * Returns true if the mesh contains any degenerated elements.
   */
  bool HasDegenerations () const { return _errMode.any(); }
  /**
   * Returns true if the mesh contains any degenerated elements of the type \a err.
   */
  bool HasDegeneration (TErrorTable tErr) const { return _errMode.test(size_t(tErr)); }
  /**
   * Returns the error types as unsigned long.
   */
  unsigned long GetDegenerations() const { return _errMode.to_ulong(); }
  /**
   * Returns an array of indices to the specified error type \a err. Depending on \a err the
   * indices can refer to facets, edges or points.
   */
  std::vector<unsigned long> GetIndices(TErrorTable err);
  /**
   * Returns the number of facets with an edge smaller than \a fMinEdgeLength.
   */
  unsigned long CountEdgeTooSmall (float fMinEdgeLength) const;
  /**
   * Removes all facets with an edge smaller than \a fMinEdgeLength without leaving holes or gaps
   * in the mesh. Returns the number of removed facets.
   */
  unsigned long RemoveEdgeTooSmall (float fMinEdgeLength = MeshDefinitions::_fMinPointDistance,
                                    float fMinEdgeAngle  = MeshDefinitions::_fMinEdgeAngle);
  /**
   * Searches for defaced facets. A facet is regarded as defaced if an angle is < 30° or > 120°.
   */
  std::vector<unsigned long> DefacedFacets() const;
  /**
   * Merges points to one if the distance between them is less than \a fMinDistance.
   * If \a open is true only points from open edges are regarded. Returns the number of removed points.
   */
  unsigned long MergePoints (bool open, float fMinDistance = MeshDefinitions::_fMinPointDistance);
  /**
   * Removes to an edge or to a point degenerated facets and returns the number of removed facets.
   */
  unsigned long RemoveDegeneratedFacets ();

protected:
  /**
   * Checks the mesh data structure for
   * \li If indexing of the facets and edges to the points is inside the range 
   * \li invalid indexing
   * \li multiple edges
   * \li invalid elements
   * \li multiple points
   * Returns true if the data structure is valid, false otherwise.
   */
  bool Evaluate ();
  /** @todo Not yet implemented. */
  bool Fixup();

private:
  /**
   * \internal Marks the facet with index \a ulFacetPos with too small edge length as deleted.
   * Two cases: left and right neighbours of the edge \a ulFront exist => delete one point and
   * correct indices.
   * only one neighbour exists then just remove the facet.
   */
  void RemoveFacetETS (unsigned long ulFacetPos, unsigned long ulFront);

  void AddErrorMode (TErrorTable tErr) { _errMode.set(size_t(tErr)); }
  void ResetErrorMode (TErrorTable tErr) { _errMode.reset(size_t(tErr)); } 
  void ResetErrorModes () { _errMode.reset(); } 

private:
  std::bitset<12> _errMode; /**< \internal */
};

} // namespace Mesh

#endif // MESH_DEGENERATION_H 
