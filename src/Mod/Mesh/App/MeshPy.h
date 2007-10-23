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


#ifndef MESH_PY_H
#define MESH_PY_H

#include <Base/PyObjectBase.h>
#include "Core/MeshKernel.h"

namespace MeshCore {
  class MeshKernel;
}

namespace Mesh
{

//===========================================================================
// MeshPy - Python wrapper 
//===========================================================================

// The DocTypeStd python class 
class AppMeshExport MeshPy :public Base::PyObjectBase
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~MeshPy();

public:
  MeshPy(PyTypeObject *T = &Type);
  MeshPy(MeshCore::MeshKernel* pcMesh, PyTypeObject *T = &Type);
  static PyObject *PyMake(PyTypeObject*, PyObject*, PyObject*);
  static int PyInit(PyObject*, PyObject*, PyObject*);

  /// sets this object to a new mesh kernel, the old will be deleted!
  void setMesh(MeshCore::MeshKernel* pcMesh);
  const MeshCore::MeshKernel& getMesh(void) const;

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------

  virtual PyObject *_repr(void);  				// the representation
  PyObject *_getattr(char *attr);					// __getattr__ function
  int _setattr(char *attr, PyObject *value);		// __setattr__ function

  
  PYFUNCDEF_D(MeshPy,pointCount)
  PYFUNCDEF_D(MeshPy,facetCount)
  PYFUNCDEF_D(MeshPy,read)
  PYFUNCDEF_D(MeshPy,write)
  PYFUNCDEF_D(MeshPy,offset)
  PYFUNCDEF_D(MeshPy,offsetSpecial)
  PYFUNCDEF_D(MeshPy,unite)
  PYFUNCDEF_D(MeshPy,intersect)
  PYFUNCDEF_D(MeshPy,diff)
  PYFUNCDEF_D(MeshPy,outer)
  PYFUNCDEF_D(MeshPy,inner)
  PYFUNCDEF_D(MeshPy,coarsen)
  PYFUNCDEF_D(MeshPy,translate)
  PYFUNCDEF_D(MeshPy,rotate)
  PYFUNCDEF_D(MeshPy,transformToEigen)
  PYFUNCDEF_D(MeshPy,scale)
  PYFUNCDEF_D(MeshPy,transform)
  PYFUNCDEF_D(MeshPy,addFacet)
  PYFUNCDEF_D(MeshPy,addFacets)
  PYFUNCDEF_D(MeshPy,clear)
  PYFUNCDEF_D(MeshPy,copy)
  PYFUNCDEF_D(MeshPy,isSolid)
  PYFUNCDEF_D(MeshPy,hasNonManifolds)
  PYFUNCDEF_D(MeshPy,removeNonManifolds)
  PYFUNCDEF_D(MeshPy,hasSelfIntersections)
  PYFUNCDEF_D(MeshPy,fixSelfIntersections)
  PYFUNCDEF_D(MeshPy,testDelaunay)
  PYFUNCDEF_D(MeshPy,makeCutToolFromShape)
  PYFUNCDEF_D(MeshPy,flipNormals)
  PYFUNCDEF_D(MeshPy,hasNonUnifomOrientedFacets)
  PYFUNCDEF_D(MeshPy,countNonUnifomOrientedFacets)
  PYFUNCDEF_D(MeshPy,harmonizeNormals)
  PYFUNCDEF_D(MeshPy,countComponents)
  PYFUNCDEF_D(MeshPy,removeComponents)
  PYFUNCDEF_D(MeshPy,fillupHoles)
  PYFUNCDEF_D(MeshPy,fixIndices)
  PYFUNCDEF_D(MeshPy,fixDeformations)
  PYFUNCDEF_D(MeshPy,fixDegenerations)
  PYFUNCDEF_D(MeshPy,removeDuplicatedPoints)
  PYFUNCDEF_D(MeshPy,removeDuplicatedFacets)
  PYFUNCDEF_D(MeshPy,refine)
  PYFUNCDEF_D(MeshPy,optimizeTopology)
  PYFUNCDEF_D(MeshPy,optimizeEdges)
  PYFUNCDEF_D(MeshPy,splitEdge)

protected:
  MeshCore::MeshKernel *_pcMesh;
};

} //namespace Mesh

 

#endif // MESH_FEATURE_PY_H 
