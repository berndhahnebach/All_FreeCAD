/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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

#include "Workbench.h"

// inclusion of the generated files (generated out of PythonWorkbenchPy.xml)
#include "PythonWorkbenchPy.h"
#include "PythonWorkbenchPy.cpp"

using namespace Gui;

/** @class PythonWorkbenchPy
 * The workbench Python class provides additional methods for manipulation of python worbench
 * objects. 
 * From the view of Python PythonWorkbenchPy is also derived from WorkbenchPy as in C++.
 * @see Workbench
 * @see WorkbenchPy
 * @see PythonWorkbench
 * @author Werner Mayer
 */

// returns a string which represent the object e.g. when printed in python
const char *PythonWorkbenchPy::representation(void)
{
	return "PythonWorkbenchPy";
}


/** Appends a new menu */
PyObject*  PythonWorkbenchPy::appendMenu(PyObject *args)
{
    PY_TRY {
        PyObject* pPath;
        PyObject* pItems;
        if ( !PyArg_ParseTuple(args, "OO", &pPath, &pItems) )
            return NULL;                             // NULL triggers exception 

        // menu path
        QStringList path;
        if (PyList_Check(pPath)) {
            int nDepth = PyList_Size(pPath);
            for (int j=0; j<nDepth;++j) {
                PyObject* item = PyList_GetItem(pPath, j);
                if (!PyString_Check(item))
                    continue;
                char* pItem = PyString_AsString(item);
                path.push_back(pItem);
            }
        } else if (PyString_Check(pPath)) {
            // one single item
            char* pItem = PyString_AsString(pPath);
            path.push_back(pItem);
        } else {
            PyErr_SetString(PyExc_AssertionError, "Expected either a string or a stringlist as first argument");
            return NULL;                             // NULL triggers exception 
        }

        // menu items
        QStringList items;
        if (PyList_Check(pItems)) {
            int nItems = PyList_Size(pItems);
            for (int i=0; i<nItems;++i) {
                PyObject* item = PyList_GetItem(pItems, i);
                if (!PyString_Check(item))
                    continue;
                char* pItem = PyString_AsString(item);
                items.push_back(pItem);
            }
        } else if (PyString_Check(pItems)) {
            // one single item
            char* pItem = PyString_AsString(pItems);
            items.push_back(pItem);
        } else {
            PyErr_SetString(PyExc_AssertionError, "Expected either a string or a stringlist as first argument");
            return NULL;                             // NULL triggers exception 
        }

        getPythonWorkbenchObject()->appendMenu( path, items );

        Py_Return; 
    } PY_CATCH;
}

/** Removes a menu */
PyObject*  PythonWorkbenchPy::removeMenu(PyObject *args)
{
    PY_TRY {
        char *psMenu;
        if (!PyArg_ParseTuple(args, "s", &psMenu))     // convert args: Python->C 
            return NULL;                             // NULL triggers exception 
    
        getPythonWorkbenchObject()->removeMenu( psMenu );
        Py_Return; 
    } PY_CATCH;
}

/** Shows a list of all menus */
PyObject*  PythonWorkbenchPy::listMenus(PyObject *args)
{
    PY_TRY {
        QStringList menus = getPythonWorkbenchObject()->listMenus();

        PyObject* pyList = PyList_New(menus.count());
        int i=0;
        for ( QStringList::Iterator it = menus.begin(); it != menus.end(); ++it, ++i ) {
            PyObject* str = PyString_FromString((const char*)(*it).toAscii());
            PyList_SetItem(pyList, i, str);
        }
        return pyList; 
    } PY_CATCH;
}

/** Appends new context menu items */
PyObject*  PythonWorkbenchPy::appendContextMenu(PyObject *args)
{
    PY_TRY {
        PyObject* pObject;
        char* psMenu;
        if ( !PyArg_ParseTuple(args, "sO", &psMenu, &pObject) )
            return NULL;                             // NULL triggers exception 
        if (!PyList_Check(pObject)) {
            PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
            return NULL;                             // NULL triggers exception 
        }

        QStringList items;
        int nSize = PyList_Size(pObject);
        for (int i=0; i<nSize;++i) {
            PyObject* item = PyList_GetItem(pObject, i);
            if (!PyString_Check(item))
                continue;
            char* pItem = PyString_AsString(item);
            items.push_back(pItem);
        }

        getPythonWorkbenchObject()->appendContextMenu( psMenu, items );

        Py_Return; 
    } PY_CATCH;
}

/** Removes a context menu */
PyObject*  PythonWorkbenchPy::removeContextMenu(PyObject *args)
{
    PY_TRY {
        char *psMenu;
        if (!PyArg_ParseTuple(args, "s", &psMenu))     // convert args: Python->C 
            return NULL;                             // NULL triggers exception 
    
        getPythonWorkbenchObject()->removeContextMenu( psMenu );
        Py_Return; 
    } PY_CATCH;
}

/** Appends a new toolbar */
PyObject*  PythonWorkbenchPy::appendToolbar(PyObject *args)
{
    PY_TRY {
        PyObject* pObject;
        char* psToolBar;
        if ( !PyArg_ParseTuple(args, "sO", &psToolBar, &pObject) )
            return NULL;                             // NULL triggers exception 
        if (!PyList_Check(pObject)) {
            PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
            return NULL;                             // NULL triggers exception 
        }

        QStringList items;
        int nSize = PyList_Size(pObject);
        for (int i=0; i<nSize;++i) {
            PyObject* item = PyList_GetItem(pObject, i);
            if (!PyString_Check(item))
                continue;
            char* pItem = PyString_AsString(item);
            items.push_back(pItem);
        }

        getPythonWorkbenchObject()->appendToolbar( psToolBar, items );

        Py_Return; 
    } PY_CATCH;
}

/** Removes a toolbar */
PyObject*  PythonWorkbenchPy::removeToolbar(PyObject *args)
{
    PY_TRY {
        char *psToolBar;
        if (!PyArg_ParseTuple(args, "s", &psToolBar))     // convert args: Python->C 
            return NULL;                             // NULL triggers exception 
    
        getPythonWorkbenchObject()->removeToolbar( psToolBar );
        Py_Return; 
    } PY_CATCH;
}

/** Shows a list of all toolbars */
PyObject*  PythonWorkbenchPy::listToolbars(PyObject *args)
{
    PY_TRY {
        QStringList bars = getPythonWorkbenchObject()->listToolbars();

        PyObject* pyList = PyList_New(bars.count());
        int i=0;
        for ( QStringList::Iterator it = bars.begin(); it != bars.end(); ++it, ++i ) {
            PyObject* str = PyString_FromString((const char*)(*it).toAscii());
            PyList_SetItem(pyList, i, str);
        }
        return pyList; 
    } PY_CATCH;
}

/** Appends a new command bar */
PyObject*  PythonWorkbenchPy::appendCommandbar(PyObject *args)
{
    PY_TRY {
        PyObject* pObject;
        char* psToolBar;
        if ( !PyArg_ParseTuple(args, "sO", &psToolBar, &pObject) )
            return NULL;                             // NULL triggers exception 
        if (!PyList_Check(pObject)) {
            PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
            return NULL;                             // NULL triggers exception 
        }

        QStringList items;
        int nSize = PyList_Size(pObject);
        for (int i=0; i<nSize;++i) {
            PyObject* item = PyList_GetItem(pObject, i);
            if (!PyString_Check(item))
                continue;
            char* pItem = PyString_AsString(item);
            items.push_back(pItem);
        }

        getPythonWorkbenchObject()->appendCommandbar( psToolBar, items );

        Py_Return; 
    } PY_CATCH;
}

/** Removes a command bar */
PyObject*  PythonWorkbenchPy::removeCommandbar(PyObject *args)
{
    PY_TRY {
        char *psToolBar;
        if (!PyArg_ParseTuple(args, "s", &psToolBar))     // convert args: Python->C 
            return NULL;                             // NULL triggers exception 
    
        getPythonWorkbenchObject()->removeCommandbar( psToolBar );
        Py_Return; 
    } PY_CATCH;
}

/** Shows a list of all command bars */
PyObject*  PythonWorkbenchPy::listCommandbars(PyObject *args)
{
    PY_TRY {
        QStringList bars = getPythonWorkbenchObject()->listCommandbars();

        PyObject* pyList = PyList_New(bars.count());
        int i=0;
        for ( QStringList::Iterator it = bars.begin(); it != bars.end(); ++it, ++i ) {
            PyObject* str = PyString_FromString((const char*)(*it).toAscii());
            PyList_SetItem(pyList, i, str);
        }
        return pyList; 
    } PY_CATCH;
}

PyObject *PythonWorkbenchPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int PythonWorkbenchPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


