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
#endif


#include "Application.h"
#include "Command.h"
#include "Document.h"
#include "WidgetFactory.h"
#include "Workbench.h"
#include "WorkbenchFactory.h"
#include "WorkbenchManager.h"

#include <Base/Interpreter.h>

using Base::Console;
using Base::Interpreter;
using namespace Gui;

namespace Gui {

} // namespace Gui

/* TRANSLATOR Gui::ApplicationWindow */

// FCApplication Methods						// Methods structure
PyMethodDef ApplicationWindow::Methods[] = {
  {"AddWorkbenchHandler",     (PyCFunction) ApplicationWindow::sAddWorkbenchHandler,     1},
  {"RemoveWorkbenchHandler",  (PyCFunction) ApplicationWindow::sRemoveWorkbenchHandler,  1},
  {"GetWorkbenchHandler",     (PyCFunction) ApplicationWindow::sGetWorkbenchHandler,     1},
  {"CreateWorkbench",         (PyCFunction) ApplicationWindow::sCreateWorkbench,         1},
  {"ActiveWorkbench",         (PyCFunction) ApplicationWindow::sActiveWorkbench,         1},
  {"ActivateWorkbench",       (PyCFunction) ApplicationWindow::sActivateWorkbench,       1},
  {"ListWorkbenches",         (PyCFunction) ApplicationWindow::sListWorkbenches,         1},
  {"GetWorkbench",            (PyCFunction) ApplicationWindow::sGetWorkbench,            1},
  {"HasWorkbench",            (PyCFunction) ApplicationWindow::sHasWorkbench,            1},
  {"UpdateGui",               (PyCFunction) ApplicationWindow::sUpdateGui,               1},
  {"CreateDialog",            (PyCFunction) ApplicationWindow::sCreateDialog,            1},
  {"AddCommand",              (PyCFunction) ApplicationWindow::sAddCommand,              1},
  {"RunCommand",              (PyCFunction) ApplicationWindow::sRunCommand,              1},
  {"SendMsgToActiveView",     (PyCFunction) ApplicationWindow::sSendActiveView,          1},
  {"hide",                    (PyCFunction) ApplicationWindow::shide,                    1},
  {"show",                    (PyCFunction) ApplicationWindow::sshow,                    1},

  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(ApplicationWindow,shide)
{
  char *psFeatStr;
  if (!PyArg_ParseTuple(args, "s;Name of the Feature to hide have to be given!",&psFeatStr))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  Document *pcDoc =  Instance->activeDocument();

  if(pcDoc)
  {
    App::Feature *pcFeat = pcDoc->getDocument()->getFeature(psFeatStr);

    if(pcFeat)
    {
      pcDoc->setHide(pcFeat);  
    }
  }
    
   Py_Return;
} 

PYFUNCIMP_S(ApplicationWindow,sshow)
{
  char *psFeatStr;
  if (!PyArg_ParseTuple(args, "s;Name of the Feature to hide have to be given!",&psFeatStr))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  Document *pcDoc =  Instance->activeDocument();

  if(pcDoc)
  {
    App::Feature *pcFeat = pcDoc->getDocument()->getFeature(psFeatStr);

    if(pcFeat)
    {
      pcDoc->setShow(pcFeat);  
    }
  }
    
   Py_Return;
} 

PYFUNCIMP_S(ApplicationWindow,sSendActiveView)
{
  char *psCommandStr;
  if (!PyArg_ParseTuple(args, "s",&psCommandStr))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  if(!Instance->sendMsgToActiveView(psCommandStr))
    Base::Console().Warning("Unknown view command: %s\n",psCommandStr);

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sUpdateGui)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                                      // NULL triggers exception 

  qApp->processEvents();

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sCreateDialog)
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

PYFUNCIMP_S(ApplicationWindow,sAddWorkbenchHandler)
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

PYFUNCIMP_S(ApplicationWindow,sRemoveWorkbenchHandler)
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

PYFUNCIMP_S(ApplicationWindow,sGetWorkbenchHandler)
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

PYFUNCIMP_S(ApplicationWindow,sListWorkbenches)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
      return NULL;                             // NULL triggers exception 
  QStringList wb = WorkbenchFactory().workbenches();
  PyObject* pyList = PyList_New(wb.count()); 
  int i=0;
  for ( QStringList::Iterator it = wb.begin(); it != wb.end(); ++it )
  {
    PyObject* str = PyString_FromString((*it).latin1());
    PyList_SetItem(pyList, i++, str);
  }

  return pyList;
} 

PYFUNCIMP_S(ApplicationWindow,sCreateWorkbench)
{
  char*       psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  if ( WorkbenchFactory().CanProduce(psKey) )
  {
    PyErr_Format(PyExc_KeyError, "Workbench '%s' already exists", psKey);
    return NULL;
  }

  WorkbenchFactory().AddProducer(psKey, new WorkbenchProducer<PythonWorkbench>);
  Workbench* wb = WorkbenchManager::instance()->getWorkbench( psKey );
  
  Base::PyObjectBase* pyObj = wb->GetPyObject();
  return pyObj;
} 

PYFUNCIMP_S(ApplicationWindow,sActiveWorkbench)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  Workbench* actWb = WorkbenchManager::instance()->active();
  if ( !actWb )
  {
    PyErr_SetString(PyExc_AssertionError, "No active workbench\n");		
    return NULL;
  }

  Base::PyObjectBase* pyObj = actWb->GetPyObject();
//  pyObj->_INCREF();
  return pyObj;
} 

PYFUNCIMP_S(ApplicationWindow,sActivateWorkbench)
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

PYFUNCIMP_S(ApplicationWindow,sGetWorkbench)
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

  Base::PyObjectBase* pyObj = wb->GetPyObject();
//  pyObj->_INCREF();
  return pyObj ? pyObj : Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sHasWorkbench)
{
  char*       psKey;
  if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  Workbench* wb = WorkbenchManager::instance()->getWorkbench( psKey );
  return wb ? Py_True : Py_False;
} 

PYFUNCIMP_S(ApplicationWindow,sAddCommand)
{
  char*       pName;
  PyObject*   pcCmdObj;
  if (!PyArg_ParseTuple(args, "sO", &pName,&pcCmdObj))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  //Py_INCREF(pcObject);

  ApplicationWindow::Instance->commandManager().addCommand(new PythonCommand(pName,pcCmdObj));

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sRunCommand)
{
  char*       pName;
  if (!PyArg_ParseTuple(args, "s", &pName))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  ApplicationWindow::Instance->commandManager().runCommandByName(pName);

  Py_INCREF(Py_None);
  return Py_None;
} 
