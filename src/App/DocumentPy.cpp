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


#include "Document.h"
#include "DocumentPy.h"
#include "Feature.h"
#include "Label.h"

#include "../Base/PyExport.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
#include "Application.h"

using Base::Console;
using Base::streq;
using namespace App;





//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject DocumentPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"DocumentPy",				/*tp_name*/
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
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef DocumentPy::Methods[] = {
//  {"DocType",      (PyCFunction) sPyDocType,         Py_NEWARGS},
  PYMETHODEDEF(Undo)
  PYMETHODEDEF(ClearUndos)
  PYMETHODEDEF(SaveAs)
  PYMETHODEDEF(Save)
  PYMETHODEDEF(SetModified)
  PYMETHODEDEF(PurgeModified)
  PYMETHODEDEF(NewCommand)
  PYMETHODEDEF(OpenCommand)
  PYMETHODEDEF(CommitCommand)
  PYMETHODEDEF(Recompute)
  PYMETHODEDEF(Dump)
	PYMETHODEDEF(AddFeature)
	PYMETHODEDEF(GetActiveFeature)
	PYMETHODEDEF(Update)

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
	PY_TRY{
		if (streq(attr, "UndoLimit"))						
			return Py_BuildValue("i", _pcDoc->GetUndoLimit()); 
		else if (streq(attr, "AvailableUndos"))				
			return Py_BuildValue("i", _pcDoc->GetAvailableUndos()); 
		else if (streq(attr, "AvailableRedos"))				
			return Py_BuildValue("i", _pcDoc->GetAvailableRedos()); 
		else if (streq(attr, "Name"))						
			return Py_BuildValue("u", _pcDoc->GetName()); 
		else if (streq(attr, "Path"))						
			return Py_BuildValue("u", _pcDoc->GetPath()); 
		else if (streq(attr, "Main")){
			//_pcDoc->Main()->IncRef();
			return new LabelPy(_pcDoc->_hDoc->Main()); 
		}
		else if (streq(attr, "IsEmpty"))					
			return Py_BuildValue("u", _pcDoc->IsEmpty()?1:0); 
		else if (streq(attr, "IsValid"))					
			return Py_BuildValue("u", _pcDoc->IsValid()?1:0); 
		else if (streq(attr, "HasOpenCommand"))				
			return Py_BuildValue("u", _pcDoc->HasOpenCommand()?1:0);
		else if (streq(attr, "StorageFormat"))						
			return Py_BuildValue("u", _pcDoc->StorageFormat()); 
		else
			_getattr_up(PyObjectBase); 						
	}PY_CATCH;
} 

int DocumentPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "UndoLimit")){						// settable new state
		_pcDoc->SetUndoLimit(PyInt_AsLong(value)); 
		return 1;
	}else if (streq(attr, "StorageFormat")){						// settable new state
		_pcDoc->ChangeStorageFormat(const_cast<const short*>((short*)PyUnicode_AS_UNICODE(value))); 
		return 1;
	}else  
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
PYFUNCIMP_D(DocumentPy,Dump)
{ 
  PY_TRY {
	  _pcDoc->Dump();
	  Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,Undo)
{ 
  PY_TRY {
	  _pcDoc->Undo(); 
	  Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,Redo)
{ 
  PY_TRY {
	  _pcDoc->Redo(); 
	  Py_Return; 
  }PY_CATCH;
} 
PYFUNCIMP_D(DocumentPy,ClearUndos)
{ 
  PY_TRY {
	  _pcDoc->ClearUndos(); 
	  Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,SaveAs)
{
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
  PY_TRY {
		_pcDoc->SaveAs(pstr);
  	Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,Save)
{ 
  PY_TRY {
	  _pcDoc->Save(); 
	  Py_Return; 
  }PY_CATCH;
} 


PYFUNCIMP_D(DocumentPy,SetModified)
{ 
  PY_TRY {
	  assert(0);
	  _pcDoc->Save(); 
	  Py_Return; 
  }PY_CATCH;
} 
	
PYFUNCIMP_D(DocumentPy,PurgeModified)
{ 
  PY_TRY {
	  _pcDoc->PurgeModified(); 
	  Py_Return; 
  }PY_CATCH;
} 
	
PYFUNCIMP_D(DocumentPy,NewCommand)
{ 
  PY_TRY {
	  _pcDoc->NewCommand(); 
	  Py_Return; 
  }PY_CATCH;
} 
		
PYFUNCIMP_D(DocumentPy,OpenCommand)
{ 
  PY_TRY {
	  _pcDoc->OpenCommand(); 
	  Py_Return; 
  }PY_CATCH;
} 
	
PYFUNCIMP_D(DocumentPy,CommitCommand)
{ 
  PY_TRY {
	  _pcDoc->CommitCommand(); 
	  Py_Return; 
  }PY_CATCH;
} 
	
PYFUNCIMP_D(DocumentPy,AbortCommand)
{ 
  PY_TRY {
	  _pcDoc->AbortCommand(); 
	  Py_Return; 
  }PY_CATCH;
} 
	
PYFUNCIMP_D(DocumentPy,Recompute)
{ 
  PY_TRY {
	  _pcDoc->Recompute(); 
	  Py_Return; 
  }PY_CATCH;
} 
		
PYFUNCIMP_D(DocumentPy,AddFeature)
{
	char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  PY_TRY {
	  Feature *pcFtr = _pcDoc->AddFeature(pstr);
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
		  Py_Error(PyExc_Exception,"No Feature with this name!");
  }PY_CATCH;
}


PYFUNCIMP_D(DocumentPy,GetActiveFeature)
{
	
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  PY_TRY {
	  Feature *pcFtr = _pcDoc->GetActiveFeature();
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
		  Py_Error(PyExc_Exception,"No active Feature");
  } PY_CATCH;
}


PYFUNCIMP_D(DocumentPy,Update)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

  PY_TRY{
	  _pcDoc->Recompute();   
    Py_Return;
  }PY_CATCH;

}


