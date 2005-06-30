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
#include "PyExportImp.h"

char format2[1024];  //Warning! Can't go over 512 characters!!! 

using namespace Base;



PyException::PyException(void)
{
  PP_Fetch_Error_Text();    /* fetch (and clear) exception */
  std::string temp = PP_last_error_type; /* exception name text */
  temp += ": ";
  temp += PP_last_error_info;            /* exception data text */

  _sErrMsg = temp;

  _stackTrace = PP_last_error_trace;     /* exception traceback text */
}


InterpreterSingleton::InterpreterSingleton()
{
	Py_Initialize(); 
}

InterpreterSingleton::~InterpreterSingleton()
{

}


std::string InterpreterSingleton::runString(const char *sCmd)
{
  PyBuf buf(sCmd);
  PyObject *module, *dict, *presult;          /* "exec code in d, d" */

  module = PP_Load_Module("__main__");         /* get module, init python */
  if (module == NULL) throw;                         /* not incref'd */
  dict = PyModule_GetDict(module);            /* get dict namespace */
  if (dict == NULL) throw;                           /* not incref'd */


  presult = PyRun_String(buf.str, Py_file_input, dict, dict); /* eval direct */
  if(!presult) throw Exception();

  presult = PyObject_Repr( presult) ;
  if(presult)
    return std::string(PyString_AsString(presult));
  else
    return std::string();
  
//  if(PyRun_SimpleString(buf.str)) 
//    throw PyException();
}

void InterpreterSingleton::runFile(const char*pxFileName)
{
	PyBuf FileName(pxFileName);
	
	FILE *fp = fopen(FileName.str,"r");
	if(fp == NULL) 
  {
    char szBuf[200];
    sprintf( szBuf, "File '%s' not found.", pxFileName );
		throw Exception( szBuf );
  }


	PyRun_SimpleFile(fp,FileName.str);

}

bool InterpreterSingleton::loadModule(const char* psModName)
{
	// buffer acrobatics
	PyBuf ModName(psModName);
	PyObject *module;

	module = PP_Load_Module(ModName.str);

	if(!module ) throw Exception("InterpreterSingleton::LoadModule(): Module not loaded!");

	Py_XINCREF(module);

	return true;
}

void InterpreterSingleton::runStringArg(const char * psCom,...)
{
	// va stuff
    va_list namelessVars;
    va_start(namelessVars, psCom);  // Get the "..." vars
    vsprintf(format2, psCom, namelessVars);
    va_end(namelessVars);
	assert(strlen(psCom) < 800);
	// loging
	//Console().Log("Run Com: %s\n",format2);

	runString(format2);
}


// Singelton:

InterpreterSingleton * InterpreterSingleton::_pcSingelton = 0;

InterpreterSingleton & InterpreterSingleton::Instance(void)
{
	// not initialized!
	if(!_pcSingelton)
		_pcSingelton = new InterpreterSingleton();
	return *_pcSingelton;
}

void InterpreterSingleton::Destruct(void)
{
	// not initialized or doubel destruct!
	assert(_pcSingelton);
	delete _pcSingelton;
}

void InterpreterSingleton::setComLineArgs(int argc,char *argv[])
{
	PySys_SetArgv(argc, argv);
}

int InterpreterSingleton::runCommandLine(char *prompt)
{
	return PP_Run_Command_Line(prompt);
}

/**
 *  Runs a member methode of an object with no parameter and no return value
 *  void (void). There are other methodes to run with returns
 */
void InterpreterSingleton::runMethodVoid(PyObject *pobject, const char *method)
{
	// net buffer because of char* <-> const char*
	PyBuf Methode (method);

	if(PP_Run_Method(pobject ,     // object
		             Methode.str,  // run method 
			         0,			   // no return type
				     0,		       // so no return object
					 "()")		   // no arguments
					 != 0)
		throw Exception("Error running InterpreterSingleton::RunMethodeVoid()");

}

PyObject* InterpreterSingleton::runMethodObject(PyObject *pobject, const char *method)
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
		throw Exception("Error runing InterpreterSingleton::RunMethodeObject()");
	
	return pcO;
}

void InterpreterSingleton::runMethod(PyObject *pobject, const char *method,
                              const char *resfmt,   void *cresult,        /* convert to c/c++ */
                              const char *argfmt,   ...  )                /* convert to python */
{
	PyBuf cMethod(method),cResfmt(resfmt),cArgfmt(argfmt);
    PyObject *pmeth, *pargs, *presult;
    va_list argslist;                              /* "pobject.method(args)" */
    va_start(argslist, argfmt);

    pmeth = PyObject_GetAttrString(pobject, cMethod.str);  
    if (pmeth == NULL)                             /* get callable object */
        throw Exception("Error runing InterpreterSingleton::RunMethod() methode not defined");                                 /* bound method? has self */

	pargs = Py_VaBuildValue(cArgfmt.str, argslist);     /* args: c->python */

    if (pargs == NULL) {
        Py_DECREF(pmeth);
        throw Exception("InterpreterSingleton::RunMethod() wrong arguments");
    }
    
	presult = PyEval_CallObject(pmeth, pargs);   /* run interpreter */

    Py_DECREF(pmeth);
    Py_DECREF(pargs);
	if(PP_Convert_Result(presult, cResfmt.str, cresult)!= 0)
	{
		PyErr_Print();
		throw Exception("Error runing InterpreterSingleton::RunMethod() exception in called methode");
	}
}


void InterpreterSingleton::dbgObserveFile(const char* sFileName)
{
	if(sFileName)
		_cDebugFileName = sFileName;
	else
		_cDebugFileName = "";
}

void InterpreterSingleton::dbgSetBreakPoint(unsigned int uiLineNumber)
{

}

void InterpreterSingleton::dbgUnsetBreakPoint(unsigned int uiLineNumber)
{

}

void InterpreterSingleton::dbgStep(void)
{

}


const std::string InterpreterSingleton::strToPython(const char* Str)
{
  std::string result;
  const char *It=Str;

  while(*It != '\0')
  {
    switch(*It)
    {
    case '\\':
      result += "\\\\";
      break;
    case '\"':
      result += "\\\"";
      break;
    case '\'':
      result += "\\\'";
      break;
    default:
      result += *It;
    }
    It++;
  }

  return result;
}


PyObject *InterpreterSingleton::CreateFrom(const std::map<std::string,std::string> &StringMap)
{
  PyObject *Dict = PyDict_New();
  PyObject *pKey,*pValue;

  for (std::map<std::string,std::string>::const_iterator It = StringMap.begin();It != StringMap.end();It++)
  {
    pKey   = PyString_FromString(It->first.c_str());
    pValue = PyString_FromString(It->second.c_str());
    PyDict_SetItem(Dict, pKey, pValue); 
  }

  return Dict;

}
