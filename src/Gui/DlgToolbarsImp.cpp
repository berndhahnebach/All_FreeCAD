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
#endif

#include "DlgToolbarsImp.h"
#include "Application.h"
#include "Tools.h"
#include "Command.h"

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
{
  AvailableActions->setSorting( -1 );
  ToolbarActions->setSorting( -1 );

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();
  for (std::map<std::string,FCCommand*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    _alCmdGroups[it->second->GetGroupName()].push_back(it->second);
  }
  for (std::map<std::string, std::vector<FCCommand*> >::iterator it2 = _alCmdGroups.begin(); it2 != _alCmdGroups.end(); ++it2)
  {
    QListViewItem* itemNode = new QListViewItem(AvailableActions, it2->first.c_str());
    itemNode->setOpen(true);
    for (std::vector<FCCommand*>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
    {
      QListViewItem* item = new QListViewItem(itemNode,FCListView::lastItem(AvailableActions), (*it3)->GetAction()->menuText());
      QPixmap pix = (*it3)->GetAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
      item->setPixmap(0, FCTools::fillUp(24,24,pix));
      itemNode->insertItem(item);
    }

    AvailableActions->insertItem(itemNode);
  }

  AvailableActions->insertItem(new QListViewItem(AvailableActions, "<Separator>"));
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbars::~DlgCustomToolbars()
{
}

void DlgCustomToolbars::apply()
{
}

void DlgCustomToolbars::cancel()
{
}

void DlgCustomToolbars::updateData()
{
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
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  if (isModified())
  {
    if (QMessageBox::information(this, "FreeCAD", tr("Do want to save your changes?"), tr("Yes"), tr("No"), QString::null, 0) == 0)
      apply();
    else
      setModified(false);
  }

  ToolbarActions->clear();
  for (std::vector<FCToolBar*>::iterator it = _aclToolbars.begin(); it != _aclToolbars.end(); ++it)
  {
    if ((*it)->name() == name)
    {
      std::vector<std::string> items = (*it)->getItems();
      for (std::vector<std::string>::iterator it2 = items.begin(); it2 != items.end(); ++it2)
      {
        if (*it2 == "Separator")
        {
          ToolbarActions->insertItem(new QListViewItem(ToolbarActions,FCListView::lastItem(ToolbarActions), "<Separator>"));
        }
        else
        {
          FCCommand* pCom = cCmdMgr.GetCommandByName(it2->c_str());
          if (pCom)
          {
            QListViewItem* item = new QListViewItem(ToolbarActions,FCListView::lastItem(ToolbarActions), pCom->GetAction()->menuText());
            QPixmap pix = pCom->GetAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
            item->setPixmap(0, FCTools::fillUp(24,24,pix));
            ToolbarActions->insertItem(item);
          }
        }
      }
    }
  }
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
        QListViewItem *i = new QListViewItem( ToolbarActions, FCListView::lastItem(ToolbarActions) );
        i->setText( 0, it.current()->text(0) );
        if (it.current()->pixmap(0) != NULL)
        {
          QPixmap pix = *(it.current()->pixmap(0));
          i->setPixmap( 0,  FCTools::fillUp(24,24,pix));
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
      QListViewItem *i = new QListViewItem( ToolbarActions, FCListView::lastItem(ToolbarActions) );
      i->setText( 0, it.current()->text(0) );
      if (it.current()->pixmap(0) != NULL)
      {
        QPixmap pix = *(it.current()->pixmap(0));
        i->setPixmap( 0, FCTools::fillUp(24,24,pix) );
      }
      ToolbarActions->setCurrentItem( i );
      ToolbarActions->ensureItemVisible( i );
    }
  }

  setModified(true);
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
  setModified(true);
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

  setModified(true);
}

/** Moves down an action */
void DlgCustomToolbars::onMoveDownAction()
{
  bool up = true;
  bool down = true;
  int count = ToolbarActions->childCount();
  QListViewItem *next = 0;
  QListViewItem *item = FCListView::lastItem(ToolbarActions);

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

  setModified(true);
}

/** Adds a new action by double click */
void DlgCustomToolbars::onDoubleClickedAction(QListViewItem* item)
{
  if (item && item->childCount()==0 && ComboToolbars->isEnabled())
    onAddAction();
}

// -------------------------------------------------------------

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
  updateData();
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbarsImp::~DlgCustomToolbarsImp()
{
}

/** Adds created or removes deleted toolbars */
void DlgCustomToolbarsImp::apply()
{
  QString text = ComboToolbars->currentText();
  FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBar(text.latin1());
  toolbar->clearUp();

  const QObjectList* children = toolbar->children ();

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  std::vector<std::string> items;
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
    FCCommand* pCom = cCmdMgr.GetCommandByActionText(item->text(0).latin1());
    if (pCom != NULL)
    {
      found = true;
      if (pCom->addTo(toolbar))
        items.push_back(pCom->GetName());
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

  toolbar->setItems(items);
  toolbar->saveXML();
}

/** Discards all changes */
void DlgCustomToolbarsImp::cancel()
{
}

/** Shows all actions from the last specified command bar */
void DlgCustomToolbarsImp::updateData()
{
  ComboToolbars->clear();
  _aclToolbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBars();
  for (std::vector<FCToolBar*>::iterator it3 = _aclToolbars.begin(); it3 != _aclToolbars.end(); ++it3)
  {
    if ((*it3)->canModify())
      ComboToolbars->insertItem((*it3)->name());
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

/** Creates new toolbar */
void DlgCustomToolbarsImp::onCreateToolbar()
{
  QString def = QString("toolbar%1").arg(ApplicationWindow::Instance->GetCustomWidgetManager()->countToolBars());
  QString text = QInputDialog::getText(tr("New toolbar"), tr("Specify the name of the new toolbar, please."),
#if QT_VERSION > 230
                                      QLineEdit::Normal,
#endif
                                      def, 0, this);

  if (!text.isNull() && !text.isEmpty())
  {
    FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBar(text.latin1());
    toolbar->show();
    _aclToolbars.push_back(toolbar);
    ComboToolbars->insertItem(text);

    // enable the widgets
    ToolbarActions->setEnabled(true);
    ComboToolbars->setEnabled (true);
  }
}

/** Deletes a toolbar */
void DlgCustomToolbarsImp::onDeleteToolbar()
{
  std::vector<std::pair<std::string, bool> > items;
  std::vector<FCToolBar*> tb = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBars();
  for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    items.push_back(std::make_pair<std::string, bool>((*it)->name(), (*it)->canModify()));

  FCCheckListDlg checklists(this, "", true) ;
  checklists.setCaption( tr("Delete selected toolbars") );
  checklists.setItems(items);
  if (checklists.exec())
  {
    std::vector<std::string> checked = checklists.getCheckedItems();
    for (std::vector<std::string>::iterator it = checked.begin(); it!=checked.end(); ++it)
    {
      ApplicationWindow::Instance->GetCustomWidgetManager()->delToolBar(it->c_str());
    }

    updateData();
  }
}

#include "DlgToolbars.cpp"
#include "moc_DlgToolbars.cpp"
#include "moc_DlgToolbarsImp.cpp"
