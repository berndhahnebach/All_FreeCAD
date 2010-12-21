/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <BRepBuilderAPI_MakeFace.hxx>
# include <gp_Dir.hxx>
# include <gp_Vec.hxx>
# include <Geom_Geometry.hxx>
# include <Geom_Surface.hxx>
# include <GeomLProp_SLProps.hxx>
# include <Precision.hxx>
# include <Standard_Failure.hxx>
# include <ShapeAnalysis_Surface.hxx>
#endif

#include <Base/GeometryPyCXX.h>
#include <Base/VectorPy.h>

#include "Geometry.h"
#include "GeometrySurfacePy.h"
#include "GeometrySurfacePy.cpp"

#include "TopoShape.h"
#include "TopoShapePy.h"
#include "TopoShapeFacePy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string GeometrySurfacePy::representation(void) const
{
    return "<Surface object>";
}

PyObject *GeometrySurfacePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(PyExc_RuntimeError,
        "You cannot create an instance of the abstract class 'GeometrySurface'.");
    return 0;
}

// constructor method
int GeometrySurfacePy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* GeometrySurfacePy::toShape(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(g);
    try {
        if (!s.IsNull()) {
            double u1,u2,v1,v2;
            s->Bounds(u1,u2,v1,v2);
            if (!PyArg_ParseTuple(args, "|dddd", &u1,&u2,&v1,&v2))
                return 0;
            BRepBuilderAPI_MakeFace mkBuilder(s, u1, u2, v1, v2);
            TopoDS_Shape sh = mkBuilder.Shape();
            return new TopoShapeFacePy(new TopoShape(sh));
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is not a surface");
    return 0;
}

PyObject* GeometrySurfacePy::value(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(g);
    try {
        if (!s.IsNull()) {
            double u,v;
            if (!PyArg_ParseTuple(args, "dd", &u,&v))
                return 0;
            gp_Pnt p = s->Value(u,v);
            return new Base::VectorPy(Base::Vector3d(p.X(),p.Y(),p.Z()));
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is not a surface");
    return 0;
}

PyObject* GeometrySurfacePy::tangent(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(g);
    try {
        if (!s.IsNull()) {
            double u,v;
            if (!PyArg_ParseTuple(args, "dd", &u,&v))
                return 0;
            gp_Dir dir;
            Py::Tuple tuple(2);
            GeomLProp_SLProps prop(s,u,v,1,Precision::Confusion());
            if (prop.IsTangentUDefined()) {
                prop.TangentU(dir);
                tuple.setItem(0, Py::Vector(Base::Vector3d(dir.X(),dir.Y(),dir.Z())));
            }
            if (prop.IsTangentVDefined()) {
                prop.TangentV(dir);
                tuple.setItem(1, Py::Vector(Base::Vector3d(dir.X(),dir.Y(),dir.Z())));
            }

            return Py::new_reference_to(tuple);
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is not a surface");
    return 0;
}

PyObject* GeometrySurfacePy::parameter(PyObject *args)
{
    Handle_Geom_Surface surf = Handle_Geom_Surface
        ::DownCast(getGeometryPtr()->handle());
    try {
        if (!surf.IsNull()) {
            PyObject *p;
            double prec = Precision::Confusion();
            if (!PyArg_ParseTuple(args, "O!|d", &(Base::VectorPy::Type), &p, &prec))
                return 0;
            Base::Vector3d v = Py::Vector(p, false).toVector();
            gp_Pnt pnt(v.x,v.y,v.z);
            ShapeAnalysis_Surface as(surf);
            gp_Pnt2d uv = as.ValueOfUV(pnt, prec);
            Py::Tuple tuple(2);
            tuple.setItem(0, Py::Float(uv.X()));
            tuple.setItem(1, Py::Float(uv.Y()));
            return Py::new_reference_to(tuple);
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is not a surface");
    return 0;
}

PyObject* GeometrySurfacePy::bounds(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_Surface surf = Handle_Geom_Surface
        ::DownCast(getGeometryPtr()->handle());
    Py::Tuple bound(4);
    Standard_Real u1,u2,v1,v2;
    surf->Bounds(u1,u2,v1,v2);
    bound.setItem(0,Py::Float(u1));
    bound.setItem(1,Py::Float(u2));
    bound.setItem(2,Py::Float(v1));
    bound.setItem(3,Py::Float(v2));
    return Py::new_reference_to(bound);
}

PyObject* GeometrySurfacePy::isUPeriodic(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_Surface surf = Handle_Geom_Surface::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = surf->IsUPeriodic();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* GeometrySurfacePy::isVPeriodic(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_Surface surf = Handle_Geom_Surface::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = surf->IsVPeriodic();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* GeometrySurfacePy::isUClosed(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_Surface surf = Handle_Geom_Surface::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = surf->IsUClosed();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* GeometrySurfacePy::isVClosed(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_Surface surf = Handle_Geom_Surface::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = surf->IsVClosed();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* GeometrySurfacePy::UPeriod(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    try {
        Handle_Geom_Surface surf = Handle_Geom_Surface::DownCast
            (getGeometryPtr()->handle());
        Standard_Real val = surf->UPeriod();
        return PyFloat_FromDouble(val);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* GeometrySurfacePy::VPeriod(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    try {
        Handle_Geom_Surface surf = Handle_Geom_Surface::DownCast
            (getGeometryPtr()->handle());
        Standard_Real val = surf->VPeriod();
        return PyFloat_FromDouble(val);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject *GeometrySurfacePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int GeometrySurfacePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
