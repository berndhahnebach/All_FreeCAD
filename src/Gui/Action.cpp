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
# include <qobjectlist.h>
# include <qstatusbar.h>
# include <qwhatsthis.h>
#endif

#include "Action.h"
#include "Command.h"
#include "DlgUndoRedo.h"
#include "ToolBoxBar.h"

using namespace Gui;
using namespace Gui::Dialog;

QStringList ActionDrag::actions;

ActionDrag::ActionDrag ( QString action, QWidget * dragSource , const char * name  )
  : QStoredDrag("Gui::ActionDrag", dragSource, name)
{
  // store the QAction name
  actions.push_back(action);
}

ActionDrag::~ActionDrag ()
{
}

bool ActionDrag::canDecode ( const QMimeSource * e )
{
  return e->provides( "Gui::ActionDrag" );
}

bool ActionDrag::decode ( const QMimeSource * e, QString&  action )
{
  if (actions.size() > 0)
  {
    action = *actions.begin();
    return true;
  }

  return false;
}

// --------------------------------------------------------------------

/**
 * Constructs an action called \a name with parent \a parent. It also stores a pointer
 * to the command object.
 */
Action::Action ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle)
:QAction(parent, name, toggle),_pcCmd(pcCmd)
{
  connect( this, SIGNAL( activated() ) ,  this, SLOT( onActivated()   ) );
  connect( this, SIGNAL( toggled(bool) ), this, SLOT( onToggled(bool) ) );
}

Action::~Action()
{ 
}

void Action::addedTo ( QWidget * actionWidget, QWidget * container )
{
  if ( container->inherits("Gui::DockWnd::CommandBar") )
  {
    QToolButton* tb = (QToolButton*) actionWidget;
    tb->setTextPosition( QToolButton::BesideIcon );
    tb->setTextLabel( menuText() );
    tb->setUsesTextLabel( true );
    tb->setFixedHeight( 30 );

    ((Gui::DockWnd::CommandBar*)container)->setDummyToLastItem();
  }
}

/**
 * Adds this action to widget \a w.
 */
bool Action::addTo(QWidget *w)
{
  return QAction::addTo( w );
}

/**
 * Removes the action from widget \a w.
 */
bool Action::removeFrom ( QWidget * w )
{
  return QAction::removeFrom( w );
}

/**
 * Activates the command.
 */
void Action::onActivated () 
{
  _pcCmd->activated();
}

/**
 * Sets whether the command is toggled.
 */
void Action::onToggled ( bool b)
{
  _pcCmd->toggled(b);
} 

/**
 * Sets whether the action is enabled.
 */
void Action::setEnabled ( bool b) 
{
  QAction::setEnabled(b);
}

// --------------------------------------------------------------------

ActionGroup::ActionGroup ( FCCommand* pcCmd,QObject * parent, const char * name, bool exclusive )
:QActionGroup( parent, name, exclusive ), _pcCmd( pcCmd )
{
  connect( this, SIGNAL( selected(QAction*) ), this, SLOT( onActivated(QAction*) ) );
}

ActionGroup::~ActionGroup()
{ 
}

/**
 * Adds this action group to widget \a w. 
 * If isExclusive() is FALSE or usesDropDown() is FALSE, the actions within the group are added 
 * to the widget individually. For example, if the widget is a menu, the actions will appear as 
 * individual menu options, and if the widget is a toolbar, the actions will appear as toolbar buttons.
 *
 * If both isExclusive() and usesDropDown() are TRUE, the actions are presented either in a combobox 
 * (if \a w is a toolbar) or in a submenu (if \a w is a menu).
 *
 * All actions should be added to the action group \a before the action group is added to the widget. 
 * If actions are added to the action group \a after the action group has been added to the widget 
 * these later actions will \a not appear.  
 */
bool ActionGroup::addTo( QWidget *w )
{
  return QActionGroup::addTo( w );
}

void ActionGroup::addedTo ( QWidget * actionWidget, QWidget * container, QAction * a )
{
}

void ActionGroup::addAction( QAction* act )
{
  if ( isExclusive() == false )
    connect( act, SIGNAL( activated() ), this, SLOT( onActivated() ));
  add( act );
}

void ActionGroup::removeAction ( QAction* act )
{
  const QObjectList *l = children();

  if ( l )
  {
    QObjectListIt it(*l);
    QObject * obj;
    while ( (obj=it.current()) != 0 ) 
    {
      if ( obj->inherits("QAction") )
      {
        if ( obj == act )
        {
          if ( isExclusive() == false )
            disconnect( act, SIGNAL( activated() ), this, SLOT( onActivated() ));
          removeChild( act );
          delete act;
          break;
        }
      }

      ++it;
    }
  }
}

void ActionGroup::clear()
{
  QObjectList *l = queryList( "QAction" );
  QObjectListIt it( *l );
  QObject *obj;
  while ( (obj = it.current()) != 0 ) 
  {
    if ( isExclusive() == false )
      disconnect( obj, SIGNAL( activated() ), this, SLOT( onActivated() ));
    removeChild( obj );
    delete obj;
    ++it;
  }

  delete l; // delete the list, not the objects
}

void ActionGroup::activate( int id)
{
  int pos = 0;
  const QObjectList *l = children();
  if ( l )
  {
    QObjectListIt it(*l);
    QObject * obj;
    while ( (obj=it.current()) != 0 ) 
    {
      QAction* act = dynamic_cast<QAction*>(obj);
      if ( act )
      {
        if ( pos == id )
        {
          act->setOn( true );
          break;
        }
      }

      ++it;
      ++pos;
    }
  }
}

void ActionGroup::activate( const QString& name )
{
  const QObjectList *l = children();
  if ( l )
  {
    QObjectListIt it(*l);
    QObject * obj;
    while ( (obj=it.current()) != 0 ) 
    {
      QAction* act = dynamic_cast<QAction*>(obj);
      if ( act )
      {
        if ( act->text() == name )
        {
          act->setOn( true );
          break;
        }
      }

      ++it;
    }
  }
}

void ActionGroup::onActivated ()
{
  if ( isExclusive() == false )
  {
    const QObject* o = sender();
    if ( o && o->inherits("QAction") )
      onActivated( (QAction*)o );
  }
}

void ActionGroup::onActivated (int i)
{
  GetCommand()->Activated(i);
}

void ActionGroup::onActivated ( QAction* act )
{
  int id=0;
  const QObjectList *l = children();

  if ( l )
  {
    QObjectListIt it(*l);
    QObject * obj;
    while ( (obj=it.current()) != 0 ) 
    {
      if ( obj->inherits("QAction") )
      {
        if ( obj == act )
        {
          GetCommand()->Activated( id );
          break;
        }

        id++;
      }

      ++it;
    }
  }
}

// --------------------------------------------------------------------

UndoRedoAction::UndoRedoAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle )
  : Action(pcCmd, parent, name, toggle)
{
}

UndoRedoAction::~UndoRedoAction()
{
}

void UndoRedoAction::addedTo ( QWidget * actionWidget, QWidget * container )
{
  if ( actionWidget->inherits("QToolButton") )
  {
    QToolButton* tb = (QToolButton*)actionWidget;
    QPopupMenu* menu = new UndoRedoDialog( tb );

    tb->setPopup( menu );
    tb->setPopupDelay(0);
    tb->setAutoRaise(true);
  }

  Action::addedTo( actionWidget, container );
}

// --------------------------------------------------------------------

UndoAction::UndoAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle )
  : UndoRedoAction(pcCmd, parent, name, toggle)
{
}

UndoAction::~UndoAction()
{
}

// --------------------------------------------------------------------

RedoAction::RedoAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle )
  : UndoRedoAction(pcCmd, parent, name, toggle)
{
}

RedoAction::~RedoAction()
{
}

#include "moc_Action.cpp"
