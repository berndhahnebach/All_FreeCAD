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
# include <qdatetime.h>
# ifdef FC_OS_WIN32
#   include <windows.h>
# endif
#endif

#include "WaitCursor.h"

using namespace Gui;

namespace Gui {
struct WaitCursorP
{
  bool wait;
  int minimumDuration;
  uint main_threadid;
  QTime measure;
};
} // namespace Gui

/**
 * Constructs this object and starts the thread immediately.
 * The minimum duration time is set to 1.500 ms. 
 */
WaitCursor::WaitCursor()
{
  d = new WaitCursorP;
  d->minimumDuration = 1500;
  d->wait = false;
#ifdef FC_OS_WIN32
  d->main_threadid = GetCurrentThreadId();
#endif

  setWaitCursor();
}

/** Restores the last cursor again. */
WaitCursor::~WaitCursor()
{
  restoreCursor();
  delete d;
}

/** 
 * Either this method is living as long as the instance is existing or until the
 * minimum duration time is reached. In the second case the waitcursor is set and the
 * thread terminates.
 */
void WaitCursor::run()
{
#ifdef FC_OS_WIN32
  AttachThreadInput(GetCurrentThreadId(), d->main_threadid, true);
#endif

  while (true)
  {
    // set the thread sleeping
    msleep(100);
    if ( d->measure.elapsed() > d->minimumDuration )
    {
      QApplication::setOverrideCursor(Qt::waitCursor);
      d->wait = true;
      break;
    }
  }

#ifdef FC_OS_WIN32
  AttachThreadInput(GetCurrentThreadId(), d->main_threadid, false);
#endif
}

/**
 * Starts the thread unless the thread is already running or the waitcursor is set.
 */
void WaitCursor::setWaitCursor()
{
  // wait cursor already set
  if ( d->wait ) return;
  // thread is already running
  if ( running() ) return;
  d->measure.start();
  start();
}

/**
 * Terminates the running thread and restores the cursor.
 */
void WaitCursor::restoreCursor()
{
  // if running just terminate it
  if ( running() )
  {
    terminate();
    wait(100);
  }

  // wait cursor already restored
  if ( !d->wait ) return;
  QApplication::restoreOverrideCursor();
  d->wait = false;
}

/**
 * Returns the minimum duration time.
 */
int WaitCursor::minimumDuration()
{
  return d->minimumDuration;
}

/**
 * Sets the minimum duration time to \a ms milliseconds. 
 */
void WaitCursor::setMinimumDuration ( int ms )
{
  d->minimumDuration = ms;
}

// --------------------------------------------------------
/*
class FCAutoWaitCursorP : public QWidget
{
public:
  FCAutoWaitCursorP() : QWidget(0L)
  {
  }

protected:
  void customEvent(QCustomEvent* e)
  {
    // if this method is called the message loop is not blocked
    FCAutoWaitCursor::Instance().mutex.lock();
    FCAutoWaitCursor::Instance().bActive = true;
    FCAutoWaitCursor::Instance().mutex.unlock();
  }
};

FCAutoWaitCursor* FCAutoWaitCursor::_pclSingleton = NULL;

void FCAutoWaitCursor::Destruct(void)
{
  // not initialized or double destruct!
  assert(_pclSingleton);
  delete _pclSingleton;
  _pclSingleton = NULL;
}

FCAutoWaitCursor &FCAutoWaitCursor::Instance(void)
{
  // not initialized?
  if(!_pclSingleton)
  {
#ifdef FC_OS_WIN32
    _pclSingleton = new FCAutoWaitCursor(GetCurrentThreadId(), 100);
#else
    _pclSingleton = new FCAutoWaitCursor(0, 100);
#endif
  }

  return *_pclSingleton;
}

FCAutoWaitCursor::FCAutoWaitCursor(uint id, int i)
    :main_threadid(id), nInterval(i), bActive(true),bRun(true)
{
  d = new FCAutoWaitCursorP;
  start();
}

FCAutoWaitCursor::~FCAutoWaitCursor()
{
#ifdef FC_OS_WIN32
  AttachThreadInput(GetCurrentThreadId(), main_threadid, false);
#endif
  bRun = false;
  wait();
  delete d;
}

void FCAutoWaitCursor::SetWaitCursor()
{
#ifdef FC_OS_WIN32 // win32 api functions
  SetCursor(LoadCursor(NULL, IDC_WAIT));
#endif
}

void FCAutoWaitCursor::run()
{
  int step = 5;
  int i=0;
  QSize size;
  QPoint pos;
  bool ignore;
  bool cursorset;

#ifdef FC_OS_WIN32
  HCURSOR hCursor = NULL;
  AttachThreadInput(GetCurrentThreadId(), main_threadid, true);
#endif

  while (bRun)
  {
    // set the thread sleeping
    msleep(nInterval);

    // application seems to be busy
    if (qApp->locked())
    {
      ignore = false;

      // search for an active window
      QWidget* w = qApp->focusWidget();
      if ( w )
      {
        // is application really busy ?
        //
        // If you press a mouse button on the edge of
        // a native window (under Windows OS) the application's
        // message loop is blocked. Then look if the window
        // is moved or resized, if so the appliaction is NOT busy.
        // (But this cannot detect cases if the user clicks on a
        //  system menu button or doesn't move the mouse after clicking)
        ignore = (size != w->size() || pos != w->pos());
        size = w->size();
        pos = w->pos();
      }

      if (i<step && !ignore)
      {
        i++;

        // seems to be busy, so send an event to the dummy widget
        if (i==step-2)
        {
          mutex.lock();
          bActive = false;
          mutex.unlock();

          // send custom event to make sure that the application is really busy
          QApplication::postEvent(d, new QCustomEvent(346789));
        }

        if (i==step)
        {
          if (bActive == false)
          {
#ifdef FC_OS_WIN32 // win32 api functions
            hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
#else
            QApplication::setOverrideCursor(Qt::waitCursor);
#endif
            cursorset = true;
          }
          else
            i = 0;
        }
      }
      else if ( ignore && cursorset)
      {
#ifdef FC_OS_WIN32 // win32 api functions
        if (hCursor)
        {
          SetCursor(hCursor);
          cursorset = false;
        }
#else
        QApplication::restoreOverrideCursor();
        cursorset = false;
#endif
      }
    }
    else
    {
      i=0;
    }
  }
}
*/
