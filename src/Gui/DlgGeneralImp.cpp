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
# include <Python.h>
#endif

#include "DlgGeneralImp.h"
#include "Application.h"
#include "PrefWidgets.h"
#include "Command.h"
#include "CommandLine.h"

/* 
 *  Constructs a FCDlgGeneral which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgGeneral::FCDlgGeneral( QWidget* parent,  const char* name, WFlags fl )
    : FCDlgGeneralBase( parent, name, fl ), newLanguage(false)
{
  // if you run this first time
  if (WindowStyle->count() == 0)
  {
    WindowStyle->insertStringList(FCStyleFactory::styles());
  }

	// search for the language files
	Languages->insertStringList(onSearchForLanguageFiles());
	onSelectLanguageItem();

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
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  bool show = hGrp->GetBool("ShowCmdLine", true);
  if (show)
    GetCmdLine().show();
  else
    GetCmdLine().hide();
}

void FCDlgGeneral::onChooseLanguage(const QString& s)
{
	if (languageFiles.find(s) != languageFiles.end())
	{
		QString file = s;
		
		QTranslator translator(0);
		translator.load(file, ".");
		translator.save("FreeCAD.qm");

		QMessageBox::information(this, "Info", tr("To take effect on the new language restart FreeCAD, please"));
	}
}

QStringList FCDlgGeneral::onSearchForLanguageFiles()
{
	languageFiles.clear();
	languageFiles.append(tr("English (Default)"));
	QStringList s = QDir(".", "*.qm").entryList("*.qm");
	s.remove("FreeCAD.qm");
	languageFiles += s;
	return languageFiles;
}

void FCDlgGeneral::onSelectLanguageItem()
{
	unsigned int size=0;
	QFileInfo fi;
	fi.setFile("FreeCAD.qm");
	if (fi.exists() && fi.isFile())
	{
		size = fi.size();
	}

	int item = 0;
	for (QStringList::Iterator it = languageFiles.begin(); it!=languageFiles.end(); ++it, ++item)
	{
		fi.setFile(*it);
		if (fi.exists() && fi.isFile())
		{
			// compare the size of the two files
			if (fi.size() == size)
			{
				Languages->setCurrentItem(item);
				break;
			}
		}
	}
}

#include "DlgGeneral.cpp"
#include "moc_DlgGeneral.cpp"
#include "moc_DlgGeneralImp.cpp"
