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

#include "DocumentObjectGroup.h"

// inclusion of the generated files (generated out of DocumentObjectGroupPy.xml)
#include "DocumentObjectGroupPy.h"
#include "DocumentObjectGroupPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *DocumentObjectGroupPy::representation(void)
{
	return "DocumentObjectGroupPy";
}


PyObject*  DocumentObjectGroupPy::addObject(PyObject *args)
{
    char *sType,*sName=0;
    if (!PyArg_ParseTuple(args, "s|s", &sType,&sName))     // convert args: Python->C
        return NULL;

    DocumentObject *pcObj = getDocumentObjectGroupObject()->addObject(sType, sName);
    if ( pcObj ) {
        return pcObj->getPyObject();
    } else {
        char szBuf[200];
        snprintf(szBuf, 200, "Cannot create object of type '%s'", sType);
        Py_Error(PyExc_Exception,szBuf);
    }
}

PyObject*  DocumentObjectGroupPy::removeObject(PyObject *args)
{
    char* pcName;
    if (!PyArg_ParseTuple(args, "s", &pcName)) 
        return false;

    getDocumentObjectGroupObject()->removeObject(pcName);

    Py_Return;
}

PyObject*  DocumentObjectGroupPy::getObject(PyObject *args)
{
    char* pcName;
    if (!PyArg_ParseTuple(args, "s", &pcName))     // convert args: Python->C 
        return NULL;                    // NULL triggers exception 

    DocumentObject* obj = getDocumentObjectGroupObject()->getObject(pcName);
    if ( obj ) {
        return obj->getPyObject();
    } else {
        Py_Return;
    }
}

PyObject*  DocumentObjectGroupPy::hasObject(PyObject *args)
{
    char* pcName;
    if (!PyArg_ParseTuple(args, "s", &pcName))     // convert args: Python->C 
        return NULL;                    // NULL triggers exception 

    DocumentObject* obj = getDocumentObjectGroupObject()->getObject(pcName);

    if ( obj ) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject *DocumentObjectGroupPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int DocumentObjectGroupPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}

