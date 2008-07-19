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
# include <Geom_CylindricalSurface.hxx>
#endif

#include <Base/VectorPy.h>

#include "Geometry.h"
#include "CirclePy.h"
#include "EllipsePy.h"
#include "LinePy.h"
#include "CylinderPy.h"
#include "CylinderPy.cpp"
#include "TopoShapeFacePy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *CylinderPy::representation(void) const
{
    return "<GeomCylinder object>";
}

PyObject *CylinderPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of CylinderPy and the Twin object 
    return new CylinderPy(new GeomCylinder);
}

// constructor method
int CylinderPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
            (getGeomCylinderPtr()->handle());
        cyl->SetRadius(1.0);
        return 0;
    }

    return -1;
}

PyObject* CylinderPy::uIso(PyObject * args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
            (getGeomCylinderPtr()->handle());
        Handle_Geom_Curve c = cyl->UIso(v);
        if (!Handle_Geom_Line::DownCast(c).IsNull()) {
            GeomLineSegment* line = new GeomLineSegment();
            Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast
                (line->handle());
            Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
                (this_curv->BasisCurve());
            this_line->SetLin(Handle_Geom_Line::DownCast(c)->Lin());
            return new LinePy(line);
        }

        PyErr_SetString(PyExc_NotImplementedError, "this type of conical curve is not implemented");
        return 0;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* CylinderPy::vIso(PyObject * args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
            (getGeomCylinderPtr()->handle());
        Handle_Geom_Curve c = cyl->VIso(v);
        if (!Handle_Geom_Circle::DownCast(c).IsNull()) {
            return new CirclePy(new GeomCircle(Handle_Geom_Circle::DownCast(c)));
        }
        if (!Handle_Geom_Ellipse::DownCast(c).IsNull()) {
            return new EllipsePy(new GeomEllipse(Handle_Geom_Ellipse::DownCast(c)));
        }

        PyErr_SetString(PyExc_NotImplementedError, "this type of conical curve is not implemented");
        return 0;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

Py::Object CylinderPy::getFace(void) const
{
    TopoDS_Shape sh = getGeometryPtr()->toShape();
    TopoShapeFacePy* face = new TopoShapeFacePy(new TopoShape(sh));
    return Py::Object(face);
}

Py::Float CylinderPy::getRadius(void) const
{
    Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
        (getGeomCylinderPtr()->handle());
    return Py::Float(cyl->Radius()); 
}

void CylinderPy::setRadius(Py::Float arg)
{
    Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
        (getGeomCylinderPtr()->handle());
    cyl->SetRadius((double)arg);
}

Py::Object CylinderPy::getCenter(void) const
{
    Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
        (getGeomCylinderPtr()->handle());
    gp_Pnt loc = cyl->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void CylinderPy::setCenter(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
            (getGeomCylinderPtr()->handle());
        cyl->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object CylinderPy::getAxis(void) const
{
    Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
        (getGeomCylinderPtr()->handle());
    gp_Ax1 axis = cyl->Axis();
    gp_Dir dir = axis.Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void CylinderPy::setAxis(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d val = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_CylindricalSurface cyl = Handle_Geom_CylindricalSurface::DownCast
            (getGeomCylinderPtr()->handle());
        try {
            gp_Ax1 axis;
            axis.SetLocation(cyl->Location());
            axis.SetDirection(gp_Dir(val.x, val.y, val.z));
            gp_Dir dir = axis.Direction();
            cyl->SetAxis(axis);
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

PyObject *CylinderPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int CylinderPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


