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
# include <qcombobox.h>
# include <qlabel.h>
# include <qlistbox.h>
# include <qstringlist.h> 
#endif

#include "DlgKeyboardImp.h"
#include "Application.h"
#include "Command.h"
#include "Widgets.h"

using namespace Gui::Dialog;

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
  CommandManager & cCmdMgr = Application::Instance->commandManager();
  std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();

  for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    QString natv = it->second->getGroupName();
    QString lang = QObject::tr(natv);
    if ( _cmdGroups.find( lang ) == _cmdGroups.end() )
      _cmdGroups[ lang ] = natv;
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
  CommandManager & cCmdMgr = Application::Instance->commandManager();
  Command* cmd = cCmdMgr.getCommandByName( txt.latin1() );
  if ( cmd && cmd->getAction() )
  {
    QKeySequence ks = cmd->getAction()->accel();
    if ( ks.isEmpty() )
      accelLineEditShortcut->setText( tr("Not defined") );
    else
      accelLineEditShortcut->setText( ks );

    textLabelDescription->setText( cmd->getAction()->toolTip() );
  }
  else
    textLabelDescription->setText( tr("Not available") );
}

/** Shows all commands of this category */
void DlgCustomKeyboardImp::onGroupSelected(const QString & group)
{
  listBoxCommands->clear();

  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( group );

  if ( It != _cmdGroups.end() )
  {
    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> aCmds = cCmdMgr.getGroupCommands( It.data().latin1() );
    for (std::vector<Command*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it)
    {
      listBoxCommands->insertItem( (*it)->getName() );
    }
  }
}

void DlgCustomKeyboardImp::onAssign()
{
}

void DlgCustomKeyboardImp::onReset()
{
}

void DlgCustomKeyboardImp::onResetAll()
{
}

void DlgCustomKeyboardImp::onShortcutPressed( const QString& sc )
{
  listBoxAssigned->clear();
  CommandManager & cCmdMgr = Application::Instance->commandManager();
  std::vector<Command*> cmd = cCmdMgr.getAllCommands();

  int i=0;
  QString cmdName;
  QKeySequence ks(sc);
  if ( !ks.isEmpty() )
  {
    for ( std::vector<Command*>::iterator it = cmd.begin(); it != cmd.end(); ++it )
    {
      if ( (*it)->getAction() && (*it)->getAction()->accel() == ks )
      {
        i++;
        cmdName = (*it)->getName(); // store the last one
        listBoxAssigned->insertItem( (*it)->getName() );
      }
    }

    if ( i > 1 )
    {
      QMessageBox::warning( this, tr("Multiple defined shortcut"), 
                            tr("The shortcut '%1' is defined more than once. This could result into unexpected behaviour.").arg(sc) );
      accelLineEdit1NewShortcut->setFocus();
    }
    else if ( i == 1 && cmdName != listBoxCommands->currentText() )
    {
      QMessageBox::warning( this, tr("Already defined shortcut"), 
                            tr("The shortcut '%1' is already assigned to '%2'.\n\nPlease define another shortcut.").arg(sc).arg(cmdName) );
      accelLineEdit1NewShortcut->setFocus();
    }
  }
}

#include "DlgKeyboard.cpp"
#include "moc_DlgKeyboard.cpp"
