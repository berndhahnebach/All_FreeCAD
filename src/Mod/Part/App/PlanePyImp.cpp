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
# include <gp_Lin.hxx>
# include <Geom_Line.hxx>
# include <Geom_Plane.hxx>
# include <Geom_TrimmedCurve.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/VectorPy.h>

#include "Geometry.h"
#include "LinePy.h"
#include "PlanePy.h"
#include "PlanePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *PlanePy::representation(void) const
{
    return "<Plane object>";
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
    Handle_Geom_ElementarySurface s = Handle_Geom_ElementarySurface::DownCast
        (getGeometryPtr()->handle());
    gp_Dir dir = s->Axis().Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void PlanePy::setAxis(Py::Object arg)
{
    Standard_Real dir_x, dir_y, dir_z;
    PyObject *p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d v = static_cast<Base::VectorPy*>(p)->value();
        dir_x = v.x;
        dir_y = v.y;
        dir_z = v.z;
    }
    else if (PyTuple_Check(p)) {
        Py::Tuple tuple(arg);
        dir_x = (double)Py::Float(tuple.getItem(0));
        dir_y = (double)Py::Float(tuple.getItem(1));
        dir_z = (double)Py::Float(tuple.getItem(2));
    }
    else {
        std::string error = std::string("type must be 'Vector' or tuple, not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }

    try {
        Handle_Geom_ElementarySurface this_surf = Handle_Geom_ElementarySurface::DownCast
            (this->getGeometryPtr()->handle());
        gp_Ax1 axis;
        axis.SetLocation(this_surf->Location());
        axis.SetDirection(gp_Dir(dir_x, dir_y, dir_z));
        gp_Dir dir = axis.Direction();
        this_surf->SetAxis(axis);
    }
    catch (Standard_Failure) {
        throw Py::Exception("cannot set axis");
    }
}

PyObject* PlanePy::uIso(PyObject * args)
{
    double u;
    if (!PyArg_ParseTuple(args, "d", &u))
        return 0;

    try {
        Handle_Geom_Plane plane = Handle_Geom_Plane::DownCast
            (getGeomPlanePtr()->handle());
        Handle_Geom_Line c = Handle_Geom_Line::DownCast(plane->UIso(u));
        GeomLineSegment* line = new GeomLineSegment();
        Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast
            (line->handle());
        Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
            (this_curv->BasisCurve());
        this_line->SetLin(c->Lin());
        return new LinePy(line);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* PlanePy::vIso(PyObject * args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_Plane plane = Handle_Geom_Plane::DownCast
            (getGeomPlanePtr()->handle());
        Handle_Geom_Line c = Handle_Geom_Line::DownCast(plane->VIso(v));
        GeomLineSegment* line = new GeomLineSegment();
        Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast
            (line->handle());
        Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
            (this_curv->BasisCurve());
        this_line->SetLin(c->Lin());
        return new LinePy(line);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
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
