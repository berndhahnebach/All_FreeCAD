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
#endif

#include "Function.h"




FCFunction::~FCFunction() {}
 



//=======================================================================
//function : GetID
//purpose  :
//=======================================================================

const Standard_GUID& FCFunction::GetID() {
  static Standard_GUID anID("681D5B67-465F-4b07-8BF6-F153C9D8B404");
  return anID;
}


//=======================================================================
//function : TPartStd_CutDriver
//purpose  : Creation of an instance of the driver. It's possible (and recommended)
//         : to have only one instance of a driver for the whole session.
//=======================================================================

FCFunction::FCFunction()
{}

//=======================================================================
//function : Validate
//purpose  : Validation of the object label, its arguments and its results.
//=======================================================================

void FCFunction::Validate(TFunction_Logbook& log) const
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

Standard_Boolean FCFunction::MustExecute(const TFunction_Logbook& log) const
{
  // If the object's label is modified:
  if (log.IsModified(Label())) return Standard_True; 

  // Cut (in our simple case) has two arguments: The original shape, and the tool shape.
  // They are on the child labels of the cut's label:
  // So, OriginalNShape  - is attached to the first  child label
  //     ToolNShape - is attached to the second child label,
  //     .
  // Let's check them:
  Handle(TDF_Reference) OriginalRef;
  Label().FindChild(1).FindAttribute(TDF_Reference::GetID(),OriginalRef);
  if (log.IsModified(OriginalRef->Get()))   return Standard_True; // Original shape.

  Handle(TDF_Reference) ToolRef;
  Label().FindChild(2).FindAttribute(TDF_Reference::GetID(),ToolRef);
  if (log.IsModified(ToolRef->Get()))   return Standard_True; // Tool shape.
  
  // if there are no any modifications concerned the cut,
  // it's not necessary to recompute (to call the method Execute()):
  return Standard_False;
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

Standard_Integer FCFunction::Execute(TFunction_Logbook& log) const
{
  // Let's get the arguments (OriginalNShape, ToolNShape of the object):

	// First, we have to retrieve the TDF_Reference attributes to obtain the root labels of the OriginalNShape and the ToolNShape:
/*	Handle(TDF_Reference)  OriginalRef, ToolRef;
	if (!Label().FindChild(1).FindAttribute(TDF_Reference::GetID(), OriginalRef )) return 1;
	TDF_Label OriginalLab = OriginalRef->Get();
	if (!Label().FindChild(2).FindAttribute(TDF_Reference::GetID(), ToolRef)) return 1;
	TDF_Label ToolLab = ToolRef->Get();

	// Get the TNaming_NamedShape attributes of these labels
	Handle(TNaming_NamedShape) OriginalNShape, ToolNShape;
	if (!( OriginalLab.FindAttribute(TNaming_NamedShape::GetID(),OriginalNShape) ))
		Standard_Failure::Raise("TOcaf_Commands::CutObjects");		
	if (!( ToolLab.FindAttribute(TNaming_NamedShape::GetID(),ToolNShape) ))
		Standard_Failure::Raise("TOcaf_Commands::CutObjects");		

	// Now, let's get the TopoDS_Shape of these TNaming_NamedShape:
	TopoDS_Shape OriginalShape  = OriginalNShape->Get();
	TopoDS_Shape ToolShape = ToolNShape->Get();

// STEP 2:
	// Let's call for algorithm computing a cut operation:
	BRepAlgoAPI_Cut mkCut(OriginalShape, ToolShape);
	// Let's check if the Cut has been successfull:
	if (!mkCut.IsDone()) 
	{
		MessageBox(0,"Cut not done.","Cut Function Driver",MB_ICONERROR);
		return 2;
	}
	TopoDS_Shape ResultShape = mkCut.Shape();

	// Build a TNaming_NamedShape using built cut
	TNaming_Builder B(Label());
	B.Modify( OriginalShape, ResultShape);
// That's all:
  // If there are no any mistakes we return 0:
  */
	return 0;
}



Standard_EXPORT Handle_Standard_Type& FCFunction_Type_()
{

//  static Handle_Standard_Type aType1 = STANDARD_TYPE(FCFunction);
//  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(FCFunction);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(TFunction_Driver);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(MMgt_TShared);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {/*aType1,*/aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FCFunction",
			                                 sizeof(FCFunction),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(FCFunction) Handle(FCFunction)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FCFunction) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCFunction))) {
       _anOtherObject = Handle(FCFunction)((Handle(FCFunction)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& FCFunction::DynamicType() const 
{ 
  return STANDARD_TYPE(FCFunction) ; 
}
Standard_Boolean FCFunction::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FCFunction) == AType || TFunction_Driver::IsKind(AType)); 
}

Handle_FCFunction::~Handle_FCFunction() {}

