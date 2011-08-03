/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2010     *
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
# include <sstream>
#endif

#include "Mod/Sketcher/App/SketchObject.h"
#include <Mod/Part/App/LinePy.h>
#include <Mod/Part/App/Geometry.h>
#include <Base/VectorPy.h>
#include <App/Document.h>

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
        std::stringstream str;
        str << "Not able to delete a geometry with the given index: " << Index;
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
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
        std::stringstream str;
        str << "Not able to delete a constraint with the given index: " << Index;
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
        return 0;
    }

    Py_Return; 
}

PyObject* SketchObjectPy::addExternal(PyObject *args)
{
    char *ObjectName;
    char *SubName;
    if (!PyArg_ParseTuple(args, "ss:Give an object and subelement name", &ObjectName,&SubName))
        return 0;

    // get the target object for the external link
    App::DocumentObject * Obj = this->getSketchObjectPtr()->getDocument()->getObject(ObjectName);
    if (!Obj) {
        std::stringstream str;
        str << ObjectName << "does not exist in the document";
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
        return 0;
    }
    // check if its belong to the sketch support 
    if (this->getSketchObjectPtr()->Support.getValue() != Obj) {
        std::stringstream str;
        str << ObjectName << "is not supported by this sketch";
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
        return 0;
    }

    // add the external 
    if (this->getSketchObjectPtr()->addExternal(Obj,SubName)) {
        std::stringstream str;
        str << "Not able to add external shape element";
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
        return 0;
    }

    Py_Return; 
}

PyObject* SketchObjectPy::delExternal(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* SketchObjectPy::delConstraintOnPoint(PyObject *args)
{
    int Index;
    if (!PyArg_ParseTuple(args, "i", &Index))
        return 0;

    if (this->getSketchObjectPtr()->delConstraintOnPoint(Index)) {
        std::stringstream str;
        str << "Not able to delete a constraint on point with the given index: " << Index;
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
        return 0;
    }

    Py_Return; 
}

PyObject* SketchObjectPy::setDatum(PyObject *args)
{
    double Datum;
    int    Index;
    if (!PyArg_ParseTuple(args, "di", &Datum, &Index))
        return 0;

    if (this->getSketchObjectPtr()->setDatum(Datum, Index)) {
        std::stringstream str;
        str << "Datum " << Datum << " of constraint with index " << Index << " is invalid";
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
        return 0;
    }

    Py_Return; 
}

PyObject* SketchObjectPy::movePoint(PyObject *args)
{
    PyObject *pcObj;
    int GeoId, PointType;

    if (!PyArg_ParseTuple(args, "iiO!", &GeoId, &PointType, &(Base::VectorPy::Type), &pcObj))
        return 0;

    Base::Vector3d v1 = static_cast<Base::VectorPy*>(pcObj)->value();

    if (this->getSketchObjectPtr()->movePoint(GeoId,(Sketcher::PointPos)PointType,v1)) {
        std::stringstream str;
        str << "Not able to move point with the id and type: (" << GeoId << ", " << PointType << ")";
        PyErr_SetString(PyExc_ValueError, str.str().c_str());
        return 0;
    }

    Py_Return; 

}

Py::Int SketchObjectPy::getConstraintCount(void) const
{
    return Py::Int(this->getSketchObjectPtr()->Constraints.getSize());
}

Py::Int SketchObjectPy::getGeometryCount(void) const
{
    return Py::Int(this->getSketchObjectPtr()->Geometry.getSize());
}

PyObject *SketchObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SketchObjectPy::setCustomAttributes(const char* attr, PyObject* obj)
{
    // search in PropertyList
    App::Property *prop = getSketchObjectPtr()->getPropertyByName(attr);
    if (prop) {
        // Read-only attributes must not be set over its Python interface
        short Type =  getSketchObjectPtr()->getPropertyType(prop);
        if (Type & App::Prop_ReadOnly) {
            std::stringstream s;
            s << "Object attribute '" << attr << "' is read-only";
            throw Py::AttributeError(s.str());
        }

        prop->setPyObject(obj);
        
        if (strcmp(attr,"Geometry") == 0)
            getSketchObjectPtr()->rebuildVertexIndex();

        return 1;
    }

    return 0;
}
