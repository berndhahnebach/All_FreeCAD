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
# include <BRepBuilderAPI_MakeWire.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Wire.hxx>
# include <gp_Ax1.hxx>
#endif

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include <Base/VectorPy.h>

#include "TopoShape.h"
#include "TopoShapeShellPy.h"
#include "TopoShapeWirePy.h"
#include "TopoShapeWirePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeWirePy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Wire object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *TopoShapeWirePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TopoShapeWirePy and the Twin object 
    return new TopoShapeWirePy(new TopoShape);
}

// constructor method
int TopoShapeWirePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pcObj;
    if (PyArg_ParseTuple(args, "O!", &(Part::TopoShapePy::Type), &pcObj)) {
        BRepBuilderAPI_MakeWire mkWire;
        TopoDS_Shape sh = static_cast<Part::TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
        if (sh.ShapeType() == TopAbs_EDGE)
            mkWire.Add(TopoDS::Edge(sh));
        else if (sh.ShapeType() == TopAbs_WIRE)
            mkWire.Add(TopoDS::Wire(sh));
        else {
            PyErr_SetString(PyExc_TypeError, "shape is neither edge nor wire");
            return -1;
        }

        try {
            getTopoShapePtr()->_Shape = mkWire.Wire();
            return 0;
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return -1;
        }
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &(PyList_Type), &pcObj)) {
        BRepBuilderAPI_MakeWire mkWire;
        Py::List list(pcObj);
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            PyObject* item = (*it).ptr();
            if (PyObject_TypeCheck(item, &(Part::TopoShapePy::Type))) {
                TopoDS_Shape sh = static_cast<Part::TopoShapePy*>(item)->getTopoShapePtr()->_Shape;
                if (sh.ShapeType() == TopAbs_EDGE)
                    mkWire.Add(TopoDS::Edge(sh));
                else if (sh.ShapeType() == TopAbs_WIRE)
                    mkWire.Add(TopoDS::Wire(sh));
                else {
                    PyErr_SetString(PyExc_TypeError, "shape is neither edge nor wire");
                    return -1;
                }
            }
            else {
                PyErr_SetString(PyExc_TypeError, "item is not a shape");
                return -1;
            }
        }

        try {
            getTopoShapePtr()->_Shape = mkWire.Wire();
            return 0;
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return -1;
        }
    }

    PyErr_SetString(PyExc_Exception, "edge or wire or list of edges and wires expected");
    return -1;
}

PyObject* TopoShapeWirePy::extrude(PyObject *args)
{
    PyObject *pVec;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type), &pVec)) {
        try {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pVec)->value();
            TopoDS_Shape shape = this->getTopoShapePtr()->makePrism(gp_Vec(vec.x,vec.y,vec.z));
            return new TopoShapeShellPy(new TopoShape(shape));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    }

    return 0;
}

PyObject* TopoShapeWirePy::revolve(PyObject *args)
{
    PyObject *pPos,*pDir;
    double d=2.0*Standard_PI;
    if (PyArg_ParseTuple(args, "O!O!|d", &(Base::VectorPy::Type), &pPos, &(Base::VectorPy::Type), &pDir,&d)) {
        try {
            Base::Vector3d pos = static_cast<Base::VectorPy*>(pPos)->value();
            Base::Vector3d dir = static_cast<Base::VectorPy*>(pDir)->value();
            TopoDS_Shape shape = this->getTopoShapePtr()->revolve(
                gp_Ax1(gp_Pnt(pos.x,pos.y,pos.z), gp_Dir(dir.x,dir.y,dir.z)),d);
            return new TopoShapeShellPy(new TopoShape(shape));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    }

    return 0;
}

Py::Object TopoShapeWirePy::getCenterOfMass(void) const
{
    GProp_GProps props;
    BRepGProp::LinearProperties(getTopoShapePtr()->_Shape, props);
    gp_Pnt c = props.CentreOfMass();
    return Py::Object(new Base::VectorPy(Base::Vector3d(c.X(),c.Y(),c.Z())));
}

PyObject *TopoShapeWirePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeWirePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


