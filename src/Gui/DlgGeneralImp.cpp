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
#include "Language/LanguageFactory.h"

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
  setParamGrpPath("User parameter:BaseApp/Preferences/General");
  setEntryName("General");

  append(UsesBigPixmaps->getHandler());
  append(AllowDragMenu->getHandler());
  append(RecentFiles->getHandler());
  append(SplashScreen->getHandler());
  append(ShowCmdLine->getHandler());
  append(SizeCmdLine->getHandler());
  append(getHandler()); // this dialog
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgGeneralImp::~DlgGeneralImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgGeneralImp::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
}

/** Restores the color map */
void DlgGeneralImp::restorePreferences()
{
  // fill up styles
  //
  QStringList styles = QStyleFactory::keys ();
  WindowStyle->insertStringList( styles );
  QString style = QApplication::style().name();
  WindowStyle->setCurrentText( style );

  // search for the language files
  QString language = hPrefGrp->GetASCII("Language", "English").c_str();
  insertLanguages();
  Languages->setCurrentText( language );
}

/** Saves the color map */
void DlgGeneralImp::savePreferences()
{
  hPrefGrp->SetASCII( "WindowStyle", WindowStyle->currentText().latin1() );

  ApplicationWindow::Instance->UpdateStyle();
  ApplicationWindow::Instance->UpdatePixmapsSize();
  setMRUSize();
  CommandLine().show();

  QString language = hPrefGrp->GetASCII("Language", "English").c_str();
  if ( QString::compare( Languages->currentText(), language ) != 0 )
  {
    CheckMessageBox::information(ApplicationWindow::Instance, "Info", tr("To take effect on the new language restart FreeCAD, please"));
    hPrefGrp->SetASCII("Language", Languages->currentText().latin1());
  }
}

/** Sets the size of the recent file list (MRU) in the
 * user parameters.
 * @see FCCmdMRU
 */
void DlgGeneralImp::setMRUSize()
{
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCmd = rclMan.GetCommandByName("Std_MRU");
  if (pCmd)
  {
    FCParameterGrp::handle hGrp = GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("RecentFiles");
    ((FCCmdMRU*)pCmd)->setMaxCount(hGrp->GetInt("RecentFiles", 4));
  }
}

/** Searches for all registered languages and insert them into a combo box */
void DlgGeneralImp::insertLanguages()
{
  QStringList list = Gui::LanguageFactory().getRegisteredLanguages();

  int pos = 0;
  Languages->insertItem(tr("English"));

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    Languages->insertItem(*it);
  }
}

#include "DlgGeneral.cpp"
#include "moc_DlgGeneral.cpp"
#include "moc_DlgGeneralImp.cpp"
