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
	sprintf(sBuf,"%d",_lValue);
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
	sprintf(sBuf,"%d",_lValue);

	if(strcmp(sSubPropName,"Value")==0)
		sprintf(sBuf,"%d",_lValue);
	else if(strcmp(sSubPropName,"Max")==0)
		sprintf(sBuf,"%d",_lMax);
	else if(strcmp(sSubPropName,"Min")==0)
		sprintf(sBuf,"%d",_lMin);
	else if(strcmp(sSubPropName,"Step")==0)
		sprintf(sBuf,"%d",_lStep);
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



