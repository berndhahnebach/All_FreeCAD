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
# include <OSD_Process.hxx>
# include <TDF_ChildIterator.hxx>
# include <TDF_Tool.hxx>
# include <TCollection_AsciiString.hxx>
# include <TDataStd_Comment.hxx>
# include <TDataStd_Integer.hxx>
# include <TDataStd_Real.hxx>
# include <TDF_ListIteratorOfAttributeList.hxx>
# include <TDF_Label.hxx>
# include <TDF_Reference.hxx>
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
#include <Base/Vector3D.h>
#include <Base/Matrix.h>
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

  OSD_Process pro;
  touchTime = pro.SystemDate();
  touchViewTime = pro.SystemDate();
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
  _PropertiesMap[Name].Label = _nextFreeLabel;
  OSD_Process pro;
  _PropertiesMap[Name].Time = pro.SystemDate();

  TDataStd_Comment::Set(L,TCollection_ExtendedString((Standard_CString) Name ));
  
  _nextFreeLabel++;
}

const char *Feature::getPropertyType(const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyType() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  if( L.IsAttribute(TDataStd_Real::GetID()) ) return "Float";
  if( L.IsAttribute(TDataStd_Name::GetID()) ) return "String";
  if( L.IsAttribute(TDataStd_Integer::GetID()) ) return "Int";
  if( L.IsAttribute(TDF_Reference::GetID()) ) return "Link";

  return "";

}


void Feature::setPropertyVector(const Vector3D& p, const char *Name)
{  /*
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Point) PointAttr;

 	if (!L.FindAttribute(TDataStd_Point::GetID(), PointAttr )) 
    throw Base::Exception("Type mismatch, no Float attribute!");

  PointAttr->Set(gp_Pnt(p.x,p.y,p.z));

  OSD_Process pro;
  It->second.Time = pro.SystemDate();
*/
}

Vector3D Feature::getPropertyVector(const char *Name)
{/*
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyVector() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Point) PointAttr;

 	if (!L.FindAttribute(TDataStd_Point::GetID(), PointAttr )) 
    throw Base::Exception("Feature::getPropertyVector(): Type mismatch, no Point attribute!");

  gp_Pnt p = PointAttr->Get();
  return Vector3D(p.X(),p.Y(),p.Z());*/
  return Vector3D();
}

void Feature::setPropertyMatrix(const Matrix4D&, const char *Name)
{

}


Matrix4D Feature::getPropertyMatrix(const char *Name)
{
  return Matrix4D();
}

void setPropertyPlane(const Vector3D& Base, const Vector3D& Norm, const char *Name)
{

}

void getPropertyPlane(Vector3D& Base, Vector3D& Norm, const char *Name)
{

}


double Feature::getPropertyFloat(const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Real) RealAttr;

 	if (!L.FindAttribute(TDataStd_Real::GetID(), RealAttr )) 
    throw Base::Exception("Type mismatch, no Float attribute!");

  return RealAttr->Get();
}

void Feature::setPropertyFloat(double d, const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyFloat() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Real) RealAttr;

 	if (!L.FindAttribute(TDataStd_Real::GetID(), RealAttr )) 
    throw Base::Exception("Type mismatch, no Float attribute!");

  RealAttr->Set(d);

  OSD_Process pro;
  It->second.Time = pro.SystemDate();

}

long Feature::getPropertyInt(const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyInt() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Integer) IntAttr;

 	if (!L.FindAttribute(TDataStd_Integer::GetID(), IntAttr )) 
    throw Base::Exception("Type mismatch, no Int attribute!");

  return IntAttr->Get();
}

void Feature::setPropertyInt(long d, const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyInt() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Integer) IntAttr;

 	if (!L.FindAttribute(TDataStd_Integer::GetID(), IntAttr )) 
    throw Base::Exception("Type mismatch, no Int attribute!");

  IntAttr->Set(d);

  OSD_Process pro;
  It->second.Time = pro.SystemDate();
}

std::string Feature::getPropertyString(const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyString() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Name) StrAttr;

 	if (!L.FindAttribute(TDataStd_Name::GetID(), StrAttr )) 
    throw Base::Exception("Type mismatch, no string attribute!");

  return std::string(TCollection_AsciiString(StrAttr->Get()).ToCString());
}

void Feature::setPropertyString(const char* s, const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyString() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDataStd_Name) StrAttr;

 	if (!L.FindAttribute(TDataStd_Name::GetID(), StrAttr )) 
    throw Base::Exception("Type mismatch, no string attribute!");

  StrAttr->Set((Standard_CString)s);

  OSD_Process pro;
  It->second.Time = pro.SystemDate();
}

void Feature::setPropertyLink(Feature *pcToLink, const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::setPropertyLink() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDF_Reference) RefAttr;

 	if (!L.FindAttribute(TDF_Reference::GetID(), RefAttr )) 
    throw Base::Exception("Type mismatch, no Link attribute!");

  RefAttr->Set( pcToLink->Label());

  OSD_Process pro;
  It->second.Time = pro.SystemDate();
}

Feature *Feature::getPropertyLink(const char *Name)
{
  std::map<std::string,FeatEntry>::iterator It = _PropertiesMap.find(Name);

  if(It == _PropertiesMap.end())
    throw Base::Exception("Feature::getPropertyLink() unknown property name");

  TDF_Label L = _cFeatureLabel.FindChild(It->second.Label);

  Handle(TDF_Reference) RefAttr;

 	if (!L.FindAttribute(TDF_Reference::GetID(), RefAttr )) 
    throw Base::Exception("Type mismatch, no Link attribute!");

  TDF_Label Link = RefAttr->Get();

  // Link unset?
  if(Link.IsNull())
    return 0;

  Handle(FeatureAttr) LinkFeat;

  if (!Link.FindAttribute(FeatureAttr::GetID(), LinkFeat)) 
    throw Base::Exception("Feature::getPropertyLink() link dont link a feature!");

  return LinkFeat->Get();

}


void Feature::TouchProperty(const char *Name)
{
  std::map<std::string,FeatEntry>::iterator it = _PropertiesMap.find( Name );
  if ( it != _PropertiesMap.end() )
  {
    _pDoc->getLogBook().SetTouched( _cFeatureLabel.FindChild( it->second.Label ) );
    OSD_Process pro;
    it->second.Time = pro.SystemDate();
  }
}

void Feature::Touch(void)
{
  _pDoc->getLogBook().SetTouched( _cFeatureLabel );
  OSD_Process pro;
  touchTime = pro.SystemDate();
}

void Feature::TouchView(void)
{
  OSD_Process pro;
  touchViewTime = pro.SystemDate();
}

void Feature::AttachLabel(const TDF_Label &rcLabel,Document* dt)
{
  // remember the document and the Feature label
	_cFeatureLabel = rcLabel;
  _pDoc = dt;

  // calls the Init methode of the Feature to set up the properties and data types
	initFeature();


}
/* Old version (logbook)
bool Feature::MustExecute(void)
{
  const TFunction_Logbook& log = _pDoc->getLogBook();
  Handle(TDF_Reference) RefAttr;
  TDF_Label L;

  // If the object's label is modified:
  if (log.IsModified(_cFeatureLabel)) 
    return Standard_True;

  // checks if a known property has changed
  for(std::map<std::string,FeatEntry>::const_iterator It = _PropertiesMap.begin();It!=_PropertiesMap.end();It++)
  {
    L = _cFeatureLabel.FindChild(It->second);
    if (log.IsModified(L ))
      return Standard_True;

    // if the property is a link?
    if (L.FindAttribute(TDF_Reference::GetID(), RefAttr ))
    {
      TDF_Label Link = RefAttr->Get();
      // check link
      if (log.IsModified(Link ))
        return Standard_True;
    }
  }

  return false;

}
*/
bool Feature::MustExecute(void)
{
  Handle(TDF_Reference) RefAttr;
  TDF_Label L;

  // If the object's label is modified:
  if (getStatus() != Valid && getStatus()!= Inactive) 
    return true;

  // checks if a known property has changed
  for(std::map<std::string,FeatEntry>::const_iterator It = _PropertiesMap.begin();It!=_PropertiesMap.end();It++)
  {
    L = _cFeatureLabel.FindChild(It->second.Label);
    // if the property is a link?
    if (L.FindAttribute(TDF_Reference::GetID(), RefAttr ))
    {
      TDF_Label Link = RefAttr->Get();
      if(Link.IsNull())
        return false;
      // get the linked feature
      Handle(FeatureAttr) LinkFeat;

      if (!Link.FindAttribute(FeatureAttr::GetID(), LinkFeat)) 
         throw Base::Exception("Feature::getPropertyLink() link dont link a feature!");

      if (LinkFeat->Get()->getTouchTime() > getTouchTime() || LinkFeat->Get()->getTouchTime() == getTouchTime())
        return true;
    }

    if (It->second.Time > getTouchTime() || It->second.Time == getTouchTime() )
      return true;

  }

  return false;

}

void Feature::recompute(void)
{

  _pDoc->RecomputeFeature(this);

  /*

  if(MustExecute())
  {
    execute(_pDoc->getLogBook());
    removeModifications();
    time(&touchTime);
  }*/
}

void Feature::removeModifications(void)
{
  // checks if a known property has changed
  for(std::map<std::string,FeatEntry>::const_iterator It = _PropertiesMap.begin();It!=_PropertiesMap.end();It++)
  {
    _pDoc->getLogBook().IsModified( _cFeatureLabel.FindChild(It->second.Label) );
  }
}

const char* Feature::getStatusString(void) const
{
  switch (_eStatus)
  {
  case Valid:
    return "Valid";
  case New:
    return "New";
  case Inactive:
    return "Inactive";
  case Recompute:
    return "Recompute";
  case Error:
    return "Error";
  default:
    return "Unknown";
  }
}

void Feature::setError(const char* pMsg,...)
{
  // temp buffer
  char* format = (char*) malloc(strlen(pMsg)+4024);

  va_list namelessVars;
  va_start(namelessVars, pMsg);  // Get the "..." vars
  vsprintf(format, pMsg, namelessVars);
  va_end(namelessVars);

  _eStatus = Error;
  _cErrorMessage = format;
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
    Console().Warning("FeatureFactorySingleton::Produce(): Feature type \"%s\" is not registered\n", sName);
#else
    Console().Log("\"%s\" is not registered\n", sName);
#endif
    return NULL;
  }

  return w;
}

