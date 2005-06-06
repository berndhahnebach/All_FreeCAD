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
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/FileInfo.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <App/Property.h>
#include <App/Topology.h>

#include "MeshPy.h"
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

    // extract ending
    std::string cEnding(Name);
    unsigned int pos = cEnding.find_last_of('.');
    if(pos == cEnding.size())
      Py_Error(PyExc_Exception,"no file ending");
    cEnding.erase(0,pos+1);

    if(cEnding == "stl" || cEnding == "ast")
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().New();
      App::Feature *pcFeature = pcDoc->AddFeature("MeshImportSTL");
      pcFeature->GetProperty("FileName").Set(Name);
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
/* module functions */
static PyObject *                        
write(PyObject *self, PyObject *args)

{
	Py_Return;    

}

/* registration table  */
struct PyMethodDef Mesh_Import_methods[] = {
    {"open" ,open , 1},				/* method name, C func ptr, always-tuple */
    {"save" ,save , 1},
    {"read" ,read , 1},
    {"write",write, 1},

    {NULL, NULL}                   /* end of table marker */
};
