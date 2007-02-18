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

#include <boost/signal.hpp>

#include <vector>

#include <Base/PyObjectBase.h>
#include <Base/Parameter.h>
#include <Base/Observer.h>


namespace Base 
{
  class ConsoleObserverStd; 
  class ConsoleObserverFile;
}

namespace App
{

  
class Document;
class ApplicationObserver;



/** transport the changes of the Application
 *  This class transport closer information what was changed in the
 *  application. 
 *@see Application
 *@see Observer
*/
class AppExport AppChanges
{
public:
  enum {
    New,
    Del
  } Why;
  App::Document* Doc;
};


/** The Application
 *  The root of the whole application
 *  @see App::Document
 */
class AppExport Application : public Base::Subject<const AppChanges&>
{

public:


  //---------------------------------------------------------------------
  // exported functions goes here +++++++++++++++++++++++++++++++++++++++
  //---------------------------------------------------------------------

  /** @name methods for document handling */
  //@{
  /** Creates a new document
    * The first name is a the identifier and some kind of an internal (english)
    * name. It has to be like an identifier in a programming language, with no
    * spaces and not starting with a number. This name gets also forced to be unique
    * in this Application. You can avoid the renaming by using getUniqueDocumentName()
    * to get a unique name before calling newDoucument().
    * The second name is a UTF8 name of any kind. It's that name normally shown to 
    * the user and stored in the App::Document::Name property.
    */
  App::Document* newDocument(const char * Name=0l, const char * UserName=0l);
  /// Closes the document \a name and removes it from the application.
  bool closeDocument(const char* name);
  /// find a unique docuement name
  std::string getUniqueDocumentName(const char *Name) const;
  /// Open an existing document from a file
  App::Document* openDocument(const char * FileName=0l);
  /// Retrieve the active document
  App::Document* getActiveDocument(void);
  /// Retrive a named document
  App::Document* getDocument(const char *Name) const;
  /// gets the (internal) name of the document
  const char * getDocumentName(const App::Document* ) const;
  /// get a list of all documents in the application
  std::vector<App::Document*> getDocuments() const;
  /// Set the active document
  void setActiveDocument(App::Document* pDoc);
  void setActiveDocument(const char *Name);
  //@}

  /** @name Signals of the Application */
	//@{
  /// signal on new Document
  boost::signal<void (const Document&)> signalNewDocument;
  /// signal on deleted Document
  boost::signal<void (const Document&)> signalDeletedDocument;
  /// signal on renaming Document
  boost::signal<void (const Document&)> signalRenameDocument;
  //@}


	/** @name methods for parameter handling */
	//@{
  /// returns the system parameter
	ParameterManager &                                GetSystemParameter(void) ;
  /// returns the user parameter
	ParameterManager &                                GetUserParameter(void) ;
  /** Gets a parameter group by a full qualified path
    * It's an easy method to get a group:
    * \code
    * // geting standard parameter
    * ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
    * std::string cDir             = hGrp->GetASCII("ProjectPath", "");
    * std::string cCameraName      = hGrp->GetASCII("CameraName", "TempCamera.inc");
    * \endcode
    */
	FCHandle<ParameterGrp>                            GetParameterGroupByPath(const char* sName);

	ParameterManager *                                GetParameterSet(const char* sName) const;
	const std::map<std::string,ParameterManager *> &  GetParameterSetList(void) const;
	void AddParameterSet(const char* sName);
	void RemoveParameterSet(const char* sName);
	//@}

	/** @name methods for the open handler 
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
  std::map<std::string,std::string> getOpenType(void) const;
  std::vector<std::string> getOpenFilter(void) const;
  /// removes a open handler type
  void rmvOpenType(const char* Type);
	//@}

	/** @name Init, Destruct an Access methods */
	//@{
  static void init(int argc, char ** argv);
  static void initTypes(void);
	static void destruct(void);
	static void destructObserver(void);
	static void runApplication(void);
	friend Application &GetApplication(void);
	static std::map<std::string,std::string> &Config(void){return mConfig;}
	static int GetARGC(void){return _argc;}
	static char** GetARGV(void){return _argv;}
	//@}

 	const char* GetHomePath(void);

  friend class App::Document;

protected:
  /// get called by the document when the name is changing
  void renameDocument(const char *OldName, const char *NewName);


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

	
  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++
  //---------------------------------------------------------------------

  // static python wrapper of the exported functions
  PYFUNCDEF_S(sOpen);             // deprecated
  PYFUNCDEF_S(sNew);              // deprecated
  PYFUNCDEF_S(sClose);            // deprecated
  PYFUNCDEF_S(sImport);           // deprecated (not implemented)
  PYFUNCDEF_S(sSave);             // deprecated
  PYFUNCDEF_S(sSaveAs);           // deprecated
  PYFUNCDEF_S(sDocument);         // deprecated
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

  PYFUNCDEF_S(sOpenDocument);
  PYFUNCDEF_S(sSaveDocument);
  PYFUNCDEF_S(sSaveDocumentAs);
  PYFUNCDEF_S(sNewDocument);
  PYFUNCDEF_S(sCloseDocument);
  PYFUNCDEF_S(sActiveDocument);
  PYFUNCDEF_S(sSetActiveDocument);
  PYFUNCDEF_S(sGetDocument);
  PYFUNCDEF_S(sListDocuments);

  static PyMethodDef    Methods[]; 

  /// pointer to the Python module of App...
  PyObject* _pcAppModule;

	friend class ApplicationObserver;

	/** @name  Private Init, Destruct an Access methods */
	//@{
  static void initConfig(int argc, char ** argv);
	static void initApplication(void);
	static void logStatus(void);
	// the one and only pointer to the application object
	static Application *_pcSingelton;
	/// argument helper function
	static void ParseOptions(int argc, char ** argv);
	/// checks if the environment is allreight
	//static void CheckEnv(void);
  // search for the home path
  static std::string FindHomePath(const char* sCall);

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

  struct OpenTypeItem {
    std::string filter;
    std::string module;
    std::vector<std::string> types;
  };

  /// open ending information
  std::vector<OpenTypeItem> _mEndings;

  std::map<std::string,Document*> DocMap;

	std::map<std::string,ParameterManager *> mpcPramManager;

	std::map<std::string,std::string> &_mConfig;
	App::Document* _pActiveDoc;

  static Base::ConsoleObserverStd  *_pConsoleObserverStd;
  static Base::ConsoleObserverFile *_pConsoleObserverFile;
};

/// Singelton getter of the Applicaton
inline App::Application &GetApplication(void){
  return *App::Application::_pcSingelton;
}



} // namespace App

#endif

