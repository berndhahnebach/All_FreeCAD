/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
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
# include <qaction.h>
# include <qdir.h>
# include <qfileinfo.h>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/Command.h>
#include <Gui/FileDialog.h>

#include "DlgPointsReadImp.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdPointsImport
//===========================================================================
DEF_STD_CMD_A(CmdPointsImport);

CmdPointsImport::CmdPointsImport()
  :Command("Points_Import")
{
  sAppModule    = "Points";
  sGroup        = QT_TR_NOOP("Points");
  sMenuText     = QT_TR_NOOP("Import Points");
  sToolTipText  = QT_TR_NOOP("Imports a point cloud");
  sWhatsThis    = QT_TR_NOOP("Imports a point cloud");
  sStatusTip    = QT_TR_NOOP("Imports a point cloud");
  sPixmap       = "Test1";
}

void CmdPointsImport::activated(int iMsg)
{
  // use current path as default
  std::string path = QDir::currentDirPath().latin1();
  FCHandle<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("General");
  path = hPath->GetASCII("FileOpenSavePath", path.c_str());
  QString dir = path.c_str();

  QString fn = Gui::FileDialog::getOpenFileName( dir, "Ascii Points (*.asc);;All Files (*.*)", Gui::getMainWindow() );
  if ( fn.isEmpty() )
    return;

  if (! fn.isEmpty() )
  {
    QFileInfo fi;
    fi.setFile(fn);

    openCommand("Points Import Create");
    doCommand(Doc,"f = App.document().AddFeature(\"Points::ImportAscii\",\"%s\")", fi.baseName().latin1());
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();
 
    updateActive();
    hPath->SetASCII("FileOpenSavePath", fi.dirPath(true).latin1());
  }
}

bool CmdPointsImport::isActive(void)
{
  if( getActiveGuiDocument() )
    return true;
  else
    return false;
}

void CreatePointsCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdPointsImport());
}
