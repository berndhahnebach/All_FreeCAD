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

//#include "Definitions.h"
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
class /*BaseExport*/ Vector3
{
public:
  /** @name Public data members */
  //@{
  _Precision x; /**< x-coordinate */
  _Precision y; /**< y-coordinate */
  _Precision z; /**< z-coordinate */
  //@}
  
  /// Construction
  inline explicit Vector3 (_Precision fx = 0.0f, _Precision fy = 0.0f, _Precision fz = 0.0f);
  /// Construction
  inline Vector3 (const Vector3<_Precision>& rcVct);

  /** @name Operator */
  //@{
  /// Returns a reference to a coordinate. \a usIndex must be in the range [0,2]
  inline _Precision & operator [] (unsigned short usIndex);
  /// Vector addition
  inline Vector3 operator +  (const Vector3<_Precision>& rcVct) const;
  inline Vector3 operator &  (const Vector3<_Precision>& rcVct) const;
  /// Vector subtraction
  inline Vector3 operator -  (const Vector3<_Precision>& rcVct) const;
  /// Negative vector
  inline Vector3 operator - (void) const;
  /// Vector summation
  inline Vector3 &operator += (const Vector3<_Precision>& rcVct);
  /// Vector subtraction
  inline Vector3 & operator -= (const Vector3<_Precision>& rcVct);
  /// Vector scaling
  inline Vector3 & operator *= (_Precision fScale);
  /// Vector scaling
  inline Vector3 operator *   (_Precision fScale);
  /// Assignment
  inline Vector3 & operator =  (const Vector3<_Precision>& rcVct);
  /// Scalar product
  inline _Precision operator *  (const Vector3<_Precision>& rcVct) const;
  /// Vector scaling
  inline Vector3 & operator / (_Precision fDiv); 
  /// Cross product
  inline Vector3 operator %  (const Vector3<_Precision>& rcVct) const;
  /// Comparing for inequality
  inline bool operator != (const Vector3<_Precision>& rcVct) const;
  /// Comparing for equality
  inline bool operator == (const Vector3<_Precision>& rcVct) const;
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

  inline void Set (float fX, float fY, float fZ);
  inline void Set (double dX, double dY, double dZ);

  void  Print (void);
  unsigned long GetMemSpace (void);

  /** @name Mathematics */
  //@{
  /// Length of the vector.
  inline _Precision Length (void) const;
  /// Squared length of the vector.
  inline _Precision Sqr (void) const;
  /// Set length to 1.
  Vector3 & Normalize (void);
  /// Get angle between both vectors.
  _Precision GetAngle (const Vector3 &rcVect) const;
  /** Transforms this point to the coordinate system defined by origin \a rclBase, 
   * vector \a vector rclDirX and vector \a vector rclDirY. 
   * \note \a rclDirX must be perpendicular to \a rclDirY, i.e. \a rclDirX * \a rclDirY = 0..
   */
  void TransformToCoordinateSystem (const Vector3 &rclBase, const Vector3 &rclDirX, const Vector3 &rclDirY);
  bool Equal(const Vector3 &rclVect) const;
  /// Projects this point onto the plane given by the base \a rclBase and the normal \a rclNorm.
  inline Vector3 & ProjToPlane (const Vector3 &rclBase, const Vector3 &rclNorm);
  /// Projects this point onto the line given by the base \a rclPoint and the direction \a rclLine.
  /**
   * Projects a point \a rclPoint onto the line defined by the origin and the direction \a rclLine.
   * The result is a vector from \a rclPoint to the point on the line. The length of this vector 
   * is the distance from \a rclPoint to the line.
   * Note: The resulting vector does not depend on the current vector.
   */
  inline Vector3 & ProjToLine (const Vector3 &rclPoint, const Vector3 &rclLine);
  /** Computes the distance to the given plane. Depending on the side this point is located
   * the distance can also be negative. The distance is positive if the point is at the same
   * side the plane normal points to, negative otherwise.
   */
  inline _Precision DistanceToPlane (const Vector3 &rclBase, const Vector3 &rclNorm) const;
  /// Computes the distance from this point to the line given by \a rclBase and \a rclDirect.
  inline _Precision DistanceToLine (const Vector3 &rclBase, const Vector3 &rclDirect) const;
  //@}

  /** @name Friends */
  //@{
  /// Returns the distance between two points
  //friend _Precision Distance (const Vector3 &rcVct1, const Vector3 &rcVct2);
  /// Returns the squared distance between two points
  //friend _Precision DistanceP2 (const Vector3 &rclV1, const Vector3 &rclV2);
  /// Multiplication of scalar with vector.
  //friend Vector3 operator * (_Precision fFac, const Vector3 &rcVct);
  //@}
};

template <class _Precision>
inline Vector3<_Precision>::Vector3 (_Precision fx, _Precision fy, _Precision fz)
: x(fx),
  y(fy),
  z(fz)
{
}

template <class _Precision>
inline Vector3<_Precision>::Vector3 (const Vector3<_Precision>& rcVct)
: x(rcVct.x),
  y(rcVct.y),
  z(rcVct.z)
{
}

template <class _Precision>
inline _Precision& Vector3<_Precision>::operator [] (unsigned short usIndex)
{
  switch (usIndex)
  {
    case 0: return x;
    case 1: return y;
    case 2: return z;
  }
  return x;
}

template <class _Precision>
inline Vector3 <_Precision>Vector3<_Precision>::operator +  (const Vector3<_Precision>& rcVct) const
{
  Vector3<_Precision> cVctRes;
  cVctRes.x = x + rcVct.x;
  cVctRes.y = y + rcVct.y;
  cVctRes.z = z + rcVct.z;
  return cVctRes;
}

template <class _Precision>
inline Vector3<_Precision> Vector3<_Precision>::operator &  (const Vector3<_Precision>& rcVct) const
{
  Vector3<_Precision> cVctRes;
  cVctRes.x = x * (_Precision) fabs(rcVct.x);
  cVctRes.y = y * (_Precision) fabs(rcVct.y);
  cVctRes.z = z * (_Precision) fabs(rcVct.z);
  return cVctRes;
}

template <class _Precision>
inline Vector3<_Precision> Vector3<_Precision>::operator -  (const Vector3<_Precision>& rcVct) const
{
  Vector3<_Precision> cVctRes;
  cVctRes.x = x - rcVct.x;
  cVctRes.y = y - rcVct.y;
  cVctRes.z = z - rcVct.z;
  return cVctRes;
}
 
template <class _Precision>
inline Vector3<_Precision> Vector3<_Precision>::operator - (void) const
{
  return Vector3(-x, -y, -z);
}

template <class _Precision>
inline Vector3<_Precision>& Vector3<_Precision>::operator += (const Vector3<_Precision>& rcVct)
{
  x += rcVct.x;
  y += rcVct.y;
  z += rcVct.z;
  return *this;
}

template <class _Precision>
inline Vector3<_Precision>& Vector3<_Precision>::operator -= (const Vector3<_Precision>& rcVct)
{
  x -= rcVct.x;
  y -= rcVct.y;
  z -= rcVct.z;                    
  return *this;
}

template <class _Precision>
inline Vector3<_Precision>& Vector3<_Precision>::operator *= (_Precision fScale)
{
  x *= fScale;
  y *= fScale;
  z *= fScale;        
  return *this;
}

template <class _Precision>
inline Vector3<_Precision> Vector3<_Precision>::operator * (_Precision fScale)
{
  Vector3<_Precision> temp(*this);
  temp *= fScale;
  return temp;
}

template <class _Precision>
inline Vector3<_Precision>& Vector3<_Precision>::operator =  (const Vector3<_Precision>& rcVct)
{
  x = rcVct.x;
  y = rcVct.y;
  z = rcVct.z;
  return *this;
}

template <class _Precision>
inline _Precision Vector3<_Precision>::operator *  (const Vector3<_Precision>& rcVct) const
{
  return (x * rcVct.x) + (y * rcVct.y) + (z * rcVct.z);
}

template <class _Precision>
inline Vector3<_Precision>& Vector3<_Precision>::operator /  (_Precision fDiv) 
{
  x /= fDiv;
  y /= fDiv;
  z /= fDiv;
  return *this;
}

template <class _Precision>
inline Vector3<_Precision> Vector3<_Precision>::operator %  (const Vector3<_Precision>& rcVct) const
{
  Vector3<_Precision> cVctRes;
  cVctRes.x = (y * rcVct.z) - (z * rcVct.y);
  cVctRes.y = (z * rcVct.x) - (x * rcVct.z);
  cVctRes.z = (x * rcVct.y) - (y * rcVct.x);
  return cVctRes;
}

template <class _Precision>
inline bool Vector3<_Precision>::operator != (const Vector3<_Precision>& rcVct) const
{ 
  return !((*this) == rcVct);
}
 
template <class _Precision>
inline bool Vector3<_Precision>::operator == (const Vector3<_Precision>& rcVct) const
{
  return (fabs (x - rcVct.x) <= FLOAT_EPS) &&
         (fabs (y - rcVct.y) <= FLOAT_EPS) &&
         (fabs (z - rcVct.z) <= FLOAT_EPS);
}  

template <class _Precision>
inline Vector3<_Precision>& Vector3<_Precision>::ProjToPlane (const Vector3<_Precision> &rclBase, const Vector3<_Precision> &rclNorm)
{
  Vector3<_Precision> clTemp(rclNorm);
  
  *this = *this - (clTemp *=  ((*this - rclBase) * clTemp));
  return *this;
}

template <class _Precision>
inline _Precision Vector3<_Precision>::DistanceToPlane (const Vector3<_Precision> &rclBase, const Vector3<_Precision> &rclNorm) const
{
  return (*this - rclBase) * rclNorm;
}

template <class _Precision>
inline _Precision Vector3<_Precision>::Length (void) const
{
  return (_Precision)sqrt ((x * x) + (y * y) + (z * z));
}

template <class _Precision>
inline _Precision Vector3<_Precision>::DistanceToLine (const Vector3<_Precision> &rclBase, const Vector3<_Precision> &rclDirect) const
{
  return (_Precision) fabs((rclDirect % Vector3(*this - rclBase)).Length() / 
                                     rclDirect.Length());
}

template <class _Precision>
inline Vector3<_Precision>& Vector3<_Precision>::ProjToLine (const Vector3<_Precision> &rclPoint, const Vector3<_Precision> &rclLine)
{
  return (*this = ((((rclPoint * rclLine) / rclLine.Sqr()) * rclLine) - rclPoint));
}

template <class _Precision>
inline _Precision Vector3<_Precision>::Sqr (void) const
{
  return (_Precision) ((x * x) + (y * y) + (z * z));
}

template <class _Precision>
inline void Vector3<_Precision>::Set (float fX, float fY, float fZ)
{
  x = _Precision(fX);
  y = _Precision(fY);
  z = _Precision(fZ);
}

template <class _Precision>
inline void Vector3<_Precision>::Set (double dX, double dY, double dZ)
{
  x = _Precision(dX);
  y = _Precision(dY);
  z = _Precision(dZ);
}

// global functions
template <class _Precision>
inline _Precision Distance (const Vector3<_Precision> &rcVct1, const Vector3<_Precision> &rcVct2)
{
  return (rcVct1 - rcVct2).Length();
}

template <class _Precision>
inline _Precision DistanceP2 (const Vector3<_Precision> &rclV1, const Vector3<_Precision> &rclV2)
{
  _Precision x=rclV1.x-rclV2.x, y=rclV1.y-rclV2.y, z=rclV1.z-rclV2.z;
  return x * x + y * y + z * z;
}

template <class _Precision>
inline Vector3<_Precision> operator * (_Precision fFac, const Vector3<_Precision> &rcVct)
{
  return Vector3<_Precision>(rcVct.x * fFac, rcVct.y * fFac, rcVct.z * fFac);
}

template <class _Precision>
void Vector3<_Precision>::ScaleX (_Precision f)
{
  x *= f;
}

template <class _Precision>
void Vector3<_Precision>::ScaleY (_Precision f)
{
  y *= f;
}

template <class _Precision>
void Vector3<_Precision>::ScaleZ (_Precision f)
{
  z *= f;
}

template <class _Precision>
void Vector3<_Precision>::Scale (_Precision fX, _Precision fY, _Precision fZ)
{
  x *= fX;
  y *= fY;
  z *= fZ;
}

template <class _Precision>
void Vector3<_Precision>::MoveX (_Precision f)
{
  x += f;
}

template <class _Precision>
void Vector3<_Precision>::MoveY (_Precision f)
{
  y += f;
}

template <class _Precision>
void Vector3<_Precision>::MoveZ (_Precision f)
{
  z += f;
}

template <class _Precision>
void Vector3<_Precision>::Move (_Precision fX, _Precision fY, _Precision fZ)
{
  x += fX;
  y += fY;
  z += fZ;
}

template <class _Precision>
void Vector3<_Precision>::RotateX (_Precision f)
{
  Vector3 cPt (*this);
  _Precision fsin, fcos;
 
  fsin = (_Precision)sin (f);
  fcos = (_Precision)cos (f);
  y = (cPt.y * fcos) - (cPt.z * fsin);
  z = (cPt.y * fsin) + (cPt.z * fcos);
}

template <class _Precision>
void Vector3<_Precision>::RotateY (_Precision f)
{
  Vector3 cPt (*this);
  _Precision fsin, fcos;
 
  fsin = (_Precision)sin (f);
  fcos = (_Precision)cos (f);
  x = (cPt.z * fsin) + (cPt.x * fcos);
  z = (cPt.z * fcos) - (cPt.x * fsin);
}

template <class _Precision>
void Vector3<_Precision>::RotateZ (_Precision f)
{
  Vector3 cPt (*this);
  _Precision fsin, fcos;
 
  fsin = (_Precision)sin (f);
  fcos = (_Precision)cos (f);
  x = (cPt.x * fcos) - (cPt.y * fsin);
  y = (cPt.x * fsin) + (cPt.y * fcos);
}

template <class _Precision>
Vector3<_Precision> & Vector3<_Precision>::Normalize (void)
{
  _Precision fLen = Length ();
  if (fLen != 0.0f)
  {
    x /= fLen;
    y /= fLen;
    z /= fLen;
  };
  return *this;
}

template <class _Precision>
_Precision Vector3<_Precision>::GetAngle (const Vector3 &rcVect) const
{
  _Precision divid, fNum;
  
  divid = Length() * ((Vector3<_Precision>&)rcVect).Length();
 
  if ((divid < -1e-10f) || (divid > 1e-10f))
  {
    fNum = (*this * rcVect) / divid;
    if (fNum < -1)
      return F_PI;
    else
      if (fNum > 1)
        return 0.0F;
      else
        return _Precision(acos(fNum));
  }
  else
    return (_Precision) FLOAT_MAX; // division by zero
}

template <class _Precision>
void Vector3<_Precision>::TransformToCoordinateSystem ( const Vector3 &rclBase,
                                             const Vector3 &rclDirX,
                                             const Vector3 &rclDirY )
{
  Vector3  clVectX, clVectY, clVectZ, clVectOld;
  
  clVectX = rclDirX;
  clVectY = rclDirY;
  clVectZ = rclDirX % rclDirY;
  clVectX.Normalize();
  clVectY.Normalize();
  clVectZ.Normalize();

  clVectOld = *this - rclBase;
   
  x = clVectX * clVectOld;
  y = clVectY * clVectOld;
  z = clVectZ * clVectOld;
}

template <class _Precision>
bool Vector3<_Precision>::Equal(const Vector3 &rclVect) const
{
  int     ex1, ex2, ey1, ey2, ez1, ez2;
  double  mx1, mx2, my1, my2, mz1, mz2;
  
  mx1 = frexp(x, &ex1);
  my1 = frexp(y, &ey1);
  mz1 = frexp(z, &ez1);

  mx2 = frexp(rclVect.x, &ex2);
  my2 = frexp(rclVect.y, &ey2);
  mz2 = frexp(rclVect.z, &ez2);
  
  return ( (ex1 == ex2) && (ey1 == ey2) && (ez1 == ez2) &&
           (fabs(mx1 - mx2) < _Precision_EPS) &&
           (fabs(my1 - my2) < _Precision_EPS) &&
           (fabs(mz1 - mz2) < _Precision_EPS) );
}

template <class _Precision>
unsigned long Vector3<_Precision>::GetMemSpace (void)
{
  return sizeof(Vector3);
}

template <class _Precision>
void Vector3<_Precision>::Print (void)
{
  printf ("x: %f, y: %f, z: %f\n", x, y, z);
}

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;


} // namespace Base

#endif // VECTOR3D_H

