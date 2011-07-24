/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include "TaskDialogPython.h"

// inclusion of the generated files (generated out of VectorPy.xml)
#include "TaskDialogPy.h"
#include "TaskDialogPy.cpp"

using namespace Base;
using namespace Gui;
using namespace Gui::TaskView;


PyObject *TaskDialogPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TaskDialogPy and the Twin object 
    return new TaskDialogPy(new TaskDialogPython());
}

// constructor method
int TaskDialogPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

// returns a string which represents the object e.g. when printed in python
std::string TaskDialogPy::representation(void) const
{
    return std::string("<TaskDialog object>");
}

PyObject* TaskDialogPy::accept(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject *TaskDialogPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int TaskDialogPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}

