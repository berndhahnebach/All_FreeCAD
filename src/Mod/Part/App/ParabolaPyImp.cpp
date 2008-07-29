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
# include <Geom_Parabola.hxx>
#endif

#include <Base/VectorPy.h>

#include "Geometry.h"
#include "ParabolaPy.h"
#include "ParabolaPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *ParabolaPy::representation(void) const
{
    return "<Parabola object>";
}

PyObject *ParabolaPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of ParabolaPy and the Twin object 
    return new ParabolaPy(new GeomParabola);
}

// constructor method
int ParabolaPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        Handle_Geom_Parabola c = Handle_Geom_Parabola::DownCast
            (getGeometryPtr()->handle());
        c->SetFocal(1.0);
        return 0;
    }

    return -1;
}

Py::Float ParabolaPy::getEccentricity(void) const
{
    Handle_Geom_Parabola curve = Handle_Geom_Parabola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->Eccentricity()); 
}

Py::Float ParabolaPy::getFocal(void) const
{
    Handle_Geom_Parabola curve = Handle_Geom_Parabola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->Focal()); 
}

void ParabolaPy::setFocal(Py::Float arg)
{
    Handle_Geom_Parabola curve = Handle_Geom_Parabola::DownCast(getGeometryPtr()->handle());
    curve->SetFocal((double)arg); 
}

Py::Object ParabolaPy::getFocus(void) const
{
    Handle_Geom_Parabola c = Handle_Geom_Parabola::DownCast
        (getGeometryPtr()->handle());
    gp_Pnt loc = c->Focus();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

Py::Float ParabolaPy::getParameter(void) const
{
    Handle_Geom_Parabola curve = Handle_Geom_Parabola::DownCast(getGeometryPtr()->handle());
    return Py::Float(curve->Parameter()); 
}

Py::Object ParabolaPy::getLocation(void) const
{
    Handle_Geom_Parabola c = Handle_Geom_Parabola::DownCast
        (getGeometryPtr()->handle());
    gp_Pnt loc = c->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void ParabolaPy::setLocation(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_Parabola c = Handle_Geom_Parabola::DownCast
            (getGeometryPtr()->handle());
        c->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object ParabolaPy::getAxis(void) const
{
    Handle_Geom_Parabola c = Handle_Geom_Parabola::DownCast
        (getGeometryPtr()->handle());
    gp_Dir dir = c->Axis().Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void ParabolaPy::setAxis(Py::Object arg)
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
        Handle_Geom_Parabola this_curv = Handle_Geom_Parabola::DownCast
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

PyObject *ParabolaPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ParabolaPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


