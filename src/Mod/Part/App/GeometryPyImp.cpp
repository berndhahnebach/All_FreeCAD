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
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <gp_Ax1.hxx>
# include <gp_Dir.hxx>
# include <gp_Pnt.hxx>
# include <gp_Vec.hxx>
# include <gp_Trsf.hxx>
# include <Geom_Geometry.hxx>
# include <Geom_Curve.hxx>
# include <Geom_Surface.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/GeometryPyCXX.h>
#include <Base/Matrix.h>
#include <Base/MatrixPy.h>
#include <Base/Vector3D.h>
#include <Base/VectorPy.h>
#include <Base/Rotation.h>
#include <Base/Placement.h>
#include <Base/PlacementPy.h>

#include "Geometry.h"
#include "GeometryPy.h"
#include "GeometryPy.cpp"

#include "TopoShape.h"
#include "TopoShapePy.h"
#include "TopoShapeEdgePy.h"
#include "TopoShapeFacePy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *GeometryPy::representation(void) const
{
    return "<Geometry object>";
}

PyObject *GeometryPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(PyExc_RuntimeError,
        "You cannot create an instance of the abstract class 'Geometry'.");
    return 0;
}

// constructor method
int GeometryPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* GeometryPy::mirror(PyObject *args)
{
    PyObject* o;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type),&o)) {
        Base::Vector3d vec = static_cast<Base::VectorPy*>(o)->value();
        gp_Pnt pnt(vec.x, vec.y, vec.z);
        getGeometryPtr()->handle()->Mirror(pnt);
        Py_Return;
    }

    PyErr_Clear();
    PyObject* axis;
    if (PyArg_ParseTuple(args, "O!O!", &(Base::VectorPy::Type),&o,
                                       &(Base::VectorPy::Type),&axis)) {
        Base::Vector3d pnt = static_cast<Base::VectorPy*>(o)->value();
        Base::Vector3d dir = static_cast<Base::VectorPy*>(axis)->value();
        gp_Ax1 ax1(gp_Pnt(pnt.x,pnt.y,pnt.z), gp_Dir(dir.x,dir.y,dir.z));
        getGeometryPtr()->handle()->Mirror(ax1);
        Py_Return;
    }

    PyErr_SetString(PyExc_Exception, "either a point (vector) or axis (vector, vector) must be given");
    return 0;
}

PyObject* GeometryPy::rotate(PyObject *args)
{
    PyObject* o;
    if (!PyArg_ParseTuple(args, "O!", &(Base::PlacementPy::Type),&o))
        return 0;

    Base::Placement* plm = static_cast<Base::PlacementPy*>(o)->getPlacementPtr();
    Base::Rotation rot(plm->_q[0],plm->_q[1],plm->_q[2],plm->_q[3]);
    Base::Vector3d pnt, dir;
    double angle;

    rot.getValue(dir, angle);
    pnt = plm->_pos;
    
    gp_Ax1 ax1(gp_Pnt(pnt.x,pnt.y,pnt.z), gp_Dir(dir.x,dir.y,dir.z));
    getGeometryPtr()->handle()->Rotate(ax1, angle);
    Py_Return;
}

PyObject* GeometryPy::scale(PyObject *args)
{
    PyObject* o;
    double scale;
    Base::Vector3d vec;
    if (PyArg_ParseTuple(args, "O!d", &(Base::VectorPy::Type),&o, &scale)) {
        vec = static_cast<Base::VectorPy*>(o)->value();
        gp_Pnt pnt(vec.x, vec.y, vec.z);
        getGeometryPtr()->handle()->Scale(pnt, scale);
        Py_Return;
    }
    
    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!d", &PyTuple_Type,&o, &scale)) {
        vec = Base::getVectorFromTuple<double>(o);
        gp_Pnt pnt(vec.x, vec.y, vec.z);
        getGeometryPtr()->handle()->Scale(pnt, scale);
        Py_Return;
    }

    PyErr_SetString(PyExc_Exception, "either vector or tuple and float expected");
    return 0;
}

PyObject* GeometryPy::transform(PyObject *args)
{
    PyObject* o;
    if (!PyArg_ParseTuple(args, "O!", &(Base::MatrixPy::Type),&o))
        return 0;
    Base::Matrix4D mat = static_cast<Base::MatrixPy*>(o)->value();
    gp_Trsf trf;
    trf.SetValues(mat[0][0],mat[0][1],mat[0][2],mat[0][3],
                  mat[1][0],mat[1][1],mat[1][2],mat[1][3],
                  mat[2][0],mat[2][1],mat[2][2],mat[2][3],
                  0.00001,0.00001);
    trf.SetScaleFactor(mat[3][3]);
    getGeometryPtr()->handle()->Transform(trf);
    Py_Return;
}

PyObject* GeometryPy::translate(PyObject *args)
{
    PyObject* o;
    Base::Vector3d vec;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type),&o)) {
        vec = static_cast<Base::VectorPy*>(o)->value();
        gp_Vec trl(vec.x, vec.y, vec.z);
        getGeometryPtr()->handle()->Translate(trl);
        Py_Return;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &PyTuple_Type,&o)) {
        vec = Base::getVectorFromTuple<double>(o);
        gp_Vec trl(vec.x, vec.y, vec.z);
        getGeometryPtr()->handle()->Translate(trl);
        Py_Return;
    }

    PyErr_SetString(PyExc_Exception, "either vector or tuple expected");
    return 0;
}

PyObject* GeometryPy::toShape(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Curve c = Handle_Geom_Curve::DownCast(g);
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(g);
    try {
        if (!c.IsNull()) {
            double u,v;
            u=c->FirstParameter();
            v=c->LastParameter();
            if (!PyArg_ParseTuple(args, "|dd", &u,&v))
                return 0;
            BRepBuilderAPI_MakeEdge mkBuilder(c, u, v);
            TopoDS_Shape sh = mkBuilder.Shape();
            return new TopoShapeEdgePy(new TopoShape(sh));
        }
        if (!s.IsNull()) {
            double u1,u2,v1,v2;
            s->Bounds(u1,u2,v1,v2);
            if (!PyArg_ParseTuple(args, "|dddd", &u1,&u2,&v1,&v2))
                return 0;
            BRepBuilderAPI_MakeFace mkBuilder(s, u1, u2, v1, v2);
            TopoDS_Shape sh = mkBuilder.Shape();
            return new TopoShapeFacePy(new TopoShape(sh));
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is neither curve nor surface");
    return 0;
}

PyObject* GeometryPy::value(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Curve c = Handle_Geom_Curve::DownCast(g);
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(g);
    try {
        if (!c.IsNull()) {
            double u;
            if (!PyArg_ParseTuple(args, "d", &u))
                return 0;
            gp_Pnt p = c->Value(u);
            return new Base::VectorPy(Base::Vector3d(p.X(),p.Y(),p.Z()));
        }
        if (!s.IsNull()) {
            double u,v;
            if (!PyArg_ParseTuple(args, "dd", &u,&v))
                return 0;
            gp_Pnt p = s->Value(u,v);
            return new Base::VectorPy(Base::Vector3d(p.X(),p.Y(),p.Z()));
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is neither curve nor surface");
    return 0;
}

PyObject *GeometryPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int GeometryPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
