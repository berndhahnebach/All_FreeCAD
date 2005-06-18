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
#include <Base/FileInfo.h>

using Base::Console;


#include "MeshPy.h"
#include "Mesh.h"
#include "MeshAlgos.h"
#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Stream.h"


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
  PYMETHODEDEF(pointCount)
  PYMETHODEDEF(faceCount)
  PYMETHODEDEF(write)
  PYMETHODEDEF(offset)
  PYMETHODEDEF(calcVertexNormales)
  PYMETHODEDEF(Union)
  PYMETHODEDEF(coarsen)
  PYMETHODEDEF(translate)
  PYMETHODEDEF(rotate)
  PYMETHODEDEF(scale)
  PYMETHODEDEF(addFacet)
  PYMETHODEDEF(clear)
  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject MeshPy::Parents[] = {&Base::PyObjectBase::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
MeshPy::MeshPy(MeshWithProperty *pcMesh, PyTypeObject *T)
: Base::PyObjectBase(T), _pcMesh(pcMesh)
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
  delete _pcMesh;
} 

//--------------------------------------------------------------------------
// MeshPy representation
//--------------------------------------------------------------------------
PyObject *MeshPy::_repr(void)
{
  std::stringstream a;
  a << "Mesh: ["
    << _pcMesh->getKernel()->CountFacets()
    << " Faces, "
    << _pcMesh->getKernel()->CountPoints()
    << " Points"
    << "]" << std::endl;
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


void MeshPy::setMesh(MeshWithProperty *pcMesh)
{
  _pcMesh = pcMesh;
}


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------


PYFUNCIMP_D(MeshPy,pointCount)
{
  return Py_BuildValue("i",_pcMesh->getKernel()->CountPoints()); 
}

PYFUNCIMP_D(MeshPy,faceCount)
{
  return Py_BuildValue("i",_pcMesh->getKernel()->CountFacets()); 
}

PYFUNCIMP_D(MeshPy,write)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

/*  Base::FileInfo File(Name);
  
  // checking on the file
  if(File.exists() && !File.isWritable())
    Py_Error(PyExc_Exception,"File not writable");
*/
  PY_TRY {
    MeshAlgos::writeBin(_pcMesh,Name);
/*    MeshSTL aReader(*(_pcMesh->getKernel()) );

    // read STL file
    FileStream str( File.filePath().c_str(), std::ios::out);
    if ( !aReader.SaveBinary( str ) )
      Py_Error(PyExc_Exception,"STL write failed to write");
*/
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,offset)
{
  double Float;
  if (! PyArg_ParseTuple(args, "d",&Float))			 
    return NULL;                         

  PY_TRY {
    MeshAlgos::offset(_pcMesh,Float);  
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,coarsen)
{
  double Float;
  if (! PyArg_ParseTuple(args, "d",&Float))			 
    return NULL;                         

  PY_TRY {
    MeshAlgos::coarsen(_pcMesh,Float);  
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,calcVertexNormales)
{
  PY_TRY {
    MeshAlgos::calcVertexNormales(_pcMesh);  
  } PY_CATCH;
  
  Py_Return;

}

PYFUNCIMP_D(MeshPy,Union)
{
 	MeshPy* pcObject;
  if (!PyArg_ParseTuple(args, "O!", &MeshPy::Type, &pcObject))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  PY_TRY {
    MeshWithProperty* m = pcObject->_pcMesh;
    MeshAlgos::Union(_pcMesh,m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,translate)
{
  double x,y,z;
  if (! PyArg_ParseTuple(args, "ddd",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.SetMoveX(x);
    m.SetMoveY(y);
    m.SetMoveZ(z);
    _pcMesh->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,rotate)
{
  double x,y,z;
  if (! PyArg_ParseTuple(args, "ddd",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.SetRotX(x);
    m.SetRotY(y);
    m.SetRotZ(z);
    _pcMesh->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,scale)
{
  double s;
  if (! PyArg_ParseTuple(args, "d",&s))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.SetScaleX(s);
    m.SetScaleY(s);
    m.SetScaleZ(s);
    _pcMesh->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,addFacet)
{
  double x1,y1,z1,x2,y2,z2,x3,y3,z3;
  if (! PyArg_ParseTuple(args, "ddddddddd",&x1,&y1,&z1,&x2,&y2,&z2,&x3,&y3,&z3))			 
    return NULL;                         

  PY_TRY {
    _pcMesh->getKernel()->AddFacet(MeshGeomFacet(Vector3D(x1,y1,z1),
                                                 Vector3D(x2,y2,z2),
                                                 Vector3D(x3,y3,z3)));
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,clear)
{
  PY_TRY {
    _pcMesh->clear();
  } PY_CATCH;

  Py_Return;
}
