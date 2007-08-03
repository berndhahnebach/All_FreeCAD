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

#include "DlgCommandsImp.h"
#include "Application.h"
#include "Command.h"
#include "BitmapFactory.h"
#include "Widgets.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgCustomCommandsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomCommandsImp::DlgCustomCommandsImp( QWidget* parent  )
  : CustomizeActionPage(parent)
{
  this->setupUi(this);
  
  // paints for active and inactive the same color
  QPalette pal = listBoxCategory->palette();
  pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight));
  pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText));
  listBoxCategory->setPalette( pal );

  connect(IconView1, SIGNAL(emitSelectionChanged(const QString &)), this, SLOT(onDescription(const QString &)));
  connect(listBoxCategory, SIGNAL(highlighted ( const QString & )), this, SLOT(onGroupSelected(const QString &)));

  CommandManager & cCmdMgr = Application::Instance->commandManager();
  std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();

  for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    QString natv = it->second->getGroupName();
    QString lang = QObject::tr(natv.toUtf8());
    if ( _cmdGroups.find( lang ) == _cmdGroups.end() )
      _cmdGroups[ lang ] = natv;
  }

  // do a special sort before adding to the combobox
  QStringList items, tmp; tmp << "File" << "Edit" << "View" << "Standard-View" << "Tools" << "Window" << "Help" << "Macros";
  for ( QStringList::Iterator It = tmp.begin(); It != tmp.end(); ++It )
    items << QObject::tr( (*It).toUtf8() );
  for ( QMap<QString, QString>::Iterator it2 = _cmdGroups.begin(); it2 != _cmdGroups.end(); ++it2)
  {
    if ( !items.contains( it2.key() ) )
      items << it2.key();
  }

  for ( QStringList::Iterator It = items.begin(); It != items.end(); ++It )
    listBoxCategory->insertItem(*It);
  listBoxCategory->setCurrentItem( 0 );
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

  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( group );

  if ( It != _cmdGroups.end() )
  {
    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> aCmds = cCmdMgr.getGroupCommands( It.value().toLatin1() );
    for (std::vector<Command*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it)
    {
      QPixmap pix;
      if ( (*it)->getPixmap() )
        pix = BitmapFactory().pixmap( (*it)->getPixmap() );
      else 
        pix = BitmapFactory().pixmap( "px" );
      QListWidgetItem* item = new QListWidgetItem(IconView1);
      item->setText((*it)->getName());
      item->setToolTip(QObject::tr((*it)->getToolTipText()));
      item->setIcon(pix);
      //IconView1->addItem(item);
    }
  }
}

void DlgCustomCommandsImp::onAddMacroAction(const QString& item)
{
  onGroupSelected(listBoxCategory->currentText());
#if 0 //TODO
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( "Macros" );
  int index = listBoxCategory->currentItem();
  QString current = listBoxCategory->text(index);
  if ( It == _cmdGroups.end() || It.data() != current )
    return;
  CommandManager & cCmdMgr = Application::Instance->commandManager();
  Command* pCmd = cCmdMgr.getCommandByName(item.ascii());
  QPixmap pix;
  if ( pCmd->getPixmap() )
    pix = BitmapFactory().pixmap( pCmd->getPixmap() );
  else 
    pix = BitmapFactory().pixmap( "px" );
  QListWidgetItem* viewItem = new QListWidgetItem(IconView1);
  viewItem->setText(pCmd->getName());
  viewItem->setToolTip(QObject::tr(pCmd->getToolTipText()));
  viewItem->setIcon(pix);
  IconView1->sortItems();
//  (void) new Gui::CommandViewItem(IconView1, item, QObject::tr( pCmd->getToolTipText() ), pix);
//  IconView1->sort();
#endif
}

void DlgCustomCommandsImp::onRemoveMacroAction(const QString& item)
{
  onGroupSelected(listBoxCategory->currentText());
#if 0
  QMap<QString, QString>::ConstIterator It = _cmdGroups.find( "Macros" );
  int index = listBoxCategory->currentItem();
  QString current = listBoxCategory->text(index);
  if ( It.data() != current )
    return;
  //Q3IconViewItem* icon = IconView1->firstItem();
  //while (icon) {
  //  if (icon->Q3IconViewItem::text() == item) {
  //    IconView1->takeItem(icon);
  //    IconView1->sort();
  //    break;
  //  } else {
  //    icon = icon->nextItem();
  //  }
  //}
#endif
}

#include "moc_DlgCommandsImp.cpp"
