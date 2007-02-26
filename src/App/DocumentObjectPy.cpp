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

#include "../Base/PyObjectBase.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
using Base::Console;

#include "Document.h"
#include "DocumentObject.h"
#include "Property.h"
#include "DocumentObjectPy.h"
#define new DEBUG_CLIENTBLOCK
using namespace App;




//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject App::DocumentObjectPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"DocumentObject",				/*tp_name*/
	sizeof(DocumentObjectPy),			/*tp_basicsize*/
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
  "About DocumentObject",                           /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  App::DocumentObjectPy::Methods,                   /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &App::PropertyContainerPy::Type,                 /*tp_base */
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
PyMethodDef App::DocumentObjectPy::Methods[] = {
// PyObjectBase
//  PYMETHODEDEF(isA)
// DocumentObjectPy 
//	PYMETHODEDEF(setModified)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::DocumentObjectPy::Parents[] = { &DocumentObjectPy    ::Type, 
                                                    &PropertyContainerPy ::Type, 
                                                    &PersistancePy       ::Type, 
                                                    &BaseClassPy         ::Type, 
                                                    &PyObjectBase        ::Type, 
                                                    NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
App::DocumentObjectPy::DocumentObjectPy(DocumentObject *pcDocumentObject, PyTypeObject *T)
: PropertyContainerPy(pcDocumentObject, T)
{
//	Base::Console().Log("Create DocumentObjectPy: %p \n",this);
}

PyObject *DocumentObjectPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new DocumentObjectPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
DocumentObjectPy::~DocumentObjectPy()						// Everything handled in parent
{
//	Base::Console().Log("Destroy DocumentObjectPy: %p \n",this);

}

//--------------------------------------------------------------------------
// DocumentObjectPy representation
//--------------------------------------------------------------------------
PyObject *DocumentObjectPy::_repr(void)
{
  std::stringstream a;
  a << "Feature: [ ";
//  for(std::map<std::string,int>::const_iterator It = _pcFeature->_PropertiesMap.begin();It!=_pcFeature->_PropertiesMap.end();It++)
//  {
//    a << It->first << "=" << _pcFeature->GetProperty(It->first.c_str()).GetAsString() << "; ";
//  }
  a << "]" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// DocumentObjectPy Attributes
//--------------------------------------------------------------------------
PyObject *DocumentObjectPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
  try{
    if (Base::streq(attr, "Document"))
      return getDocumentObject()->getPyObject(); 
    else
        _getattr_up(PropertyContainerPy);
  }catch(...){
    Py_Error(PyExc_AttributeError,"Error in get Attribute");
  }
 
} 

int DocumentObjectPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{
  if (Base::streq(attr, "Document"))           
    return 1;
  else 
    return PropertyContainerPy::_setattr(attr, value);
} 

DocumentObject *DocumentObjectPy::getDocumentObject(void) const
{
  return dynamic_cast<DocumentObject *>(_pcBaseClass);
}


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

/*
PYFUNCIMP_D(DocumentObjectPy,setModified)
{
  _pcFeature->Touch();

	Py_Return;
}
*/
