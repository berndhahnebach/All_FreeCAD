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
 *   Jürgen Riegel 2003                                                    *
 *                                                                         *
 ***************************************************************************/
 


#ifndef _FeatureAttr_h_
#define _FeatureAttr_h_


#include "Attribute.h"


class Standard_Transient;
class Handle_Standard_Type;
class Handle(TDF_Attribute);
class TDataStd_Name;
class TFunction_Logbook;


Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(FCFeatureAttr);



class FCFeatureAttr;
class FCFeature;


/** Handle class for FCFeatureAttr
 */

class Handle_FCFeatureAttr :public Handle_FCAttribute
{
public:
	AppExport void* operator new(size_t,void* anAddress){return anAddress;}
	AppExport void* operator new(size_t size){return Standard::Allocate(size);}
	AppExport void  operator delete(void *anAddress){if (anAddress) Standard::Free((Standard_Address&)anAddress);}
	AppExport Handle_FCFeatureAttr():Handle(FCAttribute)() {} 
	AppExport Handle_FCFeatureAttr(const Handle(FCFeatureAttr)& aHandle) : Handle(FCAttribute)(aHandle){}

	AppExport Handle_FCFeatureAttr(const FCFeatureAttr* anItem) : Handle(FCAttribute)((FCAttribute *)anItem){}

	AppExport Handle_FCFeatureAttr& operator=(const Handle(FCFeatureAttr)& aHandle)
	{
		Assign(&(*aHandle));
		return *this;
	}

	AppExport Handle_FCFeatureAttr& operator=(const FCFeatureAttr* anItem)
	{
		Assign((Standard_Transient *)anItem);
		return *this;
	}

	AppExport FCFeatureAttr* operator->() 
	{
		return (FCFeatureAttr *)(ControlAccess());
	}

	AppExport FCFeatureAttr* operator->() const{return(FCFeatureAttr *)ControlAccess();}
	AppExport ~Handle_FCFeatureAttr();
	AppExport static const Handle_FCFeatureAttr DownCast(const Handle(Standard_Transient)& AnObject);
};



class FCFeatureAttr : public FCAttribute 
{
public:

	/// Constructor
	AppExport FCFeatureAttr();
	/// Destructor
	AppExport ~FCFeatureAttr();

	/// Delivers the GUID of the Object
	AppExport static const Standard_GUID& GetID() ;


	/// Saving to a stream
	AppExport virtual  Standard_OStream& Dump(Standard_OStream& anOS) const;

	/// copy
	AppExport void Restore(const Handle(TDF_Attribute)& with) ;

	/// Set data
	AppExport void Set(FCFeature  * S) ;
	/// Get data
	AppExport FCFeature  * Get() const;

	/// not shure
	AppExport static  Handle_FCFeatureAttr Set(const TDF_Label& label,FCFeature  * S) ;

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
	          friend Handle_Standard_Type& FCFeatureAttr_Type_();
	AppExport const Handle(Standard_Type)& DynamicType() const;
	AppExport Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

private: 

	/// Data member string
	FCFeature *_pcFeature;

};


#endif
