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

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

#include <Base/VectorPy.h>
#include <Base/GeometryPyCXX.h>

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeSolidPy.xml)
#include "TopoShapeSolidPy.h"
#include "TopoShapeSolidPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeSolidPy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Solid object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *TopoShapeSolidPy::PyMake(struct _typeobject *, PyObject *, PyObject *)
{
    // create a new instance of TopoShapeSolidPy and the Twin object 
    return new TopoShapeSolidPy(new TopoShape);
}

// constructor method
int TopoShapeSolidPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &obj))
        return -1;

    try {
        BRepBuilderAPI_MakeSolid mkSolid;
        const TopoDS_Shape& shape = static_cast<TopoShapePy*>(obj)
            ->getTopoShapePtr()->_Shape;
        TopExp_Explorer anExp (shape, TopAbs_SHELL);
        int count=0;
        for (; anExp.More(); anExp.Next()) {
            ++count;
            mkSolid.Add(TopoDS::Shell(anExp.Current()));
        }

        if (count == 0)
            Standard_Failure::Raise("No shells found in shape");

        const TopoDS_Solid& solid = mkSolid.Solid();
        getTopoShapePtr()->_Shape = solid;
    }
    catch (Standard_Failure) {
        PyErr_SetString(PyExc_Exception, "creation of solid failed");
        return -1;
    }

    return 0;
}

Py::Object TopoShapeSolidPy::getCenterOfMass(void) const
{
    GProp_GProps props;
    BRepGProp::VolumeProperties(getTopoShapePtr()->_Shape, props);
    gp_Pnt c = props.CentreOfMass();
    return Py::Vector(Base::Vector3d(c.X(),c.Y(),c.Z()));
}

PyObject *TopoShapeSolidPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeSolidPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
