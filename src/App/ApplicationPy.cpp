/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/



#include "PreCompiled.h"

#ifndef _PreComp_
#endif


#include "Application.h"
#include "Document.h"

// FreeCAD Base header
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Parameter.h"
#include "../Base/Console.h"
#include "../Base/Factory.h"

#define new DEBUG_CLIENTBLOCK
//using Base::GetConsole;
using namespace Base;
using namespace App;



//**************************************************************************
// Python stuff

// Application Methods						// Methods structure
PyMethodDef Application::Methods[] = {
  {"New",            (PyCFunction) Application::sNew,            1},
  {"open",           (PyCFunction) Application::sOpen,           1},
//  {"Close",          (PyCFunction) Application::sClose,          1},
  {"Import"  ,       (PyCFunction) Application::sImport,         1},
  {"save",           (PyCFunction) Application::sSave,           1},
  {"saveAs",         (PyCFunction) Application::sSaveAs,         1},
  {"document",       (PyCFunction) Application::sDocument,       1},
  {"ParamGet",       (PyCFunction) Application::sGetParam,       1},
  {"Version",        (PyCFunction) Application::sGetVersion,     1},
  {"ConfigGet",      (PyCFunction) Application::sGetConfig,      1},
  {"ConfigSet",      (PyCFunction) Application::sSetConfig,      1},
  {"ConfigDump",     (PyCFunction) Application::sDumpConfig,     1},
  {"EndingAdd",      (PyCFunction) Application::sEndingAdd,      1},
  {"EndingDelete",   (PyCFunction) Application::sEndingDelete   ,1},
  {"EndingGet",      (PyCFunction) Application::sEndingGet      ,1},

  {"openDocument",   (PyCFunction) Application::sOpenDocument,   1},
  {"saveDocument",   (PyCFunction) Application::sSaveDocument,   1},
  {"saveDocumentAs", (PyCFunction) Application::sSaveDocumentAs, 1},
  {"newDocument",    (PyCFunction) Application::sNewDocument,    1},
  {"closeDocument",  (PyCFunction) Application::sCloseDocument,  1},
  {"activeDocument", (PyCFunction) Application::sActiveDocument, 1},
  {"setActiveDocument",(PyCFunction) Application::sSetActiveDocument, 1},
  {"getDocument",    (PyCFunction) Application::sGetDocument,    1},
  {"listDocuments",  (PyCFunction) Application::sListDocuments  ,1},

  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(Application,sOpen)
{
  char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
    return NULL;                             // NULL triggers exception

  try {
    // return new document
    return (GetApplication().openDocument(pstr)->GetPyObject());
  } catch(Base::Exception e) {
    PyErr_SetString(PyExc_IOError, e.what());
    return 0L;
  }
}

PYFUNCIMP_S(Application,sOpenDocument)
{
  char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
    return NULL;                             // NULL triggers exception
  try {
    // return new document
    return (GetApplication().openDocument(pstr)->GetPyObject());
  } catch(Base::Exception e) {
    PyErr_SetString(PyExc_IOError, e.what());
    return 0L;
  }
}

PYFUNCIMP_S(Application,sImport)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    Py_Return;
}

PYFUNCIMP_S(Application,sNew)
{
    char *pstr = 0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	PY_TRY{
		return GetApplication().newDocument(pstr)->GetPyObject();
	}PY_CATCH;
}

PYFUNCIMP_S(Application,sNewDocument)
{
  char *pstr = 0;
  if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C
    return NULL;                             // NULL triggers exception

  PY_TRY{
    return GetApplication().newDocument(pstr)->GetPyObject();
  }PY_CATCH;
}

PYFUNCIMP_S(Application,sClose)
{
  char *pstr = 0;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
      return NULL;                             // NULL triggers exception 

	if ( GetApplication().closeDocument(pstr) == false )
  {
		PyErr_Format(PyExc_NameError, "Unknown document '%s'", pstr);
		return NULL;
  }

  Py_Return;
}


PYFUNCIMP_S(Application,sSetActiveDocument)
{
  char *pstr = 0;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
      return NULL;                             // NULL triggers exception 

  GetApplication().setActiveDocument(pstr);

  Py_Return;
}

PYFUNCIMP_S(Application,sCloseDocument)
{
  char *pstr = 0;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
      return NULL;                             // NULL triggers exception

  if ( GetApplication().closeDocument(pstr) == false )
  {
    PyErr_Format(PyExc_NameError, "Unknown document '%s'", pstr);
    return NULL;
  }

  Py_Return;
}

PYFUNCIMP_S(Application,sSave)
{
  char *pDoc;
  if (!PyArg_ParseTuple(args, "s", &pDoc))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Document* doc = GetApplication().getDocument(pDoc);
	if ( doc )
  {
    if ( doc->save() == false )
    {
  		PyErr_Format(PyExc_Exception, "Cannot save document '%s'", pDoc);
	  	return 0L;
    }
  }
  else
  {
		PyErr_Format(PyExc_NameError, "Unknown document '%s'", pDoc);
		return NULL;
  }

  Py_Return;
}

PYFUNCIMP_S(Application,sSaveDocument)
{
  char *pDoc;
  if (!PyArg_ParseTuple(args, "s", &pDoc))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Document* doc = GetApplication().getDocument(pDoc);
  if ( doc )
  {
    if ( doc->save() == false )
    {
      PyErr_Format(PyExc_Exception, "Cannot save document '%s'", pDoc);
      return 0L;
    }
  }
  else
  {
    PyErr_Format(PyExc_NameError, "Unknown document '%s'", pDoc);
    return NULL;
  }

  Py_Return;
}

PYFUNCIMP_S(Application,sSaveAs)
{
  char *pDoc, *pFileName;
  if (!PyArg_ParseTuple(args, "ss", &pDoc, &pFileName))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Document* doc = GetApplication().getDocument(pDoc);
	if ( doc )
  {
    doc->saveAs( pFileName );
  }
  else
  {
		PyErr_Format(PyExc_NameError, "Unknown document '%s'", pDoc);
		return NULL;
  }

  Py_Return;
}

PYFUNCIMP_S(Application,sSaveDocumentAs)
{
  char *pDoc, *pFileName;
  if (!PyArg_ParseTuple(args, "ss", &pDoc, &pFileName))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Document* doc = GetApplication().getDocument(pDoc);
  if ( doc )
  {
    doc->saveAs( pFileName );
  }
  else
  {
    PyErr_Format(PyExc_NameError, "Unknown document '%s'", pDoc);
    return NULL;
  }

  Py_Return;
}

PYFUNCIMP_S(Application,sActiveDocument)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  Document* doc = GetApplication().getActiveDocument();
  if (doc) {
	  return doc->GetPyObject();
  }else{
  	PyErr_SetString(PyExc_Exception, "No active document");
	  return 0L;
  }
}

PYFUNCIMP_S(Application,sGetDocument)
{
  char *pstr=0;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception

  Document* doc = GetApplication().getDocument(pstr);
  if ( !doc )
  {
    PyErr_Format(PyExc_NameError, "Unknown document '%s'", pstr);
    return 0L;
  }

  return doc->GetPyObject();
}

PYFUNCIMP_S(Application,sDocument)
{
  char *pstr=0;
  if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Document* doc=0;
  if(pstr == 0)
  {
    doc = GetApplication().getActiveDocument();
    if ( !doc )
    {
  	  PyErr_SetString(PyExc_Exception, "No active document");
	    return 0L;
    }
  }
  else
  {
    doc = GetApplication().getDocument(pstr);
    if ( !doc )
    {
  		PyErr_Format(PyExc_NameError, "Unknown document '%s'", pstr);
	    return 0L;
    }
  }

  return doc->GetPyObject();
}

PYFUNCIMP_S(Application,sGetParam)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	PY_TRY{
		return GetPyObject(GetApplication().GetParameterGroupByPath(pstr)); 
	}PY_CATCH;
}


PYFUNCIMP_S(Application,sGetConfig)
{
	char *pstr=0;

    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	if(pstr) // if parameter give deticated group
		return Py_BuildValue("s",GetApplication()._mConfig[pstr].c_str()); 
	else
	{
		PyObject *pDict = PyDict_New();
		for(std::map<std::string,std::string>::iterator It= GetApplication()._mConfig.begin();It!=GetApplication()._mConfig.end();It++)
		{
			PyBuf Buf(It->second.c_str()),Buf2(It->first.c_str());
			PyDict_SetItemString(pDict,Buf2.str,PyString_FromString(Buf.str));
		}
		return pDict;
		
	}
}

PYFUNCIMP_S(Application,sDumpConfig)
{

    if (!PyArg_ParseTuple(args, "") )    // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	std::string str;
	for(std::map<std::string,std::string>::iterator It= GetApplication()._mConfig.begin();It!=GetApplication()._mConfig.end();It++)
	{
		str += It->first ;
		int size = It->first.size();
		for(int l = 0; l < (28-size) ; l++)
			str += " ";

		str += "= " + It->second + "\r\n";
	}
	return Py_BuildValue("s",str.c_str());
		
}

PYFUNCIMP_S(Application,sSetConfig)
{
	char *pstr,*pstr2;

    if (!PyArg_ParseTuple(args, "ss", &pstr,&pstr2))  // convert args: Python->C 
        return NULL; // NULL triggers exception 

	GetApplication()._mConfig[pstr] = pstr2;

	Py_INCREF(Py_None);
	return Py_None;
}

PYFUNCIMP_S(Application,sGetVersion)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL; // NULL triggers exception 

	PyObject* pList = PyList_New(9);  
	PyObject *pItem;
  pItem = PyString_FromString(Application::Config()["BuildVersionMajor"].c_str());
	PyList_SetItem(pList, 0, pItem);
	pItem = PyString_FromString(Application::Config()["BuildVersionMinor"].c_str());
	PyList_SetItem(pList, 1, pItem);
	pItem = PyString_FromString(Application::Config()["BuildRevision"].c_str());
	PyList_SetItem(pList, 2, pItem);
	pItem = PyString_FromString(Application::Config()["BuildRevisionRange"].c_str());
	PyList_SetItem(pList, 3, pItem);
	pItem = PyString_FromString(Application::Config()["BuildRepositoryURL"].c_str());
	PyList_SetItem(pList, 4, pItem);
	pItem = PyString_FromString(Application::Config()["BuildRevisionDate"].c_str());
	PyList_SetItem(pList, 5, pItem);
	pItem = PyString_FromString(Application::Config()["BuildCurrentDate"].c_str());
	PyList_SetItem(pList, 6, pItem);
	pItem = PyString_FromString(Application::Config()["BuildScrClean"].c_str());
	PyList_SetItem(pList, 7, pItem);
	pItem = PyString_FromString(Application::Config()["BuildFCScrMixed"].c_str());
	PyList_SetItem(pList, 8, pItem);

	return pList;
}


PYFUNCIMP_S(Application,sEndingAdd)
{
	char *psKey,*psMod;

  if (!PyArg_ParseTuple(args, "ss", &psKey,&psMod))     
		return NULL;										

  GetApplication().addOpenType(psKey,psMod);

	Py_Return;
} 

PYFUNCIMP_S(Application,sEndingDelete)
{
	char*       psKey;

  if (!PyArg_ParseTuple(args, "s", &psKey) )     
		return NULL;										

  GetApplication().rmvOpenType(psKey);

	Py_Return;
} 

PYFUNCIMP_S(Application,sEndingGet)
{
	char*       psKey=0;

  if (!PyArg_ParseTuple(args, "|s", &psKey))     // convert args: Python->C 
     return NULL;                             // NULL triggers exception 

  if(psKey)
  {
  	return Py_BuildValue("s",GetApplication().hasOpenType(psKey));
  }else{
    return Interpreter().CreateFrom(GetApplication().getOpenType());
  }

}

PYFUNCIMP_S(Application,sListDocuments)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 
  PY_TRY {
    PyObject *pDict = PyDict_New();
    PyObject *pKey; Base::PyObjectBase* pValue;
    
    for (std::map<std::string,Document*>::const_iterator It = GetApplication().DocMap.begin();It != GetApplication().DocMap.end();++It)
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
