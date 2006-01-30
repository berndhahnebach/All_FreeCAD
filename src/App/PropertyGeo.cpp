/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
#	include <assert.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......

#include <Base/Exception.h>
#include <Base/Writer.h>

#include "VectorPy.h"
#include "MatrixPy.h"

#include "PropertyGeo.h"

using namespace App;
using namespace Base;
using namespace std;




//**************************************************************************
//**************************************************************************
// PropertyVector
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyVector , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyVector::PropertyVector()
{

}


PropertyVector::~PropertyVector()
{

}

//**************************************************************************
// Base class implementer


void PropertyVector::setValue(const Base::Vector3D &vec)
{
	_cVec=vec;
  hasSetValue();
}

void PropertyVector::setValue(float x, float y, float z)
{
	_cVec=Vector3D(x,y,z);
  hasSetValue();
}

const Base::Vector3D & PropertyVector::getValue(void)const 
{
	return _cVec;
}

PyObject *PropertyVector::getPyObject(void)
{
  return new VectorPy(_cVec);
}

void PropertyVector::setPyObject(PyObject *value)
{ 
  if( PyObject_TypeCheck(value, &(VectorPy::Type)) ) {
   	VectorPy  *pcObject = (VectorPy*)value;
    _cVec = pcObject->value();
  }else if(PyTuple_Check(value)&&PyTuple_Size(value)==3) {
    PyObject* item;
    // x
    item = PyTuple_GetItem(value,0);
    if (PyFloat_Check(item))
      _cVec.x = (float)PyFloat_AsDouble(item);
    else if (PyInt_Check(item))
      _cVec.x = (float)PyInt_AsLong(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    // y
    item = PyTuple_GetItem(value,1);
    if (PyFloat_Check(item))
      _cVec.y = (float)PyFloat_AsDouble(item);
    else if (PyInt_Check(item))
      _cVec.y = (float)PyInt_AsLong(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    // z
    item = PyTuple_GetItem(value,2);
    if (PyFloat_Check(item))
      _cVec.z = (float)PyFloat_AsDouble(item);
    else if (PyInt_Check(item))
      _cVec.z = (float)PyInt_AsLong(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
  }else
    throw Base::Exception("Not allowed type used (Vector expected)...");
}

void PropertyVector::Save (Writer &writer)
{
  writer << writer.ind() << "<PropertyLink valueX=\"" <<  _cVec.x << "\" valueY=\"" <<  _cVec.y << "\" valueZ=\"" <<  _cVec.z <<"\"/>" << endl;
}

void PropertyVector::Restore(Base::Reader &reader)
{


}


//**************************************************************************
//**************************************************************************
// PropertyMatrix
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyMatrix , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyMatrix::PropertyMatrix()
{

}


PropertyMatrix::~PropertyMatrix()
{

}

//**************************************************************************
// Base class implementer


void PropertyMatrix::setValue(const Base::Matrix4D &mat)
{
	_cMat=mat;
  hasSetValue();
}


const Base::Matrix4D & PropertyMatrix::getValue(void)const 
{
	return _cMat;
}

PyObject *PropertyMatrix::getPyObject(void)
{
  return new MatrixPy(_cMat);
}

void PropertyMatrix::setPyObject(PyObject *value)
{ 
  if( PyObject_TypeCheck(value, &(MatrixPy::Type)) )
  {
   	MatrixPy  *pcObject = (MatrixPy*)value;
    _cMat = pcObject->value();
  }else
    throw Base::Exception("Not allowed type used (Vector expected)...");
}

void PropertyMatrix::Save (short indent,std::ostream &str)
{
  //str << ind(indent) << "<PropertyLink valueX=\"" <<  _cVec.x << "\" valueY=\"" <<  _cVec.y << "\" valueZ=\"" <<  _cVec.z <<"\"/>" << endl;
}

void PropertyMatrix::Restore(Base::Reader &reader)
{


}

