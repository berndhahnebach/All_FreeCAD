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
# include <qaccel.h>
# include <qdir.h>
# include <qiconview.h>
# include <qlabel.h>
# include <qlayout.h>
# include <qpushbutton.h>
# include <qtoolbutton.h>
#endif

#include "DlgActionsImp.h"
#include "Application.h"
#include "Tools.h"
#include "Command.h"
#include "BitmapFactory.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgCustomActionsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomActionsImp::DlgCustomActionsImp( QWidget* parent, const char* name, WFlags fl )
: DlgCustomActionsBase(parent, name, fl), bShown( false )
{
  // search for all macros
  std::string cMacroPath = GetApplication().
    GetParameterGroupByPath("User parameter:BaseApp/Preferences/Macro")
    ->GetASCII("MacroPath",GetApplication().GetHomePath());

  QDir d(cMacroPath.c_str(),"*.FCMacro");
  actionMacros->clear();
  for (unsigned int i=0; i<d.count(); i++ )
    actionMacros->insertItem(d[i]);

  showActions();
  newActionName();
}

/** Destroys the object and frees any allocated resources */
DlgCustomActionsImp::~DlgCustomActionsImp()
{
}

/** 
 * Displays this page. If no macros were found a message box
 * appears.
 */
void DlgCustomActionsImp::show()
{
  DlgCustomActionsBase::show();
  if (actionMacros->count() == 0 && bShown == false)
  {
    bShown = true;
    QMessageBox::warning(this, tr("No macro"),tr("No macros found."));
  }
}

void DlgCustomActionsImp::showActions()
{
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*> aclCurMacros = rclMan.GetGroupCommands("Macros");
  for (std::vector<FCCommand*>::iterator it = aclCurMacros.begin(); it != aclCurMacros.end(); ++it)
  {
    QListViewItem* item = new QListViewItem(CustomActions,CustomActions->lastItem(), (*it)->GetName());
    if (!(*it)->GetAction()->iconSet().isNull())
    {
      QPixmap p = (*it)->GetAction()->iconSet().pixmap();
      item->setPixmap(0, Tools::fillUp(24,24,p));
    }
  }
}

void DlgCustomActionsImp::onCustomActionsCanDelete( QListViewItem *i )
{
  bool canDelete = FALSE;
  QListViewItemIterator it = CustomActions->firstChild();

  for ( ; it.current(); it++ ) 
  {
    if ( it.current()->isSelected() ) 
    {
      canDelete = TRUE;
      break;
    }
  }

  buttonDelete->setEnabled( canDelete || ( i && i->isSelected() ) );
}

void DlgCustomActionsImp::onCustomActionsDoubleClicked( QListViewItem *i )
{
  if ( !i ) return; // no valid item

  actionName->setText(i->text(0));

  // search for the command in the manager and if necessary in the temporary created ones
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCmd = rclMan.GetCommandByName(i->text(0).latin1());
  MacroCommand* pScript = dynamic_cast<MacroCommand*>(pCmd);

  // if valid command
  if ( pScript )
  {
    bool bFound = false;
    for (int i = 0; i<actionMacros->count(); i++)
    {
      if (actionMacros->text(i).startsWith(pScript->GetScriptName()))
      {
        bFound = true;
        actionMacros->setCurrentItem(i);
        break;
      }
    }

    if (!bFound)
    {
      Base::Console().Error(tr("Sorry, couldn't find macro file.").latin1());
    }

    // fill up labels with the command's data
    actionWhatsThis -> setText( pScript->GetWhatsThis() );
    actionMenu      -> setText( pScript->GetMenuText() );
    actionToolTip   -> setText( pScript->GetToolTipText() );
    actionStatus    -> setText( pScript->GetStatusTip() );
    actionAccel     -> setText( QAccel::keyToString(pScript->GetAccel()) );
    PixmapLabel->clear();
    if ( pScript->GetPixmap().length() > 2)
    {
      QPixmap p = Gui::BitmapFactory().pixmap( pScript->GetPixmap() );
      PixmapLabel->setPixmap(p);
    }
  }
}

void DlgCustomActionsImp::onAddCustomAction()
{
  if (actionName->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Empty name"),tr("Please specify an action name first."));
    return;
  }

  if (actionMacros-> currentText().isEmpty())
  {
    QMessageBox::warning(this, tr("Empty macro"),tr("Please specify the macro first."));
    return;
  }

  if (actionMenu->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Empty text"),tr("Please specify the menu text first."));
    return;
  }

  // search for the command in the manager
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCmd = rclMan.GetCommandByName(actionName->text().latin1());
  MacroCommand* macro = dynamic_cast<MacroCommand*>(pCmd);

  // add pixmap to the item
  QListViewItem* item;
  if ( !macro )
  {
    // add new action
    macro = new MacroCommand(actionName->text().latin1());
    rclMan.AddCommand( macro );
    item = new QListViewItem(CustomActions,CustomActions->lastItem(), actionName->text());
  }
  else
  {
    item = CustomActions->currentItem();
    item->setPixmap( 0, QPixmap() );
  }
  
  if ( PixmapLabel->pixmap() && item )
  {
    QPixmap p = *PixmapLabel->pixmap();
    item->setPixmap(0, Tools::fillUp(24,24,p));
  }

  if ( !actionWhatsThis->text().isEmpty() )
    macro->SetWhatsThis( actionWhatsThis->text() );
  actionWhatsThis->clear();
  
  if ( !actionMacros-> currentText().isEmpty() )
    macro->SetScriptName( actionMacros-> currentText() );
  
  if ( !actionMenu->text().isEmpty() )
    macro->SetMenuText( actionMenu->text() );
  actionMenu->clear();

  if ( !actionToolTip->text().isEmpty() )
    macro->SetToolTipText( actionToolTip->text() );
  actionToolTip->clear();

  if ( !actionStatus->text().isEmpty() )
    macro->SetStatusTip( actionStatus->text() );
  actionStatus->clear();

  if ( !m_sPixmap.isEmpty() )
    macro->SetPixmap( m_sPixmap );
  PixmapLabel->clear();

  if ( !actionAccel->text().isEmpty() )
    macro->SetAccel( QAccel::stringToKey( actionAccel->text() ) );
  actionAccel->clear();

  newActionName();
}

void DlgCustomActionsImp::onDelCustomAction()
{
  // remove item from list view
  QString itemText;
  QListViewItemIterator it = CustomActions->firstChild();
  while ( it.current() ) 
  {
    if ( it.current()->isSelected() )
    {
      itemText = it.current()->text(0);
      delete it.current();
      break;
    }
    else
      it++;
  }

  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*> aclCurMacros = rclMan.GetGroupCommands("Macros");
  std::vector<FCCommand*>::iterator it2;

  // if the command is registered in the manager just remove it
  for (it2 = aclCurMacros.begin(); it2!= aclCurMacros.end(); ++it2)
  {
    if ( itemText == (*it2)->GetName() )
    {
      // remove from manager and delete it immediately
      rclMan.RemoveCommand( *it2 );
      break;
    }
  }

  newActionName();
}

void DlgCustomActionsImp::onCustomActionPixmap()
{
  // create a dialog showing all pixmaps
  //
  QDialog* dlg = new QDialog(this, "Dialog", true);
  dlg->setCaption( tr("Choose pixmap") );
  dlg->setSizeGripEnabled( true );
  QGridLayout* layout = new QGridLayout( dlg, 1, 1, 11, 6 ); 

  QIconView* iconView = new QIconView( dlg );
  iconView->setItemsMovable( false );
  iconView->setWordWrapIconText( false );
  iconView->setResizeMode(QIconView::Adjust);
  iconView->setGridX(50);
  iconView->setGridY(50);
  layout->addWidget( iconView, 0, 0 );

  QIconViewItem* item;
  QStringList names = BitmapFactory().pixmapNames();
  for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it )
  {
    item = new QIconViewItem( iconView );
    item->setPixmap( BitmapFactory().pixmap( (*it).latin1() ) );
    item->setText( *it );
  }

  QHBoxLayout* hbox = new QHBoxLayout( 0, 0, 6 ); 

  // Ok button
  QPushButton* buttonOk = new QPushButton( dlg, "buttonOk" );
  buttonOk->setAutoDefault( true );
  buttonOk->setDefault( true );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  hbox->addWidget( buttonOk );

  QSpacerItem* spacer = new QSpacerItem( 50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  hbox->addItem( spacer );

  // Cancel button
  QPushButton* buttonCancel = new QPushButton( dlg, "buttonCancel" );
  buttonCancel->setAutoDefault( true );
  hbox->addWidget( buttonCancel );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );

  layout->addLayout( hbox, 1, 0 );
  dlg->resize( QSize(400, 280) );
  
  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), dlg, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), dlg, SLOT( reject() ) );
  connect( iconView, SIGNAL( clicked ( QIconViewItem * ) ), dlg, SLOT( accept() ) );

  dlg->exec();

  PixmapLabel->clear();
  m_sPixmap = QString::null;
  if ( dlg->result() == QDialog::Accepted )
  {
    QIconViewItem* item = iconView->currentItem();

    if ( item )
    {
      m_sPixmap = item->text();
      PixmapLabel->setPixmap( *item->pixmap() );
    }
  }

  // delete this dialog with all its children
  delete dlg;
}

void DlgCustomActionsImp::newActionName()
{
  int id = 0;
  QString sName;
  bool bUsed;

  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*> aclCurMacros = rclMan.GetGroupCommands("Macros");

  do
  {
    bUsed = false;
    sName = QString("Std_Macro_%1").arg( id++ );

    std::vector<FCCommand*>::iterator it;
    for ( it = aclCurMacros.begin(); it!= aclCurMacros.end(); ++it )
    {
      if ( sName == (*it)->GetName() )
      {
        bUsed = true;
        break;
      }
    }
  }
  while ( bUsed );

  actionName->setText( sName );
}


#include "DlgActions.cpp"
#include "moc_DlgActions.cpp"
#include "moc_DlgActionsImp.cpp"
