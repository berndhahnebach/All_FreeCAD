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
  _lineMaterial.diffuseColor.set(0.1f,0.1f,0.1f);
  _lineSize = 2.0f;
  _pointMaterial.diffuseColor.set(0.1f,0.1f,0.1f);;
  _pointSize = 1.0;
  _showMode = "Shaded";
}

Feature::~Feature(void)
{
  pcFeaturePy->DecRef();
}


void Feature::AddProperty(const char *Type, const char *Name, const char *InitString)
{
  Property *Prop;

	
  if(	Base::streq(Type, "Float") )
	{
		Prop = new PropertyFloat();
		Prop->Set(InitString);
  } else if(	Base::streq(Type, "String") )
	{
		Prop = new PropertyString();
		Prop->Set(InitString);
  } else if(	Base::streq(Type, "Int") )
	{
		Prop = new PropertyInteger();
		Prop->Set(InitString);
  } else if(	Base::streq(Type, "Bool") )
	{
		Prop = new PropertyBool();
		Prop->Set(InitString);
	}


	TDF_Label L = _cFeatureLabel.FindChild(_nextFreeLabel);
  // remember for search effecience
  _PropertiesMap[Name] = _nextFreeLabel;
  _nextFreeLabel++;


	TDataStd_Name::Set(L,TCollection_ExtendedString((Standard_CString) Name ));
	PropertyAttr ::Set(L,Prop);

}


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


void Feature::TouchProperty(const char *Name)
{
    _pDoc->TouchState( _cFeatureLabel.FindChild(_PropertiesMap[Name]) );
    _pDoc->TouchState( _cFeatureLabel );
}

void Feature::AttachLabel(const TDF_Label &rcLabel,Document* dt)
{
	_cFeatureLabel = rcLabel;
  _pDoc = dt;

	// Add the one and only FreeCAD FunctionDriver to the driver Tabel !!! Move to APPinit !!!
	Handle(TFunction_Driver) myDriver = new Function();
	TFunction_DriverTable::Get()->AddDriver(Function::GetID(),myDriver);


	// Instanciate a TFunction_Function attribute connected to the current box driver
	// and attach it to the data structure as an attribute of the Box Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(_cFeatureLabel, Function::GetID());

	// Initialize and execute the box driver (look at the "Execute()" code)
    TFunction_Logbook log;

	Handle(Function) myFunctionDriver;
    // Find the TOcafFunction_BoxDriver in the TFunction_DriverTable using its GUID
	if(!TFunction_DriverTable::Get()->FindDriver(Function::GetID(), myFunctionDriver))
  {
    // Werner: What's that???
//		myFunctionDriver;
  }

	myFunctionDriver->Init(_cFeatureLabel);

	InitLabel(_cFeatureLabel);

   if (myFunctionDriver->Execute(log))
//		Base::Console().Error("Feature::InitLabel()");
		Base::Console().Log("Feature::InitLabel() Initial Execute Fails (not bad at all ;-)\n");

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

