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


#ifndef PY_GEOMETRYPY_H
#define PY_GEOMETRYPY_H

#include "PyCXX/Objects.hxx"
#include "Vector3D.h"

namespace Base {
template <typename T>
inline Vector3<T> getVectorFromTuple(PyObject* o)
{
    Py::Tuple tuple(o);
    T x = (T)Py::Float(tuple.getItem(0));
    T y = (T)Py::Float(tuple.getItem(1));
    T z = (T)Py::Float(tuple.getItem(2));
    return Vector3<T>(x,y,z);
}
}

namespace Py {

class BaseExport Vector : public Object
{
public:
    explicit Vector (PyObject *pyob, bool owned): Object(pyob, owned) {
        validate();
    }

    Vector (const Vector& ob): Object(*ob) {
        validate();
    }

    explicit Vector (const Base::Vector3d&);
    explicit Vector (const Base::Vector3f&);

    Vector(const Object& other): Object(other.ptr()) {
        validate();
    }
    virtual bool accepts (PyObject *pyob) const {
        return pyob && Vector_TypeCheck (pyob);
    }
    Vector& operator= (const Object& rhs)
    {
        return (*this = *rhs);
    }

    Vector& operator= (PyObject* rhsp);
    Vector& operator= (const Base::Vector3d&);
    Vector& operator= (const Base::Vector3f&);

    Base::Vector3d toVector() const;

private:
    static int Vector_TypeCheck(PyObject *);
};

}

#endif // PY_GEOMETRYPY_H
