/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qapplication.h>
# include <qfileinfo.h>
#endif


#include "Application.h"
#include "BitmapFactory.h"
#include "Command.h"
#include "Document.h"
#include "MainWindow.h"
#include "PythonView.h"
#include "WidgetFactory.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

#include <Base/Interpreter.h>
#include <Base/Console.h>

using namespace Gui;

// FCApplication Methods						// Methods structure
PyMethodDef Application::Methods[] = {
  {"AddWorkbenchHandler",     (PyCFunction) Application::sAddWorkbenchHandler,     1,
   "deprecated -- use addWorkbenchHandler"},
  {"addWorkbenchHandler",     (PyCFunction) Application::sAddWorkbenchHandler,     1,
   "addWorkbenchHandler(string, object) -> None\n\n"
   "Add a workbench handler under a defined name."},
  {"RemoveWorkbenchHandler",  (PyCFunction) Application::sRemoveWorkbenchHandler,  1,
   "deprecated -- use RemoveWorkbenchHandler"},
  {"removeWorkbenchHandler",  (PyCFunction) Application::sRemoveWorkbenchHandler,  1,
   "removeWorkbenchHandler(string) -> None\n\n"
   "Remove the workbench handler with name"},
  {"GetWorkbenchHandler",     (PyCFunction) Application::sGetWorkbenchHandler,     1,
   "deprecated -- use getWorkbenchHandler"},
  {"getWorkbenchHandler",     (PyCFunction) Application::sGetWorkbenchHandler,     1,
   "getWorkbenchHandler(string) -> object\n\n"
   "Get the workbench handler by its name"},
  {"ListWorkbenchHandlers",   (PyCFunction) Application::sListWorkbenchHandlers,   1,
   "deprecated -- use listWorkbenchHandlers"},
  {"listWorkbenchHandlers",   (PyCFunction) Application::sListWorkbenchHandlers,   1,
   "listWorkbenchHandlers() -> list\n\n"
   "Show a list of all workbench handlers"},
  {"CreateWorkbench",         (PyCFunction) Application::sCreateWorkbench,         1,
   "deprecated -- use createWorkbench"},
  {"createWorkbench",         (PyCFunction) Application::sCreateWorkbench,         1,
   "createWorkbench(string, [string]) -> object\n\n"
   "Create a workbench with name of type"},
  {"ActiveWorkbench",         (PyCFunction) Application::sActiveWorkbench,         1,
   "deprecated -- use activeWorkbench"},
  {"activeWorkbench",         (PyCFunction) Application::sActiveWorkbench,         1,
   "activeWorkbench() -> object\n\n"
   "Return the active workbench object"},
  {"ActivateWorkbench",       (PyCFunction) Application::sActivateWorkbench,       1,
   "deprecated -- activateWorkbench"},
  {"activateWorkbench",       (PyCFunction) Application::sActivateWorkbench,       1,
   "activateWorkbench(string) -> None\n\n"
   "Activate the workbench with name"},
  {"ListWorkbenches",         (PyCFunction) Application::sListWorkbenches,         1,
   "deprecated -- use listWorkbenches"},
  {"listWorkbenches",         (PyCFunction) Application::sListWorkbenches,         1,
   "listWorkbenches() -> list\n\n"
   "Show a list of all workbenches"},
  {"GetWorkbench",            (PyCFunction) Application::sGetWorkbench,            1,
   "deprecated -- use getWorkbench"},
  {"getWorkbench",            (PyCFunction) Application::sGetWorkbench,            1,
   "getWorkbench(string) -> object\n\n"
   "Get a workbench by its name"},
  {"HasWorkbench",            (PyCFunction) Application::sHasWorkbench,            1,
   "deprecated -- use hasWorkbench"},
  {"hasWorkbench",            (PyCFunction) Application::sHasWorkbench,            1,
   "hasWorkbench(string) -> bool\n\n"
   "Check if the workbench with name exists"},
  {"addIconPath",             (PyCFunction) Application::sAddIconPath,             1,
   "addIconPath(string) -> None\n\n"
   "Add a new path to the system where to find icon files"},
  {"UpdateGui",               (PyCFunction) Application::sUpdateGui,               1,
   "deprecated -- use updateGui"},
  {"updateGui",               (PyCFunction) Application::sUpdateGui,               1,
   "updateGui() -> None\n\n"
   "Update the main window and all its windows"},
  {"CreateDialog",            (PyCFunction) Application::sCreateDialog,            1,
   "deprecated -- use createDialog"},
  {"createDialog",            (PyCFunction) Application::sCreateDialog,            1,
   "createDialog(string) -- Open a UI file"},
  {"AddCommand",              (PyCFunction) Application::sAddCommand,              1,
   "deprecated -- use addCommand"},
  {"addCommand",              (PyCFunction) Application::sAddCommand,              1,
   "addCommand(string, object) -> None\n\n"
   "Add a command object"},
  {"RunCommand",              (PyCFunction) Application::sRunCommand,              1,
   "deprecated -- use runCommand"},
  {"runCommand",              (PyCFunction) Application::sRunCommand,              1,
   "runCommand(string) -> None\n\n"
   "Run command with name"},
  {"SendMsgToActiveView",     (PyCFunction) Application::sSendActiveView,          1,
   "deprecated -- use class View"},
  {"hide",                    (PyCFunction) Application::shide,                    1,
   "deprecated"},
  {"show",                    (PyCFunction) Application::sshow,                    1,
   "deprecated"},
  {"open",                    (PyCFunction) Application::sopen,                    1,
   "Open a macro, Inventor or VRML file"},
  {"insert",                  (PyCFunction) Application::sinsert,                  1,
   "Open a macro, Inventor or VRML file"},
  {"activeDocument",          (PyCFunction) Application::sActiveDocument,          1,
   "activeDocument() -> object or None\n\n"
   "Return the active document or None if no one exists"},
  {"getDocument",             (PyCFunction) Application::sGetDocument,             1,
   "getDocument(string) -> object\n\n"
   "Get a document by its name"},

  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(Application,sActiveDocument)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 

    Document *pcDoc =  Instance->activeDocument();
    if (pcDoc) {
	    return pcDoc->getPyObject();
    } else {
        Py_Return;
    }
} 

PYFUNCIMP_S(Application,sGetDocument)
{
  char *pstr=0;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception

  Document *pcDoc =  Instance->getDocument(pstr);
  if ( !pcDoc )
  {
    PyErr_Format(PyExc_NameError, "Unknown document '%s'", pstr);
    return 0L;
  }

  return pcDoc->getPyObject();
} 

PYFUNCIMP_S(Application,shide)
{
  char *psFeatStr;
  if (!PyArg_ParseTuple(args, "s;Name of the Feature to hide have to be given!",&psFeatStr))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  Document *pcDoc =  Instance->activeDocument();

  if(pcDoc)
  {
    pcDoc->setHide(psFeatStr);  
  }
    
   Py_Return;
} 

PYFUNCIMP_S(Application,sshow)
{
  char *psFeatStr;
  if (!PyArg_ParseTuple(args, "s;Name of the Feature to hide have to be given!",&psFeatStr))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  Document *pcDoc =  Instance->activeDocument();

  if(pcDoc)
  {
    pcDoc->setShow(psFeatStr);  
  }
    
   Py_Return;
} 

PYFUNCIMP_S(Application,sopen)
{
  // only used to open Python files
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
  PY_TRY {
    QString fileName = QString::fromUtf8(Name);
    QFileInfo fi;
    fi.setFile(fileName);
    QString ext = fi.completeSuffix().toLower();
    MDIView* view = getMainWindow()->getWindowWithCaption(fileName);
    if ( view ) {
      view->setFocus();
    }
    else if ( ext == "iv" || ext == "wrl" || ext == "vrml" || ext == "wrz" ) {
      if ( !Application::Instance->activeDocument() )
        App::GetApplication().newDocument();
      QString cmd = QString("Gui.activeDocument().addAnnotation(\"%1\",\"%2\")").arg(fi.baseName()).arg(fi.absoluteFilePath());
      Base::Interpreter().runString(cmd.toAscii());
    }
    else if ( ext == "py" || ext == "fcmacro" || ext == "fcscript" ) {
      PythonView* edit = new PythonView(getMainWindow());
      edit->open(fileName);
      edit->resize( 400, 300 );
      getMainWindow()->addWindow( edit );
    }
  } PY_CATCH;

	Py_Return;    
} 

PYFUNCIMP_S(Application,sinsert)
{
  const char* Name;
  const char* DocName;
  if (! PyArg_ParseTuple(args, "ss",&Name,&DocName))	 		 
    return NULL;                         

  PY_TRY {
    QString fileName = QString::fromUtf8(Name);
    QFileInfo fi;
    fi.setFile(fileName);
    QString ext = fi.completeSuffix().toLower();
    if ( ext == "iv" || ext == "wrl" ) {
      QString cmd = QString("Gui.activeDocument().addAnnotation(\"%1\",\"%2\")").arg(fi.baseName()).arg(fi.absoluteFilePath());
      Base::Interpreter().runString(cmd.toAscii());
    }
    else if ( ext == "py" || ext == "fcmacro" || ext == "fcscript" ) {
      PythonView* edit = new PythonView(getMainWindow());
      edit->open(fileName);
      edit->resize( 400, 300 );
      getMainWindow()->addWindow( edit );
    }
  } PY_CATCH;

	Py_Return;    
} 

PYFUNCIMP_S(Application,sSendActiveView)
{
  char *psCommandStr;
  if (!PyArg_ParseTuple(args, "s",&psCommandStr))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  const char* ppReturn=0;
  if(!Instance->sendMsgToActiveView(psCommandStr,&ppReturn))
    Base::Console().Warning("Unknown view command: %s\n",psCommandStr);

  // Print the return value to the output
  if (ppReturn) {
    return Py_BuildValue("s",ppReturn);
  }

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(Application,sUpdateGui)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  qApp->processEvents();

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(Application,sCreateDialog)
{
  char* fn = 0;
  if (!PyArg_ParseTuple(args, "s", &fn))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  PyObject* pPyResource=0L;
  try{
    pPyResource = new PyResource();
    ((PyResource*)pPyResource)->load(fn);
  } catch (const Base::Exception& e)
  {
    PyErr_SetString(PyExc_AssertionError, e.what());
  }

  return pPyResource;
} 

PYFUNCIMP_S(Application,sAddWorkbenchHandler)
{
  char*       psKey;
  PyObject*   pcObject;
  if (!PyArg_ParseTuple(args, "sO", &psKey,&pcObject))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  try {
    // Search for some methods without invoking them
    PyObject* meth = 0;
    meth = PyObject_GetAttrString(pcObject, "Activate");
    if (!meth) throw Base::PyException();
    Py_DECREF(meth);
    meth = PyObject_GetAttrString(pcObject, "GetClassName");
    if (!meth) throw Base::PyException();
    Py_DECREF(meth);
    meth = PyObject_GetAttrString(pcObject, "GetIcon");
    if (!meth) throw Base::PyException();
    Py_DECREF(meth);
  } catch (const Base::PyException& e) {
    PyErr_Format(PyExc_AttributeError, "%s", e.what());
    return NULL;
  }

  PyObject* wb = PyDict_GetItemString(Instance->_pcWorkbenchDictionary,psKey); 
  if ( wb )
  {
    PyErr_Format(PyExc_KeyError, "'%s' already exists.", psKey);
    return NULL;
  }

  PyDict_SetItemString(Instance->_pcWorkbenchDictionary,psKey,pcObject);

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(Application,sRemoveWorkbenchHandler)
{
  char*       psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  PyObject* wb = PyDict_GetItemString(Instance->_pcWorkbenchDictionary,psKey); 
  if ( !wb )
  {
    PyErr_Format(PyExc_KeyError, "No such workbench handler '%s'", psKey);
    return NULL;
  }


  PyDict_DelItemString(Instance->_pcWorkbenchDictionary,psKey);

  // If the active workbench gets removed we must load another one
  Workbench* actWb = WorkbenchManager::instance()->active();
  if (actWb && actWb->name() == psKey)
  {
    // then just load the last workbench
    int ct = PyDict_Size( Instance->_pcWorkbenchDictionary );
    if ( ct > 0 )
    {
      PyObject* list = PyDict_Keys( Instance->_pcWorkbenchDictionary ); 
      PyObject* str = PyList_GetItem( list, ct-1 );
      Py_DECREF(list); // frees the list
      const char* name = PyString_AsString( str );
      Instance->activateWorkbench( name );
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(Application,sGetWorkbenchHandler)
{
  char* psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                                // NULL triggers exception 

   
  // get the python workbench object from the dictionary
  PyObject* pcWorkbench = PyDict_GetItemString(Instance->_pcWorkbenchDictionary, psKey);
  if ( !pcWorkbench )
  {
    PyErr_Format(PyExc_KeyError, "No such workbench handler '%s'", psKey);
    return NULL;
  }

  return pcWorkbench;
} 

PYFUNCIMP_S(Application,sListWorkbenchHandlers)
{
  Py_INCREF(Instance->_pcWorkbenchDictionary);
  return Instance->_pcWorkbenchDictionary;
} 

PYFUNCIMP_S(Application,sListWorkbenches)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
      return NULL;                             // NULL triggers exception 
  QStringList wb = WorkbenchManager::instance()->workbenches();
  PyObject* pyList = PyList_New(wb.count()); 
  int i=0;
  for ( QStringList::Iterator it = wb.begin(); it != wb.end(); ++it )
  {
    PyObject* str = PyString_FromString((*it).toAscii());
    PyList_SetItem(pyList, i++, str);
  }

  return pyList;
} 

PYFUNCIMP_S(Application,sCreateWorkbench)
{
  char* psName;
  char* psType="Gui::PythonWorkbench";
  if (!PyArg_ParseTuple(args, "s|s", &psName, &psType))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  if ( WorkbenchManager::instance()->getWorkbench(psName) )
  {
    PyErr_Format(PyExc_KeyError, "Workbench '%s' already exists", psName);
    return NULL;
  }

  PY_TRY {
    // create a workbench of specified type (default PythonWorkbench)
    Workbench* wb = WorkbenchManager::instance()->createWorkbench( psName, psType );
    
    // object get incremented
    if (wb) {
      PyObject* pyObj = wb->getPyObject();
      return pyObj;
    } else {
      PyErr_Format(PyExc_KeyError, "Cannot create workbench '%s'", psName);
      return NULL;
    }
  } PY_CATCH;

  Py_Return;    
} 

PYFUNCIMP_S(Application,sActiveWorkbench)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  Workbench* actWb = WorkbenchManager::instance()->active();
  if ( !actWb )
  {
    PyErr_SetString(PyExc_AssertionError, "No active workbench\n");		
    return NULL;
  }

  // object get incremented
  PyObject* pyObj = actWb->getPyObject();
  return pyObj;
} 

PYFUNCIMP_S(Application,sActivateWorkbench)
{
  char*       psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  // search for workbench handler from the dictionary
  PyObject* pcWorkbench = PyDict_GetItemString(Instance->_pcWorkbenchDictionary, psKey);
  if ( !pcWorkbench )
  {
    PyErr_Format(PyExc_KeyError, "No such workbench '%s'", psKey);
    return NULL;
  }

  Instance->activateWorkbench(psKey);

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(Application,sGetWorkbench)
{
  char*       psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  Workbench* wb = WorkbenchManager::instance()->getWorkbench( psKey );
  if ( !wb )
  {
    PyErr_Format(PyExc_AssertionError, "No such workbench '%s'\n", psKey);		
    return NULL;
  }

  // object get incremented
  return wb->getPyObject();
} 

PYFUNCIMP_S(Application,sHasWorkbench)
{
  char*       psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  Workbench* wb = WorkbenchManager::instance()->getWorkbench( psKey );
  if ( wb ) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
} 

PYFUNCIMP_S(Application,sAddIconPath)
{
  char* filePath;
  if (!PyArg_ParseTuple(args, "s", &filePath))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 
  QString path = QString::fromUtf8(filePath);
  if (QDir::isRelativePath(path)) {
    // Home path ends with '/'
    QString home = QString::fromUtf8(App::GetApplication().GetHomePath());
    path = home + path;
  }

  BitmapFactory().addPath(path);  
  Py_INCREF(Py_None);
  return Py_None;
}

PYFUNCIMP_S(Application,sAddCommand)
{
  char*       pName;
  PyObject*   pcCmdObj;
  if (!PyArg_ParseTuple(args, "sO", &pName,&pcCmdObj))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  //Py_INCREF(pcObject);

  Application::Instance->commandManager().addCommand(new PythonCommand(pName,pcCmdObj));

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(Application,sRunCommand)
{
  char*       pName;
  if (!PyArg_ParseTuple(args, "s", &pName))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  Application::Instance->commandManager().runCommandByName(pName);

  Py_INCREF(Py_None);
  return Py_None;
} 
