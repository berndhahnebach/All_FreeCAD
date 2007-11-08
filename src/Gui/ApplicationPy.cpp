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
#include <Base/PyCXX/Objects.hxx>

using namespace Gui;

// FCApplication Methods						// Methods structure
PyMethodDef Application::Methods[] = {
  {"activateWorkbench",(PyCFunction) Application::sActivateWorkbenchHandler,1,
   "activateWorkbench(string) -> None\n\n"
   "Activate the workbench by name"},
  {"addWorkbench",     (PyCFunction) Application::sAddWorkbenchHandler,     1,
   "addWorkbench(string, object) -> None\n\n"
   "Add a workbench under a defined name."},
  {"removeWorkbench",  (PyCFunction) Application::sRemoveWorkbenchHandler,  1,
   "removeWorkbench(string) -> None\n\n"
   "Remove the workbench with name"},
  {"getWorkbench",     (PyCFunction) Application::sGetWorkbenchHandler,     1,
   "getWorkbench(string) -> object\n\n"
   "Get the workbench by its name"},
  {"listWorkbench",   (PyCFunction) Application::sListWorkbenchHandlers,    1,
   "listWorkbench() -> list\n\n"
   "Show a list of all workbenches"},
  {"activeWorkbench", (PyCFunction) Application::sActiveWorkbenchHandler,   1,
   "activeWorkbench() -> object\n\n"
   "Return the active workbench object"},
  {"addIconPath",             (PyCFunction) Application::sAddIconPath,      1,
   "addIconPath(string) -> None\n\n"
   "Add a new path to the system where to find icon files"},
  {"addIcon",                 (PyCFunction) Application::sAddIcon,          1,
   "addIcon(string, string or list) -> None\n\n"
   "Add an icon as file name or in XPM format to the system"},
  {"updateGui",               (PyCFunction) Application::sUpdateGui,        1,
   "updateGui() -> None\n\n"
   "Update the main window and all its windows"},
  {"createDialog",            (PyCFunction) Application::sCreateDialog,     1,
   "createDialog(string) -- Open a UI file"},
  {"addCommand",              (PyCFunction) Application::sAddCommand,       1,
   "addCommand(string, object) -> None\n\n"
   "Add a command object"},
  {"runCommand",              (PyCFunction) Application::sRunCommand,       1,
   "runCommand(string) -> None\n\n"
   "Run command with name"},
  {"SendMsgToActiveView",     (PyCFunction) Application::sSendActiveView,   1,
   "deprecated -- use class View"},
  {"hide",                    (PyCFunction) Application::sHide,             1,
   "deprecated"},
  {"show",                    (PyCFunction) Application::sShow,             1,
   "deprecated"},
  {"open",                    (PyCFunction) Application::sOpen,             1,
   "Open a macro, Inventor or VRML file"},
  {"insert",                  (PyCFunction) Application::sInsert,           1,
   "Open a macro, Inventor or VRML file"},
  {"activeDocument",          (PyCFunction) Application::sActiveDocument,   1,
   "activeDocument() -> object or None\n\n"
   "Return the active document or None if no one exists"},
  {"getDocument",             (PyCFunction) Application::sGetDocument,      1,
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

PYFUNCIMP_S(Application,sHide)
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

PYFUNCIMP_S(Application,sShow)
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

PYFUNCIMP_S(Application,sOpen)
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
      //QString cmd = QString("Gui.activeDocument().addAnnotation(\"%1\",\"%2\")").arg(fi.baseName()).arg(fi.absoluteFilePath());
      QString cmd = QString("App.activeDocument().addObject(\"App::InventorObject\",\"%1\").FileName=\"%2\"\n"
                            "App.activeDocument().recompute()").arg(fi.baseName()).arg(fi.absoluteFilePath());
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

PYFUNCIMP_S(Application,sInsert)
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
    if ( ext == "iv" || ext == "wrl" || ext == "vrml" || ext == "wrz" ) {
      //QString cmd = QString("Gui.activeDocument().addAnnotation(\"%1\",\"%2\")").arg(fi.baseName()).arg(fi.absoluteFilePath());
      QString cmd = QString("App.activeDocument().addObject(\"App::InventorObject\",\"%1\").FileName=\"%2\"\n"
                            "App.activeDocument().recompute()").arg(fi.baseName()).arg(fi.absoluteFilePath());
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

PYFUNCIMP_S(Application,sActivateWorkbenchHandler)
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

PYFUNCIMP_S(Application,sAddWorkbenchHandler)
{
    PyObject*   pcObject;
    std::string item;
    if (!PyArg_ParseTuple(args, "O", &pcObject))     // convert args: Python->C 
        return NULL;                    // NULL triggers exception 

    try {
        // Search for some methods and members without invoking them
        Py::Object object(pcObject);
        Py::Callable(object.getAttr(std::string("Activate")));
        Py::Callable(object.getAttr(std::string("GetClassName")));
        Py::String text(object.getAttr(std::string("MenuText")));
        item = text.as_std_string();
        object.getAttr(std::string("Icon"));
    }
    catch (const Py::Exception&) {
        return NULL;
    }

    PyObject* wb = PyDict_GetItemString(Instance->_pcWorkbenchDictionary,item.c_str()); 
    if (wb) {
        PyErr_Format(PyExc_KeyError, "'%s' already exists.", item.c_str());
        return NULL;
    }

    PyDict_SetItemString(Instance->_pcWorkbenchDictionary,item.c_str(),pcObject);

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
    PyErr_Format(PyExc_KeyError, "No such workbench '%s'", psKey);
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
    PyErr_Format(PyExc_KeyError, "No such workbench '%s'", psKey);
    return NULL;
  }

  Py_INCREF(pcWorkbench);
  return pcWorkbench;
} 

PYFUNCIMP_S(Application,sListWorkbenchHandlers)
{
  Py_INCREF(Instance->_pcWorkbenchDictionary);
  return Instance->_pcWorkbenchDictionary;
} 

PYFUNCIMP_S(Application,sActiveWorkbenchHandler)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  Workbench* actWb = WorkbenchManager::instance()->active();
  if ( !actWb )
  {
    PyErr_SetString(PyExc_AssertionError, "No active workbench\n");		
    return NULL;
  }

  // get the python workbench object from the dictionary
  QByteArray psKey = actWb->name().toUtf8();
  PyObject* pcWorkbench = PyDict_GetItemString(Instance->_pcWorkbenchDictionary, psKey);
  if ( !pcWorkbench )
  {
    PyErr_Format(PyExc_KeyError, "No such workbench '%s'", psKey);
    return NULL;
  }

  // object get incremented
  Py_INCREF(pcWorkbench);
  return pcWorkbench;
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

PYFUNCIMP_S(Application,sAddIcon)
{
    char *iconName;
    PyObject* pixmap;
    if (!PyArg_ParseTuple(args, "sO", &iconName,&pixmap))     // convert args: Python->C 
        return NULL;                    // NULL triggers exception 
    
    QPixmap icon;
    if (BitmapFactory().findPixmapInCache(iconName, icon)) {
        PyErr_SetString(PyExc_AssertionError, "Icon with this name already registered");
        return NULL;
    }

    if (PyList_Check(pixmap)) {
        // create temporary buffer
        int ct = PyList_Size(pixmap);
        QByteArray ary;

        if ( ct > 0 ) {
            PyObject* line = PyList_GetItem(pixmap,0);
            if ( line && PyString_Check(line) ) {
                const char* szBuf = PyString_AsString(line);
                int strlen = PyString_Size(line);
                ary.resize(strlen);
                for (int j=0; j<strlen; j++)
                    ary[j]=szBuf[j];
                icon.loadFromData(ary, "XPM");
            }
        }
    }
    else if (PyString_Check(pixmap)){
        QString file = QString::fromUtf8(PyString_AsString(pixmap));
        icon.load(file);
    }

    if (icon.isNull()) {
        PyErr_SetString(PyExc_Exception, "Invalid icon");
        return NULL;
    }

    BitmapFactory().addPixmapToCache(iconName, icon);

    Py_INCREF(Py_None);
    return Py_None;
}

PYFUNCIMP_S(Application,sAddCommand)
{
  char*       pName;
  char*       pActivateionString=0;
  PyObject*   pcCmdObj;
  if (!PyArg_ParseTuple(args, "sO|s", &pName,&pcCmdObj,&pActivateionString))     // convert args: Python->C 
    return NULL;                    // NULL triggers exception 

  //Py_INCREF(pcObject);

  Application::Instance->commandManager().addCommand(new PythonCommand(pName,pcCmdObj,pActivateionString));

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
