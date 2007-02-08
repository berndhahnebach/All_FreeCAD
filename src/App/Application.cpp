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
# include <sstream>
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
#include "FeatureTest.h"
#include "FeaturePython.h"
#include "Property.h"
#include "PropertyContainer.h"
#include "PropertyStandard.h"
#include "PropertyLinks.h"
#include "Document.h"
#include "DocumentObjectGroup.h"


using namespace App;


// scriptings (scripts are build in but can be overriden by command line option)
#include "InitScript.h"
#include "TestScript.h"

#ifdef _MSC_VER // New handler for Microsoft Visual C++ compiler
# include <new.h>
#else // Ansi C/C++ new handler
# include <new>
#endif

#ifdef MemDebugOn
# define new DEBUG_CLIENTBLOCK
#endif


//using Base::GetConsole;
using namespace Base;
using namespace App;
using namespace std;


//==========================================================================
// Application
//==========================================================================

ParameterManager *App::Application::_pcSysParamMngr;
ParameterManager *App::Application::_pcUserParamMngr;
Base::ConsoleObserverStd  *Application::_pConsoleObserverStd =0;
Base::ConsoleObserverFile *Application::_pConsoleObserverFile =0;

AppExport std::map<std::string,std::string> Application::mConfig;


//**************************************************************************
// Construction and destruction

PyDoc_STRVAR(FreeCAD_doc,
"The functions in the FreeCAD module allow working with documents.\n"
"The FreeCAD instance provides a list of references of documents which\n"
"can be addressed by a string. Hence the document name must be unique.\n"
"\n"
"openDocument(string) -- Create a document and load the project file into the document.\n"
"\n"
"The string argument must point to an existing file. If the file doesn't not exist or\n"
"the file cannot be loaded an I/O exception is thrown. In this case the document is kept alive.\n"
"\n"
"saveDocument(string) -- Save the document to a file.\n"
"\n"
"The document has the read-only attribute FileName which points to the file the document should\n"
"be stored to.\n"
"\n"
"saveDocumentAs() -- bla bla bla\n"
"\n"
"newDocument() -- bla bla bla\n"
"\n"
"closeDocument() -- bla bla bla\n"
"\n"
"activeDocument() -- bla bla bla\n"
"\n"
"setActiveDocument() -- bla bla bla\n"
"\n"
"getDocument() -- bla bla bla\n"
"\n"
"listDocuments() -- bla bla bla\n"
);

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
	_pcAppModule = Py_InitModule3("FreeCAD", Application::Methods, FreeCAD_doc);

  // introducing additional classes

  // NOTE: To finish the initialization of our own type objects we must
  // call PyType_Ready, otherwise we run into a segmentation fault, later on.
  // This function is responsible for adding inherited slots from a type's base class.
  PyObject* pyVecType = (PyObject *)&App::VectorPy::Type;
  if (PyType_Ready(&App::VectorPy::Type) < 0) return;
  PyModule_AddObject(_pcAppModule, "Vector", pyVecType);
  PyObject* pyMatType = (PyObject *)&App::MatrixPy::Type;
  if(PyType_Ready(&App::MatrixPy::Type) < 0) return;;
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
  std::map<std::string,Document*>::iterator pos;
  pos = DocMap.find(OldName);

  if(pos != DocMap.end())
  {
    Document* temp;
    temp = pos->second; 
    DocMap.erase(pos);
    DocMap[NewName] = temp;
    signalRenameDocument(*temp);
  } else 
    Base::Exception("Application::renameDocument(): no document with this name to rename!");

}


Document* Application::newDocument(const char * Name, const char * UserName)
{
  // get anyway a valid name!
  if(!Name)
    Name = "Unnamed";
  string name = getUniqueDocumentName(Name);

  // create the FreeCAD document
  auto_ptr<Document> newDoc(new Document() );

  // set the UserName
  if(UserName)
    newDoc->Name.setValue(UserName);
  else
    newDoc->Name.setValue(Name);

	// add the document to the internal list
	DocMap[name] = newDoc.release(); // now owned by the Application
	_pActiveDoc = DocMap[name];

  AppChanges Reason;
  Reason.Doc = _pActiveDoc;
  Reason.Why = AppChanges::New;
  Notify(Reason);
  signalNewDocument(*_pActiveDoc);

  return _pActiveDoc;
}

bool Application::closeDocument(const char* name)
{
  map<string,Document*>::iterator pos = DocMap.find( name );
  if (pos == DocMap.end()) // no such document
    return false;

  // Trigger observers before removing the document from the internal map.
  // Some observers might rely on that this document is still there.
  AppChanges Reason;
  Reason.Doc = pos->second;
  Reason.Why = AppChanges::Del;
  Notify(Reason);
  signalDeletedDocument(*pos->second);

  // For exception-safety use a smart pointer
  auto_ptr<Document> delDoc (pos->second);
  DocMap.erase( pos );

  return true;
}

App::Document* Application::getDocument(const char *Name) const
{
  map<string,Document*>::const_iterator pos;

  pos = DocMap.find(Name);

  if (pos == DocMap.end())
    return 0;

  return pos->second;
}

const char * Application::getDocumentName(const App::Document* doc) const
{
  for ( map<string,Document*>::const_iterator it = DocMap.begin(); it != DocMap.end(); ++it )
    if(it->second == doc) 
      return it->first.c_str();

  return 0;
}

std::vector<App::Document*> Application::getDocuments() const
{
  std::vector<App::Document*> docs;
  for ( map<string,Document*>::const_iterator it = DocMap.begin(); it != DocMap.end(); ++it )
    docs.push_back( it->second );
  return docs;
}

string Application::getUniqueDocumentName(const char *Name) const
{
 // strip ilegal chars
  string CleanName;
  const char *It=Name;

  while(*It != '\0')
  {
    if(   (*It>=48 && *It<=57)   // Numbers
        ||(*It>=65 && *It<=90)   // Upercase letters
        ||(*It>=97 && *It<=122)  // Upercase letters
       )
    {
      CleanName += *It;
    }else{
      switch(*It)
      {
      case 'ä': CleanName += "ae"; break;
      case 'ü': CleanName += "ue"; break;
      case 'ö': CleanName += "oe"; break;
      case 'Ä': CleanName += "Ae"; break;
      case 'Ü': CleanName += "Ue"; break;
      case 'Ö': CleanName += "Oe"; break;
      default:
        CleanName += '_';
      }
    }
    It++;
  }

  map<string,Document*>::const_iterator pos;

  // name in use?
  pos = DocMap.find(CleanName);

  if (pos == DocMap.end())
    // if not, name is OK
    return CleanName;
  else
  {
    // find highest suffix
    int nSuff = 0;  
    for(pos = DocMap.begin();pos != DocMap.end();++pos)
    {
      const string &rclObjName = pos->first;
      if (rclObjName.substr(0, strlen(CleanName.c_str())) == CleanName)  // Prefix gleich
      {
        string clSuffix(rclObjName.substr(strlen(CleanName.c_str())));
        if (clSuffix.size() > 0){
          int nPos = clSuffix.find_first_not_of("0123456789");
          if(nPos==-1)
            nSuff = max<int>(nSuff, atol(clSuffix.c_str()));
        }
      }
    }
    char szName[200];
    snprintf(szName, 200, "%s%d", CleanName.c_str(), nSuff + 1);
	
    return string(szName);
  }
	
}


Document* Application::openDocument(const char * FileName)
{
  FileInfo File(FileName);

  if ( !File.exists() ) {
    std::stringstream str;
    str << "File '" << FileName << "' does not exist!";
    throw Base::Exception(str.str().c_str());
  }

  // Before creating a new document we check whether the document is already open
  std::string filepath = File.filePath();
  for ( std::map<std::string,Document*>::iterator it = DocMap.begin(); it != DocMap.end(); ++it )
  {
    if ( filepath == it->second->FileName.getValue() )
    {
      std::stringstream str;
      str << "The project '" << FileName << "' is already open!";
      throw Base::Exception(str.str().c_str());
    }
  }

  Document* newDoc = newDocument(File.fileNamePure().c_str());

  newDoc->FileName.setValue(File.filePath());

  // read the document
  newDoc->open();

  return newDoc;
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
  // Allows that no active document is set.
  if(*Name == '\0'){
    _pActiveDoc = 0;
    return;
  }

  std::map<std::string,Document*>::iterator pos;
  pos = DocMap.find(Name);

  if(pos != DocMap.end())
	  _pActiveDoc = pos->second;
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

ParameterManager * Application::GetParameterSet(const char* sName) const
{
  std::map<std::string,ParameterManager *>::const_iterator it = mpcPramManager.find(sName);
  if ( it != mpcPramManager.end() )
    return it->second;
  else
    return 0;
}

const std::map<std::string,ParameterManager *> & Application::GetParameterSetList(void) const
{
	return mpcPramManager;
}

void Application::AddParameterSet(const char* sName)
{
  std::map<std::string,ParameterManager *>::const_iterator it = mpcPramManager.find(sName);
  if ( it != mpcPramManager.end() )
    return;
  mpcPramManager[sName] = new ParameterManager();
}

void Application::RemoveParameterSet(const char* sName)
{
  std::map<std::string,ParameterManager *>::iterator it = mpcPramManager.find(sName);
  // Must not delete user or system parameter
  if ( it == mpcPramManager.end() || it->second == _pcUserParamMngr || it->second == _pcSysParamMngr )
    return;
  delete it->second;
  mpcPramManager.erase(it);
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
  OpenTypeItem item;
  item.filter = Type;
  item.module = ModuleName;

  // Extract each file type from 'Type' literal
  std::string::size_type pos = item.filter.find("*.");
  while ( pos != std::string::npos )
  {
    unsigned int next = item.filter.find_first_of(" )", pos+1);
    unsigned int len = next-pos-2;
    std::string type = item.filter.substr(pos+2,len);
    item.types.push_back(type);
    pos = item.filter.find("*.", next);
  }

  // Due to branding stuuf replace FreeCAD through the application name
  if ( strncmp(Type, "FreeCAD", 7) == 0 ) {
    std::string AppName = Config()["ExeName"];
    AppName += item.filter.substr(7);
    item.filter = AppName;
    // put to the front of the array
    _mEndings.insert(_mEndings.begin(),item);
  } else {
    _mEndings.push_back(item);
  }
}

const char* Application::hasOpenType(const char* Type) const
{
  for ( std::vector<OpenTypeItem>::const_iterator it = _mEndings.begin(); it != _mEndings.end(); ++it )
  {
    const std::vector<std::string>& types = it->types;
    for ( std::vector<std::string>::const_iterator jt = types.begin(); jt != types.end(); ++jt )
    {
#ifdef __GNUC__
      if ( strcasecmp(Type,jt->c_str()) == 0 )
#else
      if ( _stricmp(Type,jt->c_str()) == 0 )
#endif
        return it->module.c_str();
    }
  }

  return 0;
}

void Application::rmvOpenType(const char* Type)
{
//  _mEndings.erase(Type);
}

std::map<std::string,std::string> Application::getOpenType(void) const
{
  std::map<std::string,std::string> endings;
  
  for ( std::vector<OpenTypeItem>::const_iterator it = _mEndings.begin(); it != _mEndings.end(); ++it )
  {
    const std::vector<std::string>& types = it->types;
    for ( std::vector<std::string>::const_iterator jt = types.begin(); jt != types.end(); ++jt )
    {
      endings[*jt] = it->module;;
    }
  }

  return endings;
}

std::vector<std::string> Application::getOpenFilter(void) const
{
  std::vector<std::string> filter;
  for ( std::vector<OpenTypeItem>::const_iterator it = _mEndings.begin(); it != _mEndings.end(); ++it )
  {
    filter.push_back(it->filter);
  }

  return filter;
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
  destructObserver();

  Base::Interpreter().finalize();

  ScriptFactorySingleton::Destruct();
  InterpreterSingleton::Destruct();

  Base::Type::destruct();
}

void Application::destructObserver(void)
{
  if ( _pConsoleObserverFile )
  {
    Console().DetachObserver(_pConsoleObserverFile);
    delete _pConsoleObserverFile; 
    _pConsoleObserverFile = 0;
  }
  if ( _pConsoleObserverStd )
  {
    Console().DetachObserver(_pConsoleObserverStd);
    delete _pConsoleObserverStd; 
    _pConsoleObserverFile = 0;
  }
}

/** freecadNewHandler()
 * prints an error message and throws an exception
 */
#ifdef _MSC_VER // New handler for Microsoft Visual C++ compiler
int __cdecl freecadNewHandler(size_t size )
{
  // throw an exception
  throw Base::MemoryException();
  return 0;
}
#else // Ansi C/C++ new handler
static void freecadNewHandler ()
{
  // throw an exception
  throw Base::MemoryException();
}
#endif


void Application::init(int argc, char ** argv)
{
  try {
    // install our own new handler
    #ifdef _MSC_VER // Microsoft compiler
      _set_new_handler ( freecadNewHandler ); // Setup new handler
      _set_new_mode( 1 ); // Re-route malloc failures to new handler !
    #else // Ansi compiler
      std::set_new_handler (freecadNewHandler); // ANSI new handler
    #endif

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
  catch (...)
  {
    // force to flush the log
    destructObserver();
    throw;
  }
}

void Application::initTypes(void)
{
  // Base types
  Base::Type                      ::init();
  Base::BaseClass                 ::init();
  Base::Exception                 ::init();
  Base::Persistance               ::init();
  // Properties
  App ::Property                  ::init();
  App ::PropertyContainer         ::init();
  App ::PropertyLists             ::init();
  App ::PropertyBool              ::init();
  App ::PropertyFloat             ::init();
  App ::PropertyFloatList         ::init();
  App ::PropertyFloatConstraint   ::init();
  App ::PropertyAngle             ::init();
  App ::PropertyDistance          ::init();
  App ::PropertyInteger           ::init();
  App ::PropertyIntegerConstraint ::init();
  App ::PropertyPercent           ::init();
  App ::PropertyEnumeration       ::init();
  App ::PropertyIntegerList       ::init();
  App ::PropertyString            ::init();
  App ::PropertyStringList        ::init();
  App ::PropertyLink              ::init();
  App ::PropertyLinkList          ::init();
  App ::PropertyMatrix            ::init();
  App ::PropertyVector            ::init();
  App ::PropertyVectorList        ::init();
  App ::PropertyColor             ::init();
  App ::PropertyColorList         ::init();
  App ::PropertyMaterial          ::init();
  App ::PropertyFile              ::init();
  // Document classes
  App ::DocumentObject            ::init();
  App ::AbstractFeature           ::init();
  App ::FeatureTest               ::init();
  App ::FeatureTestException      ::init();
  App ::FeaturePython             ::init();
  App ::Document                  ::init();
  App ::DocumentObjectGroup       ::init();

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
  mConfig["HomePath"] = FindHomePath(argv[0]);

	_argc = argc;
	_argv = argv;

	// extract home paths
	ExtractUser();

#	ifdef FC_DEBUG
		mConfig["Debug"] = "1";
#	else
		mConfig["Debug"] = "0";
#	endif

	// Parse the options which have impact to the init process
	ParseOptions(argc,argv);

  // init python
	mConfig["PythonSearchPath"] = Interpreter().init(argc,argv);
		
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
	
	// Banner ===========================================================
	if(!(mConfig["Verbose"] == "Strict"))
		Console().Message("%s %s, Libs: %s.%sR%s\n%s",mConfig["ExeName"].c_str(),
                                                  mConfig["ExeVersion"].c_str(),
                                                  mConfig["BuildVersionMajor"].c_str(),
                                                  mConfig["BuildVersionMinor"].c_str(),
                                                  mConfig["BuildRevision"].c_str(),
                                                  mConfig["ConsoleBanner"].c_str());
	else
		Console().Message("%s %s, Libs: %s.%sB%s\n",mConfig["ExeName"].c_str(),
                                                mConfig["ExeVersion"].c_str(),
                                                mConfig["BuildVersionMajor"].c_str(),
                                                mConfig["BuildVersionMinor"].c_str(),
                                                mConfig["BuildRevision"].c_str());

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
  Interpreter().runString(Base::ScriptFactory().ProduceScript("FreeCADInit"));
}

void Application::runApplication()
{
  std::map<std::string,std::string> EndingMap = App::GetApplication().getOpenType();

  // cycling through all the open files
  unsigned short count = 0;
  count = atoi(mConfig["OpenFileCount"].c_str());
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
    try{
      Interpreter().runFile(mConfig["FileName"].c_str());
    }catch(Base::Exception e){
      e.ReportException();
    }catch(...){
      Console().Error("Unknown exception in runing file:%s \n",mConfig["FileName"].c_str());
    }
    std::string text;
    text += "FreeCAD Console mode after runing:\n" + mConfig["FileName"] + "\n";
		Interpreter().runCommandLine(text.c_str());
	}
	else if(mConfig["RunMode"] == "Module")
	{
		// run a script
		Console().Log("Loading module: %s\n",mConfig["ScriptFileName"].c_str());
		Interpreter().loadModule(mConfig["FileName"].c_str());
#ifdef FC_DEBUG
		Interpreter().runCommandLine("FreeCAD Console mode");
#endif
	}
	else if(mConfig["RunMode"] == "Internal")
	{
		// run internal script
		Console().Log("Running internal script:\n");
		Interpreter().runString(Base::ScriptFactory().ProduceScript(mConfig["ScriptFileName"].c_str()));

		//!!! Interpreter().Launch(sScriptName);
	} else {

		Console().Log("Unknown Run mode (%d) in main()?!?\n\n",mConfig["RunMode"].c_str());
	}

}

void Application::logStatus()
{
  time_t now;
  time(&now);
  Console().Log("Init: Time: %s\n", ctime(&now));

  for(std::map<std::string,std::string>::iterator It = mConfig.begin();It!= mConfig.end();It++)
	{
		Console().Log("%s\t= %s\n",It->first.c_str(),It->second.c_str());
	}
}

void Application::LoadParameters(void)
{
	// create standard parameter sets
	_pcSysParamMngr = new ParameterManager();
	_pcUserParamMngr = new ParameterManager();

	// Init parameter sets ===========================================================
  //

  mConfig["UserParameter"]   = mConfig["UserAppData"] + "user.cfg";
	mConfig["SystemParameter"] = mConfig["UserAppData"] + "system.cfg";


  try {
	  if(_pcSysParamMngr->LoadOrCreateDocument(mConfig["SystemParameter"].c_str()) && !(mConfig["Verbose"] == "Strict"))
	  {
		  Console().Warning("   Parameter not existing, write initial one\n");
		  Console().Message("   This Warning means normaly FreeCAD running the first time or the\n"
		                       "   configuration was deleted or moved.Build up the standard configuration.\n");

	  }
  } catch (Base::Exception& e) {
    char szBuf[200];
    snprintf(szBuf, 200, "Malformed parameter file '%s'", mConfig["SystemParameter"].c_str());
    e.setMessage( szBuf );
    throw e;
  }

  try {
	  if(_pcUserParamMngr->LoadOrCreateDocument(mConfig["UserParameter"].c_str()) && !(mConfig["Verbose"] == "Strict"))
	  {
		  Console().Warning("   User settings not existing, write initial one\n");
		  Console().Message("   This Warning means normaly you running FreeCAD the first time\n"
		                       "   or your configuration was deleted or moved. The system defaults\n"
		                       "   will be reestablished for you.\n");

	  }
  } catch(Base::Exception& e) {
    char szBuf[200];
    snprintf(szBuf, 200, "Malformed parameter file '%s'", mConfig["UserParameter"].c_str());
    e.setMessage( szBuf );
    throw e;
  }
}


void Application::ParseOptions(int argc, char ** argv)
{
	static const char Usage[] = \
	" [Options] files..."\
	"Options:\n"\
	"  -h               Display this information\n"\
	"  -c               Runs FreeCAD in console mode (no windows)\n"\
	"  -cf file-name    Runs FreeCAD in server mode with script filename\n"\
	"  -cc file-name    Runs first the script and then console mode\n"\
	"  -cm module-name  Loads the Python module and exits when done\n"\
	"  -t0              Runs FreeCAD self test function\n"\
	"  -l               Enables logging in file FreeCAD.log\n"\
	"  -lf file-ame     Enables logging in file with the given filename\n"\
	"  -v               Runs FreeCAD in verbose mode\n"\
	"\n consult also the HTML documentation on http://free-cad.sourceforge.net/\n"\
	"";

  int OpenFileCount = 0;

  // logging in debug is on
#ifdef FC_DEBUG
	mConfig["LoggingFile"] = "1";
//	mConfig["LoggingFileName"]= mConfig["BinPath"] + "FreeCAD.log";
	mConfig["LoggingFileName"]= mConfig["UserAppData"] + "FreeCAD.log";
#endif

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
					case 'm':  
					case 'M':  
						mConfig["RunMode"] = "Module";
						if(argc <= i+1)
						{
              std::cerr << "Expecting a module name" << std::endl;  
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
				//mConfig["LoggingFileName"]= mConfig["BinPath"] + "FreeCAD.log";
      	mConfig["LoggingFileName"]= mConfig["UserAppData"] + "FreeCAD.log";
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
	//mConfig["FreeCADLib"] = EnvMacro::GetFreeCADLib(mConfig["HomePath"].c_str());

	// try to figure out the user
	char* user = getenv("USERNAME");
	if (user == NULL)
		user = getenv("USER");
	if (user == NULL)
		user = "Anonymous";
	mConfig["UserName"] = user;

  // On Linux systems the environment variable 'HOME' is set while Windows systems
  // have set 'HOMEDRIVE' and 'HOMEPATH' to get the user's home directory.
  // In the rare case that none of them are set we get the directory where FreeCAD is
  // installed as a fallback solution.
  //
  // Default paths for the user depending on the platform
#if defined(FC_OS_LINUX) || defined(FC_OS_CYGWIN)
  if(getenv("HOME") != 0)
    mConfig["UserHomePath"] = getenv("HOME");
#elif defined(FC_OS_WIN32)
  if(getenv("HOMEDRIVE") != 0 && getenv("HOMEPATH") != 0 )
    mConfig["UserHomePath"] = std::string(getenv("HOMEDRIVE")) + getenv("HOMEPATH");
#endif
  else
    mConfig["UserHomePath"] = mConfig["HomePath"];

  // In the second step we want the directory where user settings of FreeCAD can be
  // kept. On Windows usually 'APPDATA' is set supplemented by 'FreeCAD'.
  // On Linux the directory '.FreeCAD' (with a leading dot) is created directly under
  // the home path.
#if 0
  // Actually the name of the directory where the parameters are stored should be the name of
  // the application due to branding stuff. So make it active in one of the following releases.
  // FIXME: When activating this we should copy the parameter file from FreeCAD to the actual
  // directory (in case ExeName != FreeCAD)
  if(getenv("APPDATA") == 0)
    mConfig["UserAppData"] = mConfig["UserHomePath"] + "/." + mConfig["ExeName"] + "/";
  else
    mConfig["UserAppData"] = std::string(getenv("APPDATA")) + "/" + mConfig["ExeName"] + "/";
#else
  if(getenv("APPDATA") == 0)
    mConfig["UserAppData"] = mConfig["UserHomePath"] + "/.FreeCAD/";
  else
    mConfig["UserAppData"] = std::string(getenv("APPDATA")) + "/FreeCAD/";
#endif

  Base::FileInfo fi(mConfig["UserAppData"].c_str());
  if ( ! fi.exists() ) {
 	  if ( ! fi.createDirectory( mConfig["UserAppData"].c_str() ) )
    {
      // Want more details on console
      printf("Application::ExtractUser(): Could not create directory '%s'\n", mConfig["UserAppData"].c_str());
      // FIXME: Who should catch this exception?
      throw Base::Exception("Application::ExtractUser(): could not write in AppData directory!");
    }
  }
}

#if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN)
void SimplifyPath(std::string& sPath)
{
	// remove all unnecessary '/./' from sPath
	std::string sep; sep += PATHSEP;
	std::string pattern = sep + '.' + sep;
	std::string::size_type npos = sPath.find(pattern);
	while (npos != std::string::npos)
	{
		sPath.replace(npos, 3, sep);
		npos = sPath.find(pattern);
	}

	// remove all unnecessary '//' from sPath
	pattern = sep + sep;
	npos = sPath.find(pattern);
	while (npos != std::string::npos)
	{
		sPath.replace(npos, 2, sep);
		npos = sPath.find(pattern);
	}
}

std::string Application::FindHomePath(const char* sCall)
{
	std::string argv = sCall;
	std::string absPath;
	std::string homePath;
	std::string cwd;

	// get the current working directory
	char szDir[1024];
	if ( getcwd(szDir, sizeof(szDir)) == NULL)
		return homePath;
	cwd = szDir;

  // Called from Python
  if (Py_IsInitialized()) {
    PyObject* path_importer_cache = PySys_GetObject("path_importer_cache");
    if (path_importer_cache && PyDict_Check(path_importer_cache)) {
      PyObject *key, *value;
      int pos = 0;
      while (PyDict_Next(path_importer_cache, &pos, &key, &value)) {
        const char* KeyName = PyString_AsString(key);
			  std::string test = std::string(KeyName) + PATHSEP + argv;

			  // does it exist?
#if defined (__GNUC__)
 				if ( access(test.c_str(), 0) == 0 )
#else
 				if ( _access(test.c_str(), 0) == 0 )
#endif
			  {
				  absPath = test;
          if (KeyName == "")
            absPath = cwd + PATHSEP + absPath;
  			  break;
				}
      }
    }
  } else {
	  // absolute path
	  if ( argv[0] == PATHSEP )
	  {
		  absPath = argv;
#ifdef FC_DEBUG
  		printf("Absolute path: %s\n", absPath.c_str());
#endif
	  }
	  // relative path
	  else if ( argv.find(PATHSEP) != std::string::npos )
	  {
		  absPath = cwd + PATHSEP + argv;
#ifdef FC_DEBUG
		  printf("Relative path: %s\n", argv.c_str());
		  printf("Absolute path: %s\n", absPath.c_str());
#endif
	  }
	  // check PATH
	  else
	  {
#ifdef FC_DEBUG
  		printf("Searching in PATH variable...");
#endif
		  const char *pEnv = getenv( "PATH" );

		  if ( pEnv )
		  {
			  std::string path = pEnv;
			  std::vector<std::string> paths;

			  // split into each component
			  std::string::size_type start = 0;
			  std::string::size_type npos = path.find(':', start);
			  while ( npos != std::string::npos )
			  {
				  std::string tmp = path.substr(start, npos - start);
				  paths.push_back( path.substr(start, npos - start) );
				  start = npos + 1;
				  npos = path.find(':', start);
			  }

			  // append also last component
			  paths.push_back( path.substr(start) );

			  for (std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
			  {
				  std::string test = *it + PATHSEP + argv;

				  // no abs. path
				  if ( test[0] != PATHSEP )
					  test = cwd + PATHSEP + test;

				  // does it exist?
#if defined (__GNUC__)
  				if ( access(test.c_str(), 0) == 0 )
#else
  				if ( _access(test.c_str(), 0) == 0 )
#endif
				  {
					  absPath = test;
#ifdef FC_DEBUG
					  printf("found.\n");
					  printf("Absolute path: %s\n", absPath.c_str());
#endif
					  break;
				  }
			  }
		  }
	  }
  }

	// should be an absolute path now
	if (absPath[0] == PATHSEP)
	{
		SimplifyPath( absPath );
		std::string::size_type pos = absPath.find_last_of(PATHSEP);
		homePath.assign(absPath,0,pos);
		pos = homePath.find_last_of(PATHSEP);
		homePath.assign(homePath,0,pos+1);
	}
	else
	{
    // neither an absolute path in the specified call nor a relative path nor a call in PATH or PYTHONPATH
		printf("ERROR: no valid home path! (%s)\n", absPath.c_str());
		exit(0);
	}

	return homePath;
}
#elif defined (FC_OS_WIN32)
std::string Application::FindHomePath(const char* sCall)
{
  // We have three ways to start this application either use one of the both executables or
  // import the FreeCAD.pyd module from a running Python session. In the latter case the 
  // Python interpreter is already initialized.
	char  szFileName [MAX_PATH] ;
  if (Py_IsInitialized()) {
	  GetModuleFileName(GetModuleHandle(sCall),szFileName, MAX_PATH-1);
  } else {
	  GetModuleFileName(0, szFileName, MAX_PATH-1);
  }

	std::string Call(szFileName), TempHomePath;
	std::string::size_type pos = Call.find_last_of(PATHSEP);
	TempHomePath.assign(Call,0,pos);
	pos = TempHomePath.find_last_of(PATHSEP);
	TempHomePath.assign(TempHomePath,0,pos+1);

  // switch to posix style
  for(std::string::iterator i=TempHomePath.begin();i!=TempHomePath.end();++i) {
    if(*i == '\\')
      *i = '/';
  }

	return TempHomePath;
}
#else
# error "std::string Application::FindHomePath(const char*) not implemented"
#endif
