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


#endif // __FEATUREPARTBOX_H__
