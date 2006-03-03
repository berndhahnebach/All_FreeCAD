/***************************************************************************
 *   Copyright (c) Berthold Grupp          2005                            *
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


#ifndef MESH_SETOPERATIONS_H
#define MESH_SETOPERATIONS_H

#ifndef _PreComp_
# include <list>
# include <map>
# include <vector>
#endif

#include "MeshKernel.h"
#include "Elements.h"
#include <Base/Builder3D.h>

// forward declarations

namespace Base
{
  class Vector2D;
  class Vector3D;
  class ViewProjMethod;
  class Polygon2D;
}

using Base::Vector2D;
using Base::Vector3D;
using Base::ViewProjMethod;
using Base::Polygon2D;

namespace MeshCore
{

class MeshGeomFacet;
class MeshGeomEdge;
class MeshKernel;
class MeshFacetGrid;
class MeshFacetArray;

/**
 * The MeshAlgorithm class provides algorithms base on meshes.
 */
class AppMeshExport SetOperations
{
public:
  enum OperationType { Union, Intersect, Difference };

  /// Construction
  SetOperations (MeshKernel &cutMesh1, MeshKernel &cutMesh2, MeshKernel &result, OperationType opType, float minDistanceToPoint = 1e-5f);
  /// Destruction
  virtual ~SetOperations (void) { }

public:

  /** Cut this mesh with another one. The result is a list of polylines
   * If the distance of the polyline to one of the points is less than minDistanceToPoint the polyline goes direct to the point
   */
  // void Do (MeshKernel &cutMesh1, MeshKernel &cutMesh2, float minDistanceToPoint = 1.0e-5f );
  void Do ();

protected:
  MeshKernel   &_cutMesh1;           /** Mesh for set operations source 1 */
  MeshKernel   &_cutMesh2;           /** Mesh for set operations source 2 */
  MeshKernel   &_resultMesh;         /** Result mesh */
  OperationType _operationType;      /** Set Operation Type */
  float         _minDistanceToPoint; /** Minimal distance to facet corner points */

private:
  /** map from facet index to his cutted points (mesh 1 and mesh 2) Key: Facet-Index  Value: List of MeshPoints */
  std::map<unsigned long, std::list<MeshPoint> > _facet2points1, _facet2points2;
  /** created facets from cut points */
  std::vector<MeshGeomFacet> _facets1, _facets2;
  /** set of points refering facets of mesh1 and mesh2 */
  std::map<MeshPoint, std::pair<std::list<unsigned long>, std::list<unsigned long> > >  _points2facets12;
  /** Indices of facets cutting the line */
  std::vector<unsigned long> _cutFacets1, _cutFacets2;
  /** Facets collected from region growing */
  std::vector<MeshGeomFacet> _facetsOf1, _facetsOf2;

  /** Cut mesh 1 with mesh 2 */
  void Cut ();
  /** Trianglute each facets cutted with his cutting points */
  void TriangulateMesh (MeshKernel &cutMesh, std::map<unsigned long, std::list<MeshPoint> > &mapPoints, std::vector<MeshGeomFacet> &facets);
  /** search facets for adding (with region growing) */
  void CollectFacets (MeshKernel &meshForRegionGrowing, MeshKernel &meshOther, std::vector<MeshGeomFacet> &facetsFromCutting, std::vector<unsigned long> facetsFromCuttingIndex, std::vector<MeshGeomFacet> &facetsCollected, bool first); 

  /** visual debug helper */
  Base::Builder3D _builder;

};


}; // namespace MeshCore 

#endif  // MESH_SETOPERATIONS_H 
