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




//**************************************************************************
//**************************************************************************
// Handle_FCFeatureAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Handle_FCFeatureAttr::~Handle_FCFeatureAttr() {}

Standard_EXPORT Handle_Standard_Type& FCFeatureAttr_Type_()
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
  static Handle_Standard_Type _aType = new Standard_Type("FCFeatureAttr",
			                                 sizeof(FCFeatureAttr),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

// DownCast method
//   allow safe downcasting
//
const Handle(FCFeatureAttr) Handle(FCFeatureAttr)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FCFeatureAttr) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCFeatureAttr))) {
       _anOtherObject = Handle(FCFeatureAttr)((Handle(FCFeatureAttr)&)AnObject);
     }
  }

  return _anOtherObject ;
}




//**************************************************************************
//**************************************************************************
// FCFeatureAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& FCFeatureAttr::GetID () 
{
  static Standard_GUID FCFeatureAttrID("F46EEE9A-F770-4eae-A832-C55ECD5F8FE2");
  return FCFeatureAttrID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle_FCFeatureAttr FCFeatureAttr::Set(const TDF_Label& label,FCFeature  * S) 
{
  Handle(FCFeatureAttr) N;
  if (!label.FindAttribute(FCFeatureAttr::GetID(), N)) { 
    N = new FCFeatureAttr ();   
    label.AddAttribute(N);
  }
  N->Set(S);    
  return N;  
}

FCFeatureAttr::FCFeatureAttr () {}
FCFeatureAttr::~FCFeatureAttr () {}

void FCFeatureAttr::Set (FCFeature  * S) 
{
 
  Backup();
//  myString2 = S;
  //TCollection_ExtendedString tmpS(S);
  //tmpS.RemoveAll(':');
  //myString = tmpS;
  //myEmpty = Standard_False;
}

FCFeature  *FCFeatureAttr::Get () const {return _pcFeature;}

const Standard_GUID& FCFeatureAttr::ID () const { return GetID(); }


Handle(TDF_Attribute) FCFeatureAttr::NewEmpty () const
{  
  return new FCFeatureAttr(); 
}

void FCFeatureAttr::Restore(const Handle(TDF_Attribute)& with) 
{
  _pcFeature = Handle(FCFeatureAttr)::DownCast (with)->Get();
}


void FCFeatureAttr::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  Handle(FCFeatureAttr)::DownCast (into)->Set (_pcFeature);
}


Standard_OStream& FCFeatureAttr::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  //anOS << " Name=|"<<myString2<<"|"<<endl;
  return anOS;
}

const Handle(Standard_Type)& FCFeatureAttr::DynamicType() const 
{ 
  return STANDARD_TYPE(FCFeatureAttr) ; 
}

Standard_Boolean FCFeatureAttr::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FCFeatureAttr) == AType || TDF_Attribute::IsKind(AType)); 
}





