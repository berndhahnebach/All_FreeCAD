/** \file Interpreter.h
 *  \brief The base::interpreter header
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here is the definition of the interpreter.
 *  @see Interpreter.cpp 
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

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#if defined (_POSIX_C_SOURCE)
#	undef  _POSIX_C_SOURCE
#endif // (re-)defined in pyconfig.h
#include <Python.h>
#include <string>
#include <vector>
// Std. configurations


namespace Base {

	using std::string;
	using std::vector;

/** The Interpreter class
 *  This class manage the python interpreter and hold a lot 
 *  helper functions for handling python stuff
 */
class BaseExport InterpreterSingleton
{
public:
	InterpreterSingleton();
	~InterpreterSingleton();

	/** @name execution methodes
	 */
	//@{
	/// Run a statement on the python interpreter
	void Launch(const char *psCmd);
	/// Run file (script) on the python interpreter
	void LaunchFile(const char*pxFileName);
	/// Run a statement with arguments on the python interpreter
	void RunFCCommand(const char * psCom,...);
	/// runs a python object methode with no return value and no arguments
	void RunMethodVoid(PyObject *pobject, const char *method);
	/// runs a python object methode which returns a arbetrary object
	PyObject* RunMethodObject(PyObject *pobject, const char *method);
	/// runs a python methode with arbitrary params
	void RunMethod(PyObject *pobject, const char *method,
                   const char *resfmt=0,   void *cresult=0,   
                   const char *argfmt="()",   ...  );
	//@}

	/** @name Module handling
	 */
	//@{
	/* Loads a module
	 */
	bool LoadModule(const char* psModName);
	//@}


//	void Register(FCPythonExport *pcPyExporter);

	/** @name startup and singletons
	 */
	//@{
	void SetComLineArgs(int argc,char *argv[]);
	int  RunCommandLine(char *prompt);
	static InterpreterSingleton &Instance(void);
	static void Destruct(void);
//	static void Init(void);
	//@}

	/** @name object, mthode and attribute query
	 *  This methodes are used for code completion facility
	 */
	//@{
	/// returns a list of methods providet by the specified object
	vector<string> GetMethodesList(const char *){return vector<string>(); }
	/// returns a list of attributes providet by the specified object
	vector<string> GetAttributeList(const char *){return vector<string>(); }
	//@}

	
	/** @name methodes for debuging facility
	 */
	//@{
	/// sets the file name which should be debuged
	void DbgObserveFile(const char* sFileName="");
	/// sets a break point to a special line number in the actual file
	void DbgSetBreakPoint(unsigned int uiLineNumber);
	/// unsets a break point to a special line number in the actual file
	void DbgUnsetBreakPoint(unsigned int uiLineNumber);
	/// One step further
	void DbgStep(void);
	//@}


	/** @name static helper functions
	 */
	//@{
  /// replaces all char with escapes for usage in python console
  static const std::string StrToPython(const char* Str);
  static const std::string StrToPython(const std::string &Str){return StrToPython(Str.c_str());}


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


} //namespace Base 

#endif
