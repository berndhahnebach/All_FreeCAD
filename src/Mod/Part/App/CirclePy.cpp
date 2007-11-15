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

#include "CirclePy.h"
#include <App/VectorPy.h>

using namespace Part;

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject CirclePy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						                                    /*ob_size*/
	"Part.Circle",			                              /*tp_name*/
	sizeof(CirclePy),		                              /*tp_basicsize*/
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
  "About Part.Circle",                              /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  CirclePy::Methods,                                /*tp_methods */
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
PyMethodDef CirclePy::Methods[] = {
  PYMETHODEDEF(radius)
  PYMETHODEDEF(setRadius)
  PYMETHODEDEF(position)
  PYMETHODEDEF(setPosition)
  PYMETHODEDEF(axis)
  PYMETHODEDEF(setAxis)
	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject CirclePy::Parents[] = {&Base::PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------

PyObject* CirclePy::PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds)	// Python wrapper
{
  return new CirclePy();
}

int CirclePy::PyInit(PyObject* self, PyObject* args, PyObject*)
{
  PyObject *pcObj=0;
  if (!PyArg_ParseTuple(args, "|O!", &(CirclePy::Type), &pcObj))     // convert args: Python->C 
    return -1;                             // NULL triggers exception 

  if ( pcObj )
  {
    CirclePy* pcCircle = (CirclePy*)pcObj;
    ((CirclePy*)self)->_circle = pcCircle->_circle;
  }

  return 0;
}

CirclePy::CirclePy(PyTypeObject *T)
  : PyObjectBase(0,T)
{
}

CirclePy::CirclePy(const gp_Circ &rcCircle, PyTypeObject *T)
  : PyObjectBase(0,T), _circle(rcCircle)
{
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
CirclePy::~CirclePy()						// Everything handled in parent
{
}

//--------------------------------------------------------------------------
// VectorPy representation
//--------------------------------------------------------------------------
PyObject *CirclePy::_repr(void)
{
  gp_Ax1 axis = _circle.Axis();
  gp_Dir dir = axis.Direction();
  gp_Pnt loc = axis.Location();
  Standard_Real fRad = _circle.Radius();
  std::stringstream a;
  a << "Part.Circle (";
  a << "Radius : " << fRad << ", "; 
  a << "Position : (" << loc.X() << ", "<< loc.Y() << ", "<< loc.Z() << "), "; 
  a << "Direction : (" << dir.X() << ", "<< dir.Y() << ", "<< dir.Z() << "), "; 
  a << ")";
	return Py_BuildValue("s", a.str().c_str());
}

//--------------------------------------------------------------------------
// LinePy Attributes
//--------------------------------------------------------------------------
PyObject *CirclePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
  _getattr_up(PyObjectBase);
} 

int CirclePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  return PyObjectBase::_setattr(attr, value); 						
}

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(CirclePy,radius)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         
  return Py_BuildValue("d",_circle.Radius()); 
}

PYFUNCIMP_D(CirclePy,setRadius)
{
  double Float;
  if (! PyArg_ParseTuple(args, "d",&Float))			 
    return NULL;
  _circle.SetRadius(Float);
  Py_Return; 
}

PYFUNCIMP_D(CirclePy,position)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;
  gp_Pnt loc = _circle.Location();
  App::VectorPy* vec = new App::VectorPy(Base::Vector3f((float)loc.X(), (float)loc.Y(), (float)loc.Z()));
  return vec; 
}

PYFUNCIMP_D(CirclePy,setPosition)
{
  PyObject *pyObject;
  if ( PyArg_ParseTuple(args, "O", &pyObject) ) {
    if ( PyObject_TypeCheck(pyObject, &(App::VectorPy::Type)) ) {
      App::VectorPy *pcVector = (App::VectorPy*)pyObject;
      Base::Vector3f v = pcVector->value();
      _circle.SetLocation(gp_Pnt(v.x, v.y, v.z));
    } else {
      return NULL;
    }
  } else {
    return NULL;
  }

  Py_Return; 
}

PYFUNCIMP_D(CirclePy,axis)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;
  gp_Ax1 axis = _circle.Axis();
  gp_Dir dir = axis.Direction();
  App::VectorPy* vec = new App::VectorPy(Base::Vector3f((float)dir.X(), (float)dir.Y(), (float)dir.Z()));
  return vec; 
}

PYFUNCIMP_D(CirclePy,setAxis)
{
  PyObject *pyObject;
  if ( PyArg_ParseTuple(args, "O", &pyObject) ) {
    if ( PyObject_TypeCheck(pyObject, &(App::VectorPy::Type)) ) {
      App::VectorPy *pcVector = (App::VectorPy*)pyObject;
      Base::Vector3f v = pcVector->value();
      gp_Ax1 axis;
      axis.SetLocation(_circle.Location());
      axis.SetDirection(gp_Dir(v.x, v.y, v.z));
      gp_Dir dir = axis.Direction();
      _circle.SetAxis(axis);
    } else {
      return NULL;
    }
  } else {
    return NULL;
  }

  Py_Return; 
}
