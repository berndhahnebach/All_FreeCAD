/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "WorkbenchPy.h"
#include "Workbench.h"
#include "WorkbenchManager.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"

using namespace Gui;

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------
PyTypeObject WorkbenchPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "WorkbenchPy",          /*tp_name*/
  sizeof(WorkbenchPy),    /*tp_basicsize*/
  0,                      /*tp_itemsize*/
  /* methods */
  PyDestructor,           /*tp_dealloc*/
  0,                      /*tp_print*/
  __getattr,              /*tp_getattr*/
  __setattr,              /*tp_setattr*/
  0,                      /*tp_compare*/
  __repr,                 /*tp_repr*/
  0,                      /*tp_as_number*/
  0,                      /*tp_as_sequence*/
  0,                      /*tp_as_mapping*/
  0,                      /*tp_hash*/
  0,                      /*tp_call */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef WorkbenchPy::Methods[] = {
  PYMETHODEDEF(Name)
  PYMETHODEDEF(Activate)
  PYMETHODEDEF(AppendMenu)
  PYMETHODEDEF(RemoveMenu)
  PYMETHODEDEF(AppendContextMenu)
  PYMETHODEDEF(RemoveContextMenu)
  PYMETHODEDEF(AppendToolbar)
  PYMETHODEDEF(RemoveToolbar)
  PYMETHODEDEF(AppendCommandbar)
  PYMETHODEDEF(RemoveCommandbar)
  {NULL, NULL}          /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject WorkbenchPy::Parents[] = {&PyObjectBase::Type, NULL};     

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
WorkbenchPy::WorkbenchPy(PythonWorkbench *pcWb, PyTypeObject *T)
 : PyObjectBase( T), _pcWorkbench(pcWb)
{
  Base::Console().Log("Create WorkbenchPy (%d)\n",this);
}

PyObject *WorkbenchPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
WorkbenchPy::~WorkbenchPy()     // Everything handled in parent
{
  Base::Console().Log("Destroy WorkbenchPy: %p \n",this);
} 

//--------------------------------------------------------------------------
// WorkbenchPy representation
//--------------------------------------------------------------------------
PyObject *WorkbenchPy::_repr(void)
{
  return Py_BuildValue("s", "PythonWorkbench");
}

//--------------------------------------------------------------------------
// WorkbenchPy Attributes
//--------------------------------------------------------------------------
PyObject *WorkbenchPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{
  _getattr_up(PyObjectBase); 
} 

int WorkbenchPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  return PyObjectBase::_setattr(attr, value); 	// send up to parent
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
PYFUNCIMP_D(WorkbenchPy,Name)
{
  PY_TRY {
    QString name = _pcWorkbench->name(); 
    PyObject* pyName = PyString_FromString( name.latin1() );
    return pyName;
  }PY_CATCH;
} 

PYFUNCIMP_D(WorkbenchPy,Activate)
{
  PY_TRY {
    QString name = _pcWorkbench->name(); 
    WorkbenchManager::instance()->activate( name );
    Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(WorkbenchPy,AppendMenu)
{
  PY_TRY {
    PyObject* pObject;
    char* psMenu;
    if ( !PyArg_ParseTuple(args, "sO", &psMenu, &pObject) )
    {
      return NULL;                             // NULL triggers exception 
    }
    if (!PyList_Check(pObject))
    {
      PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
      return NULL;                             // NULL triggers exception 
    }

    QStringList items;
    int nSize = PyList_Size(pObject);
    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(pObject, i);
      if (!PyString_Check(item))
        continue;
      char* pItem = PyString_AsString(item);
      items.push_back(pItem);
    }

    _pcWorkbench->appendMenu( psMenu, items );

    Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(WorkbenchPy,RemoveMenu)
{ 
  PY_TRY {
    char *psMenu;
    if (!PyArg_ParseTuple(args, "s", &psMenu))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
    
    _pcWorkbench->removeMenu( psMenu );
    Py_Return; 
  }PY_CATCH;
}

PYFUNCIMP_D(WorkbenchPy,AppendContextMenu)
{ 
  PY_TRY {
    PyObject* pObject;
    char* psMenu;
    if ( !PyArg_ParseTuple(args, "sO", &psMenu, &pObject) )
    {
      return NULL;                             // NULL triggers exception 
    }
    if (!PyList_Check(pObject))
    {
      PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
      return NULL;                             // NULL triggers exception 
    }

    QStringList items;
    int nSize = PyList_Size(pObject);
    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(pObject, i);
      if (!PyString_Check(item))
        continue;
      char* pItem = PyString_AsString(item);
      items.push_back(pItem);
    }

    _pcWorkbench->appendContextMenu( psMenu, items );

    Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(WorkbenchPy,RemoveContextMenu)
{ 
  PY_TRY {
    char *psMenu;
    if (!PyArg_ParseTuple(args, "s", &psMenu))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
    
    _pcWorkbench->removeContextMenu( psMenu );
    Py_Return; 
  }PY_CATCH;
}

PYFUNCIMP_D(WorkbenchPy,AppendToolbar)
{ 
  PY_TRY {
    PyObject* pObject;
    char* psToolBar;
    if ( !PyArg_ParseTuple(args, "sO", &psToolBar, &pObject) )
    {
      return NULL;                             // NULL triggers exception 
    }
    if (!PyList_Check(pObject))
    {
      PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
      return NULL;                             // NULL triggers exception 
    }

    QStringList items;
    int nSize = PyList_Size(pObject);
    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(pObject, i);
      if (!PyString_Check(item))
        continue;
      char* pItem = PyString_AsString(item);
      items.push_back(pItem);
    }

    _pcWorkbench->appendToolbar( psToolBar, items );

    Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(WorkbenchPy,RemoveToolbar)
{ 
  PY_TRY {
    char *psToolBar;
    if (!PyArg_ParseTuple(args, "s", &psToolBar))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
    
    _pcWorkbench->removeToolbar( psToolBar );
    Py_Return; 
  }PY_CATCH;
}

PYFUNCIMP_D(WorkbenchPy,AppendCommandbar)
{ 
  PY_TRY {
    PyObject* pObject;
    char* psToolBar;
    if ( !PyArg_ParseTuple(args, "sO", &psToolBar, &pObject) )
    {
      return NULL;                             // NULL triggers exception 
    }
    if (!PyList_Check(pObject))
    {
      PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
      return NULL;                             // NULL triggers exception 
    }

    QStringList items;
    int nSize = PyList_Size(pObject);
    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(pObject, i);
      if (!PyString_Check(item))
        continue;
      char* pItem = PyString_AsString(item);
      items.push_back(pItem);
    }

    _pcWorkbench->appendCommandbar( psToolBar, items );

    Py_Return; 
  }PY_CATCH;
} 

PYFUNCIMP_D(WorkbenchPy,RemoveCommandbar)
{ 
  PY_TRY {
    char *psToolBar;
    if (!PyArg_ParseTuple(args, "s", &psToolBar))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
    
    _pcWorkbench->removeCommandbar( psToolBar );
    Py_Return; 
  }PY_CATCH;
}
