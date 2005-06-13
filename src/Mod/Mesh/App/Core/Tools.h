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


#ifndef MESH_TOOLS_H
#define MESH_TOOLS_H

#include "MeshKernel.h"
#include "Algorithm.h"
#include <Wm3Sphere3.h>
#include <Wm3Triangle3.h>

namespace Mesh {

/**
 * Helper class to get distances between point and mesh.
 */
class MeshFacetTools
{
public:
  MeshFacetTools (const MeshKernel &rclM);
  virtual ~MeshFacetTools () {}
  /** Returns true if the distance from the \a rclPt to the facet \a ulFacetIdx is less than \a fMaxDistance.
   * If this restriction is met \a rfDistance is set to the actual distance, otherwise false is returned.
   */
  inline bool Distance (const Vector3D &rclPt, unsigned long ulFacetIdx, float fMaxDistance, float &rfDistance) const;

protected:
  const MeshKernel      &_rclMeshBase; /**< The mesh kernel. */
  const MeshFacetArray  &_rclFAry; /**< The facet array. */
  const MeshPointArray  &_rclPAry; /**< The point array. */

private:
  MeshFacetTools (const MeshFacetTools&);
  void operator = (const MeshFacetTools&);
};

bool MeshFacetTools::Distance (const Vector3D &rclPt, unsigned long ulFacetIdx, float fMaxDistance, float &rfDistance) const
{
  const unsigned long *pulIdx = _rclFAry[ulFacetIdx]._aulPoints;

  BoundBox3D clBB;
  clBB &= _rclPAry[*(pulIdx++)];
  clBB &= _rclPAry[*(pulIdx++)];
  clBB &= _rclPAry[*pulIdx];
  clBB.Enlarge(fMaxDistance);

  if (clBB.IsInBox(rclPt) == false)
    return false;

  rfDistance = MeshFacetFunc::DistanceToPoint(_rclMeshBase.GetFacet(ulFacetIdx), rclPt);

  return rfDistance < fMaxDistance;
}

/**
 * The MeshSearchNeighbours class provides methods to get all points
 * in the neighbourhood of a given facet.
 */
class MeshSearchNeighbours 
{
public:
  MeshSearchNeighbours ( MeshKernel &rclM, float fSampleDistance = 1.0f);
  virtual ~MeshSearchNeighbours () {}
  /** Re-initilaizes internal structures. */
  void Reinit (float fSampleDistance);
  /** Collects all neighbour points from the facet (by index), the result are the points of the facets lying
   * inside a sphere of radius \a fDistance, center \a center of the original facet. This method uses the 
   * MARKED flags.
   */
  unsigned long  NeighboursFromFacet (unsigned long ulFacetIdx, float fDistance, unsigned long ulMinPoints, std::vector<Vector3D> &raclResultPoints);
  /** Searches for facets from the start facet, sample the neighbour facets and accumulates the points. */
  unsigned long  NeighboursFromSampledFacets (unsigned long ulFacetIdx, float fDistance, std::vector<Vector3D> &raclResultPoints);
  /** Searches for facets from the start facet. */
  unsigned long  NeighboursFacetFromFacet (unsigned long ulFacetIdx, float fDistance, std::vector<Vector3D> &raclResultPoints, std::vector<unsigned long> &raclResultFacets);

protected:
  /** Subsamples the mesh. */
  void SampleAllFacets (void);
  inline bool CheckDistToFacet (const MeshFacet &rclF);     // check distance to facet, add points inner radius
  bool AccumulateNeighbours (const MeshFacet &rclF, unsigned long ulFIdx); // accumulate the sample neighbours facet
  inline bool InnerPoint (const Vector3D &rclPt) const;
  inline bool TriangleCutsSphere (const MeshFacet &rclF) const;
  bool ExpandRadius (unsigned long ulMinPoints);

  struct CDistRad : public std::binary_function<const Vector3D&, const Vector3D&, bool>
  {
    CDistRad (const Vector3D clCenter) : _clCenter(clCenter) {}
    bool operator()(const Vector3D &rclPt1, const Vector3D &rclPt2) { return DistanceP2(_clCenter, rclPt1) < DistanceP2(_clCenter, rclPt2); }
    Vector3D  _clCenter;
  };

protected:
  MeshKernel  &_rclMesh;
  MeshFacetArray &_rclFAry;
  MeshPointArray &_rclPAry;
  MeshRefPointToFacets _clPt2Fa;
  float _fMaxDistanceP2;   // square distance 
  Vector3D _clCenter;         // center points of start facet
  std::set<unsigned long> _aclResult;        // result container (point indices)
  std::set<unsigned long> _aclOuter;         // next searching points
  std::vector<Vector3D> _aclPointsResult;  // result as vertex
  std::vector<std::vector<Vector3D> > _aclSampledFacets; // sample points from each facet
  float _fSampleDistance;  // distance between two sampled points
  Wm3::Sphere3<float> _clMgcSphere;
  bool _bTooFewPoints;    

private:
  MeshSearchNeighbours (const MeshSearchNeighbours&);
  void operator = (const MeshSearchNeighbours&);
};

inline bool MeshSearchNeighbours::CheckDistToFacet (const MeshFacet &rclF)
{
  bool bFound = false;

  for (int i = 0; i < 3; i++)
  {
    unsigned long ulPIdx = rclF._aulPoints[i];
    if (_rclPAry[ulPIdx].IsFlag(MeshPoint::MARKED) == false)
    {
      if (DistanceP2(_clCenter, _rclPAry[ulPIdx]) < _fMaxDistanceP2)
      {
        bFound = true;
        {
          _aclResult.insert(ulPIdx);
          _rclPAry[ulPIdx].SetFlag(MeshPoint::MARKED);
        }
      }
      _aclOuter.insert(ulPIdx);
    }
  }

  return bFound;
}

inline bool MeshSearchNeighbours::InnerPoint (const Vector3D &rclPt) const
{
  return DistanceP2(_clCenter, rclPt) < _fMaxDistanceP2;
}

/// @todo
inline bool MeshSearchNeighbours::TriangleCutsSphere (const MeshFacet &rclF) const
{/*
  Vector3<float> akP0 = _rclPAry[rclF._aulPoints[0]];
  Vector3<float> akP1 = _rclPAry[rclF._aulPoints[1]];
  Vector3<float> akP2 = _rclPAry[rclF._aulPoints[2]];

  Wm3::Triangle3<float> clTri(akP0, akP1, akP2);

  return MgcTestIntersection(clTri, _clMgcSphere);*/
  return false;
}

} // namespace Mesh


#endif  // MESH_TOOLS_H

