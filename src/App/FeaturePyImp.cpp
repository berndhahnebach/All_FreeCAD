/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2007     *
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

#include "Feature.h"

// inclusion of the generated files (generated out of FeaturePy.xml)
#include "FeaturePy.h"
#include "FeaturePy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *FeaturePy::representation(void) const
{
    return "<Feature object>";
}


PyObject*  FeaturePy::recompute(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
    
    PY_TRY {
        //getAbstractFeaturePtr()->recompute();
        Py_Return;
    } PY_CATCH;
}

Py::Int FeaturePy::getValid(void) const
{
    return Py::Int(getAbstractFeaturePtr()->isValid());
}

PyObject *FeaturePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int FeaturePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
