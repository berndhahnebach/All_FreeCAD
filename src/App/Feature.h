/** \file $RCSfile$
 *  \brief The attribute module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 


#ifndef _Feature_h_
#define _Feature_h_

#include "Attribute.h"


class Standard_Transient;
class Handle_Standard_Type;
class Handle(TDF_Attribute);
class TDataStd_Name;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(TDataStd_Name);



class FCFeature;


/** Handle class for FCFeature
 */
class Handle_FCFeature :public Handle_FCAttribute
{
public:
	AppExport void* operator new(size_t,void* anAddress){return anAddress;}
	AppExport void* operator new(size_t size){return Standard::Allocate(size);}
	AppExport void  operator delete(void *anAddress){if (anAddress) Standard::Free((Standard_Address&)anAddress);}
	AppExport Handle_FCFeature():Handle(FCAttribute)() {} 
	AppExport Handle_FCFeature(const Handle(FCFeature)& aHandle) : Handle(FCAttribute)(aHandle){}

	AppExport Handle_FCFeature(const FCFeature* anItem) : Handle(FCAttribute)((FCAttribute *)anItem){}

	AppExport Handle_FCFeature& operator=(const Handle(FCFeature)& aHandle)
	{
		Assign(&(*aHandle));
		return *this;
	}

	AppExport Handle_FCFeature& operator=(const FCFeature* anItem)
	{
		Assign((Standard_Transient *)anItem);
		return *this;
	}

	AppExport FCFeature* operator->() 
	{
		return (FCFeature *)(ControlAccess());
	}

	AppExport FCFeature* operator->() const{return(FCFeature *)ControlAccess();}
	AppExport ~Handle_FCFeature();
	AppExport static const Handle_FCFeature DownCast(const Handle(Standard_Transient)& AnObject);
};



class FCFeature : public FCAttribute 
{
public:

	/// Constructor
	AppExport FCFeature();
	/// Destructor
	AppExport ~FCFeature();

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
	AppExport static  Handle_FCFeature Set(const TDF_Label& label,const TCollection_ExtendedString& string) ;

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
	AppExport friend Handle_Standard_Type& FCFeature_Type_();
	AppExport const Handle(Standard_Type)& DynamicType() const;
	AppExport Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

private: 

	/// Data member string
	TCollection_ExtendedString myString2;

};





#endif
