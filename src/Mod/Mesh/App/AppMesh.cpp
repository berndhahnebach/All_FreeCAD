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
#endif

#include <App/Application.h>
#include <Base/Console.h>

#include <stdio.h>
#include <Python.h>

#include "FeatureMeshImportSTL.h"

/* module functions */
static PyObject *                                 /* returns object */
message(PyObject *self, PyObject *args)           /* self unused in modules */
{                                                 /* args from python call */
    char *fromPython, result[64];
    if (! PyArg_ParseTuple(args, "(s)", &fromPython))  /* convert Python -> C */
        return NULL;                              /* null=raise exception */
    else {
        strcpy(result, "Hello, ");                /* build up C string */
        strcat(result, fromPython);               /* add passed Python string */
        return Py_BuildValue("s", result);        /* convert C -> Python */
    }
}


/* module functions */
static PyObject *                        
open(PyObject *self, PyObject *args)     
{                                        
	std::string strResult;

  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  Base::Console().Log("Open in Mesh with %s",strResult.c_str());

	Py_Return;    
}


/* registration table  */
static struct PyMethodDef hello_methods[] = {
    {"message", message, 1},       
    {"open",    open,    1},       
    {NULL, NULL}                   /* end of table marker */
};



/* Python entry */
extern "C" {
void AppMeshExport initMesh() {

  (void) Py_InitModule("Mesh", hello_methods);   /* mod name, table ptr */

  Base::Console().Log("AppMesh loaded\n");
	App::FeatureFactory().AddProducer("MeshImportSTL",new App::FeatureProducer<Mesh::FeatureMeshImportSTL>);

  return;
}

} // extern "C" 
