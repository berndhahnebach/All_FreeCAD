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
 


#ifndef _PropertyAttr_h_
#define _PropertyAttr_h_

#include "Attribute.h"


class Standard_Transient;
class Handle_Standard_Type;
class Handle(TDF_Attribute);
class TDataStd_Name;


Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(TDataStd_Name);

namespace App
{

class PropertyAttr;


/** Handle class for PropertyAttr
 */
class Handle_PropertyAttr :public Handle_FCAttribute
{
public:
	AppExport void* operator new(size_t,void* anAddress){return anAddress;}
	AppExport void* operator new(size_t size){return Standard::Allocate(size);}
	AppExport void  operator delete(void *anAddress){if (anAddress) Standard::Free((Standard_Address&)anAddress);}
	AppExport Handle_PropertyAttr():Handle(FCAttribute)() {} 
	AppExport Handle_PropertyAttr(const Handle(PropertyAttr)& aHandle) : Handle(FCAttribute)(aHandle){}

	AppExport Handle_PropertyAttr(const PropertyAttr* anItem) : Handle(FCAttribute)((FCAttribute *)anItem){}

	AppExport Handle_PropertyAttr& operator=(const Handle(PropertyAttr)& aHandle)
	{
		Assign(&(*aHandle));
		return *this;
	}

	AppExport Handle_PropertyAttr& operator=(const PropertyAttr* anItem)
	{
		Assign((Standard_Transient *)anItem);
		return *this;
	}

	AppExport PropertyAttr* operator->() 
	{
		return (PropertyAttr *)(ControlAccess());
	}

	AppExport PropertyAttr* operator->() const{return(PropertyAttr *)ControlAccess();}
	AppExport ~Handle_PropertyAttr();
	AppExport static const Handle_PropertyAttr DownCast(const Handle(Standard_Transient)& AnObject);
};



class PropertyAttr : public FCAttribute 
{
public:

	/// Constructor
	AppExport PropertyAttr();
	/// Destructor
	AppExport ~PropertyAttr();

	/// Delivers the GUID of the Object
	AppExport static const Standard_GUID& GetID() ;


	/// Saving to a stream
	AppExport virtual  Standard_OStream& Dump(Standard_OStream& anOS) const;

	/// copy
	AppExport void Restore(const Handle(TDF_Attribute)& with) ;

	/// Set data
	AppExport void Set(const TCollection_ExtendedString& S) ;
	/// Get data
	AppExport TCollection_ExtendedString Get() const;

	/// not shure
	AppExport static  Handle_PropertyAttr Set(const TDF_Label& label,const TCollection_ExtendedString& string) ;

	/// Get the unique ID of the Attribute
	AppExport const Standard_GUID& ID() const;

	/// Get a empty instance
	AppExport Handle_TDF_Attribute NewEmpty() const;

	/// some kind of pasting ???
	AppExport void Paste(const Handle(TDF_Attribute)& into,const Handle(TDF_RelocationTable)& RT) const;


	/// needet for CasCade handles 
    AppExport void* operator new(size_t,void* anAddress)	{return anAddress;}
    AppExport void* operator new(size_t size)				{return Standard::Allocate(size);}
    AppExport void  operator delete(void *anAddress)		{if (anAddress) Standard::Free((Standard_Address&)anAddress);}
	AppExport friend Handle_Standard_Type& PropertyAttr_Type_();
	AppExport const Handle(Standard_Type)& DynamicType() const;
	AppExport Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

private: 

	/// Data member string
	TCollection_ExtendedString myString2;

};



} //namespace App



#endif
