/***************************************************************************
                          FCApplication.h  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
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

#pragma warning( disable : 4251 )

#include <TDocStd_Application.hxx>
#include <vector>
#include <set>


class FCDocument;
class FCApplicationOCC;
class FCApplicationObserver;
class FCParameterManager;


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

 

/** The Application 
 *  The root of the whole application
 *  @see FCDocument
 */
class AppExport FCApplication : public FCPythonExport
{

public:

	//---------------------------------------------------------------------
	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	/// Creates a new document with a template
	FCDocument* New(const char * Name=0l);
	/// Open an existing document from a file
	FCDocument* Open(const char * Name=0l);
	/// Save the active document to the name it was opened
	FCDocument* Save(void);
	/// Save the active Document to a filename
	FCDocument* SaveAs(const char * Name=0l);
	/// Retrive the active document 
	FCDocument* Active(void);
	/// Set the active document 
	void SetActive(FCDocument* pDoc);
	/// Geter for the OCC Aplication
	Handle_FCApplicationOCC GetOCCApp(void) {return _hApp;}

	FCParameterManager & GetParameter(void) {return *_pcParamMngr;}
	
	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	// static python wrapper of the exported functions
	PYFUNCDEF(sOpen);
	PYFUNCDEF(sNew);
	PYFUNCDEF(sSave);
	PYFUNCDEF(sSaveAs);
	PYFUNCDEF(sGet);
	static PyMethodDef    Methods[]; 

	friend FCApplicationObserver;

	/// Singelton functions
	static void Destruct(void);
//	static FCApplication &Instance(void);
	friend FCApplication &GetApplication(void); 
	static FCApplication *_pcSingelton;

	/// Constructor
	FCApplication(FCParameterManager *pcParamMngr);
	/// Destructor
	~FCApplication();

private:

	/// Attach an Observer to monitor the Application
	void AttacheObserver(FCApplicationObserver *pcObserver);
	/// Detache an monitoring Observer
	void DetacheObserver(FCApplicationObserver *pcObserver);
	/// Notify the Obervers on a new Doc
	void NotifyDocNew(FCDocument* pcDoc);
	/// Notify the Obervers on a deleted Doc
	void NotifyDocDelete(FCDocument* pcDoc);
	/// The Observer is a friend of the Application

private:
	/// Handle to the OCC Application
	Handle_FCApplicationOCC _hApp;
	/// Handles the FCDocument (and python) objects;
	stlport::vector<FCDocument*> _DocVector;
	/// The container of all attached Obervers
	stlport::set<FCApplicationObserver * > _aclObservers;
	FCDocument* _pActiveDoc;
	FCParameterManager *_pcParamMngr;
};

/// Singelton getter of the Applicaton
inline FCApplication &GetApplication(void){
	return *FCApplication::_pcSingelton;
}


/** The Application Observer
 *  Derive from this class if you need to get norified by a 
 *  change in the Application. The Observer attache and detache
 *  automaticly.
 *  @see FCApplication
 */
class AppExport FCApplicationObserver
{
public:
	/// Construction and attachment
	FCApplicationObserver(){GetApplication().AttacheObserver(this);}
	/// Destruction and detachment
	~FCApplicationObserver(){GetApplication().DetacheObserver(this);}

	/// This method get called when a new doc appears
	virtual void OnDocNew(FCDocument*){};
	/// This method get called when a new doc will be deleted
	virtual void OnDocDelete(FCDocument*){};
};




#endif

