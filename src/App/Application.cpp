/***************************************************************************
                          FCApplication.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#include "PreCompiled.h"

#ifndef _PreComp_
#	include <TColStd_SequenceOfExtendedString.hxx>
#	include <TCollection_ExtendedString.hxx>
#endif


#include "Application.h"
#include "Document.h"
#include "Function.h"
#include "DocTypeAttr.h"
#include "DocType.h"

// FreeCAD Base header
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Parameter.h"
#include "../Base/Console.h"
#include "../Base/EnvMacros.h"
#include "../Base/Factory.h"

// static members
#include "../Version.h"
/// Major version nummber
const unsigned int FCApplication::VersionMajor = FCVersionMajor;
/// Minor version nummber
const unsigned int FCApplication::VersionMinor = FCVersionMinor;
/// Build nummber
const unsigned int FCApplication::VersionBuild = FCVersionBuild;
/// Build date
const char *       FCApplication::VersionDisDa = FCVersionDisDa;
/// Build date
const char *       FCApplication::VersionDate  = __DATE__;
/// Build date
const char *       FCApplication::VersionTime  = __TIME__;


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


Standard_CString FCApplicationOCC::ResourcesName()
{
	//return Standard_CString ("Resources");
	return Standard_CString ("Standard");	
}

void FCApplicationOCC::Formats(TColStd_SequenceOfExtendedString& Formats)
{
	Formats.Append(TCollection_ExtendedString ("FreeCad-Std"));
	Formats.Append(TCollection_ExtendedString ("MDTV-Standard"));
}

void FCApplicationOCC::InitViewer(const Handle(TDocStd_Document)& aDoc) const
{
	// Im not doing it here!
}


FCApplicationOCC::~FCApplicationOCC() {}

FCApplicationOCC::FCApplicationOCC()
{
	// Instanciate a TOcafFunction_BoxDriver and add it to the TFunction_DriverTable
//	TFunction_DriverTable::Get()->AddDriver(FCFunction::GetID(), 
//											new FCFunction());

}

// OCAF specific stuff


Standard_EXPORT Handle_Standard_Type& FCApplicationOCC_Type_()
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
			                                 sizeof(FCApplicationOCC),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle_FCApplicationOCC Handle_FCApplicationOCC::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle_FCApplicationOCC _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCApplicationOCC))) {
       _anOtherObject = Handle_FCApplicationOCC((Handle_FCApplicationOCC&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& FCApplicationOCC::DynamicType() const
{
  return STANDARD_TYPE(FCApplicationOCC) ;
}
Standard_Boolean FCApplicationOCC::IsKind(const Handle(Standard_Type)& AType) const
{
  return (STANDARD_TYPE(FCApplicationOCC) == AType || FCApplicationOCC::IsKind(AType));
}
Handle_FCApplicationOCC::~Handle_FCApplicationOCC() {}


//==========================================================================
// FCApplication
//==========================================================================

FCParameterManager *FCApplication::_pcSysParamMngr;
FCParameterManager *FCApplication::_pcUserParamMngr;

std::map<std::string,std::string> FCApplication::mConfig;


//**************************************************************************
// Construction and destruction

FCApplication::FCApplication(FCParameterManager *pcSysParamMngr, FCParameterManager *pcUserParamMngr,std::map<std::string,std::string> &mConfig)
	://_pcSysParamMngr(pcSysParamMngr),
	 //_pcUserParamMngr(pcUserParamMngr),
	 _mConfig(mConfig),
	 _pActiveDoc(0)
{
	_hApp = new FCApplicationOCC;
	mpcPramManager["System parameter"] = _pcSysParamMngr;
	mpcPramManager["User parameter"] = _pcUserParamMngr;

	// instanciate the workbench dictionary
	_pcTemplateDictionary = PyDict_New();

	// seting up Python binding
	(void) Py_InitModule("FreeCAD", FCApplication::Methods);

}

FCApplication::~FCApplication()
{
}


//**************************************************************************
// Interface

FCDocument* FCApplication::New(const char * Name)
{
	Handle_TDocStd_Document hDoc;
	FCDocument*				pDoc;
//	PyObject* pcTemplate;

	Base::Console().Log("FCApplication::New(%s)\n",Name);

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
	pDoc = new FCDocument(hDoc);

	// add the document to the internal list
//	pDoc->IncRef();
	_DocVector.push_back(pDoc);
	_pActiveDoc = pDoc;

	// creat the type object
	DocTypeStd *pDocType = new DocTypeStd();

	pDoc->InitType(pDocType);
	// trigger Observers (open windows and so on)
	NotifyDocNew(pDoc);

	return pDoc;
}

FCDocument* FCApplication::Open(const char * Name)
{
	Handle_TDocStd_Document hDoc;
	FCDocument*				pDoc;

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
	pDoc = new FCDocument(hDoc);
	pDoc->IncRef();
	_DocVector.push_back(pDoc);
	_pActiveDoc = pDoc;


	// trigger Observers (open windows and so on)
	NotifyDocNew(pDoc);

	return pDoc;
}

FCDocument* FCApplication::Save(void)
{
	FCDocument*	pDoc = Active();

	pDoc->Save();
	
	return pDoc;
}

FCDocument* FCApplication::SaveAs(const char * Name)
{
	FCDocument*	pDoc = Active();

	pDoc->SaveAs(Name);
	
	return pDoc;
}

FCDocument* FCApplication::Active(void)
{
	
	return _pActiveDoc;
}

void FCApplication::SetActive(FCDocument* pDoc)
{
	_pActiveDoc = pDoc;
}

FCParameterManager & FCApplication::GetSystemParameter(void) 
{
	return *_pcSysParamMngr;
}

FCParameterManager & FCApplication::GetUserParameter(void) 
{
	return *_pcUserParamMngr;
}

FCParameterManager & FCApplication::GetParameterSet(const char* sName)
{
	return *mpcPramManager[sName];
}

const std::map<std::string,FCParameterManager *> & FCApplication::GetParameterSetList(void)
{
	return mpcPramManager;
}

std::vector<std::string> FCApplication::GetAllTemplates(void)
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

FCHandle<FCParameterGrp>  FCApplication::GetParameterGroupByPath(const char* sName)
{
	std::string cName = sName,cTemp;

	std::string::size_type pos = cName.find(':');

	// is there a path seperator ?
	if(pos == std::string::npos)
	{
		throw Base::Exception("FCApplication::GetParameterGroupByPath() no parameter set name specified");
	} 
	// assigning the parameter set name
    cTemp.assign(cName,0,pos);
	cName.erase(0,pos+1);

	// test if name is valid
	std::map<std::string,FCParameterManager *>::iterator It = mpcPramManager.find(cTemp.c_str());
	if (It == mpcPramManager.end())
		throw Base::Exception("FCApplication::GetParameterGroupByPath() unknown parameter set name specified");

	return It->second->GetGroup(cName.c_str());
}


//**************************************************************************
// Python stuff

// FCApplication Methods						// Methods structure
PyMethodDef FCApplication::Methods[] = {
	{"DocNew",         (PyCFunction) FCApplication::sNew,            1},
	{"DocOpen",        (PyCFunction) FCApplication::sOpen,           1},
	{"DocSave"  ,      (PyCFunction) FCApplication::sSave,           1},
	{"DocSaveAs",      (PyCFunction) FCApplication::sSaveAs,         1},
	{"DocGet",         (PyCFunction) FCApplication::sGet,            1},
	{"ParamGet",       (PyCFunction) FCApplication::sGetParam,       1},
	{"Version",        (PyCFunction) FCApplication::sGetVersion,     1},
	{"ConfigGet",      (PyCFunction) FCApplication::sGetConfig,      1},
	{"ConfigSet",      (PyCFunction) FCApplication::sSetConfig,      1},
	{"ConfigDump",     (PyCFunction) FCApplication::sDumpConfig,     1},
	{"TemplateAdd",    (PyCFunction) FCApplication::sTemplateAdd,    1},
	{"TemplateDelete", (PyCFunction) FCApplication::sTemplateDelete ,1},
	{"TemplateGet",    (PyCFunction) FCApplication::sTemplateGet    ,1},

  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(FCApplication,sOpen)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
        return NULL;                             // NULL triggers exception


	try {
		// return new document
		return (GetApplication().Open(pstr)->GetPyObject());
	}
	catch(Base::Exception e) {
		PyErr_SetString(PyExc_IOError, e.what());
		return 0L;
	}
	catch(Standard_Failure e)
	{
		Handle(Standard_Failure) E = Standard_Failure::Caught();
		stringstream strm;

		strm << E << endl;
		//strm.freeze();
		PyErr_SetString(PyExc_IOError, strm.str().c_str());
		return 0L;
	}

}

PYFUNCIMP_S(FCApplication,sNew)
{
    char *pstr = 0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	PY_TRY{
		FCDocument*	pDoc = GetApplication().New(pstr);
		if (pDoc)
			return pDoc->GetPyObject();
		else
		{
			PyErr_SetString(PyExc_IOError, "Unknown Template");
			return NULL;
		}
	}PY_CATCH;
}


PYFUNCIMP_S(FCApplication,sSave)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors
}


PYFUNCIMP_S(FCApplication,sSaveAs)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}

PYFUNCIMP_S(FCApplication,sGet)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	if(pstr == 0)
		return GetApplication().Active()->GetPyObject();

	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}

PYFUNCIMP_S(FCApplication,sGetParam)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	PY_TRY{
		return GetPyObject(GetApplication().GetParameterGroupByPath(pstr)); 
	}PY_CATCH;
}


PYFUNCIMP_S(FCApplication,sGetConfig)
{
	char *pstr=0;

    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	if(pstr) // if parameter give deticated group
		return Py_BuildValue("s",GetApplication()._mConfig[pstr].c_str()); 
	else
	{
		PyObject *pDict = PyDict_New();
		for(std::map<std::string,std::string>::iterator It= GetApplication()._mConfig.begin();It!=GetApplication()._mConfig.end();It++)
		{
			PyBuf Buf(It->second.c_str()),Buf2(It->first.c_str());
			PyDict_SetItemString(pDict,Buf2.str,PyString_FromString(Buf.str));
		}
		return pDict;
		
	}
}

PYFUNCIMP_S(FCApplication,sDumpConfig)
{

    if (!PyArg_ParseTuple(args, "") )    // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	std::string str;
	for(std::map<std::string,std::string>::iterator It= GetApplication()._mConfig.begin();It!=GetApplication()._mConfig.end();It++)
	{
		str += It->first ;
		int size = It->first.size();
		for(int l = 0; l < (28-size) ; l++)
			str += " ";

		str += "= " + It->second + "\r\n";
	}
	return Py_BuildValue("s",str.c_str());
		
}

PYFUNCIMP_S(FCApplication,sSetConfig)
{
	char *pstr,*pstr2;

    if (!PyArg_ParseTuple(args, "ss", &pstr,&pstr2))  // convert args: Python->C 
        return NULL; // NULL triggers exception 

	GetApplication()._mConfig[pstr] = pstr2;

	Py_INCREF(Py_None);
	return Py_None;
}

PYFUNCIMP_S(FCApplication,sGetVersion)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL; // NULL triggers exception 

	PyObject* pList = PyList_New(6); 
	PyObject *pItem;
	pItem = PyInt_FromLong(FCApplication::VersionMajor);
	PyList_SetItem(pList, 0, pItem);
	pItem = PyInt_FromLong(FCApplication::VersionMinor);
	PyList_SetItem(pList, 1, pItem);
	pItem = PyInt_FromLong(FCApplication::VersionBuild);
	PyList_SetItem(pList, 2, pItem);
	pItem = PyString_FromString(FCApplication::VersionDisDa);
	PyList_SetItem(pList, 3, pItem);
	pItem = PyString_FromString(FCApplication::VersionTime);
	PyList_SetItem(pList, 4, pItem);
	pItem = PyString_FromString(FCApplication::VersionDate);
	PyList_SetItem(pList, 5, pItem);

	return pList;
}

PYFUNCIMP_S(FCApplication,sTemplateAdd)
{
	char*       psKey;
	PyObject*   pcObject;
	if (!PyArg_ParseTuple(args, "sO", &psKey,&pcObject))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	Py_INCREF(pcObject);

	PyDict_SetItemString(GetApplication()._pcTemplateDictionary,psKey,pcObject);

	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(FCApplication,sTemplateDelete)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	PyDict_DelItemString(GetApplication()._pcTemplateDictionary,psKey);

	Py_INCREF(Py_None);
    return Py_None;
} 
/*
PYFUNCIMP_S(FCApplication,sWorkbenchActivate)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	Instance->ActivateWorkbench(psKey);

  Py_INCREF(Py_None);
    return Py_None;
}
*/
PYFUNCIMP_S(FCApplication,sTemplateGet)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return GetApplication()._pcTemplateDictionary;
}

//**************************************************************************
// Init, Destruct and singelton

FCApplication * FCApplication::_pcSingelton = 0;

int FCApplication::_argc;
char ** FCApplication::_argv;


void FCApplication::Destruct(void)
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



void FCApplication::InitConfig(int argc, char ** argv, const char * sHomePath )
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
	DBG_CATCH(puts("error init Interpreter\n");exit(1);)

	DBG_TRY
		// Init console ===========================================================
		Console().AttacheObserver(new CmdConsoleObserver());
		if(mConfig["Verbose"] == "Strict") Console().SetMode(ConsoleSingelton::Verbose);
		// file logging fcility
		#	ifdef FC_DEBUG
			Console().AttacheObserver(new LoggingConsoleObserver("FreeCAD.log"));
		#	endif
	DBG_CATCH(puts("error init console\n");exit(2);)
	
	// Banner ===========================================================
	if(!(mConfig["Verbose"] == "Strict"))
		Console().Message("FreeCAD (c) 2001 Juergen Riegel (GPL,LGPL)\n\n%s",sBanner);
	else
		Console().Message("FreeCAD (c) 2001 Juergen Riegel (GPL,LGPL)\n\n");


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


}

void FCApplication::SetRunMode(const char* s)
{
	mConfig["RunMode"] = s;
}

void FCApplication::SaveEnv(const char* s)
{
	char *c = getenv(s);
	if(c)
		mConfig[s] = c;
}


void FCApplication::InitApplication(void)
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
	FCApplication::_pcSingelton = new FCApplication(0,0,mConfig);


	// starting the init script
	Interpreter().Launch(Base::ScriptFactory().ProduceScript("FreeCADInit"));


}

void FCApplication::RunApplication()
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

void FCApplication::DumpConfig()
{

	for(std::map<std::string,std::string>::iterator It = mConfig.begin();It!= mConfig.end();It++)
	{
		Console().Log("  %s\t= %s\n",It->first.c_str(),It->second.c_str());
	}

}


void FCApplication::LoadParameters(void)
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


void FCApplication::ParsOptions(int argc, char ** argv)
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
							Console().Error("Expecting a file\n");  
							Console().Error("\nUsage: %s %s",argv[0],Usage);  
						}
						mConfig["FileName"]= argv[i+1];
						i++;
					case 'c':  
					case 'C':  
						mConfig["RunMode"] = "ScriptCmd";
						if(argc <= i+1)
						{
							Console().Error("Expecting a file\n");  
							Console().Error("\nUsage: %s %s",argv[0],Usage);  
						}
						mConfig["FileName"]= argv[i+1];
						i++;
						break;   
					case '\0':  
						mConfig["RunMode"] = "Cmd";
						break;   
					default:  
						Console().Error("Invalid Input %s\n",argv[i]);  
						Console().Error("\nUsage: %s %s",argv[0],Usage);  
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
				printf("\nUsage: %s %s",argv[0],Usage);
				exit(0);
				//throw FCException("Comandline break");  
				break;  
			default: 
				printf("Invalid Option: %s\n",argv[i]); 
				printf("\nUsage: %s %s",argv[0],Usage); 
				exit(1);
			} 
		} 
		else  
		{ 
			Console().Error("Illegal command line argument #%d, %s\n",i,argv[i]); 
			Console().Error("\nUsage: %s %s",argv[0],Usage); 
			throw Base::Exception("Comandline error(s)");  
		} 
	}  
}  


void FCApplication::ExtractUser()
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


void FCApplication::CheckEnv(void)
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


void FCApplication::AttacheObserver(FCApplicationObserver *pcObserver)
{
	// double insert !!
	assert(_aclObservers.find(pcObserver) == _aclObservers.end() );

	_aclObservers.insert(pcObserver);
}

void FCApplication::DetacheObserver(FCApplicationObserver *pcObserver)
{
	_aclObservers.erase(pcObserver);
}


void FCApplication::NotifyDocNew(FCDocument* pcDoc)
{
	for(std::set<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocNew(pcDoc);   // send doc to the listener
}
void FCApplication::NotifyDocDelete(FCDocument* pcDoc)
{
	for(std::set<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocDelete(pcDoc);   // send doc to the listener
}
