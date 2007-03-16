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
class WaitCursorP : public QObject
{
public:
  static void setBusy( bool );

protected:
  bool eventFilter( QObject*, QEvent* );

private:
  WaitCursorP(); // Disable constructor
  static WaitCursorP* _instance;
  bool _isOn;
};
} // namespace Gui

WaitCursorP* WaitCursorP::_instance = 0;

WaitCursorP::WaitCursorP() : QObject(0), _isOn( false )
{
}

void WaitCursorP::setBusy( bool on )
{
  if ( _instance == 0 )
    _instance = new WaitCursorP();

  if( on == _instance->_isOn )
	  return;

  if ( on ) {
    qApp->installEventFilter( _instance );
    QApplication::setOverrideCursor( Qt::WaitCursor );
  }
  else {
    qApp->removeEventFilter( _instance );
    QApplication::restoreOverrideCursor();
  }

  _instance->_isOn = on;
}

bool WaitCursorP::eventFilter( QObject*, QEvent* e)
{
  if ( e->type() == QEvent::KeyPress ||
       e->type() == QEvent::KeyRelease ||
       e->type() == QEvent::MouseButtonPress ||
       e->type() == QEvent::MouseButtonRelease ||
       e->type() == QEvent::MouseButtonDblClick )
    return true;
  return false;
}

/**
 * Constructs this object and shows the wait cursor immediately.
 */
WaitCursor::WaitCursor()
{
  setWaitCursor();
}

/** Restores the last cursor again. */
WaitCursor::~WaitCursor()
{
  restoreCursor();
}

/**
 * Sets the wait cursor if needed.
 */
void WaitCursor::setWaitCursor()
{
  WaitCursorP::setBusy(true);
}

/**
 * Restores the last cursor if needed.
 */
void WaitCursor::restoreCursor()
{
  WaitCursorP::setBusy(false);
}
