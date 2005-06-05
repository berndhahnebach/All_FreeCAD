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


#ifndef __VIEW_PROJ_H__
#define __VIEW_PROJ_H__

#include "Vector3D.h"
#include "Tools2D.h"
#include "Matrix.h"

namespace Mesh {

/**
 * Abstract base class for all project methods.
 */
class AppMeshExport ViewProjMethod
{
public:
  virtual ~ViewProjMethod(){};
  virtual bool isValid() const { return true; };
  /** Convert 3D Point to 2D projection plane */
  virtual Vector3D operator()(const Vector3D &rclPt) const = 0; 
  /** Convert a 2D Point on the projection plane in 3D space */
  virtual Vector3D inverse (const Vector2D &rclPt) const = 0;   
  /** Calculate the projection (+ mapping) matrix */
  virtual Matrix4D getProjectionMatrix (void) const = 0; 

protected:
  ViewProjMethod(){};
};

/**
 * The ViewProjMatrix class returns the result of the multiplication
 * of the 3D vector and the view transformation matrix..
 */
class AppMeshExport ViewProjMatrix : public ViewProjMethod
{
public:
  ViewProjMatrix (const Matrix4D &rclMtx) : _clMtx(rclMtx) {  _clMtxInv = _clMtx; _clMtxInv.Inverse(); };
  virtual ~ViewProjMatrix(){};

  inline Vector3D operator()(const Vector3D &rclPt) const;
  inline Vector3D inverse (const Vector2D &rclPt) const;

  Matrix4D getProjectionMatrix (void) const { return _clMtx; }

protected:
  Matrix4D _clMtx, _clMtxInv;
};

inline Vector3D ViewProjMatrix::operator()(const Vector3D &rclPt) const
{
  return Vector3D(_clMtx * rclPt);
}

inline Vector3D ViewProjMatrix::inverse (const Vector2D &rclPt) const
{
  Vector3D clPt(rclPt.fX, rclPt.fY, 0.0f);
  return Vector3D(_clMtxInv * clPt);
}

} // namespace Mesh

#endif //__VIEW_PROJ_H__ 
