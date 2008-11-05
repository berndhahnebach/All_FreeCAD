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

#include "Geometry.h"
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

PyObject* BSplineSurfacePy::increaseDegree(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::increaseMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::incrementMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::insertKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::insertKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::removeKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::segment(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getKnot(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getKnots(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setPole(PyObject *args)
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

PyObject* BSplineSurfacePy::setNotPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setPeriodic(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::setOrigin(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getMultiplicity(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* BSplineSurfacePy::getMultiplicities(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

Py::Int BSplineSurfacePy::getDegree(void) const
{
    return Py::Int();
}

Py::Int BSplineSurfacePy::getMaxDegree(void) const
{
    return Py::Int();
}

Py::Int BSplineSurfacePy::getNbPoles(void) const
{
    return Py::Int();
}

Py::Object BSplineSurfacePy::getStartPoint(void) const
{
    return Py::Object();
}

Py::Object BSplineSurfacePy::getEndPoint(void) const
{
    return Py::Object();
}

Py::Object BSplineSurfacePy::getFirstUKnotIndex(void) const
{
    return Py::Object();
}

Py::Object BSplineSurfacePy::getLastUKnotIndex(void) const
{
    return Py::Object();
}

Py::List BSplineSurfacePy::getKnotSequence(void) const
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


