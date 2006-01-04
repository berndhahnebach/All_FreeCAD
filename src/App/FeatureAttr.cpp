/** \file $RCSfile$
 *  \brief The attribute module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

#include "PreCompiled.h"

#ifndef _PreComp_
#	include <TDF_Label.hxx>
#	include <TDF_ChildIterator.hxx>
#	include <TDF_Tool.hxx>
#	include <TCollection_AsciiString.hxx>
#	include <TDF_ListIteratorOfAttributeList.hxx>
#	include <Standard_GUID.hxx>
#endif

#include "FeatureAttr.h"

using namespace App;


//**************************************************************************
//**************************************************************************
// Handle_FeatureAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Handle_FeatureAttr::~Handle_FeatureAttr() {}

AppExport Handle_Standard_Type& App::STANDARD_TYPE(FeatureAttr)
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(FCAttribute);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(FCAttribute);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(TDF_Attribute);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(TDF_Attribute);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(MMgt_TShared);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FeatureAttr",
			                                 sizeof(FeatureAttr),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(FeatureAttr) Handle(FeatureAttr)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FeatureAttr) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FeatureAttr))) {
       _anOtherObject = Handle(FeatureAttr)((Handle(FeatureAttr)&)AnObject);
     }
  }

  return _anOtherObject ;
}




//**************************************************************************
//**************************************************************************
// FeatureAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& FeatureAttr::GetID () 
{
  static Standard_GUID FeatureAttrID("F46EEE9A-F770-4eae-A832-C55ECD5F8FE2");
  return FeatureAttrID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle_FeatureAttr FeatureAttr::Set(const TDF_Label& label,Feature  * S) 
{
  Handle(FeatureAttr) N;
  if (!label.FindAttribute(FeatureAttr::GetID(), N)) { 
    N = new FeatureAttr ();   
    label.AddAttribute(N);
  }
  N->Set(S);    
  return N;  
}

FeatureAttr::FeatureAttr () {}
FeatureAttr::~FeatureAttr () {}

void FeatureAttr::Set (Feature  * S) 
{
 
	_pcFeature = S;

//  Backup();
//  myString2 = S;
  //TCollection_ExtendedString tmpS(S);
  //tmpS.RemoveAll(':');
  //myString = tmpS;
  //myEmpty = Standard_False;
}

Feature  *FeatureAttr::Get () const 
{
	return _pcFeature;
}

const Standard_GUID& FeatureAttr::ID () const { return GetID(); }


Handle(TDF_Attribute) FeatureAttr::NewEmpty () const
{  
  return new FeatureAttr(); 
}

void FeatureAttr::Restore(const Handle(TDF_Attribute)& with) 
{
  _pcFeature = Handle(FeatureAttr)::DownCast (with)->Get();
}


void FeatureAttr::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  Handle(FeatureAttr)::DownCast (into)->Set (_pcFeature);
}


Standard_OStream& FeatureAttr::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  //anOS << " Name=|"<<myString2<<"|"<<endl;
  return anOS;
}

const Handle(Standard_Type)& FeatureAttr::DynamicType() const 
{ 
  return STANDARD_TYPE(FeatureAttr) ; 
}

Standard_Boolean FeatureAttr::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FeatureAttr) == AType || TDF_Attribute::IsKind(AType)); 
}

//**************************************************************************
//**************************************************************************
// FeatureAttrStorageDriver
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


FeatureAttrStorageDriver::FeatureAttrStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer FeatureAttrStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) FeatureAttrStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Name);}

Handle(PDF_Attribute) FeatureAttrStorageDriver::NewEmpty() const
{ return new FeatureAttrPersistent; }

void FeatureAttrStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Name) S = Handle(TDataStd_Name)::DownCast (Source);
  Handle(FeatureAttrPersistent) T = Handle(FeatureAttrPersistent)::DownCast (Target);
#ifdef BUGOCC425
  if(S->Get().Length() != 0) {
#endif 
    Handle(PCollection_HExtendedString) Name = new PCollection_HExtendedString (S->Get());
    T->Set (Name);
#ifdef BUGOCC425
  }
#endif 
}


FeatureAttrStorageDriver::~FeatureAttrStorageDriver() {}
 


AppExport Handle_Standard_Type& App::FeatureAttrStorageDriver_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(MDF_ASDriver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(MDF_ASDriver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FeatureAttrStorageDriver",
			                                 sizeof(FeatureAttrStorageDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(FeatureAttrStorageDriver) Handle(FeatureAttrStorageDriver)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FeatureAttrStorageDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FeatureAttrStorageDriver))) {
       _anOtherObject = Handle(FeatureAttrStorageDriver)((Handle(FeatureAttrStorageDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& FeatureAttrStorageDriver::DynamicType() const 
{ 
  return STANDARD_TYPE(FeatureAttrStorageDriver) ; 
}
Standard_Boolean FeatureAttrStorageDriver::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FeatureAttrStorageDriver) == AType || MDF_ASDriver::IsKind(AType)); 
}
Handle_FeatureAttrStorageDriver::~Handle_FeatureAttrStorageDriver() {}



//**************************************************************************
//**************************************************************************
// FeatureAttrPersistent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

FeatureAttrPersistent::FeatureAttrPersistent() {}

FeatureAttrPersistent::FeatureAttrPersistent(const Handle(PCollection_HExtendedString)& V)
: myValue (V) {}

Handle(PCollection_HExtendedString) FeatureAttrPersistent::Get() const
{ return myValue; }

void FeatureAttrPersistent::Set(const Handle(PCollection_HExtendedString)& V) 
{ myValue = V; }


FeatureAttrPersistent::~FeatureAttrPersistent() {}


AppExport Handle_Standard_Type& App::FeatureAttrPersistent_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(PDF_Attribute);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(PDF_Attribute);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(Standard_Persistent);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(Standard_Persistent);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Storable);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Storable);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FeatureAttrPersistent",
			                                 sizeof(FeatureAttrPersistent),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(FeatureAttrPersistent) Handle(FeatureAttrPersistent)::DownCast(const Handle(Standard_Persistent)& AnObject) 
{
  Handle(FeatureAttrPersistent) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FeatureAttrPersistent))) {
       _anOtherObject = Handle(FeatureAttrPersistent)((Handle(FeatureAttrPersistent)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& FeatureAttrPersistent::DynamicType() const 
{ 
  return STANDARD_TYPE(FeatureAttrPersistent) ; 
}
Standard_Boolean FeatureAttrPersistent::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FeatureAttrPersistent) == AType || PDF_Attribute::IsKind(AType)); 
}
Handle_FeatureAttrPersistent::~Handle_FeatureAttrPersistent() {}
