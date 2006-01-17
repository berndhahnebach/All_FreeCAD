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



#include "PreCompiled.h"

#ifndef _PreComp_
# include <iostream>
# ifdef FC_OS_LINUX
# include <time.h>
# endif
#endif


#include "Application.h"
#include "Document.h"

// FreeCAD Base header
#include <Base/Interpreter.h>
#include <Base/Exception.h>
#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Base/Factory.h>
#include <Base/FileInfo.h>
#include <Base/Type.h>
#include <Base/Base.h>
#include <Base/Persistance.h>
#include <Base/Reader.h>

#include "VectorPy.h"
#include "MatrixPy.h"
#include "Feature.h"
#include "Property.h"
#include "PropertyContainer.h"
#include "PropertyStandard.h"
#include "PropertyLinks.h"
#include "Document.h"

namespace EnvMacro {
#include <Base/EnvMacros.h>
}

using namespace App;


// static members
#include "../Version.h"
/// Major version nummber
const unsigned int Application::VersionMajor = FCVersionMajor;
/// Minor version nummber
const unsigned int Application::VersionMinor = FCVersionMinor;
/// Build nummber
const unsigned int Application::VersionBuild = FCVersionBuild;
/// Build date
const char *       Application::VersionDisDa = FCVersionDisDa;
/// Build date
const char *       Application::VersionDate  = __DATE__;
/// Build date
const char *       Application::VersionTime  = __TIME__;

// scriptings (scripts are build in but can be overriden by command line option)
#include "InitScript.h"
#include "TestScript.h"

#ifdef FC_OS_LINUX
using namespace std;
#endif

//using Base::GetConsole;
using namespace Base;
using namespace App;
using namespace std;


/*
Standard_CString ApplicationOCC::ResourcesName()
{
	//return Standard_CString ("Resources");
	return Standard_CString ("Standard");	
}

void ApplicationOCC::Formats(TColStd_SequenceOfExtendedString& Formats)
{
	Formats.Append(TCollection_ExtendedString ("FreeCad-Std"));
	Formats.Append(TCollection_ExtendedString ("MDTV-Standard"));
}

void ApplicationOCC::InitViewer(const Handle(TDocStd_Document)& aDoc) const
{
	// Im not doing it here!
}


ApplicationOCC::~ApplicationOCC() {}

ApplicationOCC::ApplicationOCC()
{
	// Instanciate a TOcafFunction_BoxDriver and add it to the TFunction_DriverTable
//	TFunction_DriverTable::Get()->AddDriver(FCFunction::GetID(), 
//											new FCFunction());

}

// OCAF specific stuff


Standard_EXPORT Handle_Standard_Type& App::ApplicationOCC_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TDocStd_Application);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TDocStd_Application);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(CDF_Application);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(CDF_Application);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(CDM_Application);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(CDM_Application);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FCApplicationOCC",
			                                 sizeof(ApplicationOCC),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle_ApplicationOCC Handle_ApplicationOCC::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle_ApplicationOCC _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(ApplicationOCC))) {
       _anOtherObject = Handle_ApplicationOCC((Handle_ApplicationOCC&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& ApplicationOCC::DynamicType() const
{
  return STANDARD_TYPE(ApplicationOCC) ;
}
Standard_Boolean ApplicationOCC::IsKind(const Handle(Standard_Type)& AType) const
{
  return (STANDARD_TYPE(ApplicationOCC) == AType || TDocStd_Application::IsKind(AType));
}
Handle_ApplicationOCC::~Handle_ApplicationOCC() {}

*/
//==========================================================================
// Application
//==========================================================================

ParameterManager *App::Application::_pcSysParamMngr;
ParameterManager *App::Application::_pcUserParamMngr;
Base::ConsoleObserverStd  *Application::_pConsoleObserverStd;
Base::ConsoleObserverFile *Application::_pConsoleObserverFile;

std::map<std::string,std::string> Application::mConfig;


//**************************************************************************
// Construction and destruction

Application::Application(ParameterManager *pcSysParamMngr, ParameterManager *pcUserParamMngr,std::map<std::string,std::string> &mConfig)
	://_pcSysParamMngr(pcSysParamMngr),
	 //_pcUserParamMngr(pcUserParamMngr),
	 _mConfig(mConfig),
	 _pActiveDoc(0)
{

	//_hApp = new ApplicationOCC;
	mpcPramManager["System parameter"] = _pcSysParamMngr;
	mpcPramManager["User parameter"] = _pcUserParamMngr;


	// seting up Python binding
	_pcAppModule = Py_InitModule("FreeCAD", Application::Methods);

  // introducing additional classes
  // NOTE: As the Python docu says that PyModule_AddObject() steals
  //       a reference to its last argument we must increment it, otherwise
  //       we run into a segmentation fault, later on.
  PyObject* pyVecType = (PyObject *)&App::VectorPy::Type;
  Py_INCREF(pyVecType);
  PyModule_AddObject(_pcAppModule, "Vector", pyVecType);
  PyObject* pyMatType = (PyObject *)&App::MatrixPy::Type;
  Py_INCREF(pyMatType);
  PyModule_AddObject(_pcAppModule, "Matrix", pyMatType);


}

Application::~Application()
{
}


//**************************************************************************
// Interface

/// get called by the document when the name is changing
void Application::renameDocument(const char *OldName, const char *NewName)
{
  std::map<std::string,DocEntry>::iterator pos;
  pos = DocMap.find(OldName);

  if(pos != DocMap.end())
  {
    DocEntry temp;
    temp.pDoc = pos->second.pDoc; 
    DocMap.erase(pos);
    DocMap[NewName] = temp;
  } else 
    Base::Exception("Application::renameDocument(): no document with this name to raname!");

}


Document* Application::newDocument(const char * Name)
{

  DocEntry newDoc;

  // get anyway a valid name!
  if(!Name)
    Name = "Unnamed";
  string name = getUniqueDocumentName(Name);

  // create the FreeCAD document
  newDoc.pDoc = new Document();
  newDoc.pDoc->Name.setValue(name);

	// add the document to the internal list
	DocMap[name] = newDoc;
	_pActiveDoc = newDoc.pDoc;


	//newDoc.pDoc->Init();
	// trigger Observers (open windows and so on)

  AppChanges Reason;
  Reason.Doc = newDoc.pDoc;
  Reason.Why = AppChanges::New;
  Notify(Reason);
	//NotifyDocNew(newDoc.pDoc);

	return newDoc.pDoc;
}

bool Application::closeDocument(const char* name)
{
  //int oldDoc = _hApp->NbDocuments();

  /// @todo Remove the document properly from OCAF
  DocEntry delDoc;
  map<string,DocEntry>::iterator pos = DocMap.find( name );
  if (pos == DocMap.end()) // no such document
    return false;

  delDoc = pos->second;

  DocMap.erase( pos );

	// trigger observers
  AppChanges Reason;
  Reason.Doc = delDoc.pDoc;
  Reason.Why = AppChanges::Del;
  Notify(Reason);
  //NotifyDocDelete(delDoc.pDoc);

  if ( _pActiveDoc == delDoc.pDoc)
    _pActiveDoc = 0;
  delete delDoc.pDoc;

  /*
  int newDoc = _hApp->NbDocuments();

  if ( newDoc >= oldDoc)
    Base::Console().Warning("OCC Document of '%s' couldn't be closed.", name );
*/
  return true;
}

App::Document* Application::getDocument(const char *Name)
{
  map<string,DocEntry>::iterator pos;

  pos = DocMap.find(Name);

  if (pos == DocMap.end())
    return 0;

  return pos->second.pDoc;
}


string Application::getUniqueDocumentName(const char *Name)
{
  map<string,DocEntry>::iterator pos;

  // name in use?
  pos = DocMap.find(Name);

  if (pos == DocMap.end())
    // if not, name is OK
    return Name;
  else
  {
    // find highes sufix
    int nSuff = 0;  
    for(pos = DocMap.begin();pos != DocMap.end();++pos)
    {
      const string &rclObjName = pos->first;
      int nPos = rclObjName.find_last_not_of("0123456789");
      if (rclObjName.substr(0, nPos + 1) == Name)  // Prefix gleich
      {
        string clSuffix(rclObjName.substr(nPos + 1));
        if (clSuffix.size() > 0)
          nSuff = max<int>(nSuff, atol(clSuffix.c_str()));
      }
    }
    char szName[200];
    sprintf(szName, "%s%d", Name, nSuff + 1);
	
    return string(szName);
  }
	
}


Document* Application::openDocument(const char * FileName)
{
  DocEntry newDoc;
  FileInfo File(FileName);
  std::string Ext = File.extension();

  // checking on the extension
  if(Ext == "FCStd" || Ext == "std")
  {
  
	  // Creating a FreeCAD Document
    string name = File.fileNamePure();
	  newDoc.pDoc = new Document();
    newDoc.pDoc->Name.setValue(name);
    newDoc.pDoc->FileName.setValue(File.filePath());
	  DocMap[name] = newDoc;
	  _pActiveDoc = newDoc.pDoc;

	  // trigger Observers (open windows and so on)
    AppChanges Reason;
    Reason.Doc = newDoc.pDoc;
    Reason.Why = AppChanges::New;
    Notify(Reason);

    // read the document
    Base::Reader reader(File.filePath().c_str());
    newDoc.pDoc->Restore(reader);
    newDoc.pDoc->setSaved();


	  //NotifyDocNew(newDoc.pDoc);
  }else{
    throw Base::Exception("Unknown file extension");
  }

	return newDoc.pDoc;
}


Document* Application::getActiveDocument(void)
{
	return _pActiveDoc;
}

void Application::setActiveDocument(Document* pDoc)
{
	_pActiveDoc = pDoc;
}

void Application::setActiveDocument(const char *Name)
{
  std::map<std::string,DocEntry>::iterator pos;
  pos = DocMap.find(Name);

  if(pos != DocMap.end())
	  _pActiveDoc = pos->second.pDoc;
  else
    Base::Console().Warning("try to set unknown document active (ignored)!");
}

const char* Application::GetHomePath(void)
{
  return _mConfig["HomePath"].c_str();
}



ParameterManager & Application::GetSystemParameter(void) 
{
	return *_pcSysParamMngr;
}

ParameterManager & Application::GetUserParameter(void) 
{
	return *_pcUserParamMngr;
}

ParameterManager & Application::GetParameterSet(const char* sName)
{
	return *mpcPramManager[sName];
}

const std::map<std::string,ParameterManager *> & Application::GetParameterSetList(void)
{
	return mpcPramManager;
}

/*
std::vector<std::string> Application::GetAllTemplates(void)
{
	PyObject *key, *value;
	int pos = 0;
	std::vector<std::string> cTemp;
     

	// insert all items
	while (PyDict_Next(_pcTemplateDictionary, &pos, &key, &value)) {
		// do something interesting with the values... 
		cTemp.push_back(PyString_AsString(key));
	}

	return cTemp;
}
*/

FCHandle<ParameterGrp>  Application::GetParameterGroupByPath(const char* sName)
{
	std::string cName = sName,cTemp;

	std::string::size_type pos = cName.find(':');

	// is there a path seperator ?
	if(pos == std::string::npos)
	{
		throw Base::Exception("Application::GetParameterGroupByPath() no parameter set name specified");
	} 
	// assigning the parameter set name
    cTemp.assign(cName,0,pos);
	cName.erase(0,pos+1);

	// test if name is valid
	std::map<std::string,ParameterManager *>::iterator It = mpcPramManager.find(cTemp.c_str());
	if (It == mpcPramManager.end())
		throw Base::Exception("Application::GetParameterGroupByPath() unknown parameter set name specified");

	return It->second->GetGroup(cName.c_str());
}

void Application::addOpenType(const char* Type, const char* ModuleName)
{
  _mEndings[Type] = ModuleName;
}

const char* Application::hasOpenType(const char* Type) const
{
  for ( std::map<std::string, std::string>::const_iterator it = _mEndings.begin(); it != _mEndings.end(); ++it )
  {
#ifdef FC_OS_LINUX
  if ( strcasecmp(Type,it->first.c_str()) == 0 )
#else
  if ( stricmp(Type,it->first.c_str()) == 0 )
#endif
    return it->second.c_str();
  }

  return 0;
}

void Application::rmvOpenType(const char* Type)
{
  _mEndings.erase(Type);
}

const std::map<std::string,std::string> &Application::getOpenType(void)
{
  return _mEndings;
}


//**************************************************************************
// Init, Destruct and singelton

Application * Application::_pcSingelton = 0;

int Application::_argc;
char ** Application::_argv;


void Application::destruct(void)
{

		// saving system parameter
	Console().Log("Saving system parameter...");
	_pcSysParamMngr->SaveDocument(mConfig["SystemParameter"].c_str());
	// saving the User parameter
	Console().Log("done\nSaving user parameter...");
	_pcUserParamMngr->SaveDocument(mConfig["UserParameter"].c_str());
	Console().Log("done\n");
	// clean up
	delete _pcSysParamMngr;
	delete _pcUserParamMngr;

	// not initialized or doubel destruct!
	assert(_pcSingelton);
	delete _pcSingelton;

  // We must detach from console and delete the observer to save our file
  if ( _pConsoleObserverFile )
  {
    Console().DetachObserver(_pConsoleObserverFile);
    delete _pConsoleObserverFile; _pConsoleObserverFile = 0;
  }

  Base::Interpreter().finalize();
}

void Application::init(int argc, char ** argv)
{

  initTypes();

  if(argc==0)
  {
    char* buf = new char[256];
    strncpy(buf,mConfig["ExeName"].c_str(),98);
    initConfig(1,reinterpret_cast<char **>(&buf));
    delete [] buf; buf = 0;
  }
  else
    initConfig(argc,argv);

  initApplication();
}

void Application::initTypes(void)
{
  // Base types
  Base::Type             ::init();
  Base::BaseClass        ::init();
  Base::Exception        ::init();
  Base::Persistance      ::init();
  // Properties
  App ::Property         ::init();
  App ::PropertyContainer::init();
  App ::PropertyFloat    ::init();
  App ::PropertyInteger  ::init();
  App ::PropertyString   ::init();
  App ::PropertyLink     ::init();
  // Document classes
  App ::DocumentObject   ::init();
  App ::Feature          ::init();
  App ::Document         ::init();

  // test code
  /*
  const char * name;
  name = Base::BaseClass::getClassTypeId().getName();
  name = App::Property::getClassTypeId().getName();
  name = App::Property::getClassTypeId().getParent().getName();

  bool b;
  b = App::Property::getClassTypeId().isDerivedFrom(Base::BaseClass::getClassTypeId());
  b = App::Property::getClassTypeId().isDerivedFrom(Base::Persistance::getClassTypeId());
  b = Base::Exception::getClassTypeId().isDerivedFrom(Base::Persistance::getClassTypeId());
*/

}


void Application::initConfig(int argc, char ** argv)
{
	// find the home path....
	std::string HomePath;

#	ifdef FC_OS_WIN32
    HomePath = EnvMacro::FindHomePathWin32(0);
#	else
		HomePath = EnvMacro::FindHomePathUnix(argv[0]);
#	endif

  // try to figure out if using FreeCADLib
  std::string Temp = EnvMacro::GetFreeCADLib(HomePath.c_str());

  // sets all needed varables if a FreeCAD LibPack is found
  if(Temp != "")
  {
	  EnvMacro::EnvPrint("MeinGui Set Python ++++++++++++++++++++++++++++++++++++++++++++++");
	  // sets the python environment variables if the FREECADLIB variable is defined
	  EnvMacro::SetPythonToFreeCADLib(Temp.c_str());
  }
	
	_argc = argc;
	_argv = argv;

	// use home path out of the main modules
  //for(std::string::iterator i=HomePath.begin();i!=HomePath.end();++i)
  //  if(*i == '\\')
  //    *i = '/';

	mConfig["HomePath"] = HomePath;

	// extract home path
	ExtractUser();


	// first check the environment variables
	CheckEnv();

#	ifdef FC_DEBUG
		mConfig["Debug"] = "1";
#	else
		mConfig["Debug"] = "0";
#	endif

	// Parse the options which have impact to the init process
	ParseOptions(argc,argv);


	DBG_TRY
		// init python
		Interpreter().init(argc,argv);
	DBG_CATCH(puts("Application::InitConfig() error init Python Interpreter\n");exit(1);)

	DBG_TRY
		
    // Init console ===========================================================
    _pConsoleObserverStd = new ConsoleObserverStd();
		Console().AttachObserver(_pConsoleObserverStd);
		if(mConfig["Verbose"] == "Strict") 
      Console().SetMode(ConsoleSingelton::Verbose);

    // file logging Init ===========================================================
    if(mConfig["LoggingFile"] == "1"){
		  _pConsoleObserverFile = new ConsoleObserverFile(mConfig["LoggingFileName"].c_str());
		  Console().AttachObserver(_pConsoleObserverFile);
    }else
      _pConsoleObserverFile = 0;


	DBG_CATCH(puts("error init console\n");exit(2);)
	
	// Banner ===========================================================
	if(!(mConfig["Verbose"] == "Strict"))
		Console().Message("%s %s, Libs: %d.%dB%d\n\n%s",mConfig["ExeName"].c_str(),
                                                    mConfig["ExeVersion"].c_str(),
                                                    Application::VersionMajor,
                                                    Application::VersionMinor,
                                                    Application::VersionBuild,
                                                    mConfig["ConsoleBanner"].c_str());
	else
		Console().Message("%s %s, Libs: %d.%dB%d\n\n",mConfig["ExeName"].c_str(),
                                                  mConfig["ExeVersion"].c_str(),
                                                  Application::VersionMajor,
                                                  Application::VersionMinor,
                                                  Application::VersionBuild);


	LoadParameters();

	// capture python variables
	SaveEnv("PYTHONPATH");
	SaveEnv("PYTHONHOME");
	SaveEnv("TCL_LIBRARY");
	SaveEnv("TCLLIBPATH");

	// capture CasCade variables
	SaveEnv("CSF_MDTVFontDirectory");
	SaveEnv("CSF_MDTVTexturesDirectory");
	SaveEnv("CSF_UnitsDefinition");
	SaveEnv("CSF_UnitsLexicon");
	SaveEnv("CSF_StandardDefaults");
	SaveEnv("CSF_PluginDefaults");
	SaveEnv("CSF_LANGUAGE");
	SaveEnv("CSF_SHMessage");
	SaveEnv("CSF_XCAFDefaults");
	SaveEnv("CSF_GraphicShr");
	SaveEnv("CSF_IGESDefaults");
	SaveEnv("CSF_STEPDefaults");

	// capture path
	SaveEnv("PATH");

  logStatus();

}


void Application::SaveEnv(const char* s)
{
	char *c = getenv(s);
	if(c)
		mConfig[s] = c;
}


void Application::initApplication(void)
{

	// interpreter and Init script ==========================================================
	// register scripts
	new ScriptProducer( "FreeCADInit",    FreeCADInit    );
	new ScriptProducer( "FreeCADTest",    FreeCADTest    );


	// creating the application
	if(!(mConfig["Verbose"] == "Strict")) Console().Log("Create Application");
	Application::_pcSingelton = new Application(0,0,mConfig);


	// starting the init script
  try{
	  Interpreter().runString(Base::ScriptFactory().ProduceScript("FreeCADInit"));
  }catch(Base::PyException &e){
      cerr << e.what() << endl;
      cerr << e.getStackTrace() << endl;
      throw Base::Exception("internal error in init script!");
  }

	// Add the one and only FreeCAD FunctionDriver to the driver Tabel 
	//Handle(TFunction_Driver) myDriver = new Function();
	//TFunction_DriverTable::Get()->AddDriver(Function::GetID(),myDriver);


}

void Application::runApplication()
{
  const std::map<std::string,std::string> &EndingMap = App::GetApplication().getOpenType();

  // cycling through all the open files
  unsigned short count = atoi(mConfig["OpenFileCount"].c_str());
  std::string File;
  for (unsigned short i=0; i<count; i++)
  {
    // getting file name
    std::ostringstream temp;
    temp << "OpenFile" << i;

    FileInfo File(mConfig[temp.str()].c_str());

    std::string Ext = File.extension();

    if(Ext == "FCStd" || Ext == "std")
    {

      // try to open 
      try{
        Application::_pcSingelton->openDocument(File.fileName().c_str());
      }catch(...){
        Console().Error("Can't open file %s \n",File.fileName().c_str());
      }
    }else if(Ext == "FCscript")
    {
      Base::Interpreter().runFile(File.fileName().c_str());
    }else if(EndingMap.find(Ext) != EndingMap.end())
    {
      Base::Interpreter().loadModule(EndingMap.find(Ext)->second.c_str());
      Base::Interpreter().runStringArg("import %s",EndingMap.find(Ext)->second.c_str());
      Base::Interpreter().runStringArg("%s.open(\"%s\")",EndingMap.find(Ext)->second.c_str(),File.fileName().c_str());
      Base::Console().Log("Command line open: %s.Open(\"%s\")",EndingMap.find(Ext)->second.c_str(),File.fileName().c_str());

    }

  }

	if(mConfig["RunMode"] == "Cmd")
	{
		// Run the comandline interface
		Interpreter().runCommandLine("FreeCAD Console mode");
	}
	else if(mConfig["RunMode"] == "Script")
	{
		// run a script
		Console().Log("Running script: %s\n",mConfig["ScriptFileName"].c_str());
		Interpreter().runFile(mConfig["FileName"].c_str());
	}
	else if(mConfig["RunMode"] == "ScriptCmd")
	{
		// run a script
		Console().Log("Running script: %s\n",mConfig["ScriptFileName"].c_str());
		Interpreter().runFile(mConfig["FileName"].c_str());
		Interpreter().runCommandLine("FreeCAD Console mode");
	}
	else if(mConfig["RunMode"] == "Internal")
	{
		// run internal script
		Console().Log("Running internal script:\n");
		Interpreter().runString(Base::ScriptFactory().ProduceScript(mConfig["ScriptFileName"].c_str()));

		//!!! Interpreter().Launch(sScriptName);
	} else {

		Console().Log("Unknown Run mode (%d) in main()?!?\n\n",mConfig["RunMode"].c_str());
		exit(1);
	}

}

void Application::logStatus()
{
//#if defined(FC_OS_WIN32)
//  SYSTEMTIME time;

//  GetSystemTime(&time);

//  Console().Log("Init: Time: %d-%d-%d %d:%d,%d\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
//#elif defined(FC_OS_LINUX)
  time_t now;
  time(&now);
  Console().Log("Init: Time: %s\n", ctime(&now));
//#endif

  for(std::map<std::string,std::string>::iterator It = mConfig.begin();It!= mConfig.end();It++)
	{
		Console().Log("  %s\t= %s\n",It->first.c_str(),It->second.c_str());
	}
}

void Application::LoadParameters(void)
{
	// create standard parameter sets
	_pcSysParamMngr = new ParameterManager();
	_pcUserParamMngr = new ParameterManager();

	// Init parameter sets ===========================================================
  //
  // Default path
  std::string appData = mConfig["HomePath"];

#if defined (FC_OS_WIN32)
  const char* APPDATA = 0;//getenv("APPDATA");
  const char* FreeCADDir = "/FreeCAD/";
#elif defined(FC_OS_LINUX)
  const char* APPDATA = getenv("HOME");
  const char* FreeCADDir = "/.FreeCAD/";
#else
  const char* APPDATA = getenv("HOME");
  const char* FreeCADDir = "/FreeCAD/";
#endif
  if ( APPDATA )
  {
    std::string newDir = APPDATA; newDir += FreeCADDir;
    Base::FileInfo fi(newDir.c_str());
    if ( fi.exists() )
      appData = newDir;
		else if ( fi.createDirectory( newDir.c_str() ) )
      appData = newDir;
  }

#if defined(FC_OS_LINUX)
  mConfig["UserParameter"]   = appData + "user.cfg";
	mConfig["SystemParameter"] = appData + "system.cfg";
#else
  mConfig["UserParameter"]   = appData + "Config_" + mConfig["UserName"] + ".FCParam";
	mConfig["SystemParameter"] = appData + "AppParam.FCParam";
#endif

	//puts(mConfig["HomePath"].c_str());
	//puts(mConfig["UserParameter"].c_str());
	//puts(mConfig["SystemParameter"].c_str());

	if(_pcSysParamMngr->LoadOrCreateDocument(mConfig["SystemParameter"].c_str()) && !(mConfig["Verbose"] == "Strict"))
	{
		Console().Warning("   Parameter not existing, write initial one\n");
		Console().Message("   This Warning means normaly FreeCAD running the first time or the\n"
		                     "   configuration was deleted or moved.Build up the standard configuration.\n");

	}

	if(_pcUserParamMngr->LoadOrCreateDocument(mConfig["UserParameter"].c_str()) && !(mConfig["Verbose"] == "Strict"))
	{
		Console().Warning("   User settings not existing, write initial one\n");
		Console().Message("   This Warning means normaly you running FreeCAD the first time\n"
		                     "   or your configuration was deleted or moved. The system defaults\n"
		                     "   will be reestablished for you.\n");

	}
}


void Application::ParseOptions(int argc, char ** argv)
{
	static const char Usage[] = \
	" [Options] files..."\
	"Options:\n"\
	"  -h             Display this information\n"\
	"  -c             Runs FreeCAD in console mode (no windows)\n"\
	"  -cf file-name  Runs FreeCAD in server mode with script file-name\n"\
	"  -cc file-name  Runs first the script an then console mode\n"\
	"  -t0            Runs FreeCAD self test function\n"\
	"  -v             Runs FreeCAD in verbose mode\n"\
	"\n consult also the HTML documentation on http://free-cad.sourceforge.net/\n"\
	"";

  int OpenFileCount = 0;

	// scan command line arguments for user input. 
	for (int i = 1; i < argc; i++) 
	{ 
		if (*argv[i] == '-' ) 
		{ 
			switch (argv[i][1]) 
			{ 
			// Console modes 
			case 'c': 
			case 'C':  
				switch (argv[i][2])  
				{   
					// Console with file
					case 'f':  
					case 'F':  
						mConfig["RunMode"] = "Script";
						if(argc <= i+1)
						{
              std::cerr << "Expecting a file" << std::endl;  
							std::cerr << "\nUsage: " << argv[0] << Usage;
              throw;
						}
						mConfig["FileName"]= argv[i+1];
						i++;
            break;
					case 'c':  
					case 'C':  
						mConfig["RunMode"] = "ScriptCmd";
						if(argc <= i+1)
						{
              std::cerr << "Expecting a file" << std::endl;  
							std::cerr << "\nUsage: " << argv[0] << Usage;
              throw;
						}
						mConfig["FileName"]= argv[i+1];
						i++;
						break;   
					case '\0':  
						mConfig["RunMode"] = "Cmd";
						break;   
					default:  
            std::cerr << "Invalid Input " << argv[i] << std::endl;  
						std::cerr << "\nUsage: " << argv[0] << Usage;
						throw Base::Exception("Comandline error(s)");  
				};  
				break;  
			case 'l': 
			case 'L':  
				mConfig["LoggingFile"] = "1";
				mConfig["LoggingFileName"]= mConfig["BinPath"] + "FreeCAD.log";
				switch (argv[i][2])  
				{
					// Console with file
					case 'f':  
					case 'F':  
						if(argc <= i+1)
						{
              std::cerr << "Expecting a file" << std::endl;  
							std::cerr << "\nUsage: " << argv[0] << Usage;
              throw;
						}
						mConfig["LoggingFileName"]= argv[i+1];
						i++;
            break;
					case '\0':  
						break;   
					default:  
            std::cerr << "Invalid Input " << argv[i] << std::endl;  
						std::cerr << "\nUsage: " << argv[0] << Usage;
						throw Base::Exception("Comandline error(s)");  
				};  
				break;  
			case 't': 
			case 'T':  
				switch (argv[i][2])  
				{   
					case '0':  
						// test script level 0
						mConfig["RunMode"] = "Internal";
						mConfig["ScriptFileName"] = "FreeCADTest";
						//sScriptName = FreeCADTest;
						break;   
					default:  
						//default testing level 0
						mConfig["RunMode"] = "Internal";
						mConfig["ScriptFileName"] = "FreeCADTest";
						//sScriptName = FreeCADTest;
						break;   
				};  
				break;  
			case 'v': 
			case 'V':  
				switch (argv[i][2])  
				{   
					// run the test environment script
					case '1':  
						mConfig["Verbose"] = "Loose";
						//sScriptName = GetScriptFactory().ProduceScript("FreeCADTestEnv");
						break;   
					case '\0':  
					case '0':  
						// test script level 0
						mConfig["Verbose"] = "Strict";
						break;   
					default:  
						//default testing level 0
						mConfig["Verbose"] = "Strict";
						//Console().Error("Invalid Verbose Option: %s\n",argv[i]); 
						//Console().Error("\nUsage: %s %s",argv[0],Usage); 
						//throw FCException("Comandline error(s)");  
				};  
				break;  
			case '?': 
			case 'h': 
			case 'H': 
				std::cerr << "\nUsage: " << argv[0] << Usage;
				exit(0);
				//throw FCException("Comandline break");  
				break;  
			default: 
				printf("Invalid Option: %s\n",argv[i]); 
				std::cerr << "\nUsage: " << argv[0] << Usage;
 				throw Base::Exception("Comandline error(s)");
			} 
		} 
		else  
		{ 
      // store all file (URLS) names to open
      if(OpenFileCount < 56534){

        std::ostringstream temp;
        temp << "OpenFile" << OpenFileCount;
        mConfig[temp.str()] = argv[i];
        OpenFileCount++;
      }else{
        std::cerr << "\nToo many arguments! All arguments above 56534 will be ignored!!";
      }

    }
	}
  std::ostringstream buffer;
  buffer << OpenFileCount;
  mConfig["OpenFileCount"] = buffer.str();

}  


void Application::ExtractUser()
{
	// std paths
	mConfig["BinPath"] = mConfig["HomePath"] + "bin" + PATHSEP;
	mConfig["DocPath"] = mConfig["HomePath"] + "doc" + PATHSEP;

	// try to figure out if using FreeCADLib
	mConfig["FreeCADLib"] = EnvMacro::GetFreeCADLib(mConfig["HomePath"].c_str());

	// try to figure out the user
	char* user = getenv("USERNAME");
	if (user == NULL)
		user = getenv("USER");
	if (user == NULL)
		user = "Anonymous";
	mConfig["UserName"] = user;

	EnvMacro::PrintPath();
}

const char sEnvErrorText1[] = \
"It seems some of the variables needed by FreeCAD are not set\n"\
"or wrong set. This regards the Open CasCade or python variables:\n"\
"CSF_GraphicShr=C:\\CasRoot\\Windows_NT\\dll\\opengl.dll\n"\
"CSF_MDTVFontDirectory=C:\\CasRoot\\src\\FontMFT\\\n"\
"CSF_MDTVTexturesDirectory=C:\\CasRoot\\src\\Textures\\\n"\
"CSF_UnitsDefinition=C:\\CasRoot\\src\\UnitsAPI\\Units.dat\n"\
"CSF_UnitsLexicon=C:\\CasRoot\\src\\UnitsAPI\\Lexi_Expr.dat\n"\
"Please reinstall python or OpenCasCade!\n\n";

const char sEnvErrorText2[] = \
"It seems some of the variables needed by FreeCAD are not set\n"\
"or wrong set. This regards the Open CasCade variables:\n"\
"XXX=C:\\CasRoot\\Windows_NT\\dll\\opengl.dll\n"\
"Please reinstall XXX!\n\n";


void Application::CheckEnv(void)
{
	// set the OpenCasCade plugin variables to the FreeCAD bin path.
	EnvMacro::SetPluginDefaults(mConfig["HomePath"].c_str());

	// sets all needed varables if a FreeCAD LibPack is found
	if(mConfig["FreeCADLib"] != "")
	{
		// sets the python environment variables if the FREECADLIB variable is defined
		EnvMacro::SetPythonToFreeCADLib(mConfig["FreeCADLib"].c_str());

		// sets the OpenCasCade environment variables if the FREECADLIB variable is defined
		EnvMacro::SetCasCadeToFreeCADLib(mConfig["FreeCADLib"].c_str());
	}

	cout << flush;

	bool bFailure=false;

	EnvMacro::TestEnvExists("CSF_MDTVFontDirectory",bFailure);
	EnvMacro::TestEnvExists("CSF_MDTVTexturesDirectory",bFailure);
	EnvMacro::TestEnvExists("CSF_UnitsDefinition",bFailure);
	EnvMacro::TestEnvExists("CSF_UnitsLexicon",bFailure);

	if (bFailure) {
     		cerr<<"Environment Error(s)"<<endl<<sEnvErrorText1;
		exit(1);
	}
}

//**************************************************************************
// Observer stuff

/*
void Application::AttachObserver(ApplicationObserver *pcObserver)
{
	// double insert !!
	assert(_aclObservers.find(pcObserver) == _aclObservers.end() );

	_aclObservers.insert(pcObserver);
}

void Application::DetachObserver(ApplicationObserver *pcObserver)
{
	_aclObservers.erase(pcObserver);
}

void Application::NotifyDocNew(Document* pcDoc)
{
	for(std::set<ApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocNew(pcDoc);   // send doc to the listener
}

void Application::NotifyDocDelete(Document* pcDoc)
{
	for(std::set<ApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocDelete(pcDoc);   // send doc to the listener
}
*/ 

