/***************************************************************************
                          Process.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Process.h
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



#ifndef __PROCESS__H__
#define __PROCESS__H__

#include "Observer.h"

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#	pragma warning( disable : 4275 )  
#endif

class FCBaseProcessPrivate;

class BaseExport FCBaseProcess : public FCSubject <int>, public FCObserver<int>
{
  public:
    enum Communication 
    {
      NoComm    = 0,
      Stdin     = 1, 
      Stdout    = 2, 
      Stderr    = 4,
      AllOutput = 6, 
      All       = 7,
      NoRead
    };

    enum Signals
    {
      // avoid conflicts with the Win32 error codes
      processStarted = 1024,
      processFailed  = 1025,
      processExited  = 1026,
      processKilled  = 1027,
      receivedStdout = 1028,
      receivedStderr = 1029,
      wroteStdin     = 1030,
      launchFinished = 1031
    };

    static std::string SystemWarning( int code, const char* msg=0);

    FCBaseProcess();
    FCBaseProcess(const char* proc);
    virtual ~FCBaseProcess();

    std::string message() const;
  
    virtual bool setExecutable(const char* proc);
    std::string executable () const;
    virtual FCBaseProcess& operator<< (const char * arg);
    virtual FCBaseProcess& operator<< (const std::list<std::string>&);
    std::list<std::string> arguments() const;
    void clearArguments ();

    bool launch (const std::string& buf, bool notifyOnExit);
    bool start  (bool notifyOnExit);
    virtual void tryTerminate() const;
    virtual void kill() const;
    bool isRunning() const;

    long processID();
    bool normalExit() const;
    int exitStatus() const;

    bool canReadLineStdout() const;
    bool canReadLineStderr() const;
    bool canSendData() const;
    std::string readLineStdout();
    std::string readLineStderr();
    void writeToStdin(const std::string& buf);
    void flushStdin();
    virtual void closeStdin();
    virtual void closeStdout();
    virtual void closeStderr();
    void closeAll();

    bool appendToPath (const char* path);
    void setEnvironment (const char* var, const char* val);
    void clearEnvironment ();
    void unsetEnvironment (const char* var);
    void setWorkingDirectory(const char* dir);
    const char* workingDirectory() const;

    void setCommunication( Communication c );
    Communication communication() const;

    virtual void OnChange (FCSubject<MessageType> &rCaller,MessageType rcReason);

  protected:
    virtual bool onSendData (int dummy);
    virtual void onReceiveData (bool);
    virtual void timeout ();

  private:
    bool scanNewline( bool stdOut, std::string* store );
    bool setupCommunication(Communication comm);
    void setupEnvironment();

    const std::string& childStdout();
    const std::string& childStderr();
    void reset();
    void onCloseStdin();
    FCBaseProcessPrivate* d;
};

#endif //__PROCESS__H__
