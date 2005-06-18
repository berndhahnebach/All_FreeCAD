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


#ifndef VECTOR3D_H
#define VECTOR3D_H


#ifndef _PreComp_
# include <math.h>
#endif

#include "Definitions.h"
#include "Stream.h"

namespace Mesh {

class   Vector3D;
typedef Vector3D * PVector3D;
typedef Vector3D & RVector3D;

/** The mesh vector class. */
class AppMeshExport Vector3D
{
public:
  /** @name Public data members */
  //@{
  float x; /**< x-coordinate */
  float y; /**< y-coordinate */
  float z; /**< z-coordinate */
  //@}
  
  /// Construction
  inline explicit Vector3D (float fx = 0.0f, float fy = 0.0f, float fz = 0.0f);
  /// Construction
  inline Vector3D (const Vector3D& rcVct);

  /** @name Operator */
  //@{
  /// Returns a reference to a coordinate. \a usIndex must be in the range [0,2]
  inline float & operator [] (unsigned short usIndex);
  /// Vector addition
  inline Vector3D operator +  (const Vector3D& rcVct) const;
  inline Vector3D operator &  (const Vector3D& rcVct) const;
  /// Vector subtraction
  inline Vector3D operator -  (const Vector3D& rcVct) const;
  /// Negative vector
  inline Vector3D operator - (void) const;
  /// Vector summation
  inline RVector3D operator += (const Vector3D& rcVct);
  /// Vector subtraction
  inline RVector3D operator -= (const Vector3D& rcVct);
  /// Vector scaling
  inline RVector3D operator *= (float fScale);
  /// Vector scaling
  inline Vector3D operator *   (float fScale);
  /// Assignment
  inline RVector3D operator =  (const Vector3D& rcVct);
  /// Scalar product
  inline float operator *  (const Vector3D& rcVct) const;
  /// Vector scaling
  inline RVector3D operator / (float fDiv); 
  /// Cross product
  inline Vector3D operator %  (const Vector3D& rcVct) const;
  /// Comparing for inequality
  inline bool operator != (const Vector3D& rcVct) const;
  /// Comparing for equality
  inline bool operator == (const Vector3D& rcVct) const;
  //@}

  // I/O streaming
  DataStream & SaveData (DataStream & ofs);
  DataStream & LoadData (DataStream & ifs);

  /** @name Modification */
  //@{
  void ScaleX (float f);
  void ScaleY (float f);
  void ScaleZ (float f);
  void Scale (float fX, float fY, float fZ);
  void MoveX (float f);
  void MoveY (float f);
  void MoveZ (float f);
  void Move (float fX, float fY, float fZ);
  void RotateX (float f);
  void RotateY (float f);
  void RotateZ (float f);
  //@}

  inline void Set (float fX, float fY, float fZ);
  inline void Set (double dX, double dY, double dZ);

  void  Print (void);
  unsigned long GetMemSpace (void);

  /** @name Mathematics */
  //@{
  /// Length of the vector.
  inline float Length (void) const;
  /// Squared length of the vector.
  inline float Sqr (void) const;
  /// Set length to 1.
  RVector3D Normalize (void);
  /// Get angle between both vectors.
  float GetAngle (const Vector3D &rcVect) const;
  /** Transforms this point to the coordinate system defined by origin \a rclBase, 
   * vector \a vector rclDirX and vector \a vector rclDirY. 
   * \note \a rclDirX must be perpendicular to \a rclDirY, i.e. \a rclDirX * \a rclDirY = 0..
   */
  void TransformToCoordinateSystem (const Vector3D &rclBase, const Vector3D &rclDirX, const Vector3D &rclDirY);
  bool Equal(const Vector3D &rclVect) const;
  /// Projects this point onto the plane given by the base \a rclBase and the normal \a rclNorm.
  inline RVector3D ProjToPlane (const Vector3D &rclBase, const Vector3D &rclNorm);
  /// Projects this point onto the line given by the base \a rclPoint and the direction \a rclLine.
  /**
   * Projects a point \a rclPoint onto the line defined by the origin and the direction \a rclLine.
   * The result is a vector from \a rclPoint to the point on the line. The length of this vector 
   * is the distance from \a rclPoint to the line.
   * Note: The resulting vector does not depend on the current vector.
   */
  inline RVector3D ProjToLine (const Vector3D &rclPoint, const Vector3D &rclLine);
  /** Computes the distance to the given plane. Depending on the side this point is located
   * the distance can also be negative. The distance is positive if the point is at the same
   * side the plane normal points to, negative otherwise.
   */
  inline float DistanceToPlane (const Vector3D &rclBase, const Vector3D &rclNorm) const;
  /// Computes the distance from this point to the line given by \a rclBase and \a rclDirect.
  inline float DistanceToLine (const Vector3D &rclBase, const Vector3D &rclDirect) const;
  //@}

  /** @name Friends */
  //@{
  /// Returns the distance between two points
  friend float Distance (const Vector3D &rcVct1, const Vector3D &rcVct2);
  /// Returns the squared distance between two points
  friend float DistanceP2 (const Vector3D &rclV1, const Vector3D &rclV2);
  /// Multiplication of scalar with vector.
  friend Vector3D operator * (float fFac, const Vector3D &rcVct);
  //@}
};

inline Vector3D::Vector3D (float fx, float fy, float fz)
: x(fx),
  y(fy),
  z(fz)
{
}

inline Vector3D::Vector3D (const Vector3D& rcVct)
: x(rcVct.x),
  y(rcVct.y),
  z(rcVct.z)
{
}

inline float& Vector3D::operator [] (unsigned short usIndex)
{
  switch (usIndex)
  {
    case 0: return x;
    case 1: return y;
    case 2: return z;
  }
  return x;
}

inline Vector3D Vector3D::operator +  (const Vector3D& rcVct) const
{
  Vector3D cVctRes;
  cVctRes.x = x + rcVct.x;
  cVctRes.y = y + rcVct.y;
  cVctRes.z = z + rcVct.z;
  return cVctRes;
}

inline Vector3D Vector3D::operator &  (const Vector3D& rcVct) const
{
  Vector3D cVctRes;
  cVctRes.x = x * (float) fabs(rcVct.x);
  cVctRes.y = y * (float) fabs(rcVct.y);
  cVctRes.z = z * (float) fabs(rcVct.z);
  return cVctRes;
}

inline Vector3D Vector3D::operator -  (const Vector3D& rcVct) const
{
  Vector3D cVctRes;
  cVctRes.x = x - rcVct.x;
  cVctRes.y = y - rcVct.y;
  cVctRes.z = z - rcVct.z;
  return cVctRes;
}
 
inline Vector3D Vector3D::operator - (void) const
{
  return Vector3D(-x, -y, -z);
}

inline Vector3D& Vector3D::operator += (const Vector3D& rcVct)
{
  x += rcVct.x;
  y += rcVct.y;
  z += rcVct.z;
  return *this;
}

inline Vector3D& Vector3D::operator -= (const Vector3D& rcVct)
{
  x -= rcVct.x;
  y -= rcVct.y;
  z -= rcVct.z;                    
  return *this;
}

inline Vector3D& Vector3D::operator *= (float fScale)
{
  x *= fScale;
  y *= fScale;
  z *= fScale;        
  return *this;
}

inline Vector3D Vector3D::operator * (float fScale)
{
  Vector3D temp(*this);
  temp *= fScale;
  return temp;
}

inline Vector3D& Vector3D::operator =  (const Vector3D& rcVct)
{
  x = rcVct.x;
  y = rcVct.y;
  z = rcVct.z;
  return *this;
}

inline float Vector3D::operator *  (const Vector3D& rcVct) const
{
  return (x * rcVct.x) + (y * rcVct.y) + (z * rcVct.z);
}

inline Vector3D& Vector3D::operator /  (float fDiv) 
{
  x /= fDiv;
  y /= fDiv;
  z /= fDiv;
  return *this;
}

inline Vector3D Vector3D::operator %  (const Vector3D& rcVct) const
{
  Vector3D cVctRes;
  cVctRes.x = (y * rcVct.z) - (z * rcVct.y);
  cVctRes.y = (z * rcVct.x) - (x * rcVct.z);
  cVctRes.z = (x * rcVct.y) - (y * rcVct.x);
  return cVctRes;
}

inline bool Vector3D::operator != (const Vector3D& rcVct) const
{ 
  return !((*this) == rcVct);
}
 
inline bool Vector3D::operator == (const Vector3D& rcVct) const
{
  return (fabs (x - rcVct.x) <= FLOAT_EPS) &&
         (fabs (y - rcVct.y) <= FLOAT_EPS) &&
         (fabs (z - rcVct.z) <= FLOAT_EPS);
}  

inline Vector3D& Vector3D::ProjToPlane (const Vector3D &rclBase, const Vector3D &rclNorm)
{
  Vector3D clTemp(rclNorm);
  
  *this = *this - (clTemp *=  ((*this - rclBase) * clTemp));
  return *this;
}

inline float Vector3D::DistanceToPlane (const Vector3D &rclBase, const Vector3D &rclNorm) const
{
  return (*this - rclBase) * rclNorm;
}

inline float Vector3D::Length (void) const
{
  return (float)sqrt ((x * x) + (y * y) + (z * z));
}

inline float Vector3D::DistanceToLine (const Vector3D &rclBase, const Vector3D &rclDirect) const
{
  return (float) fabs((rclDirect % Vector3D(*this - rclBase)).Length() / 
                                     rclDirect.Length());
}

inline Vector3D& Vector3D::ProjToLine (const Vector3D &rclPoint, const Vector3D &rclLine)
{
  return (*this = ((((rclPoint * rclLine) / rclLine.Sqr()) * rclLine) - rclPoint));
}

inline float Vector3D::Sqr (void) const
{
  return (float) ((x * x) + (y * y) + (z * z));
}

inline void Vector3D::Set (float fX, float fY, float fZ)
{
  x = fX;
  y = fY;
  z = fZ;
}

inline void Vector3D::Set (double dX, double dY, double dZ)
{
  x = float(dX);
  y = float(dY);
  z = float(dZ);
}

// global functions
inline float Distance (const Vector3D &rcVct1, const Vector3D &rcVct2)
{
  return (rcVct1 - rcVct2).Length();
}

inline float DistanceP2 (const Vector3D &rclV1, const Vector3D &rclV2)
{
  float x=rclV1.x-rclV2.x, y=rclV1.y-rclV2.y, z=rclV1.z-rclV2.z; 
  return x * x + y * y + z * z;
}

inline Vector3D operator * (float fFac, const Vector3D &rcVct)
{
  return Vector3D(rcVct.x * fFac, rcVct.y * fFac, rcVct.z * fFac);
}

} // namespace Mesh

#endif // VECTOR3D_H 

