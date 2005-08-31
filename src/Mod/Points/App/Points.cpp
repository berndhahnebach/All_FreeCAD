/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
# include <math.h>
#endif

#include "Points.h"
#include <Base/Matrix.h>
using namespace Points;

/*

Point::Point ()
{
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}

Point::Point (float fX, float fY, float fZ)
{
    x = fX;
    y = fY;
    z = fZ;
}

Point::Point (float Coord[3])
{
    x = Coord[0];
    y = Coord[1];
    z = Coord[2];
}

Point::Point (const Point& rPt)
{
    x = rPt.x;
    y = rPt.y;
    z = rPt.z;
}

float& Point::operator[] (int i) const
{
    return (float&) *(&x + i);
}

Point::operator float* ()
{
    return &x;
}

Point& Point::operator= (const Point& rPt)
{
    x = rPt.x;
    y = rPt.y;
    z = rPt.z;
    return *this;
}

bool Point::operator== (const Point& rPt) const
{
    return ( x == rPt.x && y == rPt.y && z == rPt.z );
}

bool Point::operator!= (const Point& rPt) const
{
    return ( x != rPt.x || y != rPt.y || z != rPt.z );
}
*/


void PointsPropertyNormal::transform(const Matrix4D &mat)
{
  // A normal vector is only a direction with unit length, so we only need to rotate it
  // (no translations or scaling)

  // Extract scale factors (assumes an orthogonal rotation matrix)
  // Use the fact that the length of the row vectors of R are all equal to 1
  // And that scaling is applied after rotating
  double s[3];
  s[0] = sqrt(mat[0][0] * mat[0][0] + mat[0][1] * mat[0][1] + mat[0][2] * mat[0][2]);
  s[1] = sqrt(mat[1][0] * mat[1][0] + mat[1][1] * mat[1][1] + mat[1][2] * mat[1][2]);
  s[2] = sqrt(mat[2][0] * mat[2][0] + mat[2][1] * mat[2][1] + mat[2][2] * mat[2][2]);

  // Set up the rotation matrix: zero the translations and make the scale factors = 1
  Matrix4D rot;
  rot.unity();
  for (unsigned short i = 0; i < 3; i++)
    for (unsigned short j = 0; j < 3; j++)
      rot[i][j] = mat[i][j] / s[i];

  // Rotate the normal vectors
  for (std::vector<Vector3D>::iterator it = aVertexNormal.begin(); it != aVertexNormal.end(); ++it)
  {
    *it = rot * (*it);
  }
}

void PointsWithProperty::transform(const Matrix4D &rclMat)
{
  DataWithPropertyBag::transform(rclMat);
  for (std::vector<Vector3D>::iterator it = _Points.begin(); it != _Points.end(); ++it)
  {
    *it = rclMat * (*it);
  }
}
