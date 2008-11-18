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
# include <Geom_ConicalSurface.hxx>
# include <Geom_Circle.hxx>
# include <GC_MakeConicalSurface.hxx>
# include <gp_Circ.hxx>
# include <gp_Cone.hxx>
# include <gp_Lin.hxx>
# include <Geom_Line.hxx>
# include <Geom_TrimmedCurve.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/GeometryPyCXX.h>
#include <Base/VectorPy.h>

#include "Geometry.h"
#include "LinePy.h"
#include "CirclePy.h"
#include "ConePy.h"
#include "ConePy.cpp"

using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
const char *ConePy::representation(void) const
{
    return "<Cone object>";
}

PyObject *ConePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of ConePy and the Twin object 
    return new ConePy(new GeomCone);
}

// constructor method
int ConePy::PyInit(PyObject* args, PyObject* kwds)
{
    char* keywords_n[] = {NULL};
    if (PyArg_ParseTupleAndKeywords(args, kwds, "", keywords_n)) {
        Handle_Geom_ConicalSurface s = Handle_Geom_ConicalSurface::DownCast
            (getGeometryPtr()->handle());
        s->SetRadius(1.0);
        return 0;
    }

    PyObject *pV1, *pV2;
    float radius1, radius2;
    static char* keywords_pprr[] = {"Point1","Point2","Radius1","Radius2",NULL};
    PyErr_Clear();
    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!O!dd", keywords_pprr,
                                        &(Base::VectorPy::Type), &pV1,
                                        &(Base::VectorPy::Type), &pV2,
                                        &radius1, &radius2)) {
        Base::Vector3d v1 = static_cast<Base::VectorPy*>(pV1)->value();
        Base::Vector3d v2 = static_cast<Base::VectorPy*>(pV2)->value();
        GC_MakeConicalSurface mc(gp_Pnt(v1.x,v1.y,v1.z),
                                 gp_Pnt(v2.x,v2.y,v2.z),
                                 radius1, radius2);
        if (!mc.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(mc.Status()));
            return -1;
        }

        Handle_Geom_ConicalSurface cone = Handle_Geom_ConicalSurface::DownCast
            (getGeometryPtr()->handle());
        cone->SetCone(mc.Value()->Cone());
        return 0;
    }

    PyObject *pV3, *pV4;
    static char* keywords_pppp[] = {"Point1","Point2","Point3","Point4",NULL};
    PyErr_Clear();
    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!O!", keywords_pppp,
                                        &(Base::VectorPy::Type), &pV1,
                                        &(Base::VectorPy::Type), &pV2,
                                        &(Base::VectorPy::Type), &pV3,
                                        &(Base::VectorPy::Type), &pV4)) {
        Base::Vector3d v1 = static_cast<Base::VectorPy*>(pV1)->value();
        Base::Vector3d v2 = static_cast<Base::VectorPy*>(pV2)->value();
        Base::Vector3d v3 = static_cast<Base::VectorPy*>(pV3)->value();
        Base::Vector3d v4 = static_cast<Base::VectorPy*>(pV4)->value();
        GC_MakeConicalSurface mc(gp_Pnt(v1.x,v1.y,v1.z),
                                 gp_Pnt(v2.x,v2.y,v2.z),
                                 gp_Pnt(v3.x,v3.y,v3.z),
                                 gp_Pnt(v4.x,v4.y,v4.z));
        if (!mc.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(mc.Status()));
            return -1;
        }

        Handle_Geom_ConicalSurface cone = Handle_Geom_ConicalSurface::DownCast
            (getGeometryPtr()->handle());
        cone->SetCone(mc.Value()->Cone());
        return 0;
    }

    PyObject *pCone;
    double dist;
    static char* keywords_cd[] = {"Cone","Distance",NULL};
    PyErr_Clear();
    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!d", keywords_cd,
                                        &(ConePy::Type), &pCone, &dist)) {
        ConePy* pcCone = static_cast<ConePy*>(pCone);
        Handle_Geom_ConicalSurface pcone = Handle_Geom_ConicalSurface::DownCast
            (pcCone->getGeometryPtr()->handle());
        GC_MakeConicalSurface mc(pcone->Cone(), dist);
        if (!mc.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(mc.Status()));
            return -1;
        }

        Handle_Geom_ConicalSurface cone = Handle_Geom_ConicalSurface::DownCast
            (getGeometryPtr()->handle());
        cone->SetCone(mc.Value()->Cone());
        return 0;
    }

    static char* keywords_c[] = {"Cone",NULL};
    PyErr_Clear();
    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!d", keywords_c,
                                        &(ConePy::Type), &pCone)) {
        ConePy* pcCone = static_cast<ConePy*>(pCone);
        Handle_Geom_ConicalSurface pcone = Handle_Geom_ConicalSurface::DownCast
            (pcCone->getGeometryPtr()->handle());
        GC_MakeConicalSurface mc(pcone->Cone());
        if (!mc.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(mc.Status()));
            return -1;
        }

        Handle_Geom_ConicalSurface cone = Handle_Geom_ConicalSurface::DownCast
            (getGeometryPtr()->handle());
        cone->SetCone(mc.Value()->Cone());
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "Cone constructor accepts:\n"
        "-- empty parameter list\n"
        "-- Cone\n"
        "-- Cone, Distance\n"
        "-- Point1, Point2, Radius1, Radius2\n"
        "-- Point1, Point2, Point3, Point4");
    return -1;
}

PyObject* ConePy::uIso(PyObject * args)
{
    double u;
    if (!PyArg_ParseTuple(args, "d", &u))
        return 0;

    try {
        Handle_Geom_ConicalSurface cone = Handle_Geom_ConicalSurface::DownCast
            (getGeomConePtr()->handle());
        Handle_Geom_Line c = Handle_Geom_Line::DownCast(cone->UIso(u));
        GeomLineSegment* line = new GeomLineSegment();
        Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast
            (line->handle());
        Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
            (this_curv->BasisCurve());
        this_line->SetLin(c->Lin());
        return new LinePy(line);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* ConePy::vIso(PyObject * args)
{
    double v;
    if (!PyArg_ParseTuple(args, "d", &v))
        return 0;

    try {
        Handle_Geom_ConicalSurface cone = Handle_Geom_ConicalSurface::DownCast
            (getGeomConePtr()->handle());
        Handle_Geom_Curve c = cone->VIso(v);
        return new CirclePy(new GeomCircle(Handle_Geom_Circle::DownCast(c)));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

Py::Object ConePy::getApex(void) const
{
    Handle_Geom_ConicalSurface s = Handle_Geom_ConicalSurface::DownCast
        (getGeomConePtr()->handle());
    gp_Pnt loc = s->Apex();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

Py::Float ConePy::getRadius(void) const
{
    Handle_Geom_ConicalSurface s = Handle_Geom_ConicalSurface::DownCast
        (getGeomConePtr()->handle());
    return Py::Float(s->RefRadius()); 
}

void ConePy::setRadius(Py::Float arg)
{
    Handle_Geom_ConicalSurface s = Handle_Geom_ConicalSurface::DownCast
        (getGeomConePtr()->handle());
    s->SetRadius((double)arg);
}

Py::Float ConePy::getSemiAngle(void) const
{
    Handle_Geom_ConicalSurface s = Handle_Geom_ConicalSurface::DownCast
        (getGeomConePtr()->handle());
    return Py::Float(s->SemiAngle()); 
}

void ConePy::setSemiAngle(Py::Float arg)
{
    Handle_Geom_ConicalSurface s = Handle_Geom_ConicalSurface::DownCast
        (getGeomConePtr()->handle());
    s->SetSemiAngle((double)arg);
}

Py::Object ConePy::getCenter(void) const
{
    Handle_Geom_ElementarySurface s = Handle_Geom_ElementarySurface::DownCast
        (getGeomConePtr()->handle());
    gp_Pnt loc = s->Location();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)loc.X(), (float)loc.Y(), (float)loc.Z()));
    return Py::Object(vec);
}

void ConePy::setCenter(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        Handle_Geom_ElementarySurface s = Handle_Geom_ElementarySurface::DownCast
            (getGeomConePtr()->handle());
        s->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else if (PyObject_TypeCheck(p, &PyTuple_Type)) {
        Base::Vector3d loc = Base::getVectorFromTuple<double>(p);
        Handle_Geom_ElementarySurface s = Handle_Geom_ElementarySurface::DownCast
            (getGeomConePtr()->handle());
        s->SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object ConePy::getAxis(void) const
{
    Handle_Geom_ElementarySurface s = Handle_Geom_ElementarySurface::DownCast
        (getGeometryPtr()->handle());
    gp_Dir dir = s->Axis().Direction();
    Base::VectorPy* vec = new Base::VectorPy(Base::Vector3f(
        (float)dir.X(), (float)dir.Y(), (float)dir.Z()));
    return Py::Object(vec);
}

void ConePy::setAxis(Py::Object arg)
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
        Handle_Geom_ElementarySurface this_surf = Handle_Geom_ElementarySurface::DownCast
            (this->getGeometryPtr()->handle());
        gp_Ax1 axis;
        axis.SetLocation(this_surf->Location());
        axis.SetDirection(gp_Dir(dir_x, dir_y, dir_z));
        gp_Dir dir = axis.Direction();
        this_surf->SetAxis(axis);
    }
    catch (Standard_Failure) {
        throw Py::Exception("cannot set axis");
    }
}

PyObject *ConePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ConePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


