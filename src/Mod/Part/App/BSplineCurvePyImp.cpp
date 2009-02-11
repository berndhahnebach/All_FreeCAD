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
# include <Geom_BSplineCurve.hxx>
# include <gp_Pnt.hxx>
# include <TColStd_Array1OfReal.hxx>
# include <TColgp_Array1OfPnt.hxx>
# include <TColStd_Array1OfInteger.hxx>
#endif

#include <Base/VectorPy.h>
#include <Base/GeometryPyCXX.h>

#include "Geometry.h"
#include "BSplineCurvePy.h"
#include "BSplineCurvePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *BSplineCurvePy::representation(void) const
{
    return "<BSplineCurve object>";
}

PyObject *BSplineCurvePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of BSplineCurvePy and the Twin object 
    return new BSplineCurvePy(new GeomBSplineCurve);
}

// constructor method
int BSplineCurvePy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* BSplineCurvePy::isRational(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = curve->IsRational();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* BSplineCurvePy::isPeriodic(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = curve->IsPeriodic();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* BSplineCurvePy::isClosed(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    Standard_Boolean val = curve->IsClosed();
    if (val) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* BSplineCurvePy::increaseDegree(PyObject * args)
{
    int degree;
    if (!PyArg_ParseTuple(args, "i", &degree))
        return 0;
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    curve->IncreaseDegree(degree);
    Py_Return;
}

PyObject* BSplineCurvePy::increaseMultiplicity(PyObject * args)
{
    int mult=-1;
    int start, end;
    if (!PyArg_ParseTuple(args, "ii|i", &start, &end, &mult))
        return 0;

    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    if (mult == -1) {
        mult = end;
        curve->IncreaseMultiplicity(start, mult);
    }
    else {
        curve->IncreaseMultiplicity(start, end, mult);
    }

    Py_Return;
}

PyObject* BSplineCurvePy::incrementMultiplicity(PyObject * args)
{
    int start, end, mult;
    if (!PyArg_ParseTuple(args, "iii", &start, &end, &mult))
        return 0;

    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->IncrementMultiplicity(start, end, mult);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    Py_Return;
}

PyObject* BSplineCurvePy::insertKnot(PyObject * args)
{
    double U, tol = 0.0;
    int M=1;
    PyObject* add = Py_True;
    if (!PyArg_ParseTuple(args, "d|idO!", &U, &M, &tol, &PyBool_Type, &add))
        return 0;

    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->InsertKnot(U,M,tol,(add==Py_True));
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    Py_Return;
}

PyObject* BSplineCurvePy::insertKnots(PyObject * args)
{
    double tol = 0.0;
    PyObject* add = Py_True;
    PyObject* obj1;
    PyObject* obj2;
    if (!PyArg_ParseTuple(args, "O!O!|dO!", &PyList_Type, &obj1,
                                            &PyList_Type, &obj2,
                                            &tol, &PyBool_Type, &add))
        return 0;

    try {
        Py::List knots(obj1);
        TColStd_Array1OfReal k(1,knots.size());
        int index=1;
        for (Py::List::iterator it = knots.begin(); it != knots.end(); ++it) {
            Py::Float val(*it);
            k(index++) = (double)val;
        }
        Py::List mults(obj2);
        TColStd_Array1OfInteger m(1,mults.size());
        index=1;
        for (Py::List::iterator it = mults.begin(); it != mults.end(); ++it) {
            Py::Int val(*it);
            m(index++) = (int)val;
        }

        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->InsertKnots(k,m,tol,(add==Py_True));
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    Py_Return;
}

PyObject* BSplineCurvePy::removeKnot(PyObject * args)
{
    double tol;
    int Index,M;
    if (!PyArg_ParseTuple(args, "iid", &Index, &M, &tol))
        return 0;

    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        Standard_Boolean ok = curve->RemoveKnot(Index,M,tol);
        if (ok) {
            Py_INCREF(Py_True);
            return Py_True;
        }
        else {
            Py_INCREF(Py_False);
            return Py_False;
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::segment(PyObject * args)
{
    double u1,u2;
    if (!PyArg_ParseTuple(args, "dd", &u1,&u2))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->Segment(u1,u2);
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::setKnot(PyObject * args)
{
    int Index, M=-1;
    double K;
    if (!PyArg_ParseTuple(args, "id|i", &Index, &K, &M))
        return 0;

    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    if (M == -1) {
        curve->SetKnot(Index, K);
    }
    else {
        curve->SetKnot(Index, K, M);
    }

    Py_Return;
}

PyObject* BSplineCurvePy::getKnot(PyObject * args)
{
    int Index;
    if (!PyArg_ParseTuple(args, "i", &Index))
        return 0;

    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    double M = curve->Knot(Index);

    return Py_BuildValue("d",M);
}

PyObject* BSplineCurvePy::setKnots(PyObject * args)
{
    PyObject* obj;
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &obj))
        return 0;
    try {
        Py::List list(obj);
        TColStd_Array1OfReal k(1,list.size());
        int index=1;
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            Py::Float val(*it);
            k(index++) = (double)val;
        }

        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->SetKnots(k);
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getKnots(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        TColStd_Array1OfReal w(1,curve->NbKnots());
        curve->Knots(w);
        Py::List knots;
        for (Standard_Integer i=w.Lower(); i<=w.Upper(); i++) {
            knots.append(Py::Float(w(i)));
        }
        return Py::new_reference_to(knots);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::setPole(PyObject * args)
{
    int index;
    double weight=-1.0;
    PyObject* p;
    if (!PyArg_ParseTuple(args, "iO!|d", &index, &(Base::VectorPy::Type), &p, &weight))
        return 0;
    Base::Vector3d vec = static_cast<Base::VectorPy*>(p)->value();
    gp_Pnt pnt(vec.x, vec.y, vec.z);
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        if (weight < 0.0)
            curve->SetPole(index,pnt);
        else
            curve->SetPole(index,pnt,weight);
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getPole(PyObject * args)
{
    int index;
    if (!PyArg_ParseTuple(args, "i", &index))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        gp_Pnt pnt = curve->Pole(index);
        Base::VectorPy* vec = new Base::VectorPy(Base::Vector3d(
            pnt.X(), pnt.Y(), pnt.Z()));
        return vec;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getPoles(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        TColgp_Array1OfPnt p(1,curve->NbPoles());
        curve->Poles(p);
        Py::List poles;
        for (Standard_Integer i=p.Lower(); i<=p.Upper(); i++) {
            gp_Pnt pnt = p(i);
            Base::VectorPy* vec = new Base::VectorPy(Base::Vector3d(
                pnt.X(), pnt.Y(), pnt.Z()));
            poles.append(Py::Object(vec));
        }
        return Py::new_reference_to(poles);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::setWeight(PyObject * args)
{
    int index;
    double weight;
    if (!PyArg_ParseTuple(args, "id", &index,&weight))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->SetWeight(index,weight);
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getWeight(PyObject * args)
{
    int index;
    if (!PyArg_ParseTuple(args, "i", &index))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        double weight = curve->Weight(index);
        return Py_BuildValue("d", weight);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getWeights(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        TColStd_Array1OfReal w(1,curve->NbPoles());
        curve->Weights(w);
        Py::List weights;
        for (Standard_Integer i=w.Lower(); i<=w.Upper(); i++) {
            weights.append(Py::Float(w(i)));
        }
        return Py::new_reference_to(weights);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getResolution(PyObject * args)
{
    double tol;
    if (!PyArg_ParseTuple(args, "d", &tol))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        double utol;
        curve->Resolution(tol,utol);
        return Py_BuildValue("d",utol);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::movePoint(PyObject * args)
{
    double U;
    int index1, index2;
    PyObject* pnt;
    if (!PyArg_ParseTuple(args, "dO!ii", &U, &(Base::VectorPy::Type),&pnt, &index1, &index2))
        return 0;
    try {
        Base::Vector3d p = static_cast<Base::VectorPy*>(pnt)->value();
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        int first, last;
        curve->MovePoint(U, gp_Pnt(p.x,p.y,p.z), index1, index2, first, last);
        return Py_BuildValue("(ii)",first, last);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::setNotPeriodic(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->SetNotPeriodic();
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::setPeriodic(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->SetPeriodic();
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::setOrigin(PyObject * args)
{
    int index;
    if (!PyArg_ParseTuple(args, "i", &index))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        curve->SetOrigin(index);
        Py_Return;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getMultiplicity(PyObject * args)
{
    int index;
    if (!PyArg_ParseTuple(args, "i", &index))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        int mult = curve->Multiplicity(index);
        return Py_BuildValue("i", mult);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

PyObject* BSplineCurvePy::getMultiplicities(PyObject * args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    try {
        Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
            (getGeometryPtr()->handle());
        TColStd_Array1OfInteger m(1,curve->NbKnots());
        curve->Multiplicities(m);
        Py::List mults;
        for (Standard_Integer i=m.Lower(); i<=m.Upper(); i++) {
            mults.append(Py::Int(m(i)));
        }
        return Py::new_reference_to(mults);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
}

Py::Int BSplineCurvePy::getDegree(void) const
{
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(curve->Degree()); 
}

Py::Int BSplineCurvePy::getMaxDegree(void) const
{
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(curve->MaxDegree()); 
}

Py::Int BSplineCurvePy::getNbPoles(void) const
{
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(curve->NbPoles()); 
}

Py::Int BSplineCurvePy::getNbKnots(void) const
{
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(curve->NbKnots()); 
}

Py::Object BSplineCurvePy::getStartPoint(void) const
{
    Handle_Geom_BSplineCurve c = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    gp_Pnt pnt = c->StartPoint();
    return Py::Vector(Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z()));
}

Py::Object BSplineCurvePy::getEndPoint(void) const
{
    Handle_Geom_BSplineCurve c = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    gp_Pnt pnt = c->EndPoint();
    return Py::Vector(Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z()));
}

Py::Object BSplineCurvePy::getFirstUKnotIndex(void) const
{
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(curve->FirstUKnotIndex()); 
}

Py::Object BSplineCurvePy::getLastUKnotIndex(void) const
{
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    return Py::Int(curve->LastUKnotIndex()); 
}

Py::List BSplineCurvePy::getKnotSequence(void) const
{
    Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast
        (getGeometryPtr()->handle());
    Standard_Integer m = 0;
    for (int i=1; i<= curve->NbKnots(); i++)
        m += curve->Multiplicity(i);
    TColStd_Array1OfReal k(1,m);
    curve->KnotSequence(k);
    Py::List list;
    for (Standard_Integer i=k.Lower(); i<=k.Upper(); i++) {
        list.append(Py::Float(k(i)));
    }
    return list;
}

PyObject *BSplineCurvePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int BSplineCurvePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
