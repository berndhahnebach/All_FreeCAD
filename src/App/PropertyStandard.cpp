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
# include <iostream>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......

#include <Base/Exception.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "PropertyStandard.h"

using namespace App;
using namespace Base;
using namespace std;




//**************************************************************************
//**************************************************************************
// PropertyInteger
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyInteger , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyInteger::PropertyInteger()
{

}


PropertyInteger::~PropertyInteger()
{

}

//**************************************************************************
// Base class implementer


void PropertyInteger::setValue(long lValue)
{
  aboutToSetValue();
	_lValue=lValue;
  hasSetValue();
}

long PropertyInteger::getValue(void) const
{
	return _lValue;
}

PyObject *PropertyInteger::getPyObject(void)
{
  return Py_BuildValue("l", _lValue);
}

void PropertyInteger::setPyObject(PyObject *value)
{ 
  if(PyInt_Check( value) )
  {
    _lValue = PyInt_AsLong(value);
    hasSetValue();
  }else
    throw Base::Exception("Not allowed type used (float or int expected)...");

}

void PropertyInteger::Save (Writer &writer) const
{
  writer << "<Integer value=\"" <<  _lValue <<"\"/>" ;
}

void PropertyInteger::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("Integer");
  // get the value of my Attribute
  _lValue = reader.getAttributeAsInteger("value");
}

Property *PropertyInteger::Copy(void) const
{
  PropertyInteger *p= new PropertyInteger();
  p->_lValue = _lValue;
  return p;
}

void PropertyInteger::Paste(const Property &from)
{
  _lValue = dynamic_cast<const PropertyInteger&>(from)._lValue;
}


//**************************************************************************
//**************************************************************************
// PropertyIntegerList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyIntegerList , App::PropertyLists);

//**************************************************************************
// Construction/Destruction


PropertyIntegerList::PropertyIntegerList()
{

}


PropertyIntegerList::~PropertyIntegerList()
{

}

//**************************************************************************
// Base class implementer


void PropertyIntegerList::setValue(long lValue)
{
  aboutToSetValue();
  _lValueList.resize(1);
	_lValueList[0]=lValue;
  hasSetValue();
}


PyObject *PropertyIntegerList::getPyObject(void)
{
  PyObject* list = PyList_New(	getSize() );

  for(int i = 0;i<getSize(); i++)
     PyList_SetItem( list, i, PyInt_FromLong(	_lValueList[i]));

  return list;
}

void PropertyIntegerList::setPyObject(PyObject *value)
{ 
  if(PyList_Check( value) )
  {
    aboutToSetValue();

    int nSize = PyList_Size(value);
    _lValueList.resize(nSize);

    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(value, i);
      if (!PyInt_Check(item))
      {
        _lValueList.resize(1);
        _lValueList[0] = 0;
        throw Base::Exception("Not allowed type in list (int expected)...");
      }
      long pItem = PyInt_AsLong(item);
      _lValueList[i] = pItem;
    }

    hasSetValue();
  }else if(PyInt_Check( value) )
  {
    setValue(PyInt_AsLong(value));
  }else
    throw Base::Exception("Not allowed type used (int expected)...");

}

void PropertyIntegerList::Save (Writer &writer) const
{
  writer << "<IntegerList count=\"" <<  getSize() <<"\"/>" << endl;
  for(int i = 0;i<getSize(); i++)
    writer << "<I v=\"" <<  _lValueList[i] <<"\"/>" << endl; ;
  writer << "</IntegerList>" << endl ;


}

void PropertyIntegerList::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("IntegerList");
  // get the value of my Attribute
  int count = reader.getAttributeAsInteger("count");

  setSize(count);

  for(int i = 0;i<count; i++)
  {
    reader.readElement("I");
    _lValueList[i] = reader.getAttributeAsInteger("v");
  }

  reader.readEndElement("IntegerList");

}

Property *PropertyIntegerList::Copy(void) const
{
  PropertyIntegerList *p= new PropertyIntegerList();
  p->_lValueList = _lValueList;
  return p;
}

void PropertyIntegerList::Paste(const Property &from)
{
  _lValueList = dynamic_cast<const PropertyIntegerList&>(from)._lValueList;
}



//**************************************************************************
//**************************************************************************
// PropertyFloat
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyFloat , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyFloat::PropertyFloat()
{

}


PropertyFloat::~PropertyFloat()
{

}

//**************************************************************************
// Base class implementer


void PropertyFloat::setValue(float lValue)
{
  aboutToSetValue();
	_dValue=lValue;
  hasSetValue();
}

float PropertyFloat::getValue(void) const
{
	return _dValue;
}

PyObject *PropertyFloat::getPyObject(void)
{
  return Py_BuildValue("d", _dValue);
}

void PropertyFloat::setPyObject(PyObject *value)
{
  if(PyFloat_Check( value) )
  {
    _dValue = (float) PyFloat_AsDouble(value);
    hasSetValue();

  }else if(PyInt_Check( value) )
  {
    _dValue = (float) PyInt_AsLong(value);
    hasSetValue();
  }else
    throw "Not allowed type used (float or int expected)...";

}

void PropertyFloat::Save (Writer &writer) const
{
  writer <<  "<Float value=\"" <<  _dValue <<"\"/>" ;
}

void PropertyFloat::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("Float");
  // get the value of my Attribute
  _dValue = (float) reader.getAttributeAsFloat("value");
}

Property *PropertyFloat::Copy(void) const
{
  PropertyFloat *p= new PropertyFloat();
  p->_dValue = _dValue;
  return p;
}

void PropertyFloat::Paste(const Property &from)
{
  _dValue = dynamic_cast<const PropertyFloat&>(from)._dValue;
}


//**************************************************************************
// PropertyFloatList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyFloatList , App::PropertyLists);

//**************************************************************************
// Construction/Destruction


PropertyFloatList::PropertyFloatList()
{

}


PropertyFloatList::~PropertyFloatList()
{

}

//**************************************************************************
// Base class implementer


void PropertyFloatList::setValue(float lValue)
{
  aboutToSetValue();
  _lValueList.resize(1);
	_lValueList[0]=lValue;
  hasSetValue();
}


PyObject *PropertyFloatList::getPyObject(void)
{
  PyObject* list = PyList_New(	getSize() );

  for(int i = 0;i<getSize(); i++)
     PyList_SetItem( list, i, PyFloat_FromDouble(	_lValueList[i]));

  return list;
}

void PropertyFloatList::setPyObject(PyObject *value)
{ 
  if(PyList_Check( value) )
  {
    aboutToSetValue();

    int nSize = PyList_Size(value);
    _lValueList.resize(nSize);

    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(value, i);
      if (!PyFloat_Check(item))
      {
        _lValueList.resize(1);
        _lValueList[0] = 0;
        throw Base::Exception("Not allowed type in list (float expected)...");
      }
      float pItem = (float) PyFloat_AsDouble(item);
      _lValueList[i] = pItem;
    }

    hasSetValue();
  }else if(PyFloat_Check( value) )
  {
    setValue((float) PyFloat_AsDouble(value));
  }else
    throw Base::Exception("Not allowed type used (float expected)...");

}

void PropertyFloatList::Save (Writer &writer) const
{
  if(writer.isForceXML())
  {
//  writer << "<FloatList count=\"" <<  getSize() <<"\"/>" << endl;
//  for(int i = 0;i<getSize(); i++)
//    writer << "<F v=\"" <<  _lValueList[i] <<"\"/>" << endl; ;
//  writer << "</FloatList>" << endl ;
  }else{
    writer << writer.ind() << "<FloatList file=\"" << writer.addFile(getName(), this) << "\"/>" << std::endl;
  }
}

void PropertyFloatList::Restore(Base::XMLReader &reader)
{
  reader.readElement("FloatList");
  string file (reader.getAttribute("file") );

  if(file == "")
  {
//  // read my Element
//  reader.readElement("FloatList");
//  // get the value of my Attribute
//  int count = reader.getAttributeAsInteger("count");
//
//  setSize(count);
//
//  for(int i = 0;i<count; i++)
//  {
//    reader.readElement("F");
//    _lValueList[i] = (float) reader.getAttributeAsFloat("v");
//  }
//
//  reader.readEndElement("FloatList");
  }else{
    // initate a file read
    reader.addFile(file.c_str(),this);
  }
}

void PropertyFloatList::SaveDocFile (Base::Writer &writer) const
{
  try {
    unsigned long uCt = getSize();
    writer.write((const char*)&uCt, sizeof(unsigned long));
    writer.write((const char*)&(_lValueList[0]), uCt*sizeof(float));
  } catch( const Base::Exception& e) {
    throw e;
  }
}

void PropertyFloatList::RestoreDocFile(Base::Reader &reader)
{
  try {
    _lValueList.clear();
    unsigned long uCt=ULONG_MAX;
    reader.read((char*)&uCt, sizeof(unsigned long));
    _lValueList.resize(uCt);
    reader.read((char*)&(_lValueList[0]), uCt*sizeof(float));
  } catch( const Base::Exception& e) {
    throw e;
  }
}

Property *PropertyFloatList::Copy(void) const
{
  PropertyFloatList *p= new PropertyFloatList();
  p->_lValueList = _lValueList;
  return p;
}

void PropertyFloatList::Paste(const Property &from)
{
  _lValueList = dynamic_cast<const PropertyFloatList&>(from)._lValueList;
}


//**************************************************************************
//**************************************************************************
// PropertyString
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyString , App::Property);

//**************************************************************************
// Construction/Destruction

       
PropertyString::PropertyString()
{

}


PropertyString::~PropertyString()
{

}



//**************************************************************************
// Base class implementer


//**************************************************************************
// Seter getter for the property


void PropertyString::setValue(const char* sString)
{
  aboutToSetValue();
	_cValue = sString;
  hasSetValue();

}

void PropertyString::setValue(const std::string &sString)
{
	_cValue = sString;
  hasSetValue();

}

const char* PropertyString::getValue(void) const
{
	return _cValue.c_str();
}

PyObject *PropertyString::getPyObject(void)
{
  return Py_BuildValue("s", _cValue.c_str());

}

void PropertyString::setPyObject(PyObject *value)
{
  if(PyString_Check( value) )
  {
    _cValue = PyString_AsString(value);
    hasSetValue();

  }else
    throw "Not allowed type used (float or int expected)...";

}

void PropertyString::Save (Writer &writer) const
{
  writer << "<String value=\"" <<  _cValue.c_str() <<"\"/>" ;
}

void PropertyString::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("String");
  // get the value of my Attribute
  _cValue = reader.getAttribute("value");
}


Property *PropertyString::Copy(void) const
{
  PropertyString *p= new PropertyString();
  p->_cValue = _cValue;
  return p;
}

void PropertyString::Paste(const Property &from)
{
  _cValue = dynamic_cast<const PropertyString&>(from)._cValue;
}


//**************************************************************************
//**************************************************************************
// PropertyBool
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyBool , App::Property);

//**************************************************************************
// Construction/Destruction

       
PropertyBool::PropertyBool()
{

}


PropertyBool::~PropertyBool()
{

}



//**************************************************************************
// Seter getter for the property

void PropertyBool::setValue(bool lValue)
{
  aboutToSetValue();
	_lValue=lValue;
  hasSetValue();

}

bool PropertyBool::getValue(void) const
{
	return _lValue;
}

PyObject *PropertyBool::getPyObject(void)
{
  if(_lValue)
    {Py_INCREF(Py_True); return Py_True;}
  else
    {Py_INCREF(Py_False); return Py_False;}

}

void PropertyBool::setPyObject(PyObject *value)
{
  if(PyBool_Check( value) )
    setValue( value == Py_True );
  else if(PyInt_Check( value) )
    setValue( PyInt_AsLong(value)!=0 );
  else
    throw Base::Exception("Not allowed type used (bool expected)...");
}

void PropertyBool::Save (Writer &writer) const
{
  writer << "<Bool value=\"" ;
  if(_lValue)
    writer << "true" <<"\"/>" ;
  else
    writer << "false" <<"\"/>" ;

}

void PropertyBool::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("Bool");
  // get the value of my Attribute
  string b = reader.getAttribute("value");
  if(b == "true")
    _lValue = true;
  else
    _lValue = false;
}


Property *PropertyBool::Copy(void) const
{
  PropertyBool *p= new PropertyBool();
  p->_lValue = _lValue;
  return p;
}

void PropertyBool::Paste(const Property &from)
{
  _lValue = dynamic_cast<const PropertyBool&>(from)._lValue;
}



//**************************************************************************
//**************************************************************************
// PropertyColor
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyColor , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyColor::PropertyColor()
{

}


PropertyColor::~PropertyColor()
{

}

//**************************************************************************
// Base class implementer


void PropertyColor::setValue(const Color &col)
{
	_cCol=col;
  hasSetValue();
}

void PropertyColor::setValue(float r, float g, float b, float a)
{
  _cCol.set(r,g,b,a);
  hasSetValue();
}

const Color& PropertyColor::getValue(void) const 
{
	return _cCol;
}

PyObject *PropertyColor::getPyObject(void)
{
  PyObject* rgba = PyTuple_New(4);
  PyObject* r = PyFloat_FromDouble(_cCol.r);
  PyObject* g = PyFloat_FromDouble(_cCol.g);
  PyObject* b = PyFloat_FromDouble(_cCol.b);
  PyObject* a = PyFloat_FromDouble(_cCol.a);

  PyTuple_SetItem(rgba, 0, r);
  PyTuple_SetItem(rgba, 1, g);
  PyTuple_SetItem(rgba, 2, b);
  PyTuple_SetItem(rgba, 3, a);

  return rgba;
}

void PropertyColor::setPyObject(PyObject *value)
{
  App::Color cCol;
  if ( PyTuple_Check(value) && PyTuple_Size(value) == 3 )
  {
    PyObject* item;
    item = PyTuple_GetItem(value,0);
    if ( PyFloat_Check(item) )
      cCol.r = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,1);
    if ( PyFloat_Check(item) )
      cCol.g = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,2);
    if ( PyFloat_Check(item) )
      cCol.b = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
  }
  else if ( PyTuple_Check(value) && PyTuple_Size(value) == 4 )
  {
    PyObject* item;
    item = PyTuple_GetItem(value,0);
    if ( PyFloat_Check(item) )
      cCol.r = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,1);
    if ( PyFloat_Check(item) )
      cCol.g = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,2);
    if ( PyFloat_Check(item) )
      cCol.b = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,3);
    if ( PyFloat_Check(item) )
      cCol.a = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
  }
  else if ( PyInt_Check(value) )
  {
    cCol.setPackedValue(PyInt_AsLong(value));
  }
  else
  {
    throw Base::Exception("Not allowed type used...");
  }

  setValue( cCol );
}

void PropertyColor::Save (Writer &writer) const
{
  writer << writer.ind() << "<PropertyColor value=\"" <<  _cCol.getPackedValue() <<"\"/>" << endl;
}

void PropertyColor::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("PropertyColor");
  // get the value of my Attribute
  int rgba = reader.getAttributeAsInteger("value");
  _cCol.setPackedValue(rgba);
}


Property *PropertyColor::Copy(void) const
{
  PropertyColor *p= new PropertyColor();
  p->_cCol = _cCol;
  return p;
}

void PropertyColor::Paste(const Property &from)
{
  _cCol = dynamic_cast<const PropertyColor&>(from)._cCol;
}



//**************************************************************************
// PropertyColorList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyColorList , App::PropertyLists);

//**************************************************************************
// Construction/Destruction

PropertyColorList::PropertyColorList()
{

}

PropertyColorList::~PropertyColorList()
{

}

//**************************************************************************
// Base class implementer

void PropertyColorList::setValue(const Color& lValue)
{
  aboutToSetValue();
  _lValueList.resize(1);
	_lValueList[0]=lValue;
  hasSetValue();
}

PyObject *PropertyColorList::getPyObject(void)
{
  PyObject* list = PyList_New(	getSize() );

  for(int i = 0;i<getSize(); i++)
  {
    PyObject* rgba = PyTuple_New(4);
    PyObject* r = PyFloat_FromDouble(_lValueList[i].r);
    PyObject* g = PyFloat_FromDouble(_lValueList[i].g);
    PyObject* b = PyFloat_FromDouble(_lValueList[i].b);
    PyObject* a = PyFloat_FromDouble(_lValueList[i].a);

    PyTuple_SetItem(rgba, 0, r);
    PyTuple_SetItem(rgba, 1, g);
    PyTuple_SetItem(rgba, 2, b);
    PyTuple_SetItem(rgba, 3, a);

    PyList_SetItem( list, i, rgba );
  }

  return list;
}

void PropertyColorList::setPyObject(PyObject *value)
{
  if( PyList_Check( value) )
  {
    aboutToSetValue();

    int nSize = PyList_Size(value);
    _lValueList.resize(nSize);

    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(value, i);
      try {
        PropertyColor col;
        col.setPyObject( item );
        _lValueList[i] = col.getValue();
      } catch (const Base::Exception& e) {
        _lValueList.resize(1);
        _lValueList[0] = Color();
        throw e;
      }
    }

    hasSetValue();
  }
  else if ( PyTuple_Check(value) && PyTuple_Size(value) == 3 )
  {
    PropertyColor col;
    col.setPyObject( value );
    setValue( col.getValue() );
  }
  else if ( PyTuple_Check(value) && PyTuple_Size(value) == 4 )
  {
    PropertyColor col;
    col.setPyObject( value );
    setValue( col.getValue() );
  }
  else
    throw Base::Exception("Not allowed type used...");
}

void PropertyColorList::Save (Writer &writer) const
{
  if(writer.isForceXML())
  {
//    writer << "<ColorList count=\"" <<  getSize() <<"\"/>" << endl;
//    for(int i = 0;i<getSize(); i++)
//      writer << writer.ind() << "<PropertyColor value=\"" <<  _lValueList[i].getPackedValue() <<"\"/>" << endl;
//    writer << "</ColorList>" << endl ;
  }else{
    writer << writer.ind() << "<ColorList file=\"" << writer.addFile(getName(), this) << "\"/>" << std::endl;
  }
}

void PropertyColorList::Restore(Base::XMLReader &reader)
{
  reader.readElement("ColorList");
  string file (reader.getAttribute("file") );

  if(file == "")
  {
//  // read my Element
//  reader.readElement("ColorList");
//  // get the value of my Attribute
//  int count = reader.getAttributeAsInteger("count");
//
//  setSize(count);
//
//  for(int i = 0;i<count; i++)
//  {
//    reader.readElement("PropertyColor");
//    _lValueList[i].setPackedValue( reader.getAttributeAsInteger("value") );
//  }
//
//  reader.readEndElement("ColorList");
  }else{
    // initate a file read
    reader.addFile(file.c_str(),this);
  }
}

void PropertyColorList::SaveDocFile (Base::Writer &writer) const
{
  try {
    unsigned long uCt = getSize();
    writer.write((const char*)&uCt, sizeof(unsigned long));
    writer.write((const char*)&(_lValueList[0]), uCt*sizeof(Color));
  } catch( const Base::Exception& e) {
    throw e;
  }
}

void PropertyColorList::RestoreDocFile(Base::Reader &reader)
{
  try {
    _lValueList.clear();
    unsigned long uCt=ULONG_MAX;
    reader.read((char*)&uCt, sizeof(unsigned long));
    _lValueList.resize(uCt);
    reader.read((char*)&(_lValueList[0]), uCt*sizeof(Color));
  } catch( const Base::Exception& e) {
    throw e;
  }
}



Property *PropertyColorList::Copy(void) const
{
  PropertyColorList *p= new PropertyColorList();
  p->_lValueList = _lValueList;
  return p;
}

void PropertyColorList::Paste(const Property &from)
{
  _lValueList = dynamic_cast<const PropertyColorList&>(from)._lValueList;
}












