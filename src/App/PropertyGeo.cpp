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
#include <Base/Reader.h>

#include "VectorPy.h"
#include "MatrixPy.h"
#include "Placement.h"

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


void PropertyVector::setValue(const Base::Vector3f &vec)
{
  aboutToSetValue();
	_cVec=vec;
  hasSetValue();
}

void PropertyVector::setValue(float x, float y, float z)
{
  aboutToSetValue();
	_cVec=Vector3f(x,y,z);
  hasSetValue();
}

const Base::Vector3f & PropertyVector::getValue(void)const
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
    setValue( pcObject->value() );
  }else if(PyTuple_Check(value)&&PyTuple_Size(value)==3) {
    PyObject* item;
    Base::Vector3f cVec;
    // x
    item = PyTuple_GetItem(value,0);
    if (PyFloat_Check(item))
      cVec.x = (float)PyFloat_AsDouble(item);
    else if (PyInt_Check(item))
      cVec.x = (float)PyInt_AsLong(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    // y
    item = PyTuple_GetItem(value,1);
    if (PyFloat_Check(item))
      cVec.y = (float)PyFloat_AsDouble(item);
    else if (PyInt_Check(item))
      cVec.y = (float)PyInt_AsLong(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    // z
    item = PyTuple_GetItem(value,2);
    if (PyFloat_Check(item))
      cVec.z = (float)PyFloat_AsDouble(item);
    else if (PyInt_Check(item))
      cVec.z = (float)PyInt_AsLong(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    setValue( cVec );
  }else
    throw Base::Exception("Not allowed type used (Vector expected)...");
}

void PropertyVector::Save (Writer &writer) const
{
  writer.Stream() << writer.ind() << "<PropertyVector valueX=\"" <<  _cVec.x << "\" valueY=\"" <<  _cVec.y << "\" valueZ=\"" <<  _cVec.z <<"\"/>" << endl;
}

void PropertyVector::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("PropertyVector");
  // get the value of my Attribute
  _cVec.x = (float)reader.getAttributeAsFloat("valueX");
  _cVec.y = (float)reader.getAttributeAsFloat("valueY");
  _cVec.z = (float)reader.getAttributeAsFloat("valueZ");
}


Property *PropertyVector::Copy(void) const
{
  PropertyVector *p= new PropertyVector();
  p->_cVec = _cVec;
  return p;
}

void PropertyVector::Paste(const Property &from)
{
  aboutToSetValue();
  _cVec = dynamic_cast<const PropertyVector&>(from)._cVec;
  hasSetValue();
}


//**************************************************************************
// PropertyVectorList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyVectorList , App::PropertyLists);

//**************************************************************************
// Construction/Destruction

PropertyVectorList::PropertyVectorList()
{

}

PropertyVectorList::~PropertyVectorList()
{

}

//**************************************************************************
// Base class implementer

void PropertyVectorList::setValue(const Base::Vector3f& lValue)
{
  aboutToSetValue();
  _lValueList.resize(1);
	_lValueList[0]=lValue;
  hasSetValue();
}

void PropertyVectorList::setValues(const std::vector<Base::Vector3f>& values)
{
  aboutToSetValue();
  _lValueList = values;
  hasSetValue();
}

PyObject *PropertyVectorList::getPyObject(void)
{
  PyObject* list = PyList_New(	getSize() );

  for(int i = 0;i<getSize(); i++)
     PyList_SetItem( list, i, new VectorPy(	_lValueList[i]));

  return list;
}

void PropertyVectorList::setPyObject(PyObject *value)
{ 
  if(PyList_Check( value) )
  {
    aboutToSetValue();

    int nSize = PyList_Size(value);
    _lValueList.resize(nSize);

    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(value, i);
      try {
        PropertyVector val;
        val.setPyObject( item );
        _lValueList[i] = val.getValue();
      } catch (const Base::Exception& e) {
        _lValueList.resize(1);
        _lValueList[0] = Base::Vector3f();
        throw e;
      }
    }

    hasSetValue();
  }
  else if(PyObject_TypeCheck(value, &(VectorPy::Type)))
  {
    VectorPy  *pcObject = (VectorPy*)value;
    setValue( pcObject->value() );
  }
  else if ( PyTuple_Check(value) && PyTuple_Size(value) == 3 )
  {
    PropertyVector val;
    val.setPyObject( value );
    setValue( val.getValue() );
  }
  else
    throw Base::Exception("Not allowed type used (vector expected)...");
}

void PropertyVectorList::Save (Writer &writer) const
{
  if(writer.isForceXML())
  {
//  writer << "<VectorList count=\"" <<  getSize() <<"\"/>" << endl;
//  for(int i = 0;i<getSize(); i++)
//    writer << writer.ind() << "<PropertyVector valueX=\"" <<  _lValueList[i].x << "\" valueY=\"" <<  _lValueList[i].y << "\" valueZ=\"" <<  _lValueList[i].z <<"\"/>" << endl;
//  writer << "</VectorList>" << endl ;
  }else{
    writer.Stream() << writer.ind() << "<VectorList file=\"" << writer.addFile(getName(), this) << "\"/>" << std::endl;
  }
}

void PropertyVectorList::Restore(Base::XMLReader &reader)
{
  reader.readElement("VectorList");
  string file (reader.getAttribute("file") );

  if(file == "")
  {
//  // read my Element
//  reader.readElement("VectorList");
//  // get the value of my Attribute
//  int count = reader.getAttributeAsInteger("count");
//
//  setSize(count);
//
//  for(int i = 0;i<count; i++)
//  {
//    reader.readElement("PropertyVector");
//    _lValueList[i].x = (float) reader.getAttributeAsFloat("valueX");
//    _lValueList[i].y = (float) reader.getAttributeAsFloat("valueY");
//    _lValueList[i].z = (float) reader.getAttributeAsFloat("valueZ");
//  }
//
//  reader.readEndElement("VectorList");
  }else{
    // initate a file read
    reader.addFile(file.c_str(),this);
  }
}

void PropertyVectorList::SaveDocFile (Base::Writer &writer) const
{
  try {
    unsigned long uCt = getSize();
    writer.Stream().write((const char*)&uCt, sizeof(unsigned long));
    writer.Stream().write((const char*)&(_lValueList[0]), uCt*sizeof(Base::Vector3f));
  } catch( const Base::Exception& e) {
    throw e;
  }
}

void PropertyVectorList::RestoreDocFile(Base::Reader &reader)
{
  try {
    _lValueList.clear();
    unsigned long uCt=ULONG_MAX;
    reader.read((char*)&uCt, sizeof(unsigned long));
    _lValueList.resize(uCt);
    reader.read((char*)&(_lValueList[0]), uCt*sizeof(Base::Vector3f));
  } catch( const Base::Exception& e) {
    throw e;
  }
}


Property *PropertyVectorList::Copy(void) const
{
  PropertyVectorList *p= new PropertyVectorList();
  p->_lValueList = _lValueList;
  return p;
}

void PropertyVectorList::Paste(const Property &from)
{
  aboutToSetValue();
  _lValueList = dynamic_cast<const PropertyVectorList&>(from)._lValueList;
  hasSetValue();
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
  aboutToSetValue();
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
    setValue( pcObject->value() );
  }else if(PyTuple_Check(value)&&PyTuple_Size(value)==16) {
    PyObject* item;
    Base::Matrix4D cMatrix;
    
    for(int x=0; x<4;x++){
      for(int y=0; y<4;y++){
        item = PyTuple_GetItem(value,x+y*4);
        if (PyFloat_Check(item))
          cMatrix[x][y] = (float)PyFloat_AsDouble(item);
        else if (PyInt_Check(item))
          cMatrix[x][y] = (float)PyInt_AsLong(item);
        else
          throw Base::Exception("Not allowed type used in matrix tuple (a number expected)...");
      }
    }
    setValue( cMatrix );
  }else
    throw Base::Exception("Not allowed type used (Tuple (n=16) or App.Matrix expected)...");
}

void PropertyMatrix::Save (Base::Writer &writer) const
{
  writer.Stream() << writer.ind() << "<PropertyMatrix"; 
  writer.Stream() << " a11=\"" <<  _cMat[0][0] << "\" a12=\"" <<  _cMat[0][1] << "\" a13=\"" <<  _cMat[0][2] << "\" a14=\"" <<  _cMat[0][3] << "\"";
  writer.Stream() << " a21=\"" <<  _cMat[1][0] << "\" a22=\"" <<  _cMat[1][1] << "\" a23=\"" <<  _cMat[1][2] << "\" a24=\"" <<  _cMat[1][3] << "\"";
  writer.Stream() << " a31=\"" <<  _cMat[2][0] << "\" a32=\"" <<  _cMat[2][1] << "\" a33=\"" <<  _cMat[2][2] << "\" a34=\"" <<  _cMat[2][3] << "\"";
  writer.Stream() << " a41=\"" <<  _cMat[3][0] << "\" a42=\"" <<  _cMat[3][1] << "\" a43=\"" <<  _cMat[3][2] << "\" a44=\"" <<  _cMat[3][3] << "\"";
  writer.Stream() <<"/>" << endl;
}

void PropertyMatrix::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("PropertyMatrix");
  // get the value of my Attribute
  _cMat[0][0] = (float)reader.getAttributeAsFloat("a11");
  _cMat[0][1] = (float)reader.getAttributeAsFloat("a12");
  _cMat[0][2] = (float)reader.getAttributeAsFloat("a13");
  _cMat[0][3] = (float)reader.getAttributeAsFloat("a14");

  _cMat[1][0] = (float)reader.getAttributeAsFloat("a21");
  _cMat[1][1] = (float)reader.getAttributeAsFloat("a22");
  _cMat[1][2] = (float)reader.getAttributeAsFloat("a23");
  _cMat[1][3] = (float)reader.getAttributeAsFloat("a24");

  _cMat[2][0] = (float)reader.getAttributeAsFloat("a31");
  _cMat[2][1] = (float)reader.getAttributeAsFloat("a32");
  _cMat[2][2] = (float)reader.getAttributeAsFloat("a33");
  _cMat[2][3] = (float)reader.getAttributeAsFloat("a34");

  _cMat[3][0] = (float)reader.getAttributeAsFloat("a41");
  _cMat[3][1] = (float)reader.getAttributeAsFloat("a42");
  _cMat[3][2] = (float)reader.getAttributeAsFloat("a43");
  _cMat[3][3] = (float)reader.getAttributeAsFloat("a44");
}


Property *PropertyMatrix::Copy(void) const
{
  PropertyMatrix *p= new PropertyMatrix();
  p->_cMat = _cMat;
  return p;
}

void PropertyMatrix::Paste(const Property &from)
{
  aboutToSetValue();
  _cMat = dynamic_cast<const PropertyMatrix&>(from)._cMat;
  hasSetValue();
}

//**************************************************************************
//**************************************************************************
// PropertyPlacement
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyPlacement , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyPlacement::PropertyPlacement()
{

}


PropertyPlacement::~PropertyPlacement()
{

}

//**************************************************************************
// Base class implementer


void PropertyPlacement::setValue(const Base::Placement &pos)
{
  aboutToSetValue();
	_cPos=pos;
  hasSetValue();
}


const Base::Placement & PropertyPlacement::getValue(void)const 
{
	return _cPos;
}

PyObject *PropertyPlacement::getPyObject(void)
{
  return 0;
}

void PropertyPlacement::setPyObject(PyObject *value)
{ 
  assert(0);
}

void PropertyPlacement::Save (Base::Writer &writer) const
{
  writer.Stream() << writer.ind() << "<PropertyPlacement"; 
  writer.Stream() << " Px=\"" <<  _cPos.getPos().x << "\" Py=\"" <<  _cPos.getPos().y << "\" Pz=\"" <<  _cPos.getPos().z << "\"";
  writer.Stream() << " Q0=\"" <<  _cPos.getRotation()[0] << "\" Q1=\"" <<  _cPos.getRotation()[1] << "\" Q2=\"" <<  _cPos.getRotation()[2] << "\" Q3=\"" <<  _cPos.getRotation()[3] << "\"";
  writer.Stream() <<"/>" << endl;
}

void PropertyPlacement::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("PropertyPlacement");
  // get the value of my Attribute
  _cPos._Pos.x = reader.getAttributeAsFloat("Px");
  _cPos._Pos.y = reader.getAttributeAsFloat("Py");
  _cPos._Pos.z = reader.getAttributeAsFloat("Pz");

  _cPos._q[0] = reader.getAttributeAsFloat("Q0");
  _cPos._q[1] = reader.getAttributeAsFloat("Q1");
  _cPos._q[2] = reader.getAttributeAsFloat("Q2");
  _cPos._q[3] = reader.getAttributeAsFloat("Q3");
}


Property *PropertyPlacement::Copy(void) const
{
  PropertyPlacement *p= new PropertyPlacement();
  p->_cPos = _cPos;
  return p;
}

void PropertyPlacement::Paste(const Property &from)
{
  aboutToSetValue();
  _cPos = dynamic_cast<const PropertyPlacement&>(from)._cPos;
  hasSetValue();
}

//**************************************************************************
//**************************************************************************
// PropertyPlacement
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyPlacementLink , App::PropertyLink);

//**************************************************************************
// Construction/Destruction


PropertyPlacementLink::PropertyPlacementLink()
{

}


PropertyPlacementLink::~PropertyPlacementLink()
{

}

App::Placement * PropertyPlacementLink::getPlacementObject(void) const
{
  if(_pcLink->getTypeId().isDerivedFrom(App::Placement::getClassTypeId()))
    return dynamic_cast<App::Placement*>(_pcLink);
  else
    return 0;

}

//**************************************************************************
// Base class implementer

Property *PropertyPlacementLink::Copy(void) const
{
  PropertyPlacementLink *p= new PropertyPlacementLink();
  p->_pcLink = _pcLink;
  return p;
}

void PropertyPlacementLink::Paste(const Property &from)
{
  aboutToSetValue();
  _pcLink = dynamic_cast<const PropertyPlacementLink&>(from)._pcLink;
  hasSetValue();
}

