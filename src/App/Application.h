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

#ifndef _Application_
#define _Application_
#include "../Base/PyExportImp.h"
#include "../Base/Parameter.h"



#include <TDocStd_Application.hxx>
#include <TDataStd_Name.hxx>

namespace Base 
{
  class ConsoleObserverStd; 
  class ConsoleObserverFile;
}

namespace App
{

  
class Document;
class ApplicationOCC;
class ApplicationObserver;

Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(ApplicationOCC);

/**	Handle_ApplicationOCC provides the handle for the ApplicationOCC class.
  *
	* To learn more about the OpenCasCade handle mechanismus see the CasCade manual.
  */

class  Handle_ApplicationOCC : public Handle(TDocStd_Application) {
  public:
   Standard_EXPORT Handle_ApplicationOCC():Handle(TDocStd_Application)() {}
   Standard_EXPORT Handle_ApplicationOCC(const Handle_ApplicationOCC& aHandle) : Handle(TDocStd_Application)(aHandle){}
   Standard_EXPORT Handle_ApplicationOCC(const ApplicationOCC* anItem) : Handle(TDocStd_Application)((TDocStd_Application *)anItem){}
   Standard_EXPORT Handle_ApplicationOCC& operator=(const Handle_ApplicationOCC& aHandle){
      Assign(aHandle.Access());
      return *this;
     }
   Standard_EXPORT Handle_ApplicationOCC& operator=(const ApplicationOCC* anItem){
      Assign((Standard_Transient *)anItem);
      return *this;
     }
   Standard_EXPORT ApplicationOCC* operator->(){
      return (ApplicationOCC *)ControlAccess();
     }

   Standard_EXPORT ApplicationOCC* operator->() const{
      return (ApplicationOCC *)ControlAccess();
     }

   Standard_EXPORT ~Handle_ApplicationOCC();
   Standard_EXPORT static const Handle_ApplicationOCC DownCast(const Handle(Standard_Transient)& AnObject);
};


/**	ApplicationOCC provides the OpenCasCade Application functionality.
  *
  * ApplicationOCC inherits from TDocStd_Application and redefines some pure
  * virtual functions, like:
  * Format()        : gives information about the types of formates.
  * ResourcesName() : Dont know much about what this function is doing ;-)
  */


class ApplicationOCC : public TDocStd_Application
{
public:
	ApplicationOCC();
	virtual ~ApplicationOCC();

	// OpenCasCade application Stuff goes here +++++++++++++++++++++++++++++++++++
	/** gives information about the types of formates */
	virtual  Standard_CString ResourcesName();
	/** Dont know much about what this function is doing ;-) */
	virtual  void Formats(TColStd_SequenceOfExtendedString& Formats);
	/** OCAF Typing information functions (see OpenCasCade doc) */
	friend Handle_Standard_Type&   ApplicationOCC_Type_();
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
 *  @see App::Document
 */
class AppExport Application //: public PythonExport
{

public:


  //---------------------------------------------------------------------
	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	/** @name methodes for document handling */
	//@{
	/// Creates a new document 
  App::Document* newDocument(const char * Name=0l);
  /// Closes the document \a name and removes it from the application.
  bool closeDocument(const char* name);
  /// find a unique docuement name
  std::string getUniqueDocumentName(const char *Name);
  /// renams a document
  void renameDocument(const char *OldName, const char *NewName){};
	/// Open an existing document from a file
	App::Document* openDocument(const char * FileName=0l);
	/// Retrive the active document
	App::Document* getActiveDocument(void);
	/// Retrive a named document
	App::Document* getDocument(const char *Name);
	/// Set the active document
	void setActiveDocument(App::Document* pDoc);
	void setActiveDocument(const char *Name);
	/// Geter for the OCC Aplication
	Handle_ApplicationOCC GetOCCApp(void) {return _hApp;}
	//@}
	


	/** @name methodes for parameter handling */
	//@{
  /// returns the system parameter
	ParameterManager &                                GetSystemParameter(void) ;
  /// returns the user parameter
	ParameterManager &                                GetUserParameter(void) ;
  /** Gets a parameter group by a full qualified path
    * It's a easy methode to get a group:
    * \code
    * // geting standard parameter
    * ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
    * std::string cDir             = hGrp->GetASCII("ProjectPath", "");
    * std::string cCameraName      = hGrp->GetASCII("CameraName", "TempCamera.inc");
    * \endcode
    */
	FCHandle<ParameterGrp>                            GetParameterGroupByPath(const char* sName);

	ParameterManager &                                GetParameterSet(const char* sName);
	const std::map<std::string,ParameterManager *> &  GetParameterSetList(void);
	//@}

	/** @name methodes for the open handler 
   *  With this facility a Application module can register 
   *  a ending (file type) which he can handle to open. 
   *  The ending and the module name are stored and if the file
   *  type is opened the module get loaded and need to register a
   *  OpenHandler class in the OpenHandlerFactorySingelton. 
   *  After the module is loaded a OpenHandler of this type is created
   *  and the file get loaded.
   *  @see OpenHandler
   *  @see OpenHandlerFactorySingelton
   */
	//@{
  /// register a ending and a module name
  void addOpenType(const char* Type, const char* ModuleName);
  /// checks if a type is already registered and returns the module name on success or NULL at fail
  const char* hasOpenType(const char* Type) const;
  /// returns a map of all registered open types
  const std::map<std::string,std::string> &getOpenType(void);
  /// removes a open handler type
  void rmvOpenType(const char* Type);
	//@}

	/** @name Init, Destruct an Access methodes */
	//@{
  static void init(int argc, char ** argv);
	static void destruct(void);
	static void runApplication(void);
	friend Application &GetApplication(void);
	static std::map<std::string,std::string> &Config(void){return mConfig;}
	static int GetARGC(void){return _argc;}
	static char** GetARGV(void){return _argv;}
	//@}

 	const char* GetHomePath(void);



private:

	/// Constructor
	Application(ParameterManager *pcSysParamMngr, ParameterManager *pcUserParamMngr,std::map<std::string,std::string> &mConfig);
	/// Destructor
	virtual ~Application();

	/** @name member for parameter */
	//@{
	static ParameterManager *_pcSysParamMngr;
	static ParameterManager *_pcUserParamMngr;
	//@}


	/** @name static members for build time information, will be set bei the preprocesor. */
	//@{
	/// Major version nummber
	static const unsigned int VersionMajor;
	/// Minor version nummber
	static const unsigned int VersionMinor;
	/// Build nummber
	static const unsigned int VersionBuild;
	/// Build date
	static const char *       VersionDate;
	/// Build time
	static const char *    	  VersionTime;
	/// Build time
	static const char *    	  VersionDisDa;
	//@}
	
	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	// static python wrapper of the exported functions
	PYFUNCDEF_S(sOpen);
	PYFUNCDEF_S(sNew);
	PYFUNCDEF_S(sClose);
	PYFUNCDEF_S(sImport);
	PYFUNCDEF_S(sSave);
	PYFUNCDEF_S(sSaveAs);
	PYFUNCDEF_S(sDocument);
	PYFUNCDEF_S(sGetParam);
	PYFUNCDEF_S(sGetVersion);
	PYFUNCDEF_S(sGetConfig);
	PYFUNCDEF_S(sSetConfig);
	PYFUNCDEF_S(sDumpConfig);
	PYFUNCDEF_S(sTemplateAdd);
	PYFUNCDEF_S(sTemplateDelete);
	PYFUNCDEF_S(sTemplateGet);
	PYFUNCDEF_S(sEndingAdd);
	PYFUNCDEF_S(sEndingDelete);
	PYFUNCDEF_S(sEndingGet);

	static PyMethodDef    Methods[]; 

  /// pointer to the Python module of App...
  PyObject* _pcAppModule;

	friend class ApplicationObserver;

	/** @name  Private Init, Destruct an Access methodes */
	//@{
  static void initConfig(int argc, char ** argv);
	static void initApplication(void);
	static void logStatus(void);
	// the one and only pointer to the application object
	static Application *_pcSingelton;
	/// argument helper function
	static void ParsOptions(int argc, char ** argv);
	/// checks if the environment is allreight
	static void CheckEnv(void);
	/// print the help massage
	static void PrintInitHelp(void);
	/// figure out some things
	static void ExtractUser();
	/// load the user and system parameter set
	static void LoadParameters(void);
	/// puts the given env variable in the config
	static void SaveEnv(const char *);
	/// startup configuration container
	static std::map<std::string,std::string> mConfig;
	static int _argc;
	static char ** _argv;
	//@}

	/** @name Singelton functions */
	//@{
	/// Attaches an Observer to monitor the Application
	void AttachObserver(ApplicationObserver *pcObserver);
	/// Detaches an monitoring Observer
	void DetachObserver(ApplicationObserver *pcObserver);
	/// Notify the Obervers on a new Doc
	void NotifyDocNew(App::Document* pcDoc);
	/// Notify the Obervers on a deleted Doc
	void NotifyDocDelete(App::Document* pcDoc);
	/// The Observer is a friend of the Application
	/// The container of all attached Obervers
	std::set<ApplicationObserver * > _aclObservers;
	//@}


  /// open ending information
  std::map<std::string,std::string> _mEndings;
	/// Handle to the OCC Application
	Handle_ApplicationOCC _hApp;
	/// Handles the App::Document (and python) objects;
//	std::vector<App::Document*> _DocVector;

  /// map of all docuements
  struct DocEntry {
    Handle_TDocStd_Document hDoc;
    Document*  pDoc;
  };
  std::map<std::string,DocEntry> DocMap;


	std::map<std::string,ParameterManager *> mpcPramManager;
	// map for Template objects
//	PyObject*		 _pcTemplateDictionary;

	std::map<std::string,std::string> &_mConfig;
	App::Document* _pActiveDoc;

  static Base::ConsoleObserverStd  *_pConsoleObserverStd;
  static Base::ConsoleObserverFile *_pConsoleObserverFile;
};

/// Singelton getter of the Applicaton
inline App::Application &GetApplication(void){
  return *App::Application::_pcSingelton;
}


/** The Application Observer
 *  Derive from this class if you need to get norified by a 
 *  change in the Application. The Observer attaches and detaches
 *  automaticly.
 *  @see Application
 */
class AppExport ApplicationObserver
{
public:
	/// Construction and attachment
	ApplicationObserver(){GetApplication().AttachObserver(this);}
	/// Destruction and detachment
	virtual ~ApplicationObserver(){GetApplication().DetachObserver(this);}

	/// This method get called when a new doc appears
	virtual void OnDocNew(App::Document*){};
	/// This method get called when a new doc will be deleted
	virtual void OnDocDelete(App::Document*){};
};


} // namespace App

#endif

