/** \file GUIConsole.cpp
 *  \brief The console helper class (Windows only)
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include "stdio.h"
#	include "io.h"
#	include "fcntl.h"
#endif

#include "GUIConsole.h"


const UINT FCGUIConsole::s_nMaxLines = 1000;
UINT       FCGUIConsole::s_nRefCount = 0;


/** Constructor
 *  Open a Top level Window and redirect the 
 *  stdin, stdout and stderr stream to it.
 *  Dont needet in Linux!
 */
FCGUIConsole::FCGUIConsole (void)
{
  if (!s_nRefCount++)
  {
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

