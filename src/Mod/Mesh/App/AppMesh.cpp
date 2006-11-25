/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <Python.h>
#endif

#include <Base/Console.h>

#include "Mesh.h"
#include "MeshPy.h"
#include "FeatureMeshImport.h"
#include "FeatureMeshExport.h"
#include "FeatureMeshTransform.h"
#include "FeatureMeshTransformDemolding.h"
#include "FeatureMeshCurvature.h"
#include "FeatureMeshSegmentByMesh.h"
#include "FeatureMeshSetOperations.h"
#include "FeatureMeshDefects.h"
#include "FeatureMeshSolid.h"

/* registration table  */
extern struct PyMethodDef Mesh_Import_methods[];


PyDoc_STRVAR(module_doc,
"This is a module working with meshes.");

/* Python entry */
extern "C" {
void AppMeshExport initMesh() {

  Base::Console().Log("Mod: Loading Mesh module... done\n");

  PyObject* meshModule = Py_InitModule3("Mesh", Mesh_Import_methods, module_doc);   /* mod name, table ptr */

  // NOTE: To finish the initialization of our own type objects we must
  // call PyType_Ready, otherwise we run into a segmentation fault, later on.
  // This function is responsible for adding inherited slots from a type's base class.
  PyObject* pyMeshType = (PyObject *)&Mesh::MeshPy::Type;
  if(PyType_Ready(&Mesh::MeshPy::Type) < 0) return;
  PyModule_AddObject(meshModule, "mesh", pyMeshType);

  Mesh::PropertyNormalList    ::init();
  Mesh::PropertyCurvatureList ::init();
  Mesh::PropertyMeshKernel    ::init();

  Mesh::Feature               ::init();
  Mesh::Import                ::init();
  Mesh::Export                ::init();
  Mesh::Transform             ::init();
  Mesh::TransformDemolding    ::init();
  Mesh::Curvature             ::init();
  Mesh::SegmentByMesh         ::init();
  Mesh::SetOperations         ::init();
  Mesh::FixDefects            ::init();
  Mesh::HarmonizeNormals      ::init();
  Mesh::FlipNormals           ::init();
  Mesh::FixNonManifolds       ::init();
  Mesh::FixDuplicatedFaces    ::init();
  Mesh::FixDuplicatedPoints   ::init();
  Mesh::FixDegenerations      ::init();
  Mesh::FixDeformations       ::init();
  Mesh::FixIndices            ::init();
  Mesh::FillHoles             ::init();
  Mesh::RemoveComponents      ::init();

  Mesh::Sphere                ::init();
  Mesh::Ellipsoid             ::init();
  Mesh::Cylinder              ::init();
  Mesh::Cone                  ::init();
  Mesh::Torus                 ::init();
  Mesh::Cube                  ::init();
    
  return;
}


} // extern "C" 
