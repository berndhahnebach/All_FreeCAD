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

#include "PreCompiled.h"

#ifndef _PreComp_
# include <time.h>
# include <stdio.h>
#   ifdef FC_OS_WIN32
# include <io.h>
# include <windows.h>
#   endif
# include "fcntl.h"
#endif

#include "Console.h"

using namespace Base;



char format[4024];  //Warning! Can't go over 512 characters!!! 


//**************************************************************************
// Construction destruction


ConsoleSingelton::ConsoleSingelton(void)
	:_bVerbose(false)
{

}

ConsoleSingelton::~ConsoleSingelton()
{
	for(std::set<ConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        delete (*Iter);   

}


//**************************************************************************
// methodes

/**  
 *  sets the console in a special mode
 */
void ConsoleSingelton::SetMode(ConsoleMode m)
{
	if(m && Verbose)
		_bVerbose = true;
}
/**  
 *  unsets the console from a special mode
 */
void ConsoleSingelton::UnsetMode(ConsoleMode m)
{
	if(m && Verbose)
		_bVerbose = false;
}

/** Prints a Message
 *  This method issues a Message. 
 *  Messages are used show some non vital information. That means in the
 *  case FreeCAD running with GUI a Message in the status Bar apear. In console
 *  mode a message comes out. 
 *  \par
 *  You can use a printf like interface like:
 *  \code
 *  Console().Message("Doing somthing important %d times\n",i);
 *  \endcode
 *  @see Warning
 *  @see Error
 *  @see Log
 */
void ConsoleSingelton::Message( const char *pMsg, ... )
{
    va_list namelessVars;
    va_start(namelessVars, pMsg);  // Get the "..." vars
    vsprintf(format, pMsg, namelessVars);
    va_end(namelessVars);
	NotifyMessage(format);
}

/** Prints a Message
 *  This method issues a Warning. 
 *  Messages are used to get the users attantion. That means in the
 *  case FreeCAD running with GUI a Message Box is poping up. In console
 *  mode a colored message comes out! So dont use careless. For information
 *  purpose the Log or Message method is more aprobiated.
 *  \par
 *  You can use a printf like interface like:
 *  \code
 *  Console().Warning("Some defects in %s, loading anyway\n",str);
 *  \endcode
 *  @see Message
 *  @see Error
 *  @see Log
 */
void ConsoleSingelton::Warning( const char *pMsg, ... )
{
    va_list namelessVars;
    va_start(namelessVars, pMsg);  // Get the "..." vars
    vsprintf(format, pMsg, namelessVars);
    va_end(namelessVars);
	NotifyWarning(format);
}

/** Prints a Message
 *  This method issues an Error which makes some execution imposible. 
 *  Errors are used to get the users attantion. That means in the
 *  case FreeCAD running with GUI a Error Message Box is poping up. In console
 *  mode a colored message comes out! So dont use this careless. For information
 *  purpose the Log or Message method is more aprobiated.
 *  \par
 *  You can use a printf like interface like:
 *  \code
 *  Console().Error("Somthing realy bad in %s happend\n",str);
 *  \endcode
 *  @see Message
 *  @see Warning
 *  @see Log
 */
void ConsoleSingelton::Error( const char *pMsg, ... )
{
    va_list namelessVars;
    va_start(namelessVars, pMsg);  // Get the "..." vars
    vsprintf(format, pMsg, namelessVars);
    va_end(namelessVars);
	NotifyError(format);
}


/** Prints a Message
 *  this method is more for devlopment and tracking purpos.
 *  It can be used to track execution of algorithems and functions
 *  and put it in files. The normal user dont need to see it, its more
 *  for developers and experinced users. So in normal user modes the 
 *  logging is switched of.
 *  \par
 *  You can use a printf like interface like:
 *  \code
 *  Console().Log("Exectue part %d in algorithem %s\n",i,str);
 *  \endcode
 *  @see Message
 *  @see Warning
 *  @see Error
 */


void ConsoleSingelton::Log( const char *pMsg, ... )
{
	if (!_bVerbose)
	{
		va_list namelessVars;
		va_start(namelessVars, pMsg);  // Get the "..." vars
		vsprintf(format, pMsg, namelessVars);
		va_end(namelessVars);
		NotifyLog(format);
	}
}


/** Delivers the time/date
 *  this method give you a string with the actual time/date. You can
 *  use that for Log() calls to make time stamps.
 *  @return Const string with the date/time
 */
const char* ConsoleSingelton::Time(void)
{
	struct tm *newtime;
	time_t aclock;
	time( &aclock );                 // Get time in seconds 
	newtime = localtime( &aclock );  // Convert time to struct tm form 
	char* st = asctime( newtime );
	st[24] = 0;
	return st;
}



//**************************************************************************
// Observer stuff

/** Attach an Observer to Console
 *  Use this method to attache a ConsoleObserver derived class to 
 *  the Console. After the observer is attached all Messages will also
 *  forwardet to it.
 *  @see ConsoleObserver
 */
void ConsoleSingelton::AttacheObserver(ConsoleObserver *pcObserver)
{
	// double insert !!
	assert(_aclObservers.find(pcObserver) == _aclObservers.end() );

	_aclObservers.insert(pcObserver);
}

/** Detache an Observer to Console
 *  Use this method to detache a ConsoleObserver derived class.
 *  After detache you can destruct the Observer or reinsert it later.
 *  @see ConsoleObserver
 */
void ConsoleSingelton::DetacheObserver(ConsoleObserver *pcObserver)
{
	_aclObservers.erase(pcObserver);
}


void ConsoleSingelton::NotifyMessage(const char *sMsg)
{
	for(std::set<ConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Message(sMsg);   // send string to the listener
}
void ConsoleSingelton::NotifyWarning(const char *sMsg)
{
	for(std::set<ConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Warning(sMsg);   // send string to the listener
}
void ConsoleSingelton::NotifyError(const char *sMsg)
{
	for(std::set<ConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Error(sMsg);   // send string to the listener
}
void ConsoleSingelton::NotifyLog(const char *sMsg)
{
	for(std::set<ConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Log(sMsg);   // send string to the listener
}


//**************************************************************************
// Singelton stuff

ConsoleSingelton * ConsoleSingelton::_pcSingelton = 0;


void ConsoleSingelton::Destruct(void)
{
	// not initialized or doubel destruct!
	assert(_pcSingelton);
	delete _pcSingelton;
}

ConsoleSingelton & ConsoleSingelton::Instance(void)
{
	// not initialized?
	if(!_pcSingelton)
	{
		_pcSingelton = new ConsoleSingelton();
		(void) Py_InitModule("FreeCAD", ConsoleSingelton::Methods);
	}
	return *_pcSingelton;
}


//**************************************************************************
// Python stuff

// ConsoleSingelton Methods						// Methods structure
PyMethodDef ConsoleSingelton::Methods[] = {
	{"PrintMessage",         (PyCFunction) ConsoleSingelton::sPyMessage,         1},
	{"PrintLog",             (PyCFunction) ConsoleSingelton::sPyLog,             1},
	{"PrintError"  ,         (PyCFunction) ConsoleSingelton::sPyError,           1},
	{"PrintWarning",         (PyCFunction) ConsoleSingelton::sPyWarning,         1},

  {NULL, NULL}		/* Sentinel */
};


PyObject *ConsoleSingelton::sPyMessage(PyObject *self,			// static python wrapper
								PyObject *args, 
								PyObject *kwd)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	Instance().Message("%s",pstr);				 // process massage 
	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}
PyObject *ConsoleSingelton::sPyWarning(PyObject *self,			// static python wrapper
								PyObject *args, 
								PyObject *kwd)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	Instance().Warning("%s",pstr);				 // process massage 
	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}
PyObject *ConsoleSingelton::sPyError(PyObject *self,			// static python wrapper
								PyObject *args, 
								PyObject *kwd)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	Instance().Error("%s",pstr);				 // process massage 
	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}
PyObject *ConsoleSingelton::sPyLog(PyObject *self,			// static python wrapper
							PyObject *args, 
							PyObject *kwd)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	Instance().Log("%s",pstr);   				 // process massage 
	Py_INCREF(Py_None);
    return Py_None;
}

//=========================================================================
// some special observers

LoggingConsoleObserver::LoggingConsoleObserver(const char *sFileName)
	:cFileStream(sFileName)
{}

LoggingConsoleObserver::~LoggingConsoleObserver(){};

void LoggingConsoleObserver::Warning(const char *sWarn)
{
	cFileStream << sWarn;
}
void LoggingConsoleObserver::Message(const char *sMsg)
{
	cFileStream << sMsg;
}

void LoggingConsoleObserver::Error  (const char *sErr)
{
	cFileStream << sErr;
}

void LoggingConsoleObserver::Log    (const char *sLog)
{
	cFileStream << sLog;
}


void CmdConsoleObserver::Warning(const char *sWarn)
{
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN| FOREGROUND_BLUE);
#	else
	printf("\033[1;33m");
#	endif
	printf("%s",sWarn);
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#	else
	printf("\033[0m");
#	endif
}

void CmdConsoleObserver::Message(const char *sMsg)
{
	printf("%s",sMsg);
}

void CmdConsoleObserver::Error  (const char *sErr)
{
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_INTENSITY );
#	else
	printf("\033[1;31m");
#	endif
	printf("%s",sErr);
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#	else
	printf("\033[0m");
#	endif
}

void CmdConsoleObserver::Log    (const char *sErr)
{
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED |FOREGROUND_GREEN);
#	else
	printf("\033[1;36m");
#	endif
	printf("%s",sErr);
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#	else
	printf("\033[0m");
#	endif
}

