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


#ifndef WORKBENCH_H
#define WORKBENCH_H

#ifndef _PreComp_
# include <qstring.h>
# include <qstringlist.h>
#endif

#include <Base/PyExport.h>
#include <Base/Base.h>

namespace Base {
class PyObjectBase;
}

namespace Gui {

class MenuItem;
class ToolBarItem;
class WorkbenchManager;

/**
 * This is the base class for the workbench facility. Each FreeCAD module can provide its own 
 * workbench implementation. The workbench defines which GUI elements (such as toolbars, menus, 
 * dockable windows, ...) are added to the mainwindow and which gets removed or hidden. 
 * When a workbench object gets activated the first time the module - it stands for - gets 
 * loaded into RAM.
 * @author Werner Mayer
 */
class GuiExport Workbench : public Base::BaseClass, public Base::PyHandler
{
    TYPESYSTEM_HEADER();

public:
  /** Constructs a workbench object. */
  Workbench();
  virtual ~Workbench();
  /**
   * Returns the name of the workbench object.
   */
  QString name() const;
  /**
   * Set the name to the workbench object.
   */
  void setName( const QString& );
  /**
   * The default implementation returns an instance of @ref WorkbenchPy.
   */
  virtual Base::PyObjectBase* GetPyObject();
  /**
   * The method imports the user defined toolbars or command bars and creates
   * a ToolBarItem tree structure.
   */
  ToolBarItem* importCustomBars( const char* node ) const;
  /**
   * The method exports the user defined ToolBarItem tree structure into the parameter
   * settings.
   */
  void exportCustomBars( ToolBarItem*, const char* node ) const;
  /** Sets up the contextmenu for the this workbench. 
   * The default implementation does nothing.
   */
  virtual void setupContextMenu(const char* recipient,MenuItem*) const;

protected:
  /** Returns a MenuItem tree structure of menus for the this workbench. */
  virtual MenuItem* setupMenuBar() const=0;
  /** Returns a ToolBarItem tree structure of toolbars for the this workbench. */
  virtual ToolBarItem* setupToolBars() const=0;
  /** Returns a ToolBarItem tree structure of command bars for the this workbench. */
  virtual ToolBarItem* setupCommandBars() const=0;
  /**
   * Activates the workbench and adds/removes GUI elements.
   */
  bool activate();

private:
  void showOrHideToolBars(bool read) const;

private:
  QString _name;

  friend class WorkbenchManager;
};

/**
 * The StdWorkbench class defines the standard menus, toolbars, commandbars etc. 
 * To define own workbenches you should inherit from StdWorkbench instead of Workbench
 * to have defined the standard GUI elements.
 * @author Werner Mayer
 */
class GuiExport StdWorkbench : public Workbench
{
    TYPESYSTEM_HEADER();

public:
  StdWorkbench();
  virtual ~StdWorkbench();

public:
  /** Defines the standard context menu. */
  virtual void setupContextMenu(const char* recipient,MenuItem*) const;

protected:
  /** Defines the standard menus. */
  virtual MenuItem* setupMenuBar() const;
  /** Defines the standard toolbars. */
  virtual ToolBarItem* setupToolBars() const;
  /** Defines the standard command bars. */
  virtual ToolBarItem* setupCommandBars() const;
};

class GuiExport TestWorkbench : public StdWorkbench
{
    TYPESYSTEM_HEADER();

public:
  TestWorkbench();
  ~TestWorkbench();

protected:
  MenuItem* setupMenuBar() const;
  ToolBarItem* setupToolBars() const;
  ToolBarItem* setupCommandBars() const;
};

/**
 * The PythonWorkbench class allows the manipulation of the workbench from Python.
 * Therefore PythonWorkbenchPy provides the required Python interface.
 * @author Werner Mayer
 */
class GuiExport PythonWorkbench : public StdWorkbench
{
    TYPESYSTEM_HEADER();

public:
  PythonWorkbench();
  ~PythonWorkbench();
  /**
   * Creates and returns immediately the corresponding Python workbench object.
   */
  Base::PyObjectBase* GetPyObject();

  /** @name Manipulation methods */
  //@{
  /// Appends a new menu
  void appendMenu( const QStringList& menu, const QStringList& items ) const;
  /// Removes a menu
  void removeMenu( const QString& menu ) const;
  //// Shows a list of all menus
  QStringList listMenus() const;

  /// Appends new context menu items
  void appendContextMenu( const QString& menu, const QStringList& items ) const;
  /// Removes a context menu
  void removeContextMenu( const QString& menu ) const;

  /// Appends a new toolbar
  void appendToolbar( const QString& bar, const QStringList& items ) const;
  /// Removes a toolbar
  void removeToolbar( const QString& bar ) const;
  //// Shows a list of all toolbars
  QStringList listToolbars() const;

  /// Appends a new command bar
  void appendCommandbar( const QString& bar, const QStringList& items ) const;
  /// Removes a command bar
  void removeCommandbar( const QString& bar ) const;
  //// Shows a list of all command bars
  QStringList listCommandbars() const;
  //@}

protected:
  MenuItem* setupMenuBar() const;
  ToolBarItem* setupToolBars() const;
  ToolBarItem* setupCommandBars() const;

private:
  MenuItem* _menuBar;
  ToolBarItem* _toolBar;
  ToolBarItem* _commandBar;
  Base::PyObjectBase* _workbenchPy;
};

} // namespace Gui


#endif // WORKBENCH_H 
