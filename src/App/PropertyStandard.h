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
	long getValue(void) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::XMLReader &reader);


private:

	long _lValue;

};

/** Integer list properties
 * 
 */
class AppExport PropertyIntegerList: public PropertyLists
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyIntegerList();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyIntegerList();

  virtual void setSize(int newSize){_lValueList.resize(newSize);}   
  virtual int getSize(void){return _lValueList.size();}   

	/** Sets the property 
	 */
	void setValue(long);
  
  /// index operator
  long operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  
  
  void  set1Value (const int idx, long value){_lValueList.operator[] (idx) = value;}

  const std::vector<long> &getValues(void) const{return _lValueList;}

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::XMLReader &reader);


private:

  std::vector<long> _lValueList;

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


	void setValue(float lValue);
	float getValue(void) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::XMLReader &reader);

private:

	float _dValue;

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
  virtual void Restore(Base::XMLReader &reader);

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
	bool getValue(void) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::XMLReader &reader);

private:

	bool _lValue;
};



} // namespace App

#endif // __PropteryStandard_H__
