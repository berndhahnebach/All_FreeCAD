/** \file DllMain.cpp
 *  \brief initialize the dll when loaded
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Set some env variables from OCC for FreeCAD
 */

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



// === Incuding of libs: ============================================================================
#include "../Config.h"
#ifndef FC_OS_WIN32
#	error "Dont compile that file on UNIX!"
#endif

#include <windows.h>
#include <direct.h>
#include <stdio.h>

/** DllMain
 *  is called when DLL is loaded and set some variables different from OCC
 *  especialy to use the one plugin files
 */

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
//    case DLL_THREAD_ATTACH:
		// set the resource env variables
		
		char  szString [256] ;
		char  szDirectory [256] ;

		getcwd (szDirectory,sizeof szDirectory);
		if (szDirectory[strlen(szDirectory)-1] != '\\') {
			strcat(szDirectory,"\\");
		}
		
		sprintf(szString,"CSF_StandardDefaults=%s",szDirectory);
		putenv (szString);

		sprintf(szString,"CSF_PluginDefaults=%s",szDirectory);
		putenv (szString);
		
		break;

    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    //case DLL_PROCESS_DETACH:
    }
    return TRUE;
}

