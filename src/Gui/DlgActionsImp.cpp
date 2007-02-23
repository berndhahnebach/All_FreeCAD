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

#include "DlgActionsImp.h"
#include "Action.h"
#include "Application.h"
#include "Tools.h"
#include "Command.h"
#include "BitmapFactory.h"
#include "Widgets.h"
#include "ui_DlgChooseIcon.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomActionsImp */

/**
 *  Constructs a DlgCustomActionsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomActionsImp::DlgCustomActionsImp( QWidget* parent )
  : CustomizeActionPage(parent), bShown( false )
{
  this->setupUi(this);
  // search for all macros
  std::string cMacroPath = App::GetApplication().
    GetParameterGroupByPath("User parameter:BaseApp/Preferences/Macro")->GetASCII("MacroPath",App::GetApplication().GetHomePath());

  QDir d(cMacroPath.c_str(),"*.FCMacro");
  actionMacros->insertStringList(d.entryList());

  showActions();
}

/** Destroys the object and frees any allocated resources */
DlgCustomActionsImp::~DlgCustomActionsImp()
{
}

/** 
 * Displays this page. If no macros were found a message box
 * appears.
 */
void DlgCustomActionsImp::showEvent(QShowEvent* e)
{
  QWidget::showEvent(e);
  if (actionMacros->count() == 0 && bShown == false)
  {
    bShown = true;
    QMessageBox::warning(this, tr("No macro"),tr("No macros found."));
  }
}

bool DlgCustomActionsImp::event(QEvent* e)
{
  bool ok = QWidget::event(e);

  if (e->type() == QEvent::ParentChange || e->type() == QEvent::ParentAboutToChange)
  {
    QWidget* topLevel = this->parentWidget();
    while (topLevel && !topLevel->inherits("QDialog"))
      topLevel = topLevel->parentWidget();
    if ( topLevel )
    {
      int index = topLevel->metaObject()->indexOfSignal( QMetaObject::normalizedSignature("addMacroAction(const QString&)") );
      if ( index >= 0 ) {
        if ( e->type() == QEvent::ParentChange ) {
          connect(this, SIGNAL(addMacroAction( const QString& )), topLevel, SIGNAL(addMacroAction( const QString& )));
          connect(this, SIGNAL(removeMacroAction( const QString& )), topLevel, SIGNAL(removeMacroAction( const QString& )));
        } else {
          disconnect(this, SIGNAL(addMacroAction( const QString& )), topLevel, SIGNAL(addMacroAction( const QString& )));
          disconnect(this, SIGNAL(removeMacroAction( const QString& )), topLevel, SIGNAL(removeMacroAction( const QString& )));
        }
      }
    }
  }

  return ok;
}

void DlgCustomActionsImp::onAddMacroAction(const QString&)
{
  // does nothing
}

void DlgCustomActionsImp::onRemoveMacroAction(const QString&)
{
  // does nothing
}

void DlgCustomActionsImp::showActions()
{
  CommandManager& rclMan = Application::Instance->commandManager();
  std::vector<Command*> aclCurMacros = rclMan.getGroupCommands("Macros");
  for (std::vector<Command*>::iterator it = aclCurMacros.begin(); it != aclCurMacros.end(); ++it)
  {
    if ( (*it)->getPixmap() )
      listBoxActions->insertItem(BitmapFactory().pixmap((*it)->getPixmap()), (*it)->getName());
    else
      listBoxActions->insertItem((*it)->getName());
  }
}

void DlgCustomActionsImp::on_listBoxActions_highlighted( Q3ListBoxItem *i )
{
  if ( !i ) return; // no valid item

  // search for the command in the manager and if necessary in the temporary created ones
  QString actionName = i->text();
  CommandManager& rclMan = Application::Instance->commandManager();
  Command* pCmd = rclMan.getCommandByName(actionName.latin1());
  MacroCommand* pScript = dynamic_cast<MacroCommand*>(pCmd);

  // if valid command
  if ( pScript )
  {
    bool bFound = false;
    for (int i = 0; i<actionMacros->count(); i++)
    {
      if (actionMacros->text(i).startsWith(pScript->getScriptName()))
      {
        bFound = true;
        actionMacros->setCurrentItem(i);
        break;
      }
    }

    if (!bFound)
    {
      QMessageBox::critical(this, tr("Macro not found"), tr("Sorry, couldn't find macro file '%1'.").arg(pScript->getScriptName()));
    }

    // fill up labels with the command's data
    actionWhatsThis -> setText( pScript->getWhatsThis() );
    actionMenu      -> setText( pScript->getMenuText() );
    actionToolTip   -> setText( pScript->getToolTipText() );
    actionStatus    -> setText( pScript->getStatusTip() );
    actionAccel     -> setText( Q3Accel::keyToString(pScript->getAccel()) );
    pixmapLabel->clear();
    m_sPixmap = QString::null;
    const char* name = pScript->getPixmap();
    if ( name && strlen(name) > 2)
    {
      QPixmap p = Gui::BitmapFactory().pixmap( pScript->getPixmap() );
      pixmapLabel->setPixmap(p);
      m_sPixmap = name;
    }
  }
}

void DlgCustomActionsImp::on_buttonAddAction_clicked()
{
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
  QString actionName = newActionName();
  CommandManager& rclMan = Application::Instance->commandManager();
  MacroCommand* macro = new MacroCommand(actionName.latin1());
  rclMan.addCommand( macro );

  // add new action
  if ( pixmapLabel->pixmap() )
  {
    QPixmap p = *pixmapLabel->pixmap();
    listBoxActions->insertItem(Tools::fillUp(24,24,p), actionName);
  }
  else
  {
    listBoxActions->insertItem(actionName);
  }

  if ( !actionWhatsThis->text().isEmpty() )
    macro->setWhatsThis( actionWhatsThis->text() );
  actionWhatsThis->clear();
  
  if ( !actionMacros-> currentText().isEmpty() )
    macro->setScriptName( actionMacros-> currentText() );
  
  if ( !actionMenu->text().isEmpty() )
    macro->setMenuText( actionMenu->text() );
  actionMenu->clear();

  if ( !actionToolTip->text().isEmpty() )
    macro->setToolTipText( actionToolTip->text() );
  actionToolTip->clear();

  if ( !actionStatus->text().isEmpty() )
    macro->setStatusTip( actionStatus->text() );
  actionStatus->clear();

  if ( !m_sPixmap.isEmpty() )
    macro->setPixmap( m_sPixmap );
  pixmapLabel->clear();
  m_sPixmap = QString::null;

  if ( !actionAccel->text().isEmpty() )
    macro->setAccel( Q3Accel::stringToKey( actionAccel->text() ) );
  actionAccel->clear();

  // check whether the macro is already in use
  Action* action = macro->getAction();
  if ( action )
  {
    // does all the text related stuff
    macro->languageChange();
    if( macro->getPixmap() )
      action->setIcon(Gui::BitmapFactory().pixmap(macro->getPixmap()));
    action->setShortcut(macro->getAccel());
  }

  // emit signal to notify the container widget
  addMacroAction( actionName );
}

void DlgCustomActionsImp::on_buttonReplaceAction_clicked()
{
  int index = listBoxActions->currentItem();
  Q3ListBoxItem* item = listBoxActions->item(index);
  if (!item)
  {
    QMessageBox::warning(this, tr("No item selected"),tr("Please select a macro item first."));
    return;
  }

  if (actionMenu->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Empty text"),tr("Please specify the menu text first."));
    return;
  }

  // search for the command in the manager
  QString actionName = item->text();
  CommandManager& rclMan = Application::Instance->commandManager();
  Command* pCmd = rclMan.getCommandByName(actionName.latin1());
  MacroCommand* macro = dynamic_cast<MacroCommand*>(pCmd);

  if ( !actionWhatsThis->text().isEmpty() )
    macro->setWhatsThis( actionWhatsThis->text() );
  actionWhatsThis->clear();
  
  if ( !actionMacros-> currentText().isEmpty() )
    macro->setScriptName( actionMacros-> currentText() );
  
  if ( !actionMenu->text().isEmpty() )
    macro->setMenuText( actionMenu->text() );
  actionMenu->clear();

  if ( !actionToolTip->text().isEmpty() )
    macro->setToolTipText( actionToolTip->text() );
  actionToolTip->clear();

  if ( !actionStatus->text().isEmpty() )
    macro->setStatusTip( actionStatus->text() );
  actionStatus->clear();

  if ( !m_sPixmap.isEmpty() )
    macro->setPixmap( m_sPixmap );
  pixmapLabel->clear();
  m_sPixmap = QString::null;

  if ( !actionAccel->text().isEmpty() )
    macro->setAccel( Q3Accel::stringToKey( actionAccel->text() ) );
  actionAccel->clear();

  // check whether the macro is already in use
  Action* action = macro->getAction();
  if ( action )
  {
    // does all the text related stuff
    macro->languageChange();
    if( macro->getPixmap() )
      action->setIcon(Gui::BitmapFactory().pixmap(macro->getPixmap()));
    action->setShortcut(macro->getAccel());
  }

  // emit signal to notify the container widget
  replaceMacroAction( actionName );
  
  // call this at the end because it internally invokes the highlight method
  if ( macro->getPixmap() )
  {
    QPixmap p = Gui::BitmapFactory().pixmap( macro->getPixmap() );
    listBoxActions->changeItem(Tools::fillUp(24,24,p), listBoxActions->text(index), index);
  }
}

void DlgCustomActionsImp::on_buttonRemoveAction_clicked()
{
  // remove item from list view
  int current = listBoxActions->currentItem();
  QString itemText = listBoxActions->text(current);
  listBoxActions->removeItem(current);

  // if the command is registered in the manager just remove it
  CommandManager& rclMan = Application::Instance->commandManager();
  std::vector<Command*> aclCurMacros = rclMan.getGroupCommands("Macros");
  for (std::vector<Command*>::iterator it2 = aclCurMacros.begin(); it2!= aclCurMacros.end(); ++it2)
  {
    if ( itemText == (*it2)->getName() )
    {
      // emit signal to notify the container widget
      removeMacroAction( itemText );
      // remove from manager and delete it immediately
      rclMan.removeCommand( *it2 );
      break;
    }
  }
}

void DlgCustomActionsImp::on_buttonChoosePixmap_clicked()
{
  // create a dialog showing all pixmaps
  Gui::Dialog::Ui_DlgChooseIcon ui;
  QDialog dlg(this, "Dialog", true);
  ui.setupUi(&dlg);
  // signals and slots connections
  connect( ui.iconView, SIGNAL( clicked ( Q3IconViewItem * ) ), &dlg, SLOT( accept() ) );

  Q3IconViewItem* item;
  QStringList names = BitmapFactory().pixmapNames();
  for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it )
  {
    item = new Q3IconViewItem( ui.iconView );
    item->setPixmap( BitmapFactory().pixmap( (*it).latin1() ) );
    item->setText( *it );
  }

  dlg.exec();

  pixmapLabel->clear();
  m_sPixmap = QString::null;
  if ( dlg.result() == QDialog::Accepted )
  {
    Q3IconViewItem* item = ui.iconView->currentItem();

    if ( item )
    {
      m_sPixmap = item->text();
      pixmapLabel->setPixmap( *item->pixmap() );
    }
  }
}

QString DlgCustomActionsImp::newActionName()
{
  int id = 0;
  QString sName;
  bool bUsed;

  CommandManager& rclMan = Application::Instance->commandManager();
  std::vector<Command*> aclCurMacros = rclMan.getGroupCommands("Macros");

  do
  {
    bUsed = false;
    sName = QString("Std_Macro_%1").arg( id++ );

    std::vector<Command*>::iterator it;
    for ( it = aclCurMacros.begin(); it!= aclCurMacros.end(); ++it )
    {
      if ( sName == (*it)->getName() )
      {
        bUsed = true;
        break;
      }
    }
  }
  while ( bUsed );

  return sName;
}


#include "moc_DlgActionsImp.cpp" 
