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
#include <FCConfig.h>

#ifdef _PreComp_
# undef _PreComp_
# include <sstream>
#endif

#ifdef FC_LINUX
#	include <unistd.h>
#endif

#if HAVE_CONFIG_H
#	include <config.h>
#endif // HAVE_CONFIG_H

#include <stdio.h>


// FreeCAD Base header
#include <Base/Exception.h>
#include <App/Application.h>

// If you stumble here, run the target "BuildExtractRevision" on Windows systems or the Python script 
// "SubWCRev.py" on Linux based systems which builds src/Build/Version.h. Or create your own from 
// src/Build/Version.h.in!
#include <Build/Version.h>


#if defined(FC_OS_WIN32)
# include <windows.h>

/** DllMain is called when DLL is loaded
 */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      {
        // This name is preliminary, we pass it to Application::init() in initFreeCAD()
        // which does the rest.
	      char  szFileName [MAX_PATH];
	      GetModuleFileName((HMODULE)hModule, szFileName, MAX_PATH-1);
        App::Application::Config()["HomePath"] = szFileName;
      } break;
    default:
      break;
    }
    
    return TRUE;
}
#elif defined(FC_OS_LINUX)
# ifndef GNU_SOURCE
#   define GNU_SOURCE
# endif
# include <dlfcn.h>
#endif

#ifdef FC_OS_WIN32
#	define MainExport __declspec(dllexport)
#else
#	define MainExport
#endif

extern "C" {
	void MainExport initFreeCAD() {

	// Init phase ===========================================================
  App::Application::Config()["ExeName"] = "FreeCAD";
  App::Application::Config()["ExeVersion"] = "0.3";

  // Version of the Application. Extractet of SubWCRef into src/Build/Version.h
  App::Application::Config()["BuildVersionMajor"]  = FCVersionMajor;
  App::Application::Config()["BuildVersionMinor"]  = FCVersionMinor;
  App::Application::Config()["BuildRevision"]      = FCRevision;
  App::Application::Config()["BuildRevisionRange"] = FCRevisionRange;
  App::Application::Config()["BuildRepositoryURL"] = FCRepositoryURL;
  App::Application::Config()["BuildRevisionDate"]  = FCRevisionDate;
  App::Application::Config()["BuildCurrentDate"]   = FCCurrentDateT;
  App::Application::Config()["BuildScrClean"]      = FCScrClean;
  App::Application::Config()["BuildFCScrMixed"]    = FCScrMixed;

  int    argc=1;
  char** argv;
  argv = (char**)malloc(sizeof(char*)* (argc+1));
  argv[0] = (char*)malloc(1024);

#if defined(FC_OS_WIN32)
  strcpy(argv[0],App::Application::Config()["HomePath"].c_str());
#elif defined(FC_OS_LINUX)
  // get whole path of the library
  Dl_info info;
  int ret = dladdr((void*)initFreeCAD, &info);
  if ((ret == 0) || (!info.dli_fname)) {
    PyErr_SetString(PyExc_ImportError, "Cannot get path of the FreeCAD module!");
    return;
  }

  strcpy(argv[0], info.dli_fname);
#else
# error "Implement: Retrieve the path of the module for your platform."
#endif
  argv[argc] = 0;

  try {
	  // Inits the Application 
	  App::Application::init(argc,argv);
  } catch (const Base::Exception& e) {
    std::string appName = App::Application::Config()["ExeName"];
    std::stringstream msg;
    msg << "While initializing " << appName << " the  following exception occurred: '" << e.what() << "'\n\n";
    msg << "\nPlease contact the application's support team for more information.\n\n";
    printf("Initialization of %s failed:\n%s", appName.c_str(), msg.str().c_str());
  }

  free(argv[0]);
  free(argv);

		return;
	} //InitFreeCAD....
} // extern "C"

