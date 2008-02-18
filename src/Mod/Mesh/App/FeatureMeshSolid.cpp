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


namespace Mesh {
    const App::PropertyIntegerConstraint::Constraints intSampling = {0,1000,1};
    const App::PropertyFloatConstraint::Constraints floatRange = {0.0,1000.0,1.0};
}

using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Sphere, Mesh::Feature)

Sphere::Sphere(void)
{
    ADD_PROPERTY(Radius  ,(5.0));
    ADD_PROPERTY(Sampling  ,(50));
    Radius.setConstraints(&floatRange);
    Sampling.setConstraints(&intSampling);
}

short Sphere::mustExecute() const
{
    if (Radius.isTouched() || Sampling.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Sphere::execute(void)
{
    // load the 'BuildRegularGeoms' module
    Base::PyGILStateLocker lock;
    PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
    if (module) {
        // get the methods dictionary and search for the 'Sphere' method
        PyObject* dict = PyModule_GetDict(module);
        PyObject* func = PyDict_GetItemString(dict, "Sphere");
        // Create the mesh
        if (func) {
            PyObject* args = Py_BuildValue("(fi)",Radius.getValue(),Sampling.getValue());
            PyObject* result = PyEval_CallObject(func,args);
            // decrement the args and module reference
            Py_DECREF(args);
            Py_DECREF(module);

            // We know it's a list of facets
            if (result) {
                MeshPy* mesh = new MeshPy(new MeshObject);
                PyObject* args = Py_BuildValue("(O)",result);
                mesh->addFacets(args);
                Mesh.setValue(mesh->getMesh());
                Py_DECREF(args);
                Py_DECREF(mesh);
                Py_DECREF(result);
            }
        }
        else {
            PyErr_SetString(PyExc_AttributeError, "'BuildRegularGeoms' object has no attribute 'Sphere'");
        }
    }

    // Check for internal Python errors
    if (PyErr_Occurred()) {
        Base::Exception e; // do not use PyException since this clears the error indicator
        PyObject *errobj, *errdata, *errtraceback;
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        if (PyString_Check(errdata))
            e.setMessage( PyString_AsString( errdata ) );
        PyErr_Restore(errobj, errdata, errtraceback);
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
        throw e;
    }

    return App::DocumentObject::StdReturn;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Ellipsoid, Mesh::Feature)

Ellipsoid::Ellipsoid(void)
{
    ADD_PROPERTY(Radius1  ,(2.0));
    ADD_PROPERTY(Radius2  ,(4.0));
    ADD_PROPERTY(Sampling  ,(50));
    Radius1.setConstraints(&floatRange);
    Radius2.setConstraints(&floatRange);
    Sampling.setConstraints(&intSampling);
}

short Ellipsoid::mustExecute() const
{
    if (Radius1.isTouched() || 
        Radius2.isTouched() || 
        Sampling.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Ellipsoid::execute(void)
{
    // load the 'BuildRegularGeoms' module
    Base::PyGILStateLocker lock;
    PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
    if (module) {
        // get the methods dictionary and search for the 'Ellipsoid' method
        PyObject* dict = PyModule_GetDict(module);
        PyObject* func = PyDict_GetItemString(dict, "Ellipsoid");
        // Create the mesh
        if (func) {
            PyObject* args = Py_BuildValue("(ffi)",Radius1.getValue(),Radius2.getValue(),Sampling.getValue());
            PyObject* result = PyEval_CallObject(func,args);
            // decrement the args and module reference
            Py_DECREF(args);
            Py_DECREF(module);

            // We know it's a list of facets
            if (result) {
                MeshPy* mesh = new MeshPy(new MeshObject);
                PyObject* args = Py_BuildValue("(O)",result);
                mesh->addFacets(args);
                Mesh.setValue(mesh->getMesh() );
                Py_DECREF(args);
                Py_DECREF(mesh);
                Py_DECREF(result);
            }
        }
        else {
            PyErr_SetString(PyExc_AttributeError, "'BuildRegularGeoms' object has no attribute 'Ellipsoid'");
        }
    }

    // Check for internal Python errors
    if (PyErr_Occurred()) {
        Base::Exception e; // do not use PyException since this clears the error indicator
        PyObject *errobj, *errdata, *errtraceback;
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        if (PyString_Check(errdata))
            e.setMessage( PyString_AsString( errdata ) );
        PyErr_Restore(errobj, errdata, errtraceback);
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
        throw e;
    }

    return App::DocumentObject::StdReturn;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Cylinder, Mesh::Feature)

Cylinder::Cylinder(void)
{
    ADD_PROPERTY(Radius  ,(2.0));
    ADD_PROPERTY(Length  ,(10.0));
    ADD_PROPERTY(EdgeLength,(1.0));
    ADD_PROPERTY(Closed  ,(true));
    ADD_PROPERTY(Sampling  ,(50));
    Radius.setConstraints(&floatRange);
    Length.setConstraints(&floatRange);
    EdgeLength.setConstraints(&floatRange);
    Sampling.setConstraints(&intSampling);
}

short Cylinder::mustExecute() const
{
    if (Radius.isTouched() || 
        Length.isTouched() ||
        EdgeLength.isTouched() ||
        Closed.isTouched() ||
        Sampling.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Cylinder::execute(void)
{
    // load the 'BuildRegularGeoms' module
    Base::PyGILStateLocker lock;
    PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
    if (module) {
        // get the methods dictionary and search for the 'Cylinder' method
        PyObject* dict = PyModule_GetDict(module);
        PyObject* func = PyDict_GetItemString(dict, "Cylinder");
        // Create the mesh
        if (func) {
            PyObject* args = Py_BuildValue("(ffifi)",Radius.getValue(),Length.getValue(),
            Closed.getValue(),EdgeLength.getValue(),Sampling.getValue());
            PyObject* result = PyEval_CallObject(func,args);
            // decrement the args and module reference
            Py_DECREF(args);
            Py_DECREF(module);

            // We know it's a list of facets
            if (result) {
                MeshPy* mesh = new MeshPy(new MeshObject);
                PyObject* args = Py_BuildValue("(O)",result);
                mesh->addFacets(args);
                Mesh.setValue( mesh->getMesh() );
                Py_DECREF(args);
                Py_DECREF(mesh);
                Py_DECREF(result);
            }
        }
        else {
            PyErr_SetString(PyExc_AttributeError, "'BuildRegularGeoms' object has no attribute 'Cylinder'");
        }
    }

    // Check for internal Python errors
    if (PyErr_Occurred()) {
        Base::Exception e; // do not use PyException since this clears the error indicator
        PyObject *errobj, *errdata, *errtraceback;
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        if (PyString_Check(errdata))
            e.setMessage( PyString_AsString( errdata ) );
        PyErr_Restore(errobj, errdata, errtraceback);
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
        throw e;
    }

    return App::DocumentObject::StdReturn;
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
    Radius1.setConstraints(&floatRange);
    Radius2.setConstraints(&floatRange);
    Length.setConstraints(&floatRange);
    EdgeLength.setConstraints(&floatRange);
    Sampling.setConstraints(&intSampling);
}

short Cone::mustExecute() const
{
    if (Radius1.isTouched() || 
        Radius2.isTouched() || 
        Length.isTouched()  ||
        EdgeLength.isTouched() ||
        Closed.isTouched()  ||
        Sampling.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Cone::execute(void)
{
    // load the 'BuildRegularGeoms' module
    Base::PyGILStateLocker lock;
    PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
    if (module) {
        // get the methods dictionary and search for the 'Cone' method
        PyObject* dict = PyModule_GetDict(module);
        PyObject* func = PyDict_GetItemString(dict, "Cone");
        // Create the mesh
        if (func) {
            PyObject* args = Py_BuildValue("(fffifi)",Radius1.getValue(),Radius2.getValue(),Length.getValue(),
            Closed.getValue(),EdgeLength.getValue(),Sampling.getValue());
            PyObject* result = PyEval_CallObject(func,args);
            // decrement the args and module reference
            Py_DECREF(args);
            Py_DECREF(module);

            // We know it's a list of facets
            if (result) {
                MeshPy* mesh = new MeshPy(new MeshObject);
                PyObject* args = Py_BuildValue("(O)",result);
                mesh->addFacets(args);
                Mesh.setValue( mesh->getMesh() );
                Py_DECREF(args);
                Py_DECREF(mesh);
                Py_DECREF(result);
            }
        }
        else {
            PyErr_SetString(PyExc_AttributeError, "'BuildRegularGeoms' object has no attribute 'Cone'");
        }
    }

    // Check for internal Python errors
    if (PyErr_Occurred()) {
        Base::Exception e; // do not use PyException since this clears the error indicator
        PyObject *errobj, *errdata, *errtraceback;
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        if (PyString_Check(errdata))
            e.setMessage( PyString_AsString( errdata ) );
        PyErr_Restore(errobj, errdata, errtraceback);
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
        throw e;
    }

    return App::DocumentObject::StdReturn;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Torus, Mesh::Feature)

Torus::Torus(void)
{
    ADD_PROPERTY(Radius1  ,(10.0));
    ADD_PROPERTY(Radius2  ,(2.0));
    ADD_PROPERTY(Sampling  ,(50));
    Radius1.setConstraints(&floatRange);
    Radius2.setConstraints(&floatRange);
    Sampling.setConstraints(&intSampling);
}

short Torus::mustExecute() const
{
    if (Radius1.isTouched() || 
        Radius2.isTouched() || 
        Sampling.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Torus::execute(void)
{
    // load the 'BuildRegularGeoms' module
    Base::PyGILStateLocker lock;
    PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
    if (module) {
        // get the methods dictionary and search for the 'Torus' method
        PyObject* dict = PyModule_GetDict(module);
        PyObject* func = PyDict_GetItemString(dict, "Toroid");
        // Create the mesh
        if (func) {
            PyObject* args = Py_BuildValue("(ffi)",Radius1.getValue(),Radius2.getValue(),Sampling.getValue());
            PyObject* result = PyEval_CallObject(func,args);
            // decrement the args and module reference
            Py_DECREF(args);
            Py_DECREF(module);

            // We know it's a list of facets
            if (result) {
                MeshPy* mesh = new MeshPy(new MeshObject);
                PyObject* args = Py_BuildValue("(O)",result);
                mesh->addFacets(args);
                Mesh.setValue( mesh->getMesh() );
                Py_DECREF(args);
                Py_DECREF(mesh);
                Py_DECREF(result);
            }
        }
        else {
            PyErr_SetString(PyExc_AttributeError, "'BuildRegularGeoms' object has no attribute 'Toroid'");
        }
    }

    // Check for internal Python errors
    if (PyErr_Occurred()) {
        Base::Exception e; // do not use PyException since this clears the error indicator
        PyObject *errobj, *errdata, *errtraceback;
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        if (PyString_Check(errdata))
            e.setMessage( PyString_AsString( errdata ) );
        PyErr_Restore(errobj, errdata, errtraceback);
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
        throw e;
    }

    return App::DocumentObject::StdReturn;
}

// -------------------------------------------------------------

PROPERTY_SOURCE(Mesh::Cube, Mesh::Feature)

Cube::Cube(void)
{
    ADD_PROPERTY(Length  ,(10.0));
    ADD_PROPERTY(Width  ,(10.0));
    ADD_PROPERTY(Height  ,(10.0));
    Length.setConstraints(&floatRange);
    Width.setConstraints(&floatRange);
    Height.setConstraints(&floatRange);
}

short Cube::mustExecute() const
{
    if (Length.isTouched() || 
        Width.isTouched() || 
        Height.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Cube::execute(void)
{
    // load the 'BuildRegularGeoms' module
    Base::PyGILStateLocker lock;
    PyObject* module = PyImport_ImportModule("BuildRegularGeoms");
    if (module) {
        // get the methods dictionary and search for the 'Cube' method
        PyObject* dict = PyModule_GetDict(module);
        PyObject* func = PyDict_GetItemString(dict, "Cube");
        // Create the mesh
        if (func) {
            PyObject* args = Py_BuildValue("(fff)",Length.getValue(),Width.getValue(),Height.getValue());
            PyObject* result = PyEval_CallObject(func,args);
            // decrement the args and module reference
            Py_DECREF(args);
            Py_DECREF(module);

            // We know it's a list of facets
            if (result) {
                MeshPy* mesh = new MeshPy(new MeshObject);
                PyObject* args = Py_BuildValue("(O)",result);
                mesh->addFacets(args);
                Mesh.setValue( mesh->getMesh() );
                Py_DECREF(args);
                Py_DECREF(mesh);
                Py_DECREF(result);
            }
        }
        else {
            PyErr_SetString(PyExc_AttributeError, "'BuildRegularGeoms' object has no attribute 'Cube'");
        }
    } 
  
    // Check for internal Python errors
    if (PyErr_Occurred()) {
        Base::Exception e; // do not use PyException since this clears the error indicator
        PyObject *errobj, *errdata, *errtraceback;
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        if (PyString_Check(errdata))
            e.setMessage( PyString_AsString( errdata ) );
        PyErr_Restore(errobj, errdata, errtraceback);
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
        throw e;
    }

    return App::DocumentObject::StdReturn;
}
