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


#ifndef __PROPERTY_H__
#define __PROPERTY_H__

// Std. configurations

//#include "PyExport.h"

#include <string>
#include <vector>

#ifdef _MSC_VER
# pragma warning( disable : 4251 )
#endif

namespace App
{



/** Base class of all Properties
 *  This is the father of all properties. Properties are
 *  are object which are used in the document tree to parametrize 
 *  e.g. features and ist grafical output. They also used to 
 *  gain acces from the scripting facility.
 *  /par
 *  This abstract base class defines all methodes shared by all
 *  possible properties. Its also possible to define user properties
 *  and use them in the frame work....
 */
class AppExport Property
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	Property();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~Property();
	
	/** Sets the property throug a init string
	 */
	virtual void Set(const char* Str)=0;

	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more comples in cas of e.g. color
	 */
	virtual const char* GetAsString(void)=0;

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void)=0;

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue)=0;

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName)=0;

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void)=0;

};



/** Integer properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyInteger: public Property
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyInteger(long lValue, long lMax=LONG_MAX, long lMin=LONG_MIN, long lStep=1);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyInteger();

	/** Sets the property throug a init string
	 */
	virtual void Set(const char* Str);

	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more comples in cas of e.g. color
	 */
	virtual const char* GetAsString(void);

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void);

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue);

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName);

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void);

	void SetValue(long lValue);
	long GetValue(void);

	void SetMin(long lMin);
	long GetMin(void);

	void SetMax(long lMax);
	long GetMax(void);

	void SetStep(long lStep);
	long GetStep(void);

private:

	long _lValue;

	long _lMax, _lMin, _lStep;

};


/** Integer properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyFloat: public Property
{
public:

       
	/** Value Constructor
	 *  Construct with explicite Values
	 */
	PropertyFloat(double dValue = 0.0, double dMax=DBL_MAX, double dMin=DBL_MIN, double dStep=1);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyFloat();

	/** Sets the property throug a init string
	 */
	virtual void Set(const char* Str);

	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more comples in cas of e.g. color
	 */
	virtual const char* GetAsString(void);

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void);

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue);

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName);

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void);

	void SetValue(double lValue);
	double GetValue(void);

	void SetMin(double lMin);
	double GetMin(void);

	void SetMax(double lMax);
	double GetMax(void);

	void SetStep(double lStep);
	double GetStep(void);

private:

	double _dValue;

	double _dMax, _dMin, _dStep;

};


/** String properties
 * This is the father of all properties handling Strings.
 */
class AppExport PropertyString: public Property
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyString(const char* sValue, const char* sConstraint="");

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyString();

	/** Sets the property throug a init string
	 */

	virtual void Set(const char* Str);
	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more complex in cas of e.g. color
	 */
	virtual const char* GetAsString(void);

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void);

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue);

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName);

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void);

	void SetString(const char* sString);
	const char* GetString(void);

	void SetConstrain(const char* sConstrains);
	const char* GetConstrain(void);

private:

	std::string _cValue;

	std::string _cConstrain;

};

/** Bool properties
 * This is the father of all properties handling booleans.
 */
class AppExport PropertyBool : public Property
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyBool(bool lValue);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyBool();

	/** Sets the property throug a init string
	 */
	virtual void Set(const char* Str);

	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more comples in case of e.g. color
	 */
	virtual const char* GetAsString(void);

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void);

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue);

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName);

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void);

	void SetValue(bool lValue);
	bool GetValue(void);

private:

	bool _lValue;
};

/** Color properties
 * This is the father of all properties handling colors.
 */
class AppExport PropertyColor : public Property
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyColor(long lRed=0, long lGreen=0, long lBlue=0);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyColor();

	/** Sets the property throug a init string
	 */
	virtual void Set(const char* Str);

	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more comples in cas of e.g. color
	 */
	virtual const char* GetAsString(void);

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void);

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue);

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName);

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void);

	void SetRed(long lRed);
	long GetRed(void);

	void SetGreen(long lGreen);
	long GetGreen(void);

	void SetBlue(long lBlue);
	long GetBlue(void);

private:
	long _lRed, _lGreen, _lBlue;

};

/** List properties
 * This is the father of all properties handling lists.
 */
class AppExport PropertyList : public Property
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyList(const std::vector<std::string>& lValue, long lCurrent=0);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyList();

	/** Sets the property throug a init string
	 */
	virtual void Set(const char* Str);

	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more comples in cas of e.g. color
	 */
	virtual const char* GetAsString(void);

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void);

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue);

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName);

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void);

	void SetValue(const std::vector<std::string>& lValue);
	const std::vector<std::string>& GetValue(void);

  void SetCurrentItem (long lCurrent);
  long GetCurrentItem (void);

private:
  std::vector<std::string> _lValue;
  long _lCurrent;
};


} // namespace App

#endif // __PROPERTIES_H__
