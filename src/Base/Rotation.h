/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef BASE_ROTATION_H
#define BASE_ROTATION_H

#include "Vector3D.h"

namespace Base {

// forward declarations
class Matrix4D;

class BaseExport Rotation
{
public:
  /** Construction. */
  //@{
  Rotation();
  Rotation(const Vector3f& axis, const float fAngle);
  Rotation(const Matrix4D& matrix);
  Rotation(const float q[4]);
  Rotation(const float q0, const float q1, const float q2, const float q3);
  Rotation(const Vector3f& rotateFrom, const Vector3f& rotateTo);
  Rotation(const Rotation& rot);
  //@}

  /** Methods to get or set rotations. */
  //@{
  const float * getValue(void) const;
  void getValue(float & q0, float & q1, float & q2, float & q3) const;
  void setValue(const float q0, const float q1, const float q2, const float q3);
  void getValue(Vector3f & axis, float & rfAngle) const;
  void getValue(Matrix4D & matrix) const;
  void setValue(const float q[4]);
  void setValue(const Matrix4D& matrix);
  void setValue(const Vector3f & axis, const float fAngle);
  void setValue(const Vector3f & rotateFrom, const Vector3f & rotateTo);
  //@}

  /** Invert rotations. */
  //@{
  Rotation & invert(void);
  Rotation inverse(void) const;
  //@}

  /** Operators. */
  //@{
  Rotation & operator*=(const Rotation & q);
  Rotation operator *(const Rotation & q);
  bool operator==(const Rotation & q);
  bool operator!=(const Rotation & q);

  void multVec(const Vector3f & src, Vector3f & dst) const;
  void scaleAngle(const float scaleFactor);
  //@}

  static Rotation slerp(const Rotation & rot0, const Rotation & rot1, float t);
  static Rotation identity(void);

private:
  void normalize();
  float quat[4];
};

}

#endif // BASE_ROTATION_H
