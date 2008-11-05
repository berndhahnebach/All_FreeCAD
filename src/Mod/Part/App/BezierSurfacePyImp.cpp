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
# include <Geom_BezierCurve.hxx>
# include <Geom_BezierSurface.hxx>
# include <Handle_Geom_BezierCurve.hxx>
#endif

#include "Geometry.h"
#include "BezierCurvePy.h"
#include "BezierSurfacePy.h"
#include "BezierSurfacePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *BezierSurfacePy::representation(void) const
{
    return "<BezierSurface object>";
}

PyObject *BezierSurfacePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of BezierSurfacePy and the Twin object 
    return new BezierSurfacePy(new GeomBezierSurface);
}

// constructor method
int BezierSurfacePy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* BezierSurfacePy::bounds(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::isURational(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::isVRational(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::isUPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::isVPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::isUClosed(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::isVClosed(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::increase(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::insertPoleColAfter(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::insertPoleRowAfter(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::insertPoleColBefore(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::insertPoleRowBefore(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::removePoleCol(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::removePoleRow(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::segment(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::setPole(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::setPoleCol(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::setPoleRow(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::getPole(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::getPoles(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::setWeight(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::setWeightCol(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::setWeightRow(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::getWeight(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::getWeights(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::getResolution(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::exchangeUV(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BezierSurfacePy::uIso(PyObject * args)
{
    double u;
    if (!PyArg_ParseTuple(args, "d", &u))
        return 0;

    try {
        Handle_Geom_BezierSurface surf = Handle_Geom_BezierSurface::DownCast
            (getGeometryPtr()->handle());
        Handle_Geom_Curve c = surf->UIso(u);
        return new BezierCurvePy(new GeomBezierCurve(Handle_Geom_BezierCurve::DownCast(c)));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BezierSurfacePy::vIso(PyObject * args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_BezierSurface surf = Handle_Geom_BezierSurface::DownCast
            (getGeometryPtr()->handle());
        Handle_Geom_Curve c = surf->VIso(v);
        return new BezierCurvePy(new GeomBezierCurve(Handle_Geom_BezierCurve::DownCast(c)));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

Py::Int BezierSurfacePy::getUDegree(void) const
{
    Handle_Geom_BezierSurface surf = Handle_Geom_BezierSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->UDegree()); 
}

Py::Int BezierSurfacePy::getVDegree(void) const
{
    Handle_Geom_BezierSurface surf = Handle_Geom_BezierSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->UDegree()); 
}

Py::Int BezierSurfacePy::getMaxDegree(void) const
{
    Handle_Geom_BezierSurface surf = Handle_Geom_BezierSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->MaxDegree()); 
}

Py::Int BezierSurfacePy::getNbUPoles(void) const
{
    Handle_Geom_BezierSurface surf = Handle_Geom_BezierSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->NbUPoles()); 
}

Py::Int BezierSurfacePy::getNbVPoles(void) const
{
    Handle_Geom_BezierSurface surf = Handle_Geom_BezierSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->NbVPoles()); 
}

PyObject *BezierSurfacePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int BezierSurfacePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
