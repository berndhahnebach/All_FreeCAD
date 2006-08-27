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
# include <sstream>
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

  PyErr_Clear(); // must be called to keep Python interpreter in a valid state (Werner)

}


InterpreterSingleton::InterpreterSingleton()
{
	//Py_Initialize(); 
}

InterpreterSingleton::~InterpreterSingleton()
{

}


std::string InterpreterSingleton::runString(const char *sCmd)
{
  PyBuf buf(sCmd);

  PyObject *module, *dict, *presult;          /* "exec code in d, d" */

  module = PP_Load_Module("__main__");         /* get module, init python */
  if (module == NULL) 
    throw PyException();                         /* not incref'd */
  dict = PyModule_GetDict(module);            /* get dict namespace */
  if (dict == NULL) 
    throw PyException();                           /* not incref'd */


  presult = PyRun_String(buf.str, Py_file_input, dict, dict); /* eval direct */
  if(!presult)
  {
    throw PyException();
  }

  presult = PyObject_Repr( presult) ;
  if(presult)
  {
    return std::string(PyString_AsString(presult));
  }
  else
  {
    PyErr_Clear();
    return std::string();
  }
}

void InterpreterSingleton::systemExit(void)
{
  /* This code is taken from the original Python code */
  PyObject *exception, *value, *tb;
  int exitcode = 0;

  PyErr_Fetch(&exception, &value, &tb);
  if (Py_FlushLine())
    PyErr_Clear();
  fflush(stdout);
  if (value == NULL || value == Py_None)
    goto done;
  if (PyInstance_Check(value)) {
    /* The error code should be in the `code' attribute. */
    PyObject *code = PyObject_GetAttrString(value, "code");
    if (code) {
      Py_DECREF(value);
      value = code;
      if (value == Py_None)
        goto done;
    }
    /* If we failed to dig out the 'code' attribute,
       just let the else clause below print the error. */
  }
  if (PyInt_Check(value))
    exitcode = (int)PyInt_AsLong(value);
  else {
    PyObject_Print(value, stderr, Py_PRINT_RAW);
    PySys_WriteStderr("\n");
    exitcode = 1;
  }
 done:
  /* Restore and clear the exception info, in order to properly decref
   * the exception, value, and traceback.  If we just exit instead,
   * these leak, which confuses PYTHONDUMPREFS output, and may prevent
   * some finalizers from running.
   */
  PyErr_Restore(exception, value, tb);
  PyErr_Clear();
  Py_Exit(exitcode);
  /* NOTREACHED */
}

void InterpreterSingleton::runInteractiveString(const char *sCmd)
{
  PyBuf buf(sCmd);
  PyObject *module, *dict, *presult;          /* "exec code in d, d" */

  module = PP_Load_Module("__main__");         /* get module, init python */
  if (module == NULL) 
    throw PyException();                         /* not incref'd */
  dict = PyModule_GetDict(module);            /* get dict namespace */
  if (dict == NULL) 
    throw PyException();                           /* not incref'd */

  presult = PyRun_String(buf.str, Py_single_input, dict, dict); /* eval direct */
  if(!presult)
  {
	  if (PyErr_ExceptionMatches(PyExc_SystemExit)) {
		  systemExit();
	  }
    /* get latest python exception information */
    /* and print the error to the error output */
    PyObject *errobj, *errdata, *errtraceback;
    PyErr_Fetch(&errobj, &errdata, &errtraceback);

    Exception exc; // do not use PyException since this clears the error indicator
    if ( PyString_Check( errdata ) )
      exc.SetMessage( PyString_AsString( errdata ) );
    PyErr_Restore(errobj, errdata, errtraceback);
    if ( PyErr_Occurred() )
      PyErr_Print();
    throw exc;
  }
}

void InterpreterSingleton::runFile(const char*pxFileName)
{
  std::ifstream file;
  file.open(pxFileName);
  if(file){
    std::stringbuf buf;
    file >> &buf;
    file.close();
    
    runString(buf.str().c_str());
  }else
    throw Exception("Unknown file!");
}

bool InterpreterSingleton::loadModule(const char* psModName)
{
	// buffer acrobatics
	PyBuf ModName(psModName);
	PyObject *module;

	module = PP_Load_Module(ModName.str);

	if(!module ) 
    throw PyException(/*"InterpreterSingleton::LoadModule(): Module not loaded!"*/);

	Py_XINCREF(module);

	return true;
}

int InterpreterSingleton::cleanup(void (*func)(void))
{
  return Py_AtExit( func );
}

void InterpreterSingleton::finalize()
{
  try{
    Py_Finalize();
  }catch(...){}
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

const char* InterpreterSingleton::init(int argc,char *argv[])
{
  Py_SetProgramName(argv[0]);
  Py_Initialize();
	PySys_SetArgv(argc, argv);
  return Py_GetPath();
}

int InterpreterSingleton::runCommandLine(const char *prompt)
{
  PyBuf buf(prompt);

	return PP_Run_Command_Line(buf.str);
}

/**
 *  Runs a member methode of an object with no parameter and no return value
 *  void (void). There are other methods to run with returns
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
		throw PyException(/*"Error running InterpreterSingleton::RunMethodeVoid()"*/);

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
		throw PyException(/*"Error runing InterpreterSingleton::RunMethodeObject()"*/);
	
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
    if ( PyErr_Occurred() )
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

/*

PythonException::PythonException(void)
{
  ErrMsg = "Unknown Python error";
}

PythonException::PythonException(const PythonException &inst)
{
  *this = inst;

}


PythonException::PythonException(const char * sErrorType,const char * sErrorInfo,const char * sErrorTraceback)
: ErrorType(sErrorType),ErrorInfo(sErrorInfo),ErrorTraceback(sErrorTraceback)
{
  ErrMsg = ErrorType + ErrorInfo + ErrorTraceback;
}


PythonException &PythonException::operator=(const PythonException &inst)
{
  ErrMsg         = inst.ErrMsg;
  ErrorType      = inst.ErrorType;
  ErrorInfo      = inst.ErrorInfo;
  ErrorTraceback = inst.ErrorTraceback;
  return *this;
}

const char* PythonException::what(void) const throw()
{
	return ErrMsg.c_str();
}
*/

