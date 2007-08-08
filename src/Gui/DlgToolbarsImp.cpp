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

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include "DlgToolbarsImp.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Tools.h"
#include "Command.h"
#include "ToolBarManager.h"
#include "Widgets.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomToolbars */

/**
 *  Constructs a DlgCustomToolbars which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomToolbars::DlgCustomToolbars( QWidget* parent )
: CustomizeActionPage(parent), _toolBars(0) 
{
  this->setupUi(this);
  availableActions->addColumn(trUtf8("Available commands"));
  toolbarActions->addColumn(trUtf8("Current commands"));
  moveActionRightButton->setIcon(Gui::BitmapFactory().pixmap("button_right"));
  moveActionLeftButton->setIcon(Gui::BitmapFactory().pixmap("button_left"));
  moveActionUpButton->setIcon(Gui::BitmapFactory().pixmap("button_up"));
  moveActionDownButton->setIcon(Gui::BitmapFactory().pixmap("button_down"));
  
  availableActions->setSorting( -1 );
  toolbarActions->setSorting( -1 );

  refreshFullActionList();
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbars::~DlgCustomToolbars()
{
  delete _toolBars; 
}

void DlgCustomToolbars::refreshFullActionList()
{
  CommandManager & cCmdMgr = Application::Instance->commandManager();
  std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();
  std::map<std::string, std::vector<Command*> > alCmdGroups;
  for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    alCmdGroups[ it->second->getGroupName() ].push_back(it->second);
  }

  // force a special order
  QStringList items; items << "Help" << "Window" << "Tools" << "Standard-View" << "View" << "Edit" << "File" << "Macros";
  for (std::map<std::string, std::vector<Command*> >::iterator it2 = alCmdGroups.begin(); it2 != alCmdGroups.end(); ++it2)
  {
    if ( !items.contains( it2->first.c_str() ) )
      items.prepend(it2->first.c_str());
  }

  availableActions->clear();
  for (QStringList::Iterator it3 = items.begin(); it3 != items.end(); ++it3)
  {
    Q3ListViewItem* itemNode = new Q3ListViewItem(availableActions, QObject::tr((*it3).toAscii()));
    itemNode->setOpen(true);
    const std::vector<Command*>& rCmds = alCmdGroups[ (*it3).toStdString() ];
    for (std::vector<Command*>::const_iterator it4 = rCmds.begin(); it4 != rCmds.end(); ++it4)
    {
      Q3ListViewItem* item = new Q3ListViewItem(itemNode,availableActions->lastItem(), (*it4)->getName());
      QPixmap pix;
      if ( (*it4)->getPixmap() )
        pix = BitmapFactory().pixmap((*it4)->getPixmap());
      item->setPixmap(0, Tools::fillUp(24,24,pix));
      itemNode->insertItem(item);
    }

    availableActions->insertItem(itemNode);
  }

  availableActions->insertItem(new Q3ListViewItem(availableActions, "<Separator>"));
}

void DlgCustomToolbars::refreshActionList()
{
  if ( !_toolBars ) return; // no valid pointer
  QString text = toolbarsCombobox->currentText();
  ToolBarItem* bar = _toolBars->findItem( text );
  bar->clear();

  Q3ListViewItem* item = toolbarActions->firstChild();
  for (int i=0; i < toolbarActions->childCount(); item = item->itemBelow(), i++)
  {
    if (item->text(0) == "<Separator>")
    {
      *bar << "Separator";
    }
    else
    {
      *bar << item->text(0);
    }
  }
}

void DlgCustomToolbars::refreshToolBarList()
{
  if ( !_toolBars ) return;
  toolbarsCombobox->clear();
  toolbarActions->clear();

  QList<ToolBarItem*> bars = _toolBars->getItems();
  for ( QList<ToolBarItem*>::ConstIterator bar = bars.begin(); bar != bars.end(); ++bar )
  {
    toolbarsCombobox->addItem( (*bar)->command() );
  }

  if (toolbarsCombobox->count() > 0)
  {
    on_toolbarsCombobox_activated( toolbarsCombobox->itemText( 0 ) );
  }
  else
  {
    toolbarActions->setEnabled(false);
    toolbarsCombobox->setEnabled (false);
  }
}

/** Enables/disables buttons for change */
void DlgCustomToolbars::on_availableActions_clicked( Q3ListViewItem *i )
{
  bool canAdd = FALSE;
  Q3ListViewItemIterator it = availableActions->firstChild();

  for ( ; it.current(); it++ ) 
  {
    if ( it.current()->isSelected() ) 
    {
      canAdd = TRUE;
      break;
    }
  }

  moveActionRightButton->setEnabled( ( canAdd || ( i && i->isSelected() ) ) && toolbarsCombobox->isEnabled() );
}

/** Enables/disables buttons for change */
void DlgCustomToolbars::on_toolbarActions_clicked( Q3ListViewItem *i )
{
  moveActionUpButton->setEnabled( (bool) (i && i->itemAbove()) );
  moveActionDownButton->setEnabled( (bool) (i && i->itemBelow()) );

  bool canRemove = FALSE;
  Q3ListViewItemIterator it = toolbarActions->firstChild();
  for ( ; it.current(); it++ ) 
  {
    if ( it.current()->isSelected() ) 
    {
      canRemove = TRUE;
      break;
    }
  }

  moveActionLeftButton->setEnabled( canRemove || ( i && i->isSelected() ) );
}

/** Shows all buttons of the toolbar */
void DlgCustomToolbars::on_toolbarsCombobox_activated(const QString & name)
{
  if ( !_toolBars ) return;
  CommandManager & cCmdMgr = Application::Instance->commandManager();

  toolbarActions->clear();

  QList<ToolBarItem*> bars = _toolBars->getItems();
  for ( QList<ToolBarItem*>::ConstIterator bar = bars.begin(); bar != bars.end(); ++bar )
  {
    if ( (*bar)->command() == name )
    {
      QList<ToolBarItem*> items = (*bar)->getItems();
      for ( QList<ToolBarItem*>::ConstIterator item = items.begin(); item != items.end(); ++item )
      {
        if ( (*item)->command() == "Separator" )
          toolbarActions->insertItem(new Q3ListViewItem(toolbarActions,toolbarActions->lastItem(), "<Separator>"));
        else
        {
          Command* pCom = cCmdMgr.getCommandByName( (*item)->command().toAscii() );
          if (pCom)
          {
            Q3ListViewItem* item = new Q3ListViewItem(toolbarActions,toolbarActions->lastItem(), pCom->getName());
            QPixmap pix;
            if ( pCom->getPixmap() )
              pix = BitmapFactory().pixmap(pCom->getPixmap());
            item->setPixmap(0, Tools::fillUp(24,24,pix));
            toolbarActions->insertItem(item);
          }
        }
      }
      break;
    }
  }
}

/** Adds a new action */
void DlgCustomToolbars::on_moveActionRightButton_clicked()
{
  Q3ListView *src = availableActions;

  bool addKids = FALSE;
  Q3ListViewItem *nextSibling = 0;
  Q3ListViewItem *nextParent = 0;
  Q3ListViewItemIterator it = src->firstChild();
  for ( ; it.current(); it++ ) 
  {
    if ( it.current() == nextSibling )
     addKids = FALSE;

    if ( it.current()->isSelected() ) 
    {
      if ( it.current()->childCount() == 0 ) 
      {
        // Selected, no children
        Q3ListViewItem *i = new Q3ListViewItem( toolbarActions, toolbarActions->lastItem() );
        i->setText( 0, it.current()->text(0) );
        if (it.current()->pixmap(0) != NULL)
        {
          QPixmap pix = *(it.current()->pixmap(0));
          i->setPixmap( 0,  Tools::fillUp(24,24,pix));
        }
        toolbarActions->setCurrentItem( i );
        toolbarActions->ensureItemVisible( i );
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
      Q3ListViewItem *i = new Q3ListViewItem( toolbarActions, toolbarActions->lastItem() );
      i->setText( 0, it.current()->text(0) );
      if (it.current()->pixmap(0) != NULL)
      {
        QPixmap pix = *(it.current()->pixmap(0));
        i->setPixmap( 0, Tools::fillUp(24,24,pix) );
      }
      toolbarActions->setCurrentItem( i );
      toolbarActions->ensureItemVisible( i );
    }
  }

  refreshActionList();
}

/** Removes an action */
void DlgCustomToolbars::on_moveActionLeftButton_clicked()
{
  Q3ListViewItemIterator it = toolbarActions->firstChild();
  while ( it.current() ) 
  {
    if ( it.current()->isSelected() )
     delete it.current();
    else
     it++;
  }

  moveActionLeftButton->setEnabled (toolbarActions->childCount() > 0);

  refreshActionList();
}

/** Noves up an action */
void DlgCustomToolbars::on_moveActionUpButton_clicked()
{
  bool up = true;
  bool down = true;
  Q3ListViewItem *next = 0;
  Q3ListViewItem *item = toolbarActions->firstChild();

  for ( int i = 0; i < toolbarActions->childCount(); ++i ) 
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

  moveActionUpButton->setEnabled (up);
  moveActionDownButton->setEnabled (down);

  refreshActionList();
}

/** Moves down an action */
void DlgCustomToolbars::on_moveActionDownButton_clicked()
{
  bool up = true;
  bool down = true;
  int count = toolbarActions->childCount();
  Q3ListViewItem *next = 0;
  Q3ListViewItem *item = toolbarActions->lastItem();

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

  moveActionUpButton->setEnabled (up);
  moveActionDownButton->setEnabled (down);

  refreshActionList();
}

/** Adds a new action by double click */
void DlgCustomToolbars::on_availableActions_doubleClicked(Q3ListViewItem* item)
{
  if (item && item->childCount()==0 && toolbarsCombobox->isEnabled())
    on_moveActionRightButton_clicked();
}

void DlgCustomToolbars::on_createToolbarButton_clicked()
{
  if ( !_toolBars ) return;
  Workbench* cur = WorkbenchManager::instance()->active();
  QString baseName = cur ? cur->name() : "Base"; 
  QString def = QString("%1_custom_bar_%2").arg(baseName).arg(_toolBars->count()+1);
  QString text = QInputDialog::getText(this, tr("New custom bar"), tr("Specify the name of the new custom bar, please."),
                                       QLineEdit::Normal, def, 0);

  if (!text.isNull() && !text.isEmpty())
  {
    int ct = toolbarsCombobox->count(), pos = -1;
    for (int i=0; i<ct; i++)
    {
      if ( toolbarsCombobox->itemText(i) == text )
        pos = i;
    }

    if ( pos != -1 )
    {
      toolbarsCombobox->setCurrentIndex(pos);
      on_toolbarsCombobox_activated(toolbarsCombobox->currentText());
    }
    else
    {
      toolbarsCombobox->addItem(text);
      toolbarsCombobox->setCurrentIndex( toolbarsCombobox->count()-1 );
      ToolBarItem* bar = new ToolBarItem(_toolBars);
      bar->setCommand( text );
      on_toolbarsCombobox_activated(toolbarsCombobox->currentText());
    }

    // enable the widgets
    toolbarActions->setEnabled(true);
    toolbarsCombobox->setEnabled (true);
  }
}

void DlgCustomToolbars::on_deleteToolbarButton_clicked()
{
  if ( !_toolBars ) return;
  QList<CheckListItem> items;
  QList<ToolBarItem*> bars = _toolBars->getItems();
  for ( QList<ToolBarItem*>::ConstIterator bar = bars.begin(); bar != bars.end(); ++bar )
  {
    items.append( qMakePair( (*bar)->command(), true ) );
  }

  CheckListDialog checklists(this);
  checklists.setModal(true);
  checklists.setWindowTitle( tr("Delete selected bars") );
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

void DlgCustomToolbars::onAddMacroAction(const QString& item)
{
  refreshFullActionList();
}

void DlgCustomToolbars::onRemoveMacroAction(const QString& item)
{
  refreshFullActionList();
}

// -------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::DlgCustomToolbarsImp */

/**
 *  Constructs a DlgCustomToolbarsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomToolbarsImp::DlgCustomToolbarsImp( QWidget* parent )
  : DlgCustomToolbars(parent)
{
  if ( WorkbenchManager::instance()->active() )
    _toolBars = WorkbenchManager::instance()->active()->importCustomBars("Toolbars"); 
  refreshToolBarList();
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbarsImp::~DlgCustomToolbarsImp()
{
  if ( _toolBars )
  {
    ToolBarManager::getInstance()->customSetup(_toolBars);
    WorkbenchManager::instance()->active()->exportCustomBars(_toolBars, "Toolbars");
  }
}


#include "moc_DlgToolbarsImp.cpp"
