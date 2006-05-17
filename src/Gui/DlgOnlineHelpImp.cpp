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
# include <qfileinfo.h>
# include <qdir.h>
# include <qmessagebox.h>
#endif

#include "DlgOnlineHelpImp.h"
#include "PrefWidgets.h"

#include "../Base/Parameter.h"
#include "../App/Application.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgOnlineHelpImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgOnlineHelpImp::DlgOnlineHelpImp( QWidget* parent, const char* name, WFlags fl )
: DlgOnlineHelpBase(parent, name, fl)
{
  prefStartPage->setFilter( "All Html files (*.html *.htm)" );
  if ( prefStartPage->fileName().isEmpty() )
  {
    prefStartPage->setFileName( getStartpage() );
  }

  if ( DownloadLoc->fileName().isEmpty() )
  {
    // set output directory
    QString path = App::GetApplication().GetHomePath();
    path += "/doc/";
    QDir dir(path);
    DownloadLoc->setFileName( dir.absPath() );
  }
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgOnlineHelpImp::~DlgOnlineHelpImp()
{
}

/**
 * Returns the start page for the HelpView. If none is defined the default 
 * start page "<FreeCADHome>/doc/free-cad.sourceforge.net/wiki/index.php.html" 
 * is returned.
 * \remark It is not checked if the returned page really exists.
 */
QString DlgOnlineHelpImp::getStartpage()
{
  ParameterGrp::handle hURLGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/OnlineHelp");
  QString home = QString(hURLGrp->GetASCII( "Startpage", "" ).c_str());

  // help start in in config?
  if ( home.isEmpty() && App::Application::Config()["HelpStart"] != "")
  {
    home = App::GetApplication().GetHomePath();
    home +=   App::Application::Config()["HelpStart"].c_str();
  }

  if ( home.isEmpty() )
  {
    QString hm = App::GetApplication().GetHomePath();
    hm += "/doc/free-cad.sourceforge.net/wiki";
    QDir d(hm);
    home = d.path();
    home += "/index.php.html";
  }

  return home;
}

void DlgOnlineHelpImp::saveSettings()
{
  PrefLineEdit2->onSave();
  UseProxy->onSave();
  DownloadURL->onSave();
  prefExtBrowser->onSave();
  prefStartPage->onSave();
  prefAuthorize->onSave();
  DownloadLoc->onSave();
}

void DlgOnlineHelpImp::loadSettings()
{
  PrefLineEdit2->onRestore();
  UseProxy->onRestore();
  DownloadURL->onRestore();
  prefExtBrowser->onRestore();
  prefStartPage->onRestore();
  prefAuthorize->onRestore();
  DownloadLoc->onRestore();
}

void DlgOnlineHelpImp::onCheckLocation( const QString& url )
{
  QFileInfo fi( url );
  if ( !fi.permission( QFileInfo::WriteUser ) )
  {
    QMessageBox::critical(this, tr("Missing permission"), tr("You don't have write permission to '%1'\n\n"
      "Specify another directory, please.").arg(url));
  }
}

#include "DlgOnlineHelp.cpp"
#include "moc_DlgOnlineHelp.cpp"
#include "moc_DlgOnlineHelpImp.cpp"
