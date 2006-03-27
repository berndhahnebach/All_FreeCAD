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

namespace MeshCore {

class MeshKernel;
class MeshGeomFacet;
class MeshFacetIterator;

/**
 * The MeshEvalInvalids class searches for as 'Invalid' marked facets and points.
 * Basically this comes from a not properly implemented algorithm that marks facets or points
 * as 'Invalid' without removing them from the mesh kernel.
 * @see MeshFixInvalids
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalInvalids : public MeshEvaluation
{
public:
  /**
   * Construction.
   */
  MeshEvalInvalids (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalInvalids () { }
  /** 
   * Searches for as 'Invalid' marked points or facets.
   */
  bool Evaluate ();
};

/**
 * The MeshFixInvalids class deletes all elements that are marked as 'Invalid'.
 * @see MeshEvalInvalids
 * @author Werner Mayer
 */
class AppMeshExport MeshFixInvalids : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixInvalids (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixInvalids () { }
  /** 
   * Remove invalid elements.
   */
  bool Fixup ();
};

/**
 * The MeshEvalDuplicatePoints class searches for duplicated points.
 * A point is regarded as duplicated if the distances between x, y and z coordinates of two points is 
 * less than an epsilon (defined by MeshDefinitions::_fMinPointDistanceD1, default value=1.0e-5f).
 * @see MeshFixDuplicatePoints
 * @see MeshEvalDegeneratedFacets
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalDuplicatePoints : public MeshEvaluation
{
public:
  /**
   * Construction.
   */
  MeshEvalDuplicatePoints (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalDuplicatePoints () { }
  /** 
   * Searches for duplicated points.
   */
  bool Evaluate ();
};

/**
 * The MeshFixDuplicatePoints class merges duplicated points.
 * @see MeshEvalDuplicatePoints
 * @author Werner Mayer
 */
class AppMeshExport MeshFixDuplicatePoints : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixDuplicatePoints (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixDuplicatePoints () { }
  /** 
   * Merges duplicated points.
   */
  bool Fixup ();
};

/**
 * The MeshEvalDuplicateFacets class searches for duplicated facets.
 * A facet is regarded as duplicated if all its point indices refer to the same location in the point array of the mesh kernel. 
 * The actual geometric points are not taken into consideration.
 * @see MeshFixDuplicateFacets
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalDuplicateFacets : public MeshEvaluation
{
public:
  /**
   * Construction.
   */
  MeshEvalDuplicateFacets (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalDuplicateFacets () { }
  /** 
   * Searches for duplicated facets.
   */
  bool Evaluate ();
};

/**
 * The MeshFixDuplicateFacets class removes duplicated facets from the mesh structure.
 * @see MeshEvalDuplicateFacets
 * @author Werner Mayer
 */
class AppMeshExport MeshFixDuplicateFacets : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixDuplicateFacets (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixDuplicateFacets () { }
  /** 
   * Removes duplicated facets.
   */
  bool Fixup ();
};

/**
 * The MeshEvalDegeneratedFacets class searches for degenerated facets. A facet is degenerated either if its points
 * are collinear, i.e. they lie on a line or two points are coincident. In the latter case these points are duplicated.
 * @see MeshEvalDuplicatePoints
 * @see MeshFixDegeneratedFacets
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalDegeneratedFacets : public MeshEvaluation
{
public:
  /**
   * Construction.
   */
  MeshEvalDegeneratedFacets (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalDegeneratedFacets () { }
  /** 
   * Searches degenerated facets.
   */
  bool Evaluate ();
};

/**
 * The MeshFixDegeneratedFacets class tries to fix degenerated by removing the concerning facets.
 * @see MeshEvalDegeneratedFacets
 * @author Werner Mayer
 */
class AppMeshExport MeshFixDegeneratedFacets : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixDegeneratedFacets (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixDegeneratedFacets () { }
  /** 
   * Removes degenerated facets.
   */
  bool Fixup ();
};

/**
 * The MeshEvalRangeFacet class checks whether a facet points to neighbour
 * facets that are out of range.
 * @see MeshFixRangeFacet
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalRangeFacet : public MeshEvaluation
{
public:
  /**
   * Construction.
   */
  MeshEvalRangeFacet (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalRangeFacet () { }
  /** 
   * Searches for facets that has neighbour facet indices out of range.
   */
  bool Evaluate ();
};

/** 
 * The MeshFixRangeFacet class fixes facets with invalid neighbour indices.
 * @see MeshEvalRangeFacet
 * @author Werner Mayer
 */
class AppMeshExport MeshFixRangeFacet : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixRangeFacet (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixRangeFacet () { }
  /** 
   * Fixes facets with neighbour indices out of range.
   */
  bool Fixup ();
};

/** 
 * The MeshEvalRangePoint class searches for facets that has point indices out of range.
 * @see MeshFixRangePoint
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalRangePoint : public MeshEvaluation
{
public:
  /**
   * Construction.
   */
  MeshEvalRangePoint (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalRangePoint () { }
  /** 
   * Searches for facets that has point indices out of range.
   */
  bool Evaluate ();
};

/**
 * The MeshFixRangePoint class fixes the facets with point indices out of range.
 * @see MeshFixRangePoint
 * @author Werner Mayer
 */
class AppMeshExport MeshFixRangePoint : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixRangePoint (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixRangePoint () { }
  /**
   * Fixes facets with point indices out of range.
   */
  bool Fixup ();
};

/**
 * The MeshEvalCorruptedFacets class searches for facets with several equal point
 * indices.
 * @see MeshFixCorruptedFacets
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalCorruptedFacets : public MeshEvaluation
{
public:
  /**
   * Construction.
   */
  MeshEvalCorruptedFacets (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalCorruptedFacets () { }
  /** 
   * Searches for corrupted facets.
   */
  bool Evaluate ();
};

/**
 * The MeshFixCorruptedFacets class fixes corrupted facets by removing them from the mesh
 * structure.
 * @see MeshEvalCorruptedFacets
 * @author Werner Mayer
 */
class AppMeshExport MeshFixCorruptedFacets : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixCorruptedFacets (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixCorruptedFacets () { }
  /** 
   * Removes corrupted facets.
   */
  bool Fixup ();
};

/**
 * The MeshEvalDegenerations class allows to examine the mesh for degenerated elements,
 * to invalid set elememts, duplicated elements, etc.
 * Moreover the class provides methods to fixup some of the mentioned degenerations.
 */
class AppMeshExport MeshEvalDegenerations : public MeshEvaluation
{
public:
  enum TErrorTable 
  {
    InvalidFacet       =  0,    /**< as invalid marked facet */
    InvalidPoint       =  1,    /**< as invalid marked point */
    DuplicatedFacets   =  2,    /**< duplicated facets */
    DuplicatedPoints   =  3,    /**< duplicated points */
    OutOfRangeFacet    =  4,    /**< */
    OutOfRangePoint    =  5,    /**< */
    CorruptedFacets    =  6,    /**< */
    DegeneratedFacets  =  7,    /**< */
    Undefined          =  8,    /**< unknown error type */
  };

public:
  /**
   * Construction.
   */
  MeshEvalDegenerations (const MeshKernel &rclM) : MeshEvaluation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshEvalDegenerations () { }
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
   * Searches for defaced facets. A facet is regarded as defaced if an angle is < 30° or > 120°.
   */
  std::vector<unsigned long> DefacedFacets() const;

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

private:
  void AddErrorMode (TErrorTable tErr) { _errMode.set(size_t(tErr)); }
  void ResetErrorMode (TErrorTable tErr) { _errMode.reset(size_t(tErr)); } 
  void ResetErrorModes () { _errMode.reset(); } 

private:
  std::bitset<12> _errMode; /**< \internal */
};

class AppMeshExport MeshFixDegenerations : public MeshValidation
{
public:
  /**
   * Construction.
   */
  MeshFixDegenerations (MeshKernel &rclM) : MeshValidation( rclM ) { }
  /** 
   * Destruction.
   */
  ~MeshFixDegenerations () { }
  /**
   * Removes all facets with an edge smaller than \a fMinEdgeLength without leaving holes or gaps
   * in the mesh. Returns the number of removed facets.
   */
  unsigned long RemoveEdgeTooSmall (float fMinEdgeLength = MeshDefinitions::_fMinPointDistance,
                                    float fMinEdgeAngle  = MeshDefinitions::_fMinEdgeAngle);
  /**
   * Merges points to one if the distance between them is less than \a fMinDistance.
   * If \a open is true only points from open edges are regarded. Returns the number of removed points.
   */
  unsigned long MergePoints (bool open, float fMinDistance = MeshDefinitions::_fMinPointDistance);
  /**
   * Removes to an edge or to a point degenerated facets and returns the number of removed facets.
   */
  unsigned long RemoveDegeneratedFacets ();

private:
  /**
   * \internal Marks the facet with index \a ulFacetPos with too small edge length as deleted.
   * Two cases: left and right neighbours of the edge \a ulFront exist => delete one point and
   * correct indices.
   * only one neighbour exists then just remove the facet.
   */
  void RemoveFacetETS (unsigned long ulFacetPos, unsigned long ulFront);
};

} // namespace MeshCore

#endif // MESH_DEGENERATION_H 
