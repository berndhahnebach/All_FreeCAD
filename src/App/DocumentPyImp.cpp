
// 
#include "PreCompiled.h"

#include "DocumentPy.h"
#include "Document.h"
#include "DocumentObject.h"
#include <Base/PyTools.h>
using namespace App;

PyObject*  DocumentPy::save(PyObject *args)
{
  getDocumentObject()->save();
	Py_Return;
}
PyObject*  DocumentPy::restore(PyObject *args)
{
  getDocumentObject()->restore();
	Py_Return;
}
PyObject*  DocumentPy::addObject(PyObject *args)
{
	char *sType,*sName=0;
  if (!PyArg_ParseTuple(args, "s|s", &sType,&sName))     // convert args: Python->C
    return NULL;                                         // NULL triggers exception 
 
  DocumentObject *pcFtr;
  
  pcFtr = getDocumentObject()->addObject(sType,sName);
  
	if(pcFtr)
	  return pcFtr->getPyObject();
	else
  {
    char szBuf[200];
    snprintf(szBuf, 200, "No document object found of type '%s'", sType);
	  Py_Error(PyExc_Exception,szBuf);
  }
}
PyObject*  DocumentPy::removeObject(PyObject *args)
{
  char *sName;
  if (!PyArg_ParseTuple(args, "s",&sName))     // convert args: Python->C
    return NULL;                             // NULL triggers exception


  DocumentObject *pcFtr = getDocumentObject()->getObject(sName);
  if(pcFtr) {
    getDocumentObject()->remObject( sName );
    Py_Return;
  }
  else {
    char szBuf[200];
    snprintf(szBuf, 200, "No feature found with name '%s'", sName);
    Py_Error(PyExc_Exception,szBuf);
  }


}

PyObject*  DocumentPy::openTransaction(PyObject *args)
{
  char *pstr=0;
  if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
      return NULL;                             // NULL triggers exception 

  getDocumentObject()->openTransaction(pstr); 
  Py_Return; 
}
PyObject*  DocumentPy::abortTransaction(PyObject *args)
{
  getDocumentObject()->abortTransaction();
	Py_Return;

}
PyObject*  DocumentPy::commitTransaction(PyObject *args)
{
  getDocumentObject()->commitTransaction();
	Py_Return;

}

PyObject*  DocumentPy::undo(PyObject *args)
{
  getDocumentObject()->undo();
	Py_Return;
}
PyObject*  DocumentPy::redo(PyObject *args)
{
	 getDocumentObject()->redo();
	 Py_Return;
}
PyObject*  DocumentPy::clearUndos(PyObject *args)
{
   getDocumentObject()->clearUndos();
	 Py_Return;
}
PyObject*  DocumentPy::recompute(PyObject *args)
{
	 getDocumentObject()->recompute();
	 Py_Return;

}
PyObject*  DocumentPy::getObject(PyObject *args)
{
	char *sName;
  if (!PyArg_ParseTuple(args, "s",&sName))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

	DocumentObject *pcFtr = getDocumentObject()->getObject(sName);
	if(pcFtr)
	  return pcFtr->getPyObject();
	else
    Py_Return;
}

Py::Object DocumentPy::getActiveObject(void)
{
  DocumentObject *pcFtr = getDocumentObject()->getActiveObject();
  if(pcFtr)
	  return Py::Object(pcFtr->getPyObject());
  else
    throw Py::Exception("No active Object");
}

void  DocumentPy::setActiveObject(Py::Object arg)
{
  arg;
}

Py::List DocumentPy::getObjects(void)
{
  std::map<std::string,DocumentObject*> features = getDocumentObject()->ObjectMap;
  Py::List res;

  for (std::map<std::string,DocumentObject*>::const_iterator It = features.begin();It != features.end();++It)
    res.append(Py::Object(It->second->getPyObject()));

  return res;
}

void  DocumentPy::setObjects(Py::List arg)
{
  arg;
}

Py::Int DocumentPy::getUndoMode(void)
{
  return Py::Int(getDocumentObject()->getUndoMode());
}

void  DocumentPy::setUndoMode(Py::Int arg)
{
   getDocumentObject()->setUndoMode(arg); 
}

Py::Int DocumentPy::getUndoRedoMemSize(void)
{
  return Py::Int((long)getDocumentObject()->getUndoMemSize());
}

void  DocumentPy::setUndoRedoMemSize(Py::Int arg)
{
 
}

Py::Int DocumentPy::getUndoCount(void)
{
  return Py::Int((long)getDocumentObject()->getAvailableUndos());
}

void  DocumentPy::setUndoCount(Py::Int arg)
{
}

Py::Int DocumentPy::getRedoCount(void)
{
  return Py::Int((long)getDocumentObject()->getAvailableRedos());
}

void  DocumentPy::setRedoCount(Py::Int arg)
{
}

Py::List DocumentPy::getUndoNames(void)
{
  std::vector<std::string> vList = getDocumentObject()->getAvailableUndoNames();
  Py::List res;

  for (std::vector<std::string>::const_iterator It = vList.begin();It!=vList.end();++It)
    res.append(Py::String(*It));

  return res;
}

void  DocumentPy::setUndoNames(Py::List arg)
{
  
}

Py::List DocumentPy::getRedoNames(void)
{
  std::vector<std::string> vList = getDocumentObject()->getAvailableRedoNames();
  Py::List res;

  for (std::vector<std::string>::const_iterator It = vList.begin();It!=vList.end();++It)
    res.append(Py::String(*It));

  return res;

}

void  DocumentPy::setRedoNames(Py::List arg)
{
  
}

PyObject *DocumentPy::getCustomAttributes(const char* attr)
{
  DocumentObject *pObject = getDocumentObject()->getObject(attr);
  if(pObject)
      return Py::new_reference_to(Py::Object(pObject->getPyObject()));
  else
    return 0; 						

}

int DocumentPy::setCustomAttributes(const char* attr, PyObject *obj)
{
  return 0;
}

