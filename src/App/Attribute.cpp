/** \file $RCSfile$
 *  \brief The attribute module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <TDF_Label.hxx>
#	include <TDF_ChildIterator.hxx>
#	include <TDF_Tool.hxx>
#	include <TCollection_AsciiString.hxx>
#	include <TDF_ListIteratorOfAttributeList.hxx>
#	include <Standard_GUID.hxx>
#endif

#include "Attribute.h"


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Name::GetID () 
{
  static Standard_GUID TDataStd_NameID("2a96b608-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_NameID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_Name) TDataStd_Name::Set(const TDF_Label& label,const TCollection_ExtendedString& string) 
{
  Handle(TDataStd_Name) N;
  if (!label.FindAttribute(TDataStd_Name::GetID(), N)) { 
    N = new TDataStd_Name ();   
    label.AddAttribute(N);
  }
  N->Set(string);    
  return N;  
}

TDataStd_Name::TDataStd_Name () {}

void TDataStd_Name::Set (const TCollection_ExtendedString& S) 
{
 
  Backup();
  myString = S;
  //TCollection_ExtendedString tmpS(S);
  //tmpS.RemoveAll(':');
  //myString = tmpS;
  //myEmpty = Standard_False;
}

TCollection_ExtendedString TDataStd_Name::Get () const {return myString;}

const Standard_GUID& TDataStd_Name::ID () const { return GetID(); }


Handle(TDF_Attribute) TDataStd_Name::NewEmpty () const
{  
  return new TDataStd_Name(); 
}

void TDataStd_Name::Restore(const Handle(TDF_Attribute)& with) 
{
  myString = Handle(TDataStd_Name)::DownCast (with)->Get();
}


void TDataStd_Name::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_Name)::DownCast (into)->Set (myString);
}


Standard_OStream& TDataStd_Name::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << " Name=|"<<myString<<"|"<<endl;
  return anOS;
}


