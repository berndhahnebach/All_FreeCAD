/** \file Properties.h
 *  \brief FileTemplate example header
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


#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

// Std. configurations

//#include "PyExport.h"

#include <string>


/** Base class of all Properties
 * This is the father of all properties. Properties are
 * are object which are used in the document tree to parametrize 
 * e.g. features and ist grafical output. They also used to 
 * gain acces from the scripting facility.
 * /par
 * This abstract base class defines all methodes shared by all
 * possible properties. Its also possible to define user properties
 * and use them in the frame work....
 */
class AppExport FCProperty
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	FCProperty();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~FCProperty();

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
class AppExport FCPropertyInteger: public FCProperty
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	FCPropertyInteger(long lValue, long lMax=LONG_MAX, long lMin=LONG_MIN, long lStep=1);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~FCPropertyInteger();

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
class AppExport FCPropertyFloat: public FCProperty
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	FCPropertyFloat(double dValue, double dMax=DBL_MAX, double dMin=DBL_MIN, double dStep=1);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~FCPropertyFloat();

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
class AppExport FCPropertyString: public FCProperty
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	FCPropertyString(const char* sValue, const char* sConstraint="");

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~FCPropertyString();

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

#endif // __PROPERTIES_H__