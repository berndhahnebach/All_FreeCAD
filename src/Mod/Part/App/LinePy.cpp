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

  if ( pcObj )
  {
    LinePy* pcLine = (LinePy*)pcObj;
    ((LinePy*)self)->_Line = pcLine->_Line;
  }

  return 0;
}

LinePy::LinePy(PyTypeObject *T)
  : PyObjectBase(0,T)
{
}

LinePy::LinePy(const Part::Line3f &rcLine, PyTypeObject *T)
  : PyObjectBase(0,T), _Line(rcLine)
{
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
LinePy::~LinePy()						// Everything handled in parent
{
}

//--------------------------------------------------------------------------
// VectorPy representation
//--------------------------------------------------------------------------
PyObject *LinePy::_repr(void)
{
  std::stringstream a;
  a << "Part.Line (";
  a << _Line.b.x << ", "<< _Line.b.y << ", "<< _Line.b.z << "; "; 
  a << _Line.e.x << ", "<< _Line.e.y << ", "<< _Line.e.z ; 
  a << ")" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}

//--------------------------------------------------------------------------
// LinePy Attributes
//--------------------------------------------------------------------------
PyObject *LinePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "__dict__")) {
    PyObject *dict = PyDict_New();
    if (dict) {
      PyDict_SetItemString(dict,"bx", Py_BuildValue("f",_Line.b.x));
      PyDict_SetItemString(dict,"by", Py_BuildValue("f",_Line.b.y));
      PyDict_SetItemString(dict,"bz", Py_BuildValue("f",_Line.b.z));
      PyDict_SetItemString(dict,"ex", Py_BuildValue("f",_Line.e.x));
      PyDict_SetItemString(dict,"ey", Py_BuildValue("f",_Line.e.y));
      PyDict_SetItemString(dict,"ez", Py_BuildValue("f",_Line.e.z));
      if (PyErr_Occurred()) { Py_DECREF(dict);dict = NULL;}
    }
    return dict;
  }
  else if (Base::streq(attr, "bx"))
    return Py_BuildValue("f",_Line.b.x);
  else if (Base::streq(attr, "by"))
    return Py_BuildValue("f",_Line.b.y);
  else if (Base::streq(attr, "bz"))
    return Py_BuildValue("f",_Line.b.z);
  else if (Base::streq(attr, "ex"))
    return Py_BuildValue("f",_Line.e.x);
  else if (Base::streq(attr, "ey"))
    return Py_BuildValue("f",_Line.e.y);
  else if (Base::streq(attr, "ez"))
    return Py_BuildValue("f",_Line.e.z);
  else
   _getattr_up(PyObjectBase);
} 

int LinePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "bx")) {
    if (PyInt_Check(value))
      _Line.b.x = (float)PyInt_AsLong(value);
    else if (PyFloat_Check(value))
      _Line.b.x = (float)PyFloat_AsDouble(value);
    return 0;
  } else if (Base::streq(attr, "by")) {
    if (PyInt_Check(value))
      _Line.b.y = (float)PyInt_AsLong(value);
    else if (PyFloat_Check(value))
      _Line.b.y = (float)PyFloat_AsDouble(value);
    return 0;
  } else if (Base::streq(attr, "bz")) {
    if (PyInt_Check(value))
      _Line.b.z = (float)PyInt_AsLong(value);
    else if (PyFloat_Check(value))
      _Line.b.z = (float)PyFloat_AsDouble(value);
    return 0;
  } else if (Base::streq(attr, "ex")) {
    if (PyInt_Check(value))
      _Line.e.x = (float)PyInt_AsLong(value);
    else if (PyFloat_Check(value))
      _Line.e.x = (float)PyFloat_AsDouble(value);
    return 0;
  } else if (Base::streq(attr, "ey")) {
    if (PyInt_Check(value))
      _Line.e.y = (float)PyInt_AsLong(value);
    else if (PyFloat_Check(value))
      _Line.e.y = (float)PyFloat_AsDouble(value);
    return 0;
  } else if (Base::streq(attr, "ez")) {
    if (PyInt_Check(value))
      _Line.e.z = (float)PyInt_AsLong(value);
    else if (PyFloat_Check(value))
      _Line.e.z = (float)PyFloat_AsDouble(value);
    return 0;
  }

  return PyObjectBase::_setattr(attr, value); 						
}

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
