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
# include <gp_Circ.hxx>
# include <gp_Sphere.hxx>
# include <Geom_Circle.hxx>
# include <Geom_SphericalSurface.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/VectorPy.h>

#include "Geometry.h"
#include "CirclePy.h"
#include "SpherePy.h"
#include "SpherePy.cpp"
#include "TopoShapeFacePy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *SpherePy::representation(void) const
{
    Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
        (getGeomSpherePtr()->handle());
    gp_Ax1 axis = sphere->Axis();
    gp_Dir dir = axis.Direction();
    gp_Pnt loc = axis.Location();
    Standard_Real fRad = sphere->Radius();

    std::stringstream str;
    str << "Sphere (";
    str << "Radius : " << fRad << ", "; 
    str << "Center : (" << loc.X() << ", "<< loc.Y() << ", "<< loc.Z() << "), "; 
    str << "Direction : (" << dir.X() << ", "<< dir.Y() << ", "<< dir.Z() << ")"; 
    str << ")";

    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *SpherePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of SpherePy and the Twin object 
    return new SpherePy(new GeomSphere);
}

// constructor method
int SpherePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
            (getGeomSpherePtr()->handle());
        sphere->SetRadius(1.0);
        return 0;
    }

    return -1;
}

Py::Object SpherePy::getFace(void) const
{
    TopoDS_Shape sh = getGeometryPtr()->toShape();
    TopoShapeFacePy* face = new TopoShapeFacePy(new TopoShape(sh));
    return Py::Object(face);
}

Py::Float SpherePy::getRadius(void) const
{
    Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
        (getGeomSpherePtr()->handle());
    return Py::Float(sphere->Radius()); 
}

void  SpherePy::setRadius(Py::Float arg)
{
    Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
        (getGeomSpherePtr()->handle());
    sphere->SetRadius((double)arg);
}

Py::Float SpherePy::getArea(void) const
{
    Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
        (getGeomSpherePtr()->handle());
    return Py::Float(sphere->Area()); 
}

Py::Float SpherePy::getVolume(void) const
{
    Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
        (getGeomSpherePtr()->handle());
    return Py::Float(sphere->Volume()); 
}

Py::Object SpherePy::getCenter(void) const
{
    Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
        (getGeomSpherePtr()->handle());
    gp_Pnt loc = sphere->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f((float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void SpherePy::setCenter(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
            (getGeomSpherePtr()->handle());
        sphere->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object SpherePy::getAxis(void) const
{
    Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
        (getGeomSpherePtr()->handle());
    gp_Ax1 axis = sphere->Axis();
    gp_Dir dir = axis.Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void SpherePy::setAxis(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d val = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast(getGeomSpherePtr()->handle());
        try {
            gp_Ax1 axis;
            axis.SetLocation(sphere->Location());
            axis.SetDirection(gp_Dir(val.x, val.y, val.z));
            gp_Dir dir = axis.Direction();
            sphere->SetAxis(axis);
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

PyObject *SpherePy::uIso(PyObject *args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
            (getGeomSpherePtr()->handle());
        Handle_Geom_Curve c = sphere->UIso(v);
        return new CirclePy(new GeomCircle(Handle_Geom_Circle::DownCast(c)));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject *SpherePy::vIso(PyObject *args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_SphericalSurface sphere = Handle_Geom_SphericalSurface::DownCast
            (getGeomSpherePtr()->handle());
        Handle_Geom_Curve c = sphere->VIso(v);
        return new CirclePy(new GeomCircle(Handle_Geom_Circle::DownCast(c)));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject *SpherePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SpherePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
