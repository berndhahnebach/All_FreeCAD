/** \file MainCmd.cpp
 *  \brief The main for the command application
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) J�rgen Riegel (juergen.riegel@web.de) 2002                        *   
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

#ifdef FC_LINUX
#	include <autoconfig.h>
#	include <unistd.h>
#endif

#if HAVE_CONFIG_H
#	include <autoconfig.h>
#endif // HAVE_CONFIG_H

#include <stdio.h>


// FreeCAD Base header
#include "../Base/Console.h"
#include "../Base/Interpreter.h"
#include "../Base/Parameter.h"
#include "../Base/Exception.h"
#include "../Base/EnvMacros.h"
#include "../Base/Factory.h"

// FreeCAD doc header
#include "../App/Application.h"


#ifdef FC_OS_WIN32
#	pragma comment(lib,"TKernel.lib")
#endif

#ifdef FC_OS_WIN32
#	define MainExport __declspec(dllexport)
#else
#	define MainExport
#endif

#ifdef FC_OS_WIN32
BOOL APIENTRY DllMain( HANDLE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	return TRUE;
}
#endif

extern "C" {
	void MainExport initFreeCAD() {

		// Init phase ===========================================================

		int argc=1;
		char cwd[1024];

		if ( getcwd(cwd, sizeof(cwd) ) == 0L )
		{
				// Error
				exit(0);
		}

		// FreeCAD.py dient nur als Anhaengsel, um das richtige Home-Directory zu berechnen
		std::string tmp = cwd;
		tmp += PATHSEP;
		tmp += "FreeCAD.py";

		char* argv = new char[1024];
		strcpy(argv, tmp.c_str());
		EnvPrint(argv);


		// parse the options
		FCApplication::InitConfig(argc,&argv);

		FCApplication::InitApplication();

		FCApplication::DumpConfig();


		return;
	} //InitFreeCAD....
} // extern "C"




/*
void CheckEnv(void)
{

	// set the OpenCasCade plugin variables to the FreeCAD bin path.
	SetPluginDefaults(FCApplication::Config()["HomePath"].c_str());

	// sets all needed varables if a FreeCAD LibPack is found
	if(FCApplication::Config()["FreeCADLib"] != "")
	{
		// sets the python environment variables if the FREECADLIB variable is defined
		SetPythonToFreeCADLib(FCApplication::Config()["FreeCADLib"].c_str());

		// sets the OpenCasCade environment variables if the FREECADLIB variable is defined
		SetCasCadeToFreeCADLib(FCApplication::Config()["FreeCADLib"].c_str());
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

*/
