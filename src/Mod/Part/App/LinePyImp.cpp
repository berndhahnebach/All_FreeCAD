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
# include <gp_Lin.hxx>
# include <Geom_Line.hxx>
# include <Geom_TrimmedCurve.hxx>
# include <GC_MakeLine.hxx>
# include <GC_MakeSegment.hxx>
#endif

#include "Mod/Part/App/Geometry.h"
#include <Base/VectorPy.h>

// inclusion of the generated files (generated out of LinePy.xml)
#include "LinePy.h"
#include "LinePy.cpp"

using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
const char *LinePy::representation(void) const
{
    return "<GeomLine object>";
}

PyObject *LinePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of LinePy and the Twin object 
    return new LinePy(new GeomLineSegment);
}

// constructor method
int LinePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        // default line
        return 0;
    }

    PyErr_Clear();
    PyObject *pLine;
    if (PyArg_ParseTuple(args, "O!", &(LinePy::Type), &pLine)) {
        // Copy line
        LinePy* pcLine = static_cast<LinePy*>(pLine);
        // get Geom_Line of line segment
        Handle_Geom_TrimmedCurve that_curv = Handle_Geom_TrimmedCurve::DownCast
            (pcLine->getGeomLineSegmentPtr()->handle());
        Handle_Geom_Line that_line = Handle_Geom_Line::DownCast
            (that_curv->BasisCurve());
        // get Geom_Line of line segment
        Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast
            (this->getGeomLineSegmentPtr()->handle());
        Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
            (this_curv->BasisCurve());

        // Assign the lines
        this_line->SetLin(that_line->Lin());
        this_curv->SetTrim(that_curv->FirstParameter(), that_curv->LastParameter());
        return 0;
    }

    PyErr_Clear();
    PyObject *pV1, *pV2;
    if (PyArg_ParseTuple(args, "O!O!", &(Base::VectorPy::Type), &pV1,
                                       &(Base::VectorPy::Type), &pV2)) {
        Base::Vector3d v1 = static_cast<Base::VectorPy*>(pV1)->value();
        Base::Vector3d v2 = static_cast<Base::VectorPy*>(pV2)->value();
        try {
            // Create line out of two points
            if (v1 == v2) Standard_Failure::Raise("Both points are equal");
            GC_MakeSegment ms(gp_Pnt(v1.x,v1.y,v1.z),
                              gp_Pnt(v2.x,v2.y,v2.z));
            if (!ms.IsDone()) {
                PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(ms.Status()));
                return -1;
            }

            // get Geom_Line of line segment
            Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast
                (this->getGeomLineSegmentPtr()->handle());
            Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
                (this_curv->BasisCurve());
            Handle_Geom_TrimmedCurve that_curv = ms.Value();
            Handle_Geom_Line that_line = Handle_Geom_Line::DownCast(that_curv->BasisCurve());
            this_line->SetLin(that_line->Lin());
            this_curv->SetTrim(that_curv->FirstParameter(), that_curv->LastParameter());
            return 0;
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return -1;
        }
        catch (...) {
            PyErr_SetString(PyExc_Exception, "creation of line failed");
            return -1;
        }
    }

    PyErr_SetString(PyExc_TypeError, "Line constructor accepts:\n"
        "-- empty parameter list\n"
        "-- Line\n"
        "-- Vector, Vector");
    return -1;
}

PyObject* LinePy::setParameterRange(PyObject *args)
{
    double first, last;
    if (!PyArg_ParseTuple(args, "dd", &first, &last))
        return NULL;

    try {
        Handle_Geom_TrimmedCurve this_curve = Handle_Geom_TrimmedCurve::DownCast
            (this->getGeomLineSegmentPtr()->handle());
        this_curve->SetTrim(first, last);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return NULL;
    }

    Py_Return; 
}

Py::Object LinePy::getStartPoint(void) const
{
    Handle_Geom_TrimmedCurve this_curve = Handle_Geom_TrimmedCurve::DownCast
        (this->getGeomLineSegmentPtr()->handle());
    gp_Pnt pnt = this_curve->StartPoint();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)pnt.X(), (float)pnt.Y(), (float)pnt.Z()));
    return Py::Object(vec);
}

Py::Object LinePy::getEndPoint(void) const
{
    Handle_Geom_TrimmedCurve this_curve = Handle_Geom_TrimmedCurve::DownCast
        (this->getGeomLineSegmentPtr()->handle());
    gp_Pnt pnt = this_curve->EndPoint();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)pnt.X(), (float)pnt.Y(), (float)pnt.Z()));
    return Py::Object(vec);
}

PyObject *LinePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int LinePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
