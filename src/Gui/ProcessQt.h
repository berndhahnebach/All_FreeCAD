/***************************************************************************
                          ProcessQt.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ProcessQt.h
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



#ifndef __QT_PROCESS_QT__H__
#define __QT_PROCESS_QT__H__

#if QT_VERSION > 300

class FCProcess : public QProcess, public FCSubject <int>
{
  Q_OBJECT

  public:
    FCProcess(const char* proc);
    FCProcess( QObject *parent=0, const char *name=0 );
    FCProcess( const QString& arg0, QObject *parent=0, const char *name=0 );
    FCProcess( const QStringList& args, QObject *parent=0, const char *name=0 );
    virtual ~FCProcess();

    std::string message() const;

    virtual bool setExecutable(const char* proc);
    std::string executable () const;
    FCProcess& operator<<(const char * arg);
    virtual bool start( QStringList *env=0 );

    bool appendToPath (const char* path);
    void setEnvironment (const char* var, const char* val);
    void clearEnvironment ();
    void unsetEnvironment (const char* var);

  private slots:
    void OnNotifyReadyReadStdout();
    void OnNotifyReadyReadStderr();
    void OnNotifyProcessExited();
    void OnNotifyWroteToStdin();
    void OnNotifyLaunchFinished();

  private:
    void init();
    void setupEnvironment();
    std::map<std::string, std::string> env;
    std::string data;
};

#else  // QT_VERSION < 300
# error "QT Version missmatch, please set the right version in src/Config.h line 92"
#endif // QT_VERSION > 300

#endif //__QT_PROCESS_QT__H__
