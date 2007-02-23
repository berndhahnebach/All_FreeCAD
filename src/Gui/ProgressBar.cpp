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

#include "ProgressBar.h"
#include "MainWindow.h"
#include "WaitCursor.h"


using namespace Gui;


namespace Gui {
struct ProgressBarPrivate
{
  QTime measureTime;
  QTime progressTime;
  QTime updateTime;
  QTimer* forceTimer;
  int minimumDuration;
  bool oneStep;
  WaitCursor* cWaitCursor;
  int observeEventFilter;
};
}

/* TRANSLATOR Gui::ProgressBar */

ProgressBar* ProgressBar::_pclSingleton = 0L;

ProgressBar* ProgressBar::instance()
{
  // not initialized?
  if ( !_pclSingleton )
  {
    _pclSingleton = new ProgressBar(getMainWindow()->statusBar(), "Sequencer");
  }

  return _pclSingleton;
}

ProgressBar::ProgressBar ( QWidget * parent, const char * name, Qt::WFlags f )
    : Q3ProgressBar (parent, name, f), SequencerBase()
{
  d = new Gui::ProgressBarPrivate;
  d->cWaitCursor = 0L;
  d->minimumDuration = 2000; // 2 seconds
  d->forceTimer = new QTimer(this);
  connect( d->forceTimer, SIGNAL(timeout()), this, SLOT(forceShow()) );

  setFixedWidth(120);

  // write percentage to the center
  // TODO not there anymore
  //setIndicatorFollowsStyle(false);
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
  Q3ProgressBar::showEvent( e );
  d->forceTimer->stop();
}

void ProgressBar::enterControlEvents()
{
  qApp->installEventFilter(this);

  // Make sure that we get the key events, otherwise the Inventor viewer usurps the key events
  // This also disables accelerators.
  grabKeyboard();
}

void ProgressBar::leaveControlEvents()
{
  qApp->removeEventFilter(this);

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
          // eventFilter() was called from the application 50 times without performing a new step (app could hang)
          if ( d->observeEventFilter > 50 )
          {
            // tries to unlock the application if it hangs (propably due to incorrect usage of Base::Sequencer)
            if (ke->state() & ( Qt::ControlModifier | Qt::AltModifier ) )
            {
              resetData();
              return true;
            }
          }

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
        if ( o == getMainWindow() )
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

    d->observeEventFilter++;
  }

  return Q3ProgressBar::eventFilter(o, e);
}

void ProgressBar::pause()
{
  // allow key handling of dialog and restore cursor
  leaveControlEvents();
  QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void ProgressBar::resume()
{
  enterControlEvents(); // grab again
  QApplication::restoreOverrideCursor();
}

void ProgressBar::setProgress( int progr )
{
  // if number of total steps is unknown then incrementy only by one
  if ( nTotalSteps == 0 )
  {
    if ( d->oneStep )
    {
      d->oneStep = false;
      Q3ProgressBar::setProgress(progress()+1);
    }
  }
  else
  {
    Q3ProgressBar::setProgress(progr);
  }
}

void ProgressBar::startStep()
{
  setTotalSteps(nTotalSteps);
  if ( nTotalSteps == 0 )
  {
    d->progressTime.start();
    d->updateTime.start();
  }

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

    int ret = QMessageBox::question(getMainWindow(),tr("Aborting"),
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

  if ( nTotalSteps == 0 )
  {
    int elapsed = d->progressTime.elapsed();
    if ( elapsed > 50 )
    {
      d->progressTime.restart();
      d->oneStep = true;
      elapsed = d->updateTime.elapsed();
      if ( elapsed > 2000 ) // allow an update every 2 seconds only
      {
        d->updateTime.restart();
        qApp->processEvents();
      }
    }
  }
  else
  {
    d->observeEventFilter = 0;
    qApp->processEvents();
  }
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
  getMainWindow()->setPaneText( 1, "" );

  SequencerBase::resetData();
}

void ProgressBar::abort()
{
  //resets
  resetData();
  Base::AbortException exc("Aborting...");
  throw exc;
}

void ProgressBar::setText ( const char* pszTxt )
{
  // print message to the statusbar
  QString txt = pszTxt ? pszTxt : "";
  getMainWindow()->statusBar()->message(txt);
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
    getMainWindow()->setPaneText( 1, txt );

  return Q3ProgressBar::setIndicator ( indicator, progress, totalSteps );
}

#include "moc_ProgressBar.cpp"
