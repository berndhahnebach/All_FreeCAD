/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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

#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;


#include "MeshPy.h"

using namespace Mesh;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject MeshPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "MeshPy",        /*tp_name*/
  sizeof(MeshPy),  /*tp_basicsize*/
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
PyMethodDef MeshPy::Methods[] = {
//  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},
//  PYMETHODEDEF(getShape)
  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject MeshPy::Parents[] = {&Base::PyObjectBase::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
MeshPy::MeshPy(MeshFeature *pcFeature, PyTypeObject *T)
: Base::PyObjectBase(T), _pcFeature(pcFeature)
{
  Base::Console().Log("Create MeshPy: %p \n",this);
}

PyObject *MeshPy::PyMake(PyObject *ignored, PyObject *args)  // Python wrapper
{
  //return new MeshPy(name, n, tau, gamma);      // Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
MeshPy::~MeshPy()           // Everything handled in parent
{
  Base::Console().Log("Destroy MeshPy: %p \n",this);
} 

//--------------------------------------------------------------------------
// MeshPy representation
//--------------------------------------------------------------------------
PyObject *MeshPy::_repr(void)
{
  std::stringstream a;
  a << "Mesh: [ ";
  a << "]" << std::endl;
  return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// MeshPy Attributes
//--------------------------------------------------------------------------
PyObject *MeshPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{ 
  try{
    if (Base::streq(attr, "XXXX"))
      return Py_BuildValue("i",1); 
    else
        _getattr_up(PyObjectBase);
  }catch(...){
    Py_Error(PyExc_Exception,"Error in get Attribute");
  }
} 

int MeshPy::_setattr(char *attr, PyObject *value) // __setattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "XXXX"))           // settable new state
    return 1;
  else 
    return PyObjectBase::_setattr(attr, value); 
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

/*
PYFUNCIMP_D(MeshPy,getShape)
{
  Py_Return;
}
*/
