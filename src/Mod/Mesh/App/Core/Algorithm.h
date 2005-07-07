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

namespace Mesh {

class MeshGeomFacet;
class MeshGeomEdge;
class BoundBox3D;
class Vector3D;

/**
 * The MeshFacetFunc class provides methods to extract information about
 * a MeshGeomFacet.
 */
class AppMeshExport MeshFacetFunc
{
public:
  /** Checks if the facet intersects with the given bounding box. */
  static bool IntersectBoundingBox (const MeshGeomFacet &rclFacet, const BoundBox3D &rclBB);
  /** Checks if the facet is inside the bounding box or intersects with it. */
  static inline bool ContainedByOrIntersectBoundingBox (const MeshGeomFacet &rcF, const BoundBox3D &rcBB);
  /** Converts a MeshFacet into a MeshGeomFacet. */
  static inline  MeshGeomFacet ToGeomFacet (const MeshKernel &rclMesh, const MeshFacet &rclFacet);
  /** Calculates the area of a facet. */
  static inline float Area (const MeshGeomFacet &rclFacet);
  /** Checks whether the given point is inside the facet with tolerance \a fDistance. 
   * This method does actually the same as MeshGeomFacet::IsPointOf() but this implementation 
   * is done more effective through comparison of normals.
   */
  static bool IsPointOf (const MeshGeomFacet &rclFacet, const Vector3D& rclP, float fDistance);
  /** This method projects the second facet onto the plane defined by the first facet and checks then
   * if these facets intersects.
   */
  static bool IntersectWithProjectedFacet(const MeshGeomFacet &rclFacet1, const MeshGeomFacet &rclFacet2);
  /** This method checks if these facets intersects.
   */
  static bool IntersectWithFacet(const MeshGeomFacet &rclFacet1, const MeshGeomFacet &rclFacet2);
  /** Calculates the shortest distance from the line segment defined by \a rcP1 and \a rcP2 to
   * this facet.
   */
  static float DistanceToLineSegment (const MeshGeomFacet &rcF, const Vector3D &rcP1, const Vector3D &rcP2);
  /** Calculates the shortest distance from the point \a rcPt to the facet. */
  static float DistanceToPoint (const MeshGeomFacet &rclFacet, const Vector3D &rcPt)
  { Vector3D res; return DistanceToPoint(rclFacet, rcPt, res); }
  /** Calculates the shortest distance from the point \a rcPt to the facet. \a rclNt is the point of the facet
   * with shortest distance.
   */
  static float DistanceToPoint  (const MeshGeomFacet &rclFacet, const Vector3D &rclPt, Vector3D& rclNt );
  /** Calculates the intersection point of the line defined by the base \a rclPt and the direction \a rclDir
   * with the facet. The intersection must be inside the facet. If there is no intersection false is returned.
   */
  static bool IntersectWithLine (const MeshGeomFacet &rclFacet, const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes);
  /** Calculates the intersection point of the line defined by the base \a rclPt and the direction \a rclDir
   * with the facet. The intersection must be inside the facet. If there is no intersection false is returned.
   * This does actually the same as IntersectWithLine() with one additionally constraint that the angle 
   * between the direction of the line and the normal of the plane must not exceed \a fMaxAngle.
   */
  static bool Forminate (const MeshGeomFacet &rclFacet, const Vector3D &rclPt, const Vector3D &rclDir, 
                         Vector3D &rclRes, float fMaxAngle = F_PI);
  /** Checks if the facet intersects with the plane defined by the base \a rclBase and the normal 
   * \a rclNormal and returns true if two points are found, false otherwise.
   */
  static bool IntersectWithPlane (const MeshGeomFacet &rclFacet, const Vector3D &rclBase, const Vector3D &rclNormal, Vector3D &rclP1, Vector3D &rclP2);
  /**
   * Checks if the facet intersects with the plane defined by the base \a rclBase and the normal
   * \a rclNormal.
   */
  static inline bool IntersectWithPlane (const MeshGeomFacet &rclFacet, const Vector3D &rclBase, 
                                         const Vector3D &rclNormal);
  /** Checks if the plane defined by the facet \a rclFacet intersects with the line defined by the base
   * \a rclBase and the direction \a rclNormal and returns the intersection point \a rclRes if possible. 
   */
  static bool IntersectPlaneWithLine (const MeshGeomFacet &rclFacet, const Vector3D &rclBase, 
                                      const Vector3D &rclNormal, Vector3D &rclRes );
  /** Calculates the volume of the prism defined by two facets. 
   * \note The two facets must not intersect.
   */
  static inline float VolumeOfPrism (const MeshGeomFacet& rclF1, const MeshGeomFacet& rclF2);
  /** Subsamples the facet into points with resolution \a fStep. */
  static void SubSample (const MeshGeomFacet &rclFacet, float fStep, std::vector<Vector3D> &rclPoints);
  /** Calculates the center and radius of the inner circle of the facet. */
  static float CenterOfInnerCircle(const MeshGeomFacet &rclFacet, Vector3D& rclCenter);
  /** Calculates the center and radius of the outer circle of the facet. */
  static float CenterOfOuterCircle(const MeshGeomFacet &rclFacet, Vector3D& rclCenter);
  /** Returns the edge number of the facet that is nearest to the point \a rclPt. */
  static unsigned short NearestEdgeToPoint(const MeshGeomFacet& rclFacet, const Vector3D& rclPt);

protected:
  /** Sqrt(fabs(\a fVal)) */
  static float Dist (float fVal)
  { return float(sqrt(fabs(fVal))); }
};

/**
 * The MeshEdgeFunc class provides methods to extract information about
 * a MeshGeomEdge.
 */
class AppMeshExport MeshEdgeFunc
{
public:
  /** Checks if the edge is inside the bounding box or intersects with it. */
  static bool ContainedByOrIntersectBoundingBox (const MeshGeomEdge &rclEdge, const BoundBox3D &rclBB );
  /** Returns the bounding box of the edge. */
  static BoundBox3D GetBoundBox (const MeshGeomEdge &rclEdge);
  /** Checks if the edge intersects with the given bounding box. */
  static bool IntersectBoundingBox (const MeshGeomEdge &rclEdge, const BoundBox3D &rclBB);
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


inline bool MeshFacetFunc::ContainedByOrIntersectBoundingBox (const MeshGeomFacet &rclFacet, const BoundBox3D &rclBB)
{
  // Test, ob alle Eckpunkte des Facets sich auf einer der 6 Seiten der BB befinden
  if ((rclFacet.GetBoundBox() && rclBB) == false)
    return false;

  // Test, ob Facet-BB komplett in BB liegt
  if (rclBB.IsInBox(rclFacet.GetBoundBox()))
    return true;

  // Test, ob einer der Eckpunkte in BB liegt
  for (int i=0;i<3;i++)
  {
    if (rclBB.IsInBox(rclFacet._aclPoints[i]))
      return true;
  }

  // "echter" Test auf Schnitt
  if (MeshFacetFunc::IntersectBoundingBox(rclFacet, rclBB))
    return true;

  return false;
}

inline  MeshGeomFacet MeshFacetFunc::ToGeomFacet (const MeshKernel &rclMesh, const MeshFacet &rclFacet)
{ 
  MeshGeomFacet  clRet;
  clRet._aclPoints[0] = rclMesh._aclPointArray[rclFacet._aulPoints[0]];
  clRet._aclPoints[1] = rclMesh._aclPointArray[rclFacet._aulPoints[1]];
  clRet._aclPoints[2] = rclMesh._aclPointArray[rclFacet._aulPoints[2]];
  clRet._ulProp       = rclFacet._ulProp;
  clRet.CalcNormal();
  return  clRet;
}

inline float MeshFacetFunc::Area (const MeshGeomFacet &rclFacet)
{
  return ((rclFacet._aclPoints[1] - rclFacet._aclPoints[0]) % 
          (rclFacet._aclPoints[2] - rclFacet._aclPoints[0])).Length() / 2.0f;
}

inline float MeshFacetFunc::VolumeOfPrism (const MeshGeomFacet& rclF1, const MeshGeomFacet& rclF2)
{
  Vector3D P1 = rclF1._aclPoints[0];
  Vector3D P2 = rclF1._aclPoints[1];
  Vector3D P3 = rclF1._aclPoints[2];
  Vector3D Q1 = rclF2._aclPoints[0];
  Vector3D Q2 = rclF2._aclPoints[1];
  Vector3D Q3 = rclF2._aclPoints[2];

  if ((P1-Q2).Length() < (P1-Q1).Length())
  {
    Vector3D tmp = Q1;
    Q1 = Q2;
    Q2 = tmp;
  }
  if ((P1-Q3).Length() < (P1-Q1).Length())
  {
    Vector3D tmp = Q1;
    Q1 = Q3;
    Q3 = tmp;
  }
  if ((P2-Q3).Length() < (P2-Q2).Length())
  {
    Vector3D tmp = Q2;
    Q2 = Q3;
    Q3 = tmp;
  }

  Vector3D N1 = (P2-P1) % (P3-P1);
  Vector3D N2 = (P2-P1) % (Q2-P1);
  Vector3D N3 = (Q2-P1) % (Q1-P1);

  float fVol=0.0f;
  fVol += float(fabs((Q3-P1) * N1));
  fVol += float(fabs((Q3-P1) * N2));
  fVol += float(fabs((Q3-P1) * N3));

  fVol /= 6.0f;

  return fVol;;
}

inline bool MeshFacetFunc::IntersectWithPlane (const MeshGeomFacet &rclFacet, const Vector3D &rclBase, const Vector3D &rclNormal)
{
  bool bD0 = (rclFacet._aclPoints[0].DistanceToPlane(rclBase, rclNormal) > 0.0f); 
  return !((bD0 == (rclFacet._aclPoints[1].DistanceToPlane(rclBase, rclNormal) > 0.0f)) &&
           (bD0 == (rclFacet._aclPoints[2].DistanceToPlane(rclBase, rclNormal) > 0.0f)));
}


}; // namespace Mesh 

#endif  // MESH_ALGORITHM_H 
