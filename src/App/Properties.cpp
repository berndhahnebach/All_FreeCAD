/** \file Properties.cpp
 *  \brief Template file to copy for own files
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
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
#include "Properties.h"



//**************************************************************************
//**************************************************************************
// FCProperty
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
FCProperty::FCProperty(){}

FCProperty::~FCProperty(){}



//**************************************************************************
//**************************************************************************
// FCPropertyInteger
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


FCPropertyInteger::FCPropertyInteger(long lValue, long lMax, long lMin, long lStep)
	:_lValue(lValue),_lMax(lMax), _lMin(lMin), _lStep(lStep)
{

}


FCPropertyInteger::~FCPropertyInteger()
{

}

//**************************************************************************
// Base class implementer

const char* FCPropertyInteger::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%ld",_lValue);
	return sBuf;
}



const char* FCPropertyInteger::GetType(void)
{
	return "Int";
}

const char* FCPropertyInteger::SetSubProperty(const char* sSubPropName,const char* sNewValue)
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


const char* FCPropertyInteger::GetSubProperty(const char* sSubPropName)
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


const char* FCPropertyInteger::GetSubPropertyNames(void)
{
	return "Value;Max;Min;Step";
}

//**************************************************************************
// Seter getter for the property

void FCPropertyInteger::SetMin(long lMin)
{
	_lMin=lMin;
}

long FCPropertyInteger::GetMin(void)
{
	return _lMin;
}

void FCPropertyInteger::SetMax(long lMax)
{
	_lMax=lMax;
}

long FCPropertyInteger::GetMax(void)
{
	return _lMax;
}

void FCPropertyInteger::SetStep(long lStep)
{
	_lStep=lStep;
}

long FCPropertyInteger::GetStep(void)
{
	return _lStep;
}

void FCPropertyInteger::SetValue(long lValue)
{
	_lValue=lValue;
}

long FCPropertyInteger::GetValue(void)
{
	return _lValue;
}

//**************************************************************************
//**************************************************************************
// FCPropertyFloat
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


FCPropertyFloat::FCPropertyFloat(double dValue, double dMax, double dMin, double dStep)
	:_dValue(dValue),_dMax(dMax), _dMin(dMin), _dStep(dStep)
{

}


FCPropertyFloat::~FCPropertyFloat()
{

}

//**************************************************************************
// Base class implementer

const char* FCPropertyFloat::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%f",_dValue);
	return sBuf;
}



const char* FCPropertyFloat::GetType(void)
{
	return "Float";
}

const char* FCPropertyFloat::SetSubProperty(const char* sSubPropName,const char* sNewValue)
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


const char* FCPropertyFloat::GetSubProperty(const char* sSubPropName)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%f",_dValue);

	if(strcmp(sSubPropName,"Value")==0)
		sprintf(sBuf,"%f",_dValue);
	else if(strcmp(sSubPropName,"Max")==0)
		sprintf(sBuf,"%f",_dMax);
	else if(strcmp(sSubPropName,"Min")==0)
		sprintf(sBuf,"%f",_dMin);
	else if(strcmp(sSubPropName,"Step")==0)
		sprintf(sBuf,"%f",_dStep);
	else return "";

	return sBuf;
}


const char* FCPropertyFloat::GetSubPropertyNames(void)
{
	return "Value;Max;Min;Step";
}

//**************************************************************************
// Seter getter for the property

void FCPropertyFloat::SetMin(double dMin)
{
	_dMin=dMin;
}

double FCPropertyFloat::GetMin(void)
{
	return _dMin;
}

void FCPropertyFloat::SetMax(double dMax)
{
	_dMax=dMax;
}

double FCPropertyFloat::GetMax(void)
{
	return _dMax;
}

void FCPropertyFloat::SetStep(double dStep)
{
	_dStep=dStep;
}

double FCPropertyFloat::GetStep(void)
{
	return _dStep;
}

void FCPropertyFloat::SetValue(double lValue)
{
	_dValue=lValue;
}

double FCPropertyFloat::GetValue(void)
{
	return _dValue;
}

//**************************************************************************
//**************************************************************************
// FCPropertyString
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

       
FCPropertyString::FCPropertyString(const char* sValue, const char* sConstraint)
	:_cValue(sValue),_cConstrain(sConstraint)
{

}


FCPropertyString::~FCPropertyString()
{

}

//**************************************************************************
// Base class implementer

const char* FCPropertyString::GetAsString(void)
{
	return _cValue.c_str();
}



const char* FCPropertyString::GetType(void)
{
	return "String";
}

const char* FCPropertyString::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{

	if(strcmp(sSubPropName,"Value")==0)
		_cValue = sNewValue;
	else if(strcmp(sSubPropName,"Constrain")==0)
		_cConstrain = sNewValue;
	else 
		return "";

	return GetSubProperty(sSubPropName);
}


const char* FCPropertyString::GetSubProperty(const char* sSubPropName)
{
	if(strcmp(sSubPropName,"Value")==0)
		return _cValue.c_str();
	else if(strcmp(sSubPropName,"Constrain")==0)
		return _cConstrain.c_str();
	else 
		return "";
}


const char* FCPropertyString::GetSubPropertyNames(void)
{
	return "Value;Constrain";
}

//**************************************************************************
// Seter getter for the property


void FCPropertyString::SetString(const char* sString)
{
	_cValue = sString;
}

const char* FCPropertyString::GetString(void)
{
	return _cValue.c_str();
}

void FCPropertyString::SetConstrain(const char* sConstrains)
{
	_cConstrain = sConstrains;
}

const char* FCPropertyString::GetConstrain(void)
{
	return _cConstrain.c_str();
}




//**************************************************************************
//**************************************************************************
// FCPropertyBool
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

       
FCPropertyBool::FCPropertyBool(bool lValue)
	:_lValue(lValue)
{

}


FCPropertyBool::~FCPropertyBool()
{

}

//**************************************************************************
// Base class implementer

const char* FCPropertyBool::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
  sprintf(sBuf,"%s",(_lValue ? "True" : "False"));
	return sBuf;
}



const char* FCPropertyBool::GetType(void)
{
	return "Bool";
}

const char* FCPropertyBool::SetSubProperty(const char* sSubPropName,const char* sNewValue)
{

	if(strcmp(sSubPropName,"Value")==0)
    _lValue = atol(sNewValue) > 0 ? true : false;
	else return "";

	return GetSubProperty(sSubPropName);
}


const char* FCPropertyBool::GetSubProperty(const char* sSubPropName)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"%d",_lValue);

	if(strcmp(sSubPropName,"Value")==0)
    sprintf(sBuf,"%s",(_lValue ? "True" : "False"));
	else return "";

	return sBuf;
}


const char* FCPropertyBool::GetSubPropertyNames(void)
{
	return "Value";
}

//**************************************************************************
// Seter getter for the property

void FCPropertyBool::SetValue(bool lValue)
{
	_lValue=lValue;
}

bool FCPropertyBool::GetValue(void)
{
	return _lValue;
}

//**************************************************************************
//**************************************************************************
// FCPropertyColor
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

       
FCPropertyColor::FCPropertyColor(long lRed, long lGreen, long lBlue)
	:_lRed(lRed),_lGreen(lGreen), _lBlue(lBlue)
{

}


FCPropertyColor::~FCPropertyColor()
{

}

//**************************************************************************
// Base class implementer

const char* FCPropertyColor::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
	sprintf(sBuf,"[%ld, %ld, %ld]",_lRed, _lGreen, _lBlue);
	return sBuf;
}



const char* FCPropertyColor::GetType(void)
{
	return "Color";
}

const char* FCPropertyColor::SetSubProperty(const char* sSubPropName,const char* sNewValue)
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


const char* FCPropertyColor::GetSubProperty(const char* sSubPropName)
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


const char* FCPropertyColor::GetSubPropertyNames(void)
{
	return "Red;Green;Blue";
}

//**************************************************************************
// Seter getter for the property

void FCPropertyColor::SetRed(long lRed)
{
	_lRed=lRed;
}

long FCPropertyColor::GetRed(void)
{
	return _lRed;
}

void FCPropertyColor::SetGreen(long lGreen)
{
	_lGreen=lGreen;
}

long FCPropertyColor::GetGreen(void)
{
	return _lGreen;
}

void FCPropertyColor::SetBlue(long lBlue)
{
	_lBlue=lBlue;
}

long FCPropertyColor::GetBlue(void)
{
	return _lBlue;
}

//**************************************************************************
//**************************************************************************
// FCPropertyList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

       
FCPropertyList::FCPropertyList(const std::vector<std::string>& lValue, long lCurrent)
	:_lValue(lValue), _lCurrent(lCurrent)
{

}


FCPropertyList::~FCPropertyList()
{

}

//**************************************************************************
// Base class implementer

const char* FCPropertyList::GetAsString(void)
{
	static char sBuf[DBL_DIG+10];
  int pos = 0;
  for (std::vector<std::string>::iterator it = _lValue.begin(); it!=_lValue.end(); ++it)
  {
    if (pos+it->length() >= DBL_DIG)
    {
//      Console().Warning("FCPropertyList: List too long\n");
      break;
    }
  	pos += sprintf(sBuf+pos,"%s ",it->c_str());
  }
	return sBuf;
}



const char* FCPropertyList::GetType(void)
{
	return "List";
}

const char* FCPropertyList::SetSubProperty(const char* sSubPropName,const char* sNewValue)
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


const char* FCPropertyList::GetSubProperty(const char* sSubPropName)
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


const char* FCPropertyList::GetSubPropertyNames(void)
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

void FCPropertyList::SetValue(const std::vector<std::string>& lValue)
{
	_lValue=lValue;
}

const std::vector<std::string>& FCPropertyList::GetValue(void)
{
	return _lValue;
}

void FCPropertyList::SetCurrentItem (long lCurrent)
{
  _lCurrent = lCurrent;
}

long FCPropertyList::GetCurrentItem (void)
{
  return _lCurrent;
}
