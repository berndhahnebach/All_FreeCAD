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

#include<Python.h>

// Std. configurations

class BaseExport FCInterpreter
{
public:
	FCInterpreter();
	~FCInterpreter();

	void Launch(const char *psCmd);
	void LaunchFile(const char*pxFileName);
	bool LoadModule(const char* psModName);

	void RunFCCommand(const char * psCom,...);

//	void Register(FCPythonExport *pcPyExporter);
	void SetComLineArgs(int argc,char *argv[]);
	int  RunCommandLine(char *prompt);
	/// runs a python object methode with no return value and no arguments
	void RunMethodeVoid(PyObject *pobject, const char *method);
	/// runs a python object methode which returns a arbetrary object
	PyObject* RunMethodeObject(PyObject *pobject, const char *method);
	// singelton
	static FCInterpreter &Instance(void);
	static void Destruct(void);
//	static void Init(void);

protected:

	// singelton
	static FCInterpreter *_pcSingelton;
	friend FCInterpreter &GetInterpreter(void); 


};


inline FCInterpreter &GetInterpreter(void)
{
	return FCInterpreter::Instance();
}


#endif
