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




//namespace App {
//Handle_Standard_Type& FeatureAttr_Type_();
//}

namespace App
{

class FeatureAttr;
class Feature;
AppExport Handle_Standard_Type& STANDARD_TYPE(FeatureAttr);


/** Handle class for FeatureAttr
 */

class Handle_FeatureAttr :public Handle_FCAttribute
{
public:
	AppExport void* operator new(size_t,void* anAddress){return anAddress;}
	AppExport void* operator new(size_t size){return Standard::Allocate(size);}
	AppExport void  operator delete(void *anAddress){if (anAddress) Standard::Free((Standard_Address&)anAddress);}
	AppExport Handle_FeatureAttr():Handle(FCAttribute)() {} 
	AppExport Handle_FeatureAttr(const Handle(FeatureAttr)& aHandle) : Handle(FCAttribute)(aHandle){}

	AppExport Handle_FeatureAttr(const FeatureAttr* anItem) : Handle(FCAttribute)((FCAttribute *)anItem){}

	AppExport Handle_FeatureAttr& operator=(const Handle(FeatureAttr)& aHandle)
	{
		Assign(&(*aHandle));
		return *this;
	}

	AppExport Handle_FeatureAttr& operator=(const FeatureAttr* anItem)
	{
		Assign((Standard_Transient *)anItem);
		return *this;
	}

	AppExport FeatureAttr* operator->() 
	{
		return (FeatureAttr *)(ControlAccess());
	}

	AppExport FeatureAttr* operator->() const{return(FeatureAttr *)ControlAccess();}
	AppExport ~Handle_FeatureAttr();
	AppExport static const Handle_FeatureAttr DownCast(const Handle(Standard_Transient)& AnObject);
};



class FeatureAttr : public FCAttribute 
{
public:

	/// Constructor
	AppExport FeatureAttr();
	/// Destructor
	AppExport ~FeatureAttr();

	/// Delivers the GUID of the Object
	AppExport static const Standard_GUID& GetID() ;


	/// Saving to a stream
	AppExport virtual  Standard_OStream& Dump(Standard_OStream& anOS) const;

	/// copy
	AppExport void Restore(const Handle(TDF_Attribute)& with) ;

	/// Set data
	AppExport void Set(Feature  * S) ;
	/// Get data
	AppExport Feature  * Get() const;

	/// not shure
	AppExport static  Handle_FeatureAttr Set(const TDF_Label& label,Feature  * S) ;

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
	friend Handle_Standard_Type& FeatureAttr_Type_();
	AppExport const Handle(Standard_Type)& DynamicType() const;
	AppExport Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

private: 

	/// Data member string
	Feature *_pcFeature;

};

class FeatureAttrStorageDriver;

class Handle(FeatureAttrStorageDriver) : public Handle(MDF_ASDriver) {
  public:
    void* operator new(size_t,void* anAddress){return anAddress;}
    void* operator new(size_t size){ return Standard::Allocate(size);}
    void  operator delete(void *anAddress) {if (anAddress) Standard::Free((Standard_Address&)anAddress); }
    Handle(FeatureAttrStorageDriver)():Handle(MDF_ASDriver)() {} 
    Handle(FeatureAttrStorageDriver)(const Handle(FeatureAttrStorageDriver)& aHandle) : Handle(MDF_ASDriver)(aHandle){}

    Handle(FeatureAttrStorageDriver)(const FeatureAttrStorageDriver* anItem) 
      : Handle(MDF_ASDriver)((MDF_ASDriver *)anItem){}
    Handle(FeatureAttrStorageDriver)& operator=(const Handle(FeatureAttrStorageDriver)& aHandle)
    {
      Assign(aHandle.Access()); 
      return *this;
    }

    Handle(FeatureAttrStorageDriver)& operator=(const FeatureAttrStorageDriver* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    FeatureAttrStorageDriver* operator->() 
     {
      return (FeatureAttrStorageDriver *)ControlAccess();
     }

    FeatureAttrStorageDriver* operator->() const 
     {
      return (FeatureAttrStorageDriver *)ControlAccess();
     }

   AppExport ~Handle(FeatureAttrStorageDriver)();
 
   AppExport static const Handle(FeatureAttrStorageDriver) DownCast(const Handle(Standard_Transient)& AnObject);
};


class FeatureAttrStorageDriver : public MDF_ASDriver {

public:

  void* operator new(size_t,void* anAddress){return anAddress;}
  void* operator new(size_t size){return Standard::Allocate(size);}
  void  operator delete(void *anAddress){if (anAddress) Standard::Free((Standard_Address&)anAddress);}

  AppExport FeatureAttrStorageDriver(const Handle(CDM_MessageDriver)& theMessageDriver);
  AppExport   Standard_Integer VersionNumber() const;
  AppExport   Handle_Standard_Type SourceType() const;
  AppExport   Handle_PDF_Attribute NewEmpty() const;
  AppExport   void Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const;
  AppExport ~FeatureAttrStorageDriver();

  // Type management
  AppExport friend Handle_Standard_Type& FeatureAttrStorageDriver_Type_();
  AppExport const Handle(Standard_Type)& DynamicType() const;
  AppExport Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

};

class FeatureAttrPersistent;

class Handle(FeatureAttrPersistent) : public Handle(PDF_Attribute) {
  public:

    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    void* operator new(size_t size) 
      { 
        return StandardCSFDB_Allocate(size); 
      }
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }

    Handle(FeatureAttrPersistent)():Handle(PDF_Attribute)() {} 
    Handle(FeatureAttrPersistent)(const Handle(FeatureAttrPersistent)& aHandle) : Handle(PDF_Attribute)(aHandle) 
     {
     }

    Handle(FeatureAttrPersistent)(const FeatureAttrPersistent* anItem) : Handle(PDF_Attribute)((PDF_Attribute *)anItem) 
     {
     }

    Handle(FeatureAttrPersistent)& operator=(const Handle(FeatureAttrPersistent)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(FeatureAttrPersistent)& operator=(const FeatureAttrPersistent* anItem)
     {
      Assign((Standard_Persistent *)anItem);
      return *this;
     }

    FeatureAttrPersistent* operator->() 
     {
      return (FeatureAttrPersistent *)ControlAccess();
     }

    FeatureAttrPersistent* operator->() const 
     {
      return (FeatureAttrPersistent *)ControlAccess();
     }

   AppExport ~Handle(FeatureAttrPersistent)();
 
   AppExport static const Handle(FeatureAttrPersistent) DownCast(const Handle(Standard_Persistent)& AnObject);
};



class FeatureAttrPersistent : public PDF_Attribute {

public:
  void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  void* operator new(size_t size) 
  { 
    return StandardCSFDB_Allocate(size); 
  }
  void  operator delete(void *anAddress) 
  { 
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }

 // Methods PUBLIC
 // 
  AppExport FeatureAttrPersistent();
  AppExport FeatureAttrPersistent(const Handle(PCollection_HExtendedString)& V);
  AppExport   Handle_PCollection_HExtendedString Get() const;
  AppExport   void Set(const Handle(PCollection_HExtendedString)& V) ;
  AppExport ~FeatureAttrPersistent();
  FeatureAttrPersistent(const Storage_stCONSTclCOM& a) : PDF_Attribute(a){}

  Handle(PCollection_HExtendedString) _CSFDB_GetFeatureAttrPersistentmyValue() const { return myValue; }
  void _CSFDB_SetFeatureAttrPersistentmyValue(const Handle(PCollection_HExtendedString)& p) { myValue = p; }



  // Type management
  AppExport friend Handle_Standard_Type& FeatureAttrPersistent_Type_();
  AppExport const Handle(Standard_Type)& DynamicType() const;
  AppExport Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;


  Handle_PCollection_HExtendedString myValue;


};





} // namespace App

#endif
