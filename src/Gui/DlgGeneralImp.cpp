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
#	include <qaction.h>
#	include <qbutton.h>
#	include <qmessagebox.h>
#	include <qiconview.h>
#	include <qfiledialog.h>
#	include <qcombobox.h>
#	include <qlabel.h>
#	include <qstyle.h>
#	include <qstylefactory.h>
#	include <qthread.h>
#endif

#include "DlgGeneralImp.h"
#include "Application.h"
#include "PrefWidgets.h"
#include "Command.h"
#include "CommandLine.h"
#include "Language/LanguageFactory.h"

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
#if QT_VERSION > 300
  // if you run this first time
  if (WindowStyle->count() == 0)
  {
    WindowStyle->insertStringList(QStyleFactory::keys ());
  }
#endif

	// search for the language files
	std::string lang = GetApplication().GetUserParameter
		().GetGroup("BaseApp")->GetGroup("Window")->GetGroup
		("Language")->GetASCII("Language", "English");
	language = tr(lang.c_str());
	onSearchForLanguages();

  append(SpeedAnimationCmdBar->getHandler());
  append(UsesBigPixmaps->getHandler());
  append(WindowStyle->getHandler());
  append(AllowDragMenu->getHandler());
  append(RecentFiles->getHandler());
  append(SplashScreen->getHandler());
  append(ShowCmdLine->getHandler());
  append(SizeCmdLine->getHandler());

  connect(UsesBigPixmaps->getHandler(), SIGNAL(saved()), this, SLOT(onBigPixmaps()));
  connect(WindowStyle->getHandler(), SIGNAL(saved()), this, SLOT(onSetStyle()));
  connect(RecentFiles->getHandler(), SIGNAL(saved()), this, SLOT(onSetMRUSize()));
  connect(ShowCmdLine->getHandler(), SIGNAL(saved()), this, SLOT(onSetCmdLineVisible()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgGeneral::~FCDlgGeneral()
{
    // no need to delete child widgets, Qt does it all for us
}

void FCDlgGeneral::apply()
{
	if (QString::compare(Languages->currentText(), language) != 0)
	{
		FCMessageBox::information(ApplicationWindow::Instance, "Info", tr("To take effect on the new language restart FreeCAD, please"));
	  GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Window")->GetGroup
		("Language")->SetASCII("Language", Languages->currentText().latin1());
	}
}

void FCDlgGeneral::onBigPixmaps()
{
	ApplicationWindow::Instance->UpdatePixmapsSize();
}

void FCDlgGeneral::onSetStyle()
{
	ApplicationWindow::Instance->UpdateStyle();
}

void FCDlgGeneral::onSetMRUSize()
{
	FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCmd = rclMan.GetCommandByName("Std_MRU");
  if (pCmd)
  {
    FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("Recent files");
    ((FCCmdMRU*)pCmd)->SetMaxItems(hGrp->GetInt("RecentFiles", 4));
  }
}

void FCDlgGeneral::onSetCmdLineVisible()
{
  GetCmdLine().show();
}

void FCDlgGeneral::onChooseLanguage(const QString&)
{
	setModified(true);
}

void FCDlgGeneral::onSearchForLanguages()
{
	QStringList list = GetLanguageFactory().GetRegisteredLanguages();

	int pos = 0;
	Languages->insertItem(tr("English"));

	int i=1;
	for (QStringList::Iterator it = list.begin(); it != list.end(); ++it, ++i)
	{
		Languages->insertItem(*it);
		if (QString::compare(*it, language) == 0)
			pos = i;
	}

	Languages->setCurrentItem(pos);
}

#include "DlgGeneral.cpp"
#include "moc_DlgGeneral.cpp"
#include "moc_DlgGeneralImp.cpp"
