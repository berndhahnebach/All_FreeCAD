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
# include <gts.h>
# include <stdio.h>
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/FileInfo.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <App/Property.h>
#include <Mod/Part/App/TopologyPy.h>

#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "MeshPy.h"
#include "Mesh.h"
#include "MeshAlgos.h"
#include "FeatureMeshImport.h"

using namespace Mesh;
using namespace MeshCore;


/* module functions */
static PyObject * read(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;

    PY_TRY {
        std::auto_ptr<MeshObject> mesh(new MeshObject);
        mesh->load(Name);
        return new MeshPy(mesh.release());
    } PY_CATCH;

    Py_Return;
}

static PyObject * open(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;

    PY_TRY {
        //Base::Console().Log("Open in Mesh with %s",Name);
        Base::FileInfo file(Name);

        // create new document and add Import feature
        App::Document *pcDoc = App::GetApplication().newDocument("Unnamed");
        // The feature checks the file extension
        Mesh::Import *pcFeature = (Mesh::Import*)pcDoc->addObject("Mesh::Import",file.fileNamePure().c_str());
        pcFeature->FileName.setValue(Name);
        pcFeature->Label.setValue(file.fileNamePure().c_str());
        pcDoc->recompute();
    } PY_CATCH;

    Py_Return;    
}


/* module functions */
static PyObject * insert(PyObject *self, PyObject *args)
{
    const char* Name;
    const char* DocName=0;
    PyObject *pcObj = 0;

    if (!PyArg_ParseTuple(args, "s|s",&Name,&DocName))
        if (!PyArg_ParseTuple(args, "O!|s", &(MeshPy::Type), &pcObj,&DocName))
            return NULL;
    PyErr_Clear();
    PY_TRY {
        App::Document *pcDoc = 0;
        if (DocName)
            pcDoc = App::GetApplication().getDocument(DocName);
        else
            pcDoc = App::GetApplication().getActiveDocument();

        if (!pcDoc) {
            if (DocName)
                PyErr_Format(PyExc_Exception, "Insert called to the non-existing document '%s'", DocName);
            else
                PyErr_SetString(PyExc_Exception, "No active document found");
            return NULL;
        }

        if (pcObj) {
            MeshPy* pMesh = static_cast<MeshPy*>(pcObj);
            Mesh::Feature *pcFeature = (Mesh::Feature *)pcDoc->addObject("Mesh::Feature", "Mesh");
            // copy the data
            MeshObject* mesh = new MeshObject(*pMesh->getMeshObjectPtr());
            pcFeature->Mesh.setValue(mesh);
        }
        else {
            // add Import feature (the feature checks the file extension)
            Base::FileInfo file(Name);
            Mesh::Import *pcFeature = (Mesh::Import *)pcDoc->addObject("Mesh::Import", file.fileNamePure().c_str());
            pcFeature->FileName.setValue(Name);
            pcFeature->Label.setValue(file.fileNamePure().c_str());
            pcDoc->recompute();
        }
    } PY_CATCH;

    Py_Return;
}
/*
static PyObject * 
show(PyObject *self, PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PY_TRY {
        App::Document *pcDoc = App::GetApplication().getActiveDocument(); 	 
        if (!pcDoc)
            pcDoc = App::GetApplication().newDocument();
        MeshPy* pMesh = static_cast<MeshPy*>(pcObj);
        Mesh::Feature *pcFeature = (Mesh::Feature *)pcDoc->addObject("Mesh::Feature", "Mesh");
        // copy the data
        MeshObject* mesh = new MeshObject(*pMesh->getMeshObjectPtr());
        pcFeature->Mesh.setValue(mesh);
        pcDoc->recompute();
    } PY_CATCH;

    Py_Return;
}
*/
static PyObject *
createPlane(PyObject *self, PyObject *args)
{
    float x=1,y=0,z=0;
    if (!PyArg_ParseTuple(args, "|fff",&x,&y,&z))     // convert args: Python->C 
        return NULL;                                   // NULL triggers exception 

    if(y==0) 
        y=x;

    float hx = x/2.0f;
    float hy = y/2.0f;

    PY_TRY {
        std::vector<MeshCore::MeshGeomFacet> TriaList;
        TriaList.push_back(MeshCore::MeshGeomFacet(Base::Vector3f(-hx, -hy, 0.0),Base::Vector3f(hx, hy, 0.0),Base::Vector3f(-hx, hy, 0.0)));
        TriaList.push_back(MeshCore::MeshGeomFacet(Base::Vector3f(-hx, -hy, 0.0),Base::Vector3f(hx, -hy, 0.0),Base::Vector3f(hx, hy, 0.0)));

        std::auto_ptr<MeshObject> mesh(new MeshObject);
        mesh->addFacets(TriaList);
        return new MeshPy(mesh.release());
    } PY_CATCH;
}

static PyObject *
createSphere(PyObject *self, PyObject *args)
{
    float radius = 5.0f;
    int sampling = 50;
    if (!PyArg_ParseTuple(args, "|fi",&radius,&sampling))     // convert args: Python->C 
        return NULL;                                   // NULL triggers exception 

    PY_TRY {
        MeshObject* mesh = MeshObject::createSphere(radius, sampling);
        if (!mesh) {
            PyErr_SetString(PyExc_Exception, "Creation of sphere failed");
            return NULL;
        }
        return new MeshPy(mesh);
    } PY_CATCH;
}

static PyObject *
createEllipsoid(PyObject *self, PyObject *args)
{
    float radius1 = 2.0f;
    float radius2 = 4.0f;
    int sampling = 50;
    if (!PyArg_ParseTuple(args, "|ffi",&radius1,&radius2,&sampling))     // convert args: Python->C 
        return NULL;                                   // NULL triggers exception 

    PY_TRY {
        MeshObject* mesh = MeshObject::createEllipsoid(radius1, radius2, sampling);
        if (!mesh) {
            PyErr_SetString(PyExc_Exception, "Creation of ellipsoid failed");
            return NULL;
        }
        return new MeshPy(mesh);
    } PY_CATCH;
}

static PyObject *
createCylinder(PyObject *self, PyObject *args)
{
    float radius = 2.0f;
    float length = 10.0f;
    int closed = 1;
    float edgelen = 1.0f;
    int sampling = 50;
    if (!PyArg_ParseTuple(args, "|ffifi",&radius,&length,&closed,&edgelen,&sampling))     // convert args: Python->C 
        return NULL;                                   // NULL triggers exception 

    PY_TRY {
        MeshObject* mesh = MeshObject::createCylinder(radius, length, closed, edgelen, sampling);
        if (!mesh) {
            PyErr_SetString(PyExc_Exception, "Creation of cylinder failed");
            return NULL;
        }
        return new MeshPy(mesh);
    } PY_CATCH;
}

static PyObject *
createCone(PyObject *self, PyObject *args)
{
    float radius1 = 2.0f;
    float radius2 = 4.0f;
    float len = 10.0f;
    int closed = 1;
    float edgelen = 1.0f;
    int sampling = 50;
    if (!PyArg_ParseTuple(args, "|fffifi",&radius1,&radius2,&len,&closed,&edgelen,&sampling))     // convert args: Python->C 
        return NULL;                                   // NULL triggers exception 

    PY_TRY {
        MeshObject* mesh = MeshObject::createCone(radius1, radius2, len, closed, edgelen, sampling);
        if (!mesh) {
            PyErr_SetString(PyExc_Exception, "Creation of cone failed");
            return NULL;
        }
        return new MeshPy(mesh);
    } PY_CATCH;
}

static PyObject *
createTorus(PyObject *self, PyObject *args)
{
    float radius1 = 10.0f;
    float radius2 = 2.0f;
    int sampling = 50;
    if (!PyArg_ParseTuple(args, "|ffi",&radius1,&radius2,&sampling))     // convert args: Python->C 
        return NULL;                                   // NULL triggers exception 

    PY_TRY {
        MeshObject* mesh = MeshObject::createTorus(radius1, radius2, sampling);
        if (!mesh) {
            PyErr_SetString(PyExc_Exception, "Creation of torus failed");
            return NULL;
        }
        return new MeshPy(mesh);
    } PY_CATCH;
}

static PyObject * 
createBox(PyObject *self, PyObject *args)
{
    float length = 10.0f;
    float width = 10.0f;
    float height = 10.0f;
    if (!PyArg_ParseTuple(args, "|fff",&length,&width,&height))     // convert args: Python->C 
        return NULL;                                   // NULL triggers exception 

    PY_TRY {
        MeshObject* mesh = MeshObject::createCube(length, width, height);
        if (!mesh) {
            PyErr_SetString(PyExc_Exception, "Creation of box failed");
            return NULL;
        }
        return new MeshPy(mesh);
    } PY_CATCH;
}

static PyObject *                        
loftOnCurve(PyObject *self, PyObject *args)
{
/*
  Part::TopoShapePy   *pcObject;
  PyObject *pcTopoObj,*pcListObj;
  float x=0,y=0,z=1,size = 0.1;

  if (!PyArg_ParseTuple(args, "O!O(fff)f", &(Part::TopoShapePy::Type), &pcTopoObj,&pcListObj,&x,&y,&z,&size))     // convert args: Python->C 
//  if (!PyArg_ParseTuple(args, "O!O!", &(App::TopoShapePy::Type), &pcTopoObj,&PyList_Type,&pcListObj,x,y,z,size))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (Part::TopoShapePy*)pcTopoObj;
  MeshKernel *M = new MeshKernel();

  std::vector<Vector3D> poly;

  if (!PyList_Check(pcListObj))
    Py_Error(PyExc_Exception,"List of Tuble of three or two floats needed as second parameter!");
  
  int nSize = PyList_Size(pcListObj);
  for (int i=0; i<nSize;++i)
  {
    PyObject* item = PyList_GetItem(pcListObj, i);
    if (!PyTuple_Check(item))
      Py_Error(PyExc_Exception,"List of Tuble of three or two floats needed as second parameter!");
    int nTSize = PyTuple_Size(item);
    if(nTSize != 2 && nTSize != 3)
      Py_Error(PyExc_Exception,"List of Tuble of three or two floats needed as second parameter!");

    Vector3D vec(0,0,0);

    for(int l = 0; l < nTSize;l++)
    {
      PyObject* item2 = PyTuple_GetItem(item, l);
      if (!PyFloat_Check(item2))
        Py_Error(PyExc_Exception,"List of Tuble of three or two floats needed as second parameter!");
      vec[l] = PyFloat_AS_DOUBLE(item2);
    }
    poly.push_back(vec);
  }
    
  PY_TRY {
    TopoDS_Shape aShape = pcObject->getShape();
    // use the MeshAlgos 
    MeshAlgos::LoftOnCurve(*M,aShape,poly,Vector3D(x,y,z),size);

  } PY_CATCH;

  return new MeshPy(M);
*/
 	Py_Return;    

}

PyDoc_STRVAR(open_doc,
"open(string) -- Create a new document and a Mesh::Import feature to load the file into the document.");

PyDoc_STRVAR(inst_doc,
"insert(string|mesh,[string]) -- Load or insert a mesh into the given or active document.");

PyDoc_STRVAR(loft_doc,
"Loft on curve.");

/* List of functions defined in the module */

struct PyMethodDef Mesh_Import_methods[] = { 
    {"open"       ,open ,       METH_VARARGS, open_doc},
    {"insert"     ,insert,      METH_VARARGS, inst_doc},
    {"read"       ,read,        Py_NEWARGS,   "Read a mesh from a file and returns a Mesh object."},
 //   {"show"       ,show,        Py_NEWARGS,   "Put a mesh object in the active document or creates one if needed"},
    {"createBox"  ,createBox,   Py_NEWARGS,   "Create a solid mesh box"},
    {"createPlane",createPlane, Py_NEWARGS,   "Create a mesh XY plane normal +Z"},
    {"createSphere",createSphere, Py_NEWARGS,   "Create a tessellated sphere"},
    {"createEllipsoid",createEllipsoid, Py_NEWARGS,   "Create a tessellated ellipsoid"},
    {"createCylinder",createCylinder, Py_NEWARGS,   "Create a tessellated cylinder"},
    {"createCone",createCone, Py_NEWARGS,   "Create a tessellated cone"},
    {"createTorus",createTorus, Py_NEWARGS,   "Create a tessellated torus"},
    {"loftOnCurve",loftOnCurve, METH_VARARGS, loft_doc},
    {NULL, NULL}  /* sentinel */
};
