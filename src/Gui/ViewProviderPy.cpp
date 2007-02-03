/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include <Base/Exception.h>
#include <Base/Console.h>
#include <App/Property.h>
#include <App/PropertyStandard.h>

#include "ViewProviderPy.h"
#include "ViewProvider.h"

using namespace Gui;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject Gui::ViewProviderPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"ViewProvider",				/*tp_name*/
	sizeof(ViewProviderPy),			/*tp_basicsize*/
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
  "About ViewProvider",                             /*tp_doc */
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
  0,                                                /*tp_new */
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
PyMethodDef Gui::ViewProviderPy::Methods[] = {
  PYMETHODEDEF(show)
  PYMETHODEDEF(hide)
  PYMETHODEDEF(isVisible)
  PYMETHODEDEF(update)
  PYMETHODEDEF(listDisplayModes)
	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject Gui::ViewProviderPy::Parents[] = { &ViewProviderPy::Type, &PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
Gui::ViewProviderPy::ViewProviderPy(ViewProvider *pcViewProvider, PyTypeObject *T)
  : PyObjectBase( T), _pcViewProvider(pcViewProvider)
{
}

PyObject *ViewProviderPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
ViewProviderPy::~ViewProviderPy()
{
}

void ViewProviderPy::setInvalid()
{
  PyObjectBase::setInvalid();
}

//--------------------------------------------------------------------------
// ViewProviderPy representation
//--------------------------------------------------------------------------
PyObject *ViewProviderPy::_repr(void)
{
  std::stringstream a;
  a << _pcViewProvider->getTypeId().getName() << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}

//--------------------------------------------------------------------------
// ViewProviderPy Attributes
//--------------------------------------------------------------------------
PyObject *ViewProviderPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
  PY_TRY{
  {
    // search in PropertyList
    App::Property *prop = _pcViewProvider->getPropertyByName(attr);
    if(prop)
    {
      return prop->getPyObject();
    } else if (Base::streq(attr, "__dict__")) {
      // get the properties to the C++ DocumentObject class
      std::map<std::string,App::Property*> Map;
      _pcViewProvider->getPropertyMap(Map);
      PyObject *dict = PyDict_New();
      if (dict) { 
        for ( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
          PyDict_SetItem(dict, PyString_FromString(it->first.c_str()), PyString_FromString(""));
        if (PyErr_Occurred()) { Py_DECREF(dict);dict = NULL;}
      }
      return dict;
    }
    else
      _getattr_up(PyObjectBase); 						
    }
  }PY_CATCH;

  return Py_None;
} 

int ViewProviderPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{
   // search in PropertyList
  App::Property *prop = _pcViewProvider->getPropertyByName(attr);
  if(prop){
    try {
      prop->setPyObject(value);
    } catch (Base::Exception &exc) {
      PyErr_Format(PyExc_AttributeError, "Attribute (Name: %s) error: '%s' ", attr, exc.what());
      return -1;
    } catch (...) {
      PyErr_Format(PyExc_AttributeError, "Unknown error in attribute %s", attr);
      return -1;
    }
  }else{
		return PyObjectBase::_setattr(attr, value); 						
  }

  return 0;
} 

PYFUNCIMP_D(ViewProviderPy,show)
{ 
  PY_TRY {
    _pcViewProvider->show();  
    Py_Return;
  }PY_CATCH;
}

PYFUNCIMP_D(ViewProviderPy,hide)
{ 
  PY_TRY {
    _pcViewProvider->hide();  
    Py_Return;
  }PY_CATCH;
} 

PYFUNCIMP_D(ViewProviderPy,isVisible)
{ 
  PY_TRY {
    return Py_BuildValue("O", (_pcViewProvider->isShow() ? Py_True : Py_False));
  }PY_CATCH;
} 

PYFUNCIMP_D(ViewProviderPy,update)
{ 
  PY_TRY {
    _pcViewProvider->update();  
    Py_Return;
  }PY_CATCH;
} 

PYFUNCIMP_D(ViewProviderPy,listDisplayModes)
{ 
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 
  PY_TRY {
    std::vector<std::string> modes = _pcViewProvider->getDisplayModes();  
    PyObject* pyList = PyList_New(modes.size()); 
    int i=0;

    for ( std::vector<std::string>::iterator it = modes.begin(); it != modes.end(); ++it )
    {
      PyObject* str = PyString_FromString(it->c_str());
      PyList_SetItem(pyList, i++, str);
    }

    return pyList;
  }PY_CATCH;
} 

