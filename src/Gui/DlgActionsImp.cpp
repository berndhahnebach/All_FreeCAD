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

#include "DlgActionsImp.h"
#include "Application.h"
#include "Tools.h"
#include <qobjcoll.h>
#include <qtabwidget.h>

/////////////////////////////////////////////////////////////////////////////////////////////

FCDlgCustomActionsImp::FCDlgCustomActionsImp( QWidget* parent, const char* name, WFlags fl )
: FCDlgCustomActions(parent, name, fl)
{
  // connections
  //
  connect(buttonNew, SIGNAL(clicked()), this, SLOT(slotAddCustomAction()));
  connect(buttonDelete, SIGNAL(clicked()), this, SLOT(slotDelCustomAction()));
  connect(actionPixmapButton, SIGNAL(clicked()), this, SLOT(slotCustomActionPixmap()));
  connect(CustomActions, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotCustomActionsChanged(QListViewItem*)));
}

FCDlgCustomActionsImp::~FCDlgCustomActionsImp()
{
}

void FCDlgCustomActionsImp::apply()
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
      actionName->setText(it.current()->text(0));
	    canDelete = TRUE;
	    break;
	  }
  }

  buttonDelete->setEnabled( canDelete || ( i && i->isSelected() ) );
}

void FCDlgCustomActionsImp::slotAddCustomAction()
{
  if (actionName->text().isEmpty())
  {
    QMessageBox::warning(this, "Empty name","Please specify an action name first");
    return;
  }

  QListViewItem* item = new QListViewItem(CustomActions,FCListView::lastItem(CustomActions), actionName->text());
  if (PixmapLabel->pixmap() != NULL)
  {
    QPixmap p = *PixmapLabel->pixmap();
    item->setPixmap(0, FCTools::resize(24,24,p));
  }
  actionName->clear();
  actionToolTip->clear();
  actionStatus->clear();
}

void FCDlgCustomActionsImp::slotDelCustomAction()
{
  QListViewItemIterator it = CustomActions->firstChild();
  while ( it.current() ) 
  {
	  if ( it.current()->isSelected() )
     delete it.current();
	  else
     it++;
  }

  setModified(true);
}

void FCDlgCustomActionsImp::slotCustomActionPixmap()
{
  QString pix = FCFileDialog::getOpenFileName(QString::null,"Pixmap (*.xpm)",this, "", "Choose a Pixmap");
  if (!pix.isEmpty())
  {
    PixmapLabel->setPixmap(QPixmap(pix));
  }
}

void FCDlgCustomActionsImp::cancel()
{
}

#include "DlgActions.cpp"
#include "moc_DlgActions.cpp"
#include "moc_DlgActionsImp.cpp"
