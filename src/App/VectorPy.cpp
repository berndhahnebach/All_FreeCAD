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

#include "../Base/PyExportImp.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
using Base::Console;


#include "VectorPy.h"

using namespace App;




//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject App::VectorPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"VectorPy",				/*tp_name*/
	sizeof(VectorPy),			/*tp_basicsize*/
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
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "About App::VectorPy",                            /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  0,                                                /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &Base::PyObjectBase::Type,                        /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  0,                                                /*tp_init */
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
PyMethodDef App::VectorPy::Methods[] = {
//  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},
	PYMETHODEDEF(set)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::VectorPy::Parents[] = {&PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
App::VectorPy::VectorPy(const Base::Vector3f &rcVector, PyTypeObject *T)
: PyObjectBase( T), _cVector(rcVector)
{
	Base::Console().Log("Create VectorPy: %p \n",this);
}

PyObject *VectorPy::PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds)	// Python wrapper
{
	float x=0.0,y=0.0,z=0.0;
  if (!PyArg_ParseTuple(args, "|fff", &x,&y,&z))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Base::Console().Log("Constructor VectorPy\n");
	return new VectorPy(Base::Vector3f(x,y,z));
}


//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
VectorPy::~VectorPy()						// Everything handled in parent
{
	Base::Console().Log("Destroy VectorPy: %p \n",this);
}


//--------------------------------------------------------------------------
// VectorPy representation
//--------------------------------------------------------------------------
PyObject *VectorPy::_repr(void)
{
  std::stringstream a;
  a << "App.Vector ( ";
  a << _cVector.x << ", "<< _cVector.y << ", "<< _cVector.z ; 
  a << ")" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// VectorPy Attributes
//--------------------------------------------------------------------------
PyObject *VectorPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
   _getattr_up(PyObjectBase); 						
} 

int VectorPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  return PyObjectBase::_setattr(attr, value); 						
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(VectorPy,set)
{
  float x,y,z;
  PyObject *pyObject;
  if ( PyArg_ParseTuple(args, "fff", &x, &y, &z) ) {
    _cVector.Set(x,y,z);
  }
  else if ( PyArg_ParseTuple(args, "O", &pyObject) ) {
    PY_TRY{
      // clears the error from the first PyArg_ParseTuple()
      PyErr_Clear();
      if ( PyObject_TypeCheck(pyObject, &(VectorPy::Type)) ) {
   	    VectorPy  *pcVector = (VectorPy*)pyObject;
        // check for the identical object
        if ( this != pcVector )
          _cVector = pcVector->value();
      } else if ( PyTuple_Check(pyObject) && PyTuple_Size(pyObject)==3 ) {
        PyObject* item;
        // x coordinate
        item = PyTuple_GetItem(pyObject,0);
        if (PyFloat_Check(item))
          _cVector.x = (float)PyFloat_AsDouble(item);
        else if (PyInt_Check(item))
          _cVector.x = (float)PyInt_AsLong(item);
        // y coordinate
        item = PyTuple_GetItem(pyObject,1);
        if (PyFloat_Check(item))
          _cVector.y = (float)PyFloat_AsDouble(item);
        else if (PyInt_Check(item))
          _cVector.y = (float)PyInt_AsLong(item);
        // z coordinate
        item = PyTuple_GetItem(pyObject,2);
        if (PyFloat_Check(item))
          _cVector.z = (float)PyFloat_AsDouble(item);
        else if (PyInt_Check(item))
          _cVector.z = (float)PyInt_AsLong(item);
      }
	  }PY_CATCH;
  }
  else
    return NULL; // NULL triggers exception 

	Py_Return;
}


