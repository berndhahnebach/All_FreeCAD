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
#include <App/Application.h>

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
  
    Py_Return;
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
    const char *FileName,*PartName;
    if (! PyArg_ParseTuple(args, "ssO!",&FileName,&PartName,&(App::TopoShapePy::Type), &ShapeObject)) 
        return NULL;                             

    TopoDS_Shape &aShape = ((App::TopoShapePy *)ShapeObject)->getShape();
    
    PovTools::writeShape(FileName,PartName,aShape,(float)0.1);

    Py_Return;       
}


/// write project file
static PyObject *                                
writeCameraFile(PyObject *self, PyObject *args)          
{      
    PyObject *Arg[4];
    const char *FileName;
    double vecs[4][3];
    if (! PyArg_ParseTuple(args, "sO!O!O!O!",&FileName,&PyTuple_Type, &Arg[0],&PyTuple_Type, &Arg[1],&PyTuple_Type, &Arg[2],&PyTuple_Type, &Arg[3])) 
        return NULL;                             

    // go throug the Tuple of Tupls
    for(int i=0;i<4;i++)
    {
      // check the right size of the Tuple of floats
      if(PyTuple_GET_SIZE(Arg[i]) != 3)
        Py_Error(PyExc_Exception,"Wrong parameter format, four Tuple of three floats needed!");

      // go through the Tuple of floats
      for(int l=0;l<3;l++)
      {
        PyObject* temp = PyTuple_GetItem(Arg[i],l);

        // check Type
        if( PyFloat_Check(temp))
          vecs[i][l] = PyFloat_AsDouble(temp);
        else
          if( PyLong_Check(temp))
            vecs[i][l] = (double) PyLong_AsLong(temp);
          else
            if( PyInt_Check(temp))
              vecs[i][l] = (double)  PyInt_AsLong(temp);
            else
              Py_Error(PyExc_Exception,"Wrong parameter format, four Tuple of three floats needed!");

        // build up the vector of vectors
      }
    }

    // call the write methode of PovTools....
    PovTools::writeCamera(FileName,CamDef(gp_Vec(vecs[0][0],vecs[0][1],vecs[0][2]),
                                          gp_Vec(vecs[1][0],vecs[1][1],vecs[1][2]),
                                          gp_Vec(vecs[2][0],vecs[2][1],vecs[2][2]),
                                          gp_Vec(vecs[3][0],vecs[3][1],vecs[3][2])));

    Py_Return;       
}

/// write project file
static PyObject *                                
copyResource(PyObject *self, PyObject *args)          
{      
    const char *FileName,*DestDir;
    if (! PyArg_ParseTuple(args, "ss",&FileName,&DestDir)) 
        return NULL;                             

    std::string resName = App::GetApplication().GetHomePath(); 
    resName += "Mod"; 
    resName += PATHSEP ;
    resName += "Rytracing"; 
    resName += PATHSEP ;
    resName += "resources"; 
    resName += PATHSEP;
    resName += FileName;

    Base::Console().Log("Using fileName = %s\n",resName.c_str());
    

    Py_Return;       
}



/* registration table  */
struct PyMethodDef Raytracing_methodes[] = {
    {"writeProjectFile", writeProjectFile, 1},       
    {"getProjectFile",   getProjectFile  , 1},       
    {"writePartFile",    writePartFile   , 1},       
    {"writeCameraFile",  writeCameraFile   , 1},       
    {"copyResource",     copyResource   , 1},       
    {NULL, NULL}                   
};


