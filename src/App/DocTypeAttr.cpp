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

#include "DocTypeAttr.h"
using App::DocType;

//**************************************************************************
//**************************************************************************
// Handle_FCDocTypeAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Handle_FCDocTypeAttr::~Handle_FCDocTypeAttr() {}

Standard_EXPORT Handle_Standard_Type& FCDocTypeAttr_Type_()
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
  static Handle_Standard_Type _aType = new Standard_Type("FCDocTypeAttr",
			                                 sizeof(FCDocTypeAttr),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

// DownCast method
//   allow safe downcasting
//
const Handle(FCDocTypeAttr) Handle(FCDocTypeAttr)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FCDocTypeAttr) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCDocTypeAttr))) {
       _anOtherObject = Handle(FCDocTypeAttr)((Handle(FCDocTypeAttr)&)AnObject);
     }
  }

  return _anOtherObject ;
}




//**************************************************************************
//**************************************************************************
// FCDocTypeAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/// Constructor
FCDocTypeAttr::FCDocTypeAttr(DocType *pcDocType)
:_pcDocType(pcDocType)
{

}

FCDocTypeAttr::~FCDocTypeAttr () {}


DocType *FCDocTypeAttr::GetType(void)
{
	return _pcDocType;
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& FCDocTypeAttr::GetID () 
{
  static Standard_GUID FCDocTypeAttrID("6F7E2EA0-D9A4-4b2c-8C4E-B4175D9BAA31");
  return FCDocTypeAttrID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle_FCDocTypeAttr FCDocTypeAttr::Set(const TDF_Label& label,DocType *pcDocType) 
{
  Handle(FCDocTypeAttr) N;
  if (!label.FindAttribute(FCDocTypeAttr::GetID(), N)) { 
    N = new FCDocTypeAttr ();   
    label.AddAttribute(N);
  }
  N->Set(pcDocType);    
  return N;  
}


void FCDocTypeAttr::Set (DocType *pcDocType) 
{
 
  Backup();
  _pcDocType = pcDocType;
}

DocType * FCDocTypeAttr::Get () const {return _pcDocType;}

const Standard_GUID& FCDocTypeAttr::ID () const { return GetID(); }


Handle(TDF_Attribute) FCDocTypeAttr::NewEmpty () const
{  
  return new FCDocTypeAttr(); 
}

void FCDocTypeAttr::Restore(const Handle(TDF_Attribute)& with) 
{
  //myString2 = Handle(FCDocTypeAttr)::DownCast (with)->Get();
}


void FCDocTypeAttr::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  //Handle(FCDocTypeAttr)::DownCast (into)->Set (myString2);
}


Standard_OStream& FCDocTypeAttr::Dump (Standard_OStream& anOS) const
{
  //TDF_Attribute::Dump(anOS);
  //anOS << " Name=|"<<myString2<<"|"<<endl;
  return anOS;
}

const Handle(Standard_Type)& FCDocTypeAttr::DynamicType() const 
{ 
  return STANDARD_TYPE(FCDocTypeAttr) ; 
}

Standard_Boolean FCDocTypeAttr::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FCDocTypeAttr) == AType || TDF_Attribute::IsKind(AType)); 
}





