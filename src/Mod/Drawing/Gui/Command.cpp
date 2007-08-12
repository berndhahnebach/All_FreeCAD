/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#	include <qaction.h>
# include <qfiledialog.h>
# include <qimage.h>
# include <qmessagebox.h>
#endif

#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/Command.h>
#include <Gui/BitmapFactory.h>

#include "DrawingView.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

using namespace DrawingGui;

DEF_STD_CMD(CmdDrawingOpen);

CmdDrawingOpen::CmdDrawingOpen()
	:Command("Drawing_Open")
{
	sAppModule		= "Drawing";
	sGroup			  = QT_TR_NOOP("Drawing");
	sMenuText		  = QT_TR_NOOP("Open");
	sToolTipText	= QT_TR_NOOP("Drawing open image view function");
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Open";
	iAccel			  = Qt::CTRL+Qt::Key_O;
}


void CmdDrawingOpen::activated(int iMsg)
{
  // Reading an image
  QString s = QFileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an image file to open"), QString::null, 
                                           QObject::tr("Drawings (*.png *.xpm *.jpg *.bmp)"));
  if (s.isEmpty() == false)
  {
    try{
      // load the file with the module
      Command::doCommand(Command::Gui, "import Drawing, DrawingGui");
      Command::doCommand(Command::Gui, "DrawingGui.open(\"%s\")", (const char*)s.toAscii());
    } catch (const Base::PyException& e){
      // Usually thrown if the file is invalid somehow
      e.ReportException();
    }
  }
}

void CreateDrawingCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	rcCmdMgr.addCommand(new CmdDrawingOpen());
}
