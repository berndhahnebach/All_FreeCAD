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

#include "DlgGeneralImp.h"
#include "Action.h"
#include "Application.h"
#include "DockWindowManager.h"
#include "MainWindow.h"
#include "PrefWidgets.h"
#include "Workbench.h"
#include "WorkbenchManager.h"
#include "Language/Translator.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgGeneralImp */

/**
 *  Constructs a DlgGeneralImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgGeneralImp::DlgGeneralImp( QWidget* parent )
  : PreferencePage( parent ), watched(0)
{
  this->setupUi(this);
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
  // it, if it is set by the user
  QString curWbName = App::Application::Config()["StartWorkbench"].c_str();
  AutoloadModuleCombo->setCurrentText( curWbName );

  // do not save the content but the current item only
  AutoloadTabCombo->setKeepPreference( true );
  QWidget* dw = DockWindowManager::instance()->getDockWindow("Report view");
  if ( dw )
  {
    watched = dw->findChild<QTabWidget*>();
    if ( watched )
    {
      for (int i=0; i<watched->count(); i++)
        AutoloadTabCombo->insertItem( watched->label(i) );
      watched->installEventFilter(this);
    }
  }
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgGeneralImp::~DlgGeneralImp()
{
  // no need to delete child widgets, Qt does it all for us
  if (watched)
    watched->removeEventFilter(this);
}

/** Sets the size of the recent file list from the user parameters.
 * @see RecentFilesAction
 * @see StdCmdRecentFiles
 */
void DlgGeneralImp::setRecentFileSize()
{
    RecentFilesAction *recent = getMainWindow()->findChild<RecentFilesAction *>(QString("recentFiles"));
    if (recent) {
        ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("RecentFiles");
        recent->resizeList(hGrp->GetInt("RecentFiles", 4));
    }
}

void DlgGeneralImp::saveSettings()
{
  AutoloadModuleCombo->onSave();
  AutoloadTabCombo->onSave();
  RecentFiles->onSave();
  SplashScreen->onSave();
  AllowDragMenu->onSave();

  // set new user defined style
  (void)QApplication::setStyle(WindowStyle->currentText());

  setRecentFileSize();
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("General");
  QString language = hGrp->GetASCII("Language", "English").c_str();
  if ( QString::compare( Languages->currentText(), language ) != 0 )
  {
    hGrp->SetASCII("Language", Languages->currentText().latin1());
    Translator::instance()->installLanguage( Languages->currentText() );
  }
}

void DlgGeneralImp::loadSettings()
{
  // in case the user defined workbench is hidden
  QStringList work = Application::Instance->workbenches();
  QString curWbName = AutoloadModuleCombo->currentText();
  AutoloadModuleCombo->onRestore();
  QString newWbName = AutoloadModuleCombo->currentText();
  if (work.find(newWbName) == work.end())
    AutoloadModuleCombo->setCurrentText( curWbName );
  AutoloadTabCombo->onRestore();
  RecentFiles->onRestore();
  SplashScreen->onRestore();
  AllowDragMenu->onRestore();

  // fill up styles
  //
  QStringList styles = QStyleFactory::keys();
  WindowStyle->insertStringList( styles );
  QString style = QApplication::style()->objectName().toLower();
  int i=0;
  for (QStringList::ConstIterator it = styles.begin(); it != styles.end(); ++it, i++) {
    if (style == (*it).toLower()) {
      WindowStyle->setCurrentIndex( i );
      break;
    }
  }

  // search for the language files
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("General");
  QString language = hGrp->GetASCII("Language", "English").c_str();
  Languages->addItem("English"); 
  Languages->addItems(Translator::instance()->supportedLanguages());
  int ct=Languages->count();
  for (int i=0; i<ct; i++) {
    if (Languages->text(i) == language) {
      Languages->setCurrentIndex(i);
      break;
    }
  }
}

void DlgGeneralImp::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    retranslateUi(this);
  } else {
    QWidget::changeEvent(e);
  }
}

bool DlgGeneralImp::eventFilter(QObject* o, QEvent* e)
{
  // make sure that report tabs have been translated
  if (o == watched && e->type() == QEvent::LanguageChange) {
    for (int i=0; i<watched->count(); i++)
      AutoloadTabCombo->setItemText( i, watched->tabText(i) );
  }

  return QWidget::eventFilter(o, e);
}

#include "moc_DlgGeneralImp.cpp"
