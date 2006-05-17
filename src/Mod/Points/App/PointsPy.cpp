/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
#include <sstream>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include "PointsPy.h"
#include "Points.h"
#include "PointsAlgos.h"

using Base::Console;
using namespace Points;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject PointsPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "Points",        /*tp_name*/
  sizeof(PointsPy),  /*tp_basicsize*/
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
PyMethodDef PointsPy::Methods[] = {
  PYMETHODEDEF(count)
  PYMETHODEDEF(read)
  PYMETHODEDEF(write)
  PYMETHODEDEF(translate)
  PYMETHODEDEF(rotate)
  PYMETHODEDEF(scale)
  PYMETHODEDEF(addPoint)
  PYMETHODEDEF(clear)
  PYMETHODEDEF(copy)
  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PointsPy::Parents[] = {&Base::PyObjectBase::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
PointsPy::PointsPy(const PointsWithProperty& rcPoints, PyTypeObject *T)
: Base::PyObjectBase(T), _pcPoints(new PointsWithProperty())
{
  Base::Console().Log("Create PointsPy: %p \n",this);
  (*_pcPoints) = rcPoints;
}

PointsPy::PointsPy(PyTypeObject *T)
: Base::PyObjectBase(T), _pcPoints(new PointsWithProperty())
{
  Base::Console().Log("Create PointsPy: %p \n",this);
}

PyObject *PointsPy::PyMake(PyObject *ignored, PyObject *args)  // Python wrapper
{
  //return new PointsPy(name, n, tau, gamma);      // Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
PointsPy::~PointsPy()           // Everything handled in parent
{
  Base::Console().Log("Destroy PointsPy: %p \n",this);
  delete _pcPoints;
} 

//--------------------------------------------------------------------------
// PointsPy representation
//--------------------------------------------------------------------------
PyObject *PointsPy::_repr(void)
{
  std::stringstream a;
  a << _pcPoints->getKernel().size() << " points";
  return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// PointsPy Attributes
//--------------------------------------------------------------------------
PyObject *PointsPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
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

int PointsPy::_setattr(char *attr, PyObject *value) // __setattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "XXXX"))           // settable new state
    return 1;
  else 
    return PyObjectBase::_setattr(attr, value); 
}

void PointsPy::setPoints(const PointsWithProperty& rcPoints)
{
  (*_pcPoints) = rcPoints;
}

const PointsWithProperty& PointsPy::getPoints(void) const
{
  return *_pcPoints;
}

PointsWithProperty& PointsPy::refToPoints(void)
{
  return *_pcPoints;
}

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(PointsPy,count)
{
  return Py_BuildValue("i",_pcPoints->getKernel().size()); 
}

PYFUNCIMP_D(PointsPy,write)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
  PY_TRY {
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(PointsPy,read)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  Base::FileInfo File(Name);
  
  // checking on the file
  if(!File.isReadable())
    Py_Error(PyExc_Exception,"File to load not existing or not readable");

  PY_TRY {
    PointsAlgos::Load(*_pcPoints,Name);
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(PointsPy,translate)
{
  float x,y,z;
  if (! PyArg_ParseTuple(args, "fff",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.move(x,y,z);
    _pcPoints->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(PointsPy,rotate)
{
  float x,y,z;
  if (! PyArg_ParseTuple(args, "fff",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.rotX(x);
    m.rotY(y);
    m.rotZ(z);
    _pcPoints->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(PointsPy,scale)
{
  float s;
  if (! PyArg_ParseTuple(args, "f",&s))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.scale(s,s,s);
    _pcPoints->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(PointsPy,addPoint)
{
  float x,y,z;
  if (! PyArg_ParseTuple(args, "fff",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    _pcPoints->getKernel().push_back(Base::Vector3f(x,y,z));
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(PointsPy,clear)
{
  PY_TRY {
    _pcPoints->clear();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(PointsPy,copy)
{
  PY_TRY {
   return new PointsPy(*_pcPoints);
  } PY_CATCH;
}
