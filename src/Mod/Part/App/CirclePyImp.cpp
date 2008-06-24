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

#include "CirclePy.h"
#include "CirclePy.cpp"
#include "ArcPy.h"

#include <Base/VectorPy.h>

#include <gp_Circ.hxx>
#include <Geom_Circle.hxx>
#include <GC_MakeCircle.hxx>

using namespace Part;

const char* gce_ErrorStatusText(gce_ErrorType et)
{
    switch (et)
    {
    case gce_Done:
        return "Construction was successful";
    case gce_ConfusedPoints:
        return "Two points are coincident";
    case gce_NegativeRadius:
        return "Radius value is negative";
    case gce_ColinearPoints:
        return "Three points are collinear";
    case gce_IntersectionError:
        return "Intersection cannot be computed";
    case gce_NullAxis:
        return "Axis is undefined";
    case gce_NullAngle:
        return "Angle value is invalid (usually null)";
    case gce_NullRadius:
        return "Radius is null";
    case gce_InvertAxis:
        return "Axis value is invalid";
    case gce_BadAngle:
        return "Angle value is invalid";
    case gce_InvertRadius:
        return "Radius value is incorrect (usually with respect to another radius)";
    case gce_NullFocusLength:
        return "Focal distance is null";
    case gce_NullVector:
        return "Vector is null";
    case gce_BadEquation:
        return "Coefficients are incorrect (applies to the equation of a geometric object)";
    default:
        return "Creation of geometry failed";
    }
}

// returns a string which represents the object e.g. when printed in python
const char *CirclePy::representation(void) const
{
    Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
    gp_Ax1 axis = circle->Axis();
    gp_Dir dir = axis.Direction();
    gp_Pnt loc = axis.Location();
    Standard_Real fRad = circle->Radius();

    std::stringstream str;
    str << "Circle (";
    str << "Radius : " << fRad << ", "; 
    str << "Position : (" << loc.X() << ", "<< loc.Y() << ", "<< loc.Z() << "), "; 
    str << "Direction : (" << dir.X() << ", "<< dir.Y() << ", "<< dir.Z() << ")"; 
    str << ")";

    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *CirclePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of CirclePy and the Twin object 
    Handle_Geom_Circle circle = new Geom_Circle(gp_Circ());
    return new CirclePy(new GeomCircle(circle));
}

// constructor method
int CirclePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pCirc;
    PyObject *pV1, *pV2, *pV3;
    double dist;
    if (PyArg_ParseTuple(args, "O!d", &(CirclePy::Type), &pCirc, &dist)) {
        CirclePy* pcCircle = static_cast<CirclePy*>(pCirc);
        Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast
            (pcCircle->getGeomCirclePtr()->handle());
        GC_MakeCircle mc(circle->Circ(), dist);
        if (!mc.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(mc.Status()));
            return -1;
        }

        Handle_Geom_Circle circ = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
        circ->SetCirc(mc.Value()->Circ());
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!O!d", &(Base::VectorPy::Type), &pV1,
                                             &(Base::VectorPy::Type), &pV2,
                                             &dist)) {
        Base::Vector3d v1 = static_cast<Base::VectorPy*>(pV1)->value();
        Base::Vector3d v2 = static_cast<Base::VectorPy*>(pV2)->value();
        GC_MakeCircle mc(gp_Pnt(v1.x,v1.y,v1.z),
                         gp_Dir(v2.x,v2.y,v2.z),
                         dist);
        if (!mc.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(mc.Status()));
            return -1;
        }

        Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
        circle->SetCirc(mc.Value()->Circ());
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &(CirclePy::Type), &pCirc)) {
        CirclePy* pcCircle = static_cast<CirclePy*>(pCirc);
        Handle_Geom_Circle circ1 = Handle_Geom_Circle::DownCast
            (pcCircle->getGeomCirclePtr()->handle());
        Handle_Geom_Circle circ2 = Handle_Geom_Circle::DownCast
            (this->getGeomCirclePtr()->handle());
        circ2->SetCirc(circ1->Circ());
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!O!O!", &(Base::VectorPy::Type), &pV1,
                                              &(Base::VectorPy::Type), &pV2,
                                              &(Base::VectorPy::Type), &pV3)) {
        Base::Vector3d v1 = static_cast<Base::VectorPy*>(pV1)->value();
        Base::Vector3d v2 = static_cast<Base::VectorPy*>(pV2)->value();
        Base::Vector3d v3 = static_cast<Base::VectorPy*>(pV3)->value();
        GC_MakeCircle mc(gp_Pnt(v1.x,v1.y,v1.z),
                         gp_Pnt(v2.x,v2.y,v2.z),
                         gp_Pnt(v3.x,v3.y,v3.z));
        if (!mc.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(mc.Status()));
            return -1;
        }

        Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
        circle->SetCirc(mc.Value()->Circ());
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "")) {
        Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
        circle->SetRadius(1.0);
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "Circle constructor accepts:\n"
        "-- empty parameter list\n"
        "-- Circle\n"
        "-- Circle, double\n"
        "-- Vector, Vector, double\n"
        "-- Vector, Vector, Vector");
    return -1;
}

#if 0 // for later use
Py::Float CirclePy::getRadius(void) const
{
    return Py::Float(getGeom_CirclePtr()->Radius());
}

void  CirclePy::setRadius(Py::Float arg)
{
    getGeom_CirclePtr()->SetRadius((double)arg);
}

Py::Object CirclePy::getLocation(void) const
{
    gp_Pnt loc = getGeom_CirclePtr()->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void  CirclePy::setLocation(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        getGeom_CirclePtr()->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object CirclePy::getAxis(void) const
{
    gp_Ax1 axis = getGeom_CirclePtr()->Axis();
    gp_Dir dir = axis.Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void  CirclePy::setAxis(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d val = static_cast<Base::VectorPy*>(p)->value();
        gp_Ax1 axis;
        axis.SetLocation(getGeom_CirclePtr()->Location());
        axis.SetDirection(gp_Dir(val.x, val.y, val.z));
        gp_Dir dir = axis.Direction();
        getGeom_CirclePtr()->SetAxis(axis);
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}
#else // once redesigned remove this
PyObject* CirclePy::setAxis(PyObject *args)
{
    PyObject *pyObject;
    if (!PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type),&pyObject))
        return NULL;
    Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pyObject);
    Base::Vector3d* val = pcObject->getVectorPtr();
    Base::Vector3f v((float)val->x,(float)val->y,(float)val->z);
    Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
    try {
        gp_Ax1 axis;
        axis.SetLocation(circle->Location());
        axis.SetDirection(gp_Dir(v.x, v.y, v.z));
        gp_Dir dir = axis.Direction();
        circle->SetAxis(axis);
    }
    catch (const Standard_Failure&) {
        PyErr_SetString(PyExc_Exception, "cannot set axis");
        return NULL;
    }

    Py_Return; 
}

PyObject* CirclePy::axis(PyObject *args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;
    Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
    gp_Ax1 axis = circle->Axis();
    gp_Dir dir = axis.Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f((float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return vec; 
}

PyObject* CirclePy::setPosition(PyObject *args)
{
    PyObject *pyObject;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type), &pyObject))
        return NULL;
    Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pyObject);
    Base::Vector3d* val = pcObject->getVectorPtr();
    Base::Vector3f v((float)val->x,(float)val->y,(float)val->z);
    Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
    circle->SetLocation(gp_Pnt(v.x, v.y, v.z));

    Py_Return; 
}

PyObject* CirclePy::position(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
    gp_Pnt loc = circle->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f((float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return vec; 
}

PyObject* CirclePy::setRadius(PyObject *args)
{
    double Float;
    if (!PyArg_ParseTuple(args, "d",&Float))
        return NULL;
    Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
    circle->SetRadius(Float);
    Py_Return; 
}

PyObject* CirclePy::radius(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;                         
    Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast(getGeomCirclePtr()->handle());
    return Py_BuildValue("d",circle->Radius()); 
}
#endif
PyObject *CirclePy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int CirclePy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}
