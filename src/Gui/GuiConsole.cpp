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
# include "stdio.h"
# ifdef FC_OS_WIN32
#   include "io.h"
# endif
# ifdef FC_OS_WIN32
#   include <windows.h>
# endif
# include "fcntl.h"
# include <iostream>
#endif

#include "GuiConsole.h"

#ifndef FC_OS_LINUX

const unsigned int FCGUIConsole::s_nMaxLines = 1000;
unsigned int       FCGUIConsole::s_nRefCount = 0;

/** Constructor
 *  Open a Top level Window and redirect the 
 *  stdin, stdout and stderr stream to it.
 *  Dont needet in Linux!
 */
FCGUIConsole::FCGUIConsole (void)
{
  if (!s_nRefCount++)
  {
    bLog = false;

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    ::AllocConsole();

    ::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
     csbi.dwSize.Y = s_nMaxLines;
    ::SetConsoleScreenBufferSize(::GetStdHandle(STD_OUTPUT_HANDLE),csbi.dwSize);
    ::SetConsoleTitle( "FreeCAD Console");

    *stdout = *::_fdopen(::_open_osfhandle(reinterpret_cast<LONG>(::GetStdHandle(STD_OUTPUT_HANDLE)), _O_TEXT), "w");
    ::setvbuf(stdout, 0, _IONBF, 0);

    *stdin = *::_fdopen(::_open_osfhandle(reinterpret_cast<LONG>(::GetStdHandle(STD_INPUT_HANDLE)), _O_TEXT), "r");
    ::setvbuf(stdin, 0, _IONBF, 0);

    *stderr = *::_fdopen(::_open_osfhandle(reinterpret_cast<LONG>(::GetStdHandle(STD_ERROR_HANDLE)), _O_TEXT), "w");
    ::setvbuf(stderr, 0, _IONBF, 0);
  }
}

/** Destructor
 *  Close the window and redirect the streams back 
 */
FCGUIConsole::~FCGUIConsole (void)
{
  if (!--s_nRefCount)
      FreeConsole();
}


void FCGUIConsole::Message(const char *sMsg)
{
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	printf("%s",sMsg);
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
}

void FCGUIConsole::Warning(const char *sWarn)
{
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN);
	printf("%s",sWarn);
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
}
void FCGUIConsole::Error  (const char *sErr)
{
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED );
	printf("%s",sErr);
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
}

void FCGUIConsole::Log  (const char *sLog)
{
	printf("%s",sLog);
}

#else /* FC_OS_LINUX */

// safely ignore FCGUIConsole::s_nMaxLines and  FCGUIConsole::s_nRefCount
FCGUIConsole::~FCGUIConsole (void) {}
void FCGUIConsole::Message(const char *sMsg) { std::cout<<sMsg; }
void FCGUIConsole::Warning(const char *sWarn){ std::cerr<<"Warning: "<<sWarn; }
void FCGUIConsole::Error  (const char *sErr) { std::cerr<<"Error: "<<sErr;}
void FCGUIConsole::Log  (const char *sLog)   { std::clog<<sLog;}

#endif /* FC_OS_LINUX */
