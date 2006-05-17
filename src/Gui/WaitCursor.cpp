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
  static bool override;
  static bool locked;
  int minimumDuration;
  uint main_threadid;
  QTime measure;
};
} // namespace Gui

/* We cannot use Qt's QMutex as we locks/unlocks from different threads, which
 * is not allowed. So, we use a simple boolean.
 */
bool WaitCursorP::override = false;
bool WaitCursorP::locked = false;

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
#if defined(_MSC_VER) && _MSC_VER < 1400
  AttachThreadInput(GetCurrentThreadId(), d->main_threadid, true);
#endif

  while (true)
  {
    // set the thread sleeping
    msleep(100);
    // just wait if locked
    if ( !WaitCursorP::locked )
    {
      if ( d->measure.elapsed() > d->minimumDuration )
      {
        if ( !WaitCursorP::override )
        {
          // prevent application from setting wait cursor twice
          WaitCursorP::override = true;
          QApplication::setOverrideCursor(Qt::waitCursor);
          d->wait = true;
        }
        break;
      }
    }
  }

#if defined(_MSC_VER) && _MSC_VER < 1400
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

  // check whether this instance has overridden the cursor
  if ( d->wait )
  {
    // do not restore twice
    QApplication::restoreOverrideCursor();
    d->wait = false;
    WaitCursorP::override = false;
  }
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

void WaitCursor::lock()
{
  WaitCursorP::locked = true;
}

void WaitCursor::unlock()
{
  WaitCursorP::locked = false;
}

bool WaitCursor::locked()
{
  return WaitCursorP::locked;
}
