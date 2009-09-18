/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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
# include <Python.h>
#endif

#include <Base/Console.h>
#include <Base/VectorPy.h>

#include "RobotAlgos.h"

using namespace Robot;

static PyObject * 
test(PyObject *self, PyObject *args)
{
 //   PyObject *pcObjShape;
 //   PyObject *pcObjDir=0;

	//if (!PyArg_ParseTuple(args, "O!|O!", &(TopoShapePy::Type), &pcObjShape,&(Base::VectorPy::Type), &pcObjDir))     // convert args: Python->C
 //       return NULL;                             // NULL triggers exception

    PY_TRY {
		RobotAlgos Alg;

		Alg.Test();

    } PY_CATCH;

	return Py::new_reference_to(Py::Float(0.0));

}


/* registration table  */
struct PyMethodDef Robot_methods[] = {
   {"test"       ,test      ,METH_VARARGS,
     "void test(void) - runs test code for development."},
    {NULL, NULL}        /* end of table marker */
};
