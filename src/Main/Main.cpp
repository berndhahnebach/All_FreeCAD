/** \file main.cpp
 *  \brief Main!
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License (GPL)            *
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

#include "../Config.h"


#include <stdio.h>

#ifdef FC_OS_LINUX
#	include <unistd.h>
#endif

#ifdef FC_OS_WIN32
#	include <direct.h>
#	include <windows.h>
#endif


// FreeCAD Base header
#include "../Base/Console.h"
#include "../Base/Interpreter.h"
#include "../Base/Parameter.h"
#include "../Base/Exception.h"

// FreeCAD doc header
#include "../App/Application.h"


#include "Standard_Failure.hxx"
#include <xercesc/util/XMLException.hpp>
#include "iostream"

// FreeCAD Gui header

#ifdef  _FC_GUI_ENABLED_
#	include <qapplication.h>
#	include "../Gui/Application.h"
#	include "../Gui/GuiConsole.h"
#	include "../Gui/Splashscreen.h"
#	ifdef FC_OS_WIN32
#		pragma comment(lib,QTLIBNAME)
#	endif 
#	include "GuiInitScript.h"
#endif



const char sBanner[] = \
"  #####                 ####  ###   ####  \n" \
"  #                    #      # #   #   # \n" \
"  #     ##  #### ####  #     #   #  #   # \n" \
"  ####  # # #  # #  #  #     #####  #   # \n" \
"  #     #   #### ####  #    #     # #   # \n" \
"  #     #   #    #     #    #     # #   #  ##  ##  ##\n" \
"  #     #   #### ####   ### #     # ####   ##  ##  ##\n\n" ;

char sFCUser[200];


// scriptings (scripts are build in but can be overriden by command line option)
#include "InitScript.h"
#include "TestScript.h"
#include "TestEnvScript.h"
#include "StartupScript.h"
#include "InstallScript.h"

#include <string>
#include <map>

// run control default action
#ifdef _FC_GUI_ENABLED_
	/// The QT Application need to be set very early because of splasher
	QApplication* pcQApp = NULL;
	FCSplashScreen *splash = 0;
#endif

std::map<std::string,std::string> mConfig;


// some globals set by the commandline options or Init function
const char*     sScriptName;
/// pointer to the system parameter (loaded in Init())
FCParameterManager *pcSystemParameter;
/// pointer to the user parameter (loaded in Init())
FCParameterManager *pcUserParameter;


// forwards
void Init(int argc, char ** argv );
void Destruct(void);
void ParsOptions(int argc, char ** argv);
void CheckEnv(void);
void PrintInitHelp(void);
void ExtractPathAndUser(const char*);




int main( int argc, char ** argv ) 
{

  // Init phase ===========================================================
#	ifndef FC_DEBUG
	try
	{
#	endif
		// extract home path
		ExtractPathAndUser(argv[0]);

		// first check the environment variables
		CheckEnv();
		// Initialization (phase 1)
		Init(argc,argv);

		// the FreeCAD Application

#	ifndef FC_DEBUG
	}
	// catch all OCC exceptions
	catch(Standard_Failure e)
	{
		Handle(Standard_Failure) E = Standard_Failure::Caught();
		cout << "An Open CasCade exception was caught:"<< endl << E << endl;
		PrintInitHelp();
		exit(10);
	}
	// catch all FC exceptions
	catch(FCException e)
	{
		GetConsole().Error("Application init failed:");
		e.ReportException();
		PrintInitHelp();
		exit(20);
	}
	// catch XML exceptions
	catch (XMLException& e)
	{
		GetConsole().Error("Application init failed:");
		GetConsole().Error(StrX(e.getMessage()).c_str());
		PrintInitHelp();
		exit(30);
	}

	// catch all the (nasty) rest
	catch(...)
	{
		GetConsole().Error("Application init failed, because of a really nesty (unknown) error...");
		PrintInitHelp();
		exit(40);
	}
#	endif


	// Run phase ===========================================================

	int ret;
#	ifndef FC_DEBUG
	try
	{
#	endif
		if(mConfig["RunMode"] == "Gui")
		{
		// run GUI
#			ifdef _FC_GUI_ENABLED_
				// A new QApplication
				GetConsole().Log("Creating GUI Application...\n");
				// if application not yet created
				if (!pcQApp)  pcQApp = new QApplication ( argc, argv );
				ApplicationWindow * mw = new ApplicationWindow();
				pcQApp->setMainWidget(mw);

				// runing the Gui init script
				GetInterpreter().Launch(FreeCADGuiInit);

				// show the main window
				GetConsole().Log("Showing GUI Application...\n");
				mw->show();
				pcQApp->connect( pcQApp, SIGNAL(lastWindowClosed()), pcQApp, SLOT(quit()) );


				// close splasher
				if (splash)
				{
				  // wait a short moment
				  QWaitCondition().wait(1000);
				  // if splasher is still busy terminate it
				  splash->bRun = false;
				}

				// run the Application event loop
				GetConsole().Log("Running event loop...\n");
				ret = pcQApp->exec();
				GetConsole().Log("event loop left\n");
				delete pcQApp;

#			else
				GetConsole().Error("GUI mode not possible. This is a FreeCAD compiled without GUI. Use FreeCAD -c\n");
#			endif
			}
		else if(mConfig["RunMode"] == "Cmd")
		{
			// Run the comandline interface
			ret = GetInterpreter().RunCommandLine("Console mode");
		}
		else if(mConfig["RunMode"] == "Script")
		{
			// run a script
			GetConsole().Log("Running script: %s\n",mConfig["ScriptFileName"].c_str());
			GetInterpreter().LaunchFile(mConfig["ScriptFileName"].c_str());
		}
		else if(mConfig["RunMode"] == "Internal")
		{
			// run internal script
			GetConsole().Log("Running internal script:\n");
			GetInterpreter().Launch(sScriptName);
		} else {
		
			GetConsole().Log("Unknown Run mode in main()?!?\n\n");
			exit(1);
		}
#	ifndef FC_DEBUG
	}
	catch(Standard_Failure e)
	{
		GetConsole().Error("Running the application failed, OCC exception caught:\n");
		Handle(Standard_Failure) E = Standard_Failure::Caught();
		cout << "An exception was caught " << E << endl;
		exit(4);
	}
	catch(FCException e)
	{
		GetConsole().Error("Running the application failed:\n");
		e.ReportException();
		exit(5); 
	}
	catch(...)
	{
		GetConsole().Error("Running the application failed, because of a really nesty (unknown) error...\n\n");
		exit(6);
	}
#	endif
	// Destruction phase ===========================================================

	GetConsole().Log("FreeCAD terminating...\n\n");
#	ifndef FC_DEBUG
	try
	{
#	endif
		// cleans up 
		Destruct();

#	ifndef FC_DEBUG
	}
	catch(...)
	{
		GetConsole().Error("Destruction of the application failed, because of a really nesty (unknown) error...\n\n");
		exit(6);
	}
#	endif
	GetConsole().Log("FreeCAD completely terminated\n\n");

	return 0;
}

/** The Destruct function
 * close and destruct everything created during Init()
 */
void Destruct(void)
{
	pcSystemParameter->SaveDocument(mConfig["SystemParameter"].c_str());
	pcUserParameter->SaveDocument(mConfig["UserParameter"].c_str());
	delete pcSystemParameter;
	delete pcUserParameter;
}


/** The Init function
 * Initialize all the stuff and running the init script:
 * - parsing the options
 * - starting splasher
 * - Launching the FCInterpreter (starting python)
 * - Launching the FCConsole
 * - Call the Init script ("(FreeCADDir)/scripts/FreeCADInit.py")
 **/
void Init(int argc, char ** argv )
{

	// set some config defaults
#	ifdef _FC_GUI_ENABLED_
		mConfig["RunMode"] = "Gui";
#	else
		mConfig["RunMode"] = "Cmd";
#	endif
#	ifdef FC_DEBUG
		mConfig["Debug"] = "1";
#	else
		mConfig["Debug"] = "0";
#	endif

	// Pars the options which have impact to the init process
	ParsOptions(argc,argv);


	mConfig["UserParameter"]  += mConfig["HomePath"] + "FC" + mConfig["UserName"] + ".FCParam";
	mConfig["SystemParameter"] = mConfig["HomePath"] + "AppParam.FCParam";


	// Splasher phase ===========================================================
	#	ifdef _FC_GUI_ENABLED_
		// startup splasher
		// when runnig in verbose mode no splasher
		if ( ! (mConfig["Verbose"] == "Strict") && (mConfig["RunMode"] == "Gui") )
		{
			pcQApp = new QApplication ( argc, argv );
			splash = new FCSplashScreen(QApplication::desktop());
			pcQApp->setMainWidget(splash);
		}
	# endif

	// init python
	GetInterpreter();

	// Init console ===========================================================
	GetConsole().AttacheObserver(new FCCmdConsoleObserver());
	if(mConfig["Verbose"] == "Strict") GetConsole().SetMode(FCConsole::Verbose);
	// file logging fcility
#	ifdef FC_DEBUG
		GetConsole().AttacheObserver(new FCLoggingConsoleObserver("FreeCAD.log"));
#	endif

	// Banner ===========================================================
	if(!(mConfig["Verbose"] == "Strict"))
		GetConsole().Message("FreeCAD (c) 2001 Juergen Riegel (GPL,LGPL)\n\n%s",sBanner);
	else
		GetConsole().Message("FreeCAD (c) 2001 Juergen Riegel (GPL,LGPL)\n\n");

	pcSystemParameter = new FCParameterManager();
	pcUserParameter = new FCParameterManager();


	// Init parameter sets ===========================================================
	if(pcSystemParameter->LoadOrCreateDocument(mConfig["SystemParameter"].c_str()) && !(mConfig["Verbose"] == "Strict"))
	{
		GetConsole().Warning("   Parameter not existing, write initial one\n");
		GetConsole().Message("   This Warning means normaly FreeCAD running the first time or the\n"
		                     "   configuration was deleted or moved.Build up the standard configuration.\n");

	}

	if(pcUserParameter->LoadOrCreateDocument(mConfig["UserParameter"].c_str()) && !(mConfig["Verbose"] == "Strict"))
	{
		GetConsole().Warning("   User settings not existing, write initial one\n");
		GetConsole().Message("   This Warning means normaly you running FreeCAD the first time\n"
		                     "   or your configuration was deleted or moved. The system defaults\n"
		                     "   will be reestablished for you.\n");

	}

	
	// interpreter and Init script ==========================================================
	// Start the python interpreter
	FCInterpreter &rcInterperter = GetInterpreter();
	rcInterperter.SetComLineArgs(argc,argv);
	//rcInterperter.Launch("print 'Python started'\n");
	
	// starting the startup script
	rcInterperter.Launch(FreeCADStartup);

	// creating the application 
	if(!(mConfig["Verbose"] == "Strict")) GetConsole().Message("Create Application");
	FCApplication::_pcSingelton = new FCApplication(pcSystemParameter,pcUserParameter,mConfig);

	// starting the init script
	rcInterperter.Launch(FreeCADInit);

}

//**************************************************************************
// extracting the home path

void ExtractPathAndUser(const char* sCall)
{
	std::string Call(sCall);
	static std::string Temp;

	std::string::size_type pos = Call.find_last_of('\\');

	Temp.assign(Call,0,pos+1);

	mConfig["HomePath"] = Temp.c_str();

	// try to figure out the user
	 char* user = getenv("USERNAME");
	if (user == NULL) 
		user = getenv("USER");
	if (user == NULL) 
		user = "Anonymous";
	mConfig["UserName"] = user;

}

//**************************************************************************
// checking the environment


const char sEnvErrorText1[] = \
"It seems some of the variables needed by FreeCAD are not set\n"\
"or wrong set. This regards the Open CasCade variables:\n"\
"CSF_GRAPHICSHR=C:\\CasRoot\\Windows_NT\\dll\\opengl.dll\n"\
"CSF_MDTVFONTDIRECTORY=C:\\CasRoot\\src\\FontMFT\\\n"\
"CSF_MDTVTEXTURESDIRECTORY=C:\\CasRoot\\src\\Textures\\\n"\
"CSF_UNITSDEFINITION=C:\\CasRoot\\src\\UnitsAPI\\Units.dat\n"\
"CSF_UNITSLEXICON=C:\\CasRoot\\src\\UnitsAPI\\Lexi_Expr.dat\n"\
"Please reinstall OpenCasCade!\n\n";

const char sEnvErrorText2[] = \
"It seems some of the variables needed by FreeCAD are not set\n"\
"or wrong set. This regards the Open CasCade variables:\n"\
"XXX=C:\\CasRoot\\Windows_NT\\dll\\opengl.dll\n"\
"Please reinstall XXX!\n\n";


void CheckEnv(void)
{
	int bFailure = 0;
	// set the resource env variables
	char  szString [256] ;
	char  szDirectory [256] ;

	getcwd (szDirectory,sizeof szDirectory);
#	ifdef FC_OS_WIN32	
#		define PATHSEP "\\"
#	else
#		define PATHSEP "/"
#	endif
	if (szDirectory[strlen(szDirectory)-1] != PATHSEP[0]) {
		strcat(szDirectory,PATHSEP);
	}
	sprintf(szString,"CSF_StandardDefaults=%s",szDirectory);
	putenv (szString);
//	cout<<szString<<endl;

	sprintf(szString,"CSF_PluginDefaults=%s",szDirectory);
	putenv (szString);
//	cout<<szString<<endl;
        
        
#define TEST_ENVVAR_EXISTS(envvar,type) \
	if (!getenv(envvar)){ \
          cerr<<"Environment variable "<<envvar<<" is not set!"<<endl; \
          bFailure|=type;\
        }  
        TEST_ENVVAR_EXISTS("CSF_MdtvFontDirectory",1)
        TEST_ENVVAR_EXISTS("CSF_MdtvTexturesDirectory",1)
        TEST_ENVVAR_EXISTS("CSF_UnitsDefinition",1)
        TEST_ENVVAR_EXISTS("CSF_UnitsLexicon",1)

        if (bFailure&1) {    
         	cerr<<"Environment Error(s)"<<endl<<sEnvErrorText1;
			exit(1);
        }
        if (bFailure&2) {    
         	cerr<<"Environment Error(s)"<<endl<<sEnvErrorText2;
			exit(1);
        }

#undef TEST_ENVVAR_EXISTS         
         
/*	
	if(  getenv("CASROOT") )
	{
		RSstring CasRoot = getenv("CASROOT");
		TRACE("Environment CASROOT defined, using Cascade from %s\n",CasRoot.c_str());
		_putenv(("CSF_GRAPHICSHR="+CasRoot+"\\Windows_NT\\dll\\opengl.dll").c_str());
		_putenv(("CSF_MDTVFONTDIRECTORY="+CasRoot+"\\src\\FontMFT\\").c_str());
		_putenv(("CSF_MDTVTEXTURESDIRECTORY="+CasRoot+"\\src\\Textures\\").c_str());
		_putenv(("CSF_UNITSDEFINITION="+CasRoot+"\\src\\UnitsAPI\\Units.dat").c_str());
		_putenv(("CSF_UNITSLEXICON="+CasRoot+"\\src\\UnitsAPI\\Lexi_Expr.dat").c_str());
	}else
	{
		TRACE("Environment CASROOT NOT defined, using Cascade from intallation\n");
		_putenv("CSF_GRAPHICSHR=.\\opengl.dll");
		TRACE("CSF_GRAPHICSHR=%s\n",getenv("CSF_GRAPHICSHR"));
		_putenv("CSF_MDTVFONTDIRECTORY=.\\data\\");
		_putenv("CSF_MDTVTEXTURESDIRECTORY=.\\data\\");
		_putenv("CSF_UNITSDEFINITION=.\\data\\Units.dat");
		_putenv("CSF_UNITSLEXICON=.\\data\\Lexi_Expr.dat");
	}
*/

}

const char Usage[] = \
" [Options] files..."\
"Options:\n"\
"  -h             Display this information "\
"  -c             Runs FreeCAD in console mode (no windows)\n"\
"  -cf file-name  Runs FreeCAD in server mode with script file-name\n"\
"  -te            Runs FreeCAD to test environment\n"\
"  -t0            Runs FreeCAD self test function\n"\
"  -i             Install a new Module (e.g.ModuleName_0.1.FCModule) and the rest\n"\
"\n consult also the HTML documentation\n"\
"";


void ParsOptions(int argc, char ** argv)
{
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
						mConfig["RunMode"] = "Cmd";
						if(argc <= i+1)
						{
							GetConsole().Error("Expecting a file\n");  
							GetConsole().Error("\nUsage: %s %s",argv[0],Usage);  
						}
						mConfig["FileName"]= argv[i+1];
						i++;
						break;   
					case '\0':  
						mConfig["RunMode"] = "Cmd";
						break;   
					default:  
						GetConsole().Error("Invalid Input %s\n",argv[i]);  
						GetConsole().Error("\nUsage: %s %s",argv[0],Usage);  
						throw FCException("Comandline error(s)");  
				};  
				break;  
/*			case 't': 
			case 'T':  
				switch (argv[i][2])  
				{   
					// run the test environment script
					case 'e':  
					case 'E':  
						mConfig["RunMode"] = "Internal";
						sScriptName = FreeCADTestEnv;
						break;   
					case '0':  
						// test script level 0
						mConfig["RunMode"] = "Internal";
						sScriptName = FreeCADTest;
						break;   
					default:  
						//default testing level 0
						mConfig["RunMode"] = "Internal";
						sScriptName = FreeCADTest;
						break;   
				};  
				break;  */
/*			case 'i': 
			case 'I':  
				RunMode = 3;
				sScriptName = FreeCADInstall;
				break;  */
			case 'v': 
			case 'V':  
				switch (argv[i][2])  
				{   
					// run the test environment script
					case '1':  
						mConfig["Verbose"] = "Loose";
						sScriptName = FreeCADTestEnv;
						break;   
					case '\0':  
						// test script level 0
						mConfig["Verbose"] = "Strict";
						break;   
					default:  
						//default testing level 0
						GetConsole().Error("Invalid Verbose Option: %s\n",argv[i]); 
						GetConsole().Error("\nUsage: %s %s",argv[0],Usage); 
						throw FCException("Comandline error(s)");  
				};  
				break;  
			case '?': 
			case 'h': 
			case 'H': 
				GetConsole().Message("\nUsage: %s %s",argv[0],Usage);
				throw FCException("Comandline break");  
				break;  
			default: 
				GetConsole().Error("Invalid Option: %s\n",argv[i]); 
				GetConsole().Error("\nUsage: %s %s",argv[0],Usage); 
				throw FCException("Comandline error(s)");  
			} 
		} 
		else  
		{ 
			GetConsole().Error("Illegal command line argument #%d, %s\n",i,argv[i]); 
			GetConsole().Error("\nUsage: %s %s",argv[0],Usage); 
			throw FCException("Comandline error(s)");  
		} 
	}  
}  


void PrintInitHelp(void)
{
	cerr << endl << endl
		 << "  An initializing error was caught. This means mainly" << endl
		 << "  FreeCAD is not installed properly. Type \"FreeCAD -i\""<< endl
		 << "  to reinstall FreeCAD." << endl << endl
		 << "  Good luck ;-)" << endl << endl;
}


#ifdef FREECADMAINPY
BOOL APIENTRY DllMain( HANDLE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved){return TRUE;}

extern "C" {
#ifdef FC_DEBUG
void __declspec(dllexport) initFreeCADDCmdPy() {
#else
void __declspec(dllexport) initFreeCADCmdPy() {
#endif

	GetApplication();
	return;
}
}
#endif

