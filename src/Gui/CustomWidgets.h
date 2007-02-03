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


#ifndef CUSTOM_WIDGETS_H__
#define CUSTOM_WIDGETS_H__

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include "../Base/Parameter.h"
#include "PrefWidgets.h"

//#include <q3popupmenu.h>
//#include <q3toolbar.h>
////Added by qt3to4:
//#include <QtGui/QMouseEvent>
//#include <QtGui/QDragMoveEvent>
//#include <QtGui/QDragLeaveEvent>
//#include <QtGui/QDropEvent>
//#include <QtGui/QDragEnterEvent>

// forward declarations
//class QDoubleValidator;
//class QAction;
//class Q3MainWindow;

namespace Gui {
class DockWindow;
class CommandManager;
class WidgetFactoryInst;

namespace DockWnd {
class ToolBox;

} // namespace DockWnd

/**
 * The abstract CustomWidget class provides methods to customize widgets, e.g. toolbars or menus.
 * It is possible to define whether a CustomWidget can be modified or even can be removed from
 * the workbench it belongs to.
 * \author Werner Mayer
 */
class GuiExport CustomWidget
{
public:
  virtual void setCanModify(bool b);
  bool canModify() const;

  virtual ~CustomWidget();

protected:
  CustomWidget();

private:
  bool     _bCanModify;
};

/**
 * The CustomToolbar class provides method to customize toolbars.
 * \author Werner Mayer
 */
class GuiExport CustomToolBar : public Q3ToolBar, public CustomWidget
{
  Q_OBJECT

public:
  CustomToolBar ( const QString & label, Q3MainWindow *, QWidget *, bool newLine = FALSE, const char * name = 0, Qt::WFlags f = 0, const char* type = "Toolbars" );
  CustomToolBar ( Q3MainWindow * parent = 0, const char * name = 0, const char* type = "Toolbars" );
  virtual ~CustomToolBar();

  void setCanModify( bool b );

protected:
  void dropEvent       ( QDropEvent      * );
  void dragEnterEvent  ( QDragEnterEvent * );
  void dragLeaveEvent  ( QDragLeaveEvent * );
  void dragMoveEvent   ( QDragMoveEvent  * );

protected Q_SLOTS:
  void languageChange();
};

/**
 * The CustomToolbar class provides method to customize toolbars.
 * \author Werner Mayer
 */
//class GuiExport CustomPopupMenu : public Q3PopupMenu, public CustomWidget, public ParameterGrp::ObserverType
//{
//  Q_OBJECT
//
//public:
//  CustomPopupMenu ( QWidget * parent=0, const char * name=0, const char* menu = 0 );
//  virtual ~CustomPopupMenu();
//  virtual void OnChange( Base::Subject<const char*> &rCaller, const char * sReason );
//
//protected:
//  void dropEvent        ( QDropEvent      * );
//  void dragEnterEvent   ( QDragEnterEvent * );
//  void dragLeaveEvent   ( QDragLeaveEvent * );
//  void dragMoveEvent    ( QDragMoveEvent  * );
//  void mouseMoveEvent   ( QMouseEvent     * );
//
//private:
//  bool    _bAllowDrag;
//};

} // namespace Gui

#endif // CUSTOM_WIDGETS_H__
