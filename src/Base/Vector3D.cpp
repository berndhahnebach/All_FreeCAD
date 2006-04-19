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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <stdio.h>
#endif

#include "Vector3D.h"

using namespace Base;

/* Moved to .h because of Template
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
  
  divid = Length() * ((RVector3)rcVect).Length();
 
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
    return -_Precision_MAX; // division by zero
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

*/
