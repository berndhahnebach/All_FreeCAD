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


#ifndef __TOOLBOXBAR_H__
#define __TOOLBOXBAR_H__

#include "CustomWidgets.h"

namespace Gui {
namespace DockWnd {

class StackBar;

/** 
 * The CommandBar is a special kind of QToolBar with some extensions.
 * So this class is able to manage its content on its own.
 * Furthermore this class provides some methods to change its appearance.
 * \author Werner Mayer
 */
class GuiExport CommandBar : public Gui::CustomToolBar
{
  Q_OBJECT

public:
  virtual void clearUp();
  virtual void setTextToLastItem( const QString& ) = 0;
  void setDummyToLastItem();

protected:
  CommandBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
  virtual ~CommandBar ();

  void mousePressEvent( QMouseEvent * );

protected slots:
  void cleanupEventFilter();
  void popupMenuAboutToShow();
  void setNewBackgroundColor();
  void resetBackgroundColor();

private:
  QWidget*     m_Dummy;
  QPopupMenu*  m_Popup;
  QColor       m_Color;
};

/** 
 * The toolbox bar can be used as an item for the ToolBox containing all the tool buttons.
 * The icons of the buttons are drawn on the left side while the text is drawn on the right
 * side.
 * \author Werner Mayer
 */
class GuiExport ToolBoxBar : public CommandBar
{
  Q_OBJECT

public:
  ToolBoxBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
  virtual ~ToolBoxBar ();
  virtual void setTextToLastItem( const QString& );
};

/**
 * The bar seems like the Outlook bar.
 * \author Werner Mayer
 */
class GuiExport OutlookBar : public CommandBar
{
  Q_OBJECT

public:
  OutlookBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
  virtual ~OutlookBar ();
  virtual void setTextToLastItem( const QString& );
};

} // namespace DockWnd
} // namespace Gui

#endif // __TOOLBOXBAR_H__

