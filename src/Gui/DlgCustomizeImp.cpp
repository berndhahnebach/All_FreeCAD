/***************************************************************************
                          DlgCustomizeImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize toolbars and button groups
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




#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qmessagebox.h>
#	include <qiconview.h> 
#	include <qfiledialog.h>
#	include <qcombobox.h>
#       include <Python.h>
#endif

#include "DlgCustomizeImp.h"
#include "Application.h"
#include "Tools.h"
#include <qobjcoll.h>
#include <qtabwidget.h>

FCDlgCustomToolbarsImp::FCDlgCustomToolbarsImp( QWidget* parent, const char* name, WFlags fl )
: FCDlgCustomToolbars(parent, name, fl), bChanged(false)
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
      item->setPixmap(0, FCTools::resize(24,24,pix));
      itemNode->insertItem(item);
    }

    AvailableActions->insertItem(itemNode);
  }

  AvailableActions->insertItem(new QListViewItem(AvailableActions, "<Separator>"));

  connect(ComboToolbars, SIGNAL(activated ( const QString & )), this, SLOT(slotToolBarSelected(const QString &)));
  connect(CreateToolbar, SIGNAL(clicked()), this, SLOT(slotCreateToolBar()));
  m_aclToolbars = ApplicationWindow::Instance->GetToolBars();

  for (std::vector<QToolBar*>::iterator it3 = m_aclToolbars.begin(); it3 != m_aclToolbars.end(); ++it3)
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
  QToolBar* toolbar = ApplicationWindow::Instance->GetToolBar(text.latin1());

  FCTools::clearToolButtons(toolbar);
  const QObjectList* children = toolbar->children ();

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  QListViewItem* item = ToolbarActions->firstChild();
  for (int i=0; i < ToolbarActions->childCount(); item = item->itemBelow(), i++)
  {
    if (item->text(0) == "<Separator>")
    {
      toolbar->addSeparator ();
      continue;
    }

    bool found = false;
    for (std::map<std::string,FCCommand*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
    {
      if (item->text(0) == it->second->GetAction()->menuText())
      {
        it->second->GetAction()->addTo(toolbar);
        found = true;
        break;
      }
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
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  if (bChanged)
  {
    if (QMessageBox::information(this, "FreeCAD", "Do want to save your changes?", "Yes", "No", QString::null, 0) == 0)
      apply();
    else
      bChanged = false;
  }

  ToolbarActions->clear();

  for (std::vector<QToolBar*>::iterator it = m_aclToolbars.begin(); it != m_aclToolbars.end(); ++it)
  {
    if ((*it)->name() == name)
    {
      const QObjectList* children = (*it)->children ();
      for (QObjectListIt it2(*children); it2.current(); ++it2)
      {
        QString bname = it2.current()->name();
        if (it2.current()->inherits("QToolButton"))
        {
          QToolButton* b = (QToolButton*)it2.current();
          bname = b->textLabel();
          if (bname.isNull() || bname.isEmpty())
            continue;
          if (sCommands.find(bname.latin1()) != sCommands.end())
          {
            FCCommand* pCom = sCommands[bname.latin1()];
            QListViewItem* item = new QListViewItem(ToolbarActions,FCListView::lastItem(ToolbarActions), pCom->GetAction()->menuText());
            QPixmap pix = pCom->GetAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
            item->setPixmap(0, FCTools::resize(24,24,pix));
            ToolbarActions->insertItem(item);
          }
        }
        else if (it2.current()->isWidgetType())
        {
          if (bname.contains("separator", false) > 0)
          {
            ToolbarActions->insertItem(new QListViewItem(ToolbarActions,FCListView::lastItem(ToolbarActions), "<Separator>"));
          }
          else
          {
            QListViewItem* item = new QListViewItem(ToolbarActions,FCListView::lastItem(ToolbarActions), it2.current()->className());
            QPixmap pix = ApplicationWindow::Instance->GetBmpFactory().GetPixmap("trolltech");
            item->setPixmap(0, FCTools::resize(24,24,pix));
            ToolbarActions->insertItem(item);
          }
        }
      }
      break;
    }
  }
}

void FCDlgCustomToolbarsImp::slotAddAction()
{
  bChanged = true;
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
  	    	i->setPixmap( 0,  FCTools::resize(24,24,pix));
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
  	    i->setPixmap( 0, FCTools::resize(24,24,pix) );
      }
	    ToolbarActions->setCurrentItem( i );
	    ToolbarActions->ensureItemVisible( i );
  	}
  }
}

void FCDlgCustomToolbarsImp::slotRemoveAction()
{
  bChanged = true;
  QListViewItemIterator it = ToolbarActions->firstChild();
  while ( it.current() ) 
  {
	  if ( it.current()->isSelected() )
     delete it.current();
	  else
     it++;
  }
}

void FCDlgCustomToolbarsImp::slotMoveUpAction()
{
  bChanged = true;
  QListViewItem *next = 0;
  QListViewItem *item = ToolbarActions->firstChild();

  for ( int i = 0; i < ToolbarActions->childCount(); ++i ) 
  {
  	next = item->itemBelow();
	  if ( item->isSelected() && (i > 0) && !item->itemAbove()->isSelected() )
	    item->itemAbove()->moveItem( item );
	  item = next;
  }
}

void FCDlgCustomToolbarsImp::slotMoveDownAction()
{
  bChanged = true;
  int count = ToolbarActions->childCount();
  QListViewItem *next = 0;
  QListViewItem *item = FCListView::lastItem(ToolbarActions);;

  for ( int i = 0; i < count; ++i ) 
  {
	  next = item->itemAbove();
	  if ( item->isSelected() && (i > 0) && !item->itemBelow()->isSelected() )
      item->moveItem( item->itemBelow() );
	  item = next;
  }
}

void FCDlgCustomToolbarsImp::slotCreateToolBar()
{
  QString text = ToolbarName->text();
  ToolbarName->clear();
  QToolBar* toolbar = ApplicationWindow::Instance->GetToolBar(text.latin1());
  toolbar->show();
  m_aclToolbars.push_back(toolbar);
  ComboToolbars->insertItem(text);
}

void FCDlgCustomToolbarsImp::slotDblClickAddAction(QListViewItem* item)
{
  if (item && item->childCount()==0)
    slotAddAction();
}

/////////////////////////////////////////////////////////////////////////////////////////////

FCDlgCustomCommandsImp::FCDlgCustomCommandsImp( QWidget* parent, const char* name, WFlags fl  )
: FCDlgCustomCommands(parent, name, fl), bChanged(false)
{
  connect(IconView1, SIGNAL(emitSelectionChanged(QString)), this, SLOT(slotDescription(QString)));
  connect(ComboBoxCategory, SIGNAL(highlighted ( const QString & )), this, SLOT(slotGroupSelected(const QString &)));

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  for (std::map<std::string,FCCommand*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    m_alCmdGroups[it->second->GetGroupName()].push_back(it->second);
  }

  for (std::map<std::string, std::vector<FCCommand*> >::iterator it2 = m_alCmdGroups.begin(); it2 != m_alCmdGroups.end(); ++it2)
  {
    ComboBoxCategory->insertItem(it2->first.c_str());
  }

  ComboBoxCategory->setCurrentItem(0);
}

FCDlgCustomCommandsImp::~FCDlgCustomCommandsImp()
{
}

void FCDlgCustomCommandsImp::apply()
{
}

void FCDlgCustomCommandsImp::slotDescription(QString txt)
{
  TextLabel->setText(txt);
}

void FCDlgCustomCommandsImp::slotGroupSelected(const QString & group)
{
  IconView1->clear();
 
  // group of commands found
  if (m_alCmdGroups.find(group.latin1()) != m_alCmdGroups.end())
  {
    std::vector<FCCommand*> aCmds = m_alCmdGroups[group.latin1()];
    for (std::vector<FCCommand*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it)
    {
      if ((*it)->GetAction())
      (void) new FCCmdViewItem(IconView1, (*it)->GetAction());
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////

FCDlgCustomActionsImp::FCDlgCustomActionsImp( QWidget* parent, const char* name, WFlags fl )
: FCDlgCustomActions(parent, name, fl), bChanged(false)
{
  // connections
  //
  connect(buttonNew, SIGNAL(clicked()), this, SLOT(slotAddCustomAction()));
  connect(buttonDelete, SIGNAL(clicked()), this, SLOT(slotDelCustomAction()));
  connect(actionPixmapButton, SIGNAL(clicked()), this, SLOT(slotCustomActionPixmap()));
  connect(CustomActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotCustomActionsChanged(QListViewItem*)));
}

FCDlgCustomActionsImp::~FCDlgCustomActionsImp()
{
}

void FCDlgCustomActionsImp::apply()
{
}

void FCDlgCustomActionsImp::slotCustomActionsChanged( QListViewItem *i )
{
  bool canDelete = FALSE;
  QListViewItemIterator it = CustomActions->firstChild();

  for ( ; it.current(); it++ ) 
  {
    if ( it.current()->isSelected() ) 
    {
      actionName->setText(it.current()->text(0));
	    canDelete = TRUE;
	    break;
	  }
  }

  buttonDelete->setEnabled( canDelete || ( i && i->isSelected() ) );
}

void FCDlgCustomActionsImp::slotAddCustomAction()
{
  if (actionName->text().isEmpty())
  {
    QMessageBox::warning(this, "Empty name","Please specify an action name first");
    return;
  }

  QListViewItem* item = new QListViewItem(CustomActions,FCListView::lastItem(CustomActions), actionName->text());
  if (PixmapLabel->pixmap() != NULL)
  {
    QPixmap p = *PixmapLabel->pixmap();
    item->setPixmap(0, FCTools::resize(24,24,p));
  }
  actionName->clear();
  actionToolTip->clear();
  actionStatus->clear();
}

void FCDlgCustomActionsImp::slotDelCustomAction()
{
  bChanged = true;
  QListViewItemIterator it = CustomActions->firstChild();
  while ( it.current() ) 
  {
	  if ( it.current()->isSelected() )
     delete it.current();
	  else
     it++;
  }
}

void FCDlgCustomActionsImp::slotCustomActionPixmap()
{
  QString pix = FCFileDialog::getOpenFileName(QString::null,"Pixmap (*.xpm)",this, "", "Choose a Pixmap");
  if (!pix.isEmpty())
  {
    PixmapLabel->setPixmap(QPixmap(pix));
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////

/* 
 *  Constructs a FCDlgCustomize which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgCustomize::FCDlgCustomize( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  if ( !name )
  	setName( "FCDlgCustomize" );
  resize( 434, 365 ); 
  setProperty( "caption", tr( "Customize" ) );
  setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
  FCDlgCustomizeBaseLayout = new QGridLayout( this ); 
  FCDlgCustomizeBaseLayout->setSpacing( 6 );
  FCDlgCustomizeBaseLayout->setMargin( 11 );

  Layout = new QHBoxLayout; 
  Layout->setSpacing( 6 );
  Layout->setMargin( 0 );

  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setProperty( "text", tr( "&Help" ) );
  buttonHelp->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonHelp );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout->addItem( spacer );

  buttonApply = new QPushButton( this, "buttonApply" );
  buttonApply->setProperty( "text", tr( "&Apply" ) );
  buttonApply->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonApply );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setProperty( "text", tr( "&OK" ) );
  buttonOk->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  buttonOk->setProperty( "default", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonOk );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setProperty( "text", tr( "&Cancel" ) );
  buttonCancel->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonCancel );

  FCDlgCustomizeBaseLayout->addLayout( Layout, 1, 0 );

  tabWidget = new QTabWidget( this, "tabWidget" );

  // insert new tabs here !!!
  //
  //
  QWidget* tab;
  // first page
  //
  tab = new FCDlgCustomCommandsImp( tabWidget, "tab" );
  tabWidget->insertTab( tab, tr( "Commands" ) );
  tabPages.push_back(tab);
  // second page
  //
  tab = new FCDlgCustomToolbarsImp( tabWidget, "tab" );
  tabWidget->insertTab( tab, tr( "Toolbars" ) );
  tabPages.push_back(tab);
  // third page
  //
  tab = new FCDlgCustomActionsImp( tabWidget, "tab" );
  tabWidget->insertTab( tab, tr( "Actions" ) );
  tabPages.push_back(tab);


  FCDlgCustomizeBaseLayout->addWidget( tabWidget, 0, 0 );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

  // tab order
  setTabOrder( tabWidget, buttonHelp );
  setTabOrder( buttonHelp, buttonApply );
  setTabOrder( buttonApply, buttonOk );
  setTabOrder( buttonOk, buttonCancel );

  // connections
  //
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(slotOK()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(slotApply()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgCustomize::~FCDlgCustomize()
{
    // no need to delete child widgets, Qt does it all for us
  for (std::vector<QWidget*>::iterator it = tabPages.begin(); it!=tabPages.end();++it)
    delete (*it);

  tabPages.clear();
}

void FCDlgCustomize::slotOK()
{
  slotApply();
}

void FCDlgCustomize::slotApply()
{
}


#include "DlgActions.cpp"
#include "DlgCommands.cpp"
#include "DlgToolbars.cpp"
#include "moc_DlgActions.cpp"
#include "moc_DlgCommands.cpp"
#include "moc_DlgToolbars.cpp"
#include "moc_DlgCustomizeImp.cpp"
