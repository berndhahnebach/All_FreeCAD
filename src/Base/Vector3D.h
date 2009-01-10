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


#ifndef BASE_VECTOR3D_H
#define BASE_VECTOR3D_H


#include <math.h>

#define FLOAT_EPS   1.0e-4f 

#ifndef  F_PI
# define F_PI  3.1415926f
#endif

#ifndef  D_PI
# define D_PI  3.141592653589793
#endif
  
#ifndef  FLOAT_MAX
# define FLOAT_MAX 1e30f
#endif

#ifndef  DOUBLE_MAX
# define DOUBLE_MAX 1.7976931348623157E+308    /* max decimal value of a "double"*/
#endif

#ifndef  DOUBLE_MIN
# define DOUBLE_MIN 2.2250738585072014E-308    /* min decimal value of a "double"*/
#endif


namespace Base {

/** The Vector Base class. */
template <class _Precision>
class Vector3
{
public:
    /** @name Public data members */
    //@{
    _Precision x; /**< x-coordinate */
    _Precision y; /**< y-coordinate */
    _Precision z; /**< z-coordinate */
    //@}

    /// Construction
    explicit Vector3 (_Precision fx = 0.0f, _Precision fy = 0.0f, _Precision fz = 0.0f);
    /// Construction
    Vector3 (const Vector3<_Precision>& rcVct);

    /** @name Operator */
    //@{
    /// Returns a reference to a coordinate. \a usIndex must be in the range [0,2]
    _Precision & operator [] (unsigned short usIndex);
    /// Returns a const reference to a coordinate. \a usIndex must be in the range [0,2]
    const _Precision & operator [] (unsigned short usIndex) const;
    /// Vector addition
    Vector3 operator +  (const Vector3<_Precision>& rcVct) const;
    Vector3 operator &  (const Vector3<_Precision>& rcVct) const;
    /// Vector subtraction
    Vector3 operator -  (const Vector3<_Precision>& rcVct) const;
    /// Negative vector
    Vector3 operator - (void) const;
    /// Vector summation
    Vector3 &operator += (const Vector3<_Precision>& rcVct);
    /// Vector subtraction
    Vector3 & operator -= (const Vector3<_Precision>& rcVct);
    /// Vector scaling
    Vector3 & operator *= (_Precision fScale);
    /// Vector scaling
    Vector3 operator *   (_Precision fScale);
    /// Assignment
    Vector3 & operator =  (const Vector3<_Precision>& rcVct);
    /// Scalar product
    _Precision operator *  (const Vector3<_Precision>& rcVct) const;
    /// Vector scaling
    Vector3 & operator / (_Precision fDiv); 
    /// Cross product
    Vector3 operator %  (const Vector3<_Precision>& rcVct) const;
    /// Comparing for inequality
    bool operator != (const Vector3<_Precision>& rcVct) const;
    /// Comparing for equality
    bool operator == (const Vector3<_Precision>& rcVct) const;
    //@}

    /** @name Modification */
    //@{
    void ScaleX (_Precision f);
    void ScaleY (_Precision f);
    void ScaleZ (_Precision f);
    void Scale (_Precision fX, _Precision fY, _Precision fZ);
    void MoveX (_Precision f);
    void MoveY (_Precision f);
    void MoveZ (_Precision f);
    void Move (_Precision fX, _Precision fY, _Precision fZ);
    void RotateX (_Precision f);
    void RotateY (_Precision f);
    void RotateZ (_Precision f);
    //@}

    void Set (_Precision fX, _Precision fY, _Precision fZ);

    /** @name Mathematics */
    //@{
    /// Length of the vector.
    _Precision Length (void) const;
    /// Squared length of the vector.
    _Precision Sqr (void) const;
    /// Set length to 1.
    Vector3 & Normalize (void);
    /// Get angle between both vectors.
    _Precision GetAngle (const Vector3 &rcVect) const;
    /** Transforms this point to the coordinate system defined by origin \a rclBase, 
    * vector \a vector rclDirX and vector \a vector rclDirY. 
    * \note \a rclDirX must be perpendicular to \a rclDirY, i.e. \a rclDirX * \a rclDirY = 0..
    */
    void TransformToCoordinateSystem (const Vector3 &rclBase, const Vector3 &rclDirX, const Vector3 &rclDirY);
    //bool Equal(const Vector3 &rclVect) const;
    /// Projects this point onto the plane given by the base \a rclBase and the normal \a rclNorm.
    Vector3 & ProjToPlane (const Vector3 &rclBase, const Vector3 &rclNorm);
    /// Projects this point onto the line given by the base \a rclPoint and the direction \a rclLine.
    /**
     * Projects a point \a rclPoint onto the line defined by the origin and the direction \a rclLine.
     * The result is a vector from \a rclPoint to the point on the line. The length of this vector 
     * is the distance from \a rclPoint to the line.
     * Note: The resulting vector does not depend on the current vector.
     */
    Vector3 & ProjToLine (const Vector3 &rclPoint, const Vector3 &rclLine);
    /**
     * Get the perpendicular of this point to the line defined by rclBase and rclDir.
     * Note: Do not mix up this method with ProjToLine.
     */
    Vector3 Perpendicular(const Vector3 &rclBase, const Vector3 &rclDir) const;
    /** Computes the distance to the given plane. Depending on the side this point is located
     * the distance can also be negative. The distance is positive if the point is at the same
     * side the plane normal points to, negative otherwise.
     */
    _Precision DistanceToPlane (const Vector3 &rclBase, const Vector3 &rclNorm) const;
    /// Computes the distance from this point to the line given by \a rclBase and \a rclDirect.
    _Precision DistanceToLine (const Vector3 &rclBase, const Vector3 &rclDirect) const;
    //@}
};

// global functions

/// Returns the distance between two points
template <class _Precision>
inline _Precision Distance (const Vector3<_Precision> &rcVct1, const Vector3<_Precision> &rcVct2)
{
    return (rcVct1 - rcVct2).Length();
}

/// Returns the squared distance between two points
template <class _Precision>
inline _Precision DistanceP2 (const Vector3<_Precision> &rclV1, const Vector3<_Precision> &rclV2)
{
    _Precision x=rclV1.x-rclV2.x, y=rclV1.y-rclV2.y, z=rclV1.z-rclV2.z;
    return x * x + y * y + z * z;
}

/// Multiplication of scalar with vector.
template <class _Precision>
inline Vector3<_Precision> operator * (_Precision fFac, const Vector3<_Precision> &rcVct)
{
    return Vector3<_Precision>(rcVct.x * fFac, rcVct.y * fFac, rcVct.z * fFac);
}

typedef Vector3<float>  Vector3f;
typedef Vector3<double> Vector3d;


} // namespace Base

#endif // BASE_VECTOR3D_H

