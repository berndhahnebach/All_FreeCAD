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
#	include <time.h>
#	include "stdio.h"
#   ifdef FC_OS_WIN32
#	include "io.h"
#  endif
#	include "fcntl.h"
#endif

#include "Console.h"





char format[4024];  //Warning! Can't go over 512 characters!!! 


//**************************************************************************
// Construction destruction


FCConsole::FCConsole(void)
	:_bVerbose(false)
{

}

FCConsole::~FCConsole()
{
	for(std::set<FCConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        delete (*Iter);   

}


//**************************************************************************
// methodes

/**  
 *  sets the console in a special mode
 */
void FCConsole::SetMode(ConsoleMode m)
{
	if(m && Verbose)
		_bVerbose = true;
}
/**  
 *  unsets the console from a special mode
 */
void FCConsole::UnsetMode(ConsoleMode m)
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
 *  GetConsole().Message("Doing somthing important %d times\n",i);
 *  \endcode
 *  @see Warning
 *  @see Error
 *  @see Log
 */
void FCConsole::Message( const char *pMsg, ... )
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
 *  GetConsole().Warning("Some defects in %s, loading anyway\n",str);
 *  \endcode
 *  @see Message
 *  @see Error
 *  @see Log
 */
void FCConsole::Warning( const char *pMsg, ... )
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
 *  GetConsole().Error("Somthing realy bad in %s happend\n",str);
 *  \endcode
 *  @see Message
 *  @see Warning
 *  @see Log
 */
void FCConsole::Error( const char *pMsg, ... )
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
 *  GetConsole().Log("Exectue part %d in algorithem %s\n",i,str);
 *  \endcode
 *  @see Message
 *  @see Warning
 *  @see Error
 */


void FCConsole::Log( const char *pMsg, ... )
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
const char* FCConsole::Time(void)
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

/** Attach an Observer to FCConsole
 *  Use this method to attache a FCConsoleObserver derived class to 
 *  the Console. After the observer is attached all Messages will also
 *  forwardet to it.
 *  @see FCConsoleObserver
 */
void FCConsole::AttacheObserver(FCConsoleObserver *pcObserver)
{
	// double insert !!
	assert(_aclObservers.find(pcObserver) == _aclObservers.end() );

	_aclObservers.insert(pcObserver);
}

/** Detache an Observer to FCConsole
 *  Use this method to detache a FCConsoleObserver derived class.
 *  After detache you can destruct the Observer or reinsert it later.
 *  @see FCConsoleObserver
 */
void FCConsole::DetacheObserver(FCConsoleObserver *pcObserver)
{
	_aclObservers.erase(pcObserver);
}


void FCConsole::NotifyMessage(const char *sMsg)
{
	for(std::set<FCConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Message(sMsg);   // send string to the listener
}
void FCConsole::NotifyWarning(const char *sMsg)
{
	for(std::set<FCConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Warning(sMsg);   // send string to the listener
}
void FCConsole::NotifyError(const char *sMsg)
{
	for(std::set<FCConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Error(sMsg);   // send string to the listener
}
void FCConsole::NotifyLog(const char *sMsg)
{
	for(std::set<FCConsoleObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->Log(sMsg);   // send string to the listener
}


//**************************************************************************
// Singelton stuff

FCConsole * FCConsole::_pcSingelton = 0;


void FCConsole::Destruct(void)
{
	// not initialized or doubel destruct!
	assert(_pcSingelton);
	delete _pcSingelton;
}

FCConsole & FCConsole::Instance(void)
{
	// not initialized?
	if(!_pcSingelton)
	{
		_pcSingelton = new FCConsole();
		(void) Py_InitModule("FreeCAD", FCConsole::Methods);
	}
	return *_pcSingelton;
}


//**************************************************************************
// Python stuff

// FCConsole Methods						// Methods structure
PyMethodDef FCConsole::Methods[] = {
	{"PrintMessage",         (PyCFunction) FCConsole::sPyMessage,         1},
	{"PrintLog",             (PyCFunction) FCConsole::sPyLog,             1},
	{"PrintError"  ,         (PyCFunction) FCConsole::sPyError,           1},
	{"PrintWarning",         (PyCFunction) FCConsole::sPyWarning,         1},

  {NULL, NULL}		/* Sentinel */
};


PyObject *FCConsole::sPyMessage(PyObject *self,			// static python wrapper
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
PyObject *FCConsole::sPyWarning(PyObject *self,			// static python wrapper
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
PyObject *FCConsole::sPyError(PyObject *self,			// static python wrapper
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
PyObject *FCConsole::sPyLog(PyObject *self,			// static python wrapper
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

FCLoggingConsoleObserver::FCLoggingConsoleObserver(const char *sFileName)
	:cFileStream(sFileName)
{}

FCLoggingConsoleObserver::~FCLoggingConsoleObserver(){};

void FCLoggingConsoleObserver::Warning(const char *sWarn)
{
	cFileStream << sWarn;
}
void FCLoggingConsoleObserver::Message(const char *sMsg)
{
	cFileStream << sMsg;
}

void FCLoggingConsoleObserver::Error  (const char *sErr)
{
	cFileStream << sErr;
}

void FCLoggingConsoleObserver::Log    (const char *sLog)
{
	cFileStream << sLog;
}


void FCCmdConsoleObserver::Warning(const char *sWarn)
{
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN| FOREGROUND_BLUE);
#	endif
	printf("%s",sWarn);
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#	endif

}

void FCCmdConsoleObserver::Message(const char *sMsg) 
{
	printf("%s",sMsg);
}

void FCCmdConsoleObserver::Error  (const char *sErr) 
{
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_INTENSITY );
#	endif
	printf("%s",sErr);
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#	endif
}

void FCCmdConsoleObserver::Log    (const char *sErr) 
{
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED |FOREGROUND_GREEN);
#	endif
	printf("%s",sErr);
#	ifdef FC_OS_WIN32
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#	endif
}

