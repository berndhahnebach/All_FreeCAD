/***************************************************************************
                          DlgGeneralImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief 
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

#include "DlgGeneralImp.h"
#include "Application.h"
#include "PrefWidgets.h"

/* 
 *  Constructs a FCDlgGeneral which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgGeneral::FCDlgGeneral( QWidget* parent,  const char* name, WFlags fl )
    : FCDlgGeneralBase( parent, name, fl )
{
  // if you run this first time
  if (WindowStyle->count() == 0)
  {
    WindowStyle->insertStringList(FCStyleFactory::styles());
  }

  append(SpeedAnimationCmdBar->getHandler());
  append(UsesBigPixmaps->getHandler());
  append(WindowStyle->getHandler());
  append(AllowDragMenu->getHandler());
  append(RecentFiles->getHandler());

  connect(UsesBigPixmaps->getHandler(), SIGNAL(saved()), this, SLOT(onBigPixmaps()));
  connect(WindowStyle->getHandler(), SIGNAL(saved()), this, SLOT(onSetStyle()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgGeneral::~FCDlgGeneral()
{
    // no need to delete child widgets, Qt does it all for us
}

void FCDlgGeneral::onBigPixmaps()
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  bool bigPixmaps = hGrp->GetBool("BigPixmaps", false);
  if (bigPixmaps != ApplicationWindow::Instance->usesBigPixmaps())
    ApplicationWindow::Instance->setUsesBigPixmaps(bigPixmaps);
}

void FCDlgGeneral::onSetStyle()
{
  QStyle& curStyle = QApplication::style();
  QStyle* newStyle = FCStyleFactory::createStyle(WindowStyle->currentText());

  if (newStyle != NULL)
  {
    if (strcmp(newStyle->name(), curStyle.name()) != 0)
      QApplication::setStyle(newStyle);
  }
}

#include "DlgGeneral.cpp"
#include "moc_DlgGeneral.cpp"
#include "moc_DlgGeneralImp.cpp"
