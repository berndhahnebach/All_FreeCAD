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
#endif

#include "../Base/PyExportImp.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
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
	"MatrixPy",				/*tp_name*/
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
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "About App::MatrixPy",                            /*tp_doc */
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
PyMethodDef App::MatrixPy::Methods[] = {

  PYMETHODEDEF(set)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::MatrixPy::Parents[] = {&PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
App::MatrixPy::MatrixPy(const Base::Matrix4D &rcMatrix, PyTypeObject *T)
: PyObjectBase( T), _cMatrix(rcMatrix)
{
	Base::Console().Log("Create MatrixPy: %p \n",this);
}

PyObject *MatrixPy::PyMake(PyTypeObject *ignored, PyObject *args, PyObject *kwds)	// Python wrapper
{
	//float x=0.0,y=0.0,z=0.0;
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C
    return NULL;                       // NULL triggers exception 

  Base::Console().Log("Constructor MatrixPy\n");
	return new MatrixPy();
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
  a << "App.Matrix ( ";
  a << "(" << _cMatrix[0][0] << ","<< _cMatrix[1][0] << ","<< _cMatrix[2][0] << ","<< _cMatrix[3][0] << ")" << ",";
  a << "(" << _cMatrix[0][1] << ","<< _cMatrix[1][1] << ","<< _cMatrix[2][1] << ","<< _cMatrix[3][1] << ")"<< ",";
  a << "(" << _cMatrix[0][2] << ","<< _cMatrix[1][2] << ","<< _cMatrix[2][2] << ","<< _cMatrix[3][2] << ")"<< ",";
  a << "(" << _cMatrix[0][3] << ","<< _cMatrix[1][3] << ","<< _cMatrix[2][3] << ","<< _cMatrix[3][3] << ")";
  a << ")" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// MatrixPy Attributes
//--------------------------------------------------------------------------
PyObject *MatrixPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
/*		if (Base::streq(attr, "XXX"))
    {
      if(_pcMaterial->ambientColor.a == 0.0)			
			  return Py_BuildValue("(fff)",_pcMaterial->ambientColor.r,_pcMaterial->ambientColor.g,_pcMaterial->ambientColor.b); 
      else
			  return Py_BuildValue("(ffff)",_pcMaterial->ambientColor.r,_pcMaterial->ambientColor.g,_pcMaterial->ambientColor.b,_pcMaterial->ambientColor.a); 
        
    }else*/

	    _getattr_up(PyObjectBase); 						
} 


int MatrixPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	  return PyObjectBase::_setattr(attr, value); 						
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(MatrixPy,set)
{
	char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  PY_TRY{

  }PY_CATCH;

	Py_Return;
}


