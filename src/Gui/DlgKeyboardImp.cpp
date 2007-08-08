/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include <Base/Parameter.h>

#include "DlgKeyboardImp.h"
#include "Action.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Command.h"
#include "Tools.h"
#include "Widgets.h"
#include "Window.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomKeyboardImp */

/**
 *  Constructs a DlgCustomKeyboardImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomKeyboardImp::DlgCustomKeyboardImp( QWidget* parent  )
  : CustomizeActionPage(parent)
{
  this->setupUi(this);
  
  const CommandManager& cCmdMgr = Application::Instance->commandManager();
  const std::map<std::string,Command*>& sCommands = cCmdMgr.getCommands();

  for (std::map<std::string,Command*>::const_iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    QString natv = it->second->getGroupName();
    QString lang = QObject::tr(natv.toAscii());
    if ( _cmdGroups.find( lang ) == _cmdGroups.end() )
      _cmdGroups[ lang ] = natv;

#if 0 //TODO Reimplement
    CommandGroup* cmdGrp = dynamic_cast<CommandGroup*>(it->second);
    if ( cmdGrp )
    {
      int i=0;
      std::vector<CommandItem*> items = cmdGrp->getItems();
      for ( std::vector<CommandItem*>::const_iterator ci = items.begin(); ci != items.end(); ++ci )
      {
        QString sName = QString("%1_%2").arg(it->second->getName()).arg(i++);
        _groupCommands[it->second->getGroupName()][sName] = *ci;
      }
    }
    else
#endif
    {
      _groupCommands[it->second->getGroupName()][it->second->getName()] = it->second;
    }
  }

  // do a special sort before adding to the combobox
  QStringList items, tmp; tmp << "File" << "Edit" << "View" << "Standard-View" << "Tools" << "Window" << "Help" << "Macros";
  for ( QStringList::Iterator It = tmp.begin(); It != tmp.end(); ++It )
    items << QObject::tr((*It).toAscii());
  for ( QMap<QString, QString>::Iterator it2 = _cmdGroups.begin(); it2 != _cmdGroups.end(); ++it2)
  {
    if (!items.contains(it2.key()))
      items << it2.key();
  }

  comboBoxCategory->addItems( items );

  on_comboBoxCategory_activated( comboBoxCategory->currentText() );
}

/** Destroys the object and frees any allocated resources */
DlgCustomKeyboardImp::~DlgCustomKeyboardImp()
{
}

/** Shows the description for the corresponding command */
void DlgCustomKeyboardImp::on_listBoxCommands_highlighted(const QString& txt)
{
  CommandBase* cmd = 0;
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.value() );
    if ( ci != _groupCommands.end() )
    {
      QMap<QString, CommandBase*>::ConstIterator e = ci.value().find( txt );
      if ( e != ci.value().end() )
        cmd = e.value();
    }
  }

  if ( !cmd ) return; // not found

  // is QAction already created?
  if ( cmd->getAction() )
  {
    QKeySequence ks = cmd->getAction()->shortcut();
    QKeySequence ks2 = cmd->getAccel();
    QKeySequence ks3 = editShortcut->text();

    if ( ks.isEmpty() )
      accelLineEditShortcut->setText( tr("Not defined") );
    else
      accelLineEditShortcut->setText( ks );

    buttonAssign->setEnabled( !ks3.isEmpty() && ( ks != ks3 ) );
    buttonReset->setEnabled( (ks != ks2) );
  }
  else
  {
    QKeySequence ks = cmd->getAccel();
    if ( ks.isEmpty() )
      accelLineEditShortcut->setText( tr("Not defined") );
    else
      accelLineEditShortcut->setText( ks );
    buttonAssign->setEnabled( false );
    buttonReset->setEnabled( false );
  }

  textLabelDescription->setText( QObject::tr( cmd->getToolTipText() ) );
}

/** Shows all commands of this category */
void DlgCustomKeyboardImp::on_comboBoxCategory_activated(const QString & group)
{
#if 0
  listBoxCommands->clear();
  buttonAssign->setEnabled( false );
  buttonReset->setEnabled( false );

  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( group );

  if ( It != _cmdGroups.end() )
  {
    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> aCmds = cCmdMgr.getGroupCommands( It.data().latin1() );
    for (std::vector<Command*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it)
    {
      QPixmap px = ((*it)->getPixmap() ? BitmapFactory().pixmap( (*it)->getPixmap() ) : QPixmap() );
      listBoxCommands->insertItem( Tools::fillUp(24,24,px), (*it)->getName() );
    }
  }
#else //FIXME Implement
  listBoxCommands->clear();
  buttonAssign->setEnabled( false );
  buttonReset->setEnabled( false );

  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( group );

  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.value() );
    if ( ci != _groupCommands.end() )
    {
      const QMap<QString, CommandBase*>& items = ci.value();
      for ( QMap<QString, CommandBase*>::ConstIterator it = items.begin(); it != items.end(); ++it)
      {
        QPixmap px = (it.value()->getPixmap() ? BitmapFactory().pixmap( it.value()->getPixmap() ) : QPixmap() );
        listBoxCommands->insertItem( Tools::fillUp(24,24,px), it.key() );
      }
    }
  }
#endif
}

/** Assigns a new accelerator to the selected command. */
void DlgCustomKeyboardImp::on_buttonAssign_clicked()
{
  CommandBase* cmd = 0;
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.value() );
    if ( ci != _groupCommands.end() )
    {
      QMap<QString, CommandBase*>::ConstIterator e = ci.value().find( listBoxCommands->currentText() );
      if ( e != ci.value().end() )
        cmd = e.value();
    }
  }

  if ( cmd && cmd->getAction() )
  {
    QKeySequence shortcut = editShortcut->text();
    cmd->getAction()->setShortcut( shortcut );
    accelLineEditShortcut->setText( editShortcut->text() );

    const char* curText = listBoxCommands->currentText().toAscii();
    ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Shortcut");
    hGrp->SetASCII( curText, editShortcut->text().toAscii() );

    buttonReset->setEnabled( true );
  }
}

/** Resets the accelerator of the selected command to the default. */
void DlgCustomKeyboardImp::on_buttonReset_clicked()
{
  CommandBase* cmd = 0;
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.value() );
    if ( ci != _groupCommands.end() )
    {
      QMap<QString, CommandBase*>::ConstIterator e = ci.value().find( listBoxCommands->currentText() );
      if ( e != ci.value().end() )
        cmd = e.value();
    }
  }

  if ( cmd && cmd->getAction() )
  {
    cmd->getAction()->setShortcut( cmd->getAccel() );
    QString txt = cmd->getAction()->shortcut();
    accelLineEditShortcut->setText( (txt.isEmpty() ? tr("Not defined") : txt) );
    ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Shortcut");
    const char* curText = listBoxCommands->currentText().toAscii();
    hGrp->RemoveASCII( curText );
  }

  buttonReset->setEnabled( false );
}

/** Resets the accelerator of all commands to the default. */
void DlgCustomKeyboardImp::on_buttonResetAll_clicked()
{
  for ( QMap<QString, QMap<QString, CommandBase*> >::ConstIterator it = _groupCommands.begin(); it != _groupCommands.end(); ++it )
  {
    const QMap<QString, CommandBase*>& items = it.value();
    for ( QMap<QString, CommandBase*>::ConstIterator ci = items.begin(); ci != items.end(); ++ci )
    {
      if ( ci.value()->getAction() )
      {
        ci.value()->getAction()->setShortcut( ci.value()->getAccel() );
      }
    }
  }

  WindowParameter::getDefaultParameter()->RemoveGrp("Shortcut");
  buttonReset->setEnabled( false );
}

/** Checks for an already occupied shortcut. */
void DlgCustomKeyboardImp::on_editShortcut_textChanged( const QString& sc )
{
  listBoxAssigned->clear();

  int i=0;
  QString cmdName;
  QKeySequence ks(sc);
  if ( !ks.isEmpty() )
  {
    buttonAssign->setEnabled( true );
    for ( QMap<QString, QMap<QString, CommandBase*> >::ConstIterator it = _groupCommands.begin(); it != _groupCommands.end(); ++it )
    {
      const QMap<QString, CommandBase*>& items = it.value();
      for ( QMap<QString, CommandBase*>::ConstIterator ci = items.begin(); ci != items.end(); ++ci )
      {
        if ( ci.value()->getAction() && ci.value()->getAction()->shortcut() == ks )
        {
          i++;
          cmdName = ci.key(); // store the last one
          listBoxAssigned->insertItem( cmdName );
        }
      }
    }

    if ( i > 1 )
    {
      QMessageBox::warning( this, tr("Multiple defined shortcut"),
                            tr("The shortcut '%1' is defined more than once. This could result into unexpected behaviour.").arg(sc) );
      editShortcut->setFocus();
      buttonAssign->setEnabled( false );
    }
    else if ( i == 1 && cmdName != listBoxCommands->currentText() )
    {
      QMessageBox::warning( this, tr("Already defined shortcut"),
                            tr("The shortcut '%1' is already assigned to '%2'.\n\nPlease define another shortcut.").arg(sc).arg(cmdName) );
      editShortcut->setFocus();
      buttonAssign->setEnabled( false );
    }
    else // check if the current selected command has already this shortcut defined
    {
      QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
      if ( It != _cmdGroups.end() )
      {
        QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.value() );
        if ( ci != _groupCommands.end() )
        {
          QMap<QString, CommandBase*>::ConstIterator e = ci.value().find( listBoxCommands->currentText() );
          if ( e != ci.value().end() )
          {
            CommandBase* cmd = e.value();
            if ( cmd && cmd->getAction() && cmd->getAction()->shortcut() == ks )
              buttonAssign->setEnabled( false );
          }
        }
      }
    }
  }
  else
    buttonAssign->setEnabled( false );
}

void DlgCustomKeyboardImp::onAddMacroAction(const QString& item)
{
  const CommandManager& cCmdMgr = Application::Instance->commandManager();
  Command* cmd = cCmdMgr.getCommandByName( item.toAscii() );
  _groupCommands[cmd->getGroupName()][cmd->getName()] = cmd;
}

void DlgCustomKeyboardImp::onRemoveMacroAction(const QString& item)
{
  const CommandManager& cCmdMgr = Application::Instance->commandManager();
  Command* cmd = cCmdMgr.getCommandByName( item.toAscii() );
  _groupCommands[cmd->getGroupName()].remove(cmd->getName());
}

#include "moc_DlgKeyboardImp.cpp"
