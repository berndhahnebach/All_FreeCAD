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
# include <qwidgetlist.h>
#endif

#include "ProgressBar.h"
#include "Application.h"
#include "WaitCursor.h"
#include "../Base/Exception.h"

using namespace Gui;


namespace Gui {
struct ProgressBarPrivate
{
  int nElapsed; // in milliseconds
  int nRemaining;
  QTime measureTime;
  WaitCursor* cWaitCursor;
};
}

ProgressBar* ProgressBar::_pclSingleton = 0L;

ProgressBar* ProgressBar::Instance()
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

  setFixedWidth(120);

  // write percentage to the center
  setIndicatorFollowsStyle(false);
  setCenterIndicator(true);
  hide();
}

ProgressBar::~ProgressBar ()
{
  delete d;
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
    case QEvent::Close:
    case QEvent::MouseButtonDblClick:
      {
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

void ProgressBar::setProgress( int progress )
{
  if ( progress == 0 ) 
  {
    // starting
    d->measureTime.start();
    d->nElapsed = -1;
  }

  QProgressBar::setProgress( progress );
}

bool ProgressBar::start(const char* pszStr, unsigned long steps)
{
  // base stuff
  bool ret = SequencerBase::start(pszStr, steps);

  setTotalSteps(nTotalSteps);

  if ( pendingOperations() == 1 )
  {
    show();
    enterControlEvents();
    d->cWaitCursor = new Gui::WaitCursor;
  }

  return ret;
}

bool ProgressBar::next()
{
  if (!wasCanceled())
  {
    setProgress(nProgress++);
  }
  else
  {
    // force to abort the operation
    abort();
  }

  qApp->processEvents();

  return nProgress < nTotalSteps;
}

void ProgressBar::resetData()
{
  reset();
  setTotalSteps(0);
  setProgress(-1);
  hide();
  delete d->cWaitCursor;
  d->cWaitCursor = 0L;
  leaveControlEvents();
  ApplicationWindow::Instance->setPaneText( 1, "" );

  SequencerBase::resetData();
}

void ProgressBar::abort()
{
  //resets
  resetData();

  bool bMute = GuiConsoleObserver::bMute;
  GuiConsoleObserver::bMute = true;
  Base::Exception exc("Aborting...");
  GuiConsoleObserver::bMute = bMute;
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
  if ( d->nElapsed == elapsed )
    return false;
  d->nElapsed = elapsed;

  if ( totalSteps == 0 )
  {
    indicator = "";
    return true;
  }

  QString txt = indicator;
  if ( progress * 20 < totalSteps && elapsed < 5000 ) 
  {
    // Less than 5 percent complete and less than 5 secs have elapsed.
    txt = tr("Measuring...");
  }
  else 
  {
    int rest = (int) ( (double) totalSteps/progress * elapsed ) - elapsed;

    QTime time( 0,0, 0);
    time = time.addSecs( rest/1000 );
    txt = tr("Remaining: %1").arg( time.toString() );
  }

  ApplicationWindow::Instance->setPaneText( 1, txt );

  return QProgressBar::setIndicator ( indicator, progress, totalSteps );
}
