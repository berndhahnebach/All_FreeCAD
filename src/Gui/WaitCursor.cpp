/***************************************************************************
                          WaitCursor.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Automatic and semi-automatic wait cursor
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qapplication.h>
#	include <qdatetime.h>
#endif

#include "WaitCursor.h"

struct FCWaitingCursorP
{
	bool wait;
	int minimumDuration;
	uint main_threadid;
	QTime measure;
};

FCWaitingCursor::FCWaitingCursor()
{
	d = new FCWaitingCursorP;
	d->minimumDuration = 1500;
	d->wait = false;
#ifdef FC_OS_WIN32
	d->main_threadid = GetCurrentThreadId();
#endif

	setWaitCursor();
}

FCWaitingCursor::~FCWaitingCursor()
{
	restoreCursor();
	delete d;
}

void FCWaitingCursor::run()
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

void FCWaitingCursor::setWaitCursor()
{
	// wait cursor already set
	if ( d->wait ) return;
	// thread is already running
	if ( running() ) return;
	d->measure.start();
	start();
}

void FCWaitingCursor::restoreCursor()
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

int FCWaitingCursor::minimumDuration()
{
	return d->minimumDuration;
}

void FCWaitingCursor::setMinimumDuration ( int ms )
{
	d->minimumDuration = ms;
}

// --------------------------------------------------------

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
	:main_threadid(id), nInterval(i), bRun(true),bActive(true)
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
#	ifdef FC_OS_WIN32 // win32 api functions
						hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
# else
						QApplication::setOverrideCursor(Qt::waitCursor);
#	endif
						cursorset = true;
					}
					else
						i = 0;
				}
			}
			else if ( ignore && cursorset)
			{
#	ifdef FC_OS_WIN32 // win32 api functions
				if (hCursor)
				{
					SetCursor(hCursor);
					cursorset = false;
				}
# else
				QApplication::restoreOverrideCursor();
				cursorset = false;
#	endif
			}
		}
		else
		{
			i=0;
		}
	}
}
