
#include "PreCompiled.h"
#ifndef _PreComp_
# include <gp_Ax1.hxx>
# include <gp_Dir.hxx>
# include <gp_Pnt.hxx>
# include <gp_Vec.hxx>
# include <gp_Trsf.hxx>
# include <Geom_Geometry.hxx>
#endif

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
    if (!PyArg_ParseTuple(args, "O!d", &(Base::VectorPy::Type),&o, &scale))
        return 0;
    Base::Vector3d vec = static_cast<Base::VectorPy*>(o)->value();
    gp_Pnt pnt(vec.x, vec.y, vec.z);
    getGeometryPtr()->handle()->Scale(pnt, scale);
    Py_Return;
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
    if (!PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type),&o))
        return 0;
    Base::Vector3d vec = static_cast<Base::VectorPy*>(o)->value();
    gp_Vec trl(vec.x, vec.y, vec.z);
    getGeometryPtr()->handle()->Translate(trl);
    Py_Return;
}

PyObject *GeometryPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int GeometryPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
