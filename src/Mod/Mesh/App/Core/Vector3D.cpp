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

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Vector3D.h"

using namespace Mesh;

void Vector3D::ScaleX (float f)
{
  x *= f;
}

void Vector3D::ScaleY (float f)
{
  y *= f;
}

void Vector3D::ScaleZ (float f)
{
  z *= f;
}

void Vector3D::Scale (float fX, float fY, float fZ)
{
  x *= fX;
  y *= fY;
  z *= fZ;
}

void Vector3D::MoveX (float f)
{
  x += f;
}

void Vector3D::MoveY (float f)
{
  y += f;
}

void Vector3D::MoveZ (float f)
{
  z += f;
}

void Vector3D::Move (float fX, float fY, float fZ)
{
  x += fX;
  y += fY;
  z += fZ;
}

void Vector3D::RotateX (float f)
{
  Vector3D cPt (*this);
  float fsin, fcos;
 
  fsin = (float)sin (f);
  fcos = (float)cos (f);
  y = (cPt.y * fcos) - (cPt.z * fsin);
  z = (cPt.y * fsin) + (cPt.z * fcos);
}

void Vector3D::RotateY (float f)
{
  Vector3D cPt (*this);
  float fsin, fcos;
 
  fsin = (float)sin (f);
  fcos = (float)cos (f);
  x = (cPt.z * fsin) + (cPt.x * fcos);
  z = (cPt.z * fcos) - (cPt.x * fsin);
}

void Vector3D::RotateZ (float f)
{
  Vector3D cPt (*this);
  float fsin, fcos;
 
  fsin = (float)sin (f);
  fcos = (float)cos (f);
  x = (cPt.x * fcos) - (cPt.y * fsin);
  y = (cPt.x * fsin) + (cPt.y * fcos);
}

void Vector3D::Normalize (void)
{
  float fLen = Length ();
  if (fLen != 0.0f)
  {
    x /= fLen;
    y /= fLen;
    z /= fLen;
  };
}

DataStream & Vector3D::SaveData (DataStream& ofs)
{
  ofs << x << y << z;
  return ofs;
}

DataStream & Vector3D::LoadData (DataStream& ifs)
{
  ifs >> x >> y >> z;
  return ifs;
}

float Vector3D::GetAngle (const Vector3D &rcVect) const
{
  float divid, fNum;
  
  divid = Length() * ((RVector3D)rcVect).Length();
 
  if ((divid < -1e-10f) || (divid > 1e-10f))
  {
    fNum = (*this * rcVect) / divid;
    if (fNum < -1)
      return F_PI;
    else
      if (fNum > 1)
        return 0.0F;
      else
        return float(acos(fNum));
  }
  else
    return -FLOAT_MAX; // division by zero
}

void Vector3D::TransformToCoordinateSystem (const Vector3D &rclBase,
                                             const Vector3D &rclDirX,
                                             const Vector3D &rclDirY)
{
  Vector3D  clVectX, clVectY, clVectZ, clVectOld;
  
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

bool Vector3D::Equal(const Vector3D &rclVect) const
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
           (fabs(mx1 - mx2) < FLOAT_EPS) &&
           (fabs(my1 - my2) < FLOAT_EPS) &&
           (fabs(mz1 - mz2) < FLOAT_EPS) );
}

unsigned long Vector3D::GetMemSpace (void)
{
  return sizeof(Vector3D);
}

void Vector3D::Print (void)
{
  printf ("x: %f, y: %f, z: %f\n", x, y, z);
}

