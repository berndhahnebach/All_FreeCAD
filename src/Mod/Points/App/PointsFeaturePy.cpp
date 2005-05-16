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
# include <TDF_Label.hxx>
# include <TDF_ChildIterator.hxx>
# include <TDF_Tool.hxx>
# include <TCollection_AsciiString.hxx>
# include <TDF_ListIteratorOfAttributeList.hxx>
# include <TFunction_Logbook.hxx>
# include <TFunction_DriverTable.hxx>
# include <TFunction_Function.hxx>
# include <TNaming_Builder.hxx>
# include <TNaming_NamedShape.hxx>
# include <TopoDS_Shape.hxx>
# include <Standard_GUID.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;
#include <App/Topology.h>


#include "PointsFeature.h"
#include "PointsFeaturePy.h"

using namespace Points;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject PointsFeaturePy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "PointsFeaturePy",        /*tp_name*/
  sizeof(PointsFeaturePy),  /*tp_basicsize*/
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
PyMethodDef PointsFeaturePy::Methods[] = {
//  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},
  PYMETHODEDEF(getShape)
  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PointsFeaturePy::Parents[] = {&PyObjectBase::Type,&App::FeaturePy::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
PointsFeaturePy::PointsFeaturePy(PointsFeature *pcFeature, PyTypeObject *T)
: App::FeaturePy(pcFeature, T), _pcFeature(pcFeature)
{
  Base::Console().Log("Create PointsFeaturePy: %p \n",this);
}

PyObject *PointsFeaturePy::PyMake(PyObject *ignored, PyObject *args)  // Python wrapper
{
  //return new PointsFeaturePy(name, n, tau, gamma);      // Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
PointsFeaturePy::~PointsFeaturePy()           // Everything handled in parent
{
  Base::Console().Log("Destroy PointsFeaturePy: %p \n",this);
} 

//--------------------------------------------------------------------------
// PointsFeaturePy representation
//--------------------------------------------------------------------------
PyObject *PointsFeaturePy::_repr(void)
{
  std::stringstream a;
  a << "PointsFeature: [ ";
  a << "]" << std::endl;
  return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// PointsFeaturePy Attributes
//--------------------------------------------------------------------------
PyObject *PointsFeaturePy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{ 
  try{
    if (Base::streq(attr, "XXXX"))
      return Py_BuildValue("i",1); 
    else
        _getattr_up(FeaturePy);
  }catch(...){
    Py_Error(PyExc_Exception,"Error in get Attribute");
  }
} 

int PointsFeaturePy::_setattr(char *attr, PyObject *value) // __setattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "XXXX"))           // settable new state
    return 1;
  else 

    return FeaturePy::_setattr(attr, value); 
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(PointsFeaturePy,getShape)
{
  Py_Return;
}

