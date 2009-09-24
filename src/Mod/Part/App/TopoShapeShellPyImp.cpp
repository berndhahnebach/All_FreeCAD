/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
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
# include <gp_Ax1.hxx>
# include <BRep_Builder.hxx>
# include <BRepCheck_Analyzer.hxx>
# include <TopoDS_Shell.hxx>
# include <ShapeUpgrade_ShellSewing.hxx>
#endif

#include <Base/VectorPy.h>

#include "TopoShape.h"
#include "TopoShapeCompSolidPy.h"
#include "TopoShapeFacePy.h"
#include "TopoShapeShellPy.h"
#include "TopoShapeShellPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string TopoShapeShellPy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Shell object at " << getTopoShapePtr() << ">";

    return str.str();
}

PyObject *TopoShapeShellPy::PyMake(struct _typeobject *, PyObject *, PyObject *)
{
    // create a new instance of TopoShapeSolidPy and the Twin object 
    return new TopoShapeShellPy(new TopoShape);
}

// constructor method
int TopoShapeShellPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(PyList_Type), &obj))
        return -1;

    BRep_Builder builder;
    TopoDS_Shape shape;
    TopoDS_Shell shell;
    //BRepOffsetAPI_Sewing mkShell;
    builder.MakeShell(shell);
    
    try {
        Py::List list(obj);
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            if (PyObject_TypeCheck((*it).ptr(), &(Part::TopoShapeFacePy::Type))) {
                const TopoDS_Shape& sh = static_cast<TopoShapeFacePy*>((*it).ptr())->
                    getTopoShapePtr()->_Shape;
                builder.Add(shell, sh);
            }
        }

        shape = shell;
        BRepCheck_Analyzer check(shell);
        if (!check.IsValid()) {
            ShapeUpgrade_ShellSewing sewShell;
            shape = sewShell.ApplySewing(shell);
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return -1;
    }

    getTopoShapePtr()->_Shape = shape;
    return 0;
}

PyObject*  TopoShapeShellPy::add(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(Part::TopoShapeFacePy::Type), &obj))
        return NULL;

    BRep_Builder builder;
    TopoDS_Shape& shell = getTopoShapePtr()->_Shape;
    
    try {
        const TopoDS_Shape& sh = static_cast<TopoShapeFacePy*>(obj)->
            getTopoShapePtr()->_Shape;
        builder.Add(shell, sh);
        BRepCheck_Analyzer check(shell);
        if (!check.IsValid()) {
            ShapeUpgrade_ShellSewing sewShell;
            getTopoShapePtr()->_Shape = sewShell.ApplySewing(shell);
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    Py_Return;
}

PyObject *TopoShapeShellPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeShellPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
