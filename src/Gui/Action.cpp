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
#endif

#include "Action.h"
#include "Command.h"
#include "DlgUndoRedo.h"
#include "ToolBoxBar.h"
#include "WhatsThis.h"

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
Action::Action ( Command* pcCmd,QObject * parent, const char * name, bool toggle)
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
  if ( StdCmdDescription::inDescriptionMode () )
    StdCmdDescription::setSource( _pcCmd->getHelpUrl() );
  else
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
  QAction::setEnabled( b );
}

// --------------------------------------------------------------------

WorkbenchAction::WorkbenchAction (  StdCmdWorkbench* pcCmd, QObject * parent, const char * name )
  : QAction( parent, name, false ), _pcCmd(pcCmd)
{
  connect(this, SIGNAL(activated()), SLOT(onActivated()));
}

WorkbenchAction::~WorkbenchAction()
{
}

/** 
 * This slot is connected to the activated() signal and gets called whenever the user 
 * clicks a menu or combobox option or a toolbar button or presses an action's accelerator 
 * key combination.
 */
void WorkbenchAction::onActivated()
{
  _pcCmd->activate( text() );
}

// --------------------------------------------------------------------

WorkbenchGroup::WorkbenchGroup ( QObject * parent, const char * name, bool exclusive )
  :QActionGroup( parent, name, exclusive )
{
}

WorkbenchGroup::~WorkbenchGroup()
{ 
}

/**
 * If the workbench has changed this method notifies its actions to set
 * the active one if it is added to a combobox.
 */
void WorkbenchGroup::activate( const QString& name )
{
  const QObjectList *l = children();
  if ( l )
  {
    QObjectListIt it(*l);
    QObject * obj;
    while ( (obj=it.current()) != 0 ) 
    {
      QAction* act = dynamic_cast<QAction*>(obj);
      if ( act && act->text() == name )
      {
        act->setOn( true );
        break;
      }
      ++it;
    }
  }
}

// --------------------------------------------------------------------

MRUAction::MRUAction ( QObject * parent, const char * name, bool toggle )
  : QAction( parent, name, toggle ), _idx(-1)
{
}

MRUAction::~MRUAction()
{
}

/**
 * Sets the index for this action to \a index. \a index + 1 is the number that 
 * is prepended to the "menuText" property ofthis action. 
 */
void MRUAction::addedTo ( int index, QPopupMenu * menu )
{
  _idx = index;
  QString item = recentFileItem( text() );
  item.prepend( QString("&%1 ").arg( _idx + 1 ) );
  setMenuText( item );
}

/**
 * Returns the index number.
 */
int MRUAction::index() const
{
  return _idx;
}

/**
 * Returns the file name of \a fn if the \a fn is in the current
 * working directory, otherwise \a fn is returned.
 */
QString MRUAction::recentFileItem( const QString& fn )
{
  QFileInfo fi(fn);
  QString dir = fi.dirPath( true );
  QString cur = QDir::currentDirPath();
  if ( dir == cur )
    return fi.fileName();
  else
    return fn;
}

// --------------------------------------------------------------------

MRUActionGroup::MRUActionGroup ( Command* pcCmd, QObject * parent, const char * name, bool exclusive )
  :QActionGroup( parent, name, exclusive ), _pcCmd( pcCmd )
{
}

MRUActionGroup::~MRUActionGroup()
{ 
}

/**
 * Set the list of recent files. For each item an action object is
 * created and added to this action group. 
 */
void MRUActionGroup::setRecentFiles( const QStringList& files )
{
  clear();

  for ( QStringList::ConstIterator it = files.begin(); it != files.end(); it++ )
  {
    // This is a trick to fool Qt's QAction handling.
    // To the QAction constructor a null pointer is given to delay the addition to a QActionGroup.
    // Reason: If the parent QActionGroup is already added to a widget then it isn't possible to
    // get called our own addedTo() method but the QAction::addedTo() is called instead.
    MRUAction* action = new MRUAction( 0, *it );
    insertChild(action);
    action->setText( *it );
  
    connect( action, SIGNAL( activated() ), this, SLOT( onActivated() ));

    // now addedTo() is called whenever a new action is added to this action group
    add( action );
  }
}

void MRUActionGroup::clear()
{
  QObjectList *l = queryList( "QAction" );
  QObjectListIt it( *l );
  QObject *obj;
  while ( (obj = it.current()) != 0 ) 
  {
    disconnect( obj, SIGNAL( activated() ), this, SLOT( onActivated() ));
    removeChild( obj );
    delete obj;
    ++it;
  }

  delete l; // delete the list, not the objects
}

void MRUActionGroup::onActivated ()
{
  const QObject* o = sender();

  if ( o->inherits("Gui::MRUAction") )
  {
    MRUAction* act = (MRUAction*)o;
    if ( act )
      _pcCmd->activated( act->index() );
  }
}

// --------------------------------------------------------------------

UndoRedoAction::UndoRedoAction ( Command* pcCmd,QObject * parent, const char * name, bool toggle )
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

UndoAction::UndoAction ( Command* pcCmd,QObject * parent, const char * name, bool toggle )
  : UndoRedoAction(pcCmd, parent, name, toggle)
{
}

UndoAction::~UndoAction()
{
}

// --------------------------------------------------------------------

RedoAction::RedoAction ( Command* pcCmd,QObject * parent, const char * name, bool toggle )
  : UndoRedoAction(pcCmd, parent, name, toggle)
{
}

RedoAction::~RedoAction()
{
}

#include "moc_Action.cpp"
