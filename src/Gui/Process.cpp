/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qstring.h>
# ifdef FC_OS_WIN32
#   include <windows.h>
# endif
#endif

#include "../Base/Console.h"
#include "Process.h"

using namespace Gui;

Process::Process(QObject *parent)
: QProcess(parent)
{
  init();
}

Process::~Process()
{
}

void Process::init()
{
  connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(onNotifyReadyReadStdout()));
  connect(this, SIGNAL(readyReadStandardError()), this, SLOT(onNotifyReadyReadStderr()));
  connect(this, SIGNAL(processExited  ()), this, SLOT(onNotifyProcessExited  ()));
  connect(this, SIGNAL(wroteToStdin   ()), this, SLOT(onNotifyWroteToStdin   ()));
  connect(this, SIGNAL(launchFinished ()), this, SLOT(onNotifyLaunchFinished ()));
}

QString Process::systemWarning( int code, const char* pMsg)
{
#ifdef FC_OS_WIN32
  char szBuf[512];
  LPVOID lpMsgBuf;

  if (code < 0)
    code = GetLastError();

  if (pMsg)
  {
    snprintf(szBuf, 512, "'%s'", pMsg);
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
    return QString::null;

  // Process any inserts in lpMsgBuf.
  // ...
  // Display the string.
  QString msg = (LPCTSTR)lpMsgBuf;
  // Free the buffer.
  LocalFree( lpMsgBuf );

  return msg;
#else
  return "Error";
#endif
}
/*
bool Process::start( QStringList *e )
{
  if (e)
  {
    clearEnvironment();
    for ( QStringList::Iterator it = e->begin(); it!=e->end(); ++it )
    {
      int pos = (*it).indexOf('=');
      if (pos != -1) 
      {
        QString var = (*it).left(pos);
        QString val = (*it).right( (*it).length() - pos - 1 );
        setEnvironment(var, val);
      }
    }
  }

  if (env.size() > 0)
    setupEnvironment();

  if ( QProcess::start() )
  {
    Notify( Process::processStarted );
    return true;
  }
  else
  {
    Notify( Process::processFailed );
    return false;
  }
}
*/
QString Process::message() const
{
  return data;
}

void Process::onNotifyReadyReadStdout()
{
  QByteArray bytes = readStdout();
  if (bytes.size() > 0)
  {
    data = bytes.data();
  }

  Notify( Process::receivedStdout );
}

void Process::onNotifyReadyReadStderr()
{
  QByteArray bytes = readStderr();
  if (bytes.size() > 0)
  {
    data = bytes.data();
  }

  Notify( Process::receivedStderr );
}

void Process::onNotifyProcessExited()
{
  if (state() == QProcess::NotRunning)
  {
    if (exitStatus() == QProcess::NormalExit)
      Notify( Process::processExited );
    else
      Notify( Process::processKilled );
  }
}

void Process::onNotifyWroteToStdin()
{
  Notify( Process::wroteStdin );
}

void Process::onNotifyLaunchFinished()
{
  Notify( Process::launchFinished );
}

bool Process::appendToPath (const QString& path)
{
  char szPath[5000];
  if (env.find("PATH") != env.end())
  {
#ifdef FC_OS_WIN32
    sprintf(szPath, "%s;%s", env["PATH"].toAscii(), path.toAscii());
#elif defined (FC_OS_LINUX) || defined (FC_OS_CYGWIN)
    sprintf(szPath, "%s:%s", env["PATH"].latin1(), path.latin1());
#else
    Base::Console().Warning("Not yet implemented!\n");
#endif
    env["PATH"] = szPath;
  }
  else
  {
#ifdef FC_OS_WIN32
    sprintf(szPath, "%s;%s", getenv("PATH"), path.toAscii());
#elif defined (FC_OS_LINUX) || defined (FC_OS_CYGWIN)
    sprintf(szPath, "%s:%s", getenv("PATH"), path.latin1());
#else
    Base::Console().Warning("Not yet implemented!\n");
#endif
    env["PATH"] = szPath;
  }

  return true;
}

void Process::setEnvironment (const QString& var, const QString& val)
{
  env[var] = val;
}

void Process::clearEnvironment()
{
  env.clear();
}

void Process::unsetEnvironment (const QString& var)
{
  env[var] = "";
}

void Process::setupEnvironment()
{
  QMap<QString, QString>::Iterator it;
  for (it = env.begin(); it != env.end(); ++it)
  {
#ifdef FC_OS_WIN32
    ::SetEnvironmentVariable (it.key().toAscii(), it.value().toAscii());
#elif defined (FC_OS_LINUX) || defined (FC_OS_CYGWIN)
    setenv(it.key().latin1(), it.data().latin1(), 1);
#else
    Base::Console().Warning("Not yet implemented!\n");
#endif
  }
}

#include "moc_Process.cpp"
