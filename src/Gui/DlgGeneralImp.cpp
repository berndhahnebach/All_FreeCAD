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
# include <qapplication.h>
# include <qstyle.h>
# include <qstylefactory.h>
#endif

#include "DlgGeneralImp.h"
#include "Application.h"
#include "Command.h"
#include "CommandLine.h"
#include "PrefWidgets.h"
#include "Language/LanguageFactory.h"
#include "Language/Translator.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgGeneralImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgGeneralImp::DlgGeneralImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgGeneralBase( parent, name, fl )
{
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgGeneralImp::~DlgGeneralImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/** Sets the size of the recent file list (MRU) in the
 * user parameters.
 * @see StdCmdMRU
 */
void DlgGeneralImp::setMRUSize()
{
  CommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  Command* pCmd = rclMan.getCommandByName("Std_MRU");
  if (pCmd)
  {
    FCParameterGrp::handle hGrp = WindowParameter::getParameter()->GetGroup("RecentFiles");
    ((StdCmdMRU*)pCmd)->setMaxCount(hGrp->GetInt("RecentFiles", 4));
  }
}

/** Searches for all registered languages and insert them into a combo box */
void DlgGeneralImp::insertLanguages()
{
  QStringList list = Gui::LanguageFactory().getRegisteredLanguages();

  Languages->insertItem(tr("English"));

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    Languages->insertItem(*it);
  }
}

void DlgGeneralImp::saveSettings()
{
  RecentFiles->onSave();
  SplashScreen->onSave();
  ShowCmdLine->onSave();
  SizeCmdLine->onSave();
  AllowDragMenu->onSave();
  UsesBigPixmaps->onSave();

  FCParameterGrp::handle hGrp = WindowParameter::getParameter()->GetGroup("General");
  hGrp->SetASCII( "WindowStyle", WindowStyle->currentText().latin1() );

  ApplicationWindow::Instance->UpdateStyle();
  ApplicationWindow::Instance->UpdatePixmapsSize();
  setMRUSize();
  CommandLine().show();

  QString language = hGrp->GetASCII("Language", "English").c_str();
  if ( QString::compare( Languages->currentText(), language ) != 0 )
  {
    hGrp->SetASCII("Language", Languages->currentText().latin1());
    Gui::Translator::setLanguage( Languages->currentText() );
  }
}

void DlgGeneralImp::loadSettings()
{
  RecentFiles->onRestore();
  SplashScreen->onRestore();
  ShowCmdLine->onRestore();
  SizeCmdLine->onRestore();
  AllowDragMenu->onRestore();
  UsesBigPixmaps->onRestore();

  // fill up styles
  //
  QStringList styles = QStyleFactory::keys ();
  WindowStyle->insertStringList( styles );
  QString style = QApplication::style().name();
  WindowStyle->setCurrentText( style );

  // search for the language files
  FCParameterGrp::handle hGrp = WindowParameter::getParameter()->GetGroup("General");
  QString language = hGrp->GetASCII("Language", "English").c_str();
  insertLanguages();
  Languages->setCurrentText( language );
}

#include "DlgGeneral.cpp"
#include "moc_DlgGeneral.cpp"
#include "moc_DlgGeneralImp.cpp"
