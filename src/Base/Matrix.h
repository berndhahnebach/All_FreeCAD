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


#ifndef MATRIX_H
#define MATRIX_H

//#include "Definitions.h"
#include <math.h>
#include <stdio.h>

#include "Vector3D.h"
//#include "Stream.h"

namespace Base {

class   Matrix4D;
typedef Matrix4D* PMatrix4D;
typedef Matrix4D& RMatrix4D;

/**
 * The Matrix4D class.
 */
class BaseExport Matrix4D
{
public:
  /// Construction
  Matrix4D (void);
  /// Construction
  Matrix4D (const Matrix4D& rclMtrx);
  /// Destruction
  virtual ~Matrix4D () {};

  /** @name Operators */
  //@{
  /// Matrix multiplication
  inline Matrix4D& operator*= (const Matrix4D& rclMtrx);
  /// Assignment
  inline Matrix4D& operator=  (const Matrix4D& rclMtrx);
  /// Matrix multiplication
  inline Matrix4D  operator*  (const Matrix4D& rclMtrx);
  /// Multiplication matrix with vector 
  inline Vector3D  operator*  (const Vector3D& rclVct) const;
  /// Comparison
  inline bool      operator!= (const Matrix4D& rclMtrx);
  /// Comparison
  inline bool      operator== (const Matrix4D& rclMtrx);
  /// Index operator
  inline double*    operator[] (unsigned short usNdx);
  /// Index operator
  inline const double*    operator[] (unsigned short usNdx) const;

  // friend
  inline friend Vector3D& operator*= (Vector3D& rclVect, const Matrix4D& rclMtrx);
  //@}

  /** @name I/O methods 
  //@{
  /// Binary streaming of data
  DataStream& SaveData (DataStream& ofs);
  /// Binary streaming of data
  DataStream& LoadData (DataStream& ifs);
  //@}*/
  /// get the matrix in OpenGL style
  void   getGLMatrix (double dMtrx[16]) const;
  /// set the matrix in OpenGL style
  void   setGLMatrix (const double dMtrx[16]);

  virtual unsigned long GetMemSpace (void);

  /** @name Manipulation */
  //@{
  /// Makes unity matrix
  void unity        (void);
  void SetMoveX     (float fMove);
  void SetMoveY     (float fMove);
  void SetMoveZ     (float fMove);
  void SetMove      (const Vector3D& rclVct);
  void SetScaleX    (float fScale);
  void SetScaleY    (float fScale);
  void SetScaleZ    (float fScale);
  void SetScale     (const Vector3D& rclVct);
  void SetRotX      (float fAngle);
  void SetRotY      (float fAngle);
  void SetRotZ      (float fAngle);
  /// Rotation around an arbitrary axis passing the origin.
  void SetRotLine   (const Vector3D& rclVct, float fAngle);
  /// Rotation around an arbitrary axis that needn't necessarily pass the origin.
  void SetRotLine   (const Vector3D& rclBase, const Vector3D& rclDir, float fAngle);
  void Inverse      (void);
  void InverseGauss (void);
  void Transpose    (void);
  //@}

  void Print        (void) const;
  
private:
  double  dMtrx4D[4][4];
};

inline Matrix4D& Matrix4D::operator *= (const Matrix4D& rclMtrx)
{
  Matrix4D  clMat;
  short     ie, iz, is;

  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++) {
      clMat.dMtrx4D[iz][is] = 0;
      for (ie = 0; ie < 4; ie++)
        clMat.dMtrx4D[iz][is] += dMtrx4D[iz][ie] * 
                          rclMtrx.dMtrx4D[ie][is];
    }

  (*this) = clMat;
 
  return *this;
}

inline Matrix4D Matrix4D::operator * (const Matrix4D& rclMtrx)
{
  Matrix4D  clMat;
  short     ie, iz, is;

  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++) {
      clMat.dMtrx4D[iz][is] = 0;
      for (ie = 0; ie < 4; ie++)
       	clMat.dMtrx4D[iz][is] += dMtrx4D[iz][ie] * 
                          rclMtrx.dMtrx4D[ie][is];
    }

  return clMat;
}

inline Matrix4D& Matrix4D::operator= (const Matrix4D& rclMtrx)
{
  short     iz, is;

  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++) 
  {
#if _DEBUG
    assert( ! _isnan(rclMtrx.dMtrx4D[iz][is]) );
#endif
    dMtrx4D[iz][is] = rclMtrx.dMtrx4D[iz][is];
  }
  
  return *this;
}

inline Vector3D Matrix4D::operator* (const Vector3D& rclVct) const
{
  return Vector3D((float)(dMtrx4D[0][0]*rclVct.x + dMtrx4D[0][1]*rclVct.y + 
                          dMtrx4D[0][2]*rclVct.z + dMtrx4D[0][3]),
                  (float)(dMtrx4D[1][0]*rclVct.x + dMtrx4D[1][1]*rclVct.y + 
                          dMtrx4D[1][2]*rclVct.z + dMtrx4D[1][3]),
                  (float)(dMtrx4D[2][0]*rclVct.x + dMtrx4D[2][1]*rclVct.y + 
                          dMtrx4D[2][2]*rclVct.z + dMtrx4D[2][3]));
}

inline bool Matrix4D::operator== (const Matrix4D& rclMtrx)
{
  short     iz, is;
  bool      bRet = true;

  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++) 
      if (fabs(dMtrx4D[iz][is] - rclMtrx.dMtrx4D[iz][is]) > FLOAT_EPS)
        bRet = false;

  return bRet;
}

inline bool Matrix4D::operator!= (const Matrix4D& rclMtrx)
{
  return !( (*this) == rclMtrx );
}

inline Vector3D& operator*= (Vector3D& rclVect,
                              const Matrix4D& rclMtrx)
{
  rclVect = rclMtrx * rclVect;
  return rclVect;
}

inline double* Matrix4D::operator[] (unsigned short usNdx)
{
  return dMtrx4D[usNdx];
}

inline const double* Matrix4D::operator[] (unsigned short usNdx) const
{
  return dMtrx4D[usNdx];
}

} // namespace Mesh

#endif // MATRIX_H 


