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
#include <Mod/Part/App/TopologyPy.h>


#include "MeshPy.h"
#include "MeshFeature.h"
#include "MeshFeaturePy.h"

using Base::Console;
using namespace Mesh;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject MeshFeaturePy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "MeshFeaturePy",        /*tp_name*/
  sizeof(MeshFeaturePy),  /*tp_basicsize*/
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
  0,                                                /*tp_str  */
  0,                                                /*tp_getattro*/
  0,                                                /*tp_setattro*/
  /* --- Functions to access object as input/output buffer ---------*/
  0,                                                /* tp_as_buffer */
  /* --- Flags to define presence of optional/expanded features */
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "About PyObjectBase",                             /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  0,                                                /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &App::FeaturePy::Type,                            /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  0,                                                /*tp_init */
  0,                                                /*tp_alloc */
  0,                                                /*tp_new */
  0,                                                /*tp_free   Low-level free-memory routine */
  0,                                                /*tp_is_gc  For PyObject_IS_GC */
  0,                                                /*tp_bases */
  0,                                                /*tp_mro    method resolution order */
  0,                                                /*tp_cache */
  0,                                                /*tp_subclasses */
  0                                                 /*tp_weaklist */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef MeshFeaturePy::Methods[] = {
// PyObjectBase
  PYMETHODEDEF(isA)
// FeaturePy 
	PYMETHODEDEF(setModified)
	PYMETHODEDEF(setModifiedView)
	PYMETHODEDEF(isValid)
// MeshFeaturePy
  PYMETHODEDEF(getMesh)
  PYMETHODEDEF(getMeshCopy)
  PYMETHODEDEF(setMesh)

  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject MeshFeaturePy::Parents[] = {&PyObjectBase::Type,&App::FeaturePy::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
MeshFeaturePy::MeshFeaturePy(Feature *pcFeature, PyTypeObject *T)
  : App::FeaturePy(pcFeature, T),
    _pcFeature(pcFeature),
    _pcMeshPy(0)
{
  Base::Console().Log("Create MeshFeaturePy: %p \n",this);
}

PyObject *MeshFeaturePy::PyMake(PyObject *ignored, PyObject *args)  // Python wrapper
{
  //return new MeshFeaturePy(name, n, tau, gamma);      // Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
MeshFeaturePy::~MeshFeaturePy()           // Everything handled in parent
{
  Base::Console().Log("Destroy MeshFeaturePy: %p \n",this);

  if( _pcMeshPy) _pcMeshPy->DecRef();

} 

//--------------------------------------------------------------------------
// MeshFeaturePy representation
//--------------------------------------------------------------------------
PyObject *MeshFeaturePy::_repr(void)
{
  std::stringstream a;
  a << _pcFeature->getTypeId().getName() << " feature: [ ";
  a << "]" << std::endl;
  return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// MeshFeaturePy Attributes
//--------------------------------------------------------------------------
PyObject *MeshFeaturePy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
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

int MeshFeaturePy::_setattr(char *attr, PyObject *value) // __setattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "XXXX"))           // settable new state
    return 1;
  else 
    return FeaturePy::_setattr(attr, value); 
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(MeshFeaturePy,getMesh)
{
  if(! _pcMeshPy)
  {
    _pcMeshPy = new MeshPy(&(_pcFeature->Mesh.getValue()),true);

    /* We must NOT instanciating the object only one time without incrementing it every 
       time we return it. Otherwise this leads to an error in the following cases:

       1. f=App.document().GetActiveFeature()
          f.getMesh().calculateVertexCurvature() # Creates the object and increments it. 
                                                 # By using it temporarily Python decrements
                                                 # it again
          f.getMesh().calculateVertexCurvature() # Okay, but the object gets decremented again
                                                 # and gets then destroyed
          f.getMesh().calculateVertexCurvature() # Error: Access violation

       2. a=f.getMesh()                          # Creates the object and increments
          b=f.getMesh()                          # Just assigns the object
          c=f.getMesh()                          # Just assigns the object
          a=0                                    # Decrements the object
          b=0                                    # Deletes the object
          dir(c)                                 # Error: Access violation

       To avoid these problems either create the object only once and increment it every time 
       we returns it or create always a new object. This does NOT lead to a memory leak as long 
       as we reset the variable the object is assigned to.
       Werner Mayer
     */
  }
  
  // keeps the object alive
  _pcMeshPy->IncRef();
  
  return _pcMeshPy;
}

PYFUNCIMP_D(MeshFeaturePy,getMeshCopy)
{
   return new MeshPy(&(_pcFeature->Mesh.getValue()),false);
}

PYFUNCIMP_D(MeshFeaturePy,setMesh)
{
 	MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  // copy in the Feature Mesh
  _pcFeature->Mesh.setValue(*(pcObject->getMesh()));
  // and set the python object of this feature
  if(_pcMeshPy)
    _pcMeshPy->setMesh(&(_pcFeature->Mesh.getValue()));

  Py_Return;
}

