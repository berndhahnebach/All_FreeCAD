/***************************************************************************
                          DlgCmdbarsImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize command bars
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

#include "DlgCmdbarsImp.h"
#include "Application.h"
#include "Tools.h"
#include "Widgets.h"
#include "Command.h"

/////////////////////////////////////////////////////////////////////////////////////////////

FCDlgCustomCmdbarsImp::FCDlgCustomCmdbarsImp( QWidget* parent, const char* name, WFlags fl )
: FCDlgCustomCmdbars(parent, name, fl)
{
  AvailableActions->setSorting( -1 );
  AvailableActions->setColumnWidthMode(0, QListView::Maximum);
  CmdbarActions->setSorting( -1 );
  CmdbarActions->setColumnWidthMode(0, QListView::Maximum);

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

  connect(ComboCmdbars, SIGNAL(activated ( const QString & )), this, SLOT(slotCmdBarSelected(const QString &)));
  connect(CreateCmdbar, SIGNAL(clicked()), this, SLOT(slotCreateCmdBar()));
  connect(DeleteCmdbar, SIGNAL(clicked()), this, SLOT(slotDeleteCmdBar()));

  m_aclCmdbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
  for (std::vector<FCToolBar*>::iterator it3 = m_aclCmdbars.begin(); it3 != m_aclCmdbars.end(); ++it3)
  {
    if ((*it3)->canModify())
      ComboCmdbars->insertItem((*it3)->name());
  }

  slotCmdBarSelected(ComboCmdbars->text(0));

  // connections
  //
  connect(buttonRight, SIGNAL(clicked()), this, SLOT(slotAddAction()));
  connect(buttonLeft, SIGNAL(clicked()), this, SLOT(slotRemoveAction()));
  connect(buttonUp, SIGNAL(clicked()), this, SLOT(slotMoveUpAction()));
  connect(buttonDown, SIGNAL(clicked()), this, SLOT(slotMoveDownAction()));
  connect(AvailableActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotAvailableActionsChanged(QListViewItem*)));
  connect(AvailableActions, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotDblClickAddAction(QListViewItem*)));
  connect(CmdbarActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotCurrentActionsChanged(QListViewItem*)));
}

FCDlgCustomCmdbarsImp::~FCDlgCustomCmdbarsImp()
{
}

void FCDlgCustomCmdbarsImp::apply()
{
  QString text = ComboCmdbars->currentText();
  FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBar(text.latin1());
  toolbar->clearAll();

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  std::vector<std::string> items;
  QListViewItem* item = CmdbarActions->firstChild();
  for (int i=0; i < CmdbarActions->childCount(); item = item->itemBelow(), i++)
  {
    if (item->text(0) == "<Separator>")
    {
      toolbar->addSeparator ();
      items.push_back("Separator");
      continue;
    }

    FCCommand* pCom = cCmdMgr.GetCommandByActionText(item->text(0).latin1());
    if (pCom != NULL)
    {
      if (pCom->addTo(toolbar))
        items.push_back(pCom->GetName());
    }
  }

  toolbar->setItems(items);
  toolbar->saveXML();
}

void FCDlgCustomCmdbarsImp::cancel()
{
}

void FCDlgCustomCmdbarsImp::slotAvailableActionsChanged( QListViewItem *i )
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

void FCDlgCustomCmdbarsImp::slotCurrentActionsChanged( QListViewItem *i )
{
  buttonUp->setEnabled( (bool) (i && i->itemAbove()) );
  buttonDown->setEnabled( (bool) (i && i->itemBelow()) );

  bool canRemove = FALSE;
  QListViewItemIterator it = CmdbarActions->firstChild();
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

void FCDlgCustomCmdbarsImp::slotCmdBarSelected(const QString & name)
{
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  if (isModified())
  {
    if (QMessageBox::information(this, "FreeCAD", tr("Do want to save your changes?"), tr("Yes"), tr("No"), QString::null, 0) == 0)
      apply();
    else
     setModified(false);
  }

  CmdbarActions->clear();
  for (std::vector<FCToolBar*>::iterator it = m_aclCmdbars.begin(); it != m_aclCmdbars.end(); ++it)
  {
    if ((*it)->name() == name)
    {
      std::vector<std::string> items = (*it)->getItems();
      for (std::vector<std::string>::iterator it2 = items.begin(); it2 != items.end(); ++it2)
      {
        if (*it2 == "Separator")
        {
          CmdbarActions->insertItem(new QListViewItem(CmdbarActions,FCListView::lastItem(CmdbarActions), "<Separator>"));
        }
        else
        {
          FCCommand* pCom = cCmdMgr.GetCommandByName(it2->c_str());
          if (pCom)
          {
            QListViewItem* item = new QListViewItem(CmdbarActions,FCListView::lastItem(CmdbarActions), pCom->GetAction()->menuText());
            QPixmap pix = pCom->GetAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
            item->setPixmap(0, FCTools::fillUp(24,24,pix));
            CmdbarActions->insertItem(item);
          }
        }
      }
    }
  }
}

void FCDlgCustomCmdbarsImp::slotAddAction()
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
		    QListViewItem *i = new QListViewItem( CmdbarActions, FCListView::lastItem(CmdbarActions) );
		    i->setText( 0, it.current()->text(0) );
        if (it.current()->pixmap(0) != NULL)
        {
          QPixmap pix = *(it.current()->pixmap(0));
  	    	i->setPixmap( 0,  FCTools::fillUp(24,24,pix));
        }
		    CmdbarActions->setCurrentItem( i );
		    CmdbarActions->ensureItemVisible( i );
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
	    QListViewItem *i = new QListViewItem( CmdbarActions, FCListView::lastItem(CmdbarActions) );
	    i->setText( 0, it.current()->text(0) );
      if (it.current()->pixmap(0) != NULL)
      {
        QPixmap pix = *(it.current()->pixmap(0));
  	    i->setPixmap( 0, FCTools::fillUp(24,24,pix) );
      }
	    CmdbarActions->setCurrentItem( i );
	    CmdbarActions->ensureItemVisible( i );
  	}
  }

  setModified(true);
}

void FCDlgCustomCmdbarsImp::slotRemoveAction()
{
  QListViewItemIterator it = CmdbarActions->firstChild();
  while ( it.current() ) 
  {
	  if ( it.current()->isSelected() )
     delete it.current();
	  else
     it++;
  }

  setModified(true);
}

void FCDlgCustomCmdbarsImp::slotMoveUpAction()
{
  QListViewItem *next = 0;
  QListViewItem *item = CmdbarActions->firstChild();

  for ( int i = 0; i < CmdbarActions->childCount(); ++i ) 
  {
  	next = item->itemBelow();
	  if ( item->isSelected() && (i > 0) && !item->itemAbove()->isSelected() )
	    item->itemAbove()->moveItem( item );
	  item = next;
  }

  setModified(true);
}

void FCDlgCustomCmdbarsImp::slotMoveDownAction()
{
  int count = CmdbarActions->childCount();
  QListViewItem *next = 0;
  QListViewItem *item = FCListView::lastItem(CmdbarActions);

  for ( int i = 0; i < count; ++i ) 
  {
	  next = item->itemAbove();
	  if ( item->isSelected() && (i > 0) && !item->itemBelow()->isSelected() )
      item->moveItem( item->itemBelow() );
	  item = next;
  }

  setModified(true);
}

void FCDlgCustomCmdbarsImp::slotCreateCmdBar()
{
  QString def = QString("commandbar%1").arg(ApplicationWindow::Instance->GetCustomWidgetManager()->countCmdBars());
  QString text = QInputDialog::getText(tr("New command bar"), tr("Specify the name of the new command bar, please."),
#if QT_VERSION > 230
																			QLineEdit::Normal,
#endif
                                      def, 0, this);

  if (!text.isNull() && !text.isEmpty())
  {
    FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBar(text.latin1());
    toolbar->show();
    m_aclCmdbars.push_back(toolbar);
    ComboCmdbars->insertItem(text);
  }
}

void FCDlgCustomCmdbarsImp::slotDeleteCmdBar()
{
  std::vector<std::string> items;
  std::vector<FCToolBar*> tb = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
  for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    items.push_back((*it)->name());

  FCCheckListDlg checklists(this, "", true) ;
  checklists.setCaption( tr("Delete selected command bars") );
  checklists.setItems(items);
  if (checklists.exec())
  {
    std::vector<int> checked = checklists.getCheckedItems();
    for (std::vector<int>::iterator it = checked.begin(); it!=checked.end(); ++it)
    {
      ApplicationWindow::Instance->GetCustomWidgetManager()->delCmdBar(items[*it].c_str());
    }

    ComboCmdbars->clear();
    m_aclCmdbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
    for (std::vector<FCToolBar*>::iterator it3 = m_aclCmdbars.begin(); it3 != m_aclCmdbars.end(); ++it3)
    {
      ComboCmdbars->insertItem((*it3)->name());
    }

    slotCmdBarSelected(ComboCmdbars->text(0));
  }
}

void FCDlgCustomCmdbarsImp::slotDblClickAddAction(QListViewItem* item)
{
  if (item && item->childCount()==0)
    slotAddAction();
}

#include "DlgCmdbars.cpp"
#include "moc_DlgCmdbars.cpp"
#include "moc_DlgCmdbarsImp.cpp"
