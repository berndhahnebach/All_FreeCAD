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

ActionGroup::ActionGroup ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle)
:Action(pcCmd, parent, name, toggle)
{
}

ActionGroup::~ActionGroup()
{ 
}

/**
 * Adds this action group to widget \a w. 
 * An action group added to a tool bar is automatically displayed as a combo box; 
 * an action added to a pop up menu appears as a submenu.
 */
bool ActionGroup::addTo(QWidget *w)
{
  if (w->inherits("QToolBar"))
  {
    QComboBox* combo = new QComboBox(w, "Combo");
    widgets.push_back(combo);
    connect( combo, SIGNAL( destroyed() ),     this, SLOT( onDestroyed()    ) );
    connect( combo, SIGNAL(  activated(int) ), this, SLOT( onActivated(int) ) );
    combo->setMinimumWidth(130);
    QPixmap FCIcon = Gui::BitmapFactory().GetPixmap("FCIcon");
    for (std::vector<std::string>::iterator it = mItems.begin(); it!=mItems.end(); ++it)
    {
      combo->insertItem(FCIcon, it->c_str());
    }

    if (w->inherits("Gui::DockWnd::CommandBar"))
    {
      ((CommandBar*)w)->setTextToLastItem(menuText());
    }
  }
  else if (w->inherits("QPopupMenu"))
  {
    QPopupMenu* popup = new QPopupMenu(w, "Menu");
    widgets.push_back(popup);
    connect( popup, SIGNAL( destroyed() ),     this, SLOT( onDestroyed  ()   ) );
    connect( popup, SIGNAL( aboutToShow() ),   this, SLOT( onAboutToShow()   ) );
    connect( popup, SIGNAL(  activated(int) ), this, SLOT( onActivated  (int)) );

//    if (iconSet().isNull())
      ((QPopupMenu*)w)->insertItem(text(), popup);
//    else
//      ((QPopupMenu*)w)->insertItem(iconSet().pixmap(), mName.c_str(), popup);
  }
  else
    return false;

  return true;
}

/**
 * Fills up the menu.
 */
void ActionGroup::onAboutToShow()
{
  // fill up just before showing the menu 
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QPopupMenu"))
    {
      QPopupMenu* popup = (QPopupMenu*)(*it);
      popup->clear();

      int i=0;
      for (std::vector<std::string>::iterator it = mItems.begin(); it!=mItems.end(); ++it, i++)
      {
        popup->insertItem(/*QPixmap(FCIcon), */it->c_str(), i);
      }
    }
  }
}

void ActionGroup::setItems(const std::vector<std::string>& items)
{
  mItems = items;
}

void ActionGroup::insertItem(const char* item)
{
  mItems.push_back(item);
  QPixmap FCIcon = Gui::BitmapFactory().GetPixmap("FCIcon");
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      combo->insertItem(FCIcon, item);
    }
  }
}

void ActionGroup::removeItem(const char* item)
{
  std::vector<std::string>::iterator ii = std::find(mItems.begin(), mItems.end(), std::string(item));
  if (ii != mItems.end()) mItems.erase(ii);

  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      for (int i = 0; i<combo->count(); i++)
      {
        if (combo->text(i) == QString(item))
        {
          combo->removeItem(i);
          break;
        }
      }
    }
  }
}

void ActionGroup::clear()
{
  mItems.clear();
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      combo->clear();
    }
  }
}

void ActionGroup::activate(int i)
{
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      if (combo->currentItem() != i)
        combo->setCurrentItem(i);
    }
  }
}

void ActionGroup::activate(const QString& name)
{
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      if (combo->currentText() != name)
      {
        for(int i=0;i<combo->count();i++)
        {
          if (combo->text(i) == name)
          {
            combo->setCurrentItem(i);
            break;
          }
        }
      }
    }
  }
}

void ActionGroup::onActivated (int i)
{
  GetCommand()->Activated(i);
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
