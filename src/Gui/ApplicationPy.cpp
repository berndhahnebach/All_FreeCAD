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
#include "Command.h"
#include "Document.h"
#include "MainWindow.h"
#include "PythonEditor.h"
#include "WidgetFactory.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

#include <Base/Interpreter.h>

using Base::Console;
using Base::Interpreter;
using namespace Gui;

// FCApplication Methods						// Methods structure
PyMethodDef Application::Methods[] = {
  {"AddWorkbenchHandler",     (PyCFunction) Application::sAddWorkbenchHandler,     1},
  {"RemoveWorkbenchHandler",  (PyCFunction) Application::sRemoveWorkbenchHandler,  1},
  {"GetWorkbenchHandler",     (PyCFunction) Application::sGetWorkbenchHandler,     1},
  {"ListWorkbenchHandlers",   (PyCFunction) Application::sListWorkbenchHandlers,   1},
  {"CreateWorkbench",         (PyCFunction) Application::sCreateWorkbench,         1},
  {"ActiveWorkbench",         (PyCFunction) Application::sActiveWorkbench,         1},
  {"ActivateWorkbench",       (PyCFunction) Application::sActivateWorkbench,       1},
  {"ListWorkbenches",         (PyCFunction) Application::sListWorkbenches,         1},
  {"GetWorkbench",            (PyCFunction) Application::sGetWorkbench,            1},
  {"HasWorkbench",            (PyCFunction) Application::sHasWorkbench,            1},
  {"UpdateGui",               (PyCFunction) Application::sUpdateGui,               1},
  {"CreateDialog",            (PyCFunction) Application::sCreateDialog,            1},
  {"AddCommand",              (PyCFunction) Application::sAddCommand,              1},
  {"RunCommand",              (PyCFunction) Application::sRunCommand,              1},
  {"SendMsgToActiveView",     (PyCFunction) Application::sSendActiveView,          1},
  {"hide",                    (PyCFunction) Application::shide,                    1},
  {"show",                    (PyCFunction) Application::sshow,                    1},
  {"open",                    (PyCFunction) Application::sopen,                    1},
  {"insert",                  (PyCFunction) Application::sinsert,                  1},
  {"document",                (PyCFunction) Application::sdocument,                1},
  {"activeDocument",          (PyCFunction) Application::sActiveDocument,          1},
  {"getDocument",             (PyCFunction) Application::sGetDocument,             1},

  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(Application,sdocument)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  Document *pcDoc =  Instance->activeDocument();
  if (pcDoc)
  {
    return pcDoc->getPyObject();
  }
    
  Py_Return;
} 

PYFUNCIMP_S(Application,sActiveDocument)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  Document *pcDoc =  Instance->activeDocument();
  if (pcDoc) {
	  return pcDoc->getPyObject();
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
    QFileInfo fi;
    fi.setFile(Name);
    QString ext = fi.extension().lower();
    MDIView* view = getMainWindow()->getWindowWithCaption( Name );
    if ( view ) {
      view->setFocus();
    }
    else if ( ext == "iv" || ext == "wrl" ) {
      if ( !Application::Instance->activeDocument() )
        App::GetApplication().newDocument();
      QString cmd = QString("Gui.activeDocument().addAnnotation(\"%1\",\"%2\")").arg(fi.baseName()).arg(fi.absFilePath());
      Base::Interpreter().runString( cmd.ascii() );
    }
    else if ( ext == "py" || ext == "fcmacro" || ext == "fcscript" ) {
      PythonEditView* edit = new PythonEditView( Name, getMainWindow(), "Editor" );
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
    QFileInfo fi;
    fi.setFile(Name);
    QString ext = fi.extension().lower();
    if ( ext == "iv" || ext == "wrl" ) {
      QString cmd = QString("Gui.activeDocument().addAnnotation(\"%1\",\"%2\")").arg(fi.baseName()).arg(fi.absFilePath());
      Base::Interpreter().runString( cmd.ascii() );
    }
    else if ( ext == "py" || ext == "fcmacro" || ext == "fcscript" ) {
      PythonEditView* edit = new PythonEditView( Name, getMainWindow(), "Editor" );
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

  PyObject* wb = PyDict_GetItemString(Instance->_pcWorkbenchDictionary,psKey); 
  if ( wb )
  {
    PyErr_Format(PyExc_KeyError, "'%s' already exists.", psKey);
    return NULL;
  }

  PyDict_SetItemString(Instance->_pcWorkbenchDictionary,psKey,pcObject);

  Instance->refreshWorkbenchList();

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

  Instance->refreshWorkbenchList();

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(Application,sGetWorkbenchHandler)
{
  char* psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                                // NULL triggers exception 

  Base::PyBuf Name(psKey);
  
  // get the python workbench object from the dictionary
  PyObject* pcWorkbench = PyDict_GetItemString(Instance->_pcWorkbenchDictionary, Name.str);
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
    PyObject* str = PyString_FromString((*it).latin1());
    PyList_SetItem(pyList, i++, str);
  }

  return pyList;
} 

PYFUNCIMP_S(Application,sCreateWorkbench)
{
  char*       psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  if ( WorkbenchManager::instance()->getWorkbench(psKey) )
  {
    PyErr_Format(PyExc_KeyError, "Workbench '%s' already exists", psKey);
    return NULL;
  }

  //WorkbenchFactory().AddProducer(psKey, new WorkbenchProducer<PythonWorkbench>);
  Workbench* wb = WorkbenchManager::instance()->createWorkbench( psKey, PythonWorkbench::getClassTypeId().getName() );
  
  // object get incremented
  Base::PyObjectBase* pyObj = wb->GetPyObject();
  return pyObj;
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
  Base::PyObjectBase* pyObj = actWb->GetPyObject();
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
  Base::PyObjectBase* pyObj = wb->GetPyObject();
  return pyObj ? pyObj : Py_None;
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
