/** \file FileTemplate.h
 *  \brief FileTemplate example header
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
 *  @see Parameter.cpp
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


#ifndef __FEATUREPARTBOX_H__
#define __FEATUREPARTBOX_H__


#include <App/Feature.h>

namespace Part
{


class PartBoxFeature :public App::Feature
{
	virtual bool MustExecute(const TFunction_Logbook& log) const;

	virtual Standard_Integer Execute(TFunction_Logbook& log) const;

	virtual void Validate(TFunction_Logbook& log) const;

};



}



/*

#include <Standard.hxx>
#include <TFunction_Driver.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>

class TFunction_Logbook;
class Standard_Transient;
class Handle_Standard_Type;
class Handle(TFunction_Driver);
class TSampleOcafFunction_BoxDriver;

Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(TSampleOcafFunction_BoxDriver);

class Handle(TSampleOcafFunction_BoxDriver) : public Handle(TFunction_Driver) {
  public:
    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
    Handle(TSampleOcafFunction_BoxDriver)():Handle(TFunction_Driver)() {} 
    Handle(TSampleOcafFunction_BoxDriver)(const Handle(TSampleOcafFunction_BoxDriver)& aHandle) : Handle(TFunction_Driver)(aHandle) 
     {
     }

    Handle(TSampleOcafFunction_BoxDriver)(const TSampleOcafFunction_BoxDriver* anItem) : Handle(TFunction_Driver)((TFunction_Driver *)anItem) 
     {
     }

    Handle(TSampleOcafFunction_BoxDriver)& operator=(const Handle(TSampleOcafFunction_BoxDriver)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(TSampleOcafFunction_BoxDriver)& operator=(const TSampleOcafFunction_BoxDriver* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    TSampleOcafFunction_BoxDriver* operator->() 
     {
      return (TSampleOcafFunction_BoxDriver *)ControlAccess();
     }

    TSampleOcafFunction_BoxDriver* operator->() const 
     {
      return (TSampleOcafFunction_BoxDriver *)ControlAccess();
     }

   Standard_EXPORT ~Handle(TSampleOcafFunction_BoxDriver)();
 
   Standard_EXPORT static const Handle(TSampleOcafFunction_BoxDriver) DownCast(const Handle(Standard_Transient)& AnObject);
};


class TSampleOcafFunction_BoxDriver : public TFunction_Driver {

public:

    inline void* operator new(size_t,void* anAddress) {return anAddress;}
    inline void* operator new(size_t size) {return Standard::Allocate(size);}
    inline void  operator delete(void *anAddress) {if (anAddress) Standard::Free((Standard_Address&)anAddress);}

	TSampleOcafFunction_BoxDriver();
	virtual  void Validate(TFunction_Logbook& log) const;
	virtual  Standard_Boolean MustExecute(const TFunction_Logbook& log) const;
	virtual  Standard_Integer Execute(TFunction_Logbook& log) const;
	~TSampleOcafFunction_BoxDriver();




	// Type management
	//
	friend Handle_Standard_Type& TSampleOcafFunction_BoxDriver_Type_();
	const Handle(Standard_Type)& DynamicType() const;
	Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

};
*/

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
/*	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main()->GetOCCLabel());

	// Create the data structure : Set the dimensions, position and name attributes
	TDataStd_Real::Set(L.FindChild(1), 1);
	TDataStd_Real::Set(L.FindChild(2), 2);
	TDataStd_Real::Set(L.FindChild(3), 3);
	TDataStd_Real::Set(L.FindChild(4), 4);
	TDataStd_Real::Set(L.FindChild(5), 5);
	TDataStd_Real::Set(L.FindChild(6), 6);
	TDataStd_Name::Set(L, "hallo");
*/
	// Instanciate a TSampleOcafFunction_BoxDriver and add it to the TFunction_DriverTable
/*	Handle(TSampleOcafFunction_BoxDriver) myBoxDriver = new TSampleOcafFunction_BoxDriver();
	TFunction_DriverTable::Get()->AddDriver(Standard_GUID("BoxDriver"), myBoxDriver);
	// Instanciate a TFunction_Function attribute connected to the current box driver
	// and attach it to the data structure as an attribute of the Box Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L,Standard_GUID("BoxDriver"));

	// Initialize and execute the box driver (look at the "Execute()" code)
    TFunction_Logbook log;
	myBoxDriver->Init(L);
    if (myBoxDriver->Execute(log)) MessageBox(0,"DFunction_Execute : failed","Box",MB_ICONERROR);
*/

	// Make a box
/* 	BRepPrimAPI_MakeBox mkBox( gp_Pnt(1, 2 ,3), 4, 5 ,6);
	TopoDS_Shape ResultShape = mkBox.Shape();


	// Build a TNaming_NamedShape using built box
	TNaming_Builder B(L);
	B.Generated(ResultShape);

	Handle(TPrsStd_AISPresentation) hcPrs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
	// Display it
	hcPrs->Display(1);
*/


#endif // __FEATUREPARTBOX_H__
