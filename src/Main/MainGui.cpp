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
#include "../FCConfig.h"

#ifdef _PreComp_
# undef _PreComp_
#endif

#ifdef FC_LINUX
#	include <unistd.h>
#endif

#if HAVE_CONFIG_H
#	include <config.h>
#endif // HAVE_CONFIG_H

#include <stdio.h>

#include <qapplication.h>
#include <qmessagebox.h>


// FreeCAD header
#include "../Base/Console.h"
#include "../Base/Interpreter.h"
#include "../Base/Parameter.h"
#include "../Base/Exception.h"
#include "../Base/Factory.h"
#include "../App/Application.h"
#include "../Gui/BitmapFactory.h"
#include "../Gui/Icons/developers.h"
#include "../Gui/Application.h"

void PrintInitHelp(void);

const char sBanner[] = "(c) Juergen Riegel 2001-2006\n"\
"  #####                 ####  ###   ####  \n" \
"  #                    #      # #   #   # \n" \
"  #     ##  #### ####  #     #   #  #   # \n" \
"  ####  # # #  # #  #  #     #####  #   # \n" \
"  #     #   #### ####  #    #     # #   # \n" \
"  #     #   #    #     #    #     # #   #  ##  ##  ##\n" \
"  #     #   #### ####   ### #     # ####   ##  ##  ##\n\n" ;




int main( int argc, char ** argv )
{


  // Name and Version of the Application
  App::Application::Config()["ExeName"] = "FreeCAD";
  App::Application::Config()["ExeVersion"] = "0.3";

  // set the banner (for loging and console)
  App::Application::Config()["ConsoleBanner"] = sBanner;
  App::Application::Config()["AppIcon"] = "FCIcon";
  App::Application::Config()["SplashPicture"] = "FreeCADSplasher";
  App::Application::Config()["StartWorkbench"] = "Part design";
  //App::Application::Config()["HiddenDockWindow"] = "Property editor";

#ifndef FC_DEBUG
  try{
#endif

	// Init phase ===========================================================
	// sets the default run mode for FC, starts with gui if not overridden in InitConfig...
	App::Application::Config()["RunMode"] = "Gui";

	// Inits the Application 
	App::Application::init(argc,argv);

  Gui::BitmapFactory().addXPM("FreeCADSplasher", ( const char** ) splash_screen);
 
  Gui::Application::initApplication();

#ifndef FC_DEBUG
  } catch(const Base::Exception& e) {
    // Popup an own dialog box instead of that one of Windows
    QApplication app(argc,argv);
    QString appName = App::Application::Config()["ExeName"].c_str();
    QString msg;
    msg = QObject::tr("While initializing %1 the  following exception occurred: '%2'\n\n"
                      "Python is searching for its files in the following directories:\n%3\n\n"
                      "Python version information:\n%4\n").arg(appName).arg(e.what()).arg(Py_GetPath()).arg(Py_GetVersion());
    const char* pythonhome = getenv("PYTHONHOME");
    if ( pythonhome )
    {
      msg += QObject::tr("\nThe environment variable PYTHONHOME is set to '%1'.").arg(pythonhome);
      msg += QObject::tr("\nSetting this environment variable might cause Python to fail. Please contact your administrator to unset it on your system.\n\n");
    }
    else
    {
      msg += QObject::tr("\nPlease contact the application's support team for more information.\n\n");
    }

    QMessageBox::critical(0, QObject::tr("Initialization of %1 failed").arg(appName), msg);
    exit(100);
  } catch(...) {
    // Popup an own dialog box instead of that one of Windows
    QApplication app(argc,argv);
    QString appName = App::Application::Config()["ExeName"].c_str();
    QString msg = QObject::tr("Unknown runtime error occurred while initializing %1.\n\n"
                              "Please contact the application's support team for more information.\n\n").arg(appName);
    QMessageBox::critical(0, QObject::tr("Initialization of %1 failed").arg(appName), msg);
    exit(101);
  }
#endif

	// Run phase ===========================================================

  if(App::Application::Config()["RunMode"] == "Gui")
	{
	// run GUI
    Gui::Application::runApplication();

    Gui::Application::destruct();

	} else {

		App::Application::runApplication();
	}


	// Destruction phase ===========================================================
	Base::Console().Log("%s terminating...\n\n",App::Application::Config()["ExeName"].c_str());

	// cleans up 
	App::Application::destruct();

	Base::Console().Log("%s completely terminated\n\n",App::Application::Config()["ExeName"].c_str());

	return 0;
}

#ifdef _WIN32 
#include <windows.h> 

int __stdcall WinMain(HINSTANCE hInstance, 
                      HINSTANCE hPrevInstance, 
                      LPSTR lpCmdLine, int nShowCmd) 
{ 
  int          argc; 
  int          retVal; 
  char**       argv; 
  unsigned int i; 
  int          j; 

  // parse a few of the command line arguments 
  // a space delimites an argument except when it is inside a quote 

  argc = 1; 
  int pos = 0; 
  for (i = 0; i < strlen(lpCmdLine); i++) { 
    while (lpCmdLine[i] == ' ' && i < strlen(lpCmdLine)) i++; 
    if (lpCmdLine[i] == '\"') { 
      i++; 
      while (lpCmdLine[i] != '\"' && i < strlen(lpCmdLine)) { 
        i++; 
        pos++; 
      } 
      argc++; 
      pos = 0; 
    } else { 
      while (lpCmdLine[i] != ' ' && i < strlen(lpCmdLine)) { 
        i++; 
        pos++; 
      } 
      argc++; 
      pos = 0; 
    } 
  } 
  argv = (char**)malloc(sizeof(char*)* (argc+1)); 

  argv[0] = (char*)malloc(1024); 
  ::GetModuleFileName(0, argv[0],1024); 

  for(j=1; j<argc; j++) { 
    argv[j] = (char*)malloc(strlen(lpCmdLine)+10); 
  } 
  argv[argc] = 0; 

  argc = 1; 
  pos = 0; 
  for (i = 0; i < strlen(lpCmdLine); i++) { 
    while (lpCmdLine[i] == ' ' && i < strlen(lpCmdLine)) i++; 
    if (lpCmdLine[i] == '\"') { 
      i++; 
      while (lpCmdLine[i] != '\"' && i < strlen(lpCmdLine)) { 
        argv[argc][pos] = lpCmdLine[i]; 
        i++; 
        pos++; 
      } 
      argv[argc][pos] = '\0'; 
      argc++; 
      pos = 0; 
    }else{ 
      while (lpCmdLine[i] != ' ' && i < strlen(lpCmdLine)) { 
        argv[argc][pos] = lpCmdLine[i]; 
        i++; 
        pos++; 
      } 
      argv[argc][pos] = '\0'; 
      argc++; 
      pos = 0; 
    } 
  } 
  argv[argc] = 0; 

  // Initialize the processes and start the application. 
  retVal = main(argc, argv); 

  // Delete arguments 
  for(j=0; j<argc; j++){ 
    free(argv[j]); 
  } 
  free(argv); 

  return retVal;;
}

#endif
