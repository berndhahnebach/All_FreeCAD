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
#	include <qmessagebox.h>
#	include <qiconview.h> 
#	include <qfiledialog.h>
#	include <qcombobox.h>
# include <Python.h>
#endif

#include "DlgToolbarsImp.h"
#include "Application.h"
#include "Tools.h"
#include <qobjcoll.h>
#include <qtabwidget.h>

/////////////////////////////////////////////////////////////////////////////////////////////

FCDlgCustomToolbarsImp::FCDlgCustomToolbarsImp( QWidget* parent, const char* name, WFlags fl )
: FCDlgCustomToolbars(parent, name, fl)
{
  AvailableActions->setSorting( -1 );
  AvailableActions->setColumnWidthMode(0, QListView::Maximum);
  ToolbarActions->setSorting( -1 );
  ToolbarActions->setColumnWidthMode(0, QListView::Maximum);

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

  connect(ComboToolbars, SIGNAL(activated ( const QString & )), this, SLOT(slotToolBarSelected(const QString &)));
  connect(CreateToolbar, SIGNAL(clicked()), this, SLOT(slotCreateToolBar()));
  connect(DeleteToolbar, SIGNAL(clicked()), this, SLOT(slotDeleteToolBar()));

  m_aclToolbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBars();
  for (std::vector<FCToolBar*>::iterator it3 = m_aclToolbars.begin(); it3 != m_aclToolbars.end(); ++it3)
  {
    ComboToolbars->insertItem((*it3)->name());
  }

  slotToolBarSelected(ComboToolbars->text(0));

  // connections
  //
  connect(buttonRight, SIGNAL(clicked()), this, SLOT(slotAddAction()));
  connect(buttonLeft, SIGNAL(clicked()), this, SLOT(slotRemoveAction()));
  connect(buttonUp, SIGNAL(clicked()), this, SLOT(slotMoveUpAction()));
  connect(buttonDown, SIGNAL(clicked()), this, SLOT(slotMoveDownAction()));
  connect(AvailableActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotAvailableActionsChanged(QListViewItem*)));
  connect(AvailableActions, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotDblClickAddAction(QListViewItem*)));
  connect(ToolbarActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotCurrentActionsChanged(QListViewItem*)));
}

FCDlgCustomToolbarsImp::~FCDlgCustomToolbarsImp()
{
}

void FCDlgCustomToolbarsImp::apply()
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

void FCDlgCustomToolbarsImp::cancel()
{
}

void FCDlgCustomToolbarsImp::slotAvailableActionsChanged( QListViewItem *i )
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

  buttonRight->setEnabled( canAdd || ( i && i->isSelected() ) );
}

void FCDlgCustomToolbarsImp::slotCurrentActionsChanged( QListViewItem *i )
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

void FCDlgCustomToolbarsImp::slotToolBarSelected(const QString & name)
{
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  if (isModified())
  {
    if (QMessageBox::information(this, "FreeCAD", "Do want to save your changes?", "Yes", "No", QString::null, 0) == 0)
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

void FCDlgCustomToolbarsImp::slotAddAction()
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

void FCDlgCustomToolbarsImp::slotRemoveAction()
{
  QListViewItemIterator it = ToolbarActions->firstChild();
  while ( it.current() ) 
  {
	  if ( it.current()->isSelected() )
     delete it.current();
	  else
     it++;
  }

  setModified(true);
}

void FCDlgCustomToolbarsImp::slotMoveUpAction()
{
  QListViewItem *next = 0;
  QListViewItem *item = ToolbarActions->firstChild();

  for ( int i = 0; i < ToolbarActions->childCount(); ++i ) 
  {
  	next = item->itemBelow();
	  if ( item->isSelected() && (i > 0) && !item->itemAbove()->isSelected() )
	    item->itemAbove()->moveItem( item );
	  item = next;
  }

  setModified(true);
}

void FCDlgCustomToolbarsImp::slotMoveDownAction()
{
  int count = ToolbarActions->childCount();
  QListViewItem *next = 0;
  QListViewItem *item = FCListView::lastItem(ToolbarActions);

  for ( int i = 0; i < count; ++i ) 
  {
	  next = item->itemAbove();
	  if ( item->isSelected() && (i > 0) && !item->itemBelow()->isSelected() )
      item->moveItem( item->itemBelow() );
	  item = next;
  }

  setModified(true);
}

void FCDlgCustomToolbarsImp::slotCreateToolBar()
{
  QString def = tr("toolbar%1").arg(ApplicationWindow::Instance->GetCustomWidgetManager()->countToolBars());
#if QT_VERSION <= 230
  QString text = QInputDialog::getText("New toolbar", "Specify the name of the new toolbar, please.",
                                       def, 0, this);
#else
  QString text = QInputDialog::getText("New toolbar", "Specify the name of the new toolbar, please.",  QLineEdit::Normal,
                                       def, 0, this);
#endif
  if (!text.isNull() && !text.isEmpty())
  {
    FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBar(text.latin1());
    toolbar->show();
    m_aclToolbars.push_back(toolbar);
    ComboToolbars->insertItem(text);
  }
}

void FCDlgCustomToolbarsImp::slotDeleteToolBar()
{
  std::vector<std::string> items;
  std::vector<FCToolBar*> tb = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBars();
  for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    items.push_back((*it)->name());

  FCCheckListDlg checklists(this, "", true) ;
  checklists.setCaption( "Delete selected toolbars" );
  checklists.setItems(items);
  if (checklists.exec())
  {
    std::vector<int> checked = checklists.getCheckedItems();
    for (std::vector<int>::iterator it = checked.begin(); it!=checked.end(); ++it)
    {
      ApplicationWindow::Instance->GetCustomWidgetManager()->delToolBar(items[*it].c_str());
    }

    ComboToolbars->clear();
    m_aclToolbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBars();
    for (std::vector<FCToolBar*>::iterator it3 = m_aclToolbars.begin(); it3 != m_aclToolbars.end(); ++it3)
    {
      ComboToolbars->insertItem((*it3)->name());
    }

    slotToolBarSelected(ComboToolbars->text(0));
  }
}

void FCDlgCustomToolbarsImp::slotDblClickAddAction(QListViewItem* item)
{
  if (item && item->childCount()==0)
    slotAddAction();
}

#include "DlgToolbars.cpp"
#include "moc_DlgToolbars.cpp"
#include "moc_DlgToolbarsImp.cpp"
