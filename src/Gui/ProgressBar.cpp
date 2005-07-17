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
  QTime measureTime;
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

void ProgressBar::setProgress( int progress )
{
  QProgressBar::setProgress( progress );
}

void ProgressBar::startStep()
{
  setTotalSteps(nTotalSteps);

  if ( pendingOperations() == 1 )
  {
    show();
    enterControlEvents();
    d->cWaitCursor = new Gui::WaitCursor;

    // starting
    d->measureTime.start();
  }
}

void ProgressBar::nextStep()
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

  ConsoleMsgFlags ret = Base::Console().SetEnabledMsgType("MessageBox",ConsoleMsgType::MsgType_Wrn|
                                                                       ConsoleMsgType::MsgType_Err, false);
  Base::Exception exc("Aborting...");
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

  ApplicationWindow::Instance->setPaneText( 1, txt );

  return QProgressBar::setIndicator ( indicator, progress, totalSteps );
}
