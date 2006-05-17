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

#ifndef _PreComp_
# include <qaction.h>
# include <qcombobox.h>
# include <qkeysequence.h>
# include <qlabel.h>
# include <qlistbox.h>
# include <qstringlist.h> 
#endif

#include <Base/Parameter.h>

#include "DlgKeyboardImp.h"
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
DlgCustomKeyboardImp::DlgCustomKeyboardImp( QWidget* parent, const char* name, WFlags fl  )
: DlgCustomKeyboardBase(parent, name, fl)
{
  const CommandManager& cCmdMgr = Application::Instance->commandManager();
  const std::map<std::string,Command*>& sCommands = cCmdMgr.getCommands();

  for (std::map<std::string,Command*>::const_iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    QString natv = it->second->getGroupName();
    QString lang = QObject::tr(natv);
    if ( _cmdGroups.find( lang ) == _cmdGroups.end() )
      _cmdGroups[ lang ] = natv;

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
    {
      _groupCommands[it->second->getGroupName()][it->second->getName()] = it->second;
    }
  }

  // do a special sort before adding to the combobox
  QStringList items, tmp; tmp << "File" << "Edit" << "View" << "Standard-View" << "Tools" << "Window" << "Help" << "Macros";
  for ( QStringList::Iterator It = tmp.begin(); It != tmp.end(); ++It )
    items << QObject::tr( *It );
  for ( QMap<QString, QString>::Iterator it2 = _cmdGroups.begin(); it2 != _cmdGroups.end(); ++it2)
  {
    if ( items.find( it2.key() ) == items.end() )
      items << it2.key();
  }

  comboBoxCategory->insertStringList( items );

  onGroupSelected( comboBoxCategory->currentText() );
}

/** Destroys the object and frees any allocated resources */
DlgCustomKeyboardImp::~DlgCustomKeyboardImp()
{
}

/** Shows the description for the corresponding command */
void DlgCustomKeyboardImp::onDescription(const QString& txt)
{
  CommandBase* cmd = 0;
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.data() );
    if ( ci != _groupCommands.end() )
    {
      QMap<QString, CommandBase*>::ConstIterator e = ci.data().find( txt );
      if ( e != ci.data().end() )
        cmd = e.data();
    }
  }

  if ( !cmd ) return; // not found

  // is QAction already created?
  if ( cmd->getAction() )
  {
    QKeySequence ks = cmd->getAction()->accel();
    QKeySequence ks2 = cmd->getAccel();
    QKeySequence ks3 = accelLineEdit1NewShortcut->text();

    if ( ks.isEmpty() )
      accelLineEditShortcut->setText( tr("Not defined") );
    else
      accelLineEditShortcut->setText( ks );

    pushButtonAssign->setEnabled( !ks3.isEmpty() && ( ks != ks3 ) );
    pushButtonReset->setEnabled( (ks != ks2) );
  }
  else
  {
    QKeySequence ks = cmd->getAccel();
    if ( ks.isEmpty() )
      accelLineEditShortcut->setText( tr("Not defined") );
    else
      accelLineEditShortcut->setText( ks );
    pushButtonAssign->setEnabled( false );
    pushButtonReset->setEnabled( false );
  }

  textLabelDescription->setText( QObject::tr( cmd->getToolTipText() ) );
}

/** Shows all commands of this category */
void DlgCustomKeyboardImp::onGroupSelected(const QString & group)
{
  listBoxCommands->clear();
  pushButtonAssign->setEnabled( false );
  pushButtonReset->setEnabled( false );

  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( group );

  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.data() );
    if ( ci != _groupCommands.end() )
    {
      const QMap<QString, CommandBase*>& items = ci.data();
      for ( QMap<QString, CommandBase*>::ConstIterator it = items.begin(); it != items.end(); ++it)
      {
        QPixmap px = (it.data()->getPixmap() ? BitmapFactory().pixmap( it.data()->getPixmap() ) : QPixmap() );
        listBoxCommands->insertItem( Tools::fillUp(24,24,px), it.key() );
      }
    }
  }
}

/** Assigns a new accelerator to the selected command. */
void DlgCustomKeyboardImp::onAssign()
{
  CommandBase* cmd = 0;
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.data() );
    if ( ci != _groupCommands.end() )
    {
      QMap<QString, CommandBase*>::ConstIterator e = ci.data().find( listBoxCommands->currentText() );
      if ( e != ci.data().end() )
        cmd = e.data();
    }
  }

  if ( cmd && cmd->getAction() )
  {
    QKeySequence shortcut = accelLineEdit1NewShortcut->text();
    cmd->getAction()->setAccel( shortcut );
    accelLineEditShortcut->setText( accelLineEdit1NewShortcut->text() );

    const char* curText = listBoxCommands->currentText().latin1();
    ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Shortcut");
    hGrp->SetASCII( curText, accelLineEdit1NewShortcut->text().latin1() );

    pushButtonReset->setEnabled( true );
  }
}

/** Resets the accelerator of the selected command to the default. */
void DlgCustomKeyboardImp::onReset()
{
  CommandBase* cmd = 0;
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
  if ( It != _cmdGroups.end() )
  {
    QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.data() );
    if ( ci != _groupCommands.end() )
    {
      QMap<QString, CommandBase*>::ConstIterator e = ci.data().find( listBoxCommands->currentText() );
      if ( e != ci.data().end() )
        cmd = e.data();
    }
  }

  if ( cmd && cmd->getAction() )
  {
    cmd->getAction()->setAccel( cmd->getAccel() );
    QString txt = cmd->getAction()->accel();
    accelLineEditShortcut->setText( (txt.isEmpty() ? tr("Not defined") : txt) );
    ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Shortcut");
    const char* curText = listBoxCommands->currentText().latin1();
    hGrp->RemoveASCII( curText );
  }

  pushButtonReset->setEnabled( false );
}

/** Resets the accelerator of all commands to the default. */
void DlgCustomKeyboardImp::onResetAll()
{
  for ( QMap<QString, QMap<QString, CommandBase*> >::ConstIterator it = _groupCommands.begin(); it != _groupCommands.end(); ++it )
  {
    const QMap<QString, CommandBase*>& items = it.data();
    for ( QMap<QString, CommandBase*>::ConstIterator ci = items.begin(); ci != items.end(); ++ci )
    {
      if ( ci.data()->getAction() )
      {
        ci.data()->getAction()->setAccel( ci.data()->getAccel() );
      }
    }
  }

  WindowParameter::getDefaultParameter()->RemoveGrp("Shortcut");
  pushButtonReset->setEnabled( false );
}

/** Checks for an already occupied shortcut. */
void DlgCustomKeyboardImp::onShortcutPressed( const QString& sc )
{
  listBoxAssigned->clear();

  int i=0;
  QString cmdName;
  QKeySequence ks(sc);
  if ( !ks.isEmpty() )
  {
    pushButtonAssign->setEnabled( true );
    for ( QMap<QString, QMap<QString, CommandBase*> >::ConstIterator it = _groupCommands.begin(); it != _groupCommands.end(); ++it )
    {
      const QMap<QString, CommandBase*>& items = it.data();
      for ( QMap<QString, CommandBase*>::ConstIterator ci = items.begin(); ci != items.end(); ++ci )
      {
        if ( ci.data()->getAction() && ci.data()->getAction()->accel() == ks )
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
      accelLineEdit1NewShortcut->setFocus();
      pushButtonAssign->setEnabled( false );
    }
    else if ( i == 1 && cmdName != listBoxCommands->currentText() )
    {
      QMessageBox::warning( this, tr("Already defined shortcut"),
                            tr("The shortcut '%1' is already assigned to '%2'.\n\nPlease define another shortcut.").arg(sc).arg(cmdName) );
      accelLineEdit1NewShortcut->setFocus();
      pushButtonAssign->setEnabled( false );
    }
    else // check if the current selected command has already this shortcut defined
    {
      QMap<QString, QString>::ConstIterator It = _cmdGroups.find( comboBoxCategory->currentText() );
      if ( It != _cmdGroups.end() )
      {
        QMap<QString, QMap<QString, CommandBase*> >::ConstIterator ci = _groupCommands.find( It.data() );
        if ( ci != _groupCommands.end() )
        {
          QMap<QString, CommandBase*>::ConstIterator e = ci.data().find( listBoxCommands->currentText() );
          if ( e != ci.data().end() )
          {
            CommandBase* cmd = e.data();
            if ( cmd && cmd->getAction() && cmd->getAction()->accel() == ks )
              pushButtonAssign->setEnabled( false );
          }
        }
      }
    }
  }
  else
    pushButtonAssign->setEnabled( false );
}

#include "DlgKeyboard.cpp"
#include "moc_DlgKeyboard.cpp"
