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

#include "Base/Rotation.h"

// inclusion of the generated files (generated out of RotationPy.xml)
#include "VectorPy.h"
#include "RotationPy.h"
#include "RotationPy.cpp"

using namespace Base;

// returns a string which represents the object e.g. when printed in python
const char *RotationPy::representation(void) const
{
    return "<Rotation object>";
}

PyObject *RotationPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of RotationPy and the Twin object 
    return new RotationPy(new Rotation);
}

// constructor method
int RotationPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject* o;
    if (PyArg_ParseTuple(args, "")) {
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &(Base::RotationPy::Type), &o)) {
        Base::Rotation *rot = static_cast<Base::RotationPy*>(o)->getRotationPtr();
        getRotationPtr()->setValue(rot->getValue());
        return 0;
    }

    PyErr_Clear();
    double angle;
    if (PyArg_ParseTuple(args, "O!d", &(Base::VectorPy::Type), &o, &angle)) {
        getRotationPtr()->setValue(static_cast<Base::VectorPy*>(o)->value(), angle);
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "empty parameter list, four floats or Vector and float");
    return -1;
}

PyObject* RotationPy::invert(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    this->getRotationPtr()->invert();
    Py_Return;
}

Py::Tuple RotationPy::getQ(void) const
{
    double q0, q1, q2, q3;
    this->getRotationPtr()->getValue(q0,q1,q2,q3);

    Py::Tuple tuple(4);
    tuple.setItem(0, Py::Float(q0));
    tuple.setItem(1, Py::Float(q1));
    tuple.setItem(2, Py::Float(q2));
    tuple.setItem(3, Py::Float(q3));
    return tuple;
}

void RotationPy::setQ(Py::Tuple arg)
{
    double q0 = (double)Py::Float(arg.getItem(0));
    double q1 = (double)Py::Float(arg.getItem(1));
    double q2 = (double)Py::Float(arg.getItem(2));
    double q3 = (double)Py::Float(arg.getItem(3));
    this->getRotationPtr()->setValue(q0,q1,q2,q3);
}

PyObject *RotationPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int RotationPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


