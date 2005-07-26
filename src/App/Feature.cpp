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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <TDF_ChildIterator.hxx>
# include <TDF_Tool.hxx>
# include <TCollection_AsciiString.hxx>
# include <TDF_ListIteratorOfAttributeList.hxx>
# include <TFunction_Logbook.hxx>
# include <TFunction_DriverTable.hxx>
# include <TFunction_Function.hxx>
# include <Standard_GUID.hxx>
# include <TNaming_Builder.hxx>
# include <TDataStd_Name.hxx>
# include <sstream>
#endif

#include <Base/PyExportImp.h>
#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;

#include "Document.h"
#include "Feature.h"
#include "FeaturePy.h"
#include "FeatureAttr.h"
#include "Function.h"
#include "Property.h"
#include "PropertyAttr.h"

using namespace App;

//===========================================================================
// Feature
//===========================================================================

Feature::Feature(void)
:_nextFreeLabel(1),pcFeaturePy(0)
{
  _solidMaterial.ambientColor.set(0.2f,0.2f,0.2f);
  _solidMaterial.diffuseColor.set(0.8f,0.8f,0.8f);
  _solidMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _solidMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _solidMaterial.shininess = 0.2f;
  _solidMaterial.transparency = 0.0f;

  _lineMaterial.ambientColor.set(0.2f,0.2f,0.2f);
  _lineMaterial.diffuseColor.set(0.1f,0.1f,0.1f);
  _lineMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _lineMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _lineMaterial.shininess = 0.0f;
  _lineMaterial.transparency = 0.0f;
  _lineSize = 2.0f;

  _pointMaterial.ambientColor.set(0.9f,0.9f,0.9f);
  _pointMaterial.diffuseColor.set(0.8f,0.8f,0.8f);;
  _pointMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _pointMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _pointMaterial.shininess = 0.3f;
  _pointSize = 2.0;
  _showMode = "Flat";
}

Feature::~Feature(void)
{
  pcFeaturePy->DecRef();
}




void Feature::addProperty(const char *Type, const char *Name)
{
	TDF_Label L = _cFeatureLabel.FindChild(_nextFreeLabel);
	
  if(	Base::streq(Type, "Float") )
   	TDataStd_Real::Set(L, 0.0);
  else if(	Base::streq(Type, "String") )
   	TDataStd_Name::Set(L,"");
  else if(	Base::streq(Type, "Int") )
   	TDataStd_Integer::Set(L,0);
  else if(	Base::streq(Type, "Link") )
   	TDF_Reference::Set(L,TDF_Label());
	
  // remember for search effecience
  _PropertiesMap[Name] = _nextFreeLabel;
  TDataStd_Comment::Set(L,TCollection_ExtendedString((Standard_CString) Name ));
  
  _nextFreeLabel++;
}

const char *Feature::getPropertyType(const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyType() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  if( L.IsAttribute(TDataStd_Real::GetID()) ) return "Float";
  if( L.IsAttribute(TDataStd_Name::GetID()) ) return "String";
  if( L.IsAttribute(TDataStd_Integer::GetID()) ) return "Int";
  if( L.IsAttribute(TDF_Reference::GetID()) ) return "Link";

  return "";

}


double Feature::getPropertyFloat(const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDataStd_Real) RealAttr;

 	if (!L.FindAttribute(TDataStd_Real::GetID(), RealAttr )) 
    throw Base::Exception("Type mismatch, no Float attribute!");

  return RealAttr->Get();
}

void Feature::setPropertyFloat(double d, const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDataStd_Real) RealAttr;

 	if (!L.FindAttribute(TDataStd_Real::GetID(), RealAttr )) 
    throw Base::Exception("Type mismatch, no Float attribute!");

  RealAttr->Set(d);

}

long Feature::getPropertyInt(const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyInt() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDataStd_Integer) IntAttr;

 	if (!L.FindAttribute(TDataStd_Integer::GetID(), IntAttr )) 
    throw Base::Exception("Type mismatch, no Int attribute!");

  return IntAttr->Get();
}

void Feature::setPropertyInt(long d, const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDataStd_Integer) IntAttr;

 	if (!L.FindAttribute(TDataStd_Integer::GetID(), IntAttr )) 
    throw Base::Exception("Type mismatch, no Int attribute!");

  IntAttr->Set(d);

}

std::string Feature::getPropertyString(const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyString() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDataStd_Name) StrAttr;

 	if (!L.FindAttribute(TDataStd_Name::GetID(), StrAttr )) 
    throw Base::Exception("Type mismatch, no string attribute!");

  return std::string(TCollection_AsciiString(StrAttr->Get()).ToCString());
}

void Feature::setPropertyString(const char* s, const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDataStd_Name) StrAttr;

 	if (!L.FindAttribute(TDataStd_Name::GetID(), StrAttr )) 
    throw Base::Exception("Type mismatch, no string attribute!");

  StrAttr->Set((Standard_CString)s);

}

void Feature::setPropertyLink(Feature *pcToLink, const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDF_Reference) RefAttr;

 	if (!L.FindAttribute(TDF_Reference::GetID(), RefAttr )) 
    throw Base::Exception("Type mismatch, no Link attribute!");

  RefAttr->Set( pcToLink->Label());
}

Feature *Feature::getPropertyLink(const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyLink() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(TDF_Reference) RefAttr;

 	if (!L.FindAttribute(TDF_Reference::GetID(), RefAttr )) 
    throw Base::Exception("Type mismatch, no Link attribute!");

  TDF_Label Link = RefAttr->Get();
  Handle(FeatureAttr) LinkFeat;

  if (!Link.FindAttribute(FeatureAttr::GetID(), LinkFeat)) 
    throw Base::Exception("Feature::getPropertyLink() link dont link a feature!");

  return LinkFeat->Get();

}

/*
Property &Feature::GetProperty(const char *Name)
{
  std::map<std::string,int>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::GetProperty() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second);

  Handle(PropertyAttr) PropAttr;

 	if (!L.FindAttribute(PropertyAttr::GetID(), PropAttr )) 
    throw Base::Exception("Feature::GetProperty() Internal error, no PropertyAttr attached to label");


  return *(PropAttr->Get());
}

double Feature::GetFloatProperty(const char *Name)
{
    return dynamic_cast<PropertyFloat&>(GetProperty(Name)).GetValue();
}

const char *Feature::GetStringProperty(const char *Name)
{
    return dynamic_cast<PropertyString&>(GetProperty(Name)).GetAsString();
}

*/

void Feature::TouchProperty(const char *Name)
{
  std::map<std::string,int>::iterator it = _PropertiesMap.find( Name );
  if ( it != _PropertiesMap.end() )
  {
    _pDoc->TouchState( _cFeatureLabel.FindChild( it->second ) );
    _pDoc->TouchState( _cFeatureLabel );
  }
}

void Feature::Touch(void)
{
    _pDoc->TouchState( _cFeatureLabel );
}

void Feature::AttachLabel(const TDF_Label &rcLabel,Document* dt)
{
	_cFeatureLabel = rcLabel;
  _pDoc = dt;

	// Add the one and only FreeCAD FunctionDriver to the driver Tabel !!! Move to APPinit !!!
	//Handle(TFunction_Driver) myDriver = new Function();
	//TFunction_DriverTable::Get()->AddDriver(Function::GetID(),myDriver);


	// Instanciate a TFunction_Function attribute connected to the current box driver
	// and attach it to the data structure as an attribute of the Box Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(_cFeatureLabel, Function::GetID());

	// Initialize and execute the box driver (look at the "Execute()" code)
//    TFunction_Logbook log;

	Handle(Function) myFunctionDriver;
    // Find the TOcafFunction_BoxDriver in the TFunction_DriverTable using its GUID
	TFunction_DriverTable::Get()->FindDriver(Function::GetID(), myFunctionDriver);

	myFunctionDriver->Init(_cFeatureLabel);

	InitLabel(_cFeatureLabel);

//   if (myFunctionDriver->Execute(log))
//		Base::Console().Error("Feature::InitLabel()");
//		Base::Console().Log("Feature::InitLabel() Initial Execute Fails (not bad at all ;-)\n");

}

bool Feature::MustExecute(const TFunction_Logbook& log)
{
	Base::Console().Log("Feature::MustExecute()\n");

  // If the object's label is modified:
  if (log.IsModified(_cFeatureLabel)) 
    return Standard_True;

  // checks if a known property has changed
  for(std::map<std::string,int>::const_iterator It = _PropertiesMap.begin();It!=_PropertiesMap.end();It++)
  {
    if (log.IsModified( _cFeatureLabel.FindChild(It->second) ))
      return Standard_True;
  }

  return false;

}

const char* Feature::getStatusMessage(void) const
{
  switch (_eStatus)
  {
  case Valid:
    return "Valid";
  case New:
    return "New";
  case Changed:
    return "Changed";
  case Inactive:
    return "Inactive";
  case Error:
    return "Error";
  default:
    return "Unknown";
  }
}

Feature *Feature::GetFeature(const TDF_Label &l)
{
  Handle(FeatureAttr) hFeat;
  
  if(! l.FindAttribute(FeatureAttr::GetID(),hFeat))
    //throw Base::Exception("DocTypeStd::GetFeature() internal error, feature attribute missing\n");
    return 0;
  
  return hFeat->Get();
}




//===========================================================================
// FeatureFactorySingleton - Factory for Features
//===========================================================================



FeatureFactorySingleton* FeatureFactorySingleton::_pcSingleton = NULL;

FeatureFactorySingleton& FeatureFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new FeatureFactorySingleton;
  return *_pcSingleton;
}

void FeatureFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

Feature* FeatureFactorySingleton::Produce (const char* sName) const
{
	Feature* w = (Feature*)Factory::Produce(sName);

  // this Feature class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    Console().Warning("\"%s\" is not registered\n", sName);
#else
    Console().Log("\"%s\" is not registered\n", sName);
#endif
    return NULL;
  }

  return w;
}

