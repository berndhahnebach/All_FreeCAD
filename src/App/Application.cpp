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
#	include <TColStd_SequenceOfExtendedString.hxx>
#	include <TCollection_ExtendedString.hxx>
#endif


#include "Application.h"
#include "Document.h"
#include "Function.h"
//#include "DocTypeAttr.h"
//#include "DocType.h"

// FreeCAD Base header
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Parameter.h"
#include "../Base/Console.h"
#include "../Base/EnvMacros.h"
#include "../Base/Factory.h"

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


// the standard and plugin file of FreeCAD
#include "Standard.h"
#include "Plugin.h"

// scriptings (scripts are build in but can be overriden by command line option)
#include "InitScript.h"
#include "TestScript.h"

#ifdef FC_OS_LINUX
using namespace std;
#endif

//using Base::GetConsole;
using namespace Base;
using namespace App;



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
  return (STANDARD_TYPE(ApplicationOCC) == AType || ApplicationOCC::IsKind(AType));
}
Handle_ApplicationOCC::~Handle_ApplicationOCC() {}


//==========================================================================
// Application
//==========================================================================

FCParameterManager *App::Application::_pcSysParamMngr;
FCParameterManager *App::Application::_pcUserParamMngr;
Base::ConsoleObserverStd  *Application::_pConsoleObserverStd;
Base::ConsoleObserverFile *Application::_pConsoleObserverFile;

std::map<std::string,std::string> Application::mConfig;


//**************************************************************************
// Construction and destruction

Application::Application(FCParameterManager *pcSysParamMngr, FCParameterManager *pcUserParamMngr,std::map<std::string,std::string> &mConfig)
	://_pcSysParamMngr(pcSysParamMngr),
	 //_pcUserParamMngr(pcUserParamMngr),
	 _mConfig(mConfig),
	 _pActiveDoc(0)
{
	_hApp = new ApplicationOCC;
	mpcPramManager["System parameter"] = _pcSysParamMngr;
	mpcPramManager["User parameter"] = _pcUserParamMngr;

	// instanciate the workbench dictionary
	_pcTemplateDictionary = PyDict_New();

	// seting up Python binding
	(void) Py_InitModule("FreeCAD", Application::Methods);

}

Application::~Application()
{
}


//**************************************************************************
// Interface

Document* Application::New(const char * Name)
{
	Handle_TDocStd_Document hDoc;
	Document*				pDoc;
//	PyObject* pcTemplate;

	Base::Console().Log("Application::New(%s)\n",Name);

/*	if(Name)
	{
		// net buffer because of char* <-> const char*
		PyBuf NameBuf(Name);

		// get the python template object from the dictionary
		pcTemplate = PyDict_GetItemString(_pcTemplateDictionary, NameBuf.str);

		// test if the template is valid
		if(!pcTemplate)
			return NULL;
	}*/

	_hApp->NewDocument("FreeCad-Std",hDoc);
	//_hApp->NewDocument("MDTV-Standard",hDoc);
	//_hApp->NewDocument("Standard",hDoc);
	pDoc = new Document(hDoc);

	// add the document to the internal list
//	pDoc->IncRef();
	_DocVector.push_back(pDoc);
	_pActiveDoc = pDoc;

	// creat the type object
	//DocTypeStd *pDocType = new DocTypeStd();

	pDoc->Init();
	// trigger Observers (open windows and so on)
	NotifyDocNew(pDoc);

	return pDoc;
}

Document* Application::Open(const char * Name)
{
	Handle_TDocStd_Document hDoc;
	Document*				pDoc;

	// create new (empty) document 
	//pDoc = New("Standard");

	// load
	// TCollection_ExtendedString aName = (Standard_CString) Name;
	switch(_hApp->Open(TCollection_ExtendedString((Standard_CString) Name),hDoc) )
	{
		case CDF_RS_OK:
			break;
		case CDF_RS_UnknownDocument:
      throw Base::Exception("Unknown Document");
		case CDF_RS_AlreadyRetrieved:
			throw Base::Exception("Already Retrieved");
		case CDF_RS_AlreadyRetrievedAndModified:
			throw Base::Exception("AlreadyRetrievedAndModified");
		case CDF_RS_NoDriver:
			throw Base::Exception("NoDriver");
		case CDF_RS_NoVersion:
			throw Base::Exception("NoVersion");
		case CDF_RS_NoModel:
			throw Base::Exception("NoModel");
		case CDF_RS_TypeNotFoundInSchema:
			throw Base::Exception("TypeNotFoundInSchema");
		case CDF_RS_UnrecognizedFileFormat:
			throw Base::Exception("UnrecognizedFileFormat");
		case CDF_RS_PermissionDenied:
			throw Base::Exception("PermissionDenied");
		default:
			throw Base::Exception("Unknown open error");
	}
	
	// Creating a FreeCAD Document
	pDoc = new Document(hDoc);
	pDoc->IncRef();
	_DocVector.push_back(pDoc);
	_pActiveDoc = pDoc;


	// trigger Observers (open windows and so on)
	NotifyDocNew(pDoc);

	return pDoc;
}

Document* Application::Save(void)
{
	Document*	pDoc = Active();

	pDoc->Save();
	
	return pDoc;
}

Document* Application::SaveAs(const char * Name)
{
	Document*	pDoc = Active();

	pDoc->SaveAs(Name);
	
	return pDoc;
}

Document* Application::Active(void)
{
	
	return _pActiveDoc;
}

void Application::SetActive(Document* pDoc)
{
	_pActiveDoc = pDoc;
}

FCParameterManager & Application::GetSystemParameter(void) 
{
	return *_pcSysParamMngr;
}

FCParameterManager & Application::GetUserParameter(void) 
{
	return *_pcUserParamMngr;
}

FCParameterManager & Application::GetParameterSet(const char* sName)
{
	return *mpcPramManager[sName];
}

const std::map<std::string,FCParameterManager *> & Application::GetParameterSetList(void)
{
	return mpcPramManager;
}

std::vector<std::string> Application::GetAllTemplates(void)
{
	PyObject *key, *value;
	int pos = 0;
	std::vector<std::string> cTemp;
     

	// insert all items
	while (PyDict_Next(_pcTemplateDictionary, &pos, &key, &value)) {
		/* do something interesting with the values... */
		cTemp.push_back(PyString_AsString(key));
	}

	return cTemp;
}

FCHandle<FCParameterGrp>  Application::GetParameterGroupByPath(const char* sName)
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
	std::map<std::string,FCParameterManager *>::iterator It = mpcPramManager.find(cTemp.c_str());
	if (It == mpcPramManager.end())
		throw Base::Exception("Application::GetParameterGroupByPath() unknown parameter set name specified");

	return It->second->GetGroup(cName.c_str());
}



//**************************************************************************
// Init, Destruct and singelton

Application * Application::_pcSingelton = 0;

int Application::_argc;
char ** Application::_argv;


void Application::Destruct(void)
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
}



void Application::InitConfig(int argc, char ** argv, const char * sHomePath )
{
	
	static const char sBanner[] = \
"  #####                 ####  ###   ####  \n" \
"  #                    #      # #   #   # \n" \
"  #     ##  #### ####  #     #   #  #   # \n" \
"  ####  # # #  # #  #  #     #####  #   # \n" \
"  #     #   #### ####  #    #     # #   # \n" \
"  #     #   #    #     #    #     # #   #  ##  ##  ##\n" \
"  #     #   #### ####   ### #     # ####   ##  ##  ##\n\n" ;

	
	_argc = argc;
	_argv = argv;

	// use home path out of the main modules
	mConfig["HomePath"] = sHomePath;

	// extract home path
	ExtractUser();


	// first check the environment variables
	CheckEnv();

#	ifdef FC_DEBUG
		mConfig["Debug"] = "1";
#	else
		mConfig["Debug"] = "0";
#	endif

	// Pars the options which have impact to the init process
	ParsOptions(argc,argv);


	DBG_TRY
		// init python
		Interpreter().SetComLineArgs(argc,argv);
	DBG_CATCH(puts("Application::InitConfig() error init Python Interpreter\n");exit(1);)

	DBG_TRY
		
    // Init console ===========================================================
    _pConsoleObserverStd = new ConsoleObserverStd();
		Console().AttacheObserver(_pConsoleObserverStd);
		if(mConfig["Verbose"] == "Strict") 
      Console().SetMode(ConsoleSingelton::Verbose);

    // file logging Init ===========================================================
    if(mConfig["LoggingFile"] == "1"){
		  _pConsoleObserverFile = new ConsoleObserverFile(mConfig["LoggingFileName"].c_str());
		  Console().AttacheObserver(_pConsoleObserverFile);
    }else
      _pConsoleObserverFile = 0;


	DBG_CATCH(puts("error init console\n");exit(2);)
	
	// Banner ===========================================================
	if(!(mConfig["Verbose"] == "Strict"))
		Console().Message("FreeCAD %d.%dB%d (c) 2001-2005 Juergen Riegel (GPL,LGPL)\n\n%s",Application::VersionMajor,
                                                                                       Application::VersionMinor,
                                                                                       Application::VersionBuild,
                                                                                       sBanner);
	else
		Console().Message("FreeCAD %d.%dB%d (c) 2001-2005 Juergen Riegel (GPL,LGPL)\n\n",Application::VersionMajor,
                                                                                     Application::VersionMinor,
                                                                                     Application::VersionBuild);


	LoadParameters();

	// capture python variables
	SaveEnv("PYTHONPATH");
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

  LogStatus();

}

void Application::SetRunMode(const char* s)
{
	mConfig["RunMode"] = s;
}

void Application::SaveEnv(const char* s)
{
	char *c = getenv(s);
	if(c)
		mConfig[s] = c;
}


void Application::InitApplication(void)
{

	// checking on the plugin files of OpenCasCade
	std::fstream cTempStream;
	cTempStream.open((mConfig["HomePath"]+PATHSEP+"Plugin").c_str(),ios::out);
	cTempStream << Plu ;
	cTempStream.close();
	cTempStream.open((mConfig["HomePath"]+PATHSEP+"Standard").c_str(),ios::out);
	cTempStream << Stand ;
	cTempStream.close();


	// interpreter and Init script ==========================================================
	// register scripts
	new ScriptProducer( "FreeCADInit",    FreeCADInit    );
	new ScriptProducer( "FreeCADTest",    FreeCADTest    );


	// creating the application
	if(!(mConfig["Verbose"] == "Strict")) Console().Log("Create Application");
	Application::_pcSingelton = new Application(0,0,mConfig);


	// starting the init script
	Interpreter().Launch(Base::ScriptFactory().ProduceScript("FreeCADInit"));


}

void Application::RunApplication()
{


	if(mConfig["RunMode"] == "Cmd")
	{
		// Run the comandline interface
		Interpreter().RunCommandLine("Console mode");
	}
	else if(mConfig["RunMode"] == "Script")
	{
		// run a script
		Console().Log("Running script: %s\n",mConfig["ScriptFileName"].c_str());
		Interpreter().LaunchFile(mConfig["FileName"].c_str());
	}
	else if(mConfig["RunMode"] == "ScriptCmd")
	{
		// run a script
		Console().Log("Running script: %s\n",mConfig["ScriptFileName"].c_str());
		Interpreter().LaunchFile(mConfig["FileName"].c_str());
		Interpreter().RunCommandLine("FreeCAD Console mode");
	}
	else if(mConfig["RunMode"] == "Internal")
	{
		// run internal script
		Console().Log("Running internal script:\n");
		Interpreter().Launch(Base::ScriptFactory().ProduceScript(mConfig["ScriptFileName"].c_str()));

		//!!! Interpreter().Launch(sScriptName);
	} else {

		Console().Log("Unknown Run mode (%d) in main()?!?\n\n",mConfig["RunMode"].c_str());
		exit(1);
	}

}

void Application::LogStatus()
{
  SYSTEMTIME time;

  GetSystemTime(&time);

  Console().Log("Time: %d-%d-%d %d:%d,%d\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
	for(std::map<std::string,std::string>::iterator It = mConfig.begin();It!= mConfig.end();It++)
	{
		Console().Log("  %s\t= %s\n",It->first.c_str(),It->second.c_str());
	}

}


void Application::LoadParameters(void)
{
	// create standard parameter sets
	_pcSysParamMngr = new FCParameterManager();
	_pcUserParamMngr = new FCParameterManager();

		// Init parameter sets ===========================================================
	mConfig["UserParameter"]  += mConfig["HomePath"] + "FC" + mConfig["UserName"] + ".FCParam";
	mConfig["SystemParameter"] = mConfig["HomePath"] + "AppParam.FCParam";

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


void Application::ParsOptions(int argc, char ** argv)
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
				mConfig["LoggingFileName"]= "FreeCAD.log";
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
      std::cerr << "Illegal command line argument #" << i << " " << argv[i] << std::endl; 
			std::cerr << "\nUsage: " << argv[0] << Usage;
			throw Base::Exception("Comandline error(s)");  
		} 
	}  
}  


void Application::ExtractUser()
{
	// std paths
	mConfig["BinPath"] = mConfig["HomePath"] + PATHSEP + "bin" + PATHSEP;
	mConfig["DocPath"] = mConfig["HomePath"] + PATHSEP + "doc" + PATHSEP;

	// try to figure out if using FreeCADLib
	mConfig["FreeCADLib"] = GetFreeCADLib(mConfig["HomePath"].c_str());

	// try to figure out the user
	char* user = getenv("USERNAME");
	if (user == NULL)
		user = getenv("USER");
	if (user == NULL)
		user = "Anonymous";
	mConfig["UserName"] = user;

	PrintPath();
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
	SetPluginDefaults(mConfig["HomePath"].c_str());

	// sets all needed varables if a FreeCAD LibPack is found
	if(mConfig["FreeCADLib"] != "")
	{
		// sets the python environment variables if the FREECADLIB variable is defined
		SetPythonToFreeCADLib(mConfig["FreeCADLib"].c_str());

		// sets the OpenCasCade environment variables if the FREECADLIB variable is defined
		SetCasCadeToFreeCADLib(mConfig["FreeCADLib"].c_str());
	}

	cout << flush;

	bool bFailure=false;

	TestEnvExists("CSF_MDTVFontDirectory",bFailure);
	TestEnvExists("CSF_MDTVTexturesDirectory",bFailure);
	TestEnvExists("CSF_UnitsDefinition",bFailure);
	TestEnvExists("CSF_UnitsLexicon",bFailure);

	if (bFailure) {
     		cerr<<"Environment Error(s)"<<endl<<sEnvErrorText1;
		exit(1);
	}

}
//**************************************************************************
// Observer stuff


void Application::AttacheObserver(ApplicationObserver *pcObserver)
{
	// double insert !!
	assert(_aclObservers.find(pcObserver) == _aclObservers.end() );

	_aclObservers.insert(pcObserver);
}

void Application::DetacheObserver(ApplicationObserver *pcObserver)
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
