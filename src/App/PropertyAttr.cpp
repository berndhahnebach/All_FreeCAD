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
#	include <TDF_Label.hxx>
#	include <TDF_ChildIterator.hxx>
#	include <TDF_Tool.hxx>
#	include <TCollection_AsciiString.hxx>
#	include <TDF_ListIteratorOfAttributeList.hxx>
#	include <Standard_GUID.hxx>
#endif

#include "PropertyAttr.h"

using namespace App;

//**************************************************************************
//**************************************************************************
// Handle_PropertyAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Handle_PropertyAttr::~Handle_PropertyAttr() {}

Standard_EXPORT Handle_Standard_Type& App::PropertyAttr_Type_()
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
  static Handle_Standard_Type _aType = new Standard_Type("PropertyAttr",
			                                 sizeof(PropertyAttr),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

// DownCast method
//   allow safe downcasting
//
const Handle(PropertyAttr) Handle(PropertyAttr)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(PropertyAttr) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(PropertyAttr))) {
       _anOtherObject = Handle(PropertyAttr)((Handle(PropertyAttr)&)AnObject);
     }
  }

  return _anOtherObject ;
}




//**************************************************************************
//**************************************************************************
// PropertyAttr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& PropertyAttr::GetID () 
{
  static Standard_GUID PropertyAttrID("3C445C0D-147A-4549-AC7E-B782889E475E");
  return PropertyAttrID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle_PropertyAttr PropertyAttr::Set(const TDF_Label& label,const TCollection_ExtendedString& string) 
{
  Handle(PropertyAttr) N;
  if (!label.FindAttribute(PropertyAttr::GetID(), N)) { 
    N = new PropertyAttr ();   
    label.AddAttribute(N);
  }
  N->Set(string);    
  return N;  
}

PropertyAttr::PropertyAttr () {}
PropertyAttr::~PropertyAttr () {}

void PropertyAttr::Set (const TCollection_ExtendedString& S) 
{
 
  Backup();
  myString2 = S;
  //TCollection_ExtendedString tmpS(S);
  //tmpS.RemoveAll(':');
  //myString = tmpS;
  //myEmpty = Standard_False;
}

TCollection_ExtendedString PropertyAttr::Get () const {return myString2;}

const Standard_GUID& PropertyAttr::ID () const { return GetID(); }


Handle(TDF_Attribute) PropertyAttr::NewEmpty () const
{  
  return new PropertyAttr(); 
}

void PropertyAttr::Restore(const Handle(TDF_Attribute)& with) 
{
  myString2 = Handle(PropertyAttr)::DownCast (with)->Get();
}


void PropertyAttr::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  Handle(PropertyAttr)::DownCast (into)->Set (myString2);
}


Standard_OStream& PropertyAttr::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << " Name=|"<<myString2<<"|"<<endl;
  return anOS;
}

const Handle(Standard_Type)& PropertyAttr::DynamicType() const 
{ 
  return STANDARD_TYPE(PropertyAttr) ; 
}

Standard_Boolean PropertyAttr::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(PropertyAttr) == AType || TDF_Attribute::IsKind(AType)); 
}





