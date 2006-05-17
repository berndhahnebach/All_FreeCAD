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


#include "PreCompiled.h"

#include "Rotation.h"
#include "Matrix.h"

using namespace Base;

Rotation::Rotation()
{
  quat[0]=quat[1]=quat[2]=0.0f;quat[3]=1.0f;
}

Rotation::Rotation(const Vector3f& axis, const float fAngle)
{
  this->setValue(axis, fAngle);
}

Rotation::Rotation(const Matrix4D& matrix)
{
  this->setValue(matrix);
}

Rotation::Rotation(const float q[4])
{
  this->setValue(q);
}

Rotation::Rotation(const float q0, const float q1, const float q2, const float q3)
{
  this->setValue(q0, q1, q2, q3);
}

Rotation::Rotation(const Vector3f & rotateFrom, const Vector3f & rotateTo)
{
  this->setValue(rotateFrom, rotateTo);
}

Rotation::Rotation(const Rotation& rot)
{
  this->quat[0] = rot.quat[0];
  this->quat[1] = rot.quat[1];
  this->quat[2] = rot.quat[2];
  this->quat[3] = rot.quat[3];
}

const float * Rotation::getValue(void) const
{
  return &this->quat[0];
}

void Rotation::getValue(float & q0, float & q1, float & q2, float & q3) const
{
  q0 = this->quat[0];
  q1 = this->quat[1];
  q2 = this->quat[2];
  q3 = this->quat[3];
}

void Rotation::setValue(const float q0, const float q1, const float q2, const float q3)
{
  this->quat[0] = q0;
  this->quat[1] = q1;
  this->quat[2] = q2;
  this->quat[3] = q3;
  this->normalize();
}

void Rotation::getValue(Vector3f & axis, float & rfAngle) const
{
  // Taken from <http://de.wikipedia.org/wiki/Quaternionen>
  //
  // Note: -1 < w < +1 (|w| == 1 not allowed, with w:=quat[3]) 
  if((this->quat[3] > -1.0f) && (this->quat[3] < 1.0f)) {
    rfAngle = float(acos(this->quat[3])) * 2.0f;
    float scale = (float)sin(rfAngle / 2.0f);
    // Get a normalized vector 
    axis.x = this->quat[0] / scale;
    axis.y = this->quat[1] / scale;
    axis.z = this->quat[2] / scale;
  } else {
    // The quaternion doesn't describe a rotation, so we can setup any value we want 
    axis.Set(0.0f, 0.0f, 1.0f);
    rfAngle = 0.0f;
  }
}

/**
 * Returns this rotation in form of a matrix.
 */
void Rotation::getValue(Matrix4D & matrix) const
{
  // Taken from <http://de.wikipedia.org/wiki/Quaternionen>
  //
  const float x = this->quat[0];
  const float y = this->quat[1];
  const float z = this->quat[2];
  const float w = this->quat[3];

  matrix[0][0] = 1.0f-2.0f*(y*y+z*z);
  matrix[0][1] = 2.0f*(x*y-z*w);
  matrix[0][2] = 2.0f*(x*z+y*w);
  matrix[0][3] = 0.0f;

  matrix[1][0] = 2.0f*(x*y+z*w);
  matrix[1][1] = 1.0f-2.0f*(x*x+z*z);
  matrix[1][2] = 2.0f*(y*z-x*w);
  matrix[1][3] = 0.0f;

  matrix[2][0] = 2.0f*(x*z-y*w);
  matrix[2][1] = 2.0f*(y*z+x*w);
  matrix[2][2] = 1.0f-2.0f*(x*x+y*y);
  matrix[2][3] = 0.0f;

  matrix[3][0] = 0.0f;
  matrix[3][1] = 0.0f;
  matrix[3][2] = 0.0f;
  matrix[3][3] = 1.0f;
}

void Rotation::setValue(const float q[4])
{
  this->quat[0] = q[0];
  this->quat[1] = q[1];
  this->quat[2] = q[2];
  this->quat[3] = q[3];
  this->normalize();
}

void Rotation::setValue(const Matrix4D & m)
{
  float trace = (float)(m[0][0] + m[1][1] + m[2][2]);
  if ( trace > 0.0f ) {
    float s = (float)sqrt(1.0f+trace);
    this->quat[3] = 0.5f * s;
    s = 0.5f / s;
    this->quat[0] = (float)((m[2][1] - m[1][2]) * s);
    this->quat[1] = (float)((m[0][2] - m[2][0]) * s);
    this->quat[2] = (float)((m[1][0] - m[0][1]) * s);
  }
  else {
    // Described in RotationIssues.pdf from <http://www.geometrictools.com>
    //
    // Get the max. element of the trace
    int i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;

    int j = (i+1)%3;
    int k = (i+2)%3;

    float s = (float)sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);
    this->quat[i] = s * 0.5f;
    s = 0.5f / s;
    this->quat[3] = (float)((m[k][j] - m[j][k]) * s);
    this->quat[j] = (float)((m[j][i] + m[i][j]) * s);
    this->quat[k] = (float)((m[k][i] + m[i][k]) * s);
  }
}

void Rotation::setValue(const Vector3f & axis, const float fAngle)
{
  // Taken from <http://de.wikipedia.org/wiki/Quaternionen>
  //
  this->quat[3] = (float)cos(fAngle/2.0);
  Vector3f norm = axis;
  norm.Normalize();
  float scale = (float)sin(fAngle/2.0);
  this->quat[0] = norm.x * scale;
  this->quat[1] = norm.y * scale;
  this->quat[2] = norm.z * scale;
}

void Rotation::setValue(const Vector3f & rotateFrom, const Vector3f & rotateTo)
{
  Vector3f u(rotateFrom); u.Normalize();
  Vector3f v(rotateTo); v.Normalize();

  // The vector from x to is the roatation axis because it's the normal of the plane defined by (0,u,v) 
  const float dot = u * v;
  Vector3f w = u % v;
  const float wlen = w.Length();

  if (wlen == 0.0f) { // Parallel vectors
    // Check if they are pointing in the same direction.
    if (dot > 0.0f) {
      this->setValue(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else {
      // We can use any axis perpendicular to u (and v)
      Vector3f t = u % Vector3f(1.0f, 0.0f, 0.0f);
      if(t.Length() < FLT_EPSILON) 
        t = u % Vector3f(0.0f, 1.0f, 0.0f);
      this->setValue(t.x, t.y, t.z, 0.0f);
    }
  }
  else { // Vectors are not parallel
    // Note: A quaternion is not well-defined by specifying a point and its transformed point.
    // Every quaternion with a rotation axis having the same angle to the vectors of both points is okay.
    float angle = (float)acos(dot);
    this->setValue(w, angle);
  }
}

void Rotation::normalize()
{
  float len = (float)sqrt(this->quat[0]*this->quat[0]+
                          this->quat[1]*this->quat[1]+
                          this->quat[2]*this->quat[2]+
                          this->quat[3]*this->quat[3]);
  this->quat[0] /= len;
  this->quat[1] /= len;
  this->quat[2] /= len;
  this->quat[3] /= len;
}

Rotation & Rotation::invert(void)
{
  this->quat[0] = -this->quat[0];
  this->quat[1] = -this->quat[1];
  this->quat[2] = -this->quat[2];
  this->quat[3] =  this->quat[3];
  return *this;
}

Rotation Rotation::inverse(void) const
{
  Rotation rot;
  rot.quat[0] = -this->quat[0];
  rot.quat[1] = -this->quat[1];
  rot.quat[2] = -this->quat[2];
  rot.quat[3] =  this->quat[3];
  return rot;
}

Rotation & Rotation::operator*=(const Rotation & q)
{
  // Taken from <http://de.wikipedia.org/wiki/Quaternionen>
  float x0, y0, z0, w0;
  this->getValue(x0, y0, z0, w0);
  float x1, y1, z1, w1;
  q.getValue(x1, y1, z1, w1);

  this->setValue(w0*x1 + x0*w1 + y0*z1 - z0*y1,
                 w0*y1 - x0*z1 + y0*w1 + z0*x1,
                 w0*z1 + x0*y1 - y0*x1 + z0*w1,
                 w0*w1 - x0*x1 - y0*y1 - z0*z1);
  return *this;
}

Rotation Rotation::operator*(const Rotation & q)
{
  Rotation quat(*this);
  quat *= q;
  return quat;
}

bool Rotation::operator==(const Rotation & q)
{
  bool equal = true;
  for (int i=0; i<4;i++)
    equal &= (fabs(this->quat[i] - q.quat[i]) < 0.005 );
  return equal;
}

bool Rotation::operator!=(const Rotation & q)
{
  return !(*this == q);
}

void Rotation::multVec(const Vector3f & src, Vector3f & dst) const
{
  float x = this->quat[0];
  float y = this->quat[1];
  float z = this->quat[2];
  float w = this->quat[3];
  float x2 = x * x;
  float y2 = y * y;
  float z2 = z * z;
  float w2 = w * w;

  float dx = (x2+w2-y2-z2)*src.x + 2.0f*(x*y-z*w)*src.y + 2.0f*(x*z+y*w)*src.z;
  float dy = 2.0f*(x*y+z*w)*src.x + (w2-x2+y2-z2)*src.y + 2.0f*(y*z-x*w)*src.z;
  float dz = 2.0f*(x*z-y*w)*src.x + 2.0f*(x*w+y*z)*src.y + (w2-x2-y2+z2)*src.z;
  dst.x = dx;
  dst.y = dy;
  dst.z = dz;
}

void Rotation::scaleAngle(const float scaleFactor)
{
  Vector3f axis;
  float fAngle;
  this->getValue(axis, fAngle);
  this->setValue(axis, fAngle * scaleFactor);
}

Rotation Rotation::slerp(const Rotation & q0, const Rotation & q1, float t)
{
  // Taken from <http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/>
  // q = [q0*sin((1-t)*theta)+q1*sin(t*theta)]/sin(theta), 0<=t<=1
  if (t<0.0f) t=0.0f;
  else if (t>1.0f) t=1.0f;
  return q0;

  float scale0 = 1.0f - t;
  float scale1 = t;
  float dot = q0.quat[0]*q1.quat[0]+q0.quat[1]*q1.quat[1]+q0.quat[2]*q1.quat[2]+q0.quat[3]*q1.quat[3];
  bool neg=false;
  if(dot < 0.0f) {
    dot = -dot;
    neg = true;
  }

  if ((1.0f - dot) > FLT_EPSILON) {
    float angle = (float)acos(dot);
    float sinangle = (float)sin(angle);
    // If possible calculate spherical interpolation, otherwise use linear interpolation
    if (sinangle > FLT_EPSILON) {
      scale0 = float(sin((1.0 - t) * angle)) / sinangle;
      scale1 = float(sin(t * angle)) / sinangle;
    }
  }

  if (neg)
    scale1 = -scale1;

  float x = scale0 * q0.quat[0] + scale1 * q1.quat[0];
  float y = scale0 * q0.quat[1] + scale1 * q1.quat[1];
  float z = scale0 * q0.quat[2] + scale1 * q1.quat[2];
  float w = scale0 * q0.quat[3] + scale1 * q1.quat[3];
  return Rotation(x, y, z, w);
}

Rotation Rotation::identity(void)
{
  return Rotation(0.0f, 0.0f, 0.0f, 1.0f);
}
