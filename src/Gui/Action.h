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

#ifndef _PreComp_
# include <qaction.h>
# include <qdragobject.h>
# include <qtooltip.h>
#endif

class FCCommand;

namespace Gui 
{

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

/**
 * Allows actions to be added to widgets other than toolbars or popup menus.
 * @author Werner Mayer
 */
class GuiExport Action : public QAction
{
  Q_OBJECT

public:
  Action ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
  virtual ~Action();

  virtual void addedTo ( QWidget * actionWidget, QWidget * container );
  virtual bool addTo ( QWidget * w );
  virtual bool removeFrom ( QWidget * w );
  
  void setEnabled  ( bool ) ;

  FCCommand* GetCommand() { return _pcCmd; }

public slots:
  void onActivated ();
  void onToggled   ( bool ); 

private:
  FCCommand *_pcCmd;
};

// --------------------------------------------------------------------

/**
 * This allows several actions to be added to a toolbar or menu.
 * For menus a submenu is created for toolbars a combo box.
 * @author Werner Mayer
 */
class GuiExport ActionGroup : public QActionGroup
{
  Q_OBJECT

public:
  ActionGroup ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool exclusive = TRUE );
  virtual ~ActionGroup();

  virtual bool addTo( QWidget * w );

  void addAction( QAction* act );
  void removeAction ( QAction* act );
  void clear();

  FCCommand* GetCommand() { return _pcCmd; }
  
  void activate( const QString& item );
  void activate( int id);

protected:
  virtual void addedTo ( QWidget * actionWidget, QWidget * container, QAction * a );

public slots:
  void onActivated ( int i );
  void onActivated ( QAction* );

private slots:
  void onActivated ();

private:
  FCCommand *_pcCmd;
};

// --------------------------------------------------------------------

/**
 * The UndoRedoAction class reimplements the addedTo() function to make a popup menu 
 * appearing when the button with the arrow is clicked.
 * \author Werner Mayer
 */
class GuiExport UndoRedoAction : public Action
{
  Q_OBJECT

public:
  UndoRedoAction ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
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
  UndoAction ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
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
  RedoAction ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
  ~RedoAction();
};

} // namespace Gui

#endif // __ACTION_H__

