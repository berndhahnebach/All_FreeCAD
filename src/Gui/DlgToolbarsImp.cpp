/***************************************************************************
                          DlgToolbarsImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize toolbars
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qaccel.h>
#	include <qaction.h>
#	include <qbutton.h>
#	include <qinputdialog.h>
#	include <qlabel.h>
#	include <qmessagebox.h>
#	include <qiconview.h>
#	include <qfiledialog.h>
#	include <qcombobox.h>
#	include <qobjectlist.h>
#	include <qthread.h>
#endif

#include "DlgToolbarsImp.h"
#include "Application.h"
#include "Tools.h"
#include "Command.h"

using namespace Gui::Dialog;

DlgCustomToolbars::DlgCustomToolbars( QWidget* parent, const char* name, WFlags fl )
: DlgCustomToolbarsBase(parent, name, fl)
{
  AvailableActions->setSorting( -1 );
  ToolbarActions->setSorting( -1 );

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();
  for (std::map<std::string,FCCommand*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    m_alCmdGroups[it->second->GetGroupName()].push_back(it->second);
  }
  for (std::map<std::string, std::vector<FCCommand*> >::iterator it2 = m_alCmdGroups.begin(); it2 != m_alCmdGroups.end(); ++it2)
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

DlgCustomToolbars::~DlgCustomToolbars()
{
}

void DlgCustomToolbars::apply()
{
}

void DlgCustomToolbars::cancel()
{
}

void DlgCustomToolbars::onUpdate()
{
}

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
  for (std::vector<FCToolBar*>::iterator it = m_aclToolbars.begin(); it != m_aclToolbars.end(); ++it)
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

void DlgCustomToolbars::onDoubleClickedAction(QListViewItem* item)
{
  if (item && item->childCount()==0 && ComboToolbars->isEnabled())
    onAddAction();
}

// -------------------------------------------------------------

DlgCustomToolbarsImp::DlgCustomToolbarsImp( QWidget* parent, const char* name, WFlags fl )
	: DlgCustomToolbars(parent, name, fl)
{
	onUpdate();
}

DlgCustomToolbarsImp::~DlgCustomToolbarsImp()
{
}

void DlgCustomToolbarsImp::apply()
{
  QString text = ComboToolbars->currentText();
  FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBar(text.latin1());
  toolbar->clearAll();

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

void DlgCustomToolbarsImp::cancel()
{
}

void DlgCustomToolbarsImp::onUpdate()
{
  ComboToolbars->clear();
  m_aclToolbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBars();
  for (std::vector<FCToolBar*>::iterator it3 = m_aclToolbars.begin(); it3 != m_aclToolbars.end(); ++it3)
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
    m_aclToolbars.push_back(toolbar);
    ComboToolbars->insertItem(text);

		// enable the widgets
		ToolbarActions->setEnabled(true);
		ComboToolbars->setEnabled (true);
  }
}

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

		onUpdate();
  }
}

#include "DlgToolbars.cpp"
#include "moc_DlgToolbars.cpp"
#include "moc_DlgToolbarsImp.cpp"
