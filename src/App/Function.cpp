/** \file Function.cpp
 *  \brief Template file to copy for own files
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
 *  @see Parameter.h FCParameter FCParameterManager
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <Standard_GUID.hxx>
#	include <TFunction_Logbook.hxx>
#	include <TDF_Reference.hxx>
#endif

#include "../Base/Console.h"
#include "../Base/Exception.h"

#include "Function.h"
#include "FeatureAttr.h"
#include "Feature.h"

using namespace App;


//=======================================================================
//function : Function
//purpose  : Creation of an instance of the driver. It's possible (and recommended)
//         : to have only one instance of a driver for the whole session.
//=======================================================================

Function::Function(void) {}

Function::~Function() {}
 



//=======================================================================
//function : GetID
//purpose  :
//=======================================================================

const Standard_GUID& Function::GetID() {
  static Standard_GUID anID("681D5B67-465F-4b07-8BF6-F153C9D8B404");
  return anID;
}


//=======================================================================
//function : Validate
//purpose  : Validation of the object label, its arguments and its results.
//=======================================================================

void Function::Validate(TFunction_Logbook& log) const
{
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log.SetValid(Label(), Standard_True);
}

//=======================================================================
//function : MustExecute
//purpose  : We call this method to check if the object was modified to
//         : be invoked. If the object label or an argument is modified,
//         : we must recompute the object - to call the method Execute().
//=======================================================================

Standard_Boolean Function::MustExecute(const TFunction_Logbook& log) const
{
	Base::Console().Log("Function::MustExecute()\n");

	Handle(FeatureAttr) myFeatureAttr;
	// Get the Feature Attribute of this label
	if(! Label().FindAttribute(FeatureAttr::GetID(), myFeatureAttr )) 
		throw FCException("Function::Execute():Wrong document Strukture");
	
	// call the Exectute of the App::Feature
	return myFeatureAttr->Get()->MustExecute(log);

}

//=======================================================================
//function : Execute
//purpose  : 
//         : We compute the object and topologically name it.
//         : If during the execution we found something wrong,
//         : we return the number of the failure. For example:
//         : 1 - an attribute hasn't been found,
//         : 2 - algorithm failed,
//         : if there are no any mistakes occurred we return 0:
//         : 0 - no mistakes were found.
//=======================================================================

Standard_Integer Function::Execute(TFunction_Logbook& log) const
{
	Base::Console().Log("Function::Execute()\n");

	Handle(FeatureAttr) myFeatureAttr;
	// Get the Feature Attribute of this label
	if(! Label().FindAttribute(FeatureAttr::GetID(), myFeatureAttr )) 
		throw FCException("Function::Execute():Wrong document Strukture");
	
	// call the Exectute of the App::Feature
	return myFeatureAttr->Get()->Execute(log);

	
}



Standard_EXPORT Handle_Standard_Type& App::Function_Type_()
{

//  static Handle_Standard_Type aType1 = STANDARD_TYPE(Function);
//  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(Function);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(TFunction_Driver);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(MMgt_TShared);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {/*aType1,*/aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("Function",
			                                 sizeof(Function),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(Function) Handle(Function)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(Function) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(Function))) {
       _anOtherObject = Handle(Function)((Handle(Function)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& Function::DynamicType() const 
{ 
  return STANDARD_TYPE(Function) ; 
}
Standard_Boolean Function::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(Function) == AType || TFunction_Driver::IsKind(AType)); 
}

Handle_Function::~Handle_Function() {}

