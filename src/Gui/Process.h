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



#ifndef __QT_PROCESS__H__
#define __QT_PROCESS__H__

#include "../Base/Process.h"

#if QT_VERSION > 300
# include "ProcessQt.h"
#else // QT_VERSION < 300

#ifndef FC_OS_WIN32
#if QT_VERSION < 300
#	error "Does not contain QProcess. Please use Qt 3.x.x"
#endif
#endif

class FCProcess : public QObject, public FCBaseProcess
{
  Q_OBJECT

  public:
    FCProcess( QObject *parent=0, const char *name=0 );
    FCProcess( const QString& arg0, QObject *parent=0, const char *name=0 );
    FCProcess( const QStringList& args, QObject *parent=0, const char *name=0 );
    FCProcess( const char* proc);
    virtual ~FCProcess();

    virtual bool start( QStringList *env=0 );
    virtual bool launch (const QString& buf, QStringList* env=0);
    virtual bool launch (const QByteArray& buf, QStringList* env=0);
    virtual QByteArray readStdout();
    virtual QByteArray readStderr();

  public slots:
    void tryTerminate() const;
    void kill() const;
    virtual void writeToStdin( const QString& buf );
    virtual void writeToStdin( const QByteArray& buf );
    virtual void closeStdin();

  private slots:
    void timeout();

  private:
    bool onSendData (int dummy);
    void setNotifyOnExit( bool notify );
    void setIoRedirection( bool value );
    bool notifyOnExit;
    bool ioRedirection;
    QTimer* timer;
};

#endif

#endif //__QT_PROCESS__H__
