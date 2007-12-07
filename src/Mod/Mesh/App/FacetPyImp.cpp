/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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

#include "Mesh.h"
#include "Facet.h"
#include "FacetPy.h"
#include "FacetPy.cpp"

#include <Base/VectorPy.h>

using namespace Mesh;

// returns a string which represent the object e.g. when printed in python
const char *FacetPy::representation(void) const
{
    FacetPy::PointerType ptr = getFacetPtr();
    std::stringstream str;
    str << "Facet (";
    if (ptr->isBound()) {
        str << "(" << ptr->_aclPoints[0].x << ", " << ptr->_aclPoints[0].y << ", " << ptr->_aclPoints[0].z << ", Idx=" << ptr->PIndex[0] << "), ";
        str << "(" << ptr->_aclPoints[1].x << ", " << ptr->_aclPoints[1].y << ", " << ptr->_aclPoints[1].z << ", Idx=" << ptr->PIndex[1] << "), ";
        str << "(" << ptr->_aclPoints[2].x << ", " << ptr->_aclPoints[2].y << ", " << ptr->_aclPoints[2].z << ", Idx=" << ptr->PIndex[2] << "), ";
        str << "Idx=" << ptr->Index << ", (" << ptr->NIndex[0] << ", " << ptr->NIndex[1] << ", " << ptr->NIndex[2] << ")";
    }
    else {
        str << "(" << ptr->_aclPoints[0].x << ", " << ptr->_aclPoints[0].y << ", " << ptr->_aclPoints[0].z << "), ";
        str << "(" << ptr->_aclPoints[1].x << ", " << ptr->_aclPoints[1].y << ", " << ptr->_aclPoints[1].z << "), ";
        str << "(" << ptr->_aclPoints[2].x << ", " << ptr->_aclPoints[2].y << ", " << ptr->_aclPoints[2].z << ")";
    }
    str << ")";
 
    // Note: As the return type is 'const char*' we cannot create a temporary char array neither on the stack because the array would be freed
    // when leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class. This, however, is not a problem as long as we only
    // use this method in _repr().
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

// constructor method
int FacetPy::PyInit(PyObject* args, PyObject*k)
{
    return 0;
}

PyObject*  FacetPy::unbound(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    getFacetPtr()->Index = ULONG_MAX;
    getFacetPtr()->Mesh = 0;
    Py_Return;
}

Py::Int FacetPy::getIndex(void) const
{
    return Py::Int((long) getFacetPtr()->Index);
}

Py::Bool FacetPy::getBound(void) const
{
    return Py::Bool(getFacetPtr()->Index != UINT_MAX);
}

Py::Object FacetPy::getNormal(void) const
{
    Base::VectorPy* normal = new Base::VectorPy(getFacetPtr()->GetNormal());
    normal->setConst();
    return Py::Object(normal);
}

PyObject *FacetPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int FacetPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


