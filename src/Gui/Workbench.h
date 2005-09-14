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

#include "../Base/PyExport.h"

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
class GuiExport Workbench : public Base::PyHandler
{
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
   * The default implementation returns 0.
   */
  virtual Base::PyObjectBase* GetPyObject();

  ToolBarItem* importCustomBars( const char* node ) const;
  void exportCustomBars( ToolBarItem*, const char* node ) const;

protected:
  virtual MenuItem* setupMenuBar() const=0;
  virtual ToolBarItem* setupToolBars() const=0;
  virtual ToolBarItem* setupCommandBars() const=0;
  /**
   * Activates the workbench and adds/remove GUI elements.
   */
  bool activate();

private:
  QString _name;

  friend class WorkbenchManager;
};

class GuiExport StdWorkbench : public Workbench
{
public:
  StdWorkbench();
  virtual ~StdWorkbench();

protected:
  virtual MenuItem* setupMenuBar() const;
  virtual ToolBarItem* setupToolBars() const;
  virtual ToolBarItem* setupCommandBars() const;
};

class GuiExport TestWorkbench : public StdWorkbench
{
public:
  TestWorkbench();
  ~TestWorkbench();

protected:
  MenuItem* setupMenuBar() const;
  ToolBarItem* setupToolBars() const;
  ToolBarItem* setupCommandBars() const;
};

class GuiExport PythonWorkbench : public StdWorkbench
{
public:
  PythonWorkbench();
  ~PythonWorkbench();
  /**
   * Creates and returns immediately the corresponding Python workbench object.
   */
  Base::PyObjectBase* GetPyObject();

  void appendMenu( const QString& menu, const QStringList& items ) const;
  void removeMenu( const QString& menu ) const;

  void appendContextMenu( const QString& menu, const QStringList& items ) const;
  void removeContextMenu( const QString& menu ) const;

  void appendToolbar( const QString& bar, const QStringList& items ) const;
  void removeToolbar( const QString& bar ) const;

  void appendCommandbar( const QString& bar, const QStringList& items ) const;
  void removeCommandbar( const QString& bar ) const;

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
