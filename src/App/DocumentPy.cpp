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

#include "../Base/PyExport.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
#include "Application.h"
#define new DEBUG_CLIENTBLOCK
using Base::Console;
using Base::streq;
using namespace App;





//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject DocumentPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"Document",				/*tp_name*/
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
//  PYMETHODEDEF(SaveAs)
  PYMETHODEDEF(save)
//  PYMETHODEDEF(SetModified)
//  PYMETHODEDEF(PurgeModified)


  PYMETHODEDEF(setUndoMode)
  PYMETHODEDEF(openTransaction)
  PYMETHODEDEF(commitTransaction)
  PYMETHODEDEF(abortTransaction)
  PYMETHODEDEF(undo)
  PYMETHODEDEF(redo)
  PYMETHODEDEF(clearUndos)
  PYMETHODEDEF(getUndoMemSize)	
  PYMETHODEDEF(getAvailableUndoNames)
	PYMETHODEDEF(getAvailableRedoNames)
	PYMETHODEDEF(getAvailableUndos)
	PYMETHODEDEF(getAvailableRedos)


  PYMETHODEDEF(recompute)
  PYMETHODEDEF(update)
//  PYMETHODEDEF(Dump)
  PYMETHODEDEF(getActiveObject)
  PYMETHODEDEF(getObject)
  PYMETHODEDEF(addObject)
  PYMETHODEDEF(activeObject)
  PYMETHODEDEF(getObject)
  PYMETHODEDEF(removeObject)
  PYMETHODEDEF(listObjects)
  PYMETHODEDEF(getName)
  PYMETHODEDEF(getMemSize)

  //PYMETHODEDEF(beginTransaction)
  //PYMETHODEDEF(rollbackTransaction)
  //PYMETHODEDEF(endTransaction)
  PYMETHODEDEF(setTransactionMode)

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
    if (Base::streq(attr, "__dict__")) {
      PyObject *dict = PyDict_New();
      if (dict) {
//        PyDict_SetItemString(dict,"UndoLimit",      Py_BuildValue("i",_pcDoc->GetUndoLimit()));
        PyDict_SetItemString(dict,"AvailableUndos", Py_BuildValue("i",_pcDoc->getAvailableUndos()));
        PyDict_SetItemString(dict,"AvailableRedos", Py_BuildValue("i",_pcDoc->getAvailableRedos()));
//        PyDict_SetItemString(dict,"HasOpenCommand", Py_BuildValue("i",_pcDoc->HasOpenCommand()?1:0));
        PyDict_SetItemString(dict,"Name",           Py_BuildValue("s",_pcDoc->getName()));
        PyDict_SetItemString(dict,"Path",           Py_BuildValue("s",_pcDoc->getPath()));
        if (PyErr_Occurred()) { Py_DECREF(dict);dict = NULL;}
      }
      return dict;
    }
//		else if (streq(attr, "UndoLimit"))
//			return Py_BuildValue("i", _pcDoc->GetUndoLimit());
		else if (streq(attr, "AvailableUndos"))
			return Py_BuildValue("i", _pcDoc->getAvailableUndos()); 
		else if (streq(attr, "AvailableRedos"))
			return Py_BuildValue("i", _pcDoc->getAvailableRedos()); 
		else if (streq(attr, "Name"))
			return Py_BuildValue("s", _pcDoc->getName());
		else if (streq(attr, "Path"))
			return Py_BuildValue("s", _pcDoc->getPath());
//		else if (streq(attr, "Main")){
//			//_pcDoc->Main()->IncRef();
//			return new LabelPy(_pcDoc->_hDoc->Main());
//		}
//		else if (streq(attr, "IsEmpty"))
//			return Py_BuildValue("u", _pcDoc->IsEmpty()?1:0);
//		else if (streq(attr, "IsValid"))
//			return Py_BuildValue("u", _pcDoc->IsValid()?1:0);
//		else if (streq(attr, "HasOpenCommand"))
//			return Py_BuildValue("i", _pcDoc->HasOpenCommand()?1:0);
//		else if (streq(attr, "StorageFormat"))						
//			return Py_BuildValue("u", _pcDoc->storageFormat()); 
    else{
      DocumentObject *pObject = _pcDoc->getObject(attr);
      if(pObject)
        return pObject->GetPyObject();
      else
			 _getattr_up(PyObjectBase); 						
    }
} 

int DocumentPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
//	if (streq(attr, "UndoLimit")){						// settable new state
//		_pcDoc->SetUndoLimit(PyInt_AsLong(value)); 
//		return 1;
//	}else if (streq(attr, "StorageFormat")){						// settable new state
//		_pcDoc->changeStorageFormat(const_cast<const short*>((short*)PyUnicode_AS_UNICODE(value))); 
//		return 1;
//	}else  
		return PyObjectBase::_setattr(attr, value); 	// send up to parent
} 



//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(DocumentPy,setUndoMode)
{ 
	long  Int;
  if (!PyArg_ParseTuple(args, "i", &Int))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  PY_TRY {
	  _pcDoc->setUndoMode(Int); 
	  Py_Return; 
  }PY_CATCH;
} 


PYFUNCIMP_D(DocumentPy,undo)
{ 
  PY_TRY {
	  _pcDoc->undo(); 
	  Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,redo)
{ 
  PY_TRY {
	  _pcDoc->redo(); 
	  Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,clearUndos)
{ 
  PY_TRY {
	  _pcDoc->clearUndos(); 
	  Py_Return; 
  }PY_CATCH;
} 
PYFUNCIMP_D(DocumentPy,openTransaction)
{ 
  char *pstr=0;
  if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
      return NULL;                             // NULL triggers exception 

  PY_TRY {
	  _pcDoc->openTransaction(pstr); 
	  Py_Return; 
  }PY_CATCH;
} 
	
PYFUNCIMP_D(DocumentPy,commitTransaction)
{ 
  PY_TRY {
	  _pcDoc->commitTransaction(); 
	  Py_Return; 
  }PY_CATCH;
} 
	
PYFUNCIMP_D(DocumentPy,abortTransaction)
{ 
  PY_TRY {
	  _pcDoc->abortTransaction(); 
	  Py_Return; 
  }PY_CATCH;
} 


PYFUNCIMP_D(DocumentPy,getAvailableUndoNames)
{
  if (!PyArg_ParseTuple(args, ""))  // convert args: Python->C 
     return NULL;                   // NULL triggers exception 

  PY_TRY {
    std::vector<std::string> vList = _pcDoc->getAvailableUndoNames();
	  PyObject* pList = PyList_New(vList.size());  
    unsigned int i =0;
    for (std::vector<std::string>::const_iterator It = vList.begin();It!=vList.end();++It,i++)
      PyList_SetItem(pList, i, PyString_FromString(It->c_str()));

	  return pList;

  }PY_CATCH;
}

PYFUNCIMP_D(DocumentPy,getAvailableRedoNames)
{
 if (!PyArg_ParseTuple(args, ""))  // convert args: Python->C 
     return NULL;                   // NULL triggers exception 

  PY_TRY {
    std::vector<std::string> vList = _pcDoc->getAvailableRedoNames();
	  PyObject* pList = PyList_New(vList.size());  
    unsigned int i =0;
    for (std::vector<std::string>::const_iterator It = vList.begin();It!=vList.end();++It,i++)
      PyList_SetItem(pList, i, PyString_FromString(It->c_str()));

	  return pList;

  }PY_CATCH;

}

PYFUNCIMP_D(DocumentPy,getAvailableUndos)
{ 
  PY_TRY {
	   return Py_BuildValue("i",_pcDoc->getAvailableUndos()); 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,getAvailableRedos)
{ 
  PY_TRY {
	   return Py_BuildValue("i",_pcDoc->getAvailableRedos()); 
  }PY_CATCH;
} 

#if 0

PYFUNCIMP_D(DocumentPy,beginTransaction)
{ 
  PY_TRY {
    return Py_BuildValue("i",_pcDoc->beginTransaction());
  }PY_CATCH;
} 


PYFUNCIMP_D(DocumentPy,rollbackTransaction)
{ 
  PY_TRY {
	  _pcDoc->rollbackTransaction(); 
	  Py_Return; 
  }PY_CATCH;
} 


PYFUNCIMP_D(DocumentPy,endTransaction)
{ 
  PY_TRY {
	   return Py_BuildValue("i",_pcDoc->endTransaction()); 
  }PY_CATCH;
} 

#endif


PYFUNCIMP_D(DocumentPy,setTransactionMode)
{ 
	long  Int;
  if (!PyArg_ParseTuple(args, "i", &Int))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  PY_TRY {
	  _pcDoc->setTransactionMode(Int); 
	  Py_Return; 
  }PY_CATCH;
} 





/*
PyObject *DocumentPy::PyDocType(PyObject *args)
{ 
	return _pcDoc->GetDocType()->GetPyObject();
}
PYFUNCIMP_D(DocumentPy,Dump)
{ 
  PY_TRY {
	  //_pcDoc->Dump();
	  Py_Return; 
  }PY_CATCH;
} 


PYFUNCIMP_D(DocumentPy,SaveAs)
{
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
  PY_TRY {
		_pcDoc->saveAs(pstr);
  	Py_Return; 
  }PY_CATCH;
} 
 */

PYFUNCIMP_D(DocumentPy,save)
{ 
  PY_TRY {
	  _pcDoc->save(); 
	  Py_Return; 
  }PY_CATCH;
} 

/*
PYFUNCIMP_D(DocumentPy,SetModified)
{ 
  PY_TRY {
	  assert(0);
	  _pcDoc->SetModified(); 
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
		
*/	
PYFUNCIMP_D(DocumentPy,recompute)
{ 
  PY_TRY {
	  _pcDoc->recompute(); 
	  Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,update)
{ 
	char *sName;
  if (!PyArg_ParseTuple(args, "s",&sName))     // convert args: Python->C
    return NULL;                             // NULL triggers exception 

  PY_TRY {
	  _pcDoc->update(sName); 
	  Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,addObject)
{
	char *sType,*sName=0;
  if (!PyArg_ParseTuple(args, "s|s", &sType,&sName))     // convert args: Python->C
    return NULL;                             // NULL triggers exception 
 
  DocumentObject *pcFtr;
  
  PY_TRY {
	  pcFtr = _pcDoc->addObject(sType,sName);
  }PY_CATCH;
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
    {
      char szBuf[200];
      snprintf(szBuf, 200, "No document object found of type '%s'", sType);
		  Py_Error(PyExc_Exception,szBuf);
    }
}


PYFUNCIMP_D(DocumentPy,getActiveObject)
{
	
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  PY_TRY {
	  DocumentObject *pcFtr = _pcDoc->getActiveObject();
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
		  Py_Error(PyExc_Exception,"No active Object");
  } PY_CATCH;
}

PYFUNCIMP_D(DocumentPy,activeObject)
{
	
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  PY_TRY {
	  DocumentObject *pcFtr = _pcDoc->getActiveObject();
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
		  Py_Error(PyExc_Exception,"No active Object");
  } PY_CATCH;
}

PYFUNCIMP_D(DocumentPy,getObject)
{
	
	char *sName;
  if (!PyArg_ParseTuple(args, "s",&sName))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  PY_TRY {
	  DocumentObject *pcFtr = _pcDoc->getObject(sName);
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
    {
      Py_Return;

      //char szBuf[200];
      //sprintf(szBuf, "No feature found with name '%s'", sName);
		  //Py_Error(PyExc_Exception,szBuf);
    }
  } PY_CATCH;
}


PYFUNCIMP_D(DocumentPy,removeObject)
{
  char *sName;
  if (!PyArg_ParseTuple(args, "s",&sName))     // convert args: Python->C
    return NULL;                             // NULL triggers exception

  PY_TRY {
    DocumentObject *pcFtr = _pcDoc->getObject(sName);
    if(pcFtr) {
      _pcDoc->remObject( sName );
      Py_Return;
    }
    else {
      char szBuf[200];
      snprintf(szBuf, 200, "No feature found with name '%s'", sName);
      Py_Error(PyExc_Exception,szBuf);
    }
  } PY_CATCH;
}

PYFUNCIMP_D(DocumentPy,listObjects)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  PY_TRY {
    std::map<std::string,DocumentObject*> features = _pcDoc->ObjectMap;
    PyObject *pDict = PyDict_New();
    PyObject *pKey; Base::PyObjectBase* pValue;
    
    for (std::map<std::string,DocumentObject*>::const_iterator It = features.begin();It != features.end();++It)
    {
      pKey   = PyString_FromString(It->first.c_str());
      // GetPyObject() increments
      pValue = It->second->GetPyObject();
      PyDict_SetItem(pDict, pKey, pValue); 
      // now we can decrement again as PyDict_SetItem also has incremented
      pValue->DecRef();
    }

    return pDict;
  } PY_CATCH;
}

PYFUNCIMP_D(DocumentPy,getName)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 
  PY_TRY {
    return PyString_FromString(_pcDoc->getName());
  } PY_CATCH;
}

PYFUNCIMP_D(DocumentPy,getUndoMemSize)
{ 
  PY_TRY {
	   return Py_BuildValue("i",_pcDoc->getUndoMemSize()); 
  }PY_CATCH;
} 

PYFUNCIMP_D(DocumentPy,getMemSize)
{ 
  PY_TRY {
	   return Py_BuildValue("i",_pcDoc->getMemSize()); 
  }PY_CATCH;
} 
