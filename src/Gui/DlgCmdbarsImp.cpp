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

using namespace Gui::Dialog;

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
  updateData();
}

/** Destroys the object and frees any allocated resources */
DlgCustomCmdbarsImp::~DlgCustomCmdbarsImp()
{
}

/** Adds created or removes deleted command bars */
void DlgCustomCmdbarsImp::apply()
{
  QString text = ComboToolbars->currentText();
  FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBar(text.latin1());
  toolbar->clearUp();

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  std::vector<std::string> items;
  QListViewItem* item = ToolbarActions->firstChild();
  for (int i=0; i < ToolbarActions->childCount(); item = item->itemBelow(), i++)
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

/** Discards all changes */
void DlgCustomCmdbarsImp::cancel()
{
}

/** Shows all actions from the last specified command bar */
void DlgCustomCmdbarsImp::updateData()
{
  ComboToolbars->clear();
  _aclToolbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
  for ( std::vector<FCToolBar*>::iterator it3 = _aclToolbars.begin(); it3 != _aclToolbars.end(); ++it3 )
  {
    if ((*it3)->canModify())
      ComboToolbars->insertItem((*it3)->name());
  }

  if (ComboToolbars->count() > 0)
  {
    onItemActivated(ComboToolbars->text(0));
  }
  else
  {
    ToolbarActions->setEnabled(false);
    ComboToolbars->setEnabled (false);
  }
}

/** Creates new command bar */
void DlgCustomCmdbarsImp::onCreateToolbar()
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
    _aclToolbars.push_back(toolbar);
    ComboToolbars->insertItem(text);

    // enable the widgets
    ToolbarActions->setEnabled(true);
    ComboToolbars->setEnabled (true);
  }
}

/** Deletes a command bar */
void DlgCustomCmdbarsImp::onDeleteToolbar()
{
  std::vector<std::pair<std::string, bool> > items;
  std::vector<FCToolBar*> tb = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
  for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    items.push_back(std::make_pair<std::string, bool>((*it)->name(), (*it)->canModify()));

  FCCheckListDlg checklists(this, "", true) ;
  checklists.setCaption( tr("Delete selected command bars") );
  checklists.setItems(items);
  if (checklists.exec())
  {
    std::vector<std::string> checked = checklists.getCheckedItems();
    for (std::vector<std::string>::iterator it = checked.begin(); it!=checked.end(); ++it)
    {
      ApplicationWindow::Instance->GetCustomWidgetManager()->delCmdBar(it->c_str());
    }

    updateData();
  }
}

#include "moc_DlgCmdbarsImp.cpp"
