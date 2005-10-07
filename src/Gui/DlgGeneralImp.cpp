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
#include "DockWindow.h"
#include "MainWindow.h"
#include "PrefWidgets.h"
#include "Language/LanguageFactory.h"
#include "Language/Translator.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgGeneralImp */

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
  // fills the combo box with all available workbenches
  QStringList work = Application::Instance->workbenches();
  work.sort();
  // do not save the content but the current item only
  AutoloadModuleCombo->setKeepPreference( true );
  for ( QStringList::Iterator it = work.begin(); it != work.end(); ++it )
  {
    QPixmap px = Application::Instance->workbenchIcon( *it );
    if ( px.isNull() )
      AutoloadModuleCombo->insertItem( *it );
    else
      AutoloadModuleCombo->insertItem( px, *it );
  }
  // set the current workbench as default, AutoloadModuleCombo->onRestore() will change
  // it, if it is set in the preferences
  Workbench* curWb = WorkbenchManager::instance()->active();
  QString curWbName = curWb ? curWb->name() : "<none>";
  AutoloadModuleCombo->setCurrentText( curWbName );

  // do not save the content but the current item only
  AutoloadTabCombo->setKeepPreference( true );
  DockWindow* dw = DockWindowManager::instance()->getDockWindow("Report View");
  if ( dw )
  {
    QTabWidget* tab = (QTabWidget*)(dw->child ( "TabWidget", "QTabWidget" ));
    if ( tab )
    {
      for (int i=0; i<tab->count(); i++)
        AutoloadTabCombo->insertItem( tab->label(i) );
    }
  }
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
  CommandManager& rclMan = Application::Instance->commandManager();
  Command* pCmd = rclMan.getCommandByName("Std_MRU");
  if (pCmd)
  {
    ParameterGrp::handle hGrp = WindowParameter::getParameter()->GetGroup("RecentFiles");
    ((StdCmdMRU*)pCmd)->setMaxCount(hGrp->GetInt("RecentFiles", 4));
  }
}

/** Searches for all registered languages and insert them into a combo box */
void DlgGeneralImp::insertLanguages()
{
  QStringList list = Gui::LanguageFactory().getRegisteredLanguages();

  Languages->insertItem("English");

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    Languages->insertItem(*it);
  }
}

void DlgGeneralImp::saveSettings()
{
  AutoloadModuleCombo->onSave();
  AutoloadTabCombo->onSave();
  RecentFiles->onSave();
  SplashScreen->onSave();
  ShowCmdLine->onSave();
  SizeCmdLine->onSave();
  AllowDragMenu->onSave();
  UsesBigPixmaps->onSave();

  ParameterGrp::handle hGrp = WindowParameter::getParameter()->GetGroup("General");
  hGrp->SetASCII( "WindowStyle", WindowStyle->currentText().latin1() );

  getMainWindow()->updateStyle();
  getMainWindow()->updatePixmapsSize();
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
  // in case the user defined workbench is hidden
  std::string hidden = App::Application::Config()["HiddenWorkbench"];
  QString curWbName = AutoloadModuleCombo->currentText();
  AutoloadModuleCombo->onRestore();
  if ( hidden.find( AutoloadModuleCombo->currentText().latin1() ) != std::string::npos )
    AutoloadModuleCombo->setCurrentText( curWbName );
  AutoloadTabCombo->onRestore();
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
  ParameterGrp::handle hGrp = WindowParameter::getParameter()->GetGroup("General");
  QString language = hGrp->GetASCII("Language", "English").c_str();
  insertLanguages();
  Languages->setCurrentText( language );
}

#include "DlgGeneral.cpp"
#include "moc_DlgGeneral.cpp"
#include "moc_DlgGeneralImp.cpp"
