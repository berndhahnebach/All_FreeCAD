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
# include <BRepPrimAPI_MakeBox.hxx>
# include <TopoDS_Shape.hxx>
# include <TNaming_Builder.hxx>
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
// CmdPointsTest THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD_A(CmdPointsTest);

CmdPointsTest::CmdPointsTest()
  :CppCommand("Points_Test")
{
  sAppModule    = "Points";
  sGroup        = QT_TR_NOOP("Points");
  sMenuText     = QT_TR_NOOP("Import");
  sToolTipText  = QT_TR_NOOP("Points Test function");
  sWhatsThis    = QT_TR_NOOP("Points Test function");
  sStatusTip    = QT_TR_NOOP("Points Test function");
  sPixmap       = "Test1";
  iAccel        = Qt::CTRL+Qt::Key_T;
}

void CmdPointsTest::activated(int iMsg)
{

  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "Ascii Points (*.asc);;All Files (*.*)", Gui::getMainWindow() );
	if ( fn.isEmpty() )
		return;

//  PointsGui::DlgPointsReadImp cDlg(fn.latin1(), getAppWnd(),"ReadFile",true);
//	cDlg.exec();

  if (! fn.isEmpty() )
  {
    openCommand("Points Import Create");
    doCommand(Doc,"f = App.DocGet().AddFeature(\"PointsImport\",\"PointsImport\")");
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();
 
    updateActive();
  }


}

bool CmdPointsTest::isActive(void)
{
  if( getActiveGuiDocument() )
    return true;
  else
    return false;
}

void CreatePointsCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdPointsTest());
}
