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
#	include <qprocess.h>
#	include <qstring.h>
#	include <string>
#endif

#include "../Base/Console.h"
#include "../Base/Observer.h"
#include "Process.h"


#if QT_VERSION > 300

FCProcess::FCProcess(const char* proc)
: QProcess(QString(proc))
{
  init();
}

FCProcess::FCProcess( QObject *parent, const char *name)
: QProcess(parent, name)
{
  init();
}

FCProcess::FCProcess( const QString& arg0, QObject *parent, const char *name )
: QProcess(arg0, parent, name)
{
  init();
}

FCProcess::FCProcess( const QStringList& args, QObject *parent, const char *name )
: QProcess(args, parent, name)
{
  init();
}

FCProcess::~FCProcess()
{
}

void FCProcess::init()
{
  connect(this, SIGNAL(readyReadStdout()), this, SLOT(OnNotifyReadyReadStdout()));
  connect(this, SIGNAL(readyReadStderr()), this, SLOT(OnNotifyReadyReadStderr()));
  connect(this, SIGNAL(processExited  ()), this, SLOT(OnNotifyProcessExited  ()));
  connect(this, SIGNAL(wroteToStdin   ()), this, SLOT(OnNotifyWroteToStdin   ()));
  connect(this, SIGNAL(launchFinished ()), this, SLOT(OnNotifyLaunchFinished ()));
}

bool FCProcess::setExecutable(const char* proc)
{
  if (isRunning() || !proc) 
    return false;

  clearArguments();
  addArgument(proc);

  return true;
}

std::string FCProcess::executable () const
{
  if (arguments().size() > 0)
    return std::string(arguments()[0].latin1());
  else
    return "";
}

FCProcess& FCProcess::operator<<(const char * arg)
{
  addArgument(arg);
  return *this;
}

bool FCProcess::start( QStringList *e )
{
  if (e)
  {
    clearEnvironment();
    for (QStringList::Iterator it = e->begin(); it!=e->end(); ++it)
    {
      int pos = (*it).find('=');
      if (pos)
      {
        QString var = (*it).left(pos);
        QString val = (*it).right( (*it).length() - pos - 1 );
        setEnvironment(var.latin1(), val.latin1());
      }
    }
  }

  if (env.size() > 0)
    setupEnvironment();

  if (QProcess::start())
  {
    Notify(FCBaseProcess::processStarted);
    return true;
  }
  else
  {
    Notify(FCBaseProcess::processFailed);
    return false;
  }
}

std::string FCProcess::message() const
{
  return data;
}

void FCProcess::OnNotifyReadyReadStdout()
{
  QByteArray bytes = readStdout();
  if (bytes.size() > 0)
  {
    data = bytes.data();
    data.resize(bytes.size());
  }

  Notify(FCBaseProcess::receivedStdout);
}

void FCProcess::OnNotifyReadyReadStderr()
{
  QByteArray bytes = readStderr();
  if (bytes.size() > 0)
  {
    data = bytes.data();
    data.resize(bytes.size());
  }

  Notify(FCBaseProcess::receivedStderr);
}

void FCProcess::OnNotifyProcessExited()
{
  if (!isRunning())
  {
    if (normalExit())
      Notify(FCBaseProcess::processExited);
    else
      Notify(FCBaseProcess::processKilled);
  }
}

void FCProcess::OnNotifyWroteToStdin()
{
  Notify(FCBaseProcess::wroteStdin);
}

void FCProcess::OnNotifyLaunchFinished()
{
  Notify(FCBaseProcess::launchFinished);
}

bool FCProcess::appendToPath (const char* path)
{
  char szPath[5000];
  if (env.find("PATH") != env.end())
  {
#ifdef FC_OS_WIN32
    sprintf(szPath, "%s;%s", env["PATH"].c_str(), path);
#elif defined (FC_OS_LINUX) || (FC_OS_CYGWIN)
    sprintf(szPath, "%s:%s", env["PATH"].c_str(), path);
#else
    Console().Warning("Not yet implemented!\n");
#endif
    env["PATH"] = szPath;
  }
  else
  {
#ifdef FC_OS_WIN32
    sprintf(szPath, "%s;%s", getenv("PATH"), path);
#elif defined (FC_OS_LINUX) || (FC_OS_CYGWIN)
    sprintf(szPath, "%s:%s", getenv("PATH"), path);
#else
    Console().Warning("Not yet implemented!\n");
#endif
    env["PATH"] = szPath;
  }

  return true;
}

void FCProcess::setEnvironment (const char* var, const char* val)
{
  env[var] = val;
}

void FCProcess::clearEnvironment()
{
  env.clear();
}

void FCProcess::unsetEnvironment (const char* var)
{
  env[var] = "";
}

void FCProcess::setupEnvironment()
{
  std::map<std::string, std::string>::iterator it;
  for (it = env.begin(); it != env.end(); ++it)
  {
#ifdef FC_OS_WIN32
    ::SetEnvironmentVariable (it->first.c_str(), it->second.c_str());
#elif defined (FC_OS_LINUX) || (FC_OS_CYGWIN)
    setenv(it->first.c_str(), it->second.c_str(), 1);
#else
    Console().Warning("Not yet implemented!\n");
#endif
  }
}

#else // QT_VERSION < 300

FCProcess::FCProcess(QObject *parent, const char *name)
: FCBaseProcess(), notifyOnExit(false), ioRedirection(false)
{
  timer = new QTimer;
	connect( timer, SIGNAL(timeout()),this, SLOT(timeout()) );
}

FCProcess::FCProcess(const char* proc)
: FCBaseProcess(proc), notifyOnExit(false), ioRedirection(false)
{
  timer = new QTimer;
	connect( timer, SIGNAL(timeout()),this, SLOT(timeout()) );
}

FCProcess::FCProcess( const QString& arg0, QObject *parent, const char *name)
: FCBaseProcess(arg0.latin1()), notifyOnExit(false), ioRedirection(false)
{
  timer = new QTimer;
	connect( timer, SIGNAL(timeout()),this, SLOT(timeout()) );
}

FCProcess::FCProcess( const QStringList& args, QObject *parent, const char *name )
: FCBaseProcess(), notifyOnExit(false), ioRedirection(false)
{
  QStringList::ConstIterator it;
  for (it=args.begin();it!=args.end();++it)
    *this << (*it).latin1();
  timer = new QTimer;
	connect( timer, SIGNAL(timeout()),this, SLOT(timeout()) );
}

FCProcess::~FCProcess()
{
	disconnect( timer, SIGNAL(timeout()),this, SLOT(timeout()) );
  delete timer;
}

void FCProcess::tryTerminate() const
{
  FCBaseProcess::tryTerminate();
}

void FCProcess::kill() const
{
  FCBaseProcess::kill();
}

void FCProcess::writeToStdin( const QString& buf )
{
  FCBaseProcess::writeToStdin(buf.latin1());
}

void FCProcess::writeToStdin( const QByteArray& buf )
{
  std::string msg;
  msg.resize(buf.size());
  for (int i=0;i<(int)buf.size();++i)
    msg[i] = buf[i];
}

void FCProcess::closeStdin()
{
  FCBaseProcess::closeStdin();
}

void FCProcess::timeout()
{
  if ( canSendData() ) 
  	onSendData( 0 );

  if ( !canSendData() && !ioRedirection && !notifyOnExit )
	    timer->stop();

  if ( ioRedirection ) 
  {
  	onReceiveData( true );
	  onReceiveData( false );
  }

  if ( !isRunning() ) 
  {
  	timer->stop();
	  if ( notifyOnExit ) 
    {
	    Notify(processExited);
  	}

    // avoid to notify twice
    notifyOnExit = false;
  }
}

void FCProcess::setNotifyOnExit( bool notify )
{
  notifyOnExit = notify;
  if ( !ioRedirection && !notifyOnExit )
	  timer->stop();

  if ( ioRedirection ) 
  {
  	if ( isRunning() )
	    timer->start( 100 );
   }
}

void FCProcess::setIoRedirection( bool value )
{
  ioRedirection = value;
  if ( !ioRedirection && !notifyOnExit )
  	timer->stop();

  if ( notifyOnExit ) 
  {
  	if ( isRunning() )
	    timer->start( 100 );
  }
}

bool FCProcess::start( QStringList *env )
{
  setNotifyOnExit(true);
  setIoRedirection(true);

  bool success;
  if (env)
  {
    FCBaseProcess::clearEnvironment();
    for (QStringList::Iterator it = env->begin(); it!=env->end(); ++it)
    {
      int pos = (*it).find('=');
      if (pos)
      {
        QString var = (*it).left(pos);
        QString val = (*it).right( (*it).length() - pos - 1 );
        FCBaseProcess::setEnvironment(var.latin1(), val.latin1());
      }
    }

    success = FCBaseProcess::start(true);
  }
  else
    success = FCBaseProcess::start(true);


  if (success)
  {
    if ( ioRedirection || notifyOnExit )
	    timer->start( 100 );
  }

  return success;
}

bool FCProcess::launch (const QString& buf, QStringList* env)
{
  return false;
}

bool FCProcess::launch (const QByteArray& buf, QStringList* env)
{
  return false;
}

QByteArray FCProcess::readStdout()
{
  return QByteArray();
}

QByteArray FCProcess::readStderr()
{
  return QByteArray();
}

bool FCProcess::onSendData (int dummy)
{
  if (FCBaseProcess::onSendData(dummy))
  {
  	// start timer 
	  if ( canSendData() )
	    timer->start( 100 );
    
    return true;
  }

  return false; 
}

#endif

#if QT_VERSION > 300
# include "moc_ProcessQt.cpp"
#else
# include "moc_Process.cpp"
#endif
