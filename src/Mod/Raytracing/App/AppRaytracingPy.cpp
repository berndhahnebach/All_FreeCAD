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
#endif


#include <Python.h>

#include <Base/Console.h>
#include "PovTools.h"
// automatecly generated.....
#include "FreeCADpov.h"

#include <App/Topology.h>

using namespace Raytracing;


/// write empty project file
static PyObject *                                
writeProjectFile(PyObject *self, PyObject *args)          
{                                                
    char *fromPython;
    if (! PyArg_ParseTuple(args, "(s)", &fromPython)) 
        return NULL;                             
    
    std::ofstream fout;
    if(fromPython)
      fout.open(fromPython);
    else
      fout.open("FreeCAD.pov");

    fout << FreeCAD ;
    fout.close();
  
    Py_INCREF(Py_None); return Py_None;
}

/// write project file
static PyObject *                                
getProjectFile(PyObject *self, PyObject *args)          
{                                                
    return Py_BuildValue("s", FreeCAD);       
}

/// write project file
static PyObject *                                
writePartFile(PyObject *self, PyObject *args)          
{      
    PyObject *ShapeObject;
    if (! PyArg_ParseTuple(args, "O!",&(App::TopoShapePy::Type), &ShapeObject)) 
//    if (! PyArg_ParseTuple(args, "O", &ShapeObject)) 
        return NULL;                             

    TopoDS_Shape &aShape = ((App::TopoShapePy *)ShapeObject)->getShape();
    
    PovTools::writeShape("c:\\temp\\TestPart.inc","Part",aShape,(float)0.1);

    Py_INCREF(Py_None); return Py_None;       
}



/* registration table  */
struct PyMethodDef Raytracing_methodes[] = {
    {"writeProjectFile", writeProjectFile, 1},       
    {"getProjectFile",   getProjectFile  , 1},       
    {"writePartFile",    writePartFile   , 1},       
    {NULL, NULL}                   
};


