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

#include "Base/Placement.h"

// inclusion of the generated files (generated out of PlacementPy.xml)
#include "PlacementPy.h"
#include "PlacementPy.cpp"
#include "Matrix.h"
#include "MatrixPy.h"
#include "VectorPy.h"

using namespace Base;

// returns a string which represents the object e.g. when printed in python
const char *PlacementPy::representation(void) const
{
    PlacementPy::PointerType ptr = reinterpret_cast<PlacementPy::PointerType>(_pcTwinPointer);
    std::stringstream str;
    str << "Placement ((";
    str << ptr->_rot.getValue()[0] << ","<< ptr->_rot.getValue()[1] << "," << ptr->_rot.getValue()[2] << "," << ptr->_rot.getValue()[3];
    str << "),(";
    str << ptr->_pos.x << ","<< ptr->_pos.y << "," << ptr->_pos.z;
    str << "))";

    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *PlacementPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of PlacementPy and the Twin object 
    return new PlacementPy(new Placement);
}

// constructor method
int PlacementPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject* o;
    if (PyArg_ParseTuple(args, "")) {
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &(Base::MatrixPy::Type), &o)) {
        Base::Matrix4D mat = static_cast<Base::MatrixPy*>(o)->value();
        getPlacementPtr()->fromMatrix(mat);
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &(Base::PlacementPy::Type), &o)) {
        Base::Placement *plm = static_cast<Base::PlacementPy*>(o)->getPlacementPtr();
        getPlacementPtr()->_pos = plm->_pos;
        getPlacementPtr()->_rot = plm->_rot;
        return 0;
    }

    PyErr_Clear();
    PyObject* d;
    double angle;
    if (PyArg_ParseTuple(args, "O!dO!", &(Base::VectorPy::Type), &d, &angle,
                                        &(Base::VectorPy::Type), &o)) {
        Base::Rotation rot(static_cast<Base::VectorPy*>(d)->value(), angle);
        getPlacementPtr()->_pos = static_cast<Base::VectorPy*>(o)->value();
        getPlacementPtr()->_rot = rot;
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "empty parameter list, matrix or placement expected");
    return -1;
}

PyObject* PlacementPy::move(PyObject * args)
{
    PyObject *vec;
    if (!PyArg_ParseTuple(args, "O!", &(VectorPy::Type), &vec))
        return NULL;
    getPlacementPtr()->_pos += static_cast<VectorPy*>(vec)->value();
    Py_Return;
}

PyObject* PlacementPy::toMatrix(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    Base::Matrix4D mat = getPlacementPtr()->toMatrix();
    return new MatrixPy(new Matrix4D(mat));
}

PyObject* PlacementPy::inverse(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    Base::Placement p = getPlacementPtr()->inverse();
    return new PlacementPy(new Placement(p));
}

Py::Object PlacementPy::getBase(void) const
{
    return Py::Object(new VectorPy(getPlacementPtr()->_pos));
}

void PlacementPy::setBase(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(VectorPy::Type))) {
        getPlacementPtr()->_pos = static_cast<VectorPy*>(p)->value();
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Tuple PlacementPy::getRotation(void) const
{
    Py::Tuple rot(4);
    for (int i=0; i<4; i++)
        rot[i]=Py::Float(getPlacementPtr()->_rot.getValue()[i]);
    return rot;
}

void PlacementPy::setRotation(Py::Tuple arg)
{
    getPlacementPtr()->_rot.setValue((double)Py::Float(arg[0]),
                                     (double)Py::Float(arg[1]),
                                     (double)Py::Float(arg[2]),
                                     (double)Py::Float(arg[3])
                                     );
}

PyObject *PlacementPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int PlacementPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
