/***************************************************************************
                          DlgActionsImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize actions
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
#	include <qmessagebox.h>
#	include <qiconview.h> 
#	include <qfiledialog.h>
#	include <qcombobox.h>
#       include <Python.h>
#endif

#include "DlgActionsImp.h"
#include "Application.h"
#include "Tools.h"
#include "Command.h"
#include "Widgets.h"
#include <qtabwidget.h>

/////////////////////////////////////////////////////////////////////////////////////////////

FCDlgCustomActionsImp::FCDlgCustomActionsImp( QWidget* parent, const char* name, WFlags fl )
: FCDlgCustomActions(parent, name, fl)
{
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  _aclCurMacros = rclMan.GetModuleCommands("Macro");
  iCtMacros = _aclCurMacros.size();
  actionName->setText(tr("Std_Macro_%1").arg(iCtMacros));

  // search for all macros
  std::string cMacroPath = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro/")->GetASCII("MacroPath",GetApplication().GetHomePath());
  QDir d(cMacroPath.c_str(),"*.FCMacro");
	actionMacros->clear();
  for (unsigned int i=0; i<d.count(); i++ )
    actionMacros->insertItem(d[i]);

  init();

  // connections
  //
  connect(buttonNew, SIGNAL(clicked()), this, SLOT(slotAddCustomAction()));
  connect(buttonDelete, SIGNAL(clicked()), this, SLOT(slotDelCustomAction()));
  connect(actionPixmapButton, SIGNAL(clicked()), this, SLOT(slotCustomActionPixmap()));
  connect(CustomActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotCustomActionsChanged(QListViewItem*)));
  connect(CustomActions, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotCustomActionsDoubleClicked(QListViewItem*)));
}

FCDlgCustomActionsImp::~FCDlgCustomActionsImp()
{
}

void FCDlgCustomActionsImp::show()
{
  FCDlgCustomActions::show();
  if (actionMacros->count() == 0)
  {
    QMessageBox::warning(this, "No macro","No macros found");
  }
}

void FCDlgCustomActionsImp::init()
{
  for (std::vector<FCCommand*>::iterator it = _aclCurMacros.begin(); it != _aclCurMacros.end(); ++it)
  {
    QListViewItem* item = new QListViewItem(CustomActions,FCListView::lastItem(CustomActions), (*it)->GetName());
    if (!(*it)->GetAction()->iconSet().isNull())
    {
      QPixmap p = (*it)->GetAction()->iconSet().pixmap();
      item->setPixmap(0, FCTools::fillUp(24,24,p));
    }
  }
}

void FCDlgCustomActionsImp::apply()
{
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*>::iterator it;
  for (it = _aclNewMacros.begin(); it!= _aclNewMacros.end(); ++it)
  {
    rclMan.AddCommand(*it);
  }

  for (it = _aclDelMacros.begin(); it!= _aclDelMacros.end(); ++it)
  {
    rclMan.RemoveCommand(*it);
  }

  _aclCurMacros = rclMan.GetModuleCommands("Macro");
  _aclNewMacros.clear();
  _aclDelMacros.clear();
}

void FCDlgCustomActionsImp::cancel()
{
}

void FCDlgCustomActionsImp::slotCustomActionsChanged( QListViewItem *i )
{
  bool canDelete = FALSE;
  QListViewItemIterator it = CustomActions->firstChild();

  for ( ; it.current(); it++ ) 
  {
    if ( it.current()->isSelected() ) 
    {
	    canDelete = TRUE;
	    break;
	  }
  }

  buttonDelete->setEnabled( canDelete || ( i && i->isSelected() ) );
}

void FCDlgCustomActionsImp::slotCustomActionsDoubleClicked( QListViewItem *i )
{
  actionName->setText(i->text(0));
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCmd = rclMan.GetCommandByName(i->text(0).latin1());
  if (pCmd == NULL)
  {
    for (std::vector<FCCommand*>::iterator it = _aclNewMacros.begin(); it!= _aclNewMacros.end(); ++it)
    {
      if (i->text(0).startsWith((*it)->GetName()))
      {
        pCmd = *it;
        break;
      }
    }
  }

  if (pCmd != NULL)
  {
    FCScriptCommand* pScript = (FCScriptCommand*)pCmd;
    actionWhatsThis->setText(pScript->GetWhatsThis());
    bool bFound = false;
    for (int i = 0; i<actionMacros->count(); i++)
    {
      if (actionMacros->text(i).startsWith(pScript->GetScriptName()))
      {
        bFound = true;
        actionMacros->setCurrentItem(i);
        break;
      }
    }
    if (!bFound)
    {
      QMessageBox::warning(this, "Macro not found", "Sorry, macro not found");
    }

    actionMenu->setText(pScript->GetMenuText());
    actionToolTip->setText(pScript->GetToolTipText());
    actionStatus->setText(pScript->GetStatusTip());
    actionAccel->setText(QAccel::keyToString(pScript->GetAccel()));
    FCBmpFactory bmp;
	  bmp.AddPath("../../FreeCADIcons");
	  bmp.AddPath("../Icons");
    PixmapLabel->clear();
    if (QString(pScript->GetPixmap()).length() > 2)
    {
      QPixmap p = bmp.GetPixmap(pScript->GetPixmap());
      PixmapLabel->setPixmap(p);
    }
  }
}

void FCDlgCustomActionsImp::slotAddCustomAction()
{
  if (actionName->text().isEmpty())
  {
    QMessageBox::warning(this, "Empty name","Please specify an action name first");
    return;
  }

  if (actionMacros-> currentText().isEmpty())
  {
    QMessageBox::warning(this, "Empty macro","Please specify the macro first");
    return;
  }

  if (actionMenu->text().isEmpty())
  {
    QMessageBox::warning(this, "Empty text","Please specify the menu text first");
    return;
  }

  QListViewItem* item = new QListViewItem(CustomActions,FCListView::lastItem(CustomActions), actionName->text());
  if (PixmapLabel->pixmap() != NULL)
  {
    QPixmap p = *PixmapLabel->pixmap();
    item->setPixmap(0, FCTools::fillUp(24,24,p));
  }

  FCScriptCommand* macro = new FCScriptCommand(actionName->text().latin1());

  if (!actionWhatsThis->text().isEmpty())
  macro->SetWhatsThis(actionWhatsThis->text().latin1());
  actionWhatsThis->clear();
  if (!actionMacros-> currentText().isEmpty())
  macro->SetScriptName(actionMacros-> currentText().latin1());
  if (!actionMenu->text().isEmpty())
	macro->SetMenuText(actionMenu->text().latin1());
  actionMenu->clear();
  if (!actionToolTip->text().isEmpty())
	macro->SetToolTipText(actionToolTip->text().latin1());
  actionToolTip->clear();
  if (!actionStatus->text().isEmpty())
	macro->SetStatusTip(actionStatus->text().latin1());
  actionStatus->clear();
  if (!m_sPixmap.isEmpty())
	macro->SetPixmap(m_sPixmap.latin1());
//  if (!m_sPixmap.isEmpty())
//	macro->SetHelpPage("");
//  if (!m_sPixmap.isEmpty())
//	macro->SetHelpURL("");
  if (!actionAccel->text().isEmpty())
    macro->SetAccel(QAccel::stringToKey(actionAccel->text()));
  actionAccel->clear();

  _aclNewMacros.push_back(macro);

  actionName->setText(tr("Std_Macro_%1").arg(++iCtMacros));

  setModified(true);
}

void FCDlgCustomActionsImp::slotDelCustomAction()
{
  QString itemText;
  QListViewItemIterator it = CustomActions->firstChild();
  while ( it.current() ) 
  {
	  if ( it.current()->isSelected() )
    {
      itemText = it.current()->text(0);
      delete it.current();
      break;
    }
	  else
      it++;
  }

  for (std::vector<FCCommand*>::iterator it2 = _aclCurMacros.begin(); it2!= _aclCurMacros.end(); ++it2)
  {
    if (itemText == (*it2)->GetName())
    {
      _aclDelMacros.push_back(*it2);
      break;
    }
  }

  for (it2 = _aclNewMacros.begin(); it2!= _aclNewMacros.end(); ++it2)
  {
    if (itemText == (*it2)->GetName())
    {
      _aclNewMacros.erase(it2);
      break;
    }
  }

  setModified(true);
}

void FCDlgCustomActionsImp::slotCustomActionPixmap()
{
  QString pixPath = FCFileDialog::getOpenFileName(QString::null,"Pixmap (*.xpm *.gif *.png *.bmp)",this, "", "Choose a Pixmap");
  if (!pixPath.isEmpty())
  {
    m_sPixmap = pixPath.mid(pixPath.findRev("/") + 1);
    m_sPixmap = m_sPixmap.left(m_sPixmap.findRev("."));
    PixmapLabel->setPixmap(QPixmap(pixPath));
  }
}





#include "DlgActions.cpp"
#include "moc_DlgActions.cpp"
#include "moc_DlgActionsImp.cpp"
