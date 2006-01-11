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


// FreeCAD Base header
#include "../Base/Console.h"
#include "../Base/Interpreter.h"
#include "../Base/Parameter.h"
#include "../Base/Exception.h"
#include "../Base/Factory.h"

// FreeCAD doc header
#include "../App/Application.h"


using App::Application;



#ifdef FC_OS_WIN32
#	pragma comment(lib,"TKernel.lib")
#endif

#ifdef FC_OS_WIN32
#	define MainExport __declspec(dllexport)
#else
#	define MainExport
#endif


/** freecadNewHandler()
 * prints an error message and throws an exception
 */
#ifdef _MSC_VER // New handler for Microsoft Visual C++ compiler
#include <new.h>
int __cdecl freecadNewHandler(size_t size )
{
  // throw an exception
  throw std::bad_alloc("Not enough memory available");
  return 0;
}
#else // Ansi C/C++ new handler
static void freecadNewHandler ()
{
  // throw an exception
  throw std::bad_alloc("Not enough memory available");
}
#endif

extern "C" {
	void MainExport initFreeCAD() {
  // install our own new handler
#ifdef _MSC_VER // Microsoft compiler
   _set_new_handler ( freecadNewHandler ); // Setup new handler
   _set_new_mode( 1 ); // Re-route malloc failures to new handler !
#else // Ansi compiler
   std::set_new_handler (freecadNewHandler); // ANSI new handler
#endif

	// Init phase ===========================================================
  App::Application::Config()["ExeName"] = "FreeCAD";

	// Inits the Application 
	App::Application::init(0,NULL);



		return;
	} //InitFreeCAD....
} // extern "C"

