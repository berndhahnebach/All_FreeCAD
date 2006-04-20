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
# include <qinputdialog.h>
#endif

#include "PythonConsolePy.h"
#include "PythonConsole.h"

#include <Base/Console.h>
#include <Base/Exception.h>

using namespace Gui;

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------
PyTypeObject PythonStdoutPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "PythonStdout",       /*tp_name*/
  sizeof(PythonStdoutPy), /*tp_basicsize*/
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
PyMethodDef PythonStdoutPy::Methods[] = {
  PYMETHODEDEF(write)
  PYMETHODEDEF(flush)
  {NULL, NULL}          /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PythonStdoutPy::Parents[] = {&PyObjectBase::Type, NULL};     

PythonStdoutPy::PythonStdoutPy(PythonConsole *con, PyTypeObject *T)
 : PyObjectBase( T), pyConsole(con)
{
}

PythonStdoutPy::~PythonStdoutPy()
{
}

PyObject *PythonStdoutPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  return 0;
}

//--------------------------------------------------------------------------
// WorkbenchPy representation
//--------------------------------------------------------------------------
PyObject *PythonStdoutPy::_repr(void)
{
  return Py_BuildValue("s", "PythonStdout");
}

//--------------------------------------------------------------------------
// WorkbenchPy Attributes
//--------------------------------------------------------------------------
PyObject *PythonStdoutPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{
  _getattr_up(PyObjectBase); 
} 

int PythonStdoutPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  return PyObjectBase::_setattr(attr, value); 	// send up to parent
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
PYFUNCIMP_D(PythonStdoutPy,write)
{
  char *output;
  if (!PyArg_ParseTuple(args, "s", &output))     // convert args: Python->C 
      return Py_None;                            // Do not provok error messages 
  pyConsole->insertPythonOutput( output );
  return Py_None;
} 

PYFUNCIMP_D(PythonStdoutPy,flush)
{
  return Py_None;
} 

// -------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------
PyTypeObject PythonStderrPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "PythonStderr",       /*tp_name*/
  sizeof(PythonStderrPy), /*tp_basicsize*/
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
PyMethodDef PythonStderrPy::Methods[] = {
  PYMETHODEDEF(write)
  PYMETHODEDEF(flush)
  {NULL, NULL}          /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PythonStderrPy::Parents[] = {&PyObjectBase::Type, NULL};     

PythonStderrPy::PythonStderrPy(PythonConsole *con, PyTypeObject *T)
 : PyObjectBase( T), pyConsole(con)
{
}

PythonStderrPy::~PythonStderrPy()
{
}

PyObject *PythonStderrPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  return 0;
}

//--------------------------------------------------------------------------
// WorkbenchPy representation
//--------------------------------------------------------------------------
PyObject *PythonStderrPy::_repr(void)
{
  return Py_BuildValue("s", "PythonStderr");
}

//--------------------------------------------------------------------------
// WorkbenchPy Attributes
//--------------------------------------------------------------------------
PyObject *PythonStderrPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{
  _getattr_up(PyObjectBase); 
} 

int PythonStderrPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  return PyObjectBase::_setattr(attr, value); 	// send up to parent
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
PYFUNCIMP_D(PythonStderrPy,write)
{
  char *output;
  if (!PyArg_ParseTuple(args, "s", &output))     // convert args: Python->C 
      return Py_None;                            // Do not provok error messages 
  pyConsole->insertPythonError( output );
  return Py_None;
} 

PYFUNCIMP_D(PythonStderrPy,flush)
{
  return Py_None;
} 

// -------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------
PyTypeObject PythonStdinPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "PythonStdin",        /*tp_name*/
  sizeof(PythonStdinPy),  /*tp_basicsize*/
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
PyMethodDef PythonStdinPy::Methods[] = {
  PYMETHODEDEF(readline)
  {NULL, NULL}          /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PythonStdinPy::Parents[] = {&PyObjectBase::Type, NULL};     

PythonStdinPy::PythonStdinPy(PythonConsole *con, PyTypeObject *T)
 : PyObjectBase( T), pyConsole(con)
{
}

PythonStdinPy::~PythonStdinPy()
{
}

PyObject *PythonStdinPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  return 0;
}

//--------------------------------------------------------------------------
// WorkbenchPy representation
//--------------------------------------------------------------------------
PyObject *PythonStdinPy::_repr(void)
{
  return Py_BuildValue("s", "PythonStdin");
}

//--------------------------------------------------------------------------
// WorkbenchPy Attributes
//--------------------------------------------------------------------------
PyObject *PythonStdinPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{
  _getattr_up(PyObjectBase); 
} 

int PythonStdinPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  return PyObjectBase::_setattr(attr, value); 	// send up to parent
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
PYFUNCIMP_D(PythonStdinPy,readline)
{
  QString txt = QInputDialog::getText ( "Python Input Dialog", "Input for Python:", QLineEdit::Normal, QString::null, 0, 
                                        pyConsole, "PyInput" );
  return Py_BuildValue("s", txt.latin1());
} 
