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

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgGeneralImp::~DlgGeneralImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/** Applies the made changes */
void DlgGeneralImp::apply()
{
  if (QString::compare(Languages->currentText(), language) != 0)
  {
    FCMessageBox::information(ApplicationWindow::Instance, "Info", tr("To take effect on the new language restart FreeCAD, please"));
    GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Window")->GetGroup
    ("Language")->SetASCII("Language", Languages->currentText().latin1());
  }
}

/** Looks in the system parameters for the entry "BigPixmaps".
 * If it is true the tool buttons in the main window are set to use big pixmaps, 
 * and small pixmaps if "BigPixmaps" is false.
 * @see ApplicationWindow, QMainWindow
 */
void DlgGeneralImp::onBigPixmaps()
{
  ApplicationWindow::Instance->UpdatePixmapsSize();
}

/** Sets the application's style
 * @see QStyle, QApplication
 */
void DlgGeneralImp::onSetStyle()
{
  ApplicationWindow::Instance->UpdateStyle();
}

/** Sets the size of the recent file list (MRU) in the
 * system parameters.
 * @see FCCmdMRU
 */
void DlgGeneralImp::onSetMRUSize()
{
  FCCommandManager& rclMan = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCmd = rclMan.GetCommandByName("Std_MRU");
  if (pCmd)
  {
    FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("Recent files");
    ((FCCmdMRU*)pCmd)->SetMaxItems(hGrp->GetInt("RecentFiles", 4));
  }
}

/** Shows/hides the command line 
 */
void DlgGeneralImp::onSetCmdLineVisible()
{
  CommandLine().show();
}

/** \todo */
void DlgGeneralImp::onChangeLanguage(const QString&)
{
  setModified(true);
}

/** Searches for all registered languages and insert them into a combo box */
void DlgGeneralImp::onSearchForLanguages()
{
  QStringList list = Gui::LanguageFactory().getRegisteredLanguages();

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
