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
# include <BRepPrimAPI_MakeBox.hxx>
# include <TFunction_Logbook.hxx>
#endif


#include "../../../Base/Console.h"
#include "FeaturePartBox.h"


using namespace Part;

void PartBoxFeature::InitLabel(const TDF_Label &rcLabel)
{
	Base::Console().Log("PartBoxFeature::InitLabel()\n");

	AddProperty("Float","x","0.0");
	AddProperty("Float","y","0.0");
	AddProperty("Float","z","0.0");
	AddProperty("Float","l","100.0");
	AddProperty("Float","h","100.0");
	AddProperty("Float","w","100.0");

}

/*
bool PartBoxFeature::MustExecute(const TFunction_Logbook& log)
{
	Base::Console().Log("PartBoxFeature::MustExecute()\n");
	return false;
}
*/
Standard_Integer PartBoxFeature::Execute(TFunction_Logbook& log)
{
	Base::Console().Log("PartBoxFeature::Execute()\n");

/*  cout << GetFloatProperty("x") << endl;
  cout << GetFloatProperty("y") << endl;
  cout << GetFloatProperty("z") << endl;
  cout << GetFloatProperty("l") << endl;
  cout << GetFloatProperty("h") << endl;
  cout << GetFloatProperty("w") << endl;*/

	double x = GetFloatProperty("x");
  double y = GetFloatProperty("y");
  double z = GetFloatProperty("z");

  try{
	// Build a box using the dimension and position attributes
	BRepPrimAPI_MakeBox mkBox( gp_Pnt( x, y, z),
                             GetFloatProperty("l"),
                             GetFloatProperty("h"),
                             GetFloatProperty("w"));

  TopoDS_Shape ResultShape = mkBox.Shape();


	SetShape(ResultShape);

  }
  catch(...){
    return 1;
  }

  return 0;
}


void PartBoxFeature::Validate(TFunction_Logbook& log)
{
	Base::Console().Log("PartBoxFeature::Validate()\n");
 
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log.SetValid(Label(), Standard_True);


}




// sample from OCC Ocaf
#ifdef false

//=======================================================================
//function :	CreateBox
//purpose  :	Create a box containing a Named shape, a name, a Function 
//				and a Data structure containing box conctruction parameters
//=======================================================================

TDF_Label TOcaf_Commands::CreateBox(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real w, Standard_Real l, Standard_Real h, const TCollection_ExtendedString& Name)
{
  // A data structure for our box:
  // the box itself is attached to the BoxLabel label (as his name and his function attribute) 
  // its arguments (dimensions: width, length and height; and position: x, y, z) 
  // are attached to the child labels of the box:
  //
  // 0:1 Box Label ---> Name --->  Named shape ---> Function
  //       |
  //     0:1:1 -- Width Label
  //       |
  //     0:1:2 -- Length Label
  //       |
  //     0:1:3 -- Height Label
  //       |
  //     0:1:4 -- X Label
  //       |
  //     0:1:5 -- Y Label
  //       |
  //     0:1:6 -- Z Label

	// Create a new label in the data structure for the box
    TDF_Label L = TDF_TagSource::NewChild(MainLab);

	// Create the data structure : Set the dimensions, position and name attributes
	TDataStd_Real::Set(L.FindChild(1), w);
	TDataStd_Real::Set(L.FindChild(2), l);
	TDataStd_Real::Set(L.FindChild(3), h);
	TDataStd_Real::Set(L.FindChild(4), x);
	TDataStd_Real::Set(L.FindChild(5), y);
	TDataStd_Real::Set(L.FindChild(6), z);
	TDataStd_Name::Set(L, Name);

	
	// Instanciate a TFunction_Function attribute connected to the current box driver
	// and attach it to the data structure as an attribute of the Box Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L, TOcafFunction_BoxDriver::GetID());

	// Initialize and execute the box driver (look at the "Execute()" code)
    TFunction_Logbook log;

	Handle(TOcafFunction_BoxDriver) myBoxDriver;
    // Find the TOcafFunction_BoxDriver in the TFunction_DriverTable using its GUID
	if(!TFunction_DriverTable::Get()->FindDriver(TOcafFunction_BoxDriver::GetID(), myBoxDriver)) return L;
	
	myBoxDriver->Init(L);
    if (myBoxDriver->Execute(log)) CheckMessageBox(0,"DFunction_Execute : failed","Box",MB_ICONERROR);

	return L;
}



//=======================================================================
//function : Validate
//purpose  : Validation of the object label, its arguments and its results.
//=======================================================================

void TSampleOcafFunction_BoxDriver::Validate(TFunction_Logbook& log) const
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
Standard_Boolean TSampleOcafFunction_BoxDriver::MustExecute(const TFunction_Logbook& log) const
{
	// If the object's label is modified:
  if (log.IsModified(Label())) return Standard_True; 

  // Cut (in our simple case) has two arguments: The original shape, and the tool shape.
  // They are on the child labels of the box's label:
  // So, OriginalNShape  - is attached to the first  child label
  //     ToolNShape - is attached to the second child label.
  // 
  // Let's check them:
  if (log.IsModified(Label().FindChild(1))) return Standard_True; // width.
  if (log.IsModified(Label().FindChild(2))) return Standard_True; // length,
  if (log.IsModified(Label().FindChild(3))) return Standard_True; // width.
  if (log.IsModified(Label().FindChild(4))) return Standard_True; // length,
  if (log.IsModified(Label().FindChild(5))) return Standard_True; // width.
  if (log.IsModified(Label().FindChild(6))) return Standard_True; // length,
  
 // if there are no any modifications concerned the box,
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
Standard_Integer TSampleOcafFunction_BoxDriver::Execute(TFunction_Logbook& log) const
{
	// Get the values of dimension and position attributes 
	Handle(TDataStd_Real) TSR;
	Standard_Real x,y,z,l,h,w;
	if (!Label().FindChild(1).FindAttribute(TDataStd_Real::GetID(), TSR )) return 1;
	l=TSR->Get();

	if (!Label().FindChild(2).FindAttribute(TDataStd_Real::GetID(), TSR )) return 1;
	h=TSR->Get();

	if (!Label().FindChild(3).FindAttribute(TDataStd_Real::GetID(), TSR )) return 1;
	w=TSR->Get();

	if (!Label().FindChild(4).FindAttribute(TDataStd_Real::GetID(), TSR )) return 1;
	x=TSR->Get();

	if (!Label().FindChild(5).FindAttribute(TDataStd_Real::GetID(), TSR )) return 1;
	y=TSR->Get();

	if (!Label().FindChild(6).FindAttribute(TDataStd_Real::GetID(), TSR )) return 1;
	z=TSR->Get();

	// Build a box using the dimension and position attributes 
	BRepPrimAPI_MakeBox mkBox( gp_Pnt(x, y ,z), l, h ,w);
	TopoDS_Shape ResultShape = mkBox.Shape();


	// Build a TNaming_NamedShape using built box
	TNaming_Builder B(Label());
	B.Generated(ResultShape);
// That's all:
  // If there are no any mistakes we return 0:
  return 0;
}


#endif
