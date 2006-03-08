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

void PropertyInteger::Save (Writer &writer)
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

void PropertyIntegerList::Save (Writer &writer)
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

void PropertyFloat::Save (Writer &writer)
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

void PropertyFloatList::Save (Writer &writer)
{
  writer << "<FloatList count=\"" <<  getSize() <<"\"/>" << endl;
  for(int i = 0;i<getSize(); i++)
    writer << "<F v=\"" <<  _lValueList[i] <<"\"/>" << endl; ;
  writer << "</FloatList>" << endl ;


}

void PropertyFloatList::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("FloatList");
  // get the value of my Attribute
  int count = reader.getAttributeAsInteger("count");

  setSize(count);

  for(int i = 0;i<count; i++)
  {
    reader.readElement("F");
    _lValueList[i] = (float) reader.getAttributeAsFloat("v");
  }

  reader.readEndElement("FloatList");

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

void PropertyString::Save (Writer &writer)
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

void PropertyBool::Save (Writer &writer)
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
  reader.readElement("String");
  // get the value of my Attribute
  string b = reader.getAttribute("value");
  if(b == "true")
    _lValue = true;
  else
    _lValue = false;
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
  if ( PyTuple_Check(value) && PyTuple_Size(value) == 3 )
  {
    PyObject* item;
    item = PyTuple_GetItem(value,0);
    if ( PyFloat_Check(item) )
      _cCol.r = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,1);
    if ( PyFloat_Check(item) )
      _cCol.g = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,2);
    if ( PyFloat_Check(item) )
      _cCol.b = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
  }
  else if ( PyTuple_Check(value) && PyTuple_Size(value) == 4 )
  {
    PyObject* item;
    item = PyTuple_GetItem(value,0);
    if ( PyFloat_Check(item) )
      _cCol.r = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,1);
    if ( PyFloat_Check(item) )
      _cCol.g = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,2);
    if ( PyFloat_Check(item) )
      _cCol.b = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
    item = PyTuple_GetItem(value,3);
    if ( PyFloat_Check(item) )
      _cCol.a = (float)PyFloat_AsDouble(item);
    else
      throw Base::Exception("Not allowed type used in tuple (float expected)...");
  }
  else if ( PyInt_Check(value) )
  {
    _cCol.setPackedValue(PyInt_AsLong(value));
  }
  else
  {
    throw Base::Exception("Not allowed type used...");
  }
}

void PropertyColor::Save (Writer &writer)
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

void PropertyColorList::Save (Writer &writer)
{
  writer << "<ColorList count=\"" <<  getSize() <<"\"/>" << endl;
  for(int i = 0;i<getSize(); i++)
    writer << writer.ind() << "<PropertyColor value=\"" <<  _lValueList[i].getPackedValue() <<"\"/>" << endl;
  writer << "</ColorList>" << endl ;
}

void PropertyColorList::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("ColorList");
  // get the value of my Attribute
  int count = reader.getAttributeAsInteger("count");

  setSize(count);

  for(int i = 0;i<count; i++)
  {
    reader.readElement("PropertyColor");
    _lValueList[i].setPackedValue( reader.getAttributeAsInteger("value") );
  }

  reader.readEndElement("ColorList");
}














#if 0
//**************************************************************************
//**************************************************************************
// PropertyColor
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


PropertyColor::PropertyColor(long lRed, long lGreen, long lBlue)
	:_lRed(lRed),_lGreen(lGreen), _lBlue(lBlue)
{

}


PropertyColor::~PropertyColor()
{

}

//**************************************************************************
// Base class implementer

void PropertyColor::Set(const char* Str)
{

}

const char* PropertyColor::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"[%ld, %ld, %ld]",_lRed, _lGreen, _lBlue);
	return sBuf;
}



const char* PropertyColor::GetType(void)
{
	return "Color";
}

const char* PropertyColor::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{

	if(strcmp(sSubPropName,"Red")==0)
		_lRed = atol(sNewValue);
	else if(strcmp(sSubPropName,"Green")==0)
		_lGreen = atol(sNewValue);
	else if(strcmp(sSubPropName,"Blue")==0)
		_lBlue = atol(sNewValue);
	else return "";

	return GetSubProperty(sSubPropName);
}


const char* PropertyColor::GetSubProperty(const char* sSubPropName)
{
	static char sBuf[DBL_DIG+10];

	if(strcmp(sSubPropName,"Red")==0)
		sprintf(sBuf,"%ld",_lRed);
	else if(strcmp(sSubPropName,"Green")==0)
		sprintf(sBuf,"%ld",_lGreen);
	else if(strcmp(sSubPropName,"Blue")==0)
		sprintf(sBuf,"%ld",_lBlue);
	else return "";

	return sBuf;
}


const char* PropertyColor::GetSubPropertyNames(void)
{
	return "Red;Green;Blue";
}

//**************************************************************************
// Seter getter for the property

void PropertyColor::SetRed(long lRed)
{
	_lRed=lRed;
}

long PropertyColor::GetRed(void)
{
	return _lRed;
}

void PropertyColor::SetGreen(long lGreen)
{
	_lGreen=lGreen;
}

long PropertyColor::GetGreen(void)
{
	return _lGreen;
}

void PropertyColor::SetBlue(long lBlue)
{
	_lBlue=lBlue;
}

long PropertyColor::GetBlue(void)
{
	return _lBlue;
}

//**************************************************************************
//**************************************************************************
// PropertyList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

       
PropertyList::PropertyList(const std::vector<std::string>& lValue, long lCurrent)
	:_lValue(lValue), _lCurrent(lCurrent)
{

}


PropertyList::~PropertyList()
{

}

//**************************************************************************
// Base class implementer

void PropertyList::Set(const char* Str)
{

}

const char* PropertyList::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
  int pos = 0;
  for (std::vector<std::string>::iterator it = _lValue.begin(); it!=_lValue.end(); ++it)
  {
    if (pos+it->length() >= DBL_DIG)
    {
//      Console().Warning("PropertyList: List too long\n");
      break;
    }
  	pos += sprintf(sBuf+pos,"%s ",it->c_str());
  }
	return sBuf;
}



const char* PropertyList::GetType(void)
{
	return "List";
}

const char* PropertyList::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{
	if(strcmp(sSubPropName,"Current")==0)
		_lCurrent = atol(sNewValue);
  else
  {
    unsigned lPos = atol(sSubPropName);
    if (lPos < _lValue.size())
      _lValue[lPos] = sNewValue;
  	else return "";
  }

	return GetSubProperty(sSubPropName);
}


const char* PropertyList::GetSubProperty(const char* sSubPropName)
{
	static char sBuf[DBL_DIG+10];

	if(strcmp(sSubPropName,"Current")==0)
      sprintf(sBuf,"%ld",_lCurrent);
  else
  {
    unsigned lPos = atol(sSubPropName);
    if (lPos < _lValue.size())
      sprintf(sBuf,"%s",_lValue[lPos].c_str());
  	else return "";
  }

	return sBuf;
}


const char* PropertyList::GetSubPropertyNames(void)
{
	static char sBuf[DBL_DIG+10];
  int pos = 0;

  long size = _lValue.size()-1;
  pos = sprintf(sBuf, "%ld.", _lCurrent);
  for (long i = 0; i < size; ++i)
    pos += sprintf(sBuf+pos, "%ld.", i);
  sprintf(sBuf+pos, "%ld", size);
	return sBuf;
}

//**************************************************************************
// Seter getter for the property

void PropertyList::SetValue(const std::vector<std::string>& lValue)
{
	_lValue=lValue;
}

const std::vector<std::string>& PropertyList::GetValue(void)
{
	return _lValue;
}

void PropertyList::SetCurrentItem (long lCurrent)
{
  _lCurrent = lCurrent;
}

long PropertyList::GetCurrentItem (void)
{
  return _lCurrent;
}
#endif

