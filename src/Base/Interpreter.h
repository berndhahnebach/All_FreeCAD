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

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#if defined (_POSIX_C_SOURCE)
#	undef  _POSIX_C_SOURCE
#endif // (re-)defined in pyconfig.h


#include <Python.h>

// Std. configurations
#include <string>
#include <map>

#include "Exception.h"


namespace Base {

	using std::string;
	using std::vector;



class BaseExport PyException : public Exception
{
public:
  /// constructor does the whole job
  PyException(void);
  ~PyException() throw() {}

  ///  this function returns the stack trace
  const std::string &getStackTrace(void) const {return _stackTrace;}

protected:
  std::string _stackTrace;

};


/** The Interpreter class
 *  This class manage the python interpreter and hold a lot 
 *  helper functions for handling python stuff
 */
class BaseExport InterpreterSingleton
{
public:
	InterpreterSingleton();
	~InterpreterSingleton();

	/** @name execution methods
	 */
	//@{
	/// Run a statement on the python interpreter and gives back a string with the retresentation of the result.
	std::string runString(const char *psCmd);
	/// Run a statement on the python interpreter and gives back a string with the retresentation of the result.
	void runInteractiveString(const char *psCmd);
	/// Run file (script) on the python interpreter
	void runFile(const char*pxFileName);
	/// Run a statement with arguments on the python interpreter
	void runStringArg(const char * psCom,...);
	/// runs a python object methode with no return value and no arguments
	void runMethodVoid(PyObject *pobject, const char *method);
	/// runs a python object methode which returns a arbetrary object
	PyObject* runMethodObject(PyObject *pobject, const char *method);
	/// runs a python methode with arbitrary params
	void runMethod(PyObject *pobject, const char *method,
                   const char *resfmt=0,   void *cresult=0,   
                   const char *argfmt="()",   ...  );
	//@}

	/** @name Module handling
	 */
	//@{
	/* Loads a module
	 */
	bool loadModule(const char* psModName);
	//@}

  /** @name Cleanup
   */
  //@{
  /** Register a cleanup function to be called by finalize(). The cleanup function will be called with no 
   * arguments and should return no value. At most 32 cleanup functions can be registered.When the registration 
   * is successful 0 is returned; on failure -1 is returned. The cleanup function registered last is called 
   * first. Each cleanup function will be called at most once. Since Python's internal finallization will have 
   * completed before the cleanup function, no Python APIs should be called by \a func. 
   */
  int cleanup(void (*func)(void));
  /** This calls the registered cleanup functions. @see cleanup() for more details. */
  void finalize();
  /// This shuts down the application.
  void systemExit();
  //@}

//	void Register(FCPythonExport *pcPyExporter);

	/** @name startup and singletons
	 */
	//@{
  /// init the interpreter and returns the module search path
	const char* init(int argc,char *argv[]);
	int  runCommandLine(const char *prompt);
	static InterpreterSingleton &Instance(void);
	static void Destruct(void);
//	static void Init(void);
	//@}

	/** @name std container to Python container helpers
	 */
	//@{
  /// create a dictionary object from a string map, returns a reference
  PyObject *CreateFrom(const std::map<std::string,std::string> &StringMap);

	//@}


	/** @name object, mthode and attribute query
	 *  This methods are used for code completion facility
	 */
	//@{
	/// returns a list of methods providet by the specified object
	vector<string> getmethodsList(const char *){return vector<string>(); }
	/// returns a list of attributes providet by the specified object
	vector<string> getAttributeList(const char *){return vector<string>(); }
	//@}

	
	/** @name methods for debuging facility
	 */
	//@{
	/// sets the file name which should be debuged
	void dbgObserveFile(const char* sFileName="");
	/// sets a break point to a special line number in the actual file
	void dbgSetBreakPoint(unsigned int uiLineNumber);
	/// unsets a break point to a special line number in the actual file
	void dbgUnsetBreakPoint(unsigned int uiLineNumber);
	/// One step further
	void dbgStep(void);
	//@}


	/** @name static helper functions
	 */
	//@{
  /// replaces all char with escapes for usage in python console
  static const std::string strToPython(const char* Str);
  static const std::string strToPython(const std::string &Str){return strToPython(Str.c_str());}


	//@}


protected:

	// singelton
	static InterpreterSingleton *_pcSingelton;
	friend InterpreterSingleton &GetInterpreter(void);

	std::string _cDebugFileName;
	
};


/** Access to the InterpreterSingleton object
 *  This method is used to gain access to the one and only instance of 
 *  the InterpreterSingleton class.
 */  
inline InterpreterSingleton &Interpreter(void)
{
	return InterpreterSingleton::Instance();
}

/*
class BaseExport PythonException: public std::exception
{

public:
	PythonException(const char * sErrorType,const char * sErrorInfo,const char * sErrorTraceback);
  PythonException(void);
  PythonException(const PythonException &inst);
  virtual ~PythonException() throw() {}

	PythonException &operator=(const PythonException &inst);
  
  virtual const char* what(void) const throw();

  std::string ErrMsg;
	std::string ErrorType;
	std::string ErrorInfo;
	std::string ErrorTraceback;
};
*/



} //namespace Base 

#endif
