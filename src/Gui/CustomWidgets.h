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

#include "../Base/Parameter.h"
#include "PrefWidgets.h"

#ifndef _PreComp_
# include <qpopupmenu.h>
# include <qtoolbar.h>
#endif

// forward declarations
class QDoubleValidator;
class QAction;
class QMainWindow;

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
class GuiExport CustomWidget : public PrefWidget
{
public:
  bool hasCustomItems();
  const QStringList& getCustomItems() const;
  void setCustomItems( const QStringList& items );
  void addCustomItems( FCParameterGrp::handle, const QStringList& item );
  void removeCustomItems( FCParameterGrp::handle, const QStringList& item );

  void loadXML();
  void saveXML();

  virtual void setRemovable(bool b);
  bool isRemovable() const;

  virtual void setCanModify(bool b);
  bool canModify() const;

  QString getWorkbench();
  virtual ~CustomWidget();

  static FCParameterGrp::handle getParameter();

protected:
  virtual void rebuild() = 0;
  typedef QMap<FCParameterGrp::handle, QStringList> WorkbenchItems;

  CustomWidget( const char* grp, const char * name );
  virtual void restorePreferences();
  virtual void savePreferences();

  QStringList _Items;
  WorkbenchItems _WbItems;

  FCParameterGrp::handle hPrefGrp; /**< Handle to the appropriate parameter group. */

private:
  void init( const char* grp, const char* name );

  QString  _Workbench;
  bool     _bCanModify;
  bool     _bCanRemove;
};

/**
 * The CustomToolbar class provides method to customize toolbars.
 * \author Werner Mayer
 */
class GuiExport CustomToolBar : public QToolBar, public CustomWidget
{
  Q_OBJECT

public:
  CustomToolBar ( const QString & label, QMainWindow *, QWidget *, bool newLine = FALSE, const char * name = 0, WFlags f = 0, const char* type = "Toolbars" );
  CustomToolBar ( QMainWindow * parent = 0, const char * name = 0, const char* type = "Toolbars" );
  virtual ~CustomToolBar();

  virtual void clearUp();
  void setCanModify( bool b );

public:
  static bool isAllowed( QWidget* w );

protected:
  void rebuild();
  void dropEvent     ( QDropEvent      * );
  void dragEnterEvent( QDragEnterEvent * );
  void dragLeaveEvent( QDragLeaveEvent * );
  void dragMoveEvent ( QDragMoveEvent  * );

  virtual void restorePreferences();
  virtual void savePreferences();

protected slots:
  void languageChange();
};

/**
 * The CustomToolbar class provides method to customize toolbars.
 * \author Werner Mayer
 */
class GuiExport CustomPopupMenu : public QPopupMenu, public CustomWidget
{
  Q_OBJECT

public:
  CustomPopupMenu ( QWidget * parent=0, const char * name=0, const char* menu = 0 );
  virtual ~CustomPopupMenu();
  virtual void OnChange( FCSubject<const char*> &rCaller, const char * sReason );

protected:
  void rebuild();
  void dropEvent        ( QDropEvent      * );
  void dragEnterEvent   ( QDragEnterEvent * );
  void dragLeaveEvent   ( QDragLeaveEvent * );
  void dragMoveEvent    ( QDragMoveEvent  * );
  void mouseMoveEvent   ( QMouseEvent     * );

  virtual void restorePreferences();
  virtual void savePreferences();

private:
  QString _parent;
  bool    _bAllowDrag;
  FCParameterGrp::handle _hCommonGrp;
};

/**
 * Class that manages the construction/destruction of all customizable widgets.
 * At destruction time of a custom widget its content will be written to preferences.
 * At construction time the content will be restored.
 * @see CustomToolBar, CustomPopupMenu
 * \author Werner Mayer
 */
class GuiExport CustomWidgetManager
{
public:
  CustomWidgetManager( Gui::DockWnd::ToolBox* pCmdBar );
  ~CustomWidgetManager();

  bool loadCustomWidegts( const QString& workbench );
  bool update( const QString& workbench );
  bool update();

  CustomToolBar* getToolBar( const QString& name );
  QPtrList<CustomToolBar> getToolBars();
  void addToolBar   ( const QString& type, const QStringList& defIt );
  void removeToolBarFromSettings( const QString& name);
  void removeToolBar( const QString& name);
  void removeToolBarItems ( const QString& type, const QStringList& item );
  int countToolBars();

  CustomToolBar* getCommandBar( const QString& name );
  QPtrList<CustomToolBar> getCommdandBars();
  void addCommandBar    ( const QString& type, const QStringList& defIt );
  void removeCommandBarFromSettings( const QString& name);
  void removeCommandBar ( const QString& name );
  void removeCommandBarItems (const QString& type, const QStringList& item);
  int countCommandBars();

  CustomPopupMenu* getPopupMenu( const QString& name, const char* parent = 0 );
  QPtrList<CustomPopupMenu> getPopupMenus();
  void addPopupMenu ( const QString& type, const QStringList& defIt,
                      const char* parent = 0);
  void removePopupMenuFromSettings( const QString& name);
  void removePopupMenu ( const QString& name );
  void removeMenuItems ( const QString& type, const QStringList& item );
  int countPopupMenus();
  const QMap<int, QString>&  menuBarItems() const;

  void showToolBox();
  void hideToolBox();

private:
  struct CustomWidgetManagerP* d;
};

} // namespace Gui

#endif // CUSTOM_WIDGETS_H__
