/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef MESH_CURVATURE_H
#define MESH_CURVATURE_H
#if 0
#ifndef _PreComp_
# include <vector>

#endif

#include <Base/Exception.h>

namespace Base{
  class Vector3D;
  class Matrix4D;
}
using Base::Vector3D;
using Base::Matrix4D;


namespace Mesh
{

class MeshPropertyCurvature;
class MeshWithProperty;

/** 
 * The MeshCurvaturePropertyError class indicates an error if the required property object
 * couldn't be found.
 * @author Werner Mayer
 */
class MeshCurvaturePropertyError : public Base::Exception
{
public:
	MeshCurvaturePropertyError(const char * sMessage);
  MeshCurvaturePropertyError(void);
  ~MeshCurvaturePropertyError() throw() {}
};

/** 
 * The MeshKernelError class indicates an error if MeshWithProperty has no MeshKernel.
 * @author Werner Mayer
 */
class MeshKernelError : public Base::Exception
{
public:
	MeshKernelError(const char * sMessage);
  MeshKernelError(void);
  ~MeshKernelError() throw() {}
};

/** 
 * The MeshCurvature class computes the curvature information for each element of a mesh.
 * Therefore the Compute() function has to be implemented by every subclass.
 * @author Werner Mayer
 */
class MeshCurvature
{
public:
  MeshCurvature( MeshWithProperty& rclMesh );
  virtual ~MeshCurvature() {}

  virtual bool Compute() = 0;

protected:
  MeshWithProperty& _rclMesh;
  MeshPropertyCurvature* _curvature;
};

/**
 * The MeshCurvaturePerVertex class computes the curvature per vertex. 
 * This class uses the approach of Dave Eberly's MeshCurvature implementation.
 * @author Werner Mayer
 */
class MeshCurvaturePerVertex : public MeshCurvature
{
public:
  MeshCurvaturePerVertex( MeshWithProperty& rclMesh );
  ~MeshCurvaturePerVertex();

  bool Compute();
};

/**
 * The MeshCurvaturePerFace class computes the curvature per face. 
 * @author Werner Mayer
 */
class MeshCurvaturePerFace : public MeshCurvature
{
public:
  MeshCurvaturePerFace(MeshWithProperty& rclMesh);
  ~MeshCurvaturePerFace();

  bool Compute();

protected:
  unsigned long RequiredPoints() const;
  void ExtractCurvature(unsigned long, const std::vector<Vector3D>&, const Vector3D& );
};

} // namespace Mesh

#endif
#endif // MESH_CURVATURE_H
