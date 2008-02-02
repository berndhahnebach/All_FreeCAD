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
# include <unistd.h>
# endif
# include <boost/program_options.hpp>
#endif

#ifdef FC_OS_WIN32
# include <Shlobj.h>
# include <Shfolder.h>
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
#include <Base/BaseClass.h>
#include <Base/Persistance.h>
#include <Base/Reader.h>
#include <Base/VectorPy.h>
#include <Base/BoundBoxPy.h>

#include "MatrixPy.h"
#include "Feature.h"
#include "GeoFeature.h"
#include "FeatureTest.h"
#include "FeaturePython.h"
#include "ComplexGeoData.h"
#include "Property.h"
#include "PropertyContainer.h"
#include "PropertyStandard.h"
#include "PropertyLinks.h"
#include "Document.h"
#include "DocumentObjectGroup.h"
#include "InventorObject.h"
// If you stumble here, run the target "BuildExtractRevision" on Windows systems or the Python script "SubWCRev.py" on Linux based systems
// which builds src/Build/Version.h. Or create your own from src/Build/Version.h.in!
#include <Build/Version.h>

#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

using namespace App;
using namespace std;
using namespace boost;
using namespace boost::program_options;


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
     "The document has the read-only attribute FileName which points to the\n"
     "file the document should be stored to.\n"
    );

PyDoc_STRVAR(Console_doc,
     "FreeCAD Console\n"
    );

Application::Application(ParameterManager */*pcSysParamMngr*/, 
                         ParameterManager */*pcUserParamMngr*/,
                         std::map<std::string,std::string> &mConfig)
    ://_pcSysParamMngr(pcSysParamMngr),
    //_pcUserParamMngr(pcUserParamMngr),
    _mConfig(mConfig),
    _pActiveDoc(0)
{
    //_hApp = new ApplicationOCC;
    mpcPramManager["System parameter"] = _pcSysParamMngr;
    mpcPramManager["User parameter"] = _pcUserParamMngr;


    // setting up Python binding
    PyObject* pAppModule = Py_InitModule3("FreeCAD", Application::Methods, FreeCAD_doc);
    Py::Module(pAppModule).setAttr(std::string("ActiveDocument"),Py::None());

    PyObject* pConsoleModule = Py_InitModule3("Console", ConsoleSingleton::Methods, Console_doc);

    // introducing additional classes

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
    if (PyType_Ready(&Base::VectorPy::Type) < 0) return;
    union PyType_Object pyVecType = {&Base::VectorPy::Type};
    PyModule_AddObject(pAppModule, "Vector", pyVecType.o);

    if (PyType_Ready(&App::MatrixPy::Type) < 0) return;
    union PyType_Object pyMatType = {&App::MatrixPy::Type};
    PyModule_AddObject(pAppModule, "Matrix", pyMatType.o);

    if (PyType_Ready(&Base::BoundBoxPy::Type) < 0) return;
    union PyType_Object pyBoundBoxType = {&Base::BoundBoxPy::Type};
    PyModule_AddObject(pAppModule, "BoundBox", pyBoundBoxType.o);
    
    //insert Console
    Py_INCREF(pConsoleModule);
    PyModule_AddObject(pAppModule, "Console", pConsoleModule);
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

    if (pos != DocMap.end()) {
        Document* temp;
        temp = pos->second;
        DocMap.erase(pos);
        DocMap[NewName] = temp;
        signalRenameDocument(*temp);
    }
    else
        Base::Exception("Application::renameDocument(): no document with this name to rename!");

}

Document* Application::newDocument(const char * Name, const char * UserName)
{
    // get anyway a valid name!
    if (!Name)
        Name = "Unnamed";
    string name = getUniqueDocumentName(Name);

    // create the FreeCAD document
    auto_ptr<Document> newDoc(new Document() );

    // set the UserName
    if (UserName)
        newDoc->Name.setValue(UserName);
    else
        newDoc->Name.setValue(name);

    // add the document to the internal list
    DocMap[name] = newDoc.release(); // now owned by the Application
    _pActiveDoc = DocMap[name];

    // connect the signals to the application for the new document
    _pActiveDoc->signalNewObject.connect(boost::bind(&App::Application::slotNewObject, this, _1));
    _pActiveDoc->signalDeletedObject.connect(boost::bind(&App::Application::slotDeletedObject, this, _1));
    _pActiveDoc->signalChangedObject.connect(boost::bind(&App::Application::slotChangedObject, this, _1));
    _pActiveDoc->signalRenamedObject.connect(boost::bind(&App::Application::slotRenamedObject, this, _1));
    _pActiveDoc->signalActivatedObject.connect(boost::bind(&App::Application::slotActivatedObject, this, _1));


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
    signalDeleteDocument(*pos->second);

    // For exception-safety use a smart pointer
    if (_pActiveDoc == pos->second)
        setActiveDocument((Document*)0);
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
        if (it->second == doc)
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

    // check for first character whether it's a digit
    if ((*It != '\0') && (*It>=48 && *It<=57))
        CleanName = "_";

    while (*It != '\0') {
        if (   (*It>=48 && *It<=57)   // Numbers
             ||(*It>=65 && *It<=90)   // Upercase letters
             ||(*It>=97 && *It<=122)  // Upercase letters
           ) {
            CleanName += *It;
        }
        else {
            // All other letters gets replaced
            CleanName += '_';
        }

        It++;
    }

    map<string,Document*>::const_iterator pos;

    // name in use?
    pos = DocMap.find(CleanName);

    if (pos == DocMap.end())
        // if not, name is OK
        return CleanName;
    else {
        // find highest suffix
        int nSuff = 0;
        for (pos = DocMap.begin();pos != DocMap.end();++pos) {
            const string &rclObjName = pos->first;
            if (rclObjName.substr(0, strlen(CleanName.c_str())) == CleanName) { // Prefix gleich
                string clSuffix(rclObjName.substr(strlen(CleanName.c_str())));
                if (clSuffix.size() > 0) {
                    std::string::size_type nPos = clSuffix.find_first_not_of("0123456789");
                    if (nPos==std::string::npos)
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
    for ( std::map<std::string,Document*>::iterator it = DocMap.begin(); it != DocMap.end(); ++it ) {
        if ( filepath == it->second->FileName.getValue() ) {
            std::stringstream str;
            str << "The project '" << FileName << "' is already open!";
            throw Base::Exception(str.str().c_str());
        }
    }

    Document* newDoc = newDocument(File.fileNamePure().c_str());

    newDoc->FileName.setValue(File.filePath());

    // read the document
    newDoc->restore();

    return newDoc;
}

Document* Application::getActiveDocument(void) const
{
    return _pActiveDoc;
}

void Application::setActiveDocument(Document* pDoc)
{
    _pActiveDoc = pDoc;
    if (pDoc)
        signalActiveDocument(*pDoc);
}

void Application::setActiveDocument(const char *Name)
{
    // Allows that no active document is set.
    if (*Name == '\0') {
        _pActiveDoc = 0;
        return;
    }

    std::map<std::string,Document*>::iterator pos;
    pos = DocMap.find(Name);

    if (pos != DocMap.end()) {
        setActiveDocument(pos->second);
    }
    else {
        std::stringstream s;
        s << "Try to activate unknown document '" << Name << "'";
        throw Base::Exception(s.str());
    }
}

const char* Application::GetHomePath(void)
{
    return _mConfig["AppHomePath"].c_str();
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

Base::Reference<ParameterGrp>  Application::GetParameterGroupByPath(const char* sName)
{
    std::string cName = sName,cTemp;

    std::string::size_type pos = cName.find(':');

    // is there a path seperator ?
    if (pos == std::string::npos) {
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
    while ( pos != std::string::npos ) {
        std::string::size_type next = item.filter.find_first_of(" )", pos+1);
        std::string::size_type len = next-pos-2;
        std::string type = item.filter.substr(pos+2,len);
        item.types.push_back(type);
        pos = item.filter.find("*.", next);
    }

    // Due to branding stuff replace FreeCAD through the application name
    if ( strncmp(Type, "FreeCAD", 7) == 0 ) {
        std::string AppName = Config()["ExeName"];
        AppName += item.filter.substr(7);
        item.filter = AppName;
        // put to the front of the array
        _mEndings.insert(_mEndings.begin(),item);
    }
    else {
        _mEndings.push_back(item);
    }
}

const char* Application::hasOpenType(const char* Type) const
{
    for ( std::vector<OpenTypeItem>::const_iterator it = _mEndings.begin(); it != _mEndings.end(); ++it ) {
        const std::vector<std::string>& types = it->types;
        for ( std::vector<std::string>::const_iterator jt = types.begin(); jt != types.end(); ++jt ) {
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

void Application::rmvOpenType(const char* /*Type*/)
{
//  _mEndings.erase(Type);
}

std::map<std::string,std::string> Application::getOpenType(void) const
{
    std::map<std::string,std::string> endings;

    for ( std::vector<OpenTypeItem>::const_iterator it = _mEndings.begin(); it != _mEndings.end(); ++it ) {
        const std::vector<std::string>& types = it->types;
        for ( std::vector<std::string>::const_iterator jt = types.begin(); jt != types.end(); ++jt ) {
            endings[*jt] = it->module;;
        }
    }

    return endings;
}

std::vector<std::string> Application::getOpenFilter(void) const
{
    std::vector<std::string> filter;
    for ( std::vector<OpenTypeItem>::const_iterator it = _mEndings.begin(); it != _mEndings.end(); ++it ) {
        filter.push_back(it->filter);
    }

    return filter;
}

//**************************************************************************
// signaling
void Application::slotNewObject(App::DocumentObject&O)
{
    this->signalNewObject(O);
}

void Application::slotDeletedObject(App::DocumentObject&O)
{
    this->signalDeletedObject(O);
}

void Application::slotChangedObject(App::DocumentObject&O)
{
    this->signalChangedObject(O);
}

void Application::slotRenamedObject(App::DocumentObject&O)
{
    this->signalRenamedObject(O);
}

void Application::slotActivatedObject(App::DocumentObject&O)
{
    this->signalActivatedObject(O);
}


//**************************************************************************
// Init, Destruct and singleton

Application * Application::_pcSingleton = 0;

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
    assert(_pcSingleton);
    delete _pcSingleton;

    // We must detach from console and delete the observer to save our file
    destructObserver();

    Base::Interpreter().finalize();

    ScriptFactorySingleton::Destruct();
    InterpreterSingleton::Destruct();
    Base::Type::destruct();
}

void Application::destructObserver(void)
{
    if ( _pConsoleObserverFile ) {
        Console().DetachObserver(_pConsoleObserverFile);
        delete _pConsoleObserverFile;
        _pConsoleObserverFile = 0;
    }
    if ( _pConsoleObserverStd ) {
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

        boost::filesystem::path::default_name_check(boost::filesystem::no_check);

        if (argc==0) {
            char* buf = new char[256];
            strncpy(buf,mConfig["ExeName"].c_str(),98);
            initConfig(1,reinterpret_cast<char **>(&buf));
            delete [] buf;
            buf = 0;
        }
        else
            initConfig(argc,argv);

        initApplication();
    }
    catch (...) {
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
    // Complex data classes
    Data::ComplexGeoData            ::init();
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
    App ::PropertyPlacement         ::init();
    App ::PropertyPlacementLink     ::init();
    App ::PropertyComplexGeoData    ::init();
    App ::PropertyColor             ::init();
    App ::PropertyColorList         ::init();
    App ::PropertyMaterial          ::init();
    App ::PropertyFile              ::init();
    // Document classes
    App ::DocumentObject            ::init();
    App ::AbstractFeature           ::init();
    App ::GeoFeature                ::init();
    App ::FeatureTest               ::init();
    App ::FeatureTestException      ::init();
    App ::FeaturePython             ::init();
    App ::Document                  ::init();
    App ::DocumentObjectGroup       ::init();
    App ::InventorObject            ::init();
}

void Application::initConfig(int argc, char ** argv)
{
    // find the home path....
    mConfig["AppHomePath"] = FindHomePath(argv[0]);

    // Version of the Application. Extractet of SubWCRef into src/Build/Version.h
    App::Application::Config()["BuildVersionMajor"]  = FCVersionMajor;
    App::Application::Config()["BuildVersionMinor"]  = FCVersionMinor;
    App::Application::Config()["BuildRevision"]      = FCRevision;
    App::Application::Config()["BuildRevisionRange"] = FCRevisionRange;
    App::Application::Config()["BuildRepositoryURL"] = FCRepositoryURL;
    App::Application::Config()["BuildRevisionDate"]  = FCRevisionDate;
    App::Application::Config()["BuildCurrentDate"]   = FCCurrentDateT;
    App::Application::Config()["BuildScrClean"]      = FCScrClean;
    App::Application::Config()["BuildScrMixed"]    = FCScrMixed;


    _argc = argc;
    _argv = argv;

    // extract home paths
    ExtractUserPath();

#   ifdef FC_DEBUG
    mConfig["Debug"] = "1";
#   else
    mConfig["Debug"] = "0";
#   endif

    // Parse the options which have impact to the init process
    ParseOptions(argc,argv);

    // init python
    mConfig["PythonSearchPath"] = Interpreter().init(argc,argv);

    // Init console ===========================================================
    _pConsoleObserverStd = new ConsoleObserverStd();
    Console().AttachObserver(_pConsoleObserverStd);
    if (mConfig["Verbose"] == "Strict")
        Console().SetMode(ConsoleSingleton::Verbose);

    // file logging Init ===========================================================
    if (mConfig["LoggingFile"] == "1") {
        _pConsoleObserverFile = new ConsoleObserverFile(mConfig["LoggingFileName"].c_str());
        Console().AttachObserver(_pConsoleObserverFile);
    }
    else
        _pConsoleObserverFile = 0;

    // Banner ===========================================================
    if (!(mConfig["Verbose"] == "Strict"))
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
    if (c)
        mConfig[s] = c;
}

void Application::initApplication(void)
{
    // interpreter and Init script ==========================================================
    // register scripts
    new ScriptProducer( "FreeCADInit",    FreeCADInit    );
    new ScriptProducer( "FreeCADTest",    FreeCADTest    );

    // creating the application
    if (!(mConfig["Verbose"] == "Strict")) Console().Log("Create Application");
    Application::_pcSingleton = new Application(0,0,mConfig);

    // starting the init script
    Interpreter().runString(Base::ScriptFactory().ProduceScript("FreeCADInit"));
}

void Application::processCmdLineFiles(void)
{
    Base::Console().Log("Init: Processing command line files\n");

    std::map<std::string,std::string> EndingMap = App::GetApplication().getOpenType();

    // cycling through all the open files
    unsigned short count = 0;
    count = atoi(mConfig["OpenFileCount"].c_str());
    std::string File;

    if (count == 0 && mConfig["RunMode"] == "Exit")
        mConfig["RunMode"] = "Cmd";

    for (unsigned short i=0; i<count; i++) {
        // getting file name
        std::ostringstream temp;
        temp << "OpenFile" << i;

        FileInfo File(mConfig[temp.str()].c_str());

        std::string Ext = File.extension();
        Base::Console().Log("Init:     Processing file: %s\n",File.filePath().c_str());
        try {

            if (Ext == "FCStd" || Ext == "std") {
                // try to open
                Application::_pcSingleton->openDocument(File.filePath().c_str());
            }
            else if (Ext == "FCscript") {
                Base::Interpreter().runFile(File.filePath().c_str());
            }
            else if (Ext == "py") {
                Base::Interpreter().loadModule(File.filePath().c_str());
            }
            else if (EndingMap.find(Ext) != EndingMap.end()) {
                Base::Interpreter().loadModule(EndingMap.find(Ext)->second.c_str());
                Base::Interpreter().runStringArg("import %s",EndingMap.find(Ext)->second.c_str());
                Base::Interpreter().runStringArg("%s.open(\"%s\")",EndingMap.find(Ext)->second.c_str(),File.filePath().c_str());
                Base::Console().Log("Command line open: %s.Open(\"%s\")",EndingMap.find(Ext)->second.c_str(),File.filePath().c_str());
            }
            else {
                Console().Warning("File format not suported: %s \n", File.filePath().c_str());
            }

        }
        catch (const Base::Exception& e) {
            Console().Error("Exception while processing file: %s [%s]\n", File.filePath().c_str(), e.what());
        }
        catch (...) {
            Console().Error("Unknown exception while processing file: %s \n", File.filePath().c_str());
        }
    }
}

void Application::runApplication()
{
    // process all files given through command line interface
    processCmdLineFiles();

    if (mConfig["RunMode"] == "Cmd") {
        // Run the comandline interface
        Interpreter().runCommandLine("FreeCAD Console mode");
    }
    else if (mConfig["RunMode"] == "Internal") {
        // run internal script
        Console().Log("Running internal script:\n");
        Interpreter().runString(Base::ScriptFactory().ProduceScript(mConfig["ScriptFileName"].c_str()));
    }
    else if (mConfig["RunMode"] == "Exit") {
        // geting out
        Console().Log("Exiting on purpose\n");
    }
    else {
        Console().Log("Unknown Run mode (%d) in main()?!?\n\n",mConfig["RunMode"].c_str());
    }
}

void Application::logStatus()
{
    time_t now;
    time(&now);
    Console().Log("Init: Time: %s\n", ctime(&now));

    for (std::map<std::string,std::string>::iterator It = mConfig.begin();It!= mConfig.end();It++) {
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
        if (_pcSysParamMngr->LoadOrCreateDocument(mConfig["SystemParameter"].c_str()) && !(mConfig["Verbose"] == "Strict")) {
            Console().Warning("   Parameter not existing, write initial one\n");
            Console().Message("   This warning normally means that FreeCAD is running the first time\n"
                              "   or the configuration was deleted or moved. Build up the standard\n"
                              "   configuration.\n");
        }
    }
    catch (Base::Exception& e) {
        char szBuf[200];
        snprintf(szBuf, 200, "Malformed parameter file '%s'", mConfig["SystemParameter"].c_str());
        e.setMessage( szBuf );
        throw e;
    }

    try {
        if (_pcUserParamMngr->LoadOrCreateDocument(mConfig["UserParameter"].c_str()) && !(mConfig["Verbose"] == "Strict")) {
            Console().Warning("   User settings not existing, write initial one\n");
            Console().Message("   This warning normally means that FreeCAD is running the first time\n"
                              "   or your configuration was deleted or moved. The system defaults\n"
                              "   will be reestablished for you.\n");

        }
    }
    catch (Base::Exception& e) {
        char szBuf[200];
        snprintf(szBuf, 200, "Malformed parameter file '%s'", mConfig["UserParameter"].c_str());
        e.setMessage( szBuf );
        throw e;
    }
}


#if 1

#if defined(FC_OS_WIN32)
// fix weird error while linking boost???
namespace boost
{
namespace program_options {
std::string arg="arg";
}
}
#endif

pair<string, string> customSyntax(const string& s)
{
    if (s.find("-display") == 0)
        return make_pair(string("display"), string("null"));
    else if (s.find("-style") == 0)
        return make_pair(string("style"), string("null"));
    else if (s.find("-geometry") == 0)
        return make_pair(string("geometry"), string("null"));
    else if (s.find("-font") == 0)
        return make_pair(string("font"), string("null"));
    else if (s.find("-fn") == 0)
        return make_pair(string("fn"), string("null"));
    else if (s.find("-background") == 0)
        return make_pair(string("background"), string("null"));
    else if (s.find("-bg") == 0)
        return make_pair(string("bg"), string("null"));
    else if (s.find("-foreground") == 0)
        return make_pair(string("foreground"), string("null"));
    else if (s.find("-fg") == 0)
        return make_pair(string("fg"), string("null"));
    else if (s.find("-button") == 0)
        return make_pair(string("button"), string("null"));
    else if (s.find("-button") == 0)
        return make_pair(string("button"), string("null"));
    else if (s.find("-btn") == 0)
        return make_pair(string("btn"), string("null"));
    else if (s.find("-name") == 0)
        return make_pair(string("name"), string("null"));
    else if (s.find("-title") == 0)
        return make_pair(string("title"), string("null"));
    else if (s.find("-visual") == 0)
        return make_pair(string("visual"), string("null"));
//  else if (s.find("-ncols") == 0)
//    return make_pair(string("ncols"), boost::program_options::value<int>(1));
//  else if (s.find("-cmap") == 0)
//    return make_pair(string("cmap"), string("null"));
    else if ('@' == s[0])
        return std::make_pair(string("response-file"), s.substr(1));
    else
        return make_pair(string(), string());

}

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}

void Application::ParseOptions(int ac, char ** av)
{
    // Declare a group of options that will be
    // allowed only on command line
    options_description generic("Generic options");
    generic.add_options()
    ("version,v", "print version string")
    ("help,h", "print help message")
    ("console,c", "start in console mode")
    ("response-file", value<string>(),"can be specified with '@name', too")

    ;
    // Declare a group of options that will be
    // allowed both on command line and in
    // config file
    boost::program_options::options_description config("Configuration");
    config.add_options()
    ("write-log,l", value<string>(), "write a log file")
    ("run-test,t",   value<int>()   ,"test level")
    ("module-path,M", value< vector<string> >()->composing(),"additional module paths")
    ("python-path,P", value< vector<string> >()->composing(),"additional python paths")
    ;


    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    boost::program_options::options_description hidden("Hidden options");
    hidden.add_options()
    ("input-file",  boost::program_options::value< vector<string> >(), "input file")
    // this are to ignore for the window system (QApplication)
    ("style",      boost::program_options::value< string >(), "set the application GUI style")
    ("display",    boost::program_options::value< string >(), "set the X-Server")
    ("geometry ",  boost::program_options::value< string >(), "set the X-Window geometry")
    ("font",       boost::program_options::value< string >(), "set the X-Window font")
    ("fn",         boost::program_options::value< string >(), "set the X-Window font")
    ("background", boost::program_options::value< string >(), "set the X-Window background color")
    ("bg",         boost::program_options::value< string >(), "set the X-Window background color")
    ("foreground", boost::program_options::value< string >(), "set the X-Window foreground color")
    ("fg",         boost::program_options::value< string >(), "set the X-Window foreground color")
    ("button",     boost::program_options::value< string >(), "set the X-Window button color")
    ("btn",        boost::program_options::value< string >(), "set the X-Window button color")
    ("name",       boost::program_options::value< string >(), "set the X-Window name")
    ("title",      boost::program_options::value< string >(), "set the X-Window title")
    ("visual",     boost::program_options::value< string >(), "set the X-Window to color scema")
    ("ncols",      boost::program_options::value< int    >(), "set the X-Window to color scema")
    ("cmap",                                                  "set the X-Window to color scema")
    ;

    // Ignored options, will be savely ignored. Mostly uses by underlaying libs.
    //boost::program_options::options_description x11("X11 options");
    //x11.add_options()
    //    ("display",  boost::program_options::value< string >(), "set the X-Server")
    //    ;

    options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    boost::program_options::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    boost::program_options::options_description visible("Allowed options");
    visible.add(generic).add(config);

    boost::program_options::positional_options_description p;
    p.add("input-file", -1);

    variables_map vm;
    try {
        store( boost::program_options::command_line_parser(ac, av).
               options(cmdline_options).positional(p).extra_parser(customSyntax).run(), vm);

        ifstream ifs("FreeCAD.cfg");
        store(parse_config_file(ifs, config_file_options), vm);
        notify(vm);
    }
    catch (const std::exception& e) {
        cout << e.what() << endl << endl << visible << endl;
        exit(1);
    }
    catch (...) {
        cout << "Wrong or unknown option, bailing out!" << endl << endl << visible << endl;
        exit(1);
    }

    if (vm.count("help")) {
        cout << "FreeCAD" << endl<<endl;
        cout << "For detaild descripton see http://free-cad.sf.net" << endl<<endl;
        cout << "Usage:" << endl << "FreeCAD [options] File1 File2 ....." << endl;
        cout << visible << "\n";
        exit( 0);
    }

    if (vm.count("response-file")) {
        // Load the file and tokenize it
        ifstream ifs(vm["response-file"].as<string>().c_str());
        if (!ifs) {
            Base::Console().Error("Could no open the response file\n");
            exit(1);;
        }
        // Read the whole file into a string
        stringstream ss;
        ss << ifs.rdbuf();
        // Split the file content
        char_separator<char> sep(" \n\r");
        tokenizer<char_separator<char> > tok(ss.str(), sep);
        vector<string> args;
        copy(tok.begin(), tok.end(), back_inserter(args));
        // Parse the file and store the options
        store( boost::program_options::command_line_parser(ac, av).
               options(cmdline_options).positional(p).extra_parser(customSyntax).run(), vm);
    }


    if (vm.count("version")) {
        cout << "FreeCAD\n";
        exit( 0);
    }

    if (vm.count("console")) {
        mConfig["RunMode"] = "Cmd";

    }

    if (vm.count("module-path")) {
        vector<string> Mods = vm["module-path"].as< vector<string> >();
        string temp;
        for ( vector<string>::const_iterator It= Mods.begin();It != Mods.end();++It)
            temp += *It + ";";
        temp.erase(temp.end()-1);

        mConfig["AdditionalModulePaths"] = temp;
    }

    if (vm.count("python-path")) {
        vector<string> Paths = vm["python-path"].as< vector<string> >();
        string temp;
        for ( vector<string>::const_iterator It= Paths.begin();It != Paths.end();++It)
            Base::Interpreter().addPythonPaths( It->c_str());
    }

    if (vm.count("input-file")) {
        vector<string> files(vm["input-file"].as< vector<string> >());
        int OpenFileCount=0;
        for (vector<string>::const_iterator It = files.begin();It != files.end();++It) {

            //cout << "Input files are: "
            //     << vm["input-file"].as< vector<string> >() << "\n";

            std::ostringstream temp;
            temp << "OpenFile" << OpenFileCount;
            mConfig[temp.str()] = *It;
            OpenFileCount++;
        }
        std::ostringstream buffer;
        buffer << OpenFileCount;
        mConfig["OpenFileCount"] = buffer.str();
    }

    if (vm.count("write-log")) {
        mConfig["LoggingFile"] = "1";
        mConfig["LoggingFileName"] = vm["write-log"].as<string>();
    }

    if (vm.count("run-test")) {
        int level = vm["run-test"].as<int>();
        switch (level) {
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
    }

}
#else
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
    for (int i = 1; i < argc; i++) {
        if (*argv[i] == '-' ) {
            switch (argv[i][1]) {
                // Console modes
            case 'c':
            case 'C':
                switch (argv[i][2]) {
                    // Console with file
                case 'f':
                case 'F':
                    mConfig["RunMode"] = "Script";
                    if (argc <= i+1) {
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
                    if (argc <= i+1) {
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
                    if (argc <= i+1) {
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
                switch (argv[i][2]) {
                    // Console with file
                case 'f':
                case 'F':
                    if (argc <= i+1) {
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
                switch (argv[i][2]) {
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
                switch (argv[i][2]) {
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
        else {
            // store all file (URLS) names to open
            if (OpenFileCount < 56534) {

                std::ostringstream temp;
                temp << "OpenFile" << OpenFileCount;
                mConfig[temp.str()] = argv[i];
                OpenFileCount++;
            }
            else {
                std::cerr << "\nToo many arguments! All arguments above 56534 will be ignored!!";
            }

        }
    }
    std::ostringstream buffer;
    buffer << OpenFileCount;
    mConfig["OpenFileCount"] = buffer.str();

}
#endif

void Application::ExtractUserPath()
{
    // std paths
    mConfig["BinPath"] = mConfig["AppHomePath"] + "bin" + PATHSEP;
    mConfig["DocPath"] = mConfig["AppHomePath"] + "doc" + PATHSEP;

#if defined(FC_OS_LINUX) || defined(FC_OS_CYGWIN) || defined(FC_OS_MACOSX)
    // On Linux systems the environment variable 'HOME' points to the user home path.
    //
    // Default paths for the user depending on the platform
    if (getenv("HOME") != 0)
        mConfig["UserHomePath"] = getenv("HOME");

    // Try to write into our data path
    std::string appData = mConfig["UserHomePath"];
    appData += PATHSEP;
    appData += ".";
    appData += mConfig["ExeVendor"];
    Base::FileInfo fi(appData.c_str());
    if (!fi.exists()) {
        if (!fi.createDirectory(appData.c_str())) {
            // Want more details on console
            printf("Application::ExtractUserPath(): Could not create directory '%s'\n", appData.c_str());
            // FIXME: Who should catch this exception?
            throw Base::Exception("Application::ExtractUserPath(): could not write in AppData directory!");
        }
    }

    appData += PATHSEP;
    appData += mConfig["ExeName"];
    fi.setFile(appData.c_str());
    if (!fi.exists()) {
        if (!fi.createDirectory(appData.c_str())) {
            // Want more details on console
            printf("Application::ExtractUserPath(): Could not create directory '%s'\n", appData.c_str());
            // FIXME: Who should catch this exception?
            throw Base::Exception("Application::ExtractUserPath(): could not write in AppData directory!");
        }
    }

    // Actually the name of the directory where the parameters are stored should be the name of
    // the application due to branding reasons.
    appData += PATHSEP;
    mConfig["UserAppData"] = appData;

#elif defined(FC_OS_WIN32)
    TCHAR szPath[MAX_PATH];
    // Get the default path where we can save our documents. It seems that
    // 'CSIDL_MYDOCUMENTS' doesn't work on all machines, so we use 'CSIDL_PERSONAL'
    // which does the same.
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, szPath)))
        mConfig["UserHomePath"] = szPath;
    else if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath)))
        mConfig["UserHomePath"] = szPath;
    else
        mConfig["UserHomePath"] = mConfig["AppHomePath"];

    // In the second step we want the directory where user settings of the application can be
    // kept. There we create a directory with name of the vendor and a sub-directory with name
    // of the application.
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
        std::string appData = szPath;

        // Try to write into our data path
        appData += PATHSEP;
        appData += mConfig["ExeVendor"];
        Base::FileInfo fi(appData.c_str());
        if (!fi.exists()) {
            if (!fi.createDirectory(appData.c_str())) {
                // Want more details on console
                printf("Application::ExtractUserPath(): Could not create directory '%s'\n", appData.c_str());
                std::string error = "Cannot create directory ";
                error += appData;
                throw Base::Exception(error);
            }
        }

        appData += PATHSEP;
        appData += mConfig["ExeName"];
        fi.setFile(appData.c_str());
        if (!fi.exists()) {
            if (!fi.createDirectory(appData.c_str())) {
                // Want more details on console
                printf("Application::ExtractUserPath(): Could not create directory '%s'\n", appData.c_str());
                std::string error = "Cannot create directory ";
                error += appData;
                throw Base::Exception(error);
            }
        }

        // Actually the name of the directory where the parameters are stored should be the name of
        // the application due to branding reasons.
        appData += PATHSEP;
        mConfig["UserAppData"] = appData;
    }
#else
# error "Implement ExtractUserPath() for your platform."
#endif
}

#if defined (FC_OS_LINUX)
std::string Application::FindHomePath(const char* sCall)
{
    // We have three ways to start this application either use one of the both executables or
    // import the FreeCAD.so module from a running Python session. In the latter case the
    // Python interpreter is already initialized.
    std::string Call, TempHomePath;
    if (Py_IsInitialized()) {
        // sCall must already be the right path. See also MainPy.cpp
        // Possibly we get only a relative path then we need to prepend the cwd
        if (sCall[0] != '/') {
            // get the current working directory
            char szDir[PATH_MAX];
            getcwd(szDir, sizeof(szDir));
            Call = szDir;
            Call += '/';
            Call += sCall;
        }
        else {
            Call = sCall;
        }
    }
    else {
        // Find the path of the executable
        char szDir[PATH_MAX];
        int n = readlink("/proc/self/exe", szDir, PATH_MAX);
        if (n != -1) {
            Call = szDir;
        }
    }

    std::string::size_type pos = Call.find_last_of(PATHSEP);
    TempHomePath.assign(Call,0,pos);
    pos = TempHomePath.find_last_of(PATHSEP);
    TempHomePath.assign(TempHomePath,0,pos+1);

    return TempHomePath;
}

#elif defined(FC_OS_MACOSX)
#include <mach-o/dyld.h>
#include <string>
#include <stdlib.h>
#include <sys/param.h>

std::string Application::FindHomePath(const char* call)
{
    uint32_t sz = 0;
    char *buf;

    _NSGetExecutablePath(NULL, &sz); //function only returns "sz" if first arg is to small to hold value
    buf = (char*) malloc(++sz);

    if (_NSGetExecutablePath(buf, &sz) == 0) {
        char resolved[PATH_MAX];
        char* path = realpath(buf, resolved);
        free(buf);

        if (path) {
            std::string Call(resolved), TempHomePath;
            std::string::size_type pos = Call.find_last_of(PATHSEP);
            TempHomePath.assign(Call,0,pos);
            pos = TempHomePath.find_last_of(PATHSEP);
            TempHomePath.assign(TempHomePath,0,pos+1);
            return TempHomePath;
        }
    }

    return call; // error
}

#elif defined(FC_OS_CYGWIN)
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

std::string Application::FindHomePath(const char* sCall)
{
    std::string absPath;
    std::string homePath;

    // Called from Python
    if (Py_IsInitialized()) {
        char resolved[PATH_MAX];
        if (realpath(sCall, resolved))
            absPath = resolved;
    }
    else {
        // Find the path of the executable
        char resolved[PATH_MAX];
        int n = readlink("/proc/self/exe", resolved, PATH_MAX);
        if (n != -1) {
            absPath = resolved;
        }
    }

    // should be an absolute path now
    std::string::size_type pos = absPath.find_last_of(PATHSEP);
    homePath.assign(absPath,0,pos);
    pos = homePath.find_last_of(PATHSEP);
    homePath.assign(homePath,0,pos+1);

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
    }
    else {
        GetModuleFileName(0, szFileName, MAX_PATH-1);
    }

    std::string Call(szFileName), TempHomePath;
    std::string::size_type pos = Call.find_last_of(PATHSEP);
    TempHomePath.assign(Call,0,pos);
    pos = TempHomePath.find_last_of(PATHSEP);
    TempHomePath.assign(TempHomePath,0,pos+1);

    // switch to posix style
    for (std::string::iterator i=TempHomePath.begin();i!=TempHomePath.end();++i) {
        if (*i == '\\')
            *i = '/';
    }

    return TempHomePath;
}

#else
# error "std::string Application::FindHomePath(const char*) not implemented"
#endif
