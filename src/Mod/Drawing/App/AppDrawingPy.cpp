/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <BRepPrimAPI_MakeBox.hxx>
# include <TopoDS_Face.hxx>
# include <Geom_Plane.hxx>
# include <Handle_Geom_Plane.hxx>
#endif

#include <stdio.h>

# if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
# endif // (re-)defined in pyconfig.h
#include <Python.h>

#include <Base/Console.h>
#include <Base/PyObjectBase.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <App/Application.h>
#include <App/Document.h>

// Things from the part module
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/TopoShapePy.h>

#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

using Base::Console;
using namespace Part;
using namespace std;


/* module functions */
static PyObject * open(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;

    PY_TRY {
    } PY_CATCH;

    Py_Return;
}

/* module functions */
static PyObject * insert(PyObject *self, PyObject *args)
{
    const char* Name;
    const char* DocName;
    if (!PyArg_ParseTuple(args, "ss",&Name,&DocName))
        return NULL;

    PY_TRY {
    } PY_CATCH;

    Py_Return;
}

/* module functions */
static PyObject * read(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;
    PY_TRY {
    } PY_CATCH;
  
    Py_Return;
}

static PyObject * createPlane(PyObject *self, PyObject *args)
{
    double z_level;

    //const char* Name;
    if (! PyArg_ParseTuple(args, "d", &z_level))
        return NULL;                         

    PY_TRY {
        gp_Pnt aPlanePnt(0,0,z_level);
        gp_Dir aPlaneDir(0,0,1);
        Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
        BRepBuilderAPI_MakeFace 	Face(aPlane);
        return new TopoShapePy(new TopoShape(Face.Face())); 
    } PY_CATCH;
}

static PyObject * createBox(PyObject *self, PyObject *args)
{
    double X, Y, Z , L, H, W ;

    //const char* Name;
    if (!PyArg_ParseTuple(args, "dddddd", &X, &Y, &Z , &L, &H, &W))
        return NULL;                         

    PY_TRY {
        // Build a box using the dimension and position attributes
        BRepPrimAPI_MakeBox mkBox( gp_Pnt( X, Y, Z ), L, H, W );
        TopoDS_Shape ResultShape = mkBox.Shape();
        return new TopoShapePy(new TopoShape(ResultShape)); 
    } PY_CATCH;
}

/* registration table  */
struct PyMethodDef Drawing_methods[] = {
    {"open"   , open,    1},
    {"insert" , insert,  1},
    {"read"   , read,  1},
    {"createPlane" , createPlane, 1},
    {"createBox" , createBox, 1},
    {NULL, NULL}        /* end of table marker */
};
