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


#ifndef PROCESS__H__
#define PROCESS__H__

#ifndef _PreComp_
# include <qprocess.h>
#endif

namespace Gui {

/** 
 * The Process class is used to start external programs and to communicate with them.
 * For further information refer to the documentation of QProcess.
 * \author Werner Mayer
 */
class Process : public QProcess, public FCSubject <int>
{
  Q_OBJECT

public:
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

  /** 
   * Returns the appropriate message to the error code \a code.
   * Use this method for Windows systems only.
   */
  static QString systemWarning( int code, const char* msg=0 );

  /** Construction */
  Process( QObject *parent=0, const char *name=0 );
  /** Construction */
  Process( const QString& arg0, QObject *parent=0, const char *name=0 );
  /** Construction */
  Process( const QStringList& args, QObject *parent=0, const char *name=0 );
  /** Destruction */
  virtual ~Process();

  /** 
   * Returns the last message printed either to standard output or standard
   * error output.
   */
  QString message() const;

  /** Sets the executable name to \a proc. */
  virtual bool setExecutable( const QString& proc );
  /** Returns the current executable name. */
  QString executable () const;
  /** Adds the argument \a arg to the argument lit. */
  Process& operator<< ( const QString& arg );
  /** Starts the process. */
  virtual bool start( QStringList *env=0 );

  /** Append \a path to the PATH environenment of this process. */
  bool appendToPath ( const QString& path );
  /** Sets the environment \a var to \a val. */
  void setEnvironment ( const QString& var, const QString& val);
  /** Clears all environment variables of this process. */
  void clearEnvironment ();
  /** Unsets the environment \a var. */
  void unsetEnvironment ( const QString& var);

private slots:
  void onNotifyReadyReadStdout();
  void onNotifyReadyReadStderr();
  void onNotifyProcessExited();
  void onNotifyWroteToStdin();
  void onNotifyLaunchFinished();

private:
  void init();
  void setupEnvironment();
  QMap<QString, QString> env;
  QString data;
};

} // namespace Gui

#endif // PROCESS__H__
