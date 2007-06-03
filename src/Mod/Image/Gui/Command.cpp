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

#include "ImageView.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

using namespace ImageGui;

DEF_STD_CMD(CmdImageOpen);

CmdImageOpen::CmdImageOpen()
	:Command("Image_Open")
{
	sAppModule		= "Image";
	sGroup			  = QT_TR_NOOP("Image");
	sMenuText		  = QT_TR_NOOP("Open");
	sToolTipText	= QT_TR_NOOP("Image open image view function");
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Open";
	iAccel			  = Qt::CTRL+Qt::Key_O;
}


void CmdImageOpen::activated(int iMsg)
{
  // Reading an image
  QString s = QFileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an image file to open"), QString::null, 
                                           QObject::tr("Images (*.png *.xpm *.jpg *.bmp)"));
  if (s.isEmpty() == false)
  {
    try{
      // load the file with the module
      Command::doCommand(Command::Gui, "import Image, ImageGui");
      Command::doCommand(Command::Gui, "ImageGui.open(\"%s\")", (const char*)s.toAscii());
    } catch (const Base::PyException& e){
      // Usually thrown if the file is invalid somehow
      e.ReportException();
    }
  }
}

void CreateImageCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	rcCmdMgr.addCommand(new CmdImageOpen());
}
