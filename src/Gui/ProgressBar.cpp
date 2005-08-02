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
# include <qpainter.h>
# include <qstatusbar.h>
# include <qdatetime.h>
# include <qmessagebox.h>
# include <qtimer.h>
# include <qwidgetlist.h>
#endif

#include "ProgressBar.h"
#include "Application.h"
#include "WaitCursor.h"

using namespace Gui;


namespace Gui {
struct ProgressBarPrivate
{
  QTime measureTime;
  QTimer* forceTimer;
  int minimumDuration;
  WaitCursor* cWaitCursor;
};
}

/* TRANSLATOR Gui::ProgressBar */

ProgressBar* ProgressBar::_pclSingleton = 0L;

ProgressBar* ProgressBar::instance()
{
  // not initialized?
  if ( !_pclSingleton )
  {
    _pclSingleton = new ProgressBar(ApplicationWindow::Instance->statusBar(), "Sequencer");
  }

  return _pclSingleton;
}

ProgressBar::ProgressBar ( QWidget * parent, const char * name, WFlags f )
    : QProgressBar (parent, name, f), SequencerBase()
{
  d = new Gui::ProgressBarPrivate;
  d->cWaitCursor = 0L;
  d->minimumDuration = 2000; // 2 seconds
  d->forceTimer = new QTimer(this);
  connect( d->forceTimer, SIGNAL(timeout()), this, SLOT(forceShow()) );

  setFixedWidth(120);

  // write percentage to the center
  setIndicatorFollowsStyle(false);
  setCenterIndicator(true);
  hide();
}

ProgressBar::~ProgressBar ()
{
  disconnect( d->forceTimer, SIGNAL(timeout()), this, SLOT(forceShow()) );
  delete d->forceTimer;
  delete d;
}

int ProgressBar::minimumDuration() const
{
  return d->minimumDuration;
}

void ProgressBar::setMinimumDuration ( int ms )
{
  if ( progress() == 0 )
  {
    d->forceTimer->stop();
    d->forceTimer->start( ms );
  }

  d->minimumDuration = ms;
}

void ProgressBar::forceShow()
{
  if ( !isVisible() && !wasCanceled() )
    show();
}

void ProgressBar::showEvent( QShowEvent* e )
{
  QProgressBar::showEvent( e );
  d->forceTimer->stop();
}

void ProgressBar::enterControlEvents()
{
  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );         // iterate over the widgets
  QWidget * w;
  while ( (w=it.current()) != 0 ) {  // for each widget...
    ++it;
    if (w != this)
      w->installEventFilter(this);
  }
  delete list;                      // delete the list, not the widgets

  // Make sure that we get the key events, otherwise the Inventor viewer usurps the key events
  // This also disables accelerators.
  grabKeyboard();
}

void ProgressBar::leaveControlEvents()
{
  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );         // iterate over the widgets
  QWidget * w;
  while ( (w=it.current()) != 0 ) {  // for each widget...
    ++it;
    if (w != this)
      w->removeEventFilter(this);
  }
  delete list;                      // delete the list, not the widgets

  // relase the keyboard again
  releaseKeyboard();
}

bool ProgressBar::eventFilter(QObject* o, QEvent* e)
{
  if (isRunning() && e != 0L)
  {
    switch ( e->type() )
    {
      // check for ESC
    case QEvent::KeyPress:
      {
        QKeyEvent* ke = (QKeyEvent*)e;
        if (ke->key() == Qt::Key_Escape)
        {
          // cancel the operation
          tryToCancel();
        }

        return true;
      } break;

      // ignore alle these events
    case QEvent::KeyRelease:
    case QEvent::Enter:
    case QEvent::Leave:
    case QEvent::MouseButtonDblClick:
    case QEvent::ContextMenu:
      {
        return true;
      } break;
      
      // special case if the main window's close button was pressed 
    case QEvent::Close:
      {
        // avoid to exit while app is working
        // note: all other widget types are allowed to be closed anyway
        if ( o == ApplicationWindow::Instance )
          return true; 
      } break;

      // do a system beep and ignore the event
    case QEvent::MouseButtonPress:
      {
        QApplication::beep();
        return true;
      } break;

    default:
      {
      }  break;
    }
  }

  return QProgressBar::eventFilter(o, e);
}

void ProgressBar::pause()
{
  // allow key handling of dialog and restore cursor
  releaseKeyboard();
  delete d->cWaitCursor;
  d->cWaitCursor = 0L;
}

void ProgressBar::resume()
{
  grabKeyboard(); // grab again
  //rerun observer thread
  d->cWaitCursor = new Gui::WaitCursor;
}

void ProgressBar::setProgress( int progr )
{
  // if number of total steps is unknown then incrementy only by one
  if ( nTotalSteps == 0 )
    QProgressBar::setProgress(progress()+1);
  else
    QProgressBar::setProgress(progr);
}

void ProgressBar::startStep()
{
  setTotalSteps(nTotalSteps);

  if ( pendingOperations() == 1 )
  {
    enterControlEvents();
    d->cWaitCursor = new Gui::WaitCursor;
    // delay showing the bar
    d->forceTimer->start( d->minimumDuration );

    // starting
    d->measureTime.start();
  }
}

void ProgressBar::nextStep(bool canAbort)
{
  if (wasCanceled() && canAbort)
  {
    // restore cursor
    pause();

    int ret = QMessageBox::question(ApplicationWindow::Instance,tr("Aborting"),
                tr("Do you really want to abort the operation?"),  QMessageBox::Yes, 
                QMessageBox::No|QMessageBox::Default);
    // continue and show up wait cursor if needed
    resume();

    // force to abort the operation
    if ( ret == QMessageBox::Yes )
    {
      abort();
    }
    else
    {
      rejectCancel();
      setProgress(nProgress+1);
    }
  }
  else
  {
    setProgress(nProgress+1);
  }
  
  qApp->processEvents();
}

void ProgressBar::resetData()
{
  reset();
  setTotalSteps(0);
  setProgress(-1);
  hide();
  delete d->cWaitCursor;
  d->cWaitCursor = 0L;
  d->forceTimer->stop();
  leaveControlEvents();
  ApplicationWindow::Instance->setPaneText( 1, "" );

  SequencerBase::resetData();
}

void ProgressBar::abort()
{
  //resets
  resetData();

  ConsoleMsgFlags ret = Base::Console().SetEnabledMsgType("MessageBox",ConsoleMsgType::MsgType_Wrn|
                                                                       ConsoleMsgType::MsgType_Err, false);
  Base::AbortException exc("Aborting...");
  Base::Console().SetEnabledMsgType("MessageBox",ret, true);
  throw exc;
}

void ProgressBar::setText ( const char* pszTxt )
{
  // print message to the statusbar
  QString txt = pszTxt ? pszTxt : "";
  ApplicationWindow::Instance->statusBar()->message(txt);
}

bool ProgressBar::setIndicator ( QString & indicator, int progress, int totalSteps )
{
  int elapsed = d->measureTime.elapsed();

  if ( totalSteps == 0 )
  {
    indicator = "";
    return true;
  }

  QString txt = QString::null;
  if ( progress * 20 < totalSteps && elapsed < 5000 ) 
  {
    // Less than 5 percent complete and less than 5 secs have elapsed.
    txt = tr("Measuring...");
  }
  else 
  {
    int rest = (int) ( (double) totalSteps/progress * elapsed ) - elapsed;

    // more than 1 secs have elapsed and at least 100 ms are remaining
    if ( elapsed > 1000 && rest > 100 )
    {
      QTime time( 0,0, 0);
      time = time.addSecs( rest/1000 );
      txt = tr("Remaining: %1").arg( time.toString() );
    }
  }

  if ( isVisible() )
    ApplicationWindow::Instance->setPaneText( 1, txt );

  return QProgressBar::setIndicator ( indicator, progress, totalSteps );
}

#include "moc_ProgressBar.cpp"
