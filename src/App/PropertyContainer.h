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

#include <Base/Persistance.h>

namespace Base {
  class Writer;
}


namespace App
{
  class Property;
  class PropertyContainer;

struct AppExport PropertyData
{
  std::map<std::string,int> propertyData;
  const PropertyData *parentPropertyData;

  void addProperty(PropertyContainer *container,const char* PropName, Property *Prop);
  const char* getName(const PropertyContainer *container,const Property* prop) const;
  Property *getPropertyByName(PropertyContainer *container,const char* name) const;
  void getPropertyMap(PropertyContainer *container,std::map<std::string,Property*> &Map) const;

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

  /// get called by the container when a Proptery was changed
  virtual void onChanged(Property* prop){};


  Property *getPropertyByName(const char* name);
  const char* getName(Property* prop) const;
  void getPropertyMap(std::map<std::string,Property*> &Map);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::Reader &reader);


  friend class Property;

	
protected: 
  //void hasChanged(Propterty* prop);
  static const  PropertyData * getPropertyDataPtr(void); 
  virtual const PropertyData& getPropertyData(void) const; 
private: 
  static PropertyData propertyData; 

};


#define ADD_PROPERTY(_prop_, _defaultval_) \
  do { \
    this->_prop_.setValue _defaultval_;\
    this->_prop_.setContainer(this); \
    propertyData.addProperty(this, #_prop_, &this->_prop_); \
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
  initSubclass(_class_::classTypeId, #_class_ , #_parentclass_ ); \
  _class_::propertyData.parentPropertyData = _parentclass_::getPropertyDataPtr();\
}



} // namespace App

#endif // __PropteryContainer_H__
