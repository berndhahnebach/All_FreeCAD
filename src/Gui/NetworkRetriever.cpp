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
# include <qapplication.h>
# include <qdir.h>
# include <qfiledialog.h>
# include <qfileinfo.h>
# include <qlineedit.h>
# include <qmessagebox.h>
# include <qtimer.h>
# include <qurl.h>
#endif

#include "NetworkRetriever.h"
#include "Action.h"
#include "BitmapFactory.h"
#include "MainWindow.h"
#include "DlgAuthorization.h"

#include "../App/Application.h"
#include "../Base/Console.h"

using namespace Gui;
using Gui::Dialog::DlgAuthorization;

namespace Gui {

struct NetworkRetrieverP
{
  // wget options
  int tries;
  int level;
  QString outputFile;
  QString user;
  QString passwd;
  bool timeStamp;
  bool img;
  bool convert;
  bool recurse;
  bool folRel;
  bool html;
  // wget argument
  QString startUrl;

  QString proxy;
  QString dir;
  bool fail;
};

} // namespace Gui

/* TRANSLATOR Gui::NetworkRetriever */

NetworkRetriever::NetworkRetriever( QObject * parent, const char * name )
: QObject( parent, name )
{
  d = new NetworkRetrieverP;
  d->tries = 3;
  d->level = 1;
  d->timeStamp = false;
  d->img = false;
  d->html = false;
  d->convert = true;
  d->recurse = false;
  d->folRel = false;

  wget = new Process( this, "wget" );
  wget->Attach(this);

  // if wgets exits emit signal
  connect(wget, SIGNAL( processExited () ), this, SIGNAL( wgetExited() ));

  // if application quits kill wget immediately to avoid dangling processes
  connect( qApp, SIGNAL(lastWindowClosed()), wget, SLOT(kill()) );
}

NetworkRetriever::~NetworkRetriever()
{
  wget->Detach(this);
  delete wget;
  delete d;
}

/**
 * This method is connected to QTimer::singleShot() and executed after 5 seconds. If wget then is still running
 * we can assume that everything is fine.
 * \note This test is necessary since \a wget writes all its output on stderr and we cannot determine surely 
 * if an error occurred or not.
 *
 * \todo There is still a problem that is not solved so far. If wget requires the proxy settings and if these 
 * are not set, wget could take more than 5 seconds without downloading anything. 
 */
void NetworkRetriever::testFailure()
{
  if ( wget->isRunning() )
  {
    d->fail = false;
    Base::Console().Message( tr("Download started...\n") );
  }
}

void NetworkRetriever::OnChange (Base::Subject<Gui::Process::MessageType> &rCaller,Gui::Process::MessageType rcReason)
{
  if (&rCaller != wget)
    return;

  switch (rcReason)
  {
    // 'started' signal
    case Gui::Process::processStarted:
    {
      // set this to true and wait a while if wget is still running
      d->fail = true;
      QTimer::singleShot( 5000, this, SLOT( testFailure() ) );
    } break;

    // 'exited' signal
    case Gui::Process::processExited:
    {
      if (!d->fail)
      {
        Base::Console().Message( tr("Download finished.\n") );
        QMessageBox::information(getMainWindow(), tr("Download Online help"), tr("Download finished."));
      }
      else
      {
        QUrl url( d->startUrl );
        QString err = tr("Cannot find host '%1'.").arg( url.host() );
        Base::Console().Error( err + "\n" );
      }
    } break;

    // 'failed' signal
    case Gui::Process::processFailed:
    {
      Base::Console().Warning( tr("Download failed.\n") );
    } break;

    // 'killed' signal
    case Gui::Process::processKilled:
    {
      d->fail = true;
      Base::Console().Warning( tr("Download was canceled.\n") );
    } break;

    // 'output' signal or 'error output' signal
    case Gui::Process::receivedStdout:
    case Gui::Process::receivedStderr:
      break;
    // 'wroteStdin' signal
    case Gui::Process::wroteStdin:
      break;
    // 'launch' signal
    case Gui::Process::launchFinished:
      break;
  }
}

/**
 * Sets the number of retries to \a tries. If \a tries is 0 the number of tries
 * is unlimited. The default value of the tries property is set to 3.
 */
void NetworkRetriever::setNumberOfTries( int tries )
{
  d->tries = tries;
}

/**
 * Sets output file to \a out where documents are written to.
 */
void NetworkRetriever::setOutputFile( const QString& out )
{
  d->outputFile = out;
}

/**
 * If \a ts is true the timestamping is enabled, otherwise timestamping is
 * disabled. If timestamping is enabled files are no more re-retrieved unless
 * they are newer than the local files. As default the timestamping property is disabled.
 */
void NetworkRetriever::setEnableTimestamp(bool ts)
{
  d->timeStamp = ts;
}

/**
 * If you are behind a proxy server then you have to specify your proxy url with \a proxy.
 * Moreover, if the proxy requires user authentification then you can specify the username
 * with \a user and the password with \a passwd.
 */
void NetworkRetriever::setProxy( const QString& proxy, const QString& user, const QString& passwd )
{
  d->proxy = proxy;
  d->user = user;
  d->passwd = passwd;
}

/**
 * If \a recursive is true all referenced files are downloaded recursivly.
 * As default recursivion is disabled. \a level specifies the maximum recursion 
 * depth. If \a level is 0 the recursion depth is infinite. As default the level
 * property is 1.
 * \note: Use this with care!
 */
void NetworkRetriever::setEnableRecursive( bool recursive, int level )
{
  d->recurse = recursive;
  d->level = level;
}

/**
 * If \a folRel is true wget follows relative links only. As default
 * the follows relative property is false.
 */
void NetworkRetriever::setFollowRelative( bool folRel )
{
  d->folRel = folRel;
}

/**
 * If \a convert is true all non-relative links are converted to 
 * relative links. As default the convert property is true.
 */
void NetworkRetriever::setEnableConvert( bool convert )
{
  d->convert = convert;
}

/**
 * If \a img is true wget tries to get all needed image files
 * to display the HTML page. As default this behaviour is disabled..
 */
void NetworkRetriever::setFetchImages( bool img )
{
  d->img = img;
}

/**
 * Saves all text/html documents with .html extionsion if \a html is true.
 * As default the html property is false.
 */
void NetworkRetriever::setEnableHTMLExtension( bool html )
{
  d->html = html;
}

/**
 * Sets the output directory to \a dir where all downloaded are written into.
 */
void NetworkRetriever::setOutputDirectory( const QString& dir )
{
  d->dir = dir;
}

/**
 * wget starts to download \a startUrl and all referenced pages.
 */
bool NetworkRetriever::startDownload( const QString& startUrl )
{
  if ( !testWget() )
    return false;

  d->startUrl = startUrl;

  // proxy as environment variable
  if ( !d->proxy.isEmpty() )
  {
    wget->setEnvironment("http_proxy", d->proxy);
    wget->setEnvironment("ftp_proxy", d->proxy);
  }
  else
  {
    wget->unsetEnvironment("http_proxy");
    wget->unsetEnvironment("ftp_proxy");
  }

  wget->clearArguments();
  wget->setExecutable( "wget" );

  // since the wget option '--directory-prefix' seems not to work as expected
  // and QProcess::setWorkingDirectory() fails if the 'doc' directory doesn't
  // exist we must check for this and create it if needed.
  if ( !d->dir.isEmpty() )
  {
    QDir dir(d->dir);
    if ( dir.exists( d->dir, true ) == false )
    {
      if ( dir.mkdir( d->dir, true ) == false)
      {
        Base::Console().Error("Directory '%s' could not be created.", d->dir.latin1());
        return true; // please, no error message
      }
    }

    wget->setWorkingDirectory( dir );
  }

  // user authentification
  if ( !d->proxy.isEmpty() )
  {
    if ( !d->user.isEmpty() )
    {
      (*wget) << QString("--proxy-user=%1").arg( d->user );
      if ( !d->passwd.isEmpty() )
      {
        (*wget) << QString("--proxy-passwd=%1").arg( d->passwd );
      }
    }
  }

  // output file
  if ( !d->outputFile.isEmpty() )
    (*wget) << QString("--output-document=%1").arg( d->outputFile );
  // timestamping enabled -> update newer files only
  if ( d->timeStamp )
    (*wget) << "-N";
  // get all needed image files
  if ( d->img )
    (*wget) << "-p";
  // follow relative links only
  if ( d->folRel )
    (*wget) << "-L";
  if ( d->recurse )
  {
    (*wget) << "-r";
    (*wget) << QString("--level=%1").arg( d->level );
  }

  // convert absolute links in to relative
  if ( d->convert )
    (*wget) << "-k";
  // number of tries
  (*wget) << QString("--tries=%1").arg( d->tries );
  // use HTML file extension
  if ( d->html )
    (*wget) << "-E";

  // start URL
  (*wget)<< startUrl;

#ifdef FC_OS_LINUX
  // on Linux it seems that we have to change cwd
  QString cwd = QDir::currentDirPath ();
  if ( !d->dir.isEmpty() )
  {
    QDir::setCurrent(d->dir);
  }

  bool ok = wget->start();
  QDir::setCurrent( cwd );
#else
  bool ok = wget->start();
#endif

  return ok;
}

/**
 * Returns true if wget is still downloading, otherwise returns false.
 */
bool NetworkRetriever::isDownloading() const
{
  return wget->isRunning();
}

/**
 * Kills wget if it is still running.
 */
void NetworkRetriever::abort()
{
  if ( wget->isRunning())
  {
    wget->tryTerminate();
    QTimer::singleShot( 2000, wget, SLOT( kill() ) );
  }
}

/**
 * This is a test if wget is in PATH environment or not.
 * If the test succeeds true is returned, false otherwise.
 */
bool NetworkRetriever::testWget()
{
  QProcess proc;
  proc.addArgument("wget");
  return proc.start();
}

// --------------------------------------------------------------------

/* TRANSLATOR Gui::StdCmdOnlineHelp */

StdCmdOnlineHelp::StdCmdOnlineHelp( QObject * parent, const char * name )
  : QObject( parent, name ), Command("Std_OnlineHelp")
{
  sGroup        = QT_TR_NOOP("Help");
  sMenuText     = QT_TR_NOOP("Download online help");
  sToolTipText  = QT_TR_NOOP("Download %1's online help");
  sWhatsThis    = QT_TR_NOOP("Download %1's online help");
  sStatusTip    = QT_TR_NOOP("Download %1's online help");
  sPixmap       = "help";

  wget = new NetworkRetriever( this );
  // downloading recursively and depth 5
  wget->setEnableRecursive( true, 5 );
  wget->setNumberOfTries( 3 );
  wget->setEnableHTMLExtension( true );
  wget->setEnableConvert( true );

  wget->setEnableTimestamp( true );
  wget->setFetchImages( true );
  wget->setFollowRelative( false );

  connect( wget, SIGNAL( wgetExited() ), this, SLOT( wgetExit() ) );
}

StdCmdOnlineHelp::~StdCmdOnlineHelp()
{
  delete wget;
}

QAction * StdCmdOnlineHelp::createAction(void)
{
  QAction *pcAction;

  QString exe = App::Application::Config()["ExeName"].c_str();
  pcAction = new Action(this,getMainWindow(),sName);
  pcAction->setText      ( tr(sMenuText) );
  pcAction->setMenuText  ( tr(sMenuText) );
  pcAction->setToolTip   ( tr(sToolTipText).arg(exe) );
  pcAction->setStatusTip ( tr(sStatusTip).arg(exe)   );
  pcAction->setWhatsThis ( tr(sWhatsThis).arg(exe)   );
  pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  return pcAction;
}

void StdCmdOnlineHelp::languageChange()
{
  if ( _pcAction )
  {
    QString exe = App::Application::Config()["ExeName"].c_str();
    _pcAction->setText      ( tr(sMenuText) );
    _pcAction->setMenuText  ( tr(sMenuText) );
    _pcAction->setToolTip   ( tr(sToolTipText).arg(exe) );
    _pcAction->setStatusTip ( tr(sStatusTip).arg(exe)   );
    _pcAction->setWhatsThis ( tr(sWhatsThis).arg(exe)   );
  }
}

void StdCmdOnlineHelp::activated(int iMsg)
{
  if ( !wget->isDownloading() )
  {
    ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
    hGrp = hGrp->GetGroup("Preferences")->GetGroup("OnlineHelp");
    std::string url = hGrp->GetASCII("DownloadURL", "http://free-cad.sourceforge.net/index.html");
    std::string prx = hGrp->GetASCII("ProxyText", "");
    bool bUseProxy  = hGrp->GetBool ("UseProxy", false);
    bool bAuthor    = hGrp->GetBool ("Authorize", false);

    if (bUseProxy)
    {
      QString username = QString::null;
      QString password = QString::null;

      if ( bAuthor )
      {
        DlgAuthorization dlg( getMainWindow() );

        if ( dlg.exec() == QDialog::Accepted )
        {
          username = dlg.username->text();
          password = dlg.password->text();
        }
      }

      wget->setProxy( prx.c_str(), username, password );
    }

    int loop=3;
    bool canStart = false;

    // set output directory
    QString path = App::GetApplication().GetHomePath();
    path += "/doc/";
    ParameterGrp::handle hURLGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/OnlineHelp");
    path = QString(hURLGrp->GetASCII( "DownloadLocation", path.latin1() ).c_str());

    while ( loop > 0 )
    {
      loop--;
      QFileInfo fi( path );
      if ( !fi.exists() )
      {
        if ( QMessageBox::critical(getMainWindow(), tr("Non-existing directory"), tr("The directory '%1' does not exist.\n\n"
                                                       "Do you want to specify an existing directory?").arg( fi.filePath() ), 
             QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) != QMessageBox::Yes )
        {
          // exit the command
          return;
        }
        else 
        {
          path = QFileDialog::getExistingDirectory();
          if ( path.isEmpty() )
            return;
        }
      }

      if ( !fi.permission( QFileInfo::WriteUser ) )
      {
        if ( QMessageBox::critical(getMainWindow(), tr("Missing permission"), tr("You don't have write permission to '%1'\n\n"
                                                       "Do you want to specify another directory?").arg( fi.filePath() ), 
             QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) != QMessageBox::Yes )
        {
          // exit the command
          return;
        }
        else 
        {
          path = QFileDialog::getExistingDirectory();
          if ( path.isEmpty() )
            return;
        }
      }
      else
      {
        wget->setOutputDirectory( path );
        canStart = true;
        break;
      }
    }

    if ( canStart )
    {
      bool ok = wget->startDownload( url.c_str() );
      if ( ok == false )
        Base::Console().Error("The tool 'wget' couldn't be found. Please check your installation.");
      else if ( wget->isDownloading() && _pcAction )
        _pcAction->setMenuText(tr("Stop downloading"));
    }
  }
  else // kill the process now
  {
    wget->abort();
  }
}

void StdCmdOnlineHelp::wgetExit()
{
  if ( _pcAction )
    _pcAction->setMenuText( tr( sMenuText ) );
}

#include "moc_NetworkRetriever.cpp"

#include "DlgAuthorization.cpp"
#include "moc_DlgAuthorization.cpp"

