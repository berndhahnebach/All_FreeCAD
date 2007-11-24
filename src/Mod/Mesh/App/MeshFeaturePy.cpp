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
#endif

#include <Base/Console.h>
#include <Base/Handle.h>

#include "Core/Evaluation.h"
#include "MeshPy.h"
#include "MeshFeature.h"
#include "MeshFeaturePy.h"

using namespace Mesh;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject MeshFeaturePy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                                                /*ob_size*/
  "Mesh.Mesh::Feature",                             /*tp_name*/
  sizeof(MeshFeaturePy),                            /*tp_basicsize*/
  0,                                                /*tp_itemsize*/
                                                    /* methods */
  PyDestructor,                                     /*tp_dealloc*/
  0,                                                /*tp_print*/
  __getattr,                                        /*tp_getattr*/
  __setattr,                                        /*tp_setattr*/
  0,                                                /*tp_compare*/
  __repr,                                           /*tp_repr*/
  0,                                                /*tp_as_number*/
  0,                                                /*tp_as_sequence*/
  0,                                                /*tp_as_mapping*/
  0,                                                /*tp_hash*/
  0,                                                /*tp_call */
  0,                                                /*tp_str  */
  0,                                                /*tp_getattro*/
  0,                                                /*tp_setattro*/
  /* --- Functions to access object as input/output buffer ---------*/
  0,                                                /* tp_as_buffer */
  /* --- Flags to define presence of optional/expanded features */
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "The Mesh::Feature class handles meshes.\n"
  "The Mesh.MeshFeature() function is for internal use only and cannot be used to create instances of this class.\n"
  "Therefore you must have a reference to a document, e.g. 'd' then you can create an instance with "
  "d.addObject(\"Mesh::Feature\").",                 /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  MeshFeaturePy::Methods,                           /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &App::DocumentObjectPy::Type,                     /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  0,                                                /*tp_init */
  0,                                                /*tp_alloc */
  PyMake,                                           /*tp_new */
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
// MeshFeaturePy
  PYMETHODEDEF(countPoints)
  PYMETHODEDEF(countFacets)
  PYMETHODEDEF(hasNonUnifomOrientedFacets)
  PYMETHODEDEF(countNonUnifomOrientedFacets)
  PYMETHODEDEF(isSolid)
  PYMETHODEDEF(hasNonManifolds)

  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject MeshFeaturePy::Parents[] = {&PyObjectBase::Type, &App::DocumentObjectPy::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
MeshFeaturePy::MeshFeaturePy(Feature *pcFeature, PyTypeObject *T)
  : App::DocumentObjectPy(pcFeature, T), _pcFeature(pcFeature)
{
  Base::Console().Log("Create Mesh::Feature: %p \n",this);
}

PyObject *MeshFeaturePy::PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds)  // Python wrapper
{
  PyErr_SetString(PyExc_RuntimeError, "You cannot create directly an instance of 'Mesh::Feature'.\n"
    "You must have created a document where you can create an instance with addObject(\"Mesh::Feature\").");
  return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
MeshFeaturePy::~MeshFeaturePy()           // Everything handled in parent
{
  Base::Console().Log("Destroy Mesh::Feature: %p \n",this);
} 

//--------------------------------------------------------------------------
// MeshFeaturePy representation
//--------------------------------------------------------------------------
PyObject *MeshFeaturePy::_repr(void)
{
  std::stringstream a;
  a << _pcFeature->getTypeId().getName() << " object at " << _pcFeature;
  return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// MeshFeaturePy Attributes
//--------------------------------------------------------------------------
PyObject *MeshFeaturePy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{ 
  try{
    _getattr_up(DocumentObjectPy);
  }catch(...){
    Py_Error(PyExc_Exception,"Error in get Attribute");
  }
} 

int MeshFeaturePy::_setattr(char *attr, PyObject *value) // __setattr__ function: note only need to handle new state
{ 
  return DocumentObjectPy::_setattr(attr, value); 
} 

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(MeshFeaturePy,countPoints)
{
  return Py_BuildValue("i",_pcFeature->Mesh.getValue().countPoints()); 
}

PYFUNCIMP_D(MeshFeaturePy,countFacets)
{
  return Py_BuildValue("i",_pcFeature->Mesh.getValue().countFacets()); 
}

PYFUNCIMP_D(MeshFeaturePy,hasNonUnifomOrientedFacets)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = _pcFeature->Mesh.getValue().countNonUnifomOrientedFacets() > 0;
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PYFUNCIMP_D(MeshFeaturePy,countNonUnifomOrientedFacets)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    unsigned long count = _pcFeature->Mesh.getValue().countNonUnifomOrientedFacets();
    return Py_BuildValue("k", count); 
}

PYFUNCIMP_D(MeshFeaturePy,isSolid)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = _pcFeature->Mesh.getValue().isSolid();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PYFUNCIMP_D(MeshFeaturePy,hasNonManifolds)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = _pcFeature->Mesh.getValue().hasNonManifolds();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

