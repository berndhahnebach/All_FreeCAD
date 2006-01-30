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


#ifndef __PropteryStandard_H__
#define __PropteryStandard_H__

// Std. configurations


#include <string>
#include <vector>

#ifdef _MSC_VER
# pragma warning( disable : 4251 )
#endif


#include "Property.h"

namespace Base {
  class Writer;
}


namespace App
{



/** Integer properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyInteger: public Property
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyInteger();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyInteger();

	/** Sets the property 
	 */
	void setValue(long);

	/** This method returns a string representation of the property
	 */
	long getValue(void);

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::Reader &reader);


private:

	long _lValue;

};


/** Integer properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyFloat: public Property
{
  TYPESYSTEM_HEADER();

public:

       
	/** Value Constructor
	 *  Construct with explicite Values
	 */
	PropertyFloat(void);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyFloat();


	void setValue(double lValue);
	double getValue(void);

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::Reader &reader);

private:

	double _dValue;

};


/** String properties
 * This is the father of all properties handling Strings.
 */
class AppExport PropertyString: public Property
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyString(void);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyString();


	void setValue(const char* sString);
  void setValue(const std::string &sString);
	const char* getValue(void) const;

  bool isEmpty(void){return _cValue == "";}

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::Reader &reader);

private:

	std::string _cValue;

};

/** Bool properties
 * This is the father of all properties handling booleans.
 */
class AppExport PropertyBool : public Property
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyBool(void);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyBool();

	void setValue(bool lValue);
	bool getValue(void);

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::Reader &reader);

private:

	bool _lValue;
};

#if 0

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
	virtual void Set(const char* Str="");

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
#endif

} // namespace App

#endif // __PropteryStandard_H__
