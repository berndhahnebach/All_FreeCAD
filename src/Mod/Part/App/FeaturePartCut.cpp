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
#endif


#include "../../../Base/Console.h"
#include "FeaturePartCut.h"


using namespace Part;

void PartCutFeature::InitLabel(const TDF_Label &rcLabel)
{
	Base::Console().Log("PartCutFeature::InitLabel()\n");

	AddProperty("Float","x","0.0");
	AddProperty("Float","y","0.0");
	AddProperty("Float","z","0.0");
	AddProperty("Float","l","100.0");
	AddProperty("Float","h","100.0");
	AddProperty("Float","w","100.0");

}

/*
bool PartCutFeature::MustExecute(const TFunction_Logbook& log)
{
	Base::Console().Log("PartCutFeature::MustExecute()\n");
	return false;
}
*/
Standard_Integer PartCutFeature::Execute(TFunction_Logbook& log)
{
	Base::Console().Log("PartCutFeature::Execute()\n");

/*  cout << GetFloatProperty("x") << endl;
  cout << GetFloatProperty("y") << endl;
  cout << GetFloatProperty("z") << endl;
  cout << GetFloatProperty("l") << endl;
  cout << GetFloatProperty("h") << endl;
  cout << GetFloatProperty("w") << endl;*/

  try{
	// Build a Cut using the dimension and position attributes 
	BRepPrimAPI_MakeBox mkBox( gp_Pnt(GetFloatProperty("x"), 
                                    GetFloatProperty("y") ,
                                    GetFloatProperty("z")), 
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


void PartCutFeature::Validate(TFunction_Logbook& log)
{
	Base::Console().Log("PartCutFeature::Validate()\n");
 
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log.SetValid(Label(), Standard_True);


}







/*

//=======================================================================
//function : TPartStd_BoxDriver
//purpose  : Creation of an instance of the driver. It's possible (and recommended)
//         : to have only one instance of a driver for the whole session.
//=======================================================================

TSampleOcafFunction_BoxDriver::TSampleOcafFunction_BoxDriver()
{}

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

TSampleOcafFunction_BoxDriver::~TSampleOcafFunction_BoxDriver() {}
 


Standard_EXPORT Handle_Standard_Type& TSampleOcafFunction_BoxDriver_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("TSampleOcafFunction_BoxDriver",
			                                 sizeof(TSampleOcafFunction_BoxDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(TSampleOcafFunction_BoxDriver) Handle(TSampleOcafFunction_BoxDriver)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(TSampleOcafFunction_BoxDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(TSampleOcafFunction_BoxDriver))) {
       _anOtherObject = Handle(TSampleOcafFunction_BoxDriver)((Handle(TSampleOcafFunction_BoxDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& TSampleOcafFunction_BoxDriver::DynamicType() const 
{ 
  return STANDARD_TYPE(TSampleOcafFunction_BoxDriver) ; 
}
Standard_Boolean TSampleOcafFunction_BoxDriver::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(TSampleOcafFunction_BoxDriver) == AType || TFunction_Driver::IsKind(AType)); 
}

Handle_TSampleOcafFunction_BoxDriver::~Handle_TSampleOcafFunction_BoxDriver() {}

*/
