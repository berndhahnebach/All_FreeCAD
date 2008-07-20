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
# include <Geom_ToroidalSurface.hxx>
# include <gp_Torus.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/VectorPy.h>

#include "Geometry.h"
#include "ToroidPy.h"
#include "ToroidPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *ToroidPy::representation(void) const
{
    return "<GeomToroid object>";
}

PyObject *ToroidPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of ToroidPy and the Twin object 
    return new ToroidPy(new GeomToroid);
}

// constructor method
int ToroidPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
            (getGeomToroidPtr()->handle());
        torus->SetMajorRadius(5.0);
        torus->SetMinorRadius(1.0);
        return 0;
    }

    return -1;
}

PyObject* ToroidPy::uIso(PyObject * args)
{
    double u;
    if (!PyArg_ParseTuple(args, "d", &u))
        return 0;

    try {
        Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
            (getGeomToroidPtr()->handle());
        Handle_Geom_Curve c = torus->UIso(u);

        PyErr_SetString(PyExc_NotImplementedError, "this type of conical curve is not implemented");
        return 0;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* ToroidPy::vIso(PyObject * args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
            (getGeomToroidPtr()->handle());
        Handle_Geom_Curve c = torus->VIso(v);

        PyErr_SetString(PyExc_NotImplementedError, "this type of conical curve is not implemented");
        return 0;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

Py::Float ToroidPy::getMajorRadius(void) const
{
    Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
        (getGeomToroidPtr()->handle());
    return Py::Float(torus->MajorRadius()); 
}

void ToroidPy::setMajorRadius(Py::Float arg)
{
    try {
        Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
            (getGeomToroidPtr()->handle());
        torus->SetMajorRadius((double)arg);
    }
    catch (Standard_Failure) {
        throw Py::Exception("Major radius must be positive and higher than minor radius");
    }
}

Py::Float ToroidPy::getMinorRadius(void) const
{
    Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
        (getGeomToroidPtr()->handle());
    return Py::Float(torus->MinorRadius()); 
}

void ToroidPy::setMinorRadius(Py::Float arg)
{
    try {
        Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
            (getGeomToroidPtr()->handle());
        torus->SetMinorRadius((double)arg);
    }
    catch (Standard_Failure) {
        throw Py::Exception("Minor radius must be positive and lower than major radius");
    }
}

Py::Object ToroidPy::getCenter(void) const
{
    Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
        (getGeomToroidPtr()->handle());
    gp_Pnt loc = torus->Location();
    Base::VectorPy* vec = new Base::VectorPy(
        Base::Vector3f((float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void ToroidPy::setCenter(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
            (getGeomToroidPtr()->handle());
        torus->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object ToroidPy::getAxis(void) const
{
    Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
        (getGeomToroidPtr()->handle());
    gp_Ax1 axis = torus->Axis();
    gp_Dir dir = axis.Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void ToroidPy::setAxis(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d val = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
            (getGeomToroidPtr()->handle());
        try {
            gp_Ax1 axis;
            axis.SetLocation(torus->Location());
            axis.SetDirection(gp_Dir(val.x, val.y, val.z));
            gp_Dir dir = axis.Direction();
            torus->SetAxis(axis);
        }
        catch (Standard_Failure) {
            throw Py::Exception("cannot set axis");
        }
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Float ToroidPy::getArea(void) const
{
    Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
        (getGeomToroidPtr()->handle());
    return Py::Float(torus->Area()); 
}

Py::Float ToroidPy::getVolume(void) const
{
    Handle_Geom_ToroidalSurface torus = Handle_Geom_ToroidalSurface::DownCast
        (getGeomToroidPtr()->handle());
    return Py::Float(torus->Volume()); 
}

PyObject *ToroidPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ToroidPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
