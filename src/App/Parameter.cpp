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

#include "Parameter.h"


//**************************************************************************
//**************************************************************************
// Handle_FCParameter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Handle_FCParameter::~Handle_FCParameter() {}

Standard_EXPORT Handle_Standard_Type& FCParameter_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(FCAttribute);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(FCAttribute);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(TDF_Attribute);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(TDF_Attribute);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(MMgt_TShared);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FCParameter",
			                                 sizeof(FCParameter),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

// DownCast method
//   allow safe downcasting
//
const Handle(FCParameter) Handle(FCParameter)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FCParameter) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCParameter))) {
       _anOtherObject = Handle(FCParameter)((Handle(FCParameter)&)AnObject);
     }
  }

  return _anOtherObject ;
}




//**************************************************************************
//**************************************************************************
// FCParameter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& FCParameter::GetID () 
{
  static Standard_GUID FCParameterID("3C445C0D-147A-4549-AC7E-B782889E475E");
  return FCParameterID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle_FCParameter FCParameter::Set(const TDF_Label& label,const TCollection_ExtendedString& string) 
{
  Handle(FCParameter) N;
  if (!label.FindAttribute(FCParameter::GetID(), N)) { 
    N = new FCParameter ();   
    label.AddAttribute(N);
  }
  N->Set(string);    
  return N;  
}

FCParameter::FCParameter () {}
FCParameter::~FCParameter () {}

void FCParameter::Set (const TCollection_ExtendedString& S) 
{
 
  Backup();
  myString2 = S;
  //TCollection_ExtendedString tmpS(S);
  //tmpS.RemoveAll(':');
  //myString = tmpS;
  //myEmpty = Standard_False;
}

TCollection_ExtendedString FCParameter::Get () const {return myString2;}

const Standard_GUID& FCParameter::ID () const { return GetID(); }


Handle(TDF_Attribute) FCParameter::NewEmpty () const
{  
  return new FCParameter(); 
}

void FCParameter::Restore(const Handle(TDF_Attribute)& with) 
{
  myString2 = Handle(FCParameter)::DownCast (with)->Get();
}


void FCParameter::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  Handle(FCParameter)::DownCast (into)->Set (myString2);
}


Standard_OStream& FCParameter::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << " Name=|"<<myString2<<"|"<<endl;
  return anOS;
}

const Handle(Standard_Type)& FCParameter::DynamicType() const 
{ 
  return STANDARD_TYPE(FCParameter) ; 
}

Standard_Boolean FCParameter::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FCParameter) == AType || TDF_Attribute::IsKind(AType)); 
}





