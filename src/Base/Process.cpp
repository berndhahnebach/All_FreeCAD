/***************************************************************************
                          Process.cpp  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Process.cpp
 *  \brief 
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/



#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "Process.h"
#include "Console.h"



bool FCProcess::RunProcess(const char* psCom, ...)
{
  char format[1024];  //Warning! Can't go over 512 characters!!! 
	// va stuff
  va_list namelessVars;
  va_start(namelessVars, psCom);  // Get the "..." vars
  vsprintf(format, psCom, namelessVars);
  va_end(namelessVars);
	assert(strlen(psCom) < 800);


  bool failed;
#ifdef FC_OS_WIN32
  failed = (WinExec(format, SW_SHOW) < 32); // windows
#else
  failed = (system(format) != 0); // other OS (not windows)
#endif

  if (failed)
    GetConsole().Log("Run %s failed!\n",format);
  else
    GetConsole().Log("Run %s\n",format);

  return !failed;
}

bool FCProcess::AppendToPath (const char* path)
{
  char szOldPath[5000];
  char szNewPath[5000];

#ifdef FC_OS_WIN32
  sprintf(szOldPath, "%s;%s", getenv("Path"), path);
  ::SetEnvironmentVariable ( "Path",szOldPath);
#elif FC_OS_LINUX || FC_OS_CYGWIN
  sprintf(szOldPath, "%s:%s", getenv("Path"), path);
#elif
  GetConsole().Warning("Not yet implemented!\n");
#endif

	sprintf(szNewPath,"Path=%s",szOldPath);
	putenv (szNewPath);

  return true;
}

void FCProcess::SetEnvironment (const char* var, const char* val)
{
  char szBuf[1024];
  sprintf(szBuf, "%s=%s", var, val);
  putenv(szBuf);
}

void FCProcess::UnsetEnvironment (const char* var)
{
  char szBuf[1024];
  sprintf(szBuf, "%s=", var);
  putenv(szBuf);
}

// -------------------------------------

FCProcess* FCProcess::pclSingleton = 0L;

FCProcess& FCProcess::Instance()
{
	// not initialized!
	if(!pclSingleton)
		pclSingleton = new FCProcess();
	return *pclSingleton;
}

void FCProcess::Destruct()
{
	assert(pclSingleton);
	delete pclSingleton;
  pclSingleton = 0L;
}

FCProcess::FCProcess()
{
}

FCProcess::~FCProcess()
{
}
