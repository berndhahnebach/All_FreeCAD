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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
# include <sstream>
#	include <stdlib.h>
#endif

#include "PyExportImp.h"
#include "Console.h"

using namespace Base;


// Constructor
PyObjectBase::PyObjectBase(PyTypeObject *T) 				
{
  this->ob_type = T;
  _Py_NewReference(this);
#ifdef FC_LOGPYOBJECTS
  Base::Console().Log("PyO+: %s (%p)\n",T->tp_name, this);
#endif
}
/// destructor
PyObjectBase::~PyObjectBase() 
{
#ifdef FC_LOGPYOBJECTS
  Base::Console().Log("PyO-: %s (%p)\n",this->ob_type->tp_name, this);
#endif
}

/*------------------------------
 * PyObjectBase Type		-- Every class, even the abstract one should have a Type
------------------------------*/

PyTypeObject PyObjectBase::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						                                   	/*ob_size*/
	"PyObjectBase",			                              /*tp_name*/
	sizeof(PyObjectBase),	                            /*tp_basicsize*/
	0,						                                   	/*tp_itemsize*/
	/* --- methods ---------------------------------------------- */
	PyDestructor,                                    	/*tp_dealloc*/
	0,			 		                                    	/*tp_print*/
	__getattr, 	                                    	/*tp_getattr*/
	__setattr, 	                                    	/*tp_setattr*/
	0,				                      	              	/*tp_compare*/
	__repr,		                      	                /*tp_repr*/
	0,				                      	              	/*tp_as_number*/
	0,		 		                      	              	/*tp_as_sequence*/
	0,				                      	              	/*tp_as_mapping*/
	0,				                      	              	/*tp_hash*/
	0,				                      		              /*tp_call */
  0,                                                /*tp_str  */
  0,                                                /*tp_getattro*/
  0,                                                /*tp_setattro*/
  /* --- Functions to access object as input/output buffer ---------*/
  0,                                                /* tp_as_buffer */
  /* --- Flags to define presence of optional/expanded features */
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "About PyObjectBase",                             /*tp_doc */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  0,                                                /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  0,                                                /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  0,                                                /*tp_init */
  0,                                                /*tp_alloc */
  0,                                                /*tp_new */
  0,                                                /*tp_free   Low-level free-memory routine */
  0,                                                /*tp_is_gc  For PyObject_IS_GC */
  0,                                                /*tp_bases */
  0,                                                /*tp_mro    method resolution order */
  0,                                                /*tp_cache */
  0,                                                /*tp_subclasses */
  0                                                 /*tp_weaklist */
};

/*------------------------------
 * PyObjectBase Methods 	-- Every class, even the abstract one should have a Methods
------------------------------*/
PyMethodDef PyObjectBase::Methods[] = {
 	PYMETHODEDEF(isA)

  {NULL, NULL}		/* Sentinel */
};

/*------------------------------
 * PyObjectBase Parents		-- Every class, even the abstract one should have parents
------------------------------*/
PyParentObject PyObjectBase::Parents[] = {&PyObjectBase::Type, NULL};

/*------------------------------
 * PyObjectBase attributes	-- attributes
------------------------------*/
PyObject *PyObjectBase::_getattr(char *attr)
{
  if (streq(attr, "type"))
    return Py_BuildValue("s", (*(GetParents()))->tp_name);
  else if (streq(attr, "__class__"))
    return Py_BuildValue("s","ObjectBase");
  else if (streq(attr, "__members__"))
    return Py_FindMethod(Methods, this, attr); 
  
  Py_Return;
}

int PyObjectBase::_setattr(char *attr, PyObject *value)
{
  std::string err = "Unknown attribute: ";
  err += attr;
  PyErr_SetString(PyExc_AttributeError,err.c_str());
  return -1;
}

// static wrapper
int __setattr(PyObject *PyObj, char *attr, PyObject *value)
{
  return ((PyObjectBase*) PyObj)->_setattr(attr, value);
}

/*------------------------------
 * PyObjectBase repr		-- representations
------------------------------*/
PyObject *PyObjectBase::_repr(void)
{
  std::stringstream a;
  a << "PyObjectBase: [ ";
  a << "]" << std::endl;
# ifdef FCDebug
    Console().Log("PyObjectBase::_repr() not overwriten representation!");
# endif
	return Py_BuildValue("s", a.str().c_str());
}

/*------------------------------
 * PyObjectBase isA		-- the isA functions
------------------------------*/
bool PyObjectBase::IsA(PyTypeObject *T)		// if called with a Type, use "typename"
{
  return IsA(T->tp_name);
}

bool PyObjectBase::IsA(const char *type_name)		// check typename of each parent
{
  int i;
  PyParentObject  P;
  PyParentObject *Ps = GetParents();

  for (P = Ps[i=0]; P != NULL; P = Ps[i++])
      if (streq(P->tp_name, type_name))
	return true;
  return false;
}

PYFUNCIMP_D(PyObjectBase,isA)
{
  char *type_name;
  Py_Try(PyArg_ParseTuple(args, "s", &type_name));
  if(IsA(type_name))
    {Py_INCREF(Py_True); return Py_True;}
  else
    {Py_INCREF(Py_False); return Py_False;};
}


float PyObjectBase::getFloatFromPy(PyObject *value)
{
  if(PyFloat_Check( value) )
  {
    return (float) PyFloat_AsDouble(value);
  }else if(PyInt_Check( value) )
  {
    return (float) PyInt_AsLong(value);
  }else
    throw "Not allowed type used (float or int expected)...";
}

