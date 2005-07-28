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
# include <TopoDS_Shape.hxx>
# include <TFunction_Logbook.hxx>
#endif


#include "../../../Base/Console.h"
#include "FeaturePartCut.h"

#include <BRepAlgoAPI_Cut.hxx>


using namespace Part;

void PartCutFeature::InitLabel(const TDF_Label &rcLabel)
{
	Base::Console().Log("PartCutFeature::InitLabel()\n");

  addProperty("Link","First");
  addProperty("Link","Second");

}

Standard_Integer PartCutFeature::Execute(TFunction_Logbook& log)
{
	Base::Console().Log("PartCutFeature::Execute()\n");

  try{

    PartFeature *pcFirst  = dynamic_cast<PartFeature*>(getPropertyLink("First"));
    PartFeature *pcSecond = dynamic_cast<PartFeature*>(getPropertyLink("Second"));

    // Now, let's get the TopoDS_Shape of these TNaming_NamedShape:
	  TopoDS_Shape OriginalShape  = pcFirst->getShape();
	  TopoDS_Shape ToolShape = pcSecond->getShape();

  // STEP 2:
	  // Let's call for algorithm computing a cut operation:
	  BRepAlgoAPI_Cut mkCut(OriginalShape, ToolShape);
	  // Let's check if the Cut has been successfull:
	  if (!mkCut.IsDone()) 
		  return 2;

    TopoDS_Shape ResultShape = mkCut.Shape();


  	setShape(ResultShape);

  }
  catch(...){
    return 1;
  }

  return 0;
}


void PartCutFeature::Validate(TFunction_Logbook& log)
{
	Base::Console().Log("PartCutFeature::Validate()\n");
 
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log.SetValid(Label(), Standard_True);


}






// sample from OCC Ocaf
#ifdef false

//=======================================================================
//function : Validate
//purpose  : Validation of the object label, its arguments and its results.
//=======================================================================

void TOcafFunction_CutDriver::Validate(TFunction_Logbook& log) const
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

Standard_Boolean TOcafFunction_CutDriver::MustExecute(const TFunction_Logbook& log) const
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

Standard_Integer TOcafFunction_CutDriver::Execute(TFunction_Logbook& log) const
{
  // Let's get the arguments (OriginalNShape, ToolNShape of the object):

	// First, we have to retrieve the TDF_Reference attributes to obtain the root labels of the OriginalNShape and the ToolNShape:
	Handle(TDF_Reference)  OriginalRef, ToolRef;
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
		CheckMessageBox(0,"Cut not done.","Cut Function Driver",MB_ICONERROR);
		return 2;
	}
	TopoDS_Shape ResultShape = mkCut.Shape();

	// Build a TNaming_NamedShape using built cut
	TNaming_Builder B(Label());
	B.Modify( OriginalShape, ResultShape);
// That's all:
  // If there are no any mistakes we return 0:
  return 0;
}

TOcafFunction_CutDriver::~TOcafFunction_CutDriver() {}
 

TDF_Label TOcaf_Commands::Cut(TDF_Label ObjectLab, TDF_Label ToolObjectLab)
{
  // A data structure for our cut operation:
  // the result itself is attached to the Result label (as his name and his function attribute) 
  // its arguments (Original object label; Tool object label) are attached to the child 
  // labels of the Result Label:
  //
  // 0:1 Result Label ---> Name --->  Named shape ---> Function
  //       |
  //     0:1:1 -- Original object label Label
  //       |
  //     0:1:2 --  object label Label
  // 

	// Create a new label in the data structure for the box
    TDF_Label L = TDF_TagSource::NewChild(MainLab);

	// Create the data structure : Set a reference attribute on the Original and the Tool objects, set the name attribute
	TDF_Reference::Set(L.FindChild(1), ObjectLab);
	TDF_Reference::Set(L.FindChild(2), ToolObjectLab);
	Handle(TDataStd_Name) ObjectName;
	ObjectLab.FindAttribute(TDataStd_Name::GetID(),ObjectName);
	TDataStd_Name::Set(L, ObjectName->Get());


	// Instanciate a TFunction_Function attribute connected to the current cut driver
	// and attach it to the data structure as an attribute of the Result Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L, TOcafFunction_CutDriver::GetID());

	// Initialize and execute the cut driver (look at the "Execute()" code)
    TFunction_Logbook log;

	Handle(TOcafFunction_CutDriver) myCutDriver;
    // Find the TOcafFunction_CutDriver in the TFunction_DriverTable using its GUID 
	if(!TFunction_DriverTable::Get()->FindDriver(TOcafFunction_CutDriver::GetID(), myCutDriver)) return L;
		
	myCutDriver->Init(L);
    if (myCutDriver->Execute(log)) CheckMessageBox(0,"DFunction_Execute : failed","Cut",MB_ICONERROR);

	return L;
}

#endif
