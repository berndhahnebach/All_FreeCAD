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

#include "gp_Circ.hxx"

// inclusion of the generated files (generated out of CirclePy.xml)
#include "CirclePy.h"
#include "CirclePy.cpp"

#include <Base/VectorPy.h>
#include <Geom_Circle.hxx>
#include <GC_MakeCircle.hxx>

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *CirclePy::representation(void) const
{
    CirclePy::PointerType ptr = reinterpret_cast<CirclePy::PointerType>(_pcTwinPointer);
    std::stringstream str;

    gp_Ax1 axis = getgp_CircPtr()->Axis();
    gp_Dir dir = axis.Direction();
    gp_Pnt loc = axis.Location();
    Standard_Real fRad = getgp_CircPtr()->Radius();
    str << "Part.Circle (";
    str << "Radius : " << fRad << ", "; 
    str << "Position : (" << loc.X() << ", "<< loc.Y() << ", "<< loc.Z() << "), "; 
    str << "Direction : (" << dir.X() << ", "<< dir.Y() << ", "<< dir.Z() << "), "; 
    str << ")";

    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

// constructor method
int CirclePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pCirc;
    PyObject *pV1, *pV2, *pV3;
    double dist;
    if (PyArg_ParseTuple(args, "O!d", &(CirclePy::Type), &pCirc, &dist)) {
        CirclePy* pcCircle = static_cast<CirclePy*>(pCirc);
        GC_MakeCircle mc(pcCircle->value(), dist);
        if (mc.Value().IsNull()) {
            PyErr_SetString(PyExc_Exception, "creation of circle failed");
            return -1;
        }

        CirclePy::PointerType ptr = reinterpret_cast<CirclePy::PointerType>(_pcTwinPointer);
        *ptr = mc.Value()->Circ();
    }
    else if (PyArg_ParseTuple(args, "O!", &(CirclePy::Type), &pCirc)) {
        CirclePy* pcCircle = static_cast<CirclePy*>(pCirc);
        CirclePy::PointerType ptr = reinterpret_cast<CirclePy::PointerType>(_pcTwinPointer);
        *ptr = pcCircle->value();
        PyErr_Clear();
    }
    else if (PyArg_ParseTuple(args, "O!O!O!", &(Base::VectorPy::Type), &pV1,
                                              &(Base::VectorPy::Type), &pV2,
                                              &(Base::VectorPy::Type), &pV3)) {
        Base::Vector3d v1 = static_cast<Base::VectorPy*>(pV1)->value();
        Base::Vector3d v2 = static_cast<Base::VectorPy*>(pV2)->value();
        Base::Vector3d v3 = static_cast<Base::VectorPy*>(pV3)->value();
        GC_MakeCircle mc(gp_Pnt(v1.x,v1.y,v1.z),
                         gp_Pnt(v2.x,v2.y,v2.z),
                         gp_Pnt(v3.x,v3.y,v3.z));
        if (mc.Value().IsNull()) {
            PyErr_SetString(PyExc_Exception, "cannot create circle with collinear points");
            return -1;
        }

        CirclePy::PointerType ptr = reinterpret_cast<CirclePy::PointerType>(_pcTwinPointer);
        *ptr = mc.Value()->Circ();
        PyErr_Clear();
    }
    else if (PyArg_ParseTuple(args, "")) {
        PyErr_Clear();
        getgp_CircPtr()->SetRadius(1.0);
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Circle constructor accepts:\n"
            "-- empty parameter list\n"
            "-- Circle\n"
            "-- Circle, double\n"
            "-- Vector, Vector, Vector");
        return -1;
    }

    return 0;
}
#if 0 // for later use
Py::Float CirclePy::getRadius(void) const
{
    return Py::Float(getgp_CircPtr()->Radius());
}

void  CirclePy::setRadius(Py::Float arg)
{
    getgp_CircPtr()->SetRadius((double)arg);
}

Py::Object CirclePy::getLocation(void) const
{
    gp_Pnt loc = getgp_CircPtr()->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void  CirclePy::setLocation(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        getgp_CircPtr()->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object CirclePy::getAxis(void) const
{
    gp_Ax1 axis = getgp_CircPtr()->Axis();
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
        axis.SetLocation(getgp_CircPtr()->Location());
        axis.SetDirection(gp_Dir(val.x, val.y, val.z));
        gp_Dir dir = axis.Direction();
        getgp_CircPtr()->SetAxis(axis);
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
    if ( PyArg_ParseTuple(args, "O", &pyObject) ) {
        if ( PyObject_TypeCheck(pyObject, &(Base::VectorPy::Type)) ) {
            Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pyObject);
            Base::Vector3d* val = pcObject->getVectorPtr();
            Base::Vector3f v((float)val->x,(float)val->y,(float)val->z);
            gp_Ax1 axis;
            axis.SetLocation(getgp_CircPtr()->Location());
            axis.SetDirection(gp_Dir(v.x, v.y, v.z));
            gp_Dir dir = axis.Direction();
            getgp_CircPtr()->SetAxis(axis);
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }

    Py_Return; 
}

PyObject* CirclePy::axis(PyObject *args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;
    gp_Ax1 axis = getgp_CircPtr()->Axis();
    gp_Dir dir = axis.Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f((float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return vec; 
}

PyObject* CirclePy::setPosition(PyObject *args)
{
    PyObject *pyObject;
    if ( PyArg_ParseTuple(args, "O", &pyObject) ) {
        if ( PyObject_TypeCheck(pyObject, &(Base::VectorPy::Type)) ) {
            Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pyObject);
            Base::Vector3d* val = pcObject->getVectorPtr();
            Base::Vector3f v((float)val->x,(float)val->y,(float)val->z);
            getgp_CircPtr()->SetLocation(gp_Pnt(v.x, v.y, v.z));
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }

    Py_Return; 
}

PyObject* CirclePy::position(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    gp_Pnt loc = getgp_CircPtr()->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f((float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return vec; 
}

PyObject* CirclePy::setRadius(PyObject *args)
{
    double Float;
    if (!PyArg_ParseTuple(args, "d",&Float))
        return NULL;
    getgp_CircPtr()->SetRadius(Float);
    Py_Return; 
}

PyObject* CirclePy::radius(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;                         
    return Py_BuildValue("d",getgp_CircPtr()->Radius()); 
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
