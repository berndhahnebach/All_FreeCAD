/***************************************************************************
                          DlgCustomizeImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize toolbars and button groups
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

#include "DlgCustomizeImp.h"
#include "Application.h"
#include <qobjcoll.h>

/* 
 *  Constructs a FCDlgCustomize which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgCustomize::FCDlgCustomize( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : FCDlgCustomizeBase( parent, name, modal, fl )
{
  // first tab
  connect(IconView1, SIGNAL(emitSelectionChanged(QString)), this, SLOT(slotDescription(QString)));
  connect(ComboBoxCategory, SIGNAL(activated ( const QString & )), this, SLOT(slotGroupSelected(const QString &)));

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  FCmap<FCstring,FCCommand*> sCommands = cCmdMgr.GetCommands();

  AvailableActions->insertItem("<Separator>");
  for (FCmap<FCstring,FCCommand*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it)
  {
    // !!!! "Standard commands" muss durch Kategorie ersetzt werden
    m_alCmdGroups["Standard commands"].push_back(it->second);
    AvailableActions->insertItem(it->second->GetAction()->iconSet().pixmap(), it->second->GetAction()->menuText());
  }

  for (FCmap<FCstring, FCvector<FCCommand*> >::iterator it2 = m_alCmdGroups.begin(); it2 != m_alCmdGroups.end(); ++it2)
  {
    ComboBoxCategory->insertItem(it2->first.c_str());
    ComboBoxCategory2->insertItem(it2->first.c_str());
  }

  slotGroupSelected(ComboBoxCategory->text(0));

  // second tab
  ToolbarActions->setVariableHeight(false);
  AvailableActions->setVariableHeight(false);
  connect(ComboToolbars, SIGNAL(activated ( const QString & )), this, SLOT(slotToolBarSelected(const QString &)));
  connect(CreateToolbar, SIGNAL(clicked()), this, SLOT(slotCreateToolBar()));
  m_aclToolbars = ApplicationWindow::Instance->GetToolBars();
  for (FCvector<QToolBar*>::iterator it3 = m_aclToolbars.begin(); it3 != m_aclToolbars.end(); ++it3)
  {
    ComboToolbars->insertItem((*it3)->name());
  }

  slotToolBarSelected(ComboToolbars->text(0));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgCustomize::~FCDlgCustomize()
{
    // no need to delete child widgets, Qt does it all for us
}

void FCDlgCustomize::slotDescription(QString txt)
{
  TextLabel->setText(txt);
}

void FCDlgCustomize::slotGroupSelected(const QString & group)
{
  IconView1->clear();
 
  // group of commands found
  if (m_alCmdGroups.find(group.latin1()) != m_alCmdGroups.end())
  {
    FCvector<FCCommand*> aCmds = m_alCmdGroups[group.latin1()];
    for (FCvector<FCCommand*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it)
    {
      if ((*it)->GetAction())
      (void) new FCCmdViewItem(IconView1, (*it)->GetAction());
    }
  }
}

void FCDlgCustomize::slotToolBarSelected(const QString & name)
{
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  FCmap<FCstring,FCCommand*> sCommands = cCmdMgr.GetCommands();

  ToolbarActions->clear();

  for (FCvector<QToolBar*>::iterator it = m_aclToolbars.begin(); it != m_aclToolbars.end(); ++it)
  {
    if ((*it)->name() == name)
    {
      const QObjectList* children = (*it)->children ();
      for (QObjectListIt it(*children); it.current(); ++it)
      {
        QString name = it.current()->name();
        if (it.current()->inherits("QToolButton"))
        {
          QToolButton* b = (QToolButton*)it.current();
          name = b->textLabel();
          if (sCommands.find(name.latin1()) != sCommands.end())
          {
            FCCommand* pCom = sCommands[name.latin1()];
            ToolbarActions->insertItem(pCom->GetAction()->iconSet().pixmap(), pCom->GetAction()->menuText());
          }
        }
        else if (name.contains("separator", false) > 0)
        {
          ToolbarActions->insertItem("<Separator>");
        }
      }
      break;
    }
  }
}

void FCDlgCustomize::slotCreateToolBar()
{
  QString text = ToolbarName->text();
  ToolbarName->clear();
  QToolBar* toolbar = ApplicationWindow::Instance->GetToolBar(text.latin1());
  toolbar->show();
}

#include "DlgCustomize.cpp"
#include "moc_DlgCustomize.cpp"
#include "moc_DlgCustomizeImp.cpp"
