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

#include "Points.h"
#include "PointsPy.h"
#include "PointsAlgos.h"

using namespace Points;

/* module functions */
static PyObject *                        
open(PyObject *self, PyObject *args)     
{                                        
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  PY_TRY {
    
    Base::Console().Log("Open in Points with %s",Name);
    Base::FileInfo file(Name);

    // extract ending
    if(file.extension() == "")
      Py_Error(PyExc_Exception,"no file ending");

    if(file.hasExtension("asc"))
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().newDocument(file.fileNamePure().c_str());
      App::Feature *pcFeature = pcDoc->addFeature("PointsImport", "PointsOpen");
      pcFeature->setPropertyString(Name, "FileName");
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

static PyObject *                        
insert(PyObject *self, PyObject *args)     
{                                        
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  PY_TRY {
    
    Base::Console().Log("Import in Points with %s",Name);
    Base::FileInfo file(Name);

    // extract ending
    if(file.extension() == "")
      Py_Error(PyExc_Exception,"no file ending");

    if(file.hasExtension("asc"))
    {
      App::Document *pcDoc = App::GetApplication().getActiveDocument();
      if (!pcDoc)
        throw "Import called without a active document??";
      App::Feature *pcFeature = pcDoc->addFeature("PointsImport", "PointsOpen");
      pcFeature->setPropertyString(Name, "FileName");
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
    PointsWithProperty* points = new PointsWithProperty;
    // load the mesh and create a mesh python object with it
    PointsAlgos::Load(*points, File.filePath().c_str());
    return new PointsPy( points );
  } PY_CATCH;
  Py_Return;
}

static PyObject *                        
create(PyObject *self, PyObject *args)

{
  if (! PyArg_ParseTuple(args, "") )			 
    return NULL;                         

  PY_TRY {
    // load the mesh and create a mesh python object with it
    return new PointsPy(new PointsWithProperty());    
  } PY_CATCH;
}

// registration table  
struct PyMethodDef Points_Import_methods[] = {
    {"open",  open,   1},				/* method name, C func ptr, always-tuple */
    {"insert",insert, 1},
    {"save",  save,   1},
    {"read",  read,   1},
    {"create",create, 1},

    {NULL, NULL}                /* end of table marker */
};
