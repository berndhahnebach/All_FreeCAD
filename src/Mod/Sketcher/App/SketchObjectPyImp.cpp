/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2010     *
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
#include <boost/shared_ptr.hpp>

#include "Mod/Sketcher/App/SketchObject.h"
#include <Mod/Part/App/LinePy.h>
#include <Mod/Part/App/Geometry.h>
#include <Base/VectorPy.h>

// inclusion of the generated files (generated out of SketchObjectSFPy.xml)
#include "SketchObjectPy.h"
#include "SketchObjectPy.cpp"
// other python types
#include "ConstraintPy.h"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string SketchObjectPy::representation(void) const
{
    return "<Sketcher::SketchObject>";
}


PyObject* SketchObjectPy::solve(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* SketchObjectPy::addGeometry(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj))
        return 0;

    if (PyObject_TypeCheck(pcObj, &(Part::GeometryPy::Type))) {
        Part::Geometry *geo = static_cast<Part::GeometryPy*>(pcObj)->getGeometryPtr();
        return Py::new_reference_to(Py::Int(this->getSketchObjectPtr()->addGeometry(geo)));
    }
    Py_Return; 
}

PyObject* SketchObjectPy::delGeometry(PyObject *args)
{
    int Index;
    if (!PyArg_ParseTuple(args, "i", &Index))
        return 0;

    if (this->getSketchObjectPtr()->delGeometry(Index)) {
        Base::Console().Error("Not able to delete a geometry with the given index: %i.\n", Index);
        return 0;
    }

    Py_Return; 
}

PyObject* SketchObjectPy::addConstraint(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj))
        return 0;

    if (PyObject_TypeCheck(pcObj, &(Sketcher::ConstraintPy::Type))) {
        Sketcher::Constraint *constr = static_cast<Sketcher::ConstraintPy*>(pcObj)->getConstraintPtr();
        return Py::new_reference_to(Py::Int(this->getSketchObjectPtr()->addConstraint(constr)));
    }
    Py_Return; 
}

PyObject* SketchObjectPy::delConstraint(PyObject *args)
{
    int Index;
    if (!PyArg_ParseTuple(args, "i", &Index))
        return 0;

    if (this->getSketchObjectPtr()->delConstraint(Index)) {
        Base::Console().Error("Not able to delete a constraint with the given index: %i.\n", Index);
        return 0;
    }

    Py_Return; 
}

PyObject* SketchObjectPy::delConstraintOnPoint(PyObject *args)
{
    int Index;
    if (!PyArg_ParseTuple(args, "i", &Index))
        return 0;

    this->getSketchObjectPtr()->delConstraintOnPoint(Index);

    Py_Return; 
}

PyObject* SketchObjectPy::setDatum(PyObject *args)
{
    double Datum;
    int    Index;
    if (!PyArg_ParseTuple(args, "di", &Datum, &Index))
        return 0;

    this->getSketchObjectPtr()->setDatum(Datum, Index);

    Py_Return; 
}

PyObject* SketchObjectPy::movePoint(PyObject *args)
{
    PyObject *pcObj;
    int GeoId, PointType;

    if (!PyArg_ParseTuple(args, "iiO!", &GeoId, &PointType, &(Base::VectorPy::Type), &pcObj))
        return 0;

    Base::Vector3d v1 = static_cast<Base::VectorPy*>(pcObj)->value();

    this->getSketchObjectPtr()->movePoint(GeoId,(Sketcher::PointPos)PointType,v1);

    Py_Return; 

}

Py::Int SketchObjectPy::getConstraintCount(void) const
{
    //return Py::Int();
    throw Py::AttributeError("Not yet implemented");
}

Py::Int SketchObjectPy::getGeometryCount(void) const
{
    //return Py::Int();
    throw Py::AttributeError("Not yet implemented");
}

PyObject *SketchObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SketchObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


