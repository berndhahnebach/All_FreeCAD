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

#include "DocumentObjectGroupPy.h"
#include "DocumentObjectGroup.h"
#define new DEBUG_CLIENTBLOCK
using namespace App;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject App::DocumentObjectGroupPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"App.DocumentObjectGroup",				/*tp_name*/
	sizeof(DocumentObjectGroupPy),			/*tp_basicsize*/
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
  "About DocumentObjectGroup",                      /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  App::DocumentObjectGroupPy::Methods,              /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &App::FeaturePy::Type,                            /*tp_base */
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
PyMethodDef App::DocumentObjectGroupPy::Methods[] = {
	PYMETHODEDEF(addObject)
	PYMETHODEDEF(removeObject)
	PYMETHODEDEF(getObject)
	PYMETHODEDEF(hasObject)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::DocumentObjectGroupPy::Parents[] = {  &DocumentObjectGroupPy::Type,
                                                          &DocumentObjectPy    ::Type, 
                                                          &PropertyContainerPy ::Type, 
                                                          &PersistancePy       ::Type, 
                                                          &BaseClassPy         ::Type, 
                                                          &PyObjectBase        ::Type, 
                                                          NULL};
//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
App::DocumentObjectGroupPy::DocumentObjectGroupPy(DocumentObjectGroup *pcGroup, PyTypeObject *T)
  : DocumentObjectPy(pcGroup, T)
{
}

PyObject *DocumentObjectGroupPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
DocumentObjectGroupPy::~DocumentObjectGroupPy()						// Everything handled in parent
{
}

//--------------------------------------------------------------------------
// DocumentObjectGroupPy representation
//--------------------------------------------------------------------------
PyObject *DocumentObjectGroupPy::_repr(void)
{
  std::stringstream a;
  a << getDocumentObjectGroup()->getTypeId().getName() << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}

//--------------------------------------------------------------------------
// DocumentObjectGroupPy Attributes
//--------------------------------------------------------------------------
PyObject *DocumentObjectGroupPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{ 
  try{
    if (Base::streq(attr, "XXXX"))
      return Py_BuildValue("i",1); 
    else
        _getattr_up(DocumentObjectPy);
  }catch(...){
    Py_Error(PyExc_AttributeError,"Error in get Attribute");
  }
} 

int DocumentObjectGroupPy::_setattr(char *attr, PyObject *value) // __setattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "XXXX"))           // settable new state
    return 1;
  else 
    return DocumentObjectPy::_setattr(attr, value); 
} 

DocumentObjectGroup *DocumentObjectGroupPy::getDocumentObjectGroup(void) const 
{
  return dynamic_cast<DocumentObjectGroup *>(_pcBaseClass);
}
 
//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(DocumentObjectGroupPy,addObject)
{
	char *sType,*sName=0;
  if (!PyArg_ParseTuple(args, "s|s", &sType,&sName))     // convert args: Python->C
    return NULL;

  DocumentObject *pcObj = getDocumentObjectGroup()->addObject(sType, sName);
  if ( pcObj ) {
    return pcObj->getPyObject();
  } else {
    char szBuf[200];
    snprintf(szBuf, 200, "Cannot create object of type '%s'", sType);
		Py_Error(PyExc_Exception,szBuf);
  }
}

PYFUNCIMP_D(DocumentObjectGroupPy,removeObject)
{
  char* pcName;
  if (!PyArg_ParseTuple(args, "s", &pcName)) 
    return false;

  getDocumentObjectGroup()->removeObject(pcName);

  Py_Return;
}

PYFUNCIMP_D(DocumentObjectGroupPy,hasObject)
{
  char* pcName;
  if (!PyArg_ParseTuple(args, "s", &pcName))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  DocumentObject* obj = getDocumentObjectGroup()->getObject(pcName);

  if ( obj ) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

PYFUNCIMP_D(DocumentObjectGroupPy,getObject)
{
  char* pcName;
  if (!PyArg_ParseTuple(args, "s", &pcName))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  DocumentObject* obj = getDocumentObjectGroup()->getObject(pcName);
  if ( obj ) {
    return obj->getPyObject();
  } else {
    Py_Return;
  }
}
