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
  // this style is very nice ;-)
  //  setIndicatorFollowsStyle(false);
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

bool ProgressBar::start(const char* pszStr, unsigned long steps)
{
  // base stuff
  bool ret = SequencerBase::start(pszStr, steps);

  setTotalSteps(nTotalSteps);

  if ( pendingOperations() == 1 )
  {
    show();
    enterControlEvents();
    d->nElapsed = 0;
    d->measureTime.start();
    d->cWaitCursor = new Gui::WaitCursor;
  }
  else
    d->measureTime.restart();

  return ret;
}

bool ProgressBar::next()
{
  if (!wasCanceled())
  {
    setProgress(nProgress++);

    // estimate the remaining time in milliseconds
    int diff = d->measureTime.restart();
    d->nElapsed += diff;
    d->nRemaining = d->nElapsed * ( totalSteps() - nProgress ) / nProgress;
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

void ProgressBar::setText (const char* pszTxt)
{
  // print message to the statusbar
  QString txt = pszTxt ? pszTxt : "";
  ApplicationWindow::Instance->statusBar()->message(txt);
}

/*
void ProgressBar::drawContents( QPainter *p )
{
  const int total_steps = totalSteps();
  const int progress_val = progress();
  QString progress_str;

  setIndicator ( progress_str, progress_val, total_steps );

  const QRect bar = contentsRect();

  if (total_steps) 
  {
    int u = bar.width();
    int pw;

    if ( u > 0 && progress_val >= INT_MAX / u && total_steps >= u )
      pw = (u * (progress_val / u)) / (total_steps / u);
    else
      pw = bar.width() * progress_val / total_steps;  

    p->setPen( colorGroup().highlightedText() );
    p->setClipRect( bar.x(), bar.y(), pw, bar.height() );
    p->fillRect( bar, colorGroup().brush( QColorGroup::Highlight ) );
    p->drawText( bar, AlignCenter, progress_str );

    p->setClipRect( bar.x()+pw, bar.y(), bar.width()-pw, bar.height() );
  }

  if ( progress_val != total_steps )
    p->fillRect( bar, colorGroup().brush( QColorGroup::Background ) );

#if QT_VERSION <= 230
  p->setPen( style()==MotifStyle? colorGroup().foreground() : colorGroup().text() );
#else
  p->setPen( style().isA("QMotifStyle")? colorGroup().foreground() : colorGroup().text());
#endif
  p->drawText( bar, AlignCenter, progress_str );
}
*/
bool ProgressBar::setIndicator ( QString & indicator, int progress, int totalSteps )
{
  return QProgressBar::setIndicator(indicator, progress, totalSteps);
  /*
    if ( totalSteps == 0 )
      return QProgressBar::setIndicator(indicator, progress, totalSteps);
    bool ret = false;
    if (progress != -1)
    {
      int nRemaining = d->nRemaining;
      nRemaining /= 1000;

      // get time format
      int second = nRemaining %  60; nRemaining /= 60;
      int minute = nRemaining %  60; nRemaining /= 60;
      int hour   = nRemaining %  60;
      QString h,m,s;
      if (hour < 10)   
        h = QString("0%1").arg(hour);
      else
        h = QString("%1").arg(hour);
      if (minute < 10) 
        m = QString("0%1").arg(minute);
      else
        m = QString("%1").arg(minute);
      if (second < 10) 
        s = QString("0%1").arg(second);
      else
        s = QString("%1").arg(second);

      // nice formating for output
      int steps = (100 * progress) / totalSteps;
      QString ind = QString("%1% (%2:%3:%4)").arg(steps).arg(h).arg(m).arg(s);

      if (ind != indicator)
      {
        indicator = ind;
        ret = true;
      }
    }
    else
    {
      indicator = QString::null;
    }

    return ret;
  */
}
