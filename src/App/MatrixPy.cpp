/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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

#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Vector3D.h>
#include <Base/VectorPy.h>

using Base::Vector3f;
using Base::Console;

#include "MatrixPy.h"
using Base::Matrix4D;


using namespace App;




//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject App::MatrixPy::Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,						/*ob_size*/
    "App.Matrix",				/*tp_name*/
    sizeof(MatrixPy),			/*tp_basicsize*/
    0,						/*tp_itemsize*/
    /* methods */
    PyDestructor,	  		/*tp_dealloc*/
    0,			 			/*tp_print*/
    __getattr, 				/*tp_getattr*/
    __setattr, 				/*tp_setattr*/
    0,						/*tp_compare*/
    __repr,					/*tp_repr*/
    0,						/*tp_as_number*/
    0,						/*tp_as_sequence*/
    0,						/*tp_as_mapping*/
    0,						/*tp_hash*/
    0,						/*tp_call */
    0,                                                /*tp_str  */
    0,                                                /*tp_getattro*/
    0,                                                /*tp_setattro*/
    /* --- Functions to access object as input/output buffer ---------*/
    0,                                                /* tp_as_buffer */
    /* --- Flags to define presence of optional/expanded features */
    Py_TPFLAGS_HAVE_CLASS,                            /*tp_flags */
    "About App.Matrix",                               /*tp_doc */
    0,                                                /*tp_traverse */
    0,                                                /*tp_clear */
    0,                                                /*tp_richcompare */
    0,                                                /*tp_weaklistoffset */
    0,                                                /*tp_iter */
    0,                                                /*tp_iternext */
    MatrixPy::Methods,                                /*tp_methods */
    0,                                                /*tp_members */
    0,                                                /*tp_getset */
    &Base::PyObjectBase::Type,                        /*tp_base */
    0,                                                /*tp_dict */
    0,                                                /*tp_descr_get */
    0,                                                /*tp_descr_set */
    0,                                                /*tp_dictoffset */
    0,                                                /*tp_init */
    0,                                                /*tp_alloc */
    MatrixPy::PyMake,                                 /*tp_new */
    0,                                                /*tp_free   Low-level free-memory routine */
    0,                                                /*tp_is_gc  For PyObject_IS_GC */
    0,                                                /*tp_bases */
    0,                                                /*tp_mro    method resolution order */
    0,                                                /*tp_cache */
    0,                                                /*tp_subclasses */
    0                                                 /*tp_weaklist */

};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef App::MatrixPy::Methods[] = {

//  PYMETHODEDEF(set)
    PYMETHODEDEF(move)
    PYMETHODEDEF(rotateX)
    PYMETHODEDEF(rotateY)
    PYMETHODEDEF(rotateZ)
    PYMETHODEDEF(scale)
    PYMETHODEDEF(transform)
    PYMETHODEDEF(unity)

    {
        NULL, NULL
    }		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::MatrixPy::Parents[] = {&PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
App::MatrixPy::MatrixPy(const Base::Matrix4D &rcMatrix, PyTypeObject *T)
        : PyObjectBase(0, T), _cMatrix(rcMatrix)
{
    Base::Console().Log("Create MatrixPy: %p \n",this);
}

PyObject *MatrixPy::PyMake(PyTypeObject *ignored, PyObject *args, PyObject *kwds)	// Python wrapper
{
    double a11=1.0, a12=0.0, a13=0.0, a14=0.0;
    double a21=0.0, a22=1.0, a23=0.0, a24=0.0;
    double a31=0.0, a32=0.0, a33=1.0, a34=0.0;
    double a41=0.0, a42=0.0, a43=0.0, a44=1.0;

    if (!PyArg_ParseTuple(args, "|dddddddddddddddd",&a11,&a12,&a13,&a14,
                          &a21,&a22,&a23,&a24,    
                          &a31,&a32,&a33,&a34,
                          &a41,&a42,&a43,&a44))     // convert args: Python->C
        return NULL;                       // NULL triggers exception

    Base::Console().Log("Constructor MatrixPy\n");
    return new MatrixPy( Matrix4D (a11,a12,a13,a14,
                                   a21,a22,a23,a24,
                                   a31,a32,a33,a34,
                                   a41,a42,a43,a44) );
}


//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
MatrixPy::~MatrixPy()						// Everything handled in parent
{
    Base::Console().Log("Destroy MatrixPy: %p \n",this);
}


//--------------------------------------------------------------------------
// MatrixPy representation
//--------------------------------------------------------------------------
PyObject *MatrixPy::_repr(void)
{
    std::stringstream a;
    a << "App.Matrix (";
    a << "(" << _cMatrix[0][0] << ","<< _cMatrix[0][1] << ","<< _cMatrix[0][2] << ","<< _cMatrix[0][3] << ")" << ",";
    a << "(" << _cMatrix[1][0] << ","<< _cMatrix[1][1] << ","<< _cMatrix[1][2] << ","<< _cMatrix[1][3] << ")"<< ",";
    a << "(" << _cMatrix[2][0] << ","<< _cMatrix[2][1] << ","<< _cMatrix[2][2] << ","<< _cMatrix[2][3] << ")"<< ",";
    a << "(" << _cMatrix[3][0] << ","<< _cMatrix[3][1] << ","<< _cMatrix[3][2] << ","<< _cMatrix[3][3] << ")";
    a << ")";
    return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// MatrixPy Attributes
//--------------------------------------------------------------------------
PyObject *MatrixPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
    if (Base::streq(attr, "__dict__")) {
        PyObject *dict = PyDict_New();
        if (dict) {
            PyDict_SetItemString(dict,"a11", PyFloat_FromDouble(_cMatrix[0][0]));
            PyDict_SetItemString(dict,"a12", PyFloat_FromDouble(_cMatrix[0][1]));
            PyDict_SetItemString(dict,"a13", PyFloat_FromDouble(_cMatrix[0][2]));
            PyDict_SetItemString(dict,"a14", PyFloat_FromDouble(_cMatrix[0][3]));
            PyDict_SetItemString(dict,"a21", PyFloat_FromDouble(_cMatrix[1][0]));
            PyDict_SetItemString(dict,"a22", PyFloat_FromDouble(_cMatrix[1][1]));
            PyDict_SetItemString(dict,"a23", PyFloat_FromDouble(_cMatrix[1][2]));
            PyDict_SetItemString(dict,"a24", PyFloat_FromDouble(_cMatrix[1][3]));
            PyDict_SetItemString(dict,"a31", PyFloat_FromDouble(_cMatrix[2][0]));
            PyDict_SetItemString(dict,"a32", PyFloat_FromDouble(_cMatrix[2][1]));
            PyDict_SetItemString(dict,"a33", PyFloat_FromDouble(_cMatrix[2][2]));
            PyDict_SetItemString(dict,"a34", PyFloat_FromDouble(_cMatrix[2][3]));
            PyDict_SetItemString(dict,"a41", PyFloat_FromDouble(_cMatrix[3][0]));
            PyDict_SetItemString(dict,"a42", PyFloat_FromDouble(_cMatrix[3][1]));
            PyDict_SetItemString(dict,"a43", PyFloat_FromDouble(_cMatrix[3][2]));
            PyDict_SetItemString(dict,"a44", PyFloat_FromDouble(_cMatrix[3][3]));
            if (PyErr_Occurred()) {
                Py_DECREF(dict);
                dict = NULL;
            }
        }
        return dict;
    }
    else if (Base::streq(attr, "a11"))
        return PyFloat_FromDouble(_cMatrix[0][0]);
    else if (Base::streq(attr, "a12"))
        return PyFloat_FromDouble(_cMatrix[0][1]);
    else if (Base::streq(attr, "a13"))
        return PyFloat_FromDouble(_cMatrix[0][2]);
    else if (Base::streq(attr, "a14"))
        return PyFloat_FromDouble(_cMatrix[0][3]);
    else if (Base::streq(attr, "a21"))
        return PyFloat_FromDouble(_cMatrix[1][0]);
    else if (Base::streq(attr, "a22"))
        return PyFloat_FromDouble(_cMatrix[1][1]);
    else if (Base::streq(attr, "a23"))
        return PyFloat_FromDouble(_cMatrix[1][2]);
    else if (Base::streq(attr, "a24"))
        return PyFloat_FromDouble(_cMatrix[1][3]);
    else if (Base::streq(attr, "a31"))
        return PyFloat_FromDouble(_cMatrix[2][0]);
    else if (Base::streq(attr, "a32"))
        return PyFloat_FromDouble(_cMatrix[2][1]);
    else if (Base::streq(attr, "a33"))
        return PyFloat_FromDouble(_cMatrix[2][2]);
    else if (Base::streq(attr, "a34"))
        return PyFloat_FromDouble(_cMatrix[2][3]);
    else if (Base::streq(attr, "a41"))
        return PyFloat_FromDouble(_cMatrix[3][0]);
    else if (Base::streq(attr, "a42"))
        return PyFloat_FromDouble(_cMatrix[3][1]);
    else if (Base::streq(attr, "a43"))
        return PyFloat_FromDouble(_cMatrix[3][2]);
    else if (Base::streq(attr, "a44"))
        return PyFloat_FromDouble(_cMatrix[3][3]);
    else
        _getattr_up(PyObjectBase);
}


int MatrixPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{
    if (Base::streq(attr, "a11")) {
        if (PyInt_Check(value))
            _cMatrix[0][0] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[0][0] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a12")) {
        if (PyInt_Check(value))
            _cMatrix[0][1] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[0][1] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a13")) {
        if (PyInt_Check(value))
            _cMatrix[0][2] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[0][2] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a14")) {
        if (PyInt_Check(value))
            _cMatrix[0][3] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[0][3] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a21")) {
        if (PyInt_Check(value))
            _cMatrix[1][0] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[1][0] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a22")) {
        if (PyInt_Check(value))
            _cMatrix[1][1] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[1][1] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a23")) {
        if (PyInt_Check(value))
            _cMatrix[1][2] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[1][2] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a24")) {
        if (PyInt_Check(value))
            _cMatrix[1][3] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[1][3] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a31")) {
        if (PyInt_Check(value))
            _cMatrix[2][0] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[2][0] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a32")) {
        if (PyInt_Check(value))
            _cMatrix[2][1] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[2][1] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a33")) {
        if (PyInt_Check(value))
            _cMatrix[2][2] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[2][2] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a34")) {
        if (PyInt_Check(value))
            _cMatrix[2][3] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[2][3] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a41")) {
        if (PyInt_Check(value))
            _cMatrix[3][0] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[3][0] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a42")) {
        if (PyInt_Check(value))
            _cMatrix[3][1] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[3][1] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a43")) {
        if (PyInt_Check(value))
            _cMatrix[3][2] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[3][2] = (float)PyFloat_AsDouble(value);
        return 0;
    }
    else if (Base::streq(attr, "a44")) {
        if (PyInt_Check(value))
            _cMatrix[3][3] = (double)PyInt_AsLong(value);
        else if (PyFloat_Check(value))
            _cMatrix[3][3] = (float)PyFloat_AsDouble(value);
        return 0;
    }


    return PyObjectBase::_setattr(attr, value);
}


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
/*
PYFUNCIMP_D(MatrixPy,set)
{
	char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
    return NULL;                             // NULL triggers exception

  PY_TRY{

  }PY_CATCH;

	Py_Return;
}
*/

PYFUNCIMP_D(MatrixPy,move)
{
    double x,y,z;
    Base::Vector3d vec;
    PyObject *pcVecObj;

    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z)) {   // convert args: Python->C
        vec.x = x;
        vec.y = y;
        vec.z = z;
    }
    else if (PyArg_ParseTuple(args, "O!:three floats or a vector is needed", &(Base::VectorPy::Type), &pcVecObj)) {
        Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pcVecObj);
        Base::Vector3d* val = pcObject->getVectorPtr();
        vec.Set(val->x,val->y,val->z);
        // clears the error from the first PyArg_ParseTuple()6
        PyErr_Clear();
    }
    else
        return NULL;                                 // NULL triggers exception

    PY_TRY {
        _cMatrix.move(vec);

    }
    PY_CATCH;

    Py_Return;
}

PYFUNCIMP_D(MatrixPy,scale)
{
    double x,y,z;
    Base::Vector3d vec;
    PyObject *pcVecObj;

    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z)) {   // convert args: Python->C
        vec.x = x;
        vec.y = y;
        vec.z = z;
    }
    else if (PyArg_ParseTuple(args, "O!:three floats or a vector is needed", &(Base::VectorPy::Type), &pcVecObj)) {   // convert args: Python->C
        Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pcVecObj);
        Base::Vector3d* val = pcObject->getVectorPtr();
        vec.Set(val->x,val->y,val->z);
        // clears the error from the first PyArg_ParseTuple()6
        PyErr_Clear();
    }
    else
        return NULL;                                 // NULL triggers exception

    PY_TRY {
        _cMatrix.scale(vec);

    }
    PY_CATCH;

    Py_Return;
}

PYFUNCIMP_D(MatrixPy,unity)
{
    PY_TRY {
        _cMatrix.unity();
    }PY_CATCH;

    Py_Return;
}

PYFUNCIMP_D(MatrixPy,transform)
{
    Base::Vector3d vec;
    Matrix4D mat;
    PyObject *pcVecObj,*pcMatObj;

    if (PyArg_ParseTuple(args, "O!O!: a transform point (Vector) and a transform matrix (Matrix) is needed",
        &(Base::VectorPy::Type), &pcVecObj, &(MatrixPy::Type), &pcMatObj) ) {   // convert args: Python->C
        Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pcVecObj);
        Base::Vector3d* val = pcObject->getVectorPtr();
        vec.Set(val->x,val->y,val->z);
        mat = ((MatrixPy*)pcMatObj)->value();
        // clears the error from the first PyArg_ParseTuple()6
        PyErr_Clear();
    }
    else
        return NULL;                                 // NULL triggers exception

    PY_TRY {
        _cMatrix.transform(vec,mat);

    }
    PY_CATCH;

    Py_Return;
}

PYFUNCIMP_D(MatrixPy,rotateX)
{
    float a;

    if (!PyArg_ParseTuple(args, "f: angle to rotate (float) needed", &a))     // convert args: Python->C
        return NULL;                                 // NULL triggers exception

    PY_TRY {
        _cMatrix.rotX(a);

    }PY_CATCH;

    Py_Return;
}
PYFUNCIMP_D(MatrixPy,rotateY)
{
    float a;

    if (!PyArg_ParseTuple(args, "f: angle to rotate (float) needed", &a))     // convert args: Python->C
        return NULL;                                 // NULL triggers exception

    PY_TRY {
        _cMatrix.rotY(a);

    }PY_CATCH;

    Py_Return;
}
PYFUNCIMP_D(MatrixPy,rotateZ)
{
    float a;

    if (!PyArg_ParseTuple(args, "f: angle to rotate (float) needed", &a))     // convert args: Python->C
        return NULL;                                 // NULL triggers exception

    PY_TRY {
        _cMatrix.rotZ(a);

    }PY_CATCH;

    Py_Return;
}


