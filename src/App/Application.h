/***************************************************************************
                          FCApplication.h  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@bluewin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _FCApplication_
#define _FCApplication_
#include "../Base/Export.h" 
#include "../Base/PyExport.h"

#pragma warning( disable : 4786 )

#include <TDocStd_Application.hxx>
#include <vector>
#include <set>


class FCDocument;
class FCApplicationOCC;
class FCApplicationObserver;


Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(FCApplicationOCC);

/**	Handle_FCApplicationOCC provides the handle for the FCApplicationOCC class.
  *
	* To learn more about the OpenCasCade handle mechanismus see the CasCade manual.
  */

class  Handle_FCApplicationOCC : public Handle(TDocStd_Application) {
  public:
   Standard_EXPORT Handle_FCApplicationOCC():Handle(TDocStd_Application)() {}
   Standard_EXPORT Handle_FCApplicationOCC(const Handle_FCApplicationOCC& aHandle) : Handle(TDocStd_Application)(aHandle){}
   Standard_EXPORT Handle_FCApplicationOCC(const FCApplicationOCC* anItem) : Handle(TDocStd_Application)((TDocStd_Application *)anItem){}
   Standard_EXPORT Handle_FCApplicationOCC& operator=(const Handle_FCApplicationOCC& aHandle){
      Assign(aHandle.Access());
      return *this;
     }
   Standard_EXPORT Handle_FCApplicationOCC& operator=(const FCApplicationOCC* anItem){
      Assign((Standard_Transient *)anItem);
      return *this;
     }
   Standard_EXPORT FCApplicationOCC* operator->(){
      return (FCApplicationOCC *)ControlAccess();
     }

   Standard_EXPORT FCApplicationOCC* operator->() const{
      return (FCApplicationOCC *)ControlAccess();
     }

   Standard_EXPORT ~Handle_FCApplicationOCC();
   Standard_EXPORT static const Handle_FCApplicationOCC DownCast(const Handle(Standard_Transient)& AnObject);
};


/**	FCApplicationOCC provides the OpenCasCade Application functionality.
  *
	* FCApplicationOCC inherits from TDocStd_Application and redefines some pure
	* virtual functions, like:
	* Format()        : gives information about the types of formates.
	* ResourcesName() : Dont know much about what this function is doing ;-)
  */


class FCApplicationOCC : public TDocStd_Application
{

public:
	FCApplicationOCC::~FCApplicationOCC();

	// OpenCasCade application Stuff goes here +++++++++++++++++++++++++++++++++++
	/** gives information about the types of formates */
	virtual  Standard_CString ResourcesName();
	/** Dont know much about what this function is doing ;-) */
	virtual  void Formats(TColStd_SequenceOfExtendedString& Formats);
	/** OCAF Typing information functions (see OpenCasCade doc) */
	friend Handle_Standard_Type&   FCApplicationOCC_Type_();
	/** OCAF Typing information functions (see OpenCasCade doc) */
	const Handle(Standard_Type)&   DynamicType() const;
	/** OCAF Typing information functions (see OpenCasCade doc) */
	Standard_Boolean	           IsKind(const Handle(Standard_Type)&) const;	
protected:
	/** This function can do some init stuff on a a new Viewer (not neccesary) */
	virtual  void InitViewer(const Handle(TDocStd_Document)& aDoc) const;

};



class AppExport FCApplication :public FCPythonExport
{

public:

	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	FCDocument* New(const char * Name=0l);
	FCDocument* Open(const char * Name=0l);
	FCDocument* Save(void);
	FCDocument* SaveAs(const char * Name=0l);
	 
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	// static python wrapper of the exported functions
	PYFUNCDEF(sOpen);
	PYFUNCDEF(sNew);
	PYFUNCDEF(sSave);
	PYFUNCDEF(sSaveAs);
	PYFUNCDEF(sGet);
	static PyMethodDef    Methods[]; 

private:
	// Singelton!
	FCApplication(void);
	~FCApplication();
	// Singelton functions
	static void Destruct(void);
	static FCApplication &Instance(void);
	static FCApplication *_pcSingelton;
	friend FCApplication &GetApplication(void); 

	// Observer
	void AttacheObserver(FCApplicationObserver *pcObserver);
	void DetacheObserver(FCApplicationObserver *pcObserver);
	void NotifyDocNew(FCDocument* pcDoc);
	void NotifyDocDelete(FCDocument* pcDoc);
	friend FCApplicationObserver;

	// OCC Application
#	pragma warning( disable : 4251 )
	Handle_FCApplicationOCC _hApp;
	// Handles the FCDocument (and python) objects;
	stlport::vector<FCDocument*> _DocVector;
	stlport::set<FCApplicationObserver * > _aclObservers;
#	pragma warning( default : 4251 )
};

inline FCApplication &GetApplication(void){
	return FCApplication::Instance();
}

class AppExport FCApplicationObserver
{
public:
	FCApplicationObserver(){GetApplication().AttacheObserver(this);}
	~FCApplicationObserver(){GetApplication().DetacheObserver(this);}

	virtual void OnDocNew(FCDocument* pcDoc){};
	virtual void OnDocDelete(FCDocument* pcDoc){};
};




#endif

