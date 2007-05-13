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
#include <list>
#include <vector>
#include <boost/filesystem/path.hpp >

#ifdef _MSC_VER
# pragma warning( disable : 4251 )
#endif


#include "Property.h"
#include "Material.h"

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
  
	PropertyInteger();
	~PropertyInteger();

	/** Sets the property 
	 */
	void setValue(long);

	/** This method returns a string representation of the property
	 */
	long getValue(void) const;

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::IntEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return sizeof(long);}

protected:

	long _lValue;

};

/** Path properties
 * This properties handling file system paths.
 */
class AppExport PropertyPath: public Property
{
  TYPESYSTEM_HEADER();

public:
  
	PropertyPath();
	~PropertyPath();

	/** Sets the property 
	 */
	void setValue(const boost::filesystem::path &);

  /** Sets the property 
	 */
	void setValue(const char *);

	/** This method returns a string representation of the property
	 */
  boost::filesystem::path getValue(void) const;

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::TextEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return _cValue.string().size();}

protected:

	boost::filesystem::path _cValue;

};

/** Enum properties
 * This property fullfill the need of enumarations. It holds basicly a 
 * state (integer) and a list of valid state names. If the valid state
 * list is not set it act basicly like a IntegerProperty and do no checking.
 * If the list is set it checks on the range and if you set the state with
 * a string if its included in the enumarations.
 * In DEBUG the bounderies get checked, otherwise the caller of setValue()
 * has the responsebility to check the correctnes.
 * This mean if you set by setValue(const char*) with an not included value
 * and not using isPartOf()bevor,
 * in DEBUG you get an assert() in release its set to 0.
 */
class AppExport PropertyEnumeration: public PropertyInteger
{
  TYPESYSTEM_HEADER();

public:
	/// Standard constructor
	PropertyEnumeration();

	/// destructor
	~PropertyEnumeration();

  /// Enumeration methods 
  //@{
  /** setting the enumaration string list
    * The list is a NULL terminated array of pointers to a const char* string
    * \code
    * const char enums[] = {"Black","White","Other",NULL}
    * \endcode
    */
  void setEnums(const char** plEnums);
  /** set the enum by a string
    * is slower the setValue(long). Use long if possible
    */
  void setValue(const char* value);
  /** set directly the enum value
    * In DEBUG checks for bounderies.
    * Is faster then using setValue(const char*).
    */
  void setValue(long);
  /// checks if the property is set to a certain string value
  bool isValue(const char* value) const;
  /// checks if a string is included in the enumeration
  bool isPartOf(const char* value) const;
  /// get the value as string
  const char* getValueAsString(void) const;
  /// get all possible enum values as vector of strings
  std::vector<std::string> getEnumVector(void) const;
  /// get the pointer to the enum list
  const char** getEnums(void) const;
  //@}

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::ComboEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

private:
	const char** _EnumArray;
};
/** Constraint integer properties
 * This property fullfill the need of constraint integer. It holds basicly a 
 * state (integer) and a struct of bounderies. If the bounderies
 * is not set it act basicly like a IntegerProperty and do no checking.
 * The constraints struct can be created on the heap or build in.
 */

class AppExport PropertyIntegerConstraint: public PropertyInteger
{
  TYPESYSTEM_HEADER();

public:
	/// Standard constructor
	PropertyIntegerConstraint();

	/// destructor
	~PropertyIntegerConstraint();

  /// Constraint methods 
  //@{
  /// the boundery struct
  struct Constraints {
    long LowerBound, UpperBound, StepSize;
  };
  /** setting the bounderies
    * This sets the constriant struct. It can be dynamcly 
    * allocated or set as an static in the class the property
    * blongs to:
    * \code
    * const Constraints percent = {0,100,1}
    * \endcode
    */
  void setConstraints(const Constraints* sConstraint);
  /// get the constriant struct
  const Constraints*  getConstraints(void) const;
  //@}

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::IntConstraintEditorItem"; }

  virtual void setPyObject(PyObject *);

protected:
	const Constraints* _ConstStruct;
};

/** Percent property
 * This property is a special interger property and holds only
 * numbers between 0 and 100.
 */

class AppExport PropertyPercent: public PropertyIntegerConstraint
{
  TYPESYSTEM_HEADER();

public:
	/// Standard constructor
	PropertyPercent();

	/// destructor
	~PropertyPercent();
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
  virtual int getSize(void) const {return _lValueList.size();}   

	/** Sets the property 
	 */
	void setValue(long);
  
  /// index operator
  long operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  
  
  void  set1Value (const int idx, long value){_lValueList.operator[] (idx) = value;}
  void setValues (const std::vector<long>& values);

  const std::vector<long> &getValues(void) const{return _lValueList;}

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return _lValueList.size() * sizeof(long);}
private:

  std::vector<long> _lValueList;

};


/** Float properties
 * This is the father of all properties handling floats.
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

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::FloatEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return sizeof(float);}

protected:

	float _dValue;

};

/** Constraint float properties
 * This property fullfill the need of constraint float. It holds basicly a 
 * state (float) and a struct of bounderies. If the bounderies
 * is not set it act basicly like a IntegerProperty and do no checking.
 * The constraints struct can be created on the heap or build in.
 */
class AppExport PropertyFloatConstraint: public PropertyFloat
{
  TYPESYSTEM_HEADER();

public:

       
	/** Value Constructor
	 *  Construct with explicite Values
	 */
	PropertyFloatConstraint(void);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyFloatConstraint();


 /// Constraint methods 
  //@{
  /// the boundery struct
  struct Constraints {
    float LowerBound, UpperBound, StepSize;
  };
  /** setting the bounderies
    * This sets the constriant struct. It can be dynamcly 
    * allocated or set as an static in the class the property
    * blongs to:
    * \code
    * const Constraints percent = {0.0,100.0,1.0}
    * \endcode
    */
  void setConstraints(const Constraints* sConstrain);
  /// get the constriant struct
  const Constraints*  getConstraints(void) const;
  //@}

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::FloatConstraintEditorItem"; }

  virtual void setPyObject(PyObject *);


protected:
	const Constraints* _ConstStruct;


};

/** Distance property
 * This is a property for representing distances. It basicly a float
 * property. On the Gui it has a quantity like m or mm.
 */
class AppExport PropertyDistance: public PropertyFloat
{
  TYPESYSTEM_HEADER();
public:
  PropertyDistance(void){}
  virtual ~PropertyDistance(){}
  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::FloatEditorItem"; }
};

/** Angle property
 * This is a property for representing angles. It basicly a float
 * property. On the Gui it has a quantity like RAD.
 */
class AppExport PropertyAngle: public PropertyFloat
{
  TYPESYSTEM_HEADER();
public:
  PropertyAngle(void){}
  virtual ~PropertyAngle(){}
  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::FloatEditorItem"; }
};


class AppExport PropertyFloatList: public PropertyLists
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyFloatList();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyFloatList();

  virtual void setSize(int newSize){_lValueList.resize(newSize);}   
  virtual int getSize(void) const {return _lValueList.size();}   

	/** Sets the property 
	 */
	void setValue(float);
  
  /// index operator
  float operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  
  
  void set1Value (const int idx, float value){_lValueList.operator[] (idx) = value;}
  void setValues (const std::vector<float>& values);

  const std::vector<float> &getValues(void) const{return _lValueList;}

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual void SaveDocFile (Base::Writer &writer) const;
  virtual void RestoreDocFile(Base::Reader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return _lValueList.size() * sizeof(float);}

private:
  std::vector<float> _lValueList;
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

  bool isEmpty(void){return _cValue.empty();}

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::TextEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return _cValue.size();}

private:

	std::string _cValue;

};

class AppExport PropertyStringList: public PropertyLists
{
  TYPESYSTEM_HEADER();

public:
       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyStringList();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyStringList();

  virtual void setSize(int newSize){_lValueList.resize(newSize);}   
  virtual int getSize(void) const {return _lValueList.size();}   

	/** Sets the property 
	 */
  void setValue(const std::string&);
  void setValue(const std::vector<std::string>&);
  void setValue(const std::list<std::string>&);
  
  /// index operator
  const std::string& operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  
  void  set1Value (const int idx, const std::string& value){_lValueList.operator[] (idx) = value;}

  const std::vector<std::string> &getValues(void) const{return _lValueList;}

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::ListEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const;


private:
  std::vector<std::string> _lValueList;
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

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::BoolEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return sizeof(bool);}

private:

	bool _lValue;
};


/** Color properties
 * This is the father of all properties handling colors.
 */
class AppExport PropertyColor : public Property
{
  TYPESYSTEM_HEADER();

public:
      
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyColor();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyColor();

	/** Sets the property 
	 */
  void setValue(const Color &col);
  void setValue(float r, float g, float b, float a=0.0f);

	/** This method returns a string representation of the property
	 */
	const Color &getValue(void) const;

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::ColorEditorItem"; }

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return sizeof(Color);}


private:
  Color _cCol;
};

class AppExport PropertyColorList: public PropertyLists
{
  TYPESYSTEM_HEADER();

public:
       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyColorList();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyColorList();

  virtual void setSize(int newSize){_lValueList.resize(newSize);}   
  virtual int getSize(void) const {return _lValueList.size();}   

	/** Sets the property 
	 */
	void setValue(const Color&);
  
  /// index operator
  const Color& operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  
  void  set1Value (const int idx, const Color& value){_lValueList.operator[] (idx) = value;}

  const std::vector<Color> &getValues(void) const{return _lValueList;}

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual void SaveDocFile (Base::Writer &writer) const;
  virtual void RestoreDocFile(Base::Reader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return _lValueList.size() * sizeof(Color);}

private:
  std::vector<Color> _lValueList;
};

/** Material properties
 * This is the father of all properties handling colors.
 */
class AppExport PropertyMaterial : public Property
{
  TYPESYSTEM_HEADER();

public:
      
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyMaterial();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyMaterial();

	/** Sets the property 
	 */
  void setValue(const Material &mat);
  void setAmbientColor(const Color& col);
  void setDiffuseColor(const Color& col);
  void setSpecularColor(const Color& col);
  void setEmmisiveColor(const Color& col);
  void setShininess(float);
  void setTransparency(float);

	/** This method returns a string representation of the property
	 */
	const Material &getValue(void) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

  virtual unsigned int getMemSize (void) const{return sizeof(_cMat);}

private:
  Material _cMat;
};

/** File properties
 */
class AppExport PropertyFile : public PropertyString
{
  TYPESYSTEM_HEADER();

public:
	PropertyFile(void);
	virtual ~PropertyFile();

  virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::FileEditorItem"; }
};


} // namespace App

#endif // __PropteryStandard_H__
