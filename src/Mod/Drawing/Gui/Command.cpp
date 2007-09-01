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
#endif

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>

#include "DrawingView.h"

using namespace DrawingGui;

DEF_STD_CMD(CmdDrawingOpen);

CmdDrawingOpen::CmdDrawingOpen()
	:Command("Drawing_Open")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Open SVG...");
    sToolTipText    = QT_TR_NOOP("Open a scalable vector graphic");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/document-new";
}


void CmdDrawingOpen::activated(int iMsg)
{
    // Reading an image
    QString filename = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an SVG file to open"), QString::null, 
                                           QObject::tr("Scalable Vector Graphics (*.svg *.svgz)"));
    if (!filename.isEmpty())
    {
        // load the file with the module
        Command::doCommand(Command::Gui, "import Drawing, DrawingGui");
        Command::doCommand(Command::Gui, "DrawingGui.open(\"%s\")", (const char*)filename.toUtf8());
    }
}

void CreateDrawingCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdDrawingOpen());
}
