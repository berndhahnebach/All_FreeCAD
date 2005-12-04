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
# include <Inventor/SoInput.h>
#endif


#include "Document.h"
#include "DocumentPy.h"

#include <Base/PyExport.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Matrix.h>
#include <App/MatrixPy.h>
#include "Application.h"
#include "ViewProviderExtern.h"

using Base::Console;
using Base::streq;
using namespace Gui;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject DocumentPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"GuiDocumentPy",				/*tp_name*/
	sizeof(DocumentPy),			/*tp_basicsize*/
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
  "About PyObjectBase",                             /*tp_doc */
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
PyMethodDef DocumentPy::Methods[] = {
//  {"DocType",      (PyCFunction) sPyDocType,         Py_NEWARGS},
  PYMETHODEDEF(hide)
  PYMETHODEDEF(show)
  PYMETHODEDEF(setPos)
  PYMETHODEDEF(addAnnotation)
  PYMETHODEDEF(update)

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject DocumentPy::Parents[] = {&PyObjectBase::Type, NULL};     

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
DocumentPy::DocumentPy(Document *pcDoc, PyTypeObject *T)
 : PyObjectBase( T), _pcDoc(pcDoc)
{
  Base::Console().Log("Create DocumentPy: %p \n",this);
}

PyObject *DocumentPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new DocumentPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
DocumentPy::~DocumentPy()						// Everything handled in parent
{
	Base::Console().Log("Destroy DocumentPy: %p \n",this);
} 


//--------------------------------------------------------------------------
// DocumentPy representation
//--------------------------------------------------------------------------
PyObject *DocumentPy::_repr(void)
{
	return Py_BuildValue("s", "FreeCAD Document");
}
//--------------------------------------------------------------------------
// DocumentPy Attributes
//--------------------------------------------------------------------------
PyObject *DocumentPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
			 _getattr_up(PyObjectBase); 						
} 

int DocumentPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
		return PyObjectBase::_setattr(attr, value); 	// send up to parent
} 



//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
/*
PyObject *DocumentPy::PyDocType(PyObject *args)
{ 
	return _pcDoc->GetDocType()->GetPyObject();
}
 */
PYFUNCIMP_D(DocumentPy,addAnnotation)
{ 
  char *psAnnoName,*psFileName,*psModName=0;
  if (!PyArg_ParseTuple(args, "ss|s;Name of the Annotation and a file name have to be given!",&psAnnoName,&psFileName,&psModName))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {

    ViewProviderInventorExtern *pcExt = new ViewProviderInventorExtern();

    pcExt->setModeByFile(psModName?psModName:"Main",psFileName);

    _pcDoc->setAnotationViewProvider(psAnnoName,pcExt);

    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,hide)
{ 
  char *psFeatStr;
  if (!PyArg_ParseTuple(args, "s;Name of the Feature to hide have to be given!",&psFeatStr))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {

    _pcDoc->setHide(psFeatStr);  
    
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,show)
{ 
  char *psFeatStr;
  if (!PyArg_ParseTuple(args, "s;Name of the Feature to show have to be given!",&psFeatStr))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    _pcDoc->setShow(psFeatStr);  
    
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,setPos)
{ 
  char *psFeatStr;
  Matrix4D mat;
  PyObject *pcMatObj;
  if (!PyArg_ParseTuple(args, "sO!;Name of the Feature and the transformation matrix have to be given!",
                        &psFeatStr,
                        &(App::MatrixPy::Type), &pcMatObj))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  mat = ((App::MatrixPy*)pcMatObj)->value();

  PY_TRY {
    _pcDoc->setPos(psFeatStr,mat);  
    
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,update)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  PY_TRY {
    _pcDoc->update();

    Py_Return;

  }PY_CATCH;
} 

