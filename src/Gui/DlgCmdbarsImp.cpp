/***************************************************************************
                          DlgCmdbarsImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize command bars
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





#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qaccel.h>
#	include <qaction.h>
#	include <qbutton.h>
#	include <qinputdialog.h>
#	include <qlabel.h>
#	include <qmessagebox.h>
#	include <qiconview.h>
#	include <qfiledialog.h>
#	include <qcombobox.h>
#	include <qthread.h>
#endif

#include "DlgCmdbarsImp.h"
#include "Application.h"
#include "Tools.h"
#include "Command.h"

using namespace Gui::Dialog;

CDlgCustomCmdbarsImp::CDlgCustomCmdbarsImp( QWidget* parent, const char* name, WFlags fl )
	: CDlgCustomToolbars(parent, name, fl)
{
	onUpdate();
}

CDlgCustomCmdbarsImp::~CDlgCustomCmdbarsImp()
{
}

void CDlgCustomCmdbarsImp::apply()
{
  QString text = ComboToolbars->currentText();
  FCToolBar* toolbar = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBar(text.latin1());
  toolbar->clearAll();

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

void CDlgCustomCmdbarsImp::cancel()
{
}

void CDlgCustomCmdbarsImp::onUpdate()
{
  ComboToolbars->clear();
  m_aclToolbars = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
  for (std::vector<FCToolBar*>::iterator it3 = m_aclToolbars.begin(); it3 != m_aclToolbars.end(); ++it3)
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

void CDlgCustomCmdbarsImp::onCreateToolbar()
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
    m_aclToolbars.push_back(toolbar);
    ComboToolbars->insertItem(text);

		// enable the widgets
		ToolbarActions->setEnabled(true);
		ComboToolbars->setEnabled (true);
  }
}

void CDlgCustomCmdbarsImp::onDeleteToolbar()
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

		onUpdate();
  }
}

#include "moc_DlgCmdbarsImp.cpp"
