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

#include "Feature.h"














/*


//**************************************************************************
//**************************************************************************
// Handle_FCFeature
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Handle_FCFeature::~Handle_FCFeature() {}

Standard_EXPORT Handle_Standard_Type& FCFeature_Type_()
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
  static Handle_Standard_Type _aType = new Standard_Type("FCFeature",
			                                 sizeof(FCFeature),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

// DownCast method
//   allow safe downcasting
//
const Handle(FCFeature) Handle(FCFeature)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FCFeature) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCFeature))) {
       _anOtherObject = Handle(FCFeature)((Handle(FCFeature)&)AnObject);
     }
  }

  return _anOtherObject ;
}




//**************************************************************************
//**************************************************************************
// FCFeature
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& FCFeature::GetID () 
{
  static Standard_GUID FCFeatureID("F46EEE9A-F770-4eae-A832-C55ECD5F8FE2");
  return FCFeatureID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle_FCFeature FCFeature::Set(const TDF_Label& label,const TCollection_ExtendedString& string) 
{
  Handle(FCFeature) N;
  if (!label.FindAttribute(FCFeature::GetID(), N)) { 
    N = new FCFeature ();   
    label.AddAttribute(N);
  }
  N->Set(string);    
  return N;  
}

FCFeature::FCFeature () {}
FCFeature::~FCFeature () {}

void FCFeature::Set (const TCollection_ExtendedString& S) 
{
 
  Backup();
  myString2 = S;
  //TCollection_ExtendedString tmpS(S);
  //tmpS.RemoveAll(':');
  //myString = tmpS;
  //myEmpty = Standard_False;
}

TCollection_ExtendedString FCFeature::Get () const {return myString2;}

const Standard_GUID& FCFeature::ID () const { return GetID(); }


Handle(TDF_Attribute) FCFeature::NewEmpty () const
{  
  return new FCFeature(); 
}

void FCFeature::Restore(const Handle(TDF_Attribute)& with) 
{
  myString2 = Handle(FCFeature)::DownCast (with)->Get();
}


void FCFeature::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  Handle(FCFeature)::DownCast (into)->Set (myString2);
}


Standard_OStream& FCFeature::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << " Name=|"<<myString2<<"|"<<endl;
  return anOS;
}

const Handle(Standard_Type)& FCFeature::DynamicType() const 
{ 
  return STANDARD_TYPE(FCFeature) ; 
}

Standard_Boolean FCFeature::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FCFeature) == AType || TDF_Attribute::IsKind(AType)); 
}

*/



