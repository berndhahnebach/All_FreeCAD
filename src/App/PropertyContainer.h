/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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


#ifndef __PropteryContainer_H__
#define __PropteryContainer_H__

#include <map>
#include <Base/Persistance.h>

namespace Base {
  class Writer;
}


namespace App
{
  class Property;
  class PropertyContainer;
  class DocumentObject;

enum AppExport PropertyType 
{
  Prop_None     = 0,
  Prop_ReadOnly = 1,
  Prop_Output   = 2,
  Prop_Hidden   = 4
};

struct AppExport PropertyData
{
  struct PropertySpec
  {
    short Offset,Type;
    const char * Group;
  };
  std::map<std::string,PropertySpec> propertyData;
  const PropertyData *parentPropertyData;

  void addProperty(PropertyContainer *container,const char* PropName, Property *Prop, const char* PropertyGroup= 0, PropertyType = Prop_None );
  const char* getName(const PropertyContainer *container,const Property* prop) const;
  short getType(const PropertyContainer *container,const Property* prop) const;
  const char* getGroup(const PropertyContainer *container,const Property* prop) const;
  Property *getPropertyByName(const PropertyContainer *container,const char* name) const;
  void getPropertyMap(const PropertyContainer *container,std::map<std::string,Property*> &Map) const;
  void getPropertyList(const PropertyContainer *container,std::vector<Property*> &List) const;

};


/** Base class of all classes with propteries
 */
class AppExport PropertyContainer: public Base::Persistance
{

  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyContainer();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyContainer();

  virtual unsigned int getMemSize (void) const;

  /// find a property by its name
  virtual Property *getPropertyByName(const char* name) const;
  /// get the name of a property
  virtual const char* getName(const Property* prop) const;
  /// get all properties of the class (including properies of the parent)
  virtual void getPropertyMap(std::map<std::string,Property*> &Map) const;
  /// get all properties of the class (including properies of the parent)
  virtual void getPropertyList(std::vector<Property*> &List) const;

  /// get the Type of a Property
  short getPropertyType(const Property* prop) const 
  {return propertyData.getType(this,prop);}
  /// get the Group of a Property
  const char* getPropertyGroup(const Property* prop) const
  {return propertyData.getGroup(this,prop);}

  /// returns a list of objects this object is pointing to by Links
  std::vector<DocumentObject*> getOutList(void);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);


  friend class Property;

	
protected: 
  /// get called by the container when a Proptery was changed
  virtual void onChanged(const Property* prop){};
  /// get called befor the value is changed
  virtual void onBevorChange(const Property* prop){};

  //void hasChanged(Propterty* prop);
  static const  PropertyData * getPropertyDataPtr(void); 
  virtual const PropertyData& getPropertyData(void) const; 

 private: 
  static PropertyData propertyData; 

};

/// Property define 
#define ADD_PROPERTY(_prop_, _defaultval_) \
  do { \
    this->_prop_.setValue _defaultval_;\
    this->_prop_.setContainer(this); \
    propertyData.addProperty(this, #_prop_, &this->_prop_); \
  } while (0)

#define ADD_PROPERTY_TYPE(_prop_, _defaultval_, _group_,_type_) \
  do { \
    this->_prop_.setValue _defaultval_;\
    this->_prop_.setContainer(this); \
    propertyData.addProperty(this, #_prop_, &this->_prop_, (_group_),(_type_)); \
  } while (0)



#define PROPERTY_HEADER(_class_) \
  TYPESYSTEM_HEADER(); \
protected: \
  static const App::PropertyData * getPropertyDataPtr(void); \
  virtual const App::PropertyData &getPropertyData(void) const; \
private: \
  static App::PropertyData propertyData 

/// 
#define PROPERTY_SOURCE(_class_, _parentclass_) \
TYPESYSTEM_SOURCE_P(_class_);\
const App::PropertyData * _class_::getPropertyDataPtr(void){return &propertyData;} \
const App::PropertyData & _class_::getPropertyData(void) const{return propertyData;} \
App::PropertyData _class_::propertyData; \
void _class_::init(void){\
  initSubclass(_class_::classTypeId, #_class_ , #_parentclass_, &(_class_::create) ); \
  _class_::propertyData.parentPropertyData = _parentclass_::getPropertyDataPtr();\
}

#define PROPERTY_SOURCE_ABSTRACT(_class_, _parentclass_) \
TYPESYSTEM_SOURCE_ABSTRACT_P(_class_);\
const App::PropertyData * _class_::getPropertyDataPtr(void){return &propertyData;} \
const App::PropertyData & _class_::getPropertyData(void) const{return propertyData;} \
App::PropertyData _class_::propertyData; \
void _class_::init(void){\
  initSubclass(_class_::classTypeId, #_class_ , #_parentclass_, &(_class_::create) ); \
  _class_::propertyData.parentPropertyData = _parentclass_::getPropertyDataPtr();\
}



} // namespace App

#endif // __PropteryContainer_H__
