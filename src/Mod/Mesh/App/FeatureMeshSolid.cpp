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
#ifndef _PreComp_
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <App/Document.h>

#include "FeatureMeshSolid.h"
#include "MeshPy.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Sphere, Mesh::Feature)

Sphere::Sphere(void)
{
  ADD_PROPERTY(Radius  ,(5.0));
  ADD_PROPERTY(Sampling  ,(50));
}

int Sphere::execute(void)
{
  // load the 'BuildRegularGeoms' module
  PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
  if ( module ) {
    // get the methods dictionary and search for the 'Sphere' method
    PyObject* dict = PyModule_GetDict(module);
    PyObject* func = PyDict_GetItemString(dict, "Sphere");
    // Create the mesh
    PyObject* args = Py_BuildValue("(fi)",Radius.getValue(),Sampling.getValue());
    PyObject* result = PyEval_CallObject(func,args);
    // decrement the args and module reference
    Py_DECREF(args);
    Py_DECREF(module);

    // We know it's a list of facets
    if ( result ) {
      MeshPy* mesh = new MeshPy;
      PyObject* args = Py_BuildValue("(O)",result);
      mesh->addFacets(args);
      Mesh.setValue( mesh->getMesh() );
      Py_DECREF(args);
      Py_DECREF(mesh);
      Py_DECREF(result);
    }
  }

  return 0;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Ellipsoid, Mesh::Feature)

Ellipsoid::Ellipsoid(void)
{
  ADD_PROPERTY(Radius1  ,(2.0));
  ADD_PROPERTY(Radius2  ,(4.0));
  ADD_PROPERTY(Sampling  ,(50));
}

int Ellipsoid::execute(void)
{
  // load the 'BuildRegularGeoms' module
  PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
  if ( module ) {
    // get the methods dictionary and search for the 'Ellipsoid' method
    PyObject* dict = PyModule_GetDict(module);
    PyObject* func = PyDict_GetItemString(dict, "Ellipsoid");
    // Create the mesh
    PyObject* args = Py_BuildValue("(ffi)",Radius1.getValue(),Radius2.getValue(),Sampling.getValue());
    PyObject* result = PyEval_CallObject(func,args);
    // decrement the args and module reference
    Py_DECREF(args);
    Py_DECREF(module);

    // We know it's a list of facets
    if ( result ) {
      MeshPy* mesh = new MeshPy;
      PyObject* args = Py_BuildValue("(O)",result);
      mesh->addFacets(args);
      Mesh.setValue( mesh->getMesh() );
      Py_DECREF(args);
      Py_DECREF(mesh);
      Py_DECREF(result);
    }
  }

  return 0;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Cylinder, Mesh::Feature)

Cylinder::Cylinder(void)
{
  ADD_PROPERTY(Radius  ,(10.0));
  ADD_PROPERTY(Length  ,(2.0));
  ADD_PROPERTY(EdgeLength,(1.0));
  ADD_PROPERTY(Closed  ,(true));
  ADD_PROPERTY(Sampling  ,(50));
}

int Cylinder::execute(void)
{
  // load the 'BuildRegularGeoms' module
  PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
  if ( module ) {
    // get the methods dictionary and search for the 'Cylinder' method
    PyObject* dict = PyModule_GetDict(module);
    PyObject* func = PyDict_GetItemString(dict, "Cylinder");
    // Create the mesh
    PyObject* args = Py_BuildValue("(ffifi)",Radius.getValue(),Length.getValue(),
      Closed.getValue(),EdgeLength.getValue(),Sampling.getValue());
    PyObject* result = PyEval_CallObject(func,args);
    // decrement the args and module reference
    Py_DECREF(args);
    Py_DECREF(module);

    // We know it's a list of facets
    if ( result ) {
      MeshPy* mesh = new MeshPy;
      PyObject* args = Py_BuildValue("(O)",result);
      mesh->addFacets(args);
      Mesh.setValue( mesh->getMesh() );
      Py_DECREF(args);
      Py_DECREF(mesh);
      Py_DECREF(result);
    }
  }

  return 0;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Cone, Mesh::Feature)

Cone::Cone(void)
{
  ADD_PROPERTY(Radius1  ,(2.0));
  ADD_PROPERTY(Radius2  ,(4.0));
  ADD_PROPERTY(Length  ,(10.0));
  ADD_PROPERTY(EdgeLength,(1.0));
  ADD_PROPERTY(Closed  ,(true));
  ADD_PROPERTY(Sampling  ,(50));
}

int Cone::execute(void)
{
  // load the 'BuildRegularGeoms' module
  PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
  if ( module ) {
    // get the methods dictionary and search for the 'Cone' method
    PyObject* dict = PyModule_GetDict(module);
    PyObject* func = PyDict_GetItemString(dict, "Cone");
    // Create the mesh
    PyObject* args = Py_BuildValue("(fffifi)",Radius1.getValue(),Radius2.getValue(),Length.getValue(),
      Closed.getValue(),EdgeLength.getValue(),Sampling.getValue());
    PyObject* result = PyEval_CallObject(func,args);
    // decrement the args and module reference
    Py_DECREF(args);
    Py_DECREF(module);

    // We know it's a list of facets
    if ( result ) {
      MeshPy* mesh = new MeshPy;
      PyObject* args = Py_BuildValue("(O)",result);
      mesh->addFacets(args);
      Mesh.setValue( mesh->getMesh() );
      Py_DECREF(args);
      Py_DECREF(mesh);
      Py_DECREF(result);
    }
  }

  return 0;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Torus, Mesh::Feature)

Torus::Torus(void)
{
  ADD_PROPERTY(Radius1  ,(10.0));
  ADD_PROPERTY(Radius2  ,(2.0));
  ADD_PROPERTY(Sampling  ,(50));
}

int Torus::execute(void)
{
  // load the 'BuildRegularGeoms' module
  PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
  if ( module ) {
    // get the methods dictionary and search for the 'Torus' method
    PyObject* dict = PyModule_GetDict(module);
    PyObject* func = PyDict_GetItemString(dict, "Toroid");
    // Create the mesh
    PyObject* args = Py_BuildValue("(ffi)",Radius1.getValue(),Radius2.getValue(),Sampling.getValue());
    PyObject* result = PyEval_CallObject(func,args);
    // decrement the args and module reference
    Py_DECREF(args);
    Py_DECREF(module);

    // We know it's a list of facets
    if ( result ) {
      MeshPy* mesh = new MeshPy;
      PyObject* args = Py_BuildValue("(O)",result);
      mesh->addFacets(args);
      Mesh.setValue( mesh->getMesh() );
      Py_DECREF(args);
      Py_DECREF(mesh);
      Py_DECREF(result);
    }
  }

  return 0;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Cube, Mesh::Feature)

Cube::Cube(void)
{
  ADD_PROPERTY(Length  ,(10.0));
  ADD_PROPERTY(Width  ,(10.0));
  ADD_PROPERTY(Height  ,(10.0));
}

int Cube::execute(void)
{
  // load the 'BuildRegularGeoms' module
  PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
  if ( module ) {
    // get the methods dictionary and search for the 'Cube' method
    PyObject* dict = PyModule_GetDict(module);
    PyObject* func = PyDict_GetItemString(dict, "Cube");
    // Create the mesh
    PyObject* args = Py_BuildValue("(fff)",Length.getValue(),Width.getValue(),Height.getValue());
    PyObject* result = PyEval_CallObject(func,args);
    // decrement the args and module reference
    Py_DECREF(args);
    Py_DECREF(module);

    // We know it's a list of facets
    if ( result ) {
      MeshPy* mesh = new MeshPy;
      PyObject* args = Py_BuildValue("(O)",result);
      mesh->addFacets(args);
      Mesh.setValue( mesh->getMesh() );
      Py_DECREF(args);
      Py_DECREF(mesh);
      Py_DECREF(result);
    }
  }

  return 0;
}
