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


#ifndef GUI_WAIT_CURSOR_H
#define GUI_WAIT_CURSOR_H

#include <qthread.h>


namespace Gui {

/**
 * This class sets a waitcursor automatically while a  slow operation is running. 
 * Therefore you just have to create an instance of WaitCursor before the time 
 * consuming operation.
 * If the operation takes less time than the @ref minimumDuration (1500 ms
 * by default), the waiting cursor will not appear at all. For example
 *
 * \code:
 * WaitCursor ac;
 * ...
 * ...                   // slow operation
 * ...
 * \endcode
 *  
 * The @ref setWaitCursor() method is called in the constructor, @ref restoreCursor()
 * at destruction time of WaitCursor.
 *
 * Sometimes you have two slow operations with some user interactions between them.
 * Avoiding to show the waiting cursor then you have to call the methods @ref restoreCursor()
 * and setWaitCursor manually, like:
 *
 * \code:
 * WaitCursor ac;
 * ...
 * ...                   // 1st slow operation
 * ac.restoreCursor();
 * ...
 * ...                  // some dialog stuff
 * ac.setWaitCursor();
 * ...
 * ...                  // 2nd slow operation
 * \endcode
 *  
 * @author Werner Mayer
 */
class GuiExport WaitCursor : public QThread
{
public:
  WaitCursor();
  ~WaitCursor();

  void setWaitCursor();
  void restoreCursor();
  int  minimumDuration();
  void setMinimumDuration ( int ms );

private:
  void run();
  struct WaitCursorP* d;
};

} // namespace Gui

#endif // GUI_WAIT_CURSOR_H 

