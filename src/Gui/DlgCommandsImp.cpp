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
# include <qlabel.h>
#endif

#include "DlgCommandsImp.h"
#include "Application.h"
#include "Command.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgCustomCommandsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomCommandsImp::DlgCustomCommandsImp( QWidget* parent, const char* name, WFlags fl  )
: DlgCustomCommandsBase(parent, name, fl)
{
  IconView1->setHScrollBarMode( QScrollView::AlwaysOff );

  // paints for active and inactive the same color
  QPalette pal = ComboBoxCategory->palette();
  pal.setInactive( pal.active() );
  ComboBoxCategory->setPalette( pal );

  connect(IconView1, SIGNAL(emitSelectionChanged(const QString &)), this, SLOT(onDescription(const QString &)));
  connect(ComboBoxCategory, SIGNAL(highlighted ( const QString & )), this, SLOT(onGroupSelected(const QString &)));

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  QMap<QString, int> cmdGroups;
  for (std::map<std::string,FCCommand*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    cmdGroups[ it->second->GetGroupName() ]++;
  }

  for ( QMap<QString, int>::Iterator it2 = cmdGroups.begin(); it2 != cmdGroups.end(); ++it2)
  {
    ComboBoxCategory->insertItem( it2.key() );
  }

  ComboBoxCategory->setCurrentItem( 0 );
}

/** Destroys the object and frees any allocated resources */
DlgCustomCommandsImp::~DlgCustomCommandsImp()
{
}

/** Shows the description for the corresponding command */
void DlgCustomCommandsImp::onDescription(const QString& txt)
{
  TextLabel->setText( txt );
}

/** Shows all commands of this category */
void DlgCustomCommandsImp::onGroupSelected(const QString & group)
{
  IconView1->clear();
 
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*> aCmds = cCmdMgr.GetGroupCommands( group.latin1() );
  for (std::vector<FCCommand*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it)
  {
    (void) new Gui::CommandViewItem(IconView1, (*it)->GetName(), (*it)->GetAction());
  }
}

void DlgCustomCommandsImp::showEvent( QShowEvent* e )
{
  DlgCustomCommandsBase::showEvent( e );

  // try to update the command view
  if ( !ComboBoxCategory->findItem("Macros", 0) )
  {
    FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
    std::vector<FCCommand*> aclCurMacros = rclMan.GetGroupCommands("Macros");
    if ( aclCurMacros.size() > 0)
    {
      ComboBoxCategory->insertItem("Macros");
      ComboBoxCategory->sort();
    }
  }
}

#include "DlgCommands.cpp"
#include "moc_DlgCommands.cpp"
#include "moc_DlgCommandsImp.cpp"
