/** \file MainCmd.cpp
 *  \brief The main for the command application
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

#ifdef _PreComp_
# undef _PreComp_
#endif

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


using Base::Console;
using App::Application;


#ifdef _MSC_VER
# pragma comment(lib,"TKernel.lib")
#endif


void PrintInitHelp(void);


int main( int argc, char ** argv ) 
{
	EnvPrint("Main =================================================");

	// find the home path....
	std::string sHomePath;
#	ifdef FC_OS_WIN32
		sHomePath = FindHomePathWin32(0);
#	else
		sHomePath = FindHomePathUnix(argv[0]);
#	endif



	// Init phase ===========================================================
	// sets the default run mode for FC, starts with command prompt if not overridden in InitConfig...
  Application::SetRunMode("Cmd");

	// parse the options 
	Application::InitConfig(argc,argv,sHomePath.c_str());

	Application::InitApplication();


	// Run phase ===========================================================


	Application::RunApplication();


	// Destruction phase ===========================================================
	Console().Log("FreeCAD terminating...\n\n");

	// cleans up 
	Application::Destruct();

	Console().Log("FreeCAD completely terminated\n\n");

	return 0;
}



/*
void CheckEnv(void)
{

	// set the OpenCasCade plugin variables to the FreeCAD bin path.
	SetPluginDefaults(Application::Config()["HomePath"].c_str());

	// sets all needed varables if a FreeCAD LibPack is found
	if(Application::Config()["FreeCADLib"] != "")
	{
		// sets the python environment variables if the FREECADLIB variable is defined
		SetPythonToFreeCADLib(Application::Config()["FreeCADLib"].c_str());

		// sets the OpenCasCade environment variables if the FREECADLIB variable is defined
		SetCasCadeToFreeCADLib(Application::Config()["FreeCADLib"].c_str());
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
