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


#ifndef GUI_WORKBENCH_H
#define GUI_WORKBENCH_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include <Base/BaseClass.h>

namespace Base {
class PyObjectBase;
}

namespace Gui {

class MenuItem;
class ToolBarItem;
class DockWindowItems;
class WorkbenchManager;

/**
 * This is the base class for the workbench facility. Each FreeCAD module can provide its own 
 * workbench implementation. The workbench defines which GUI elements (such as toolbars, menus, 
 * dockable windows, ...) are added to the mainwindow and which gets removed or hidden. 
 * When a workbench object gets activated the first time the module - it stands for - gets 
 * loaded into RAM.
 * @author Werner Mayer
 */
class GuiExport Workbench : public Base::BaseClass
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
    PyObject* getPyObject();
    /** Sets up the contextmenu for this workbench. 
     * The default implementation does nothing.
     */
    virtual void setupContextMenu(const char* recipient,MenuItem*) const;
    /** 
     * Activates the workbench and adds/removes GUI elements.
     */
    bool activate();
    /**
     * Translates the window titles of all menus, toolbars and dock windows.
     */
    void retranslate() const;
    /** Run some actions when the workbench gets activated. */
    virtual void activated();
    /** Run some actions when the workbench gets deactivated. */
    virtual void deactivated();

protected:
    /** Returns a MenuItem tree structure of menus for this workbench. */
    virtual MenuItem* setupMenuBar() const=0;
    /** Returns a ToolBarItem tree structure of toolbars for this workbench. */
    virtual ToolBarItem* setupToolBars() const=0;
    /** Returns a ToolBarItem tree structure of command bars for this workbench. */
    virtual ToolBarItem* setupCommandBars() const=0;
    /** Returns a DockWindowItems structure of dock windows this workbench. */
    virtual DockWindowItems* setupDockWindows() const=0;

private:
    /**
     * The method imports the user defined toolbars or toolbox bars and creates
     * a ToolBarItem tree structure.
     */
    void setupCustomToolbars(ToolBarItem* root, const char* toolbar) const;
    void setupCustomShortcuts() const;

private:
    QString _name;
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
    /** Returns a DockWindowItems structure of dock windows this workbench. */
    virtual DockWindowItems* setupDockWindows() const;
};

/**
 * The NoneWorkbench class defines a slim workbench. 
 * @author Werner Mayer
 */
class GuiExport NoneWorkbench : public StdWorkbench
{
    TYPESYSTEM_HEADER();

public:
    NoneWorkbench();
    virtual ~NoneWorkbench();

    /** Defines the standard context menu. */
    virtual void setupContextMenu(const char* recipient,MenuItem*) const;

protected:
    /** Defines the standard menus. */
    virtual MenuItem* setupMenuBar() const;
    /** Defines the standard toolbars. */
    virtual ToolBarItem* setupToolBars() const;
    /** Defines the standard command bars. */
    virtual ToolBarItem* setupCommandBars() const;
    /** Returns a DockWindowItems structure of dock windows this workbench. */
    virtual DockWindowItems* setupDockWindows() const;
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
    PyObject* getPyObject();

    /** @name Manipulation methods */
    //@{
    /// Appends a new menu
    void appendMenu(const QStringList& menu, const QStringList& items) const;
    /// Removes a menu
    void removeMenu(const QString& menu ) const;
    //// Shows a list of all menus
    QStringList listMenus() const;

    /// Appends new context menu items
    void appendContextMenu(const QStringList& menu, const QStringList& items) const;
    /// Removes a context menu
    void removeContextMenu(const QString& menu ) const;
    void setupContextMenu(const char* recipient,MenuItem*) const;
    void clearContextMenu();

    /// Appends a new toolbar
    void appendToolbar(const QString& bar, const QStringList& items) const;
    /// Removes a toolbar
    void removeToolbar(const QString& bar) const;
    //// Shows a list of all toolbars
    QStringList listToolbars() const;

    /// Appends a new command bar
    void appendCommandbar(const QString& bar, const QStringList& items) const;
    /// Removes a command bar
    void removeCommandbar(const QString& bar) const;
    //// Shows a list of all command bars
    QStringList listCommandbars() const;
    //@}

protected:
    MenuItem* setupMenuBar() const;
    ToolBarItem* setupToolBars() const;
    ToolBarItem* setupCommandBars() const;

private:
    MenuItem* _menuBar;
    MenuItem* _contextMenu;
    ToolBarItem* _toolBar;
    ToolBarItem* _commandBar;
    Base::PyObjectBase* _workbenchPy;
};

} // namespace Gui


#endif // GUI_WORKBENCH_H 
