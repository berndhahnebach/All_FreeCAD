/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "LinePy.h"
#include <Base/VectorPy.h>

using namespace Part;

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject LinePy::Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,						                                    /*ob_size*/
    "Part.Line",				                              /*tp_name*/
    sizeof(LinePy),			                              /*tp_basicsize*/
    0,						                                    /*tp_itemsize*/
    /* ---------------------- Methods -----------------------------*/
    PyDestructor,	  		                              /*tp_dealloc*/
    0,			 			                                    /*tp_print*/
    __getattr, 				                                /*tp_getattr*/
    __setattr, 				                                /*tp_setattr*/
    0,						                                    /*tp_compare*/
    __repr,					                                  /*tp_repr*/
    0,						                                    /*tp_as_number*/
    0,						                                    /*tp_as_sequence*/
    0,						                                    /*tp_as_mapping*/
    0,						                                    /*tp_hash*/
    0,						                                    /*tp_call */
    0,                                                /*tp_str  */
    0,                                                /*tp_getattro*/
    0,                                                /*tp_setattro*/
    /* --- Functions to access object as input/output buffer ---------*/
    0,                                                /* tp_as_buffer */
    /* --- Flags to define presence of optional/expanded features */
    Py_TPFLAGS_HAVE_CLASS,                            /*tp_flags */
    "About Line",                                     /*tp_doc */
    0,                                                /*tp_traverse */
    0,                                                /*tp_clear */
    0,                                                /*tp_richcompare */
    0,                                                /*tp_weaklistoffset */
    0,                                                /*tp_iter */
    0,                                                /*tp_iternext */
    LinePy::Methods,                                  /*tp_methods */
    0,                                                /*tp_members */
    0,                                                /*tp_getset */
    &Base::PyObjectBase::Type,                        /*tp_base */
    0,                                                /*tp_dict */
    0,                                                /*tp_descr_get */
    0,                                                /*tp_descr_set */
    0,                                                /*tp_dictoffset */
    PyInit,                                           /*tp_init */
    0,                                                /*tp_alloc */
    PyMake,                                           /*tp_new */
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
PyMethodDef LinePy::Methods[] = {
    PYMETHODEDEF(setStartPoint)
    PYMETHODEDEF(setEndPoint)
    {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject LinePy::Parents[] = {&Base::PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------

PyObject* LinePy::PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds)	// Python wrapper
{
    return new LinePy();
}

int LinePy::PyInit(PyObject* self, PyObject* args, PyObject*)
{
    PyObject *pcObj=0;
    if (!PyArg_ParseTuple(args, "|O!", &(LinePy::Type), &pcObj))     // convert args: Python->C 
        return -1;                             // NULL triggers exception 

    if (pcObj) {
        LinePy* pcLine = (LinePy*)pcObj;
        ((LinePy*)self)->_Line = pcLine->_Line;
    }

    return 0;
}

LinePy::LinePy(PyTypeObject *T)
  : PyObjectBase(0,T)
{
}

LinePy::LinePy(const Line3f &rcLine, PyTypeObject *T)
  : PyObjectBase(0,T), _Line(rcLine)
{
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
LinePy::~LinePy()                       // Everything handled in parent
{
}

//--------------------------------------------------------------------------
// VectorPy representation
//--------------------------------------------------------------------------
PyObject *LinePy::_repr(void)
{
    const Base::Vector3f& pt1 = _Line.first;
    const Base::Vector3f& pt2 = _Line.second;
    
    std::stringstream a;
    a << "Part.Line (";
    a << pt1.x << ", "<< pt1.y << ", " << pt1.z << "; "; 
    a << pt2.x << ", "<< pt2.y << ", " << pt2.z; 
    a << ")";
    return Py_BuildValue("s", a.str().c_str());
}

//--------------------------------------------------------------------------
// LinePy Attributes
//--------------------------------------------------------------------------
PyObject *LinePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
    _getattr_up(PyObjectBase);
} 

int LinePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{
    return PyObjectBase::_setattr(attr, value); 						
}

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(LinePy,setStartPoint)
{
    PyObject *pyObject;
    if ( PyArg_ParseTuple(args, "O", &pyObject) ) {
        if ( PyObject_TypeCheck(pyObject, &(Base::VectorPy::Type)) ) {
            Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pyObject);
            Base::Vector3d* val = pcObject->getVectorPtr();
            Base::Vector3f vec((float)val->x,(float)val->y,(float)val->z);
            _Line.first = vec;
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }

    Py_Return; 
}

PYFUNCIMP_D(LinePy,setEndPoint)
{
    PyObject *pyObject;
    if (PyArg_ParseTuple(args, "O", &pyObject)) {
        if (PyObject_TypeCheck(pyObject, &(Base::VectorPy::Type))) {
            Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(pyObject);
            Base::Vector3d* val = pcObject->getVectorPtr();
            Base::Vector3f vec((float)val->x,(float)val->y,(float)val->z);
            _Line.second = vec;
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }

    Py_Return; 
}
