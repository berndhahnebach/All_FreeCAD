/** \file $RCSfile$
 *  \brief The console module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
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
 



#ifndef __CONSOLE_H__
#define __CONSOLE_H__

// Std. configurations
#include "PyExport.h"

//#pragma warning(disable: 4786)  // specifier longer then 255 chars
#include <assert.h>
#include <iostream>
#include <fstream> 
#include <set>
 

/** The console observer class
 *  This class distribute the Messages issued to the FCConsole class. 
 *  If you need to catch some of the Messages you need to inherit from
 *  this class and implement some of the methodes.
 *  @see FCConsole
  */
class BaseExport FCConsoleObserver
{
public:
	/// get calles when a Warning is issued
	virtual void Warning(const char *){};
	/// get calles when a Message is issued
	virtual void Message(const char *){};
	/// get calles when a Error is issued
	virtual void Error  (const char *)=0;
	/// get calles when a Log Message is issued
	virtual void Log    (const char *){};
};


/** The console class
 *  This class manage all the stdio stuff. This includes 
 *  Messages, Warnings, Log entries and Errors. The incomming
 *  Messages are distributed with the FCConsoleObserver. The
 *  FCConsole class itself makes no IO, it's more like a manager.
 *  \par
 *  FCConsole is a singelton! That means you can access the only
 *  instance of the class from every where in c++ by simply using:
 *  \code
 *  #include <Base/Console.h>
 *  GetConsole().Log("Stage: %d",i);
 *  \endcode
 *  \par
 *  FCConsole is abel to switch between several modes to, e.g. switch
 *  the logging on or off, or treat Warnings as Errors, and so on...
 *  @see FCConsoleObserver
 */
class BaseExport FCConsole :public FCPythonExport
{

public:

	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	/// Prints a Message 
    virtual void Message ( const char * pMsg, ... ) ;
	/// Prints a warning Message 
    virtual void Warning ( const char * pMsg, ... ) ;
	/// Prints a error Message 
    virtual void Error   ( const char * pMsg, ... ) ;
	/// Prints a log Message 
    virtual void Log     ( const char * pMsg, ... ) ;

	/// Delivers a time/date string 
	const char* Time(void);
	/// Attach an Observer to FCConsole
	void AttacheObserver(FCConsoleObserver *pcObserver);
	/// Detache an Observer from FCConsole
	void DetacheObserver(FCConsoleObserver *pcObserver);
	/// enumaration for the console modes
	enum ConsoleMode{
		Verbose = 1,	// supress Log messages
		} ;

	/// Change mode
	void SetMode(ConsoleMode m);
	/// Change mode
	void UnsetMode(ConsoleMode m);
	 
protected:
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	// static python wrapper of the exported functions
	static PyObject *sPyLog    (PyObject *self,PyObject *args,PyObject *kwd);
	static PyObject *sPyMessage(PyObject *self,PyObject *args,PyObject *kwd);
	static PyObject *sPyWarning(PyObject *self,PyObject *args,PyObject *kwd);
	static PyObject *sPyError  (PyObject *self,PyObject *args,PyObject *kwd);
	static PyMethodDef    Methods[]; 

	bool _bVerbose;

private:
	// Singelton!
	FCConsole(void);
	virtual ~FCConsole();
	// singelton 
	static void Destruct(void);
	static FCConsole &Instance(void);
	static FCConsole *_pcSingelton;
	friend FCConsole &GetConsole(void); 

	// observer processing 
	void NotifyMessage(const char *sMsg);
	void NotifyWarning(const char *sMsg);
	void NotifyError  (const char *sMsg);
	void NotifyLog    (const char *sMsg);
	// observer list
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#endif
	std::set<FCConsoleObserver * > _aclObservers;

};

inline FCConsole &GetConsole(void){
	return FCConsole::Instance();
}


//=========================================================================
// some special observers

class BaseExport FCLoggingConsoleObserver : public FCConsoleObserver
{
public:
	FCLoggingConsoleObserver(const char *sFileName);
	virtual ~FCLoggingConsoleObserver();
	virtual void Warning(const char *sWarn);
	virtual void Message(const char *sMsg);
	virtual void Error  (const char *sErr);
	virtual void Log    (const char *sLog);

protected:
	std::ofstream cFileStream;
};

// simple print observer
class BaseExport FCCmdConsoleObserver: public FCConsoleObserver
{
public:
	virtual void Warning(const char *sWarn);
	virtual void Message(const char *sMsg); 
	virtual void Error  (const char *sErr); 
	virtual void Log    (const char *sErr); 
};


#endif
