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
#include "BitmapFactory.h"
#include "Command.h"
#include "DlgUndoRedo.h"
#include "HtmlView.h"
#include "ToolBoxBar.h"

using namespace Gui;
using namespace Gui::Dialog;
using namespace Gui::DockWnd;

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

/**
 * Adds this action to widget \a w.
 */
bool Action::addTo(QWidget *w)
{
  if (QAction::addTo(w) == true)
  {
    // connect html help with widget
    const QObjectList *l = w->children();
    if(l!=0)
    {
      QObjectListIt it(*l);
      QObject* o = it.toLast();
      if (o && o->isWidgetType())
      {
        FCWhatsThis::add((QWidget*)o, whatsThis());
      }
    }

    if (w->inherits("Gui::DockWnd::CommandBar"))
    {
      ((CommandBar*)w)->setTextToLastItem(menuText());
    }
  }
  else
    return false;

  return true;
}

/**
 * Removes the action from widget \a w.
 */
bool Action::removeFrom ( QWidget * w )
{
  QWidget* o;
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!=widgets.end(); ++it)
  {
    if ((*it)->parentWidget() == w)
    {
      o = *it;
      widgets.erase(it);
      disconnect( o, SIGNAL( destroyed() ), this, SLOT( onDestroyed() ) );
      delete o;
    }
  }

  return QAction::removeFrom(w);
}

/** 
 * Sets whether a toggle action is on but only if this action is a toggle action.
 */
void Action::onToolButtonToggled( bool on )
{
  if ( !isToggleAction() )
    return;
  setOn( on );
}

/**
 * Prints the text \a text on the status bar.
 */
void Action::onShowStatusText( const QString& text )
{
  QObject* par;
  if ( ( par = parent() ) && par->inherits( "QActionGroup" ) )
    par = par->parent();

  if ( !par || !par->isWidgetType() )
    return;

  QObjectList* l = ( (QWidget*)par )->topLevelWidget()->queryList("QStatusBar");
  for ( QStatusBar* bar = (QStatusBar*) l->first(); bar; bar = (QStatusBar*)l->next() ) 
  {
    if ( text.isEmpty() )
      bar->clear();
    else
      bar->message( text );
  }

  delete l;
}

/**
 * Clears the text on the status bar.
 */
void Action::onClearStatusText()
{
  onShowStatusText( QString::null );

}

/**
 * Removes the just destroyed widget.
 */
void Action::onDestroyed()
{
  const QWidget* w = (QWidget*)sender();
  std::vector<QWidget*>::iterator it;
  if ((it = std::find(widgets.begin(), widgets.end(), w)) != widgets.end())
    widgets.erase(it);
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
  // update all widgets containing this action
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!=widgets.end(); ++it)
  {
    (*it)->setEnabled(b);
  }
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

UndoAction::UndoAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle )
  : Action(pcCmd, parent, name, toggle)
{
  tipGroup = new QToolTipGroup(0);
}

bool UndoAction::addTo(QWidget* w)
{
  if (w->inherits("QToolBar"))
  {
    QWidget* dlg = ((FCCmdUndo*)GetCommand())->GetWidget();

    QToolButton* button = new ToolButtonDropDown((QToolBar*)w, iconSet().pixmap(), dlg);
    button->setToggleButton( isToggleAction() );
    button->setIconSet( iconSet() );

    // do this before the tool tip stuff
    if (w->inherits("Gui::DockWnd::CommandBar"))
    {
      ((CommandBar*)w)->setTextToLastItem(menuText());
      button->setTextLabel(menuText());
      button->setUsesTextLabel(true);
    }

    QToolTip::add( button, toolTip(), tipGroup, statusTip() );
    QWhatsThis::add(button, whatsThis());
    FCWhatsThis::add(button, whatsThis());

    connect( button,   SIGNAL( destroyed() ),             this, SLOT( onDestroyed() ) );
    connect( button,   SIGNAL( clicked() ),               this, SIGNAL( activated() ) );
    connect( button,   SIGNAL( toggled(bool) ),           this, SLOT( onToolButtonToggled(bool) ) );
    connect( tipGroup, SIGNAL( showTip(const QString&) ), this, SLOT(onShowStatusText(const QString&)) );
    connect( tipGroup, SIGNAL( removeTip() ),             this, SLOT(onClearStatusText()) );

    widgets.push_back(button);

    return true;
  }

  return Action::addTo(w);
}

// --------------------------------------------------------------------

RedoAction::RedoAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle )
  : Action(pcCmd, parent, name, toggle)
{
  tipGroup = new QToolTipGroup(0);
}

bool RedoAction::addTo(QWidget* w)
{
  if (w->inherits("QToolBar"))
  {
    QWidget* dlg = ((FCCmdRedo*)GetCommand())->GetWidget();

    QToolButton* button = new ToolButtonDropDown((QToolBar*)w, iconSet().pixmap(), dlg);
    button->setToggleButton( isToggleAction() );
    button->setIconSet( iconSet() );

    // do this before the tool tip stuff
    if (w->inherits("Gui::DockWnd::CommandBar"))
    {
      ((CommandBar*)w)->setTextToLastItem(menuText());
      button->setTextLabel(menuText());
      button->setUsesTextLabel(true);
    }

    QToolTip::add( button, toolTip(), tipGroup, statusTip() );
    QWhatsThis::add(button, whatsThis());
    FCWhatsThis::add(button, whatsThis());

    connect( button,   SIGNAL( destroyed() ),             this, SLOT( onDestroyed() ) );
    connect( button,   SIGNAL( clicked() ),               this, SIGNAL( activated() ) );
    connect( button,   SIGNAL( toggled(bool) ),           this, SLOT( onToolButtonToggled(bool) ) );
    connect( tipGroup, SIGNAL( showTip(const QString&) ), this, SLOT(onShowStatusText(const QString&)) );
    connect( tipGroup, SIGNAL( removeTip() ),             this, SLOT(onClearStatusText()) );
    
    widgets.push_back(button);

    return true;
  }

  return Action::addTo(w);
}

#include "moc_Action.cpp"
