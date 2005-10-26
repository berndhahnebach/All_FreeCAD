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
#	include <stdio.h>
# if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
# endif // (re-)defined in pyconfig.h
#	include <Python.h>
# include <BRep_Builder.hxx>
# include <BRepTools.hxx>
# include <gts.h>
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/FileInfo.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <App/Property.h>
#include <Mod/Part/App/TopologyPy.h>

#include "MeshPy.h"
#include "Mesh.h"
#include "MeshAlgos.h"

using namespace Mesh;


/* module functions */
static PyObject *                        
open(PyObject *self, PyObject *args)     
{                                        
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
    
  PY_TRY {

    Base::Console().Log("Open in Mesh with %s",Name);
    Base::FileInfo file(Name);

    // extract ending
    if(file.extension() == "")
      Py_Error(PyExc_Exception,"no file ending");

    if(file.hasExtension("stl") || file.hasExtension("ast"))
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().newDocument(file.fileNamePure().c_str());
      App::Feature *pcFeature = pcDoc->addFeature("MeshImport","MeshOpen");
      pcFeature->setPropertyString(Name,"FileName");
      pcFeature->TouchProperty("FileName");
      pcDoc->Recompute();

    }

    else
    {
      Py_Error(PyExc_Exception,"unknown file ending");
    }


  } PY_CATCH;

	Py_Return;    
}


/* module functions */
static PyObject *                        
insert(PyObject *self, PyObject *args)     
{                                        
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))	 		 
    return NULL;                         
    
  PY_TRY {

    Base::Console().Log("Insert in Mesh with %s",Name);
    Base::FileInfo file(Name);

    // extract ending
    if(file.extension() == "")
      Py_Error(PyExc_Exception,"no file ending");

    if(file.hasExtension("stl") || file.hasExtension("ast"))
    {
      // add Import feature
      App::Document *pcDoc = App::GetApplication().getActiveDocument();
      if (!pcDoc)
        throw "Import called without a active document??";
      App::Feature *pcFeature = pcDoc->addFeature("MeshImport", "MeshImport");
      pcFeature->setPropertyString(Name,"FileName");
      pcFeature->TouchProperty("FileName");
      pcDoc->Recompute();

    }

    else
    {
      Py_Error(PyExc_Exception,"unknown file ending");
    }


  } PY_CATCH;

	Py_Return;    
}

/* module functions */
static PyObject *                        
save(PyObject *self, PyObject *args)

{
	Py_Return;    

}

/* module functions */
static PyObject *                        
read(PyObject *self, PyObject *args)

{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  Base::FileInfo File(Name);
  
  // checking on the file
  if(!File.isReadable())
    Py_Error(PyExc_Exception,"File to load not existing or not readable");

  PY_TRY {
    // load the mesh and create a mesh python object with it
    return new MeshPy(MeshAlgos::Load(File.filePath().c_str()));    
  } PY_CATCH;
}

static PyObject *                        
newMesh(PyObject *self, PyObject *args)

{
  if (! PyArg_ParseTuple(args, "") )			 
    return NULL;                         

  PY_TRY {
    // load the mesh and create a mesh python object with it
    return new MeshPy(new MeshWithProperty());    
  } PY_CATCH;
}

static PyObject *                        
loftOnCurve(PyObject *self, PyObject *args)

{
#if 0
  Part::TopoShapePy   *pcObject;
  PyObject *pcTopoObj,*pcListObj;
  float x=0,y=0,z=1,size = 0.1;

  if (!PyArg_ParseTuple(args, "O!O(fff)f", &(Part::TopoShapePy::Type), &pcTopoObj,&pcListObj,&x,&y,&z,&size))     // convert args: Python->C 
//  if (!PyArg_ParseTuple(args, "O!O!", &(App::TopoShapePy::Type), &pcTopoObj,&PyList_Type,&pcListObj,x,y,z,size))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (Part::TopoShapePy*)pcTopoObj;
  MeshWithProperty *M = new MeshWithProperty();

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
#else
  Base::Console().Error("Linker error: Part::TopoShapePy\n");
  Py_Return;
#endif
}

/* registration table  */
struct PyMethodDef Mesh_Import_methods[] = {
    {"open"       ,open ,       1},				/* method name, C func ptr, always-tuple */
    {"save"       ,save ,       1},
    {"insert"     ,insert,      1},
    {"read"       ,read ,       1},
    {"newMesh"    ,newMesh,     1},
    {"loftOnCurve",loftOnCurve, 1},

    {NULL, NULL}                   /* end of table marker */
};
