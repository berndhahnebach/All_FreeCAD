/** \file Interpreter.cpp
 *  \brief The base::interpreter module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here is the implementation of the interpreter.
 *  @see Interpreter.h 
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


#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <Python.h>
#endif

#include "Console.h"
#include "Interpreter.h"
#include "PyTools.h"
#include "Exception.h"


char format2[1024];  //Warning! Can't go over 512 characters!!! 


FCInterpreter::FCInterpreter()
{
	Py_Initialize(); 
}

FCInterpreter::~FCInterpreter()
{

}


void FCInterpreter::Launch(const char *psCmd)
{
	int i = strlen(psCmd);
	char *sBuf = (char*) malloc(i+2);
	strcpy(sBuf,psCmd);

	int ret = PyRun_SimpleString(sBuf);

	free (sBuf);

	if(ret == -1) throw FCException("script fails");
}

void FCInterpreter::LaunchFile(const char*pxFileName)
{
	static char sBuf[_MAX_PATH];
	strcpy(sBuf,pxFileName);
	
	FILE *fp = fopen(sBuf,"r");
	if(fp == NULL) 
		throw FCException("File not found");
	PyRun_SimpleFile(fp,sBuf);
}

bool FCInterpreter::LoadModule(const char* psModName)
{
	// buffer acrobatics
	static char sBuf[_MAX_PATH];
	strcpy(sBuf,psModName);

	if(!PP_Load_Module(sBuf))
		return false;

	return true;
}

void FCInterpreter::RunFCCommand(const char * psCom,...)
{
    va_list namelessVars;
    va_start(namelessVars, psCom);  // Get the "..." vars
    vsprintf(format2, psCom, namelessVars);
    va_end(namelessVars);
	assert(strlen(psCom) < 800);
	GetConsole().Log("Run Com: %s\n",format2);
	Launch(format2);
}


// Singelton:

FCInterpreter * FCInterpreter::_pcSingelton = 0;

FCInterpreter & FCInterpreter::Instance(void)
{
	// not initialized!
	if(!_pcSingelton)
		_pcSingelton = new FCInterpreter();
	return *_pcSingelton;
}

void FCInterpreter::Destruct(void)
{
	// not initialized or doubel destruct!
	assert(_pcSingelton);
	delete _pcSingelton;
}

void FCInterpreter::SetComLineArgs(int argc,char *argv[])
{
	PySys_SetArgv(argc, argv);
}

int FCInterpreter::RunCommandLine(char *prompt)
{
	return PP_Run_Command_Line(prompt);
}

/*
void FCInterpreter::Register(FCPythonExport *pcPyExporter)
{
	pcPyExporter->PyRegister();
}
*/
