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

#include "PropertyStandard.h"

using namespace App;
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
	_lValue=lValue;
  hasSetValue();
}

long PropertyInteger::getValue(void)
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

void PropertyInteger::Save (short indent,std::ostream &str)
{
  str << "<Integer value=\"" <<  _lValue <<"\"/>" ;
}

void PropertyInteger::Restore(Base::Reader &reader)
{
  // read my Element
  reader.readElement("Integer");
  // get the value of my Attribute
  _lValue = reader.getAttributeAsInteger("value");
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


void PropertyFloat::setValue(double lValue)
{
	_dValue=lValue;
  hasSetValue();

}

double PropertyFloat::getValue(void)
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
    _dValue = (double) PyFloat_AsDouble(value);
    hasSetValue();

  }else if(PyInt_Check( value) )
  {
    _dValue = (double) PyInt_AsLong(value);
    hasSetValue();
  }else
    throw "Not allowed type used (float or int expected)...";

}

void PropertyFloat::Save (short indent,std::ostream &str)
{
  str <<  "<Float value=\"" <<  _dValue <<"\"/>" ;
}

void PropertyFloat::Restore(Base::Reader &reader)
{
  // read my Element
  reader.readElement("Float");
  // get the value of my Attribute
  _dValue = reader.getAttributeAsFloat("value");
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

void PropertyString::Save (short indent,std::ostream &str)
{
  str << "<String value=\"" <<  _cValue.c_str() <<"\"/>" ;
}

void PropertyString::Restore(Base::Reader &reader)
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
	_lValue=lValue;
  hasSetValue();

}

bool PropertyBool::getValue(void)
{
	return _lValue;
}

PyObject *PropertyBool::getPyObject(void)
{
  if(_lValue)
    {Py_INCREF(Py_True); return Py_True;}
  else
    {Py_INCREF(Py_False); return Py_False;};

}

void PropertyBool::setPyObject(PyObject *value)
{
  if(PyInt_Check( value) )
  {
    _lValue = PyInt_AsLong(value)!=0;
    hasSetValue();
  }else
    throw Base::Exception("Not allowed type used (bool expected)...");
}

void PropertyBool::Save (short indent,std::ostream &str)
{
  str << "<Bool value=\"" ;
  if(_lValue)
    str << "true" <<"\"/>" ;
  else
    str << "false" <<"\"/>" ;

}

void PropertyBool::Restore(Base::Reader &reader)
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


/*
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
*/
