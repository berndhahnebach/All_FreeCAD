/***************************************************************************
                          DlgCommandsImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Shows all available commands in a view
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




#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qmessagebox.h>
#	include <qiconview.h> 
#	include <qfiledialog.h>
#	include <qcombobox.h>
#       include <Python.h>
#endif

#include "DlgCommandsImp.h"
#include "Application.h"
#include "Tools.h"
#include <qobjcoll.h>
#include <qtabwidget.h>

/////////////////////////////////////////////////////////////////////////////////////////////

FCDlgCustomCommandsImp::FCDlgCustomCommandsImp( QWidget* parent, const char* name, WFlags fl  )
: FCDlgCustomCommands(parent, name, fl)
{
  apply();

  connect(IconView1, SIGNAL(emitSelectionChanged(QString)), this, SLOT(slotDescription(QString)));
  connect(ComboBoxCategory, SIGNAL(highlighted ( const QString & )), this, SLOT(slotGroupSelected(const QString &)));

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  for (std::map<std::string,FCCommand*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    m_alCmdGroups[it->second->GetGroupName()].push_back(it->second);
  }

  for (std::map<std::string, std::vector<FCCommand*> >::iterator it2 = m_alCmdGroups.begin(); it2 != m_alCmdGroups.end(); ++it2)
  {
    ComboBoxCategory->insertItem(it2->first.c_str());
  }

  ComboBoxCategory->setCurrentItem(0);
}

FCDlgCustomCommandsImp::~FCDlgCustomCommandsImp()
{
}

void FCDlgCustomCommandsImp::apply()
{
  std::vector<FCToolBar*> bars;
  std::vector<FCToolBar*>::iterator it;

  bars = ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBars();
  for (it = bars.begin(); it!=bars.end(); ++it)
  {
    (*it)->saveXML();
  }

  bars = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
  for (it = bars.begin(); it!=bars.end(); ++it)
  {
    (*it)->saveXML();
  }
}

void FCDlgCustomCommandsImp::cancel()
{
  QString wb = ApplicationWindow::Instance->GetActiveWorkbench();
  if (ApplicationWindow::Instance->GetCustomWidgetManager() != NULL)
  {
    ApplicationWindow::Instance->GetCustomWidgetManager()->update(wb.latin1());
  }
}

void FCDlgCustomCommandsImp::slotDescription(QString txt)
{
  TextLabel->setText(txt);
}

void FCDlgCustomCommandsImp::slotGroupSelected(const QString & group)
{
  IconView1->clear();
 
  // group of commands found
  if (m_alCmdGroups.find(group.latin1()) != m_alCmdGroups.end())
  {
    std::vector<FCCommand*> aCmds = m_alCmdGroups[group.latin1()];
    for (std::vector<FCCommand*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it)
    {
      if ((*it)->GetAction())
      (void) new FCCmdViewItem(IconView1, (*it)->GetName(), (*it)->GetAction());
    }
  }
}

#include "DlgCommands.cpp"
#include "moc_DlgCommands.cpp"
#include "moc_DlgCommandsImp.cpp"
