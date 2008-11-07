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
# include <Geom_BSplineCurve.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Handle_Geom_BSplineCurve.hxx>
# include <TColStd_Array2OfReal.hxx>
# include <TColgp_Array1OfPnt.hxx>
# include <TColgp_Array2OfPnt.hxx>
#endif

#include "Geometry.h"
#include "BSplineCurvePy.h"
#include "BSplineSurfacePy.h"
#include "BSplineSurfacePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *BSplineSurfacePy::representation(void) const
{
    return "<BSplineSurface object>";
}

PyObject *BSplineSurfacePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of BSplineSurfacePy and the Twin object 
    return new BSplineSurfacePy(new GeomBSplineSurface);
}

// constructor method
int BSplineSurfacePy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* BSplineSurfacePy::bounds(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    Py::Tuple bound(4);
    Standard_Real u1,u2,v1,v2;
    surf->Bounds(u1,u2,v1,v2);
    bound.setItem(0,Py::Float(u1));
    bound.setItem(1,Py::Float(u2));
    bound.setItem(2,Py::Float(v1));
    bound.setItem(3,Py::Float(v2));
    return Py::new_reference_to(bound);
}

PyObject* BSplineSurfacePy::isURational(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = surf->IsURational();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* BSplineSurfacePy::isVRational(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = surf->IsVRational();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* BSplineSurfacePy::isUPeriodic(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
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

PyObject* BSplineSurfacePy::isVPeriodic(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
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

PyObject* BSplineSurfacePy::isUClosed(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
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

PyObject* BSplineSurfacePy::isVClosed(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
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

PyObject* BSplineSurfacePy::increaseDegree(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::increaseUMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::increaseVMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::incrementUMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::incrementVMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::insertUKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::insertUKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::insertVKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::insertVKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::removeUKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::removeVKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::segment(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setUKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setVKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getUKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getVKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setUKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setVKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getUKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getVKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setPole(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setPoleCol(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setPoleRow(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getPole(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getPoles(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setWeight(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setWeightCol(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setWeightRow(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getWeight(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getWeights(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getResolution(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::movePoint(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setUNotPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setVNotPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setUPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setVPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setUOrigin(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setVOrigin(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getUMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getVMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getUMultiplicities(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getVMultiplicities(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::exchangeUV(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;

    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    surf->ExchangeUV();
    Py_Return;
}

PyObject* BSplineSurfacePy::uIso(PyObject * args)
{
    double u;
    if (!PyArg_ParseTuple(args, "d", &u))
        return 0;

    try {
        Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
            (getGeometryPtr()->handle());
        Handle_Geom_Curve c = surf->UIso(u);
        return new BSplineCurvePy(new GeomBSplineCurve(Handle_Geom_BSplineCurve::DownCast(c)));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineSurfacePy::vIso(PyObject * args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
            (getGeometryPtr()->handle());
        Handle_Geom_Curve c = surf->VIso(v);
        return new BSplineCurvePy(new GeomBSplineCurve(Handle_Geom_BSplineCurve::DownCast(c)));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

Py::Int BSplineSurfacePy::getUDegree(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    int deg = surf->UDegree();
    return Py::Int(deg);
}

Py::Int BSplineSurfacePy::getVDegree(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    int deg = surf->VDegree();
    return Py::Int(deg);
}

Py::Int BSplineSurfacePy::getMaxDegree(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->MaxDegree()); 
}

Py::Int BSplineSurfacePy::getNbUPoles(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->NbUPoles()); 
}

Py::Int BSplineSurfacePy::getNbVPoles(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->NbVPoles()); 
}

Py::Int BSplineSurfacePy::getNbUKnots(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->NbUKnots()); 
}

Py::Int BSplineSurfacePy::getNbVKnots(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(surf->NbVKnots()); 
}

Py::Object BSplineSurfacePy::getFirstUKnotIndex(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    int index = surf->FirstUKnotIndex();
    return Py::Int(index);
}

Py::Object BSplineSurfacePy::getLastUKnotIndex(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    int index = surf->LastUKnotIndex();
    return Py::Int(index);
}

Py::Object BSplineSurfacePy::getFirstVKnotIndex(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    int index = surf->FirstVKnotIndex();
    return Py::Int(index);
}

Py::Object BSplineSurfacePy::getLastVKnotIndex(void) const
{
    Handle_Geom_BSplineSurface surf = Handle_Geom_BSplineSurface::DownCast
        (getGeometryPtr()->handle());
    int index = surf->LastVKnotIndex();
    return Py::Int(index);
}

Py::List BSplineSurfacePy::getUKnotSequence(void) const
{
    return Py::List();
}

Py::List BSplineSurfacePy::getVKnotSequence(void) const
{
    return Py::List();
}

PyObject *BSplineSurfacePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int BSplineSurfacePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
