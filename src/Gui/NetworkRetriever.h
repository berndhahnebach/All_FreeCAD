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


#ifndef TOOLS_H__
#define TOOLS_H__

#include "Process.h"
#include "Command.h"

namespace Gui {

/**
 * The NetworkRetriever class encapsulates the GNU tool \a wget.
 * wget is a non-interactive network retriever to download e.g. the whole
 * file structure from a server.
 * \author Werner Mayer
 */
class NetworkRetriever : public QObject, Process::ObserverType  
{
  Q_OBJECT

public:
	NetworkRetriever( QObject * parent = 0, const char * name = 0 );
	virtual ~NetworkRetriever();

  void setNumberOfTries( int );
  void setOutputFile( const QString& );
  void setEnableTimestamp(bool);
  void setProxy( const QString&, const QString& = QString::null, const QString& = QString::null );
  void setEnableRecursive( bool, int = 0 );
  void setFollowRelative( bool );
  void setEnableConvert( bool );
  void setFetchImages( bool );
  void setEnableHTMLExtension( bool );

  void setOutputDirectory( const QString& );
  bool startDownload( const QString& );
  bool isDownloading() const;
  void abort();

  virtual void OnChange (Base::Subject<Gui::Process::MessageType> &rCaller,Gui::Process::MessageType rcReason);

  static bool testWget();

Q_SIGNALS:
  void wgetExited();

private Q_SLOTS:
  void testFailure();

private:
  Process* wget;
  struct NetworkRetrieverP* d;
};

// --------------------------------------------------------------------

class StdCmdOnlineHelp : public QObject, public Command
{
  Q_OBJECT

public:
  StdCmdOnlineHelp( QObject * parent = 0, const char * name = 0 );
  virtual ~StdCmdOnlineHelp();
  /** i18n stuff of the command. */
  virtual void languageChange();

protected:
  virtual void activated(int iMsg);

  /** Creates the action object. */
  virtual Action* createAction();

private Q_SLOTS:
  void wgetExit();

private:
  NetworkRetriever* wget;
};

} // namespace Gui

#endif // TOOLS_H__
