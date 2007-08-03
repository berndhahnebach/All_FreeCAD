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
#include "WorkbenchManager.h"

// inclusion of the generated files (generated out of WorkbenchPy.xml)
#include "WorkbenchPy.h"
#include "WorkbenchPy.cpp"

using namespace Gui;

/** @class WorkbenchPy
 * The workbench Python base class doesn't allow you to manipulate the C++ workbench class behind.
 * You're only allowed either to activate the workbench class or get its name.
 * The WorkbenchPy class is associated to all C++ workbench classes except of PythonWorkbench.
 * @see Workbench
 * @see PythonWorkbench
 * @see PythonWorkbenchPy
 * @author Werner Mayer
 */

/** Retrieves the workbench name */
PyObject*  WorkbenchPy::name(PyObject *args)
{
    PY_TRY {
        QString name = getWorkbenchObject()->name(); 
        PyObject* pyName = PyString_FromString( name.toLatin1() );
        return pyName;
    }PY_CATCH;
}

/** Activates the workbench object */
PyObject*  WorkbenchPy::activate(PyObject *args)
{
    PY_TRY {
        QString name = getWorkbenchObject()->name(); 
        WorkbenchManager::instance()->activate( name, getWorkbenchObject()->getTypeId().getName() );
        Py_Return; 
    }PY_CATCH;
}

PyObject *WorkbenchPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int WorkbenchPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


