/***************************************************************************
 *   (c) Juergen Riegel (juergen.riegel@web.de) 2002                       *
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
#include <Base/Interpreter.h>
#include <Base/Exception.h>
#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Base/Factory.h>

#define new DEBUG_CLIENTBLOCK
//using Base::GetConsole;
using namespace Base;
using namespace App;



//**************************************************************************
// Python stuff

// Application Methods						// Methods structure
PyMethodDef Application::Methods[] = {
    {"ParamGet",       (PyCFunction) Application::sGetParam,       1,
     "Get parameters by path"},
    {"Version",        (PyCFunction) Application::sGetVersion,     1,
     "Print the version to the output."},
    {"ConfigGet",      (PyCFunction) Application::sGetConfig,      1,
     "ConfigGet([string]) -- Get the value for the given key.\n"
     "If no key is given the complete configuration is dumped to\n"
     "the output."},
    {"ConfigSet",      (PyCFunction) Application::sSetConfig,      1,
     "ConfigSet(string, string) -- Set the given key to the given value."},
    {"ConfigDump",     (PyCFunction) Application::sDumpConfig,     1,
     "Dump the configuration to the output."},
    {"EndingAdd",      (PyCFunction) Application::sEndingAdd,      1,
     "Register file extension"},
    {"EndingDelete",   (PyCFunction) Application::sEndingDelete   ,1,
     "Unregister file extension"},
    {"EndingGet",      (PyCFunction) Application::sEndingGet      ,1,
     "Not yet implemented"},

    {"open",   (PyCFunction) Application::sOpenDocument,   1,
     "See openDocument(string)"},
    {"openDocument",   (PyCFunction) Application::sOpenDocument,   1,
     "openDocument(string) -> object\n\n"
     "Create a document and load the project file into the document.\n"
     "The string argument must point to an existing file. If the file doesn't exist\n"
     "or the file cannot be loaded an I/O exception is thrown. In this case the\n"
     "document is kept alive."},
//  {"saveDocument",   (PyCFunction) Application::sSaveDocument,   1,
//   "saveDocument(string) -- Save the document to a file."},
//  {"saveDocumentAs", (PyCFunction) Application::sSaveDocumentAs, 1},
    {"newDocument",    (PyCFunction) Application::sNewDocument,    1,
     "newDocument([string]) -> object\n\n"
     "Create a new document with a given name.\n"
     "The document name must be unique which\n"
     "is checked automatically."},
    {"closeDocument",  (PyCFunction) Application::sCloseDocument,  1,
     "closeDocument(string) -> None\n\n"
     "Close the document with a given name."},
    {"activeDocument", (PyCFunction) Application::sActiveDocument, 1,
     "activeDocument() -> object or None\n\n"
     "Return the active document or None if there is no one."},
    {"setActiveDocument",(PyCFunction) Application::sSetActiveDocument, 1,
     "setActiveDocement(string) -> None\n\n"
     "Set the active document by its name."},
    {"getDocument",    (PyCFunction) Application::sGetDocument,    1,
     "getDocument(string) -> object\n\n"
     "Get a document by its name or raise an exception\n"
     "if there is no document with the given name."},
    {"listDocuments",  (PyCFunction) Application::sListDocuments  ,1,
     "listDocuments() -> list\n\n"
     "Return a list of names of all documents."},

    {NULL, NULL, 0, NULL}		/* Sentinel */
};

PyObject* Application::sOpenDocument(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception
    try {
        // return new document
        return (GetApplication().openDocument(pstr)->getPyObject());
    }
    catch (Base::Exception e) {
        PyErr_SetString(PyExc_IOError, e.what());
        return 0L;
    }
}

PyObject* Application::sNewDocument(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr = 0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PY_TRY {
        return GetApplication().newDocument(pstr)->getPyObject();
    }PY_CATCH;
}

PyObject* Application::sSetActiveDocument(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr = 0;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    try {
        GetApplication().setActiveDocument(pstr);
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_Exception, e.what());
        return NULL;
    }

    Py_Return;
}

PyObject* Application::sCloseDocument(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr = 0;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    if ( GetApplication().closeDocument(pstr) == false ) {
        PyErr_Format(PyExc_NameError, "Unknown document '%s'", pstr);
        return NULL;
    }

    Py_Return;
}

PyObject* Application::sSaveDocument(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pDoc;
    if (!PyArg_ParseTuple(args, "s", &pDoc))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    Document* doc = GetApplication().getDocument(pDoc);
    if ( doc ) {
        if ( doc->save() == false ) {
            PyErr_Format(PyExc_Exception, "Cannot save document '%s'", pDoc);
            return 0L;
        }
    }
    else {
        PyErr_Format(PyExc_NameError, "Unknown document '%s'", pDoc);
        return NULL;
    }

    Py_Return;
}
/*
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
*/
PyObject* Application::sActiveDocument(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C
        return NULL;                       // NULL triggers exception

    Document* doc = GetApplication().getActiveDocument();
    if (doc) {
        return doc->getPyObject();
    }
    else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}

PyObject* Application::sGetDocument(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    Document* doc = GetApplication().getDocument(pstr);
    if ( !doc ) {
        PyErr_Format(PyExc_NameError, "Unknown document '%s'", pstr);
        return 0L;
    }

    return doc->getPyObject();
}

PyObject* Application::sGetParam(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PY_TRY {
        return GetPyObject(GetApplication().GetParameterGroupByPath(pstr));
    }PY_CATCH;
}


PyObject* Application::sGetConfig(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr=0;

    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception
    if (pstr) // if parameter give deticated group
        return Py_BuildValue("s",GetApplication()._mConfig[pstr].c_str());
    else {
        PyObject *pDict = PyDict_New();
        for (std::map<std::string,std::string>::iterator It= GetApplication()._mConfig.begin();
             It!=GetApplication()._mConfig.end();It++) {
            PyDict_SetItemString(pDict,It->first.c_str(),PyString_FromString(It->second.c_str()));
        }
        return pDict;

    }
}

PyObject* Application::sDumpConfig(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    if (!PyArg_ParseTuple(args, "") )    // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PyObject *dict = PyDict_New();
    for (std::map<std::string,std::string>::iterator It= GetApplication()._mConfig.begin();
         It!=GetApplication()._mConfig.end();It++) {
        PyDict_SetItemString(dict,It->first.c_str(), PyString_FromString(It->second.c_str()));
    }
    return dict;
}

PyObject* Application::sSetConfig(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *pstr,*pstr2;

    if (!PyArg_ParseTuple(args, "ss", &pstr,&pstr2))  // convert args: Python->C
        return NULL; // NULL triggers exception

    GetApplication()._mConfig[pstr] = pstr2;

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* Application::sGetVersion(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
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
    pItem = PyString_FromString(Application::Config()["BuildScrMixed"].c_str());
    PyList_SetItem(pList, 8, pItem);

    return pList;
}


PyObject* Application::sEndingAdd(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char *psKey,*psMod;

    if (!PyArg_ParseTuple(args, "ss", &psKey,&psMod))
        return NULL;

    GetApplication().addOpenType(psKey,psMod);

    Py_Return;
}

PyObject* Application::sEndingDelete(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char*       psKey;

    if (!PyArg_ParseTuple(args, "s", &psKey) )
        return NULL;

    GetApplication().rmvOpenType(psKey);

    Py_Return;
}

PyObject* Application::sEndingGet(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    char*       psKey=0;

    if (!PyArg_ParseTuple(args, "|s", &psKey))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    if (psKey) {
        return Py_BuildValue("s",GetApplication().hasOpenType(psKey));
    }
    else {
        return Interpreter().CreateFrom(GetApplication().getOpenType());
    }

}

PyObject* Application::sListDocuments(PyObject * /*self*/, PyObject *args,PyObject * /*kwd*/)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C
        return NULL;                       // NULL triggers exception
    PY_TRY {
        PyObject *pDict = PyDict_New();
        PyObject *pKey;
        Base::PyObjectBase* pValue;

        for (std::map<std::string,Document*>::const_iterator It = GetApplication().DocMap.begin();
             It != GetApplication().DocMap.end();++It) {
            pKey   = PyString_FromString(It->first.c_str());
            // GetPyObject() increments
            pValue = static_cast<Base::PyObjectBase*>(It->second->getPyObject());
            PyDict_SetItem(pDict, pKey, pValue);
            // now we can decrement again as PyDict_SetItem also has incremented
            pValue->DecRef();
        }

        return pDict;
    } PY_CATCH;
}
