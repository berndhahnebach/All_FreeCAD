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


#ifndef __ACTION_H__
#define __ACTION_H__

#include <qaction.h>
#include <qdragobject.h>
#include <qmainwindow.h>

namespace Gui 
{
class Command;
struct DockWindowActionP;

/**
 * Class for drag and drop a 'QAction' object
 * @author Werner Mayer
 */
class ActionDrag : public QStoredDrag
{
public:
  ActionDrag ( QString action, QWidget * dragSource = 0, const char * name = 0 );
  virtual ~ActionDrag ();

  static bool canDecode ( const QMimeSource * e );
  static bool decode ( const QMimeSource * e, QString&  action );

public:
  static QStringList actions;
};

// --------------------------------------------------------------------

/**
 * The Action class is the link between Qt's QAction class and FreeCAD's 
 * command classes (@ref Command). So, it is possible to have all actions
 * (from toolbars, menus, ...) implemented in classes instead of slots of
 * the main window. 
 * @author Werner Mayer
 */
class GuiExport Action : public QAction
{
  Q_OBJECT

public:
  Action ( Command* pcCmd, QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
  virtual ~Action();

  virtual void addedTo ( QWidget * actionWidget, QWidget * container );
  virtual bool addTo ( QWidget * w );
  virtual bool removeFrom ( QWidget * w );
  
  void setEnabled  ( bool ) ;

  Command* command() { return _pcCmd; }

public slots:
  void onActivated ();
  void onToggled   ( bool ); 

private:
  Command *_pcCmd;
};

// --------------------------------------------------------------------

/**
 * The ActionGroup class is the link between Qt's QActionGroup class and 
 * FreeCAD's command group classes (@ref CommandGroup). So, it is possible to 
 * have all actions (from toolbars, menus, ...) implemented in classes instead of 
 * slots of the main window. 
 * @author Werner Mayer
 */
class GuiExport ActionGroup : public QActionGroup
{
  Q_OBJECT

public:
  ActionGroup ( Command* pcCmd, QObject * parent, const char * name = 0 );
  virtual ~ActionGroup();

  Command* command() { return _pcCmd; }

  int currentActive() const;
  void setCurrentActive(int);

public slots:
  void onActivated ( QAction* );

private:
  Command *_pcCmd;
};

// --------------------------------------------------------------------

/**
 * The WorkbenchAction class represents a workbench. When this action gets 
 * activated the workbench - it represents - gets loaded.
 * @see WorkbenchGroup
 * @author Werner Mayer
 */
class StdCmdWorkbench;
class GuiExport WorkbenchAction : public QAction
{
  Q_OBJECT

public:
  /** 
   * Creates an action for the command \a pcCmd to load the workbench \a name
   * when it gets activated.
   */
  WorkbenchAction ( StdCmdWorkbench* pcCmd, QObject * parent, const char * name = 0 );
  virtual ~WorkbenchAction();

private slots:
  void onActivated();

private:
  StdCmdWorkbench *_pcCmd;
};

// --------------------------------------------------------------------

/**
 * The WorkbenchGroup class is an action group that holds WorkbenchAction objects
 * to switch between workbenches.
 * @see WorkbenchAction
 * @author Werner Mayer
 */
class GuiExport WorkbenchGroup : public QActionGroup
{
  Q_OBJECT

public:
  WorkbenchGroup ( QObject * parent = 0, const char * name = 0, bool exclusive = TRUE );
  virtual ~WorkbenchGroup();

  /** Activates the workbench with name \a item. */
  void activate( const QString& item );
};

// --------------------------------------------------------------------

/**
 * The MRUAction class represents an item of the recent file list. Usually
 * this list is provided by a popupmenu.
 * @see MRUActionGroup
 * @author Werner Mayer
 */
class GuiExport MRUAction : public QAction
{
  Q_OBJECT

protected:
  MRUAction ( QObject * parent, const char * name = 0, bool toggle = FALSE );
  virtual ~MRUAction();

  int index() const;

protected:
  virtual void addedTo ( int index, QPopupMenu * menu );

private:
  QString recentFileItem( const QString& fn );
  int _idx;

  friend class MRUActionGroup;
};

/**
 * The MRUActionGroup class holds all MRUAction objects for the recent file list. 
 * Usually this list is provided by a popupmenu that is added to the "File" menu.
 * @see MRUAction
 * @author Werner Mayer
 */
class GuiExport MRUActionGroup : public QActionGroup
{
  Q_OBJECT

public:
  MRUActionGroup ( Command* pcCmd, QObject * parent = 0, const char * name = 0, bool exclusive = TRUE );
  virtual ~MRUActionGroup();

  void setRecentFiles( const QStringList& );

private slots:
  void onActivated ();

private:
  void clear();

private:
  Command *_pcCmd;
};

// --------------------------------------------------------------------

/**
 * The SepAction class represents an separator of a toolbar or popup menu. This action can be added to a group
 * after the group has been added to a widget.
 * @author Werner Mayer
 */
class GuiExport SepAction : public QAction
{
  Q_OBJECT

protected:
  SepAction ( QObject * parent, const char * name = 0, bool toggle = FALSE );
  virtual ~SepAction();

  bool addTo(QWidget*);

protected:
  virtual void addedTo ( int index, QPopupMenu * menu );

  friend class ListActionGroup;
};

/**
 * The ListAction class represents an item of a dynamic list that may grow (but not shrink). Usually
 * this list is provided as a popupmenu.
 * @see ListActionGroup
 * @author Werner Mayer
 */
class GuiExport ListAction : public QAction
{
  Q_OBJECT

protected:
  ListAction ( QObject * parent, const char * name = 0, bool toggle = FALSE );
  virtual ~ListAction();

  int index() const;

protected:
  virtual void addedTo ( int index, QPopupMenu * menu );

private:
  int _idx;

  friend class ListActionGroup;
};

/**
 * The ListActionGroup class is the container for ListAction objects.
 * @see ListAction
 * @author Werner Mayer
 */
class GuiExport ListActionGroup : public QActionGroup
{
  Q_OBJECT

public:
  ListActionGroup ( Command* pcCmd, QObject * parent = 0, const char * name = 0, bool exclusive = TRUE );
  virtual ~ListActionGroup();

  void addActionData( const QString&, const QString& = QString::null );
  QString getData(int) const;
  QAction* getAction(int) const;

private slots:
  void onActivated ();

private:
  Command *_pcCmd;
};

// --------------------------------------------------------------------

/**
 * The UndoRedoAction class reimplements the addedTo() function to make a popup menu 
 * appearing when the button with the arrow is clicked.
 * @author Werner Mayer
 */
class GuiExport UndoRedoAction : public Action
{
  Q_OBJECT

public:
  UndoRedoAction ( Command* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
  virtual ~UndoRedoAction();
  
  void addedTo ( QWidget * actionWidget, QWidget * container );
};

// --------------------------------------------------------------------

/**
 * Special action for the undo button
 * @author Werner Mayer
 */
class GuiExport UndoAction : public UndoRedoAction
{
  Q_OBJECT

public:
  UndoAction ( Command* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
  ~UndoAction();
};

// --------------------------------------------------------------------

/**
 * Special action for the redo button
 * @author Werner Mayer
 */
class GuiExport RedoAction : public UndoRedoAction
{
  Q_OBJECT

public:
  RedoAction ( Command* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
  ~RedoAction();
};

// --------------------------------------------------------------------

/**
 * Special action to show dock windows in a popup menu.
 * @author Werner Mayer
 */
class GuiExport DockWindowAction : public QAction
{
  Q_OBJECT

public:
  DockWindowAction ( QMainWindow::DockWindows, QObject * parent = 0, const char * name = 0 );
  virtual ~DockWindowAction();

  bool addTo ( QWidget * w );
  bool removeFrom ( QWidget * w );
  void setMenuText ( const QString & );

private:
  DockWindowActionP* d;
};

// --------------------------------------------------------------------

/**
 * Special action to show all dockable views -- except of toolbars -- in an own popup menu.
 * @author Werner Mayer
 */
class GuiExport DockViewAction : public DockWindowAction
{
  Q_OBJECT

public:
  DockViewAction ( QObject * parent = 0, const char * name = 0 );
  virtual ~DockViewAction();
};

// --------------------------------------------------------------------

/**
 * Special action to show all toolbars in an own popup menu.
 * @author Werner Mayer
 */
class GuiExport ToolBarAction : public DockWindowAction
{
  Q_OBJECT

public:
  ToolBarAction ( QObject * parent = 0, const char * name = 0 );
  virtual ~ToolBarAction();
};

// --------------------------------------------------------------------

/**
 * @author Werner Mayer
 */
class GuiExport WindowAction : public QAction
{
  Q_OBJECT

public:
  WindowAction ( QObject * parent = 0, const char * name = 0 );
  virtual ~WindowAction();

  bool addTo ( QWidget * w );
  bool removeFrom ( QWidget * w );
};

} // namespace Gui

#endif // __ACTION_H__

