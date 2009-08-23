/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <Inventor/nodes/SoSeparator.h>
#endif

#include "ViewProvider.h"

// inclusion of the generated files (generated out of ViewProviderPy2.xml)
#include "ViewProviderPy.h"
#include "ViewProviderPy.cpp"
#include <Base/Interpreter.h>

using namespace Gui;

// returns a string which represent the object e.g. when printed in python
const char *ViewProviderPy::representation(void) const
{
    return "<View provider object>";
}

PyObject*  ViewProviderPy::show(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    PY_TRY {
        getViewProviderPtr()->show();  
        Py_Return;
    } PY_CATCH;
}

PyObject*  ViewProviderPy::hide(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    PY_TRY {
        getViewProviderPtr()->hide();  
        Py_Return;
    } PY_CATCH;
}

PyObject*  ViewProviderPy::isVisible(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    PY_TRY {
        return Py_BuildValue("O", (getViewProviderPtr()->isShow() ? Py_True : Py_False));
    } PY_CATCH;
}

PyObject*  ViewProviderPy::listDisplayModes(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    PY_TRY {
        std::vector<std::string> modes = getViewProviderPtr()->getDisplayModes();  
        PyObject* pyList = PyList_New(modes.size()); 
        int i=0;

        for ( std::vector<std::string>::iterator it = modes.begin(); it != modes.end(); ++it ) {
            PyObject* str = PyString_FromString(it->c_str());
            PyList_SetItem(pyList, i++, str);
        }

        return pyList;
    } PY_CATCH;
}

PyObject*  ViewProviderPy::toString(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                     // NULL triggers exception 
    PY_TRY {
        std::string buffer = getViewProviderPtr()->toString();
        return Py::new_reference_to(Py::String(buffer));
    } PY_CATCH;
}

PyObject *ViewProviderPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int ViewProviderPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}

Py::Object ViewProviderPy::getAnnotation(void) const
{
    try {
        SoNode* node = getViewProviderPtr()->getAnnotation();
        PyObject* Ptr = Base::Interpreter().createSWIGPointerObj("SoSeparator *", node, 1);
        node->ref();
        return Py::Object(Ptr, true);
    }
    catch (const Base::Exception& e) {
        throw Py::Exception(e.what());
    }
}

void  ViewProviderPy::setAnnotation(Py::Object arg)
{

}

Py::Object ViewProviderPy::getRootNode(void) const
{
    try {
        SoNode* node = getViewProviderPtr()->getRoot();
        PyObject* Ptr = Base::Interpreter().createSWIGPointerObj("SoSeparator *", node, 1);
        node->ref();
        return Py::Object(Ptr, true);
    }
    catch (const Base::Exception& e) {
        throw Py::Exception(e.what());
    }
}

void  ViewProviderPy::setRootNode(Py::Object arg)
{

}
