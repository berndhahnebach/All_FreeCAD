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
# include <qinputdialog.h>
# include <qobjectlist.h>
# include <qpushbutton.h>
# include <vector>
#endif

#include "DlgToolbarsImp.h"
#include "Application.h"
#include "Tools.h"
#include "Command.h"
#include "CustomWidgets.h"
#include "ToolBarManager.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgCustomToolbars which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomToolbars::DlgCustomToolbars( QWidget* parent, const char* name, WFlags fl )
: DlgCustomToolbarsBase(parent, name, fl)
#ifdef NEW_WB_FRAMEWORK
  , _toolBars(0) 
#endif
{
  AvailableActions->setSorting( -1 );
  ToolbarActions->setSorting( -1 );

  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();
  std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();
  std::map<std::string, std::vector<Command*> > alCmdGroups;
  for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    alCmdGroups[it->second->getGroupName()].push_back(it->second);
  }
  for (std::map<std::string, std::vector<Command*> >::iterator it2 = alCmdGroups.begin(); it2 != alCmdGroups.end(); ++it2)
  {
    QListViewItem* itemNode = new QListViewItem(AvailableActions, it2->first.c_str());
    itemNode->setOpen(true);
    for (std::vector<Command*>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
    {
      QListViewItem* item = new QListViewItem(itemNode,AvailableActions->lastItem(), (*it3)->getAction()->menuText());
      QPixmap pix = (*it3)->getAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
      item->setPixmap(0, Tools::fillUp(24,24,pix));
      itemNode->insertItem(item);
    }

    AvailableActions->insertItem(itemNode);
  }

  AvailableActions->insertItem(new QListViewItem(AvailableActions, "<Separator>"));
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbars::~DlgCustomToolbars()
{
#ifdef NEW_WB_FRAMEWORK
  delete _toolBars; 
#endif
}

void DlgCustomToolbars::refreshActionList()
{
#ifdef NEW_WB_FRAMEWORK
  QString text = ComboToolbars->currentText();
  ToolBarItem* bar = _toolBars->findItem( text );
  bar->clear();

  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();

  QListViewItem* item = ToolbarActions->firstChild();
  for (int i=0; i < ToolbarActions->childCount(); item = item->itemBelow(), i++)
  {
    if (item->text(0) == "<Separator>")
    {
      *bar << "Separator";
    }
    else
    {
      Command* pCom = cCmdMgr.getCommandByActionText(item->text(0).latin1());
      if (pCom != 0)
      {
        *bar << pCom->getName();
      }
    }
  }
#endif
}

void DlgCustomToolbars::cancel()
{
}

void DlgCustomToolbars::refreshToolBarList()
{
#ifdef NEW_WB_FRAMEWORK
  ComboToolbars->clear();
  ToolbarActions->clear();

  QPtrList<ToolBarItem> bars = _toolBars->getItems();
  ToolBarItem* bar;
  for ( bar=bars.first(); bar; bar=bars.next() )
  {
    ComboToolbars->insertItem( bar->command() );
  }

  if (ComboToolbars->count() > 0)
  {
    onItemActivated( ComboToolbars->text( 0 ) );
  }
  else
  {
    ToolbarActions->setEnabled(false);
    ComboToolbars->setEnabled (false);
  }
#endif
}

/** Enables/disables buttons for change */
void DlgCustomToolbars::onAllActionsChanged( QListViewItem *i )
{
  bool canAdd = FALSE;
  QListViewItemIterator it = AvailableActions->firstChild();

  for ( ; it.current(); it++ ) 
  {
    if ( it.current()->isSelected() ) 
    {
      canAdd = TRUE;
      break;
    }
  }

  buttonRight->setEnabled( ( canAdd || ( i && i->isSelected() ) ) && ComboToolbars->isEnabled() );
}

/** Enables/disables buttons for change */
void DlgCustomToolbars::onNewActionChanged( QListViewItem *i )
{
  buttonUp->setEnabled( (bool) (i && i->itemAbove()) );
  buttonDown->setEnabled( (bool) (i && i->itemBelow()) );

  bool canRemove = FALSE;
  QListViewItemIterator it = ToolbarActions->firstChild();
  for ( ; it.current(); it++ ) 
  {
    if ( it.current()->isSelected() ) 
    {
      canRemove = TRUE;
      break;
    }
  }

  buttonLeft->setEnabled( canRemove || ( i && i->isSelected() ) );
}

/** Shows all buttons of the toolbar */
void DlgCustomToolbars::onItemActivated(const QString & name)
{
#ifndef NEW_WB_FRAMEWORK
  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();

  ToolbarActions->clear();
  Gui::CustomToolBar* it;
  for ( it = _aclToolbars.first(); it; it = _aclToolbars.next() )
  {
    if (it->name() == name)
    {
      QStringList items = it->getCustomItems();
      for ( QStringList::Iterator it2 = items.begin(); it2 != items.end(); ++it2 )
      {
        if (*it2 == "Separator")
        {
          ToolbarActions->insertItem(new QListViewItem(ToolbarActions,ToolbarActions->lastItem(), "<Separator>"));
        }
        else
        {
          Command* pCom = cCmdMgr.getCommandByName( (*it2).latin1() );
          if (pCom)
          {
            QListViewItem* item = new QListViewItem(ToolbarActions,ToolbarActions->lastItem(), pCom->getAction()->menuText());
            QPixmap pix = pCom->getAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
            item->setPixmap(0, Tools::fillUp(24,24,pix));
            ToolbarActions->insertItem(item);
          }
        }
      }
    }
  }
#else
  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();

  ToolbarActions->clear();
  ToolBarItem* bar;
  QPtrList<ToolBarItem> bars = _toolBars->getItems();
  for ( bar=bars.first(); bar; bar=bars.next() )
  {
    if ( bar->command() == name )
    {
      QPtrList<ToolBarItem> items = bar->getItems();
      ToolBarItem* item;
      for ( item=items.first(); item; item=items.next() )
      {
        if ( item->command() == "Separator" )
          ToolbarActions->insertItem(new QListViewItem(ToolbarActions,ToolbarActions->lastItem(), "<Separator>"));
        else
        {
          Command* pCom = cCmdMgr.getCommandByName( item->command().latin1() );
          if (pCom)
          {
            QListViewItem* item = new QListViewItem(ToolbarActions,ToolbarActions->lastItem(), pCom->getAction()->menuText());
            QPixmap pix = pCom->getAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
            item->setPixmap(0, Tools::fillUp(24,24,pix));
            ToolbarActions->insertItem(item);
          }
        }
      }
      break;
    }
  }
#endif
}

/** Adds a new action */
void DlgCustomToolbars::onAddAction()
{
  QListView *src = AvailableActions;

  bool addKids = FALSE;
  QListViewItem *nextSibling = 0;
  QListViewItem *nextParent = 0;
  QListViewItemIterator it = src->firstChild();
  for ( ; it.current(); it++ ) 
  {
    if ( it.current() == nextSibling )
     addKids = FALSE;

    if ( it.current()->isSelected() ) 
    {
      if ( it.current()->childCount() == 0 ) 
      {
        // Selected, no children
        QListViewItem *i = new QListViewItem( ToolbarActions, ToolbarActions->lastItem() );
        i->setText( 0, it.current()->text(0) );
        if (it.current()->pixmap(0) != NULL)
        {
          QPixmap pix = *(it.current()->pixmap(0));
          i->setPixmap( 0,  Tools::fillUp(24,24,pix));
        }
        ToolbarActions->setCurrentItem( i );
        ToolbarActions->ensureItemVisible( i );
      } 
      else if ( !addKids ) 
      {
        addKids = TRUE;
        nextSibling = it.current()->nextSibling();
        nextParent = it.current()->parent();
        while ( nextParent && !nextSibling ) 
        {
          nextSibling = nextParent->nextSibling();
          nextParent = nextParent->parent();
        }
      }
    } 
    else if ( (it.current()->childCount() == 0) && addKids ) 
    {
      // Leaf node, and we _do_ process children
      QListViewItem *i = new QListViewItem( ToolbarActions, ToolbarActions->lastItem() );
      i->setText( 0, it.current()->text(0) );
      if (it.current()->pixmap(0) != NULL)
      {
        QPixmap pix = *(it.current()->pixmap(0));
        i->setPixmap( 0, Tools::fillUp(24,24,pix) );
      }
      ToolbarActions->setCurrentItem( i );
      ToolbarActions->ensureItemVisible( i );
    }
  }

  refreshActionList();
}

/** Removes an action */
void DlgCustomToolbars::onRemoveAction()
{
  QListViewItemIterator it = ToolbarActions->firstChild();
  while ( it.current() ) 
  {
    if ( it.current()->isSelected() )
     delete it.current();
    else
     it++;
  }

  buttonLeft->setEnabled (ToolbarActions->childCount() > 0);

  refreshActionList();
}

/** Noves up an action */
void DlgCustomToolbars::onMoveUpAction()
{
  bool up = true;
  bool down = true;
  QListViewItem *next = 0;
  QListViewItem *item = ToolbarActions->firstChild();

  for ( int i = 0; i < ToolbarActions->childCount(); ++i ) 
  {
    next = item->itemBelow();
 
    if ( item->isSelected() && (i > 0) && !item->itemAbove()->isSelected() )
    {
      item->itemAbove()->moveItem( item );
      up   &= (item->itemAbove() != 0L);
      down &= (item->itemBelow() != 0L);
    }
    item = next;
  }

  buttonUp->setEnabled (up);
  buttonDown->setEnabled (down);

  refreshActionList();
}

/** Moves down an action */
void DlgCustomToolbars::onMoveDownAction()
{
  bool up = true;
  bool down = true;
  int count = ToolbarActions->childCount();
  QListViewItem *next = 0;
  QListViewItem *item = ToolbarActions->lastItem();

  for ( int i = 0; i < count; ++i ) 
  {
    next = item->itemAbove();
    if ( item->isSelected() && (i > 0) && !item->itemBelow()->isSelected() )
    {
      item->moveItem( item->itemBelow() );
      up   &= (item->itemAbove() != 0L);
      down &= (item->itemBelow() != 0L);
    }
    item = next;
  }

  buttonUp->setEnabled (up);
  buttonDown->setEnabled (down);

  refreshActionList();
}

/** Adds a new action by double click */
void DlgCustomToolbars::onDoubleClickedAction(QListViewItem* item)
{
  if (item && item->childCount()==0 && ComboToolbars->isEnabled())
    onAddAction();
}

#ifdef NEW_WB_FRAMEWORK
void DlgCustomToolbars::onCreateToolbar()
{
  QString def = QString("custom bar%1").arg(_toolBars->count()+1);
  QString text = QInputDialog::getText(tr("New custom bar"), tr("Specify the name of the new custom bar, please."),
                                      QLineEdit::Normal, def, 0, this);

  if (!text.isNull() && !text.isEmpty())
  {
    int ct = ComboToolbars->count(), pos = -1;
    for (int i=0; i<ct; i++)
    {
      if ( ComboToolbars->text(i) == text )
        pos = i;
    }

    if ( pos != -1 )
    {
      ComboToolbars->setCurrentItem(pos);
      onItemActivated(ComboToolbars->currentText());
    }
    else
    {
      ComboToolbars->insertItem(text);
      ComboToolbars->setCurrentItem( ComboToolbars->count()-1 );
      ToolBarItem* bar = new ToolBarItem(_toolBars);
      bar->setCommand( text );
      onItemActivated(ComboToolbars->currentText());
    }

    // enable the widgets
    ToolbarActions->setEnabled(true);
    ComboToolbars->setEnabled (true);
  }
}

void DlgCustomToolbars::onDeleteToolbar()
{
  QValueList<CheckListItem> items;
  QPtrList<ToolBarItem> bars = _toolBars->getItems();
  ToolBarItem* bar;

  for ( bar=bars.first(); bar; bar=bars.next() )
  {
    items.append( qMakePair( bar->command(), true ) );
  }

  CheckListDialog checklists(this, "", true) ;
  checklists.setCaption( tr("Delete selected bars") );
  checklists.setCheckableItems( items );
  if (checklists.exec())
  {
    QStringList checked = checklists.getCheckedItems();
    for ( QStringList::Iterator it = checked.begin(); it!=checked.end(); ++it )
    {
      ToolBarItem* bar = _toolBars->findItem( *it );
      if ( bar )
      {
        _toolBars->removeItem( bar );
        delete bar;
      }
    }

    refreshToolBarList();
  }
}
#endif
// -------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::DlgCustomToolbarsImp */

/**
 *  Constructs a DlgCustomToolbarsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomToolbarsImp::DlgCustomToolbarsImp( QWidget* parent, const char* name, WFlags fl )
  : DlgCustomToolbars(parent, name, fl)
{
#ifdef NEW_WB_FRAMEWORK
  _toolBars = WorkbenchManager::instance()->active()->importCustomBars("Toolbars"); 
#endif
  refreshToolBarList();
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbarsImp::~DlgCustomToolbarsImp()
{
#ifdef NEW_WB_FRAMEWORK
  ToolBarManager::getInstance()->customSetup(_toolBars);
  WorkbenchManager::instance()->active()->exportCustomBars(_toolBars, "Toolbars");
#endif
}

/** Adds created or removes deleted toolbars */
#ifndef NEW_WB_FRAMEWORK
void DlgCustomToolbarsImp::refreshActionList()
{
  QString text = ComboToolbars->currentText();
  Gui::CustomToolBar* toolbar = ApplicationWindow::Instance->customWidgetManager()->getToolBar( text );
  toolbar->clear();

  const QObjectList* children = toolbar->children ();

  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();

  QStringList items;
  QListViewItem* item = ToolbarActions->firstChild();
  for (int i=0; i < ToolbarActions->childCount(); item = item->itemBelow(), i++)
  {
    if (item->text(0) == "<Separator>")
    {
      toolbar->addSeparator ();
      items.push_back("Separator");
      continue;
    }

    bool found = false;
    Command* pCom = cCmdMgr.getCommandByActionText(item->text(0).latin1());
    if (pCom != NULL)
    {
      found = true;
      if (pCom->addTo(toolbar))
        items.push_back(pCom->getName());
    }

    if (!found)
    {
      for (QObjectListIt it2(*children); it2.current(); ++it2)
      {
        if (it2.current()->isWidgetType())
        {
          QWidget* w = (QWidget*)it2.current();
          w->reparent(toolbar,QPoint(0,0));
          break;
        }
      }
    }
  }

  toolbar->setCustomItems( items );
  toolbar->saveXML();
}
#endif
/** Discards all changes */
void DlgCustomToolbarsImp::cancel()
{
}

/** Shows all actions from the last specified commandbar */
#ifndef NEW_WB_FRAMEWORK
void DlgCustomToolbarsImp::refreshToolBarList()
{
  ComboToolbars->clear();
  ToolbarActions->clear();
  _aclToolbars = ApplicationWindow::Instance->customWidgetManager()->getToolBars();
  Gui::CustomToolBar* it;
  for ( it = _aclToolbars.first(); it; it = _aclToolbars.next() )
  {
    if ( it->canModify())
      ComboToolbars->insertItem( it->name() );
  }

  if (ComboToolbars->count() > 0)
  {
    onItemActivated(ComboToolbars->text(0));
  }
  else
  {
    ToolbarActions->setEnabled(false);
    ComboToolbars->setEnabled (false);
  }
}
#endif
/** Creates new toolbar */
#ifndef NEW_WB_FRAMEWORK
void DlgCustomToolbarsImp::onCreateToolbar()
{
  QString def = QString("toolbar%1").arg(ApplicationWindow::Instance->customWidgetManager()->countToolBars());
  QString text = QInputDialog::getText( tr("New toolbar"), tr("Specify the name of the new toolbar, please."),
                                      QLineEdit::Normal, def, 0, this );

  if (!text.isNull() && !text.isEmpty())
  {
    Gui::CustomToolBar* toolbar = ApplicationWindow::Instance->customWidgetManager()->getToolBar( text );
    toolbar->show();
    _aclToolbars.append( toolbar );
    ComboToolbars->insertItem( text );

    // enable the widgets
    ToolbarActions->setEnabled(true);
    ComboToolbars->setEnabled (true);
  }
}

/** Deletes a toolbar */
void DlgCustomToolbarsImp::onDeleteToolbar()
{
  QValueList<CheckListItem> items;
  QPtrList<Gui::CustomToolBar> tb = ApplicationWindow::Instance->customWidgetManager()->getToolBars();
  Gui::CustomToolBar* it;
  for ( it = tb.first(); it; it = tb.next() )
    items.append( qMakePair( QString(it->name()), it->canModify() ) );

  CheckListDialog checklists(this, "", true) ;
  checklists.setCaption( tr("Delete selected toolbars") );
  checklists.setCheckableItems( items );
  if (checklists.exec())
  {
    QStringList checked = checklists.getCheckedItems();
    for ( QStringList::Iterator it = checked.begin(); it!=checked.end(); ++it )
    {
      ApplicationWindow::Instance->customWidgetManager()->removeToolBarFromSettings( (*it).latin1() );
    }

    refreshToolBarList();
  }
}
#endif

#include "DlgToolbars.cpp"
#include "moc_DlgToolbars.cpp"
#include "moc_DlgToolbarsImp.cpp"
