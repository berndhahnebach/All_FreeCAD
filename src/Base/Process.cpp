/***************************************************************************
                          Process.cpp  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Process.cpp
 *  \brief 
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/



#include "PreCompiled.h"

#ifndef _PreComp_
# include <list>
# include <map>
# include <queue>
#endif

#include "Process.h"
#include "Console.h"

class FCBaseProcessPrivate
{
  public:
    FCBaseProcessPrivate()
    {
#ifdef FC_OS_WIN32
      pInfo = 0L;
			hChildProcess = NULL;
	    hStdInputRd = NULL;
	    hStdInputWr = NULL;
	    hStdOutputRd = NULL;
	    hStdOutputWr = NULL;
	    hStdErrorRd = NULL;
	    hStdErrorWr = NULL;
      comm = FCBaseProcess::Communication(
              FCBaseProcess::Stdin  |
              FCBaseProcess::Stdout |
              FCBaseProcess::Stderr );
      stdinBufRead = 0;
      wroteStdin = false;
      closeStdin = false;
      exitValues = false;
      exitNormal = false;
      exitStatus = 0;
      exitCode = 0;
#endif
    }

    ~FCBaseProcessPrivate()
    {
      reset();
    }

    void delProcessInfo()
    {
#ifdef FC_OS_WIN32
	    delete pInfo;
	    pInfo = 0;
#endif
    }

    void newProcessInfo()
    {
#ifdef FC_OS_WIN32
	    delete pInfo;
	    pInfo = new PROCESS_INFORMATION;
#endif
    }

    void reset()
    {
#ifdef FC_OS_WIN32
	    while ( !stdinBuf.empty() )
	      stdinBuf.pop();
  
      if( hStdInputWr != NULL )
  	    CloseHandle( hStdInputWr );
	    if( hStdOutputRd != NULL )
	      CloseHandle( hStdOutputRd );
    	if( hStdErrorRd != NULL )
	      CloseHandle( hStdErrorRd );
	    stdinBufRead = 0;
	    hStdInputRd = NULL;
	    hStdInputWr = NULL;
	    hStdOutputRd = NULL;
	    hStdOutputWr = NULL;
	    hStdErrorRd = NULL;
	    hStdErrorWr = NULL;
	    exitValues = false;

	    delProcessInfo();
#endif
    }

    std::string dir;
    std::list<std::string> args;
    std::string bufferStdout;
    std::string bufferStderr;
    std::queue<std::string> stdinBuf;
    std::map<std::string, std::string> env;
    FCBaseProcess::Communication comm;

    bool wroteStdin;
    bool closeStdin;
    bool exitValues;
    bool exitNormal;
    int exitStatus;
#ifdef FC_OS_WIN32
    DWORD exitCode;
    unsigned int stdinBufRead;

    PROCESS_INFORMATION* pInfo;
    HANDLE hStdInputRd, hStdInputWr;
    HANDLE hStdErrorRd, hStdErrorWr;
    HANDLE hStdOutputRd, hStdOutputWr;
		HANDLE hChildProcess;
#endif
};

std::string FCBaseProcess::SystemWarning( int code, const char* pMsg)
{
#ifdef FC_OS_WIN32
  char szBuf[512];
  LPVOID lpMsgBuf;

  if (code < 0)
    code = GetLastError();

  if (pMsg)
  {
    sprintf(szBuf, "'%s'", pMsg);
    va_list pArguments = szBuf;
    FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM | 
      FORMAT_MESSAGE_ARGUMENT_ARRAY ,
      NULL,
      code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0,
      &pArguments);
  }
  else
  {
    FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0,
      NULL);
  }

  if (!lpMsgBuf)
    return "";

  // Process any inserts in lpMsgBuf.
  // ...
  // Display the string.
  std::string msg = (LPCTSTR)lpMsgBuf;
  // Free the buffer.
  LocalFree( lpMsgBuf );

  return msg;
#else
  return "Error";
#endif
}

FCBaseProcess::FCBaseProcess()
{
#ifndef FC_OS_WIN32
  GetConsole().Error("Not yet implemented for other OS as Windows");
  throw;
#endif
  d = new FCBaseProcessPrivate;
  Attach(this);
}

FCBaseProcess::FCBaseProcess(const char* proc)
{
#ifndef FC_OS_WIN32
  GetConsole().Error("Not yet implemented for other OS as Windows");
  throw;
#endif
  d = new FCBaseProcessPrivate;
  setExecutable(proc);
  Attach(this);
}

FCBaseProcess::~FCBaseProcess()
{
  delete d;
  d = NULL;
  Detach(this);
}

void FCBaseProcess::reset()
{
  d->reset();
  d->exitStatus = 0;
  d->exitNormal = false;
  d->bufferStdout.resize( 0 );
  d->bufferStderr.resize( 0 );
}

const std::string& FCBaseProcess::childStdout()
{
#ifdef FC_OS_WIN32
  if( d->hStdOutputRd != NULL )
  	onReceiveData( true );
#endif

  return d->bufferStdout;
}

const std::string& FCBaseProcess::childStderr()
{
#ifdef FC_OS_WIN32
  if( d->hStdErrorRd != NULL )
	  onReceiveData( false );
#endif

  return d->bufferStderr;
}

bool FCBaseProcess::appendToPath (const char* path)
{
  char szPath[5000];
  if (d->env.find("PATH") != d->env.end())
  {
#ifdef FC_OS_WIN32
    sprintf(szPath, "%s;%s", d->env["PATH"].c_str(), path);
#elif defined (FC_OS_LINUX) || (FC_OS_CYGWIN)
    sprintf(szPath, "%s:%s", d->env["PATH"].c_str(), path);
#else
  GetConsole().Warning("Not yet implemented!\n");
  return false;
#endif
    d->env["PATH"] = szPath;
  }
  else
  {
#ifdef FC_OS_WIN32
    sprintf(szPath, "%s;%s", getenv("PATH"), path);
#elif defined (FC_OS_LINUX) || (FC_OS_CYGWIN)
    sprintf(szPath, "%s:%s", getenv("PATH"), path);
#else
  GetConsole().Warning("Not yet implemented!\n");
  return false;
#endif
    d->env["PATH"] = szPath;
  }

  return true;
}

void FCBaseProcess::setEnvironment (const char* var, const char* val)
{
#ifdef ENVIRONMENT_VAR
  char szBuf[1024];
  sprintf(szBuf, "%s=%s", var, val);
  putenv(szBuf);
#else
  d->env[var] = val;
#endif
}

void FCBaseProcess::unsetEnvironment (const char* var)
{
#ifdef ENVIRONMENT_VAR
  char szBuf[1024];
  sprintf(szBuf, "%s=", var);
  putenv(szBuf);
#else
  std::map<std::string, std::string>::iterator it = d->env.find(var);
  if (it != d->env.end())
  {
    d->env[var] = "";
  }
#endif
}

void FCBaseProcess::clearEnvironment ()
{
  d->env.clear();
}

bool FCBaseProcess::setExecutable(const char* proc)
{
  if (isRunning() || !proc) 
    return false;

  d->args.clear();
  d->args.push_back(proc);

  return true;
}

std::string FCBaseProcess::executable () const
{
  if (d->args.size() > 0)
    return d->args.front();
  else
    return "";
}

FCBaseProcess& FCBaseProcess::operator<< (const char * arg)
{
  d->args.push_back(arg);
  return *this;
}

FCBaseProcess& FCBaseProcess::operator<< (const std::list<std::string>& list)
{
  d->args = list;
  return *this;
}

std::list<std::string> FCBaseProcess::arguments() const
{
  return d->args;
}

void FCBaseProcess::clearArguments ()
{
  d->args.clear();
}

bool FCBaseProcess::setupCommunication(Communication comm)
{
#ifdef FC_OS_WIN32
  // Setting the security descriptor for an object and it 
	// decides whether the object's handle will be inheritable
	// or not.
  SECURITY_ATTRIBUTES secAttr;
	secAttr.bInheritHandle = TRUE;
	secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttr.lpSecurityDescriptor = NULL;


  HANDLE hStdin, hStdout, hStderr;
  if (comm & Stdin) 
  {
  	if ( !CreatePipe( &d->hStdInputRd, &hStdin, &secAttr, 0 ) )
	    return false;
	  if ( !DuplicateHandle( GetCurrentProcess(), hStdin, GetCurrentProcess(), &d->hStdInputWr, 0, false, DUPLICATE_SAME_ACCESS ) )
	    return false;
	  if ( !CloseHandle( hStdin ) )
	    return false;
  }

  if ( comm & Stdout ) 
  {
  	if ( !CreatePipe( &hStdout, &d->hStdOutputWr, &secAttr, 0 ) )
	    return false;
	  if ( !DuplicateHandle( GetCurrentProcess(), hStdout, GetCurrentProcess(), &d->hStdOutputRd, 0, false, DUPLICATE_SAME_ACCESS ) )
	    return false;
	  if ( !CloseHandle( hStdout ) )
	    return false;
  }

  if ( comm & Stderr ) 
  {
  	if ( !CreatePipe( &hStderr, &d->hStdErrorWr, &secAttr, 0 ) )
	    return false;
	  if ( !DuplicateHandle( GetCurrentProcess(), hStderr, GetCurrentProcess(), &d->hStdErrorRd, 0, false, DUPLICATE_SAME_ACCESS ) )
	    return false;
	  if ( !CloseHandle( hStderr ) )
	    return false;
  }
#endif

  return true;
}

void FCBaseProcess::setupEnvironment()
{
  std::map<std::string, std::string>::iterator it;
  for (it = d->env.begin(); it != d->env.end(); ++it)
  {
#ifdef FC_OS_WIN32
    ::SetEnvironmentVariable (it->first.c_str(), it->second.c_str());
#elif defined (FC_OS_LINUX) || (FC_OS_CYGWIN)
    setenv(it->first.c_str(), it->second.c_str(), 1);
#else
    GetConsole().Warning("Not yet implemented!\n");
#endif
  }
}

bool FCBaseProcess::start (bool notifyOnExit)
{
  if (!d)
    return false;

  if (d->args.size() < 1)
  {
    GetConsole().Warning("No executable specified\n");
    return false;
  }

  reset();

  if (!setupCommunication(d->comm))
    return false;

#ifdef FC_OS_WIN32

  d->newProcessInfo();
	
  STARTUPINFO startInfo = 
  {
    sizeof( STARTUPINFO ), 0, 0, 0,
	  (DWORD)CW_USEDEFAULT, 
    (DWORD)CW_USEDEFAULT, 
    (DWORD)CW_USEDEFAULT, 
    (DWORD)CW_USEDEFAULT,
    0, 0, 0, STARTF_USESTDHANDLES,
    0, 0, 0,
    d->hStdInputRd, 
    d->hStdOutputWr, 
    d->hStdErrorWr
	};

  // command line arguments
  int pos=0;
  char arguments[1024];
  for (std::list<std::string>::iterator it = d->args.begin(); it != d->args.end(); ++it)
  {
    pos += sprintf(arguments+pos, "%s ", it->c_str());
  }

  pos = 0;
  if (d->env.size())
    setupEnvironment();
//    GetConsole().Error("Setting environment for new process does not yet work");
//  char szTmp[5000];
//  for (std::map<std::string, std::string>::iterator sit = d->env.begin(); sit!=d->env.end(); ++sit)
//  {
//    pos += sprintf(szTmp+pos, "%s=%s", sit->first.c_str(), sit->second.c_str());
//    szTmp[pos++] = '\0';
//    szTmp[pos++] = '\0';
//  }
//
//  char* szEnv = new char[pos+4];
//  sprintf(szEnv, szTmp);
//  szEnv[pos+0] = '\0';
//  szEnv[pos+1] = '\0';
//  szEnv[pos+2] = '\0';
//  szEnv[pos+3] = '\0';

#ifdef FC_DEBUG
//  GetConsole().Log(szEnv);
#endif

  // Creating a child process and passing a master process made
	// command line argument.
  bool bReturn = CreateProcess(0,arguments,NULL,NULL,TRUE,CREATE_NO_WINDOW|CREATE_UNICODE_ENVIRONMENT,
                               /*d->env.size()>0?szEnv:*/NULL,NULL,&startInfo,d->pInfo ) ? true:false;

//  delete szEnv;szEnv=0;

  if (bReturn) 
  {
		// Opens a handle to the child process so that it could be
		// used in the wait function (WaitForSingleObject).
		d->hChildProcess = OpenProcess(SYNCHRONIZE,FALSE,d->pInfo->dwProcessId); 

#ifdef FC_DEBUG
    GetConsole().Log("Running %s\n",arguments);
#endif

    Notify(processStarted);
	}
  else
  {
    d->delProcessInfo();
#ifdef FC_DEBUG
    GetConsole().Log("Running %s failed\n",d->args.front().c_str()); 
#endif

    Notify(processFailed);

    return false;
  }

  if ( (d->comm & Stdin) != 0 )
  	CloseHandle( d->hStdInputRd );
  if ( (d->comm & Stdout) != 0 )
    CloseHandle( d->hStdOutputWr );
  if ( (d->comm & Stderr) != 0 )
	  CloseHandle( d->hStdErrorWr );

  return true;

#else // FC_OS_WIN32
  return false;
#endif
}

bool FCBaseProcess::launch (const std::string& buf, bool notifyOnExit)
{
  if (start(notifyOnExit))
  {
    if (!buf.empty())
    {
      d->closeStdin = true;
      writeToStdin(buf);
    }
    else
    {
      closeStdin();
      Notify(launchFinished);
    }

    return true;
  }
  else
  {
    Notify(launchFinished);
    return false;
  }
}

long FCBaseProcess::processID()
{
#ifdef FC_OS_WIN32
  return long(d->pInfo->dwProcessId);
#else
  return 0;
#endif
}

const char* FCBaseProcess::workingDirectory() const
{
  return d->dir.c_str();
}

void FCBaseProcess::setWorkingDirectory(const char* dir)
{
  d->dir = dir;
}

void FCBaseProcess::setCommunication( Communication c )
{
  d->comm = c;
}

FCBaseProcess::Communication FCBaseProcess::communication() const
{
  return d->comm;
}

bool FCBaseProcess::isRunning() const
{
#ifdef FC_OS_WIN32
  if ( !d->pInfo )
  	return false;

  if ( WaitForSingleObject( d->hChildProcess, 0) == WAIT_OBJECT_0 ) 
  {
	  FCBaseProcess *that = (FCBaseProcess*)this;
	  that->onReceiveData( true ); 
	  that->onReceiveData( false );
  	if ( !d->exitValues ) 
    {
	    DWORD exitCode;
	    if ( GetExitCodeProcess( d->hChildProcess, &exitCode ) ) 
      {
  		  if ( exitCode != STILL_ACTIVE ) 
        {
		      d->exitNormal = exitCode != d->exitCode;
		      d->exitStatus = exitCode;
  		  }
	    }
	    d->exitValues = true;
  	}

  	d->delProcessInfo();
	  return false;
  } 
  else 
    return true;
#else
  return false;
#endif
}

bool FCBaseProcess::normalExit() const
{
  if ( isRunning() )
  	return false;
  else
  	return d->exitNormal;
}

int FCBaseProcess::exitStatus() const
{
  if ( isRunning() )
  	return 0;
  else
	  return d->exitStatus;
}

#ifdef FC_OS_WIN32
static BOOL CALLBACK try_terminate( HWND hWnd, LPARAM procId )
{
  DWORD tmpProcId;
  GetWindowThreadProcessId( hWnd, &tmpProcId );
  if( tmpProcId == (DWORD)procId )
  	PostMessage( hWnd, WM_CLOSE, 0, 0 );

  return TRUE;
}
#endif

void FCBaseProcess::tryTerminate() const
{
#ifdef FC_OS_WIN32
  if ( d->pInfo )
  	EnumWindows( try_terminate, (LPARAM)d->pInfo->dwProcessId );
#endif
}

void FCBaseProcess::kill() const
{
#ifdef FC_OS_WIN32
  if ( d->pInfo )
  {
  	TerminateProcess( d->pInfo->hProcess, d->exitCode );
    FCBaseProcess* that = (FCBaseProcess*)this;
    that->Notify(processKilled);
  }
#endif
}

void FCBaseProcess::flushStdin()
{
  onSendData( 0 );
}

bool FCBaseProcess::onSendData(int dummy)
{
#ifdef FC_OS_WIN32
  DWORD dwWritten;

  // if the process has terminated or if the buffer is empty
  if ( d->stdinBuf.empty() || !isRunning() )
  	return false;

  if ( !WriteFile( d->hStdInputWr, d->stdinBuf.front().c_str() + d->stdinBufRead,
		               d->stdinBuf.front().size() - d->stdinBufRead, &dwWritten, NULL ) ) 
  	return false;

  d->stdinBufRead += dwWritten;
  if ( d->stdinBufRead == d->stdinBuf.front().size() ) 
  {
  	d->stdinBufRead = 0;
	  d->stdinBuf.pop();
  	if ( wroteStdin && d->stdinBuf.empty() )
      Notify(wroteStdin);
  
    onSendData( dummy );

	if ( !d->stdinBuf.empty() )
    return true;
  }

  return false;
#endif
}

#ifdef FC_OS_WIN32
static unsigned int readStdOutput( HANDLE dev, std::string& buf, unsigned int oldSize, unsigned int bytes )
{
  if ( bytes > 0 ) 
  {
  	unsigned long r;
    char* newBuf = new char[bytes];
  	if ( ReadFile( dev, newBuf, bytes, &r, 0 ) )
    {
      for (unsigned long i=0;i<r;++i)
        buf[i+oldSize] = newBuf[i];
      delete newBuf;
	    return r;
    }

    delete newBuf;
  }

  return 0;
}
#endif

void FCBaseProcess::onReceiveData (bool stdoutput)
{
#ifdef FC_OS_WIN32
  HANDLE hStdRead;
 
  if ( stdoutput )
  	hStdRead = d->hStdOutputRd;
  else 
  	hStdRead = d->hStdErrorRd;

  // get the number of bytes to be read
  unsigned long i, r;
  char dummy;
  if ( !PeekNamedPipe( hStdRead, &dummy, 1, &r, &i, 0 ) ) 
  {
//#ifdef FC_DEBUG
//      int code = GetLastError();
//      if ( fd == 1 )
//        GetConsole().Error("Standard output: %s\n", SystemWarning(code).c_str());
//      else if ( fd == 2 )
//        GetConsole().Error("Standard error output: %s\n", SystemWarning(code).c_str());
//#endif
  	return;
  }

  if ( i > 0 ) 
  {
    std::string buffer;
  	unsigned int oldSize;
	  if ( stdoutput ) 
	    buffer = d->bufferStdout;
	  else 
	    buffer = d->bufferStderr;

  	oldSize = buffer.size();
	  buffer.resize( oldSize + i );

    // get the data now
	  unsigned int sz = readStdOutput( hStdRead, buffer, oldSize, i );
	  if ( sz != i )
	    buffer.resize( oldSize + i );

  	if ( stdoutput ) 
	    d->bufferStdout = buffer;
	  else
	    d->bufferStderr = buffer;

    if ( sz == 0 )
	    return;
	
    if ( stdoutput )
	    Notify(receivedStdout);
	  else
	    Notify(receivedStderr);
  }
#endif
}

bool FCBaseProcess::canReadLineStdout() const
{
  FCBaseProcess *that = (FCBaseProcess*)this;
  return that->scanNewline( true, 0 );
}

bool FCBaseProcess::canReadLineStderr() const
{
  FCBaseProcess *that = (FCBaseProcess*)this;
  return that->scanNewline( false, 0 );
}

bool FCBaseProcess::canSendData() const
{
  return !d->stdinBuf.empty();
}

std::string FCBaseProcess::readLineStdout()
{
  std::string a;
  std::string s;
  if ( scanNewline( true, &a ) ) 
  {
  	if ( a.empty() )
	    s = "";
	  else
	    s = std::string( a );

  }

  return s;
}

std::string FCBaseProcess::readLineStderr()
{
  std::string a;
  std::string s;
  if ( scanNewline( false, &a ) ) 
  {
  	if ( a.empty() )
	    s = "";
	  else
	    s = std::string( a );

  }

  return s;
}

bool FCBaseProcess::scanNewline( bool stdOut, std::string* store )
{
  std::string buf;
  
  if ( stdOut )
	  buf = childStdout();
  else
  	buf = childStderr();

  unsigned int n = buf.size();
  unsigned int i;

  for ( i=0; i<n; i++ ) 
  {
  	if ( buf[i] == '\n' )
	    break;
	}

  if ( i >= n )
  	return false;

  if ( store ) 
  {
  	unsigned int lineLength = i;
	  if ( lineLength>0 && buf[lineLength-1] == '\r' )
	    lineLength--; 
	  store->resize( lineLength );

	  if ( stdOut )
    {
      int consume = i+1;
      unsigned int n = d->bufferStdout.size();
      if ( consume==-1 || (unsigned int)consume >= n ) 
      {
    	  d->bufferStdout.resize( 0 );
      } 
      else 
      {
        std::string tmp; tmp.resize ( n - consume );
    	  d->bufferStdout = tmp;
      }
    }
	  else
    {
      int consume = i+1;
      unsigned int n = d->bufferStderr.size();
      if ( consume==-1 || (unsigned int)consume >= n ) 
      {
    	  d->bufferStderr.resize( 0 );
      } 
      else 
      {
        std::string tmp; tmp.resize ( n - consume );
    	  d->bufferStderr = tmp;
      }
    }
  }
  
  return true;
}

void FCBaseProcess::onCloseStdin()
{
  d->closeStdin = false;
  closeStdin();
  Notify(launchFinished);
}

void FCBaseProcess::writeToStdin(const std::string& buf)
{
  d->stdinBuf.push(buf);
  onSendData(0);
}

void FCBaseProcess::closeStdin()
{
#ifdef FC_OS_WIN32
  if ( d->hStdInputWr != 0 ) 
  {
  	CloseHandle( d->hStdInputWr );
	  d->hStdInputWr = 0;
  }
#endif
}

void FCBaseProcess::closeStdout()
{
#ifdef FC_OS_WIN32
  if ( d->hStdInputRd != 0 ) 
  {
  	CloseHandle( d->hStdOutputRd );
	  d->hStdOutputRd = 0;
  }
#endif
}

void FCBaseProcess::closeStderr()
{
#ifdef FC_OS_WIN32
  if ( d->hStdErrorRd != 0 ) 
  {
  	CloseHandle( d->hStdErrorRd );
	  d->hStdErrorRd = 0;
  }
#endif
}

void FCBaseProcess::closeAll()
{
  closeStdin();
  closeStdout();
  closeStderr();
}

std::string FCBaseProcess::message() const
{
  std::string msg;
  if (!d->bufferStderr.empty())
  {
    msg = d->bufferStderr;
    d->bufferStderr.clear();
  }

  if (!d->bufferStdout.empty())
  {
    msg = d->bufferStdout;
    d->bufferStdout.clear();
  }

  return msg;
}

void FCBaseProcess::OnChange (FCSubject<FCBaseProcess::MessageType> &rCaller,FCBaseProcess::MessageType rcReason)
{
  if (&rCaller != this)
    return;

  // observe incoming signals
  switch (rcReason)
  {
    case processStarted:
      break;
    case processFailed:
      break;
    case processExited:
      break;
    case processKilled:
      break;
    case receivedStdout:
      break;
    case receivedStderr:
      break;
    case wroteStdin:
      onCloseStdin();
      break;
    case launchFinished:
      break;
    default:
      timeout();
      break;
  }
}

void FCBaseProcess::timeout ()
{
}
