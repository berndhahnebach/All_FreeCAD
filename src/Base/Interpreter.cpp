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


#include "PreCompiled.h"

#ifndef _PreComp_
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
	PyBuf Cmd(psCmd);

	int ret = PyRun_SimpleString(Cmd.str);

	if(ret == -1) throw FCException("script failed");
}

void FCInterpreter::LaunchFile(const char*pxFileName)
{
	PyBuf FileName(pxFileName);
	
	FILE *fp = fopen(FileName.str,"r");
	if(fp == NULL) 
		throw FCException("File not found");

	PyRun_SimpleFile(fp,FileName.str);

}

bool FCInterpreter::LoadModule(const char* psModName)
{
	// buffer acrobatics
	PyBuf ModName(psModName);
	PyObject *module;

	module = PP_Load_Module(ModName.str);

	if(!module ) throw FCException("FCInterpreter::LoadModule(): Module not loaded!");

	Py_XINCREF(module);

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

/**
 *  Runs a member methode of an object with no parameter and no return value
 *  void (void). There are other methodes to run with returns
 */
void FCInterpreter::RunMethodVoid(PyObject *pobject, const char *method)
{
	// net buffer because of char* <-> const char*
	PyBuf Methode (method);

	if(PP_Run_Method(pobject ,     // object
		             Methode.str,  // run method 
			         0,			   // no return type
				     0,		       // so no return object
					 "()")		   // no arguments
					 != 0)
		throw FCException("Error runing FCInterpreter::RunMethodeVoid()");

}

PyObject* FCInterpreter::RunMethodObject(PyObject *pobject, const char *method)
{
	// net buffer because of char* <-> const char*
	PyBuf Methode (method);

	PyObject *pcO;

	if(PP_Run_Method(pobject ,     // object
		             Methode.str,  // run method 
			         "O",		   // return type
				     &pcO,		   // return object
					 "()")		   // no arguments
					 != 0)
		throw FCException("Error runing FCInterpreter::RunMethodeObject()");
	
	return pcO;
}

void FCInterpreter::RunMethod(PyObject *pobject, const char *method,
                              const char *resfmt,   void *cresult,        /* convert to c/c++ */
                              const char *argfmt,   ...  )                /* convert to python */
{
	PyBuf cMethod(method),cResfmt(resfmt),cArgfmt(argfmt);
    PyObject *pmeth, *pargs, *presult;
    va_list argslist;                              /* "pobject.method(args)" */
    va_start(argslist, argfmt);

    pmeth = PyObject_GetAttrString(pobject, cMethod.str);  
    if (pmeth == NULL)                             /* get callable object */
        throw FCException("Error runing FCInterpreter::RunMethod() methode not defined");                                 /* bound method? has self */

	pargs = Py_VaBuildValue(cArgfmt.str, argslist);     /* args: c->python */

    if (pargs == NULL) {
        Py_DECREF(pmeth);
        throw FCException("FCInterpreter::RunMethod() wrong arguments");
    }
    
	presult = PyEval_CallObject(pmeth, pargs);   /* run interpreter */

    Py_DECREF(pmeth);
    Py_DECREF(pargs);
	if(PP_Convert_Result(presult, cResfmt.str, cresult)!= 0)
	{
		PyErr_Print();
		throw FCException("Error runing FCInterpreter::RunMethod() exception in called methode");
	}
}
