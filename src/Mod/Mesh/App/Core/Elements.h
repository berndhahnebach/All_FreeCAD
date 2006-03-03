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


#ifndef MESH_ELEMENTS_H
#define MESH_ELEMENTS_H

#ifndef _PreComp_
# include <vector>
#endif

#include "Definitions.h"

#include <Base/BoundBox.h>

namespace Base{
  class Vector3D;
}
using Base::Vector3D;
using Base::BoundBox3D;


namespace MeshCore {

class MeshHelpEdge;
class MeshPoint;

/**
 * Helper class providing an operator for comparison 
 * of two edges. The class holds the point indices of the
 * underlying edge.
 */
class AppMeshExport MeshHelpEdge
{
public:
  inline bool operator == (const MeshHelpEdge &rclEdge) const;
  unsigned long   _ulIndex[2];  // point indices
};

/**
 * Structure that holds the facet index with the two corner point
 * indices of the facet's orientation this edge is attached to.
 */
class AppMeshExport MeshIndexEdge
{
public:
  unsigned long  _ulFacetIndex;  // Facet index
  unsigned short _ausCorner[2];  // corner point indices of the facet
};

/**
 * The MeshPoint class represents a point in the mesh data structure. The class inherits from
 * Vector3D and provides some additional information such as flag state and property value.
 * The flags can be modified by the Set() and Reset() and queried by IsFlag().
 * A point can temporary be in an invalid state (e.g during deletion of several points), but
 * must not be set in general, i.e. always usable within a mesh-internal algorithm.
 */
class AppMeshExport MeshPoint: public Base::Vector3D
{
public:
  enum TFlagType {INVALID=1, VISIT=2, SEGMENT=4, MARKED=8, REV1=16, REV2=32, TMP0=64, TMP1=128};

  /** @name Construction */
  //@{
  MeshPoint (void) : _ucFlag(0), _ulProp(0) { }
  inline MeshPoint (const Vector3D &rclPt);
  inline MeshPoint (const MeshPoint &rclPt);
  ~MeshPoint (void) { }
  //@}

public:
  /** @name Flag state */
  //@{
  void SetFlag (TFlagType tF)
  { _ucFlag |= (unsigned char)(tF); }
  void ResetFlag (TFlagType tF)
  { _ucFlag &= ~(unsigned char)(tF); }
  bool IsFlag (TFlagType tF) const
  { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF);  }
  void ResetInvalid (void)
  { ResetFlag(INVALID); }
  void  SetInvalid (void)
  { SetFlag(INVALID); }
  bool IsValid (void) const
  { return !IsFlag(INVALID); }
  //@}

  // Assignment
  inline MeshPoint& operator = (const MeshPoint &rclPt);

  // compare operator
  inline bool operator == (const MeshPoint &rclPt) const;
  inline bool operator == (const Vector3D &rclV) const;
  inline bool operator < (const MeshPoint &rclPt) const;

public:
  unsigned char _ucFlag; /**< Flag member */
  unsigned long   _ulProp; /**< Free usable property */
};

/**
 * The MeshGeomEdge class is geometric counterpart to MeshEdge that holds the 
 * geometric data points of an edge.
 */
class AppMeshExport MeshGeomEdge
{
public:
  MeshGeomEdge (void) : _bBorder(false) {}

  /** Checks if the edge is inside the bounding box or intersects with it. */
  bool ContainedByOrIntersectBoundingBox (const BoundBox3D &rclBB ) const;
  /** Returns the bounding box of the edge. */
  BoundBox3D GetBoundBox () const;
  /** Checks if the edge intersects with the given bounding box. */
  bool IntersectBoundingBox (const BoundBox3D &rclBB) const;

public:
  Vector3D _aclPoints[2];  /**< Corner points */
  bool     _bBorder;       /**< Set to true if border edge */
};

/**
 * The MeshFacet class represent a triangle facet in the mesh data.structure. A facet indexes
 * three neighbour facets and also three corner points.
 * This class only keeps topologic information but no geometric information at all.
 * 
 * Here are the most important conventions concerning the facet's orientation:
 * \li neighbour or edge number of 0 is defined by corner 0 and 1
 * \li neighbour or edge number of 1 is defined by corner 1 and 2
 * \li neighbour or edge number of 2 is defined by corner 2 and 0
 * \li neighbour index is set to ULONG_MAX if there is no neighbour facet
 */
class MeshFacet
{
public:
  enum TFlagType {INVALID=1, VISIT=2, SEGMENT=4, MARKED=8, REV1=16, REV2=32, TMP0=64, TMP1=128};

public:
  /** @name Construction */
  //@{
  inline MeshFacet (void);
  inline MeshFacet(const MeshFacet &rclF);
  ~MeshFacet (void) { }
  //@}

  /** @name Flag state */
  //@{
  void SetFlag (TFlagType tF)
  { _ucFlag |= (unsigned char)(tF); }
  void ResetFlag (TFlagType tF)
  { _ucFlag &= ~(unsigned char)(tF); }
  bool IsFlag (TFlagType tF) const
  { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF); }
  void ResetInvalid (void)
  { ResetFlag(INVALID); }
  /**
   * Marks a facet as invalid. Should be used only temporary from within an algorithm
   * (e.g. deletion of several facets) but must not be set permanently.
   * From outside the data-structure must not have invalid facets.
   */
  void  SetInvalid (void)
  { SetFlag(INVALID); }
  bool IsValid (void) const
  { return !IsFlag(INVALID); }
  //@}

  // Assignment
  inline MeshFacet& operator = (const MeshFacet &rclF);

  /**
   * Returns the indices of the corner points of the given edge number. 
   */
  inline void GetEdge (unsigned short usSide, MeshHelpEdge &rclEdge) const;
  /**
   * Returns the indices of the corner points of the given edge number. 
   */
  inline std::pair<unsigned long, unsigned long> GetEdge (unsigned short usSide) const;
  /**
   * Returns the edge-number to the given index of neighbour facet.
   * If \a ulNIndex is not a neighbour USHRT_MAX is returned.
   */
  inline unsigned short Side (unsigned long ulNIndex) const;
  /**
   * Returns the edge-number defined by two points. If one point is
   * not a corner point USHRT_MAX is returned.
   */
  inline unsigned short Side (unsigned long ulP0, unsigned long P1) const;
  /**
   * Replaces the index of the corner point that is equal to \a ulOrig
   * by \a ulNew. If the facet does not have a corner point with this index
   * nothing happens.
   */
  inline void Transpose (unsigned long ulOrig, unsigned long ulNew);
  /**
   * Decrement the index for each corner point that is higher than \a ulIndex.
   */
  inline void Decrement (unsigned long ulIndex);
  /**
   * Replaces the index of the neighbour facet that is equal to \a ulOrig
   * by \a ulNew. If the facet does not have a neighbourt with this index
   * nothing happens.
   */
  inline void ReplaceNeighbour (unsigned long ulOrig, unsigned long ulNew);
  /**
   * Checks if the neighbour exists at the given edge-number.
   */
  bool HasNeighbour (unsigned short usSide) const
  { return (_aulNeighbours[usSide] != ULONG_MAX); }
  /** Flips the orientation of the facet. The edge array must be adjusted. */
  void FlipNormal (void)
  {
    std::swap(_aulPoints[1], _aulPoints[2]);
    std::swap(_aulNeighbours[0], _aulNeighbours[2]);
  }

public:
  unsigned char _ucFlag; /**< Flag member. */
  unsigned long _ulProp; /**< Free usable property. */
  unsigned long _aulPoints[3];     /**< Indices of corner points. */
  unsigned long _aulNeighbours[3]; /**< Indices of neighbour facets. */
};

/**
 * The MeshGeomFacet class is geometric counterpart to MeshFacet that holds the 
 * geometric data points of a triangle.
 */
class AppMeshExport MeshGeomFacet 
{
public:
  /** @name Construction */
  //@{
  /// default constructor
  MeshGeomFacet (void); 
  /// Constructor with the corner points
  MeshGeomFacet (const Vector3D &v1,const Vector3D &v2,const Vector3D &v3);
   /// Destruction
  ~MeshGeomFacet (void) { }
  //@}

public:
  /**
   * Checks if the point is part of the facet. A point is regarded as part
   * of a facet if the distance is lower \s fDistance and the projected point
   * in the facet normal direction is inside the triangle.
   */
  bool IsPointOf (const Vector3D &rclPoint, float fDistance) const;
  /** Checks whether the given point is inside the facet with tolerance \a fDistance. 
   * This method does actually the same as IsPointOf() but this implementation 
   * is done more effective through comparison of normals.
   */
  bool IsPointOfFace (const Vector3D& rclP, float fDistance) const;
  /** Calculates the weights \a w1, ...,  \a w3 of the corners to get the point \a rclP, i.e.
   * rclP = w0*v0 + w1*v1 + w2*v2 (v0-v2 are the corners corners).
   * If w0+w1+w2==1.0 then the point rclP lies on the plane that is spanned by the facet, otherwise
   * the point doesn't lie on the plane. 
   * If the sum of wi is 1 and if each wi is between [0,1] than the point lies inside
   * the facet or on the border, respectively.
   *
   * If the point doesn't lie on the plane false is returned, true otherwise.
   */
  bool Weights(const Vector3D& rclP, float& w0, float& w1, float& w2) const;
  /**
   * Calculates the distance of a point to the triangle.
   */
  inline float Distance (const Vector3D &rclPoint) const;
  /**
   * Enlarges the triangle.
   */
  void Enlarge (float fDist);
  /**
   * Calculates the facet normal for storing internally.
   */
  inline void CalcNormal (void);
  /**
   * Arrange the facet normal so the both vectors have the same orientation.
   */
  inline void ArrangeNormal (const Vector3D &rclN);
  /**
   * Adjusts the facet's orientation to its normal.
   */
  inline void AdjustCirculationDirection (void);
  /** Checks if the normal is not yet calculated. */
  void NormalInvalid (void) { _bNormalCalculated = false; }
  /** Query the flag state of the facet. */
  bool IsFlag (MeshFacet::TFlagType tF) const
  { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF); }
    /** Set flag state */
  void SetFlag (MeshFacet::TFlagType tF)
  { _ucFlag |= (unsigned char)(tF); }
  /** Reset flag state */
  void ResetFlag (MeshFacet::TFlagType tF)
  { _ucFlag &= ~(unsigned char)(tF); }
  /** Calculates the facet's gravity point. */
  inline Vector3D GetGravityPoint (void) const;
  /** Returns the normal of the facet. */
  inline Vector3D GetNormal (void) const;
  /** Sets the facet's normal. */
  inline void SetNormal (const Vector3D &rclNormal);
  /** Returns the wrapping bounding box. */
  inline BoundBox3D GetBoundBox (void) const;
  /** Calculates the area of a facet. */
  inline float Area () const;
  /** Checks if the facet is inside the bounding box or intersects with it. */
  inline bool ContainedByOrIntersectBoundingBox (const BoundBox3D &rcBB) const;
  /** Checks if the facet intersects with the given bounding box. */
  bool IntersectBoundingBox ( const BoundBox3D &rclBB ) const;
  /** This method projects the second facet onto the plane defined by this facet and checks then
   * if both facets intersects.
   */
  bool IntersectWithProjectedFacet(const MeshGeomFacet &rclFacet) const;
  /** This method checks if both facets intersects.
   */
  bool IntersectWithFacet(const MeshGeomFacet &rclFacet) const;
   /**
   * Intersect the facet with the other facet
   * The result is line given by two points (if intersected).
   * Return True if the two facets has a intersections otherwise false
   */
  bool IntersectWithFacet (const MeshGeomFacet& facet, Vector3D& rclPt0, Vector3D& rclPt1) const;
  /** Calculates the shortest distance from the line segment defined by \a rcP1 and \a rcP2 to
   * this facet.
   */
  float DistanceToLineSegment (const Vector3D &rcP1, const Vector3D &rcP2) const;
  /** Calculates the shortest distance from the point \a rcPt to the facet. */
  float DistanceToPoint (const Vector3D &rcPt) const
  { Base::Vector3D res; return DistanceToPoint(rcPt, res); }
  /** Calculates the shortest distance from the point \a rcPt to the facet. \a rclNt is the point of the facet
   * with shortest distance.
   */
  float DistanceToPoint  ( const Vector3D &rclPt, Base::Vector3D& rclNt ) const;
  /** Calculates the intersection point of the line defined by the base \a rclPt and the direction \a rclDir
   * with the facet. The intersection must be inside the facet. If there is no intersection false is returned.
   */
  bool IntersectWithLine (const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes) const;
  /** Calculates the intersection point of the line defined by the base \a rclPt and the direction \a rclDir
   * with the facet. The intersection must be inside the facet. If there is no intersection false is returned.
   * This does actually the same as IntersectWithLine() with one additionally constraint that the angle 
   * between the direction of the line and the normal of the plane must not exceed \a fMaxAngle.
   */
  bool Foraminate (const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes, float fMaxAngle = F_PI) const;
  /** Checks if the facet intersects with the plane defined by the base \a rclBase and the normal 
   * \a rclNormal and returns true if two points are found, false otherwise.
   */
  bool IntersectWithPlane (const Vector3D &rclBase, const Vector3D &rclNormal, Vector3D &rclP1, Vector3D &rclP2) const;
  /**
   * Checks if the facet intersects with the plane defined by the base \a rclBase and the normal
   * \a rclNormal.
   */
  inline bool IntersectWithPlane (const Vector3D &rclBase, const Vector3D &rclNormal) const;
  /** Checks if the plane defined by the facet \a rclFacet intersects with the line defined by the base
   * \a rclBase and the direction \a rclNormal and returns the intersection point \a rclRes if possible. 
   */
  bool IntersectPlaneWithLine (const Vector3D &rclBase, const Vector3D &rclNormal, Vector3D &rclRes ) const;
  /** Calculates the volume of the prism defined by two facets. 
   * \note The two facets must not intersect.
   */
  float VolumeOfPrism (const MeshGeomFacet& rclF) const;
  /** Subsamples the facet into points with resolution \a fStep. */
  void SubSample (float fStep, std::vector<Vector3D> &rclPoints) const;
  /** Calculates the center and radius of the inner circle of the facet. */
  float CenterOfInnerCircle(Vector3D& rclCenter) const;
  /** Calculates the center and radius of the outer circle of the facet. */
  float CenterOfOuterCircle(Vector3D& rclCenter) const;
  /** Returns the edge number of the facet that is nearest to the point \a rclPt. */
  unsigned short NearestEdgeToPoint(const Vector3D& rclPt) const;

protected:
  Vector3D  _clNormal; /**< Normal of the facet. */
  bool  _bNormalCalculated; /**< True if the normal is already calculated. */

public:
  Vector3D  _aclPoints[3]; /**< Geometric corner points. */
  unsigned char _ucFlag; /**< Flag property */
  unsigned long   _ulProp; /**< Free usable property. */
};

typedef  std::vector<MeshPoint>  TMeshPointArray;
/**
 * Stores all data points of the mesh structure.
 */
class MeshPointArray: public TMeshPointArray
{
public:
  // Iterator interface
  typedef std::vector<MeshPoint>::iterator        _TIterator;
  typedef std::vector<MeshPoint>::const_iterator  _TConstIterator;

  /** @name Construction */
  //@{
  // constructor
  MeshPointArray (void) { }
  // constructor
  MeshPointArray (unsigned long ulSize) : TMeshPointArray(ulSize) { }
  // Destructor
  ~MeshPointArray (void) { }
  //@}

  /** @name Flag state */
  //@{
  /// Sets the flag for all points
  void SetFlag (MeshPoint::TFlagType tF);
  /// Resets the flag for all points
  void ResetFlag (MeshPoint::TFlagType tF);
  /// Sets all points invalid
  void ResetInvalid (void);
  /// Sets the property for all points
  void SetProperty (unsigned long ulVal);
  //@}

  // Assignment
  MeshPointArray& operator = (const MeshPointArray &rclPAry);
  /**
   * Searches for the first point index  Two points are equal if the distance is less
   * than FLOAT_EPS. If no such points is found ULONG_MAX is returned. 
   */
  unsigned long Get (const MeshPoint &rclPoint);
  /**
   * Searches for the first point index  Two points are equal if the distance is less
   * than FLOAT_EPS. If no such points is found the point is added to the array at end
   * and its index is returned. 
   */
  unsigned long GetOrAddIndex (const MeshPoint &rclPoint);
};

typedef std::vector<MeshFacet>  TMeshFacetArray;

/**
 * Stores all facets of the mesh data-structure.
 */
class AppMeshExport MeshFacetArray: public TMeshFacetArray
{
public:
  // Iterator interface
  typedef std::vector<MeshFacet>::iterator        _TIterator;
  typedef std::vector<MeshFacet>::const_iterator  _TConstIterator;

public:
  /** @name Construction */
  //@{
  /// constructor
  MeshFacetArray (void) { }
  /// constructor
  MeshFacetArray (unsigned long ulSize) : TMeshFacetArray(ulSize) { }
  /// destructor
  ~MeshFacetArray (void) { }
  //@}

  /** @name Flag state */
  //@{
  /// Sets the flag for all facets. 
  void SetFlag (MeshFacet::TFlagType tF);
  /// Reets the flag for all facets. 
  void ResetFlag (MeshFacet::TFlagType tF);
  /// Sets all facets invalid
  void ResetInvalid (void);
  /// Sets the property for all facets
  void SetProperty (unsigned long ulVal);
  //@}

  // Assignment
  MeshFacetArray& operator = (const MeshFacetArray &rclFAry);

  /**
   * Removes the facet from the array the iterator points to. All neighbour
   * indices of the other facets get adjusted.
   */
  void Erase (_TIterator pIter);
  /**
   * Checks and flips the point indices if needed. @see MeshFacet::Transpose().
   */
  void TransposeIndicies (unsigned long ulOrig, unsigned long ulNew);
  /**
   * Decrements all point indices that are higher than \a ulIndex.
   */
  void DecrementIndicies (unsigned long ulIndex);
};

inline MeshPoint::MeshPoint (const Vector3D &rclPt)
: Vector3D(rclPt),
  _ucFlag(0),
  _ulProp(0)
{
}

inline MeshPoint::MeshPoint (const MeshPoint &rclPt)
: Vector3D(rclPt),
  _ucFlag(rclPt._ucFlag),
  _ulProp(rclPt._ulProp)
{
}

inline MeshPoint& MeshPoint::operator = (const MeshPoint &rclPt)
{
  Vector3D::operator=(rclPt);
  _ucFlag = rclPt._ucFlag;
  _ulProp = rclPt._ulProp;
  return *this;
}

inline bool MeshPoint::operator == (const MeshPoint &rclPt) const
{
  return Base::DistanceP2(*this, rclPt) < MeshDefinitions::_fMinPointDistanceP2;
}

inline bool MeshPoint::operator == (const Vector3D &rclV) const
{
  return Base::DistanceP2(*this, rclV) < MeshDefinitions::_fMinPointDistanceP2;
}

inline bool MeshPoint::operator < (const MeshPoint &rclPt) const
{
  if (fabs(x - rclPt.x) < MeshDefinitions::_fMinPointDistanceD1)
  {
    if (fabs(y - rclPt.y) < MeshDefinitions::_fMinPointDistanceD1)
    {
      if (fabs(z - rclPt.z) < MeshDefinitions::_fMinPointDistanceD1)
        return false;
      else
        return z < rclPt.z;
    }
    else
      return y < rclPt.y;
  }
  else
    return x < rclPt.x;
}

inline float MeshGeomFacet::Distance (const Vector3D &rclPoint) const
{
  Vector3D clNorm(_clNormal);
  clNorm.Normalize();
  return float(fabs(rclPoint.DistanceToPlane(_aclPoints[0], clNorm)));
}

inline void MeshGeomFacet::CalcNormal (void)
{
  _clNormal = (_aclPoints[1] - _aclPoints[0]) % (_aclPoints[2] - _aclPoints[0]);
  _clNormal.Normalize();
  _bNormalCalculated = true;
}

inline Vector3D MeshGeomFacet::GetNormal (void) const
{
  if (_bNormalCalculated == false)
    const_cast<MeshGeomFacet*>(this)->CalcNormal();

  return _clNormal;
}

inline void MeshGeomFacet::SetNormal (const Vector3D &rclNormal)
{
  _clNormal = rclNormal;
  _bNormalCalculated = true;
}

inline void MeshGeomFacet::ArrangeNormal (const Vector3D &rclN)
{
  if ((rclN * _clNormal) < 0.0f)
    _clNormal = -_clNormal;
}

inline Vector3D MeshGeomFacet::GetGravityPoint (void) const
{
  return (1.0f / 3.0f) * (_aclPoints[0] + _aclPoints[1] + _aclPoints[2]);
}

inline void MeshGeomFacet::AdjustCirculationDirection (void)
{
  Vector3D clN = (_aclPoints[1] - _aclPoints[0]) % (_aclPoints[2] - _aclPoints[0]);
  if ((clN * _clNormal) < 0.0f)
    std::swap(_aclPoints[1], _aclPoints[2]);
}

inline BoundBox3D MeshGeomFacet::GetBoundBox (void) const
{
  return BoundBox3D(_aclPoints, 3);
}

inline float MeshGeomFacet::Area () const
{
  return ((_aclPoints[1] - _aclPoints[0]) % (_aclPoints[2] - _aclPoints[0])).Length() / 2.0f;
}

inline bool MeshGeomFacet::ContainedByOrIntersectBoundingBox ( const BoundBox3D &rclBB ) const
{
  // Test, ob alle Eckpunkte des Facets sich auf einer der 6 Seiten der BB befinden
  if ((GetBoundBox() && rclBB) == false)
    return false;

  // Test, ob Facet-BB komplett in BB liegt
  if (rclBB.IsInBox(GetBoundBox()))
    return true;

  // Test, ob einer der Eckpunkte in BB liegt
  for (int i=0;i<3;i++)
  {
    if (rclBB.IsInBox(_aclPoints[i]))
      return true;
  }

  // "echter" Test auf Schnitt
  if (IntersectBoundingBox(rclBB))
    return true;

  return false;
}

inline bool MeshGeomFacet::IntersectWithPlane (const Vector3D &rclBase, const Vector3D &rclNormal) const
{
  bool bD0 = (_aclPoints[0].DistanceToPlane(rclBase, rclNormal) > 0.0f); 
  return !((bD0 == (_aclPoints[1].DistanceToPlane(rclBase, rclNormal) > 0.0f)) &&
           (bD0 == (_aclPoints[2].DistanceToPlane(rclBase, rclNormal) > 0.0f)));
}

inline MeshFacet::MeshFacet (void)
: _ucFlag(0),
  _ulProp(0)
{
  memset(_aulNeighbours, ULONG_MAX, sizeof(ULONG_MAX) * 3);
  memset(_aulPoints, ULONG_MAX, sizeof(ULONG_MAX) * 3);
}

inline MeshFacet::MeshFacet(const MeshFacet &rclF)
: _ucFlag(rclF._ucFlag),
  _ulProp(rclF._ulProp)
{
  _aulPoints[0] = rclF._aulPoints[0];
  _aulPoints[1] = rclF._aulPoints[1];
  _aulPoints[2] = rclF._aulPoints[2];

  _aulNeighbours[0] = rclF._aulNeighbours[0];
  _aulNeighbours[1] = rclF._aulNeighbours[1];
  _aulNeighbours[2] = rclF._aulNeighbours[2];
}

inline MeshFacet& MeshFacet::operator = (const MeshFacet &rclF)
{
  _ucFlag          = rclF._ucFlag;
  _ulProp          = rclF._ulProp;

  _aulPoints[0]    = rclF._aulPoints[0];
  _aulPoints[1]    = rclF._aulPoints[1];
  _aulPoints[2]    = rclF._aulPoints[2];

  _aulNeighbours[0] = rclF._aulNeighbours[0];
  _aulNeighbours[1] = rclF._aulNeighbours[1];
  _aulNeighbours[2] = rclF._aulNeighbours[2];

  return *this;
}

inline void MeshFacet::GetEdge (unsigned short usSide, MeshHelpEdge &rclEdge) const
{
  rclEdge._ulIndex[0] = _aulPoints[usSide];
  rclEdge._ulIndex[1] = _aulPoints[(usSide+1) % 3];
}

inline std::pair<unsigned long, unsigned long> MeshFacet::GetEdge (unsigned short usSide) const
{
  return std::pair<unsigned long, unsigned long>(_aulPoints[usSide], _aulPoints[(usSide+1)%3]);
}

inline void MeshFacet::Transpose (unsigned long ulOrig, unsigned long ulNew)
{
  if (_aulPoints[0] == ulOrig)
    _aulPoints[0] = ulNew;
  else if (_aulPoints[1] == ulOrig)
    _aulPoints[1] = ulNew;
  else if (_aulPoints[2] == ulOrig)
    _aulPoints[2] = ulNew;
}

inline void MeshFacet::Decrement (unsigned long ulIndex)
{
  if (_aulPoints[0] > ulIndex) _aulPoints[0]--;
  if (_aulPoints[1] > ulIndex) _aulPoints[1]--;
  if (_aulPoints[2] > ulIndex) _aulPoints[2]--;
}

inline void MeshFacet::ReplaceNeighbour (unsigned long ulOrig, unsigned long ulNew)
{
  if (_aulNeighbours[0] == ulOrig)
    _aulNeighbours[0] = ulNew;
  else if (_aulNeighbours[1] == ulOrig)
    _aulNeighbours[1] = ulNew;
  else if (_aulNeighbours[2] == ulOrig)
    _aulNeighbours[2] = ulNew;
}

inline unsigned short MeshFacet::Side (unsigned long ulNIndex) const
{
  if (_aulNeighbours[0] == ulNIndex)
    return 0;
  else if (_aulNeighbours[1] == ulNIndex)
    return 1;
  else if (_aulNeighbours[2] == ulNIndex)
    return 2;
  else
    return USHRT_MAX;
}

inline unsigned short MeshFacet::Side (unsigned long ulP0, unsigned long ulP1) const
{
  if (_aulPoints[0] == ulP0)
  {
    if (_aulPoints[1] == ulP1)
      return 0;  // Kante 0-1 ==> 0
    else if (_aulPoints[2] == ulP1)
      return 2;  // Kante 0-2 ==> 2
  }
  else if (_aulPoints[1] == ulP0)
  {
    if (_aulPoints[0] == ulP1)
      return 0; // Kante 1-0 ==> 0
    else if (_aulPoints[2] == ulP1)
      return 1; // Kante 1-2 ==> 1
  }
  else if (_aulPoints[2] == ulP0)
  {
    if (_aulPoints[0] == ulP1)
      return 2; // Kante 2-0 ==> 2
    else if (_aulPoints[1] == ulP1)
      return 1; // Kante 2-1 ==> 1
  }
  
  return USHRT_MAX;
}

/**
 * Binary function to query the flags for use with generic STL functions..
 */
template <class TCLASS>
class MeshIsFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
public:
  bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
  { return rclElem.IsFlag(tFlag); }
};

/**
 * Binary function to query the flags for use with generic STL functions..
 */
template <class TCLASS>
class MeshIsNotFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
public:
  bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
  { return !rclElem.IsFlag(tFlag); }
};

/**
 * Binary function to set the flags for use with generic STL functions..
 */
template <class TCLASS>
class MeshSetFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
public:
  bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
  { rclElem.SetFlag(tFlag); return true; }
};

/**
 * Binary function to reset the flags for use with generic STL functions..
 */
template <class TCLASS>
class MeshResetFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
public:
  bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
  { rclElem.ResetFlag(tFlag); return true; }
};

} // namespace MeshCore

#endif // MESH_ELEMENTS_H 
