/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <gp_Dir.hxx>
# include <gp_Pnt.hxx>
# include <Geom_Plane.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/VectorPy.h>

#include "Geometry.h"
#include "TopoShapeFacePy.h"
#include "PlanePy.h"
#include "PlanePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *PlanePy::representation(void) const
{
    return "<GeomPlane object>";
}

PyObject *PlanePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of PlanePy and the Twin object 
    return new PlanePy(new GeomPlane);
}

// constructor method
int PlanePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        // do nothing
        return 0;
    }

    return -1;
}


PyObject* PlanePy::setParameterRange(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

Py::Object PlanePy::getFace(void) const
{
    TopoDS_Shape sh = getGeometryPtr()->toShape();
    TopoShapeFacePy* face = new TopoShapeFacePy(new TopoShape(sh));
    return Py::Object(face);
}

Py::Object PlanePy::getPosition(void) const
{
    Handle_Geom_Plane this_surf = Handle_Geom_Plane::DownCast
        (this->getGeomPlanePtr()->handle());
    gp_Pnt pnt = this_surf->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)pnt.X(), (float)pnt.Y(), (float)pnt.Z()));
    return Py::Object(vec);
}

void PlanePy::setPosition(Py::Object arg)
{
    gp_Pnt loc;
    PyObject *p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d v = static_cast<Base::VectorPy*>(p)->value();
        loc.SetX(v.x);
        loc.SetY(v.y);
        loc.SetZ(v.z);
    }
    else if (PyTuple_Check(p)) {
        Py::Tuple tuple(arg);
        loc.SetX((double)Py::Float(tuple.getItem(0)));
        loc.SetY((double)Py::Float(tuple.getItem(1)));
        loc.SetZ((double)Py::Float(tuple.getItem(2)));
    }
    else {
        std::string error = std::string("type must be 'Vector' or tuple, not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }

    try {
        Handle_Geom_Plane this_surf = Handle_Geom_Plane::DownCast
            (this->getGeomPlanePtr()->handle());
        this_surf->SetLocation(loc);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        throw Py::Exception(e->GetMessageString());
    }
}

Py::Object PlanePy::getAxis(void) const
{
    Handle_Geom_Plane this_surf = Handle_Geom_Plane::DownCast
        (this->getGeomPlanePtr()->handle());
    gp_Dir dir = this_surf->Axis().Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void PlanePy::setAxis(Py::Object arg)
{
    gp_Dir dir;
    PyObject *p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d v = static_cast<Base::VectorPy*>(p)->value();
        dir.SetX(v.x);
        dir.SetY(v.y);
        dir.SetZ(v.z);
    }
    else if (PyTuple_Check(p)) {
        Py::Tuple tuple(arg);
        dir.SetX((double)Py::Float(tuple.getItem(0)));
        dir.SetY((double)Py::Float(tuple.getItem(1)));
        dir.SetZ((double)Py::Float(tuple.getItem(2)));
    }
    else {
        std::string error = std::string("type must be 'Vector' or tuple, not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }

    try {
        Handle_Geom_Plane this_surf = Handle_Geom_Plane::DownCast
            (this->getGeomPlanePtr()->handle());
        gp_Ax1 ax1 = this_surf->Axis();
        ax1.SetDirection(dir);
        this_surf->SetAxis(ax1);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        throw Py::Exception(e->GetMessageString());
    }
}

PyObject *PlanePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int PlanePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
