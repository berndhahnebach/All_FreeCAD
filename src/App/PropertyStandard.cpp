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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <Standard_ConstructionError.hxx>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "PropertyStandard.h"

using namespace App;




//**************************************************************************
//**************************************************************************
// PropertyInteger
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


PropertyInteger::PropertyInteger(long lValue, long lMax, long lMin, long lStep)
	:_lValue(lValue),_lMax(lMax), _lMin(lMin), _lStep(lStep)
{

}


PropertyInteger::~PropertyInteger()
{

}

//**************************************************************************
// Base class implementer

void PropertyInteger::Set(const char* Str)
{

}


const char* PropertyInteger::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%ld",_lValue);
	return sBuf;
}



const char* PropertyInteger::GetType(void)
{
	return "Int";
}

const char* PropertyInteger::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{

	if(strcmp(sSubPropName,"Value")==0)
		_lValue = atol(sNewValue);
	else if(strcmp(sSubPropName,"Max")==0)
		_lMax = atol(sNewValue);
	else if(strcmp(sSubPropName,"Min")==0)
		_lMin = atol(sNewValue);
	else if(strcmp(sSubPropName,"Step")==0)
		_lStep = atol(sNewValue);
	else return "";

	return GetSubProperty(sSubPropName);
}


const char* PropertyInteger::GetSubProperty(const char* sSubPropName)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%ld",_lValue);

	if(strcmp(sSubPropName,"Value")==0)
		sprintf(sBuf,"%ld",_lValue);
	else if(strcmp(sSubPropName,"Max")==0)
		sprintf(sBuf,"%ld",_lMax);
	else if(strcmp(sSubPropName,"Min")==0)
		sprintf(sBuf,"%ld",_lMin);
	else if(strcmp(sSubPropName,"Step")==0)
		sprintf(sBuf,"%ld",_lStep);
	else return "";

	return sBuf;
}


const char* PropertyInteger::GetSubPropertyNames(void)
{
	return "Value;Max;Min;Step";
}

//**************************************************************************
// Seter getter for the property

void PropertyInteger::SetMin(long lMin)
{
	_lMin=lMin;
}

long PropertyInteger::GetMin(void)
{
	return _lMin;
}

void PropertyInteger::SetMax(long lMax)
{
	_lMax=lMax;
}

long PropertyInteger::GetMax(void)
{
	return _lMax;
}

void PropertyInteger::SetStep(long lStep)
{
	_lStep=lStep;
}

long PropertyInteger::GetStep(void)
{
	return _lStep;
}

void PropertyInteger::SetValue(long lValue)
{
	_lValue=lValue;
}

long PropertyInteger::GetValue(void)
{
	return _lValue;
}

//**************************************************************************
//**************************************************************************
// PropertyFloat
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


PropertyFloat::PropertyFloat(double dValue, double dMax, double dMin, double dStep)
	:_dValue(dValue),_dMax(dMax), _dMin(dMin), _dStep(dStep)
{

}


PropertyFloat::~PropertyFloat()
{

}

//**************************************************************************
// Base class implementer

void PropertyFloat::Set(const char* Str)
{
  SetValue(atof(Str));
}

const char* PropertyFloat::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%e",_dValue);
	return sBuf;
}



const char* PropertyFloat::GetType(void)
{
	return "Float";
}

const char* PropertyFloat::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{

	if(strcmp(sSubPropName,"Value")==0)
		_dValue = atof(sNewValue);
	else if(strcmp(sSubPropName,"Max")==0)
		_dMax = atof(sNewValue);
	else if(strcmp(sSubPropName,"Min")==0)
		_dMin = atof(sNewValue);
	else if(strcmp(sSubPropName,"Step")==0)
		_dStep = atof(sNewValue);
	else return "";

	return GetSubProperty(sSubPropName);
}


const char* PropertyFloat::GetSubProperty(const char* sSubPropName)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%e",_dValue);

	if(strcmp(sSubPropName,"Value")==0)
		sprintf(sBuf,"%e",_dValue);
	else if(strcmp(sSubPropName,"Max")==0)
		sprintf(sBuf,"%e",_dMax);
	else if(strcmp(sSubPropName,"Min")==0)
		sprintf(sBuf,"%e",_dMin);
	else if(strcmp(sSubPropName,"Step")==0)
		sprintf(sBuf,"%e",_dStep);
	else return "";

	return sBuf;
}


const char* PropertyFloat::GetSubPropertyNames(void)
{
	return "Value;Max;Min;Step";
}

//**************************************************************************
// Seter getter for the property

void PropertyFloat::SetMin(double dMin)
{
	_dMin=dMin;
}

double PropertyFloat::GetMin(void)
{
	return _dMin;
}

void PropertyFloat::SetMax(double dMax)
{
	_dMax=dMax;
}

double PropertyFloat::GetMax(void)
{
	return _dMax;
}

void PropertyFloat::SetStep(double dStep)
{
	_dStep=dStep;
}

double PropertyFloat::GetStep(void)
{
	return _dStep;
}

void PropertyFloat::SetValue(double lValue)
{
	_dValue=lValue;
}

double PropertyFloat::GetValue(void)
{
	return _dValue;
}

//**************************************************************************
//**************************************************************************
// PropertyString
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

       
PropertyString::PropertyString(const char* sValue, const char* sConstraint)
	:_cValue(sValue),_cConstrain(sConstraint)
{

}


PropertyString::~PropertyString()
{

}

//**************************************************************************
// Base class implementer

void PropertyString::Set(const char* Str)
{
  _cValue = Str;
}

const char* PropertyString::GetAsString(void)
{
	return _cValue.c_str();
}



const char* PropertyString::GetType(void)
{
	return "String";
}

const char* PropertyString::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{

	if(strcmp(sSubPropName,"Value")==0)
		_cValue = sNewValue;
	else if(strcmp(sSubPropName,"Constrain")==0)
		_cConstrain = sNewValue;
	else 
		return "";

	return GetSubProperty(sSubPropName);
}


const char* PropertyString::GetSubProperty(const char* sSubPropName)
{
	if(strcmp(sSubPropName,"Value")==0)
		return _cValue.c_str();
	else if(strcmp(sSubPropName,"Constrain")==0)
		return _cConstrain.c_str();
	else 
		return "";
}


const char* PropertyString::GetSubPropertyNames(void)
{
	return "Value;Constrain";
}

//**************************************************************************
// Seter getter for the property


void PropertyString::SetString(const char* sString)
{
	_cValue = sString;
}

const char* PropertyString::GetString(void)
{
	return _cValue.c_str();
}

void PropertyString::SetConstrain(const char* sConstrains)
{
	_cConstrain = sConstrains;
}

const char* PropertyString::GetConstrain(void)
{
	return _cConstrain.c_str();
}




//**************************************************************************
//**************************************************************************
// PropertyBool
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

       
PropertyBool::PropertyBool(bool lValue)
	:_lValue(lValue)
{

}


PropertyBool::~PropertyBool()
{

}

//**************************************************************************
// Base class implementer

void PropertyBool::Set(const char* Str)
{

}

const char* PropertyBool::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
  sprintf(sBuf,"%s",(_lValue ? "True" : "False"));
	return sBuf;
}



const char* PropertyBool::GetType(void)
{
	return "Bool";
}

const char* PropertyBool::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{

	if(strcmp(sSubPropName,"Value")==0)
    _lValue = atol(sNewValue) > 0 ? true : false;
	else return "";

	return GetSubProperty(sSubPropName);
}


const char* PropertyBool::GetSubProperty(const char* sSubPropName)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%d",_lValue);

	if(strcmp(sSubPropName,"Value")==0)
    sprintf(sBuf,"%s",(_lValue ? "True" : "False"));
	else return "";

	return sBuf;
}


const char* PropertyBool::GetSubPropertyNames(void)
{
	return "Value";
}

//**************************************************************************
// Seter getter for the property

void PropertyBool::SetValue(bool lValue)
{
	_lValue=lValue;
}

bool PropertyBool::GetValue(void)
{
	return _lValue;
}

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
