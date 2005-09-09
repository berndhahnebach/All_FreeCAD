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
# include <qinputdialog.h>
#endif

#include "DlgCmdbarsImp.h"
#include "Application.h"
#include "Command.h"
#include "CustomWidgets.h"
#include "CommandBarManager.h"
#include "Tools.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomCmdbarsImp */

/**
 *  Constructs a DlgCustomCmdbarsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomCmdbarsImp::DlgCustomCmdbarsImp( QWidget* parent, const char* name, WFlags fl )
  : DlgCustomToolbars(parent, name, fl)
{
  setCaption( tr( "Commandbars" ) );
  updateData();
}

/** Destroys the object and frees any allocated resources */
DlgCustomCmdbarsImp::~DlgCustomCmdbarsImp()
{
}

/** Adds created or removes deleted commandbars */
void DlgCustomCmdbarsImp::apply()
{
#ifndef NEW_WB_FRAMEWORK
  QString text = ComboToolbars->currentText();
  Gui::CustomToolBar* toolbar = ApplicationWindow::Instance->customWidgetManager()->getCommandBar(text.latin1());
  toolbar->clear();

  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();

  QStringList items;
  QListViewItem* item = ToolbarActions->firstChild();
  for (int i=0; i < ToolbarActions->childCount(); item = item->itemBelow(), i++)
  {
    if (item->text(0) == "<Separator>")
    {
      toolbar->addSeparator ();
      items.push_back("Separator");
      continue;
    }

    Command* pCom = cCmdMgr.getCommandByActionText(item->text(0).latin1());
    if (pCom != NULL)
    {
      if (pCom->addTo(toolbar))
        items.push_back( pCom->getName() );
    }
  }

  toolbar->setCustomItems( items );
  toolbar->saveXML();
#else
  // @todo add new items to parameter group
  QString text = ComboToolbars->currentText();
  QToolBar* bar = CommandBarManager::getInstance()->getOrCreateCommandBar( text );
  bar->clear();

  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();

  QStringList items;
  QListViewItem* item = ToolbarActions->firstChild();
  for (int i=0; i < ToolbarActions->childCount(); item = item->itemBelow(), i++)
  {
    if (item->text(0) == "<Separator>")
    {
      bar->addSeparator ();
      continue;
    }

    Command* pCom = cCmdMgr.getCommandByActionText(item->text(0).latin1());
    if (pCom != 0)
    {
      pCom->addTo(bar);
    }
  }
#endif
}

/** Discards all changes */
void DlgCustomCmdbarsImp::cancel()
{
}

/** Shows all actions from the last specified commandbar */
void DlgCustomCmdbarsImp::updateData()
{
#ifndef NEW_WB_FRAMEWORK
  ComboToolbars->clear();
  ToolbarActions->clear();
  _aclToolbars = ApplicationWindow::Instance->customWidgetManager()->getCommdandBars();

  Gui::CustomToolBar* bar;
  for ( bar = _aclToolbars.first(); bar; bar = _aclToolbars.next() )
  {
    if (bar->canModify())
      ComboToolbars->insertItem( bar->name() );
  }

  if (ComboToolbars->count() > 0)
  {
    onItemActivated( ComboToolbars->text( 0 ) );
  }
  else
  {
    ToolbarActions->setEnabled(false);
    ComboToolbars->setEnabled (false);
  }
#else
  ComboToolbars->clear();
  ToolbarActions->clear();
  QPtrList<QToolBar> bars = CommandBarManager::getInstance()->commandBars();

  QToolBar* bar;
  for ( bar = bars.first(); bar; bar = bars.next() )
  {
    CustomToolBar* cw = dynamic_cast<CustomToolBar*>(bar);
    if ( cw && cw->canModify())
      ComboToolbars->insertItem( cw->name() );
  }

  if (ComboToolbars->count() > 0)
  {
    onItemActivated( ComboToolbars->text( 0 ) );
  }
  else
  {
    ToolbarActions->setEnabled(false);
    ComboToolbars->setEnabled (false);
  }
#endif
}

/** Creates new commandbar */
void DlgCustomCmdbarsImp::onCreateToolbar()
{
#ifndef NEW_WB_FRAMEWORK
  QString def = QString("commandbar%1").arg(ApplicationWindow::Instance->customWidgetManager()->countCommandBars());
  QString text = QInputDialog::getText(tr("New commandbar"), tr("Specify the name of the new commandbar, please."),
                                      QLineEdit::Normal, def, 0, this);

  if (!text.isNull() && !text.isEmpty())
  {
    Gui::CustomToolBar* toolbar = ApplicationWindow::Instance->customWidgetManager()->getCommandBar(text.latin1());
    toolbar->show();
    _aclToolbars.append( toolbar );
    ComboToolbars->insertItem(text);

    // enable the widgets
    ToolbarActions->setEnabled(true);
    ComboToolbars->setEnabled (true);
  }
#else
  QString def = QString("commandbar%1").arg(CommandBarManager::getInstance()->commandBars().count()+1);
  QString text = QInputDialog::getText(tr("New commandbar"), tr("Specify the name of the new commandbar, please."),
                                      QLineEdit::Normal, def, 0, this);

  if (!text.isNull() && !text.isEmpty())
  {
    CommandBarManager::getInstance()->getOrCreateCommandBar( text, true, true )->show();
    int ct = ComboToolbars->count(), pos = -1;
    for (int i=0; i<ct; i++)
    {
      if ( ComboToolbars->text(i) == text )
        pos = i;
    }

    if ( pos != -1 )
    {
      ComboToolbars->setCurrentItem(pos);
    }
    else
    {
      ComboToolbars->insertItem(text);
      ComboToolbars->setCurrentItem( ComboToolbars->count()-1 );
    }

    // enable the widgets
    ToolbarActions->setEnabled(true);
    ComboToolbars->setEnabled (true);
  }
#endif
}

#ifdef NEW_WB_FRAMEWORK
void DlgCustomCmdbarsImp::onItemActivated( const QString& name )
{
  QPtrList<QToolBar> bars = CommandBarManager::getInstance()->commandBars();
  QToolBar* bar;
  std::vector<std::string> items;

  for ( bar = bars.first(); bar; bar = bars.next() )
  {
    CustomToolBar* cw = dynamic_cast<CustomToolBar*>(bar);
    if ( cw && cw->canModify() )
    {
      if (cw->name() == name)
      {
        // @todo read the elements from parameter
        break;
      }
    }
  }

  CommandManager & cCmdMgr = ApplicationWindow::Instance->commandManager();
  ToolbarActions->clear();
  for ( std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it )
  {
    if (*it == "Separator")
    {
      ToolbarActions->insertItem(new QListViewItem(ToolbarActions,ToolbarActions->lastItem(), "<Separator>"));
    }
    else
    {
      Command* pCom = cCmdMgr.getCommandByName( (*it).c_str() );
      if (pCom)
      {
        QListViewItem* item = new QListViewItem(ToolbarActions,ToolbarActions->lastItem(), pCom->getAction()->menuText());
        QPixmap pix = pCom->getAction()->iconSet().pixmap(/*QIconSet::Large,true*/);
        item->setPixmap(0, Tools::fillUp(24,24,pix));
        ToolbarActions->insertItem(item);
      }
    }
  }
}
#endif
/** Deletes a commandbar */
void DlgCustomCmdbarsImp::onDeleteToolbar()
{
#ifndef NEW_WB_FRAMEWORK
  QValueList<CheckListItem> items;
  QPtrList<Gui::CustomToolBar> tb = ApplicationWindow::Instance->customWidgetManager()->getCommdandBars();
  Gui::CustomToolBar* it;
  for ( it = tb.first(); it; it = tb.next() )
    items.append( qMakePair( QString(it->name()), it->canModify() ) );

  CheckListDialog checklists(this, "", true) ;
  checklists.setCaption( tr("Delete selected commandbars") );
  checklists.setCheckableItems( items );
  if (checklists.exec())
  {
    QStringList checked = checklists.getCheckedItems();
    for ( QStringList::Iterator it = checked.begin(); it!=checked.end(); ++it )
    {
      ApplicationWindow::Instance->customWidgetManager()->removeCommandBarFromSettings( (*it).latin1() );
    }

    updateData();
  }
#else
  QValueList<CheckListItem> items;
  QPtrList<QToolBar> tb = CommandBarManager::getInstance()->commandBars();
  QToolBar* it;
  for ( it = tb.first(); it; it = tb.next() )
  {
    CustomToolBar* cw = dynamic_cast<CustomToolBar*>(it);
    if ( cw )
      items.append( qMakePair( QString(it->name()), cw->canModify() ) );
    else
      items.append( qMakePair( QString(it->name()), false ) );
  }

  CheckListDialog checklists(this, "", true) ;
  checklists.setCaption( tr("Delete selected commandbars") );
  checklists.setCheckableItems( items );
  if (checklists.exec())
  {
    QStringList checked = checklists.getCheckedItems();
    for ( QStringList::Iterator it = checked.begin(); it!=checked.end(); ++it )
    {
      // @todo remove toolbar from preferences
//      ApplicationWindow::Instance->customWidgetManager()->removeCommandBarFromSettings( (*it).latin1() );
    }

    updateData();
  }
#endif
}

#include "moc_DlgCmdbarsImp.cpp"
