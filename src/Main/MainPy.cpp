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


#ifdef FC_OS_WIN32
#	pragma comment(lib,"TKernel.lib")
#endif

#ifdef FC_OS_WIN32
#	define MainExport __declspec(dllexport)
#else
#	define MainExport
#endif


#ifdef FC_OS_WIN32

std::string path;

BOOL APIENTRY DllMain( HANDLE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{

	path = FindHomePathWin32(hModule);

	return TRUE;
}
#endif

extern "C" {
	void MainExport initFreeCAD() {

		// Init phase ===========================================================

		std::string sHomePath;

		// find home path
#		ifdef FC_OS_WIN32
			sHomePath = path;
#		else
			sHomePath = FindPyHomePathUnix("FreeCAD.so");
#		endif

		char* argv = "FreeCAD";
		// parse the options
		FCApplication::InitConfig(1,&argv,sHomePath.c_str());

		FCApplication::InitApplication();

		FCApplication::DumpConfig();


		return;
	} //InitFreeCAD....
} // extern "C"

