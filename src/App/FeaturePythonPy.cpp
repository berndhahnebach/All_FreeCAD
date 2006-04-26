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

#include "Document.h"
#include "Feature.h"
#include "Property.h"
#include "FeaturePythonPy.h"
#include "FeaturePython.h"

using namespace App;




//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject App::FeaturePythonPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FeaturePython",				/*tp_name*/
	sizeof(FeaturePythonPy),			/*tp_basicsize*/
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
  "About FeaturePython",                          /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  0,                                                /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &App::DocumentObjectPy::Type,                     /*tp_base */
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
PyMethodDef App::FeaturePythonPy::Methods[] = {
// PyObjectBase
  PYMETHODEDEF(isA)
// DocumentObjectPy
// FeaturePy 
	PYMETHODEDEF(setModified)
	PYMETHODEDEF(setModifiedView)
	PYMETHODEDEF(isValid)
// FeaturePythonPy 
	PYMETHODEDEF(addProperty)
	PYMETHODEDEF(setClass)
	PYMETHODEDEF(execute)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::FeaturePythonPy::Parents[] = { &FeaturePythonPy::Type,&FeaturePy::Type, &DocumentObjectPy::Type, &PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
FeaturePythonPy::FeaturePythonPy(FeaturePython *pcFeature, PyTypeObject *T)
: FeaturePy(pcFeature, T), executeCallback(0)
{
//	Base::Console().Log("Create FeaturePythonPy: %p \n",this);
}

PyObject *FeaturePythonPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FeaturePythonPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
FeaturePythonPy::~FeaturePythonPy()						// Everything handled in parent
{
//	Base::Console().Log("Destroy FeaturePythonPy: %p \n",this);
  Py_XDECREF(executeCallback);  /* Dispose of callback */

}

//--------------------------------------------------------------------------
// FeaturePythonPy representation
//--------------------------------------------------------------------------
PyObject *FeaturePythonPy::_repr(void)
{
  std::stringstream a;
  a << "FeaturePython: [ ";
//  for(std::map<std::string,int>::const_iterator It = _pcFeature->_PropertiesMap.begin();It!=_pcFeature->_PropertiesMap.end();It++)
//  {
//    a << It->first << "=" << _pcFeature->GetProperty(It->first.c_str()).GetAsString() << "; ";
//  }
  a << "]" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// FeaturePythonPy Attributes
//--------------------------------------------------------------------------
PyObject *FeaturePythonPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
  PY_TRY{
	  if (Base::streq(attr, "__dict__")){
      PyObject* dict = FeaturePy::_getattr(attr);
      if (dict){
        const std::map<std::string,Property*>& Map = reinterpret_cast<FeaturePython*>(_pcFeature)->objectProperties;
        for ( std::map<std::string,App::Property*>::const_iterator it = Map.begin(); it != Map.end(); ++it )
          PyDict_SetItem(dict, PyString_FromString(it->first.c_str()), PyString_FromString(""));
      }
      return dict;
    }
     // search in object PropertyList
    std::map<std::string,Property*>::const_iterator pos = reinterpret_cast<FeaturePython*>(_pcFeature)->objectProperties.find(attr);

    if (pos == reinterpret_cast<FeaturePython*>(_pcFeature)->objectProperties.end())
    {
      _getattr_up(FeaturePy); 						
    }else
    {
      Property *prop = reinterpret_cast<FeaturePython*>(_pcFeature)->objectProperties[attr];
      return prop->getPyObject();
    }

    
    // search in PropertyList
/*    Property *prop = _pcFeature->getPropertyByName(attr);
    if(prop)
      return prop->getPyObject();
    else
      _getattr_up(FeaturePy); */
    
	}PY_CATCH;

  return Py_None;
} 

int FeaturePythonPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{
  if (Base::streq(attr, "execute")){
    if ( value == NULL) {
      Py_XDECREF(executeCallback);  /* Dispose of callback */
      executeCallback = value;
    } else {
      // Here we can check whether 'value' is a method or function but we cannot check the argument list
      // This will do Python for us in the execute method (and throws an exception if not empty).
      if (!PyCallable_Check(value)){
        PyErr_SetString(PyExc_TypeError, "Value must be callable");
        return -1;
      }

      Py_XINCREF(value);         /* Add a reference to new callback */
      Py_XDECREF(executeCallback);  /* Dispose of callback */
      executeCallback = value;
    }
    return 0;
  }

  // search in object PropertyList
  std::map<std::string,Property*>::const_iterator pos = reinterpret_cast<FeaturePython*>(_pcFeature)->objectProperties.find(attr);

  if (pos == reinterpret_cast<FeaturePython*>(_pcFeature)->objectProperties.end())
    return FeaturePy::_setattr(attr, value); 						
  else
  {
    Property *prop = reinterpret_cast<FeaturePython*>(_pcFeature)->objectProperties[attr];
    prop->setPyObject(value);
    return 0;
  }
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(FeaturePythonPy,addProperty)
{
  char *sType,*sName=0;
  if (!PyArg_ParseTuple(args, "s|s", &sType,&sName))     // convert args: Python->C
    return NULL;                             // NULL triggers exception 
 
 
  PY_TRY {
	  reinterpret_cast<FeaturePython*>(_pcFeature)->addDynamicProperty(sType,sName);
  }PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(FeaturePythonPy,setClass)
{
  //char *sType,*sName=0;
  if (!PyArg_ParseTuple(args, ""))    // convert args: Python->C
    return NULL;                             // NULL triggers exception 
 
 
  PY_TRY {
	  //reinterpret_cast<FeaturePython*>(_pcFeature)->addDynamicProperty(sType,sName);
  }PY_CATCH;

  Py_Return;
}

/** Invokes the registered callback function.
 * To register a callback function in Python do it as follows:
 * \code
 *  # Create a document and add a FeaturePython object
 *  d=FreeCAD.newDocument()
 *  f=d.addObject("App::FeaturePython")
 *
 *  def myCallback():
 *      FreeCAD.PrintMessage("Hello, World!")
 *
 *  # Save the callback function
 *  f.execute = myCallback
 *
 *  # Performing a recomputation of the document invokes the callback function
 *  d.recompute()
 * \endcode
 *
 * \note You must not pass any parameters to the callback function, it's argument list must be empty.
 */
PYFUNCIMP_D(FeaturePythonPy,execute)
{
  if ( executeCallback )
  {
    PyObject *result;
    /* Time to call the callback */
    result = PyEval_CallObject(executeCallback, NULL);
    if ( result == NULL )
      return NULL; /* Pass error back */
    Py_DECREF(result);  /* Dispose of result */
    Py_Return;
  }
  else
  {
    PyErr_SetString(PyExc_NotImplementedError , "FeaturePython.execute not implemented");
    return NULL;
  }
}
