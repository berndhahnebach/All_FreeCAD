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
# include <qaccel.h>
#endif

#include "DlgActionsImp.h"
#include "Application.h"
#include "FileDialog.h"
#include "Tools.h"
#include "Command.h"
#include "BitmapFactory.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgCustomActionsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomActionsImp::DlgCustomActionsImp( QWidget* parent, const char* name, WFlags fl )
: DlgCustomActionsBase(parent, name, fl), bShown( false )
{
  // search for all macros
  std::string cMacroPath = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro/")->GetASCII("MacroPath",GetApplication().GetHomePath());
  QDir d(cMacroPath.c_str(),"*.FCMacro");
  actionMacros->clear();
  for (unsigned int i=0; i<d.count(); i++ )
    actionMacros->insertItem(d[i]);

  showPixmaps();
  newActionName();

  // connections
  //
  connect(buttonNew, SIGNAL(clicked()), this, SLOT(onAddCustomAction()));
  connect(buttonDelete, SIGNAL(clicked()), this, SLOT(onDelCustomAction()));
  connect(actionPixmapButton, SIGNAL(clicked()), this, SLOT(onCustomActionPixmap()));
  connect(CustomActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(onCustomActionsCanDelete(QListViewItem*)));
  connect(CustomActions, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(onCustomActionsDoubleClicked(QListViewItem*)));
}

/** Destroys the object and frees any allocated resources */
DlgCustomActionsImp::~DlgCustomActionsImp()
{
  cancel();
}

/** 
 * Displays this page. If no macros were found a message box
 * appears.
 */
void DlgCustomActionsImp::show()
{
  DlgCustomActionsBase::show();
  if (actionMacros->count() == 0 && bShown == false)
  {
    bShown = true;
    QMessageBox::warning(this, tr("No macro"),tr("No macros found."));
  }
}

void DlgCustomActionsImp::showPixmaps()
{
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*> aclCurMacros = rclMan.GetGroupCommands("Macros");
  for (std::vector<FCCommand*>::iterator it = aclCurMacros.begin(); it != aclCurMacros.end(); ++it)
  {
    QListViewItem* item = new QListViewItem(CustomActions,CustomActions->lastItem(), (*it)->GetName());
    if (!(*it)->GetAction()->iconSet().isNull())
    {
      QPixmap p = (*it)->GetAction()->iconSet().pixmap();
      item->setPixmap(0, Tools::fillUp(24,24,p));
    }
  }
}

void DlgCustomActionsImp::apply()
{
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*>::iterator it;
  for (it = _aclNewMacros.begin(); it!= _aclNewMacros.end(); ++it)
  {
    // add to the manager
    rclMan.AddCommand(*it);
  }

  for (it = _aclDelMacros.begin(); it!= _aclDelMacros.end(); ++it)
  {
    // remove from manager and delete it immediately
    rclMan.RemoveCommand(*it);
  }

  _aclNewMacros.clear();
  _aclDelMacros.clear();
}

void DlgCustomActionsImp::cancel()
{
  // delete all temporary created commands again as they were not appended to the command manager
  std::vector<FCCommand*>::iterator it;
  for (it = _aclNewMacros.begin(); it!= _aclNewMacros.end(); ++it)
  {
    delete (*it);
  }

  _aclNewMacros.clear();
}

void DlgCustomActionsImp::onCustomActionsCanDelete( QListViewItem *i )
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

void DlgCustomActionsImp::onCustomActionsDoubleClicked( QListViewItem *i )
{
  if ( !i ) return; // no valid item

  actionName->setText(i->text(0));
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();

  // search for the command in the manager and if necessary in the temporary created ones
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

  // command exists
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
      Base::Console().Error(tr("Sorry, couldn't find macro file.").latin1());
    }

    actionMenu->setText(pScript->GetMenuText());
    actionToolTip->setText(pScript->GetToolTipText());
    actionStatus->setText(pScript->GetStatusTip());
    actionAccel->setText(QAccel::keyToString(pScript->GetAccel()));
    PixmapLabel->clear();
    if (QString(pScript->GetPixmap()).length() > 2)
    {
      QPixmap p = Gui::BitmapFactory().pixmap(pScript->GetPixmap());
      PixmapLabel->setPixmap(p);
    }
  }
}

void DlgCustomActionsImp::onAddCustomAction()
{
  if (actionName->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Empty name"),tr("Please specify an action name first."));
    return;
  }

  if (actionMacros-> currentText().isEmpty())
  {
    QMessageBox::warning(this, tr("Empty macro"),tr("Please specify the macro first."));
    return;
  }

  if (actionMenu->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Empty text"),tr("Please specify the menu text first."));
    return;
  }

  // search for the command in the manager and if necessary in the temporary created ones
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCmd = rclMan.GetCommandByName(actionName->text().latin1());
  if (pCmd == NULL)
  {
    for (std::vector<FCCommand*>::iterator it = _aclNewMacros.begin(); it!= _aclNewMacros.end(); ++it)
    {
      if (actionName->text().startsWith((*it)->GetName()))
      {
        pCmd = *it;
        break;
      }
    }
  }

  // command exists
  FCScriptCommand* macro;
  if (pCmd != NULL)
  {
    macro = (FCScriptCommand*)pCmd;
  }
  else
  {
    macro = new FCScriptCommand(actionName->text().latin1());
    _aclNewMacros.push_back(macro);

    QListViewItem* item = new QListViewItem(CustomActions,CustomActions->lastItem(), actionName->text());
    
    if (PixmapLabel->pixmap() != NULL)
    {
      QPixmap p = *PixmapLabel->pixmap();
      item->setPixmap(0, Tools::fillUp(24,24,p));
    }
  }

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
//  macro->SetHelpPage("");
//  if (!m_sPixmap.isEmpty())
//  macro->SetHelpURL("");
  if (!actionAccel->text().isEmpty())
    macro->SetAccel(QAccel::stringToKey(actionAccel->text()));
  actionAccel->clear();

  newActionName();

  setModified(true);
}

void DlgCustomActionsImp::onDelCustomAction()
{
  // remove item from list view
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

  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*> aclCurMacros = rclMan.GetGroupCommands("Macros");
  std::vector<FCCommand*>::iterator it2;

  // if the command is registered in the manager just mark it for deletion
  // but do not delete it now
  for (it2 = aclCurMacros.begin(); it2!= aclCurMacros.end(); ++it2)
  {
    if (itemText == (*it2)->GetName())
    {
      _aclDelMacros.push_back(*it2);
      break;
    }
  }

  // if the command is temporary created remove and delete it
  for (it2 = _aclNewMacros.begin(); it2!= _aclNewMacros.end(); ++it2)
  {
    if (itemText == (*it2)->GetName())
    {
      delete *it2;
      _aclNewMacros.erase(it2);
      break;
    }
  }

  newActionName();

  setModified(true);
}

void DlgCustomActionsImp::onCustomActionPixmap()
{
  QString pixPath = FileDialog::getOpenFileName(QString::null,"Pixmap (*.xpm *.gif *.png *.bmp)",this, "", 
    tr("Choose a Pixmap"));
  if (!pixPath.isEmpty())
  {
    m_sPixmap = pixPath.mid(pixPath.findRev("/") + 1);
    m_sPixmap = m_sPixmap.left(m_sPixmap.findRev("."));
    PixmapLabel->setPixmap(QPixmap(pixPath));
  }
}

void DlgCustomActionsImp::newActionName()
{
  int id = 0;
  QString sName;
  bool bUsed;

  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  std::vector<FCCommand*> aclCurMacros = rclMan.GetGroupCommands("Macros");

  do
  {
    bUsed = false;
    sName = QString("Std_Macro_%1").arg( id++ );

    std::vector<FCCommand*>::iterator it;
    for ( it = aclCurMacros.begin(); it!= aclCurMacros.end(); ++it )
    {
      if (sName == (*it)->GetName())
      {
        bUsed = true;
        break;
      }
    }

    for ( it = _aclNewMacros.begin(); it!= _aclNewMacros.end(); ++it )
    {
      if (sName == (*it)->GetName())
      {
        bUsed = true;
        break;
      }
    }
  }
  while ( bUsed );

  actionName->setText( sName );
}


#include "DlgActions.cpp"
#include "moc_DlgActions.cpp"
#include "moc_DlgActionsImp.cpp"
