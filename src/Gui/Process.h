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
