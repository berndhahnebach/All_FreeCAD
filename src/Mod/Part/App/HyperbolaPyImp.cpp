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
# include <Geom_Hyperbola.hxx>
#endif

#include <Base/VectorPy.h>

#include "Geometry.h"
#include "HyperbolaPy.h"
#include "HyperbolaPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *HyperbolaPy::representation(void) const
{
    return "<GeomHyperbola object>";
}

PyObject *HyperbolaPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of HyperbolaPy and the Twin object 
    return new HyperbolaPy(new GeomHyperbola);
}

// constructor method
int HyperbolaPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        Handle_Geom_Hyperbola c = Handle_Geom_Hyperbola::DownCast
            (getGeometryPtr()->handle());
        c->SetMajorRadius(1.0);
        c->SetMinorRadius(1.0);
        return 0;
    }

    return -1;
}

Py::Float HyperbolaPy::getEccentricity(void) const
{
    Handle_Geom_Hyperbola curve = Handle_Geom_Hyperbola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->Eccentricity()); 
}

Py::Float HyperbolaPy::getFocal(void) const
{
    Handle_Geom_Hyperbola curve = Handle_Geom_Hyperbola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->Focal()); 
}

Py::Object HyperbolaPy::getFocus1(void) const
{
    Handle_Geom_Hyperbola c = Handle_Geom_Hyperbola::DownCast
        (getGeometryPtr()->handle());
    gp_Pnt loc = c->Focus1();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

Py::Object HyperbolaPy::getFocus2(void) const
{
    Handle_Geom_Hyperbola c = Handle_Geom_Hyperbola::DownCast
        (getGeometryPtr()->handle());
    gp_Pnt loc = c->Focus2();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

Py::Float HyperbolaPy::getParameter(void) const
{
    Handle_Geom_Hyperbola curve = Handle_Geom_Hyperbola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->Parameter()); 
}

Py::Float HyperbolaPy::getMajorRadius(void) const
{
    Handle_Geom_Hyperbola curve = Handle_Geom_Hyperbola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->MajorRadius()); 
}

void HyperbolaPy::setMajorRadius(Py::Float arg)
{
    Handle_Geom_Hyperbola curve = Handle_Geom_Hyperbola::DownCast(getGeometryPtr()->handle());
    curve->SetMajorRadius((double)arg); 
}

Py::Float HyperbolaPy::getMinorRadius(void) const
{
    Handle_Geom_Hyperbola curve = Handle_Geom_Hyperbola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->MinorRadius()); 
}

void HyperbolaPy::setMinorRadius(Py::Float arg)
{
    Handle_Geom_Hyperbola curve = Handle_Geom_Hyperbola::DownCast(getGeometryPtr()->handle());
    curve->SetMinorRadius((double)arg); 
}

Py::Object HyperbolaPy::getLocation(void) const
{
    Handle_Geom_Hyperbola c = Handle_Geom_Hyperbola::DownCast
        (getGeometryPtr()->handle());
    gp_Pnt loc = c->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void HyperbolaPy::setLocation(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_Hyperbola c = Handle_Geom_Hyperbola::DownCast
            (getGeometryPtr()->handle());
        c->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object HyperbolaPy::getAxis(void) const
{
    Handle_Geom_Hyperbola c = Handle_Geom_Hyperbola::DownCast
        (getGeometryPtr()->handle());
    gp_Dir dir = c->Axis().Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void HyperbolaPy::setAxis(Py::Object arg)
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
        Handle_Geom_Hyperbola this_curv = Handle_Geom_Hyperbola::DownCast
            (this->getGeometryPtr()->handle());
        gp_Ax1 axis;
        axis.SetLocation(this_curv->Location());
        axis.SetDirection(gp_Dir(dir_x, dir_y, dir_z));
        gp_Dir dir = axis.Direction();
        this_curv->SetAxis(axis);
    }
    catch (Standard_Failure) {
        throw Py::Exception("cannot set axis");
    }
}

PyObject *HyperbolaPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int HyperbolaPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


