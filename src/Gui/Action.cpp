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

#include "Action.h"
#include "Application.h"
#include "Command.h"
#include "DlgUndoRedo.h"
#include "MainWindow.h"
#include "WhatsThis.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

#include <Base/Exception.h>

using namespace Gui;
using namespace Gui::Dialog;

/**
 * Constructs an action called \a name with parent \a parent. It also stores a pointer
 * to the command object.
 */
Action::Action ( Command* pcCmd,QObject * parent )
  : QObject(parent), _pcCmd(pcCmd), _action(0)
{
  _action = new QAction( this );
  connect(_action, SIGNAL(triggered(bool)), this, SLOT(onActivated()));
}

Action::~Action()
{ 
  delete _action;
}

/**
 * Adds this action to widget \a w.
 */
void Action::addTo(QWidget *w)
{
  w->addAction(_action);
}

/**
 * Activates the command.
 */
void Action::onActivated () 
{
  if ( StdCmdDescription::inDescriptionMode () )
    StdCmdDescription::setSource( _pcCmd->getHelpUrl() );
  else
    _pcCmd->invoke(0);
}

/**
 * Sets whether the command is toggled.
 */
void Action::onToggled ( bool b)
{
  _pcCmd->invoke( b ? 1 : 0 );
} 

void Action::setCheckable ( bool b )
{
  _action->setCheckable(b);
  if ( b ) {
    disconnect(_action, SIGNAL(triggered(bool)), this, SLOT(onActivated()));
    connect(_action, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
  } else {
    connect(_action, SIGNAL(triggered(bool)), this, SLOT(onActivated()));
    disconnect(_action, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
  }
}

void Action::setChecked ( bool b )
{
  _action->setChecked(b);
}

bool Action::isChecked() const
{
  return _action->isChecked();
}

/**
 * Sets whether the action is enabled.
 */
void Action::setEnabled ( bool b) 
{
  _action->setEnabled(b);
}

void Action::setVisible ( bool b) 
{
  _action->setVisible( b );
}

void Action::setShortcut ( const QKeySequence & key )
{
  _action->setShortcut(key);
}

QKeySequence Action::shortcut() const
{
  return _action->shortcut();
}

void Action::setIcon ( const QIcon & icon)
{
  _action->setIcon(icon);
}

void Action::setStatusTip ( const QString & s)
{
  _action->setStatusTip(s);
}

QString Action::statusTip() const
{
  return _action->statusTip();
}

void Action::setText ( const QString & s)
{
  _action->setText(s);
}

QString Action::text() const
{
  return _action->text();
}

void Action::setToolTip ( const QString & s)
{
  _action->setToolTip(s);
}
  
QString Action::toolTip() const
{
  return _action->toolTip();
}

void Action::setWhatsThis ( const QString & s)
{
  _action->setWhatsThis(s);
}

QString Action::whatsThis() const
{
  return _action->whatsThis();
}

// --------------------------------------------------------------------

/**
 * Constructs an action called \a name with parent \a parent. It also stores a pointer
 * to the command object.
 */
ActionGroup::ActionGroup ( Command* pcCmd,QObject * parent)
  : Action(pcCmd, parent), _group(0), _dropDown(false)
{
  _group = new QActionGroup( this );
  connect(_group, SIGNAL(triggered(QAction*)), this, SLOT(onActivated (QAction*)));
}

ActionGroup::~ActionGroup()
{ 
  delete _group;
}

/**
 * Adds this action to widget \a w.
 */
void ActionGroup::addTo(QWidget *w)
{
  // When adding an action that has defined a menu then shortcuts
  // of the menu actions don't work. To make this working we must 
  // set the menu explicitly. This means calling QAction::setMenu()
  // and adding this action to the widget doesn't work.
  if ( _dropDown ) {
    if (w->inherits("QMenu")) {
      QMenu* menu = qobject_cast<QMenu*>(w);
      menu = menu->addMenu(_action->menuText());
      menu->addActions(_group->actions());
    } else if (w->inherits("QToolBar")) {
      w->addAction(_action);
      QToolButton* tb = w->findChildren<QToolButton*>().last();
      tb->setPopupMode(QToolButton::MenuButtonPopup);
      tb->addActions(_group->actions());
    } else {
      w->addActions(_group->actions()); // no drop-down 
    }
  } else {
    w->addActions(_group->actions());
  }
}

void ActionGroup::setEnabled( bool b )
{
  Action::setEnabled(b);
  _group->setEnabled(b);
}

void ActionGroup::setVisible( bool b )
{
  Action::setVisible(b);
  _group->setVisible(b);
}

QAction* ActionGroup::addAction(const QString& text)
{
  int index = _group->actions().size();
  QAction* action = _group->addAction(text);
  action->setData(QVariant(index));
  return action;
}

QList<QAction*> ActionGroup::actions() const
{
  return _group->actions();
}

int ActionGroup::checkedAction() const
{
  QAction* checked = _group->checkedAction();
  return checked ? checked->data().toInt() : -1;
}

void ActionGroup::setCheckedAction(int i)
{
  _group->actions()[i]->setChecked(true);
}

/**
 * Activates the command.
 */
void ActionGroup::onActivated (QAction* a) 
{
  if ( StdCmdDescription::inDescriptionMode () )
    StdCmdDescription::setSource( _pcCmd->getHelpUrl() );
  else
    _pcCmd->invoke(a->data().toInt());
}

// --------------------------------------------------------------------

WorkbenchComboBox::WorkbenchComboBox(QWidget* parent) : QComboBox(parent)
{
  connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
}

WorkbenchComboBox::~WorkbenchComboBox()
{
}

void WorkbenchComboBox::actionEvent ( QActionEvent* e )
{
  QAction *action = e->action();
  switch (e->type()) {
  case QEvent::ActionAdded:
    {
      if (action->isVisible()) {
        QIcon icon = action->icon();
        if (icon.isNull())
          this->addItem(action->text(), action->data());
        else
          this->addItem(icon, action->text(), action->data());
        if (action->isChecked())
          this->setCurrentIndex(action->data().toInt());
      }
      break;
    }
  case QEvent::ActionChanged:
    {
      //TODO
      break;
    }
  case QEvent::ActionRemoved:
    {
      //TODO
      break;
    }
  default:
    break;
  }
}

void WorkbenchComboBox::onActivated(int i)
{
  this->actions()[i]->trigger();
}

void WorkbenchComboBox::onActivated(QAction* a)
{
  setCurrentItem(a->data().toInt());
}

WorkbenchGroup::WorkbenchGroup (  Command* pcCmd, QObject * parent )
  : ActionGroup( pcCmd, parent )
{
  for (int i=0; i<50; i++) {
    QAction* action = _group->addAction("");
    action->setVisible(false);
    action->setCheckable(true);
  }
}

WorkbenchGroup::~WorkbenchGroup()
{
}

void WorkbenchGroup::addTo(QWidget *w)
{
  refreshWorkbenchList();
  if (w->inherits("QToolBar"))
  {
    QToolBar* bar = qobject_cast<QToolBar*>(w);
    QComboBox* box = new WorkbenchComboBox(w);
    box->setToolTip(_action->toolTip());
    box->setStatusTip(_action->statusTip());
    box->setWhatsThis(_action->whatsThis());
    box->addActions(_group->actions());
    connect(_group, SIGNAL(triggered(QAction*)), box, SLOT(onActivated (QAction*)));
    bar->addWidget(box);
  }
  else if (w->inherits("QMenu"))
  {
    QMenu* menu = qobject_cast<QMenu*>(w);
    menu = menu->addMenu(_action->menuText());
    menu->addActions(_group->actions());
  }
}

void WorkbenchGroup::refreshWorkbenchList()
{
  QString active = WorkbenchManager::instance()->active()->name();
  QStringList items = Application::Instance->workbenches();
  items.sort();

  QList<QAction*> workbenches = _group->actions();
  int numWorkbenches = std::min<int>(workbenches.count(), items.count());
  for ( int index = 0; index < numWorkbenches; index++ ) {
    QPixmap px = Application::Instance->workbenchIcon( items[index] );
    workbenches[index]->setIcon(px);
    workbenches[index]->setText(items[index]);
    workbenches[index]->setStatusTip(tr("Select the '%1' workbench").arg(items[index]));
    workbenches[index]->setData(QVariant(index));
    workbenches[index]->setVisible(true);
    workbenches[index]->setCheckable(true);
    if ( items[index] == active )
    workbenches[index]->setChecked(true);
  }

  // if less workbenches than actions
  for (int index = numWorkbenches; index < workbenches.count(); index++)
    workbenches[index]->setVisible(false);
}

// --------------------------------------------------------------------

RecentFilesAction::RecentFilesAction ( Command* pcCmd, int maxRecentFiles, QObject * parent )
  : ActionGroup( pcCmd, parent )
{
  for (int i=0; i<maxRecentFiles; i++) {
    _group->addAction("")->setVisible(false);
  }
}

RecentFilesAction::~RecentFilesAction()
{
}

/**
 * Set the list of recent files. For each item an action object is
 * created and added to this action group. 
 */
void RecentFilesAction::setRecentFiles( const QStringList& files )
{
  QList<QAction*> recentFiles = _group->actions();

  int numRecentFiles = std::min<int>(recentFiles.count(), files.count());
  for ( int index = 0; index < numRecentFiles; index++ ) {
    QFileInfo fi(files[index]);
    recentFiles[index]->setText(QString("&%1 %2").arg(index+1).arg(fi.fileName()));
    recentFiles[index]->setStatusTip(tr("Open file %1").arg(files[index]));
    recentFiles[index]->setData(QVariant(index));
    recentFiles[index]->setVisible(true);
  }

  // if less file names than actions
  for (int index = numRecentFiles; index < recentFiles.count(); index++)
    recentFiles[index]->setVisible(false);
}

// --------------------------------------------------------------------

UndoAction::UndoAction ( Command* pcCmd,QObject * parent )
  : Action(pcCmd, parent)
{
  _toolAction = new QAction(this);
  _toolAction->setMenu(new UndoDialog());
  connect(_toolAction, SIGNAL(triggered(bool)), this, SLOT(onActivated()));
}

UndoAction::~UndoAction()
{
  QMenu* menu = _toolAction->menu();
  delete menu;
  delete _toolAction;
}

void UndoAction::addTo ( QWidget * w )
{
  if (w->inherits("QToolBar")) {
    _toolAction->setText(_action->text());
    _toolAction->setToolTip(_action->toolTip());
    _toolAction->setStatusTip(_action->statusTip());
    _toolAction->setWhatsThis(_action->whatsThis());
    _toolAction->setShortcut(_action->shortcut());
    _toolAction->setIcon(_action->icon());
    w->addAction(_toolAction);
  } else {
    w->addAction(_action);
  }
}

void UndoAction::setEnabled  ( bool b )
{
  Action::setEnabled(b);
  _toolAction->setEnabled(b);
}

void UndoAction::setVisible ( bool b )
{
  Action::setVisible(b);
  _toolAction->setVisible(b);
}

// --------------------------------------------------------------------

RedoAction::RedoAction ( Command* pcCmd,QObject * parent )
  : Action(pcCmd, parent)
{
  _toolAction = new QAction(this);
  _toolAction->setMenu(new RedoDialog());
  connect(_toolAction, SIGNAL(triggered(bool)), this, SLOT(onActivated()));
}

RedoAction::~RedoAction()
{
  QMenu* menu = _toolAction->menu();
  delete menu;
  delete _toolAction;
}

void RedoAction::addTo ( QWidget * w )
{
  if (w->inherits("QToolBar")) {
    _toolAction->setText(_action->text());
    _toolAction->setToolTip(_action->toolTip());
    _toolAction->setStatusTip(_action->statusTip());
    _toolAction->setWhatsThis(_action->whatsThis());
    _toolAction->setShortcut(_action->shortcut());
    _toolAction->setIcon(_action->icon());
    w->addAction(_toolAction);
  } else {
    w->addAction(_action);
  }
}

void RedoAction::setEnabled  ( bool b )
{
  Action::setEnabled(b);
  _toolAction->setEnabled(b);
}

void RedoAction::setVisible ( bool b )
{
  Action::setVisible(b);
  _toolAction->setVisible(b);
}

// --------------------------------------------------------------------

DockWidgetAction::DockWidgetAction ( Command* pcCmd, QObject * parent )
  : Action(pcCmd, parent), _menu(0)
{
}

DockWidgetAction::~DockWidgetAction()
{
  delete _menu;
}

void DockWidgetAction::addTo ( QWidget * w )
{
  if (!_menu) {
    _menu = new QMenu();
    _action->setMenu(_menu);
  }

  w->addAction(_action);

  QList<QDockWidget*> dock = getMainWindow()->findChildren<QDockWidget*>();
  for (QList<QDockWidget*>::Iterator it = dock.begin(); it != dock.end(); ++it) {
    QAction* action = (*it)->toggleViewAction();
    action->setToolTip(tr("Toogles this dockable window"));
    action->setStatusTip(tr("Toogles this dockable window"));
    action->setWhatsThis(tr("Toogles this dockable window"));
    _menu->addAction(action);
  }
}

// --------------------------------------------------------------------

ToolBarAction::ToolBarAction ( Command* pcCmd, QObject * parent )
  : Action(pcCmd, parent), _menu(0)
{
}

ToolBarAction::~ToolBarAction()
{
  delete _menu;
}

void ToolBarAction::addTo ( QWidget * w )
{
  if (!_menu) {
    _menu = new QMenu();
    _action->setMenu(_menu);
  }

  w->addAction(_action);

  QWidget* mw = getMainWindow();
  QList<QToolBar*> dock = mw->findChildren<QToolBar*>();
  for (QList<QToolBar*>::Iterator it = dock.begin(); it != dock.end(); ++it) {
    if ((*it)->parentWidget() == mw) {
      QAction* action = (*it)->toggleViewAction();
      action->setToolTip(tr("Toogles this toolbar"));
      action->setStatusTip(tr("Toogles this toolbar"));
      action->setWhatsThis(tr("Toogles this toolbar"));
      _menu->addAction(action);
    }
  }
}

// --------------------------------------------------------------------

WindowAction::WindowAction ( Command* pcCmd, QObject * parent )
  : ActionGroup(pcCmd, parent), _menu(0)
{
}

WindowAction::~WindowAction()
{
}

void WindowAction::addTo ( QWidget * w )
{
  QMenu* menu = qobject_cast<QMenu*>(w);
  if ( !menu ) {
    if (!_menu) {
      _menu = new QMenu();
      _action->setMenu(_menu);
      _menu->addActions(_group->actions());
      connect( _menu, SIGNAL( aboutToShow()), getMainWindow(), SLOT( onWindowsMenuAboutToShow() ) );
    }
    
    w->addAction(_action);
  } else {
    menu->addActions(_group->actions());
    connect( menu, SIGNAL( aboutToShow()), getMainWindow(), SLOT( onWindowsMenuAboutToShow() ) );
  }
}

#include "moc_Action.cpp"
