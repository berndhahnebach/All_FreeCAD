/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
# if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
# endif // (re-)defined in pyconfig.h
# include <Python.h>
# include <qapplication.h>
# include <qstatusbar.h>
# include <qfiledialog.h>
# include <qobjectlist.h>
# include <qmessagebox.h>
# include <qprinter.h>
# include <qtimer.h>
#endif

#ifdef FC_USE_OCAFBROWSER
# include <DebugBrowser.hxx>
#endif

#include "../Base/Exception.h"
#include "../Base/Interpreter.h"
#include "../Base/Sequencer.h"
#include "../App/Document.h"
#include "Action.h"
#include "Process.h"
#include "Application.h"
#include "Document.h"
#include "Splashscreen.h"
#include "Command.h"
#include "MainWindow.h"
#include "WhatsThis.h"
#include "DlgUndoRedo.h"
#include "BitmapFactory.h"
#include "View.h"

//#include "DlgDocTemplatesImp.h"
#include "DlgParameterImp.h"
#include "DlgMacroExecuteImp.h"
#include "DlgMacroRecordImp.h"
#include "Macro.h"
#include "DlgPreferencesImp.h"
#include "DlgCustomizeImp.h"
#include "Widgets.h"
#include "NetworkRetriever.h"
#include "GuiConsole.h"

using Base::Console;
using Base::Sequencer;
using namespace Gui;



//===========================================================================
// Std_Open
//===========================================================================

DEF_STD_CMD(StdCmdOpen);

StdCmdOpen::StdCmdOpen()
  :Command("Std_Open")
{
  // seting the
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Open");
  sToolTipText  = QT_TR_NOOP("Open a Document or import Files");
  sWhatsThis    = QT_TR_NOOP("Open a Document or import Files");
  sStatusTip    = QT_TR_NOOP("Open a Document or import Files");
  sPixmap       = "Open";
  iAccel        = Qt::CTRL+Qt::Key_O;
}

void StdCmdOpen::activated(int iMsg)
{
  // fill the list of registered endings
  const std::map<std::string,std::string> &EndingMap = App::GetApplication().getOpenType();
  std::string EndingList;
  
  EndingList = "All suported formats (";

  std::map<std::string,std::string>::const_iterator It;
  for(It=EndingMap.begin();It != EndingMap.end();It++)
  {
    EndingList += ";*." + It->first;
  }

  EndingList += ");;";

  for(It=EndingMap.begin();It != EndingMap.end();It++)
  {
    EndingList += It->second + " (*." + It->first + ");;";
  }
  EndingList += "All files (*.*)";
  

  QStringList FileList = QFileDialog::getOpenFileNames( EndingList.c_str(),QString::null, getMainWindow() );

  for ( QStringList::Iterator it = FileList.begin(); it != FileList.end(); ++it ) {
     getGuiApplication()->open((*it).latin1());
  }
}

//===========================================================================
// Std_New
//===========================================================================

DEF_STD_CMD(StdCmdNew);

StdCmdNew::StdCmdNew()
  :Command("Std_New")
{
  // seting the 
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&New");
  sToolTipText  = QT_TR_NOOP("Create a new empty Document");
  sWhatsThis    = QT_TR_NOOP("Create a new empty Document");
  sStatusTip    = QT_TR_NOOP("Create a new empty Document");
  sPixmap       = "New";
  iAccel        = Qt::CTRL+Qt::Key_N;
}

void StdCmdNew::activated(int iMsg)
{
  doCommand(Command::Doc,"App.New()");
}

//===========================================================================
// Std_Save
//===========================================================================
DEF_STD_CMD_A(StdCmdSave);

StdCmdSave::StdCmdSave()
  :Command("Std_Save")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Save");
  sToolTipText  = QT_TR_NOOP("Save the active document");
  sWhatsThis    = QT_TR_NOOP("Save the active document");
  sStatusTip    = QT_TR_NOOP("Save the active document");
  sPixmap       = "Save";
  iAccel        = Qt::CTRL+Qt::Key_S;
}

void StdCmdSave::activated(int iMsg)
{
  if( getActiveGuiDocument() )
//    DoCommand(Command::Doc,"FreeCAD.DocGetActive().Save()");		
    getActiveGuiDocument()->save();
  else
    doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Save\")");
//    getGuiApplication()->SendMsgToActiveView("Save");
}

bool StdCmdSave::isActive(void)
{
  if( getActiveGuiDocument() )
    return true;
  else
    return getGuiApplication()->sendHasMsgToActiveView("Save");
}

//===========================================================================
// Std_SaveAs
//===========================================================================
DEF_STD_CMD_A(StdCmdSaveAs);

StdCmdSaveAs::StdCmdSaveAs()
  :Command("Std_SaveAs")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("Save &As...");
  sToolTipText  = QT_TR_NOOP("Save the active document under a new file name");
  sWhatsThis    = QT_TR_NOOP("Save the active document under a new file name");
  sStatusTip    = QT_TR_NOOP("Save the active document under a new file name");
  iAccel        = 0;
}

void StdCmdSaveAs::activated(int iMsg)
{
  if( getActiveGuiDocument() )
    getActiveGuiDocument()->saveAs();
  else
    doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SaveAs\")");
}

bool StdCmdSaveAs::isActive(void)
{
  if( getActiveGuiDocument() )
    return true;
  else
    return getGuiApplication()->sendHasMsgToActiveView("SaveAs");
}

//===========================================================================
// Std_Print
//===========================================================================
DEF_STD_CMD_A(StdCmdPrint );

StdCmdPrint::StdCmdPrint()
  :Command("Std_Print")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Print...");
  sToolTipText  = QT_TR_NOOP("Print the window");
  sWhatsThis    = QT_TR_NOOP("Print the window");
  sStatusTip    = QT_TR_NOOP("Print the window");
  sPixmap       = "Print";
  iAccel        = Qt::CTRL+Qt::Key_P;;
}

void StdCmdPrint::activated(int iMsg)
{
  if ( getMainWindow()->activeWindow() )
  {
    getMainWindow()->statusBar()->message("Printing...");
    QPrinter printer( QPrinter::HighResolution );
    getMainWindow()->activeWindow()->print( &printer );
  }
}

bool StdCmdPrint::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Print");
}

//===========================================================================
// Std_Quit
//===========================================================================

DEF_STD_CMD(StdCmdQuit );

StdCmdQuit::StdCmdQuit()
  :Command("Std_Quit")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("E&xit");
  sToolTipText  = QT_TR_NOOP("Quits the application");
  sWhatsThis    = QT_TR_NOOP("Quits the application");
  sStatusTip    = QT_TR_NOOP("Quits the application");
  iAccel        = Qt::ALT+Qt::Key_F4;
}

void StdCmdQuit::activated(int iMsg)
{
  getMainWindow()->close();
}

//===========================================================================
// Std_Undo
//===========================================================================

DEF_STD_CMD_AC(StdCmdUndo);

StdCmdUndo::StdCmdUndo()
  :Command("Std_Undo")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Undo");
  sToolTipText  = QT_TR_NOOP("Undo exactly one action");
  sWhatsThis    = QT_TR_NOOP("Undo exactly one action");
  sStatusTip    = QT_TR_NOOP("Undo exactly one action");
  sPixmap       = "Undo";
  iAccel        = Qt::CTRL+Qt::Key_Z;
}

void StdCmdUndo::activated(int iMsg)
{
//  Application::Instance->slotUndo();
  getGuiApplication()->sendMsgToActiveView("Undo");
}

bool StdCmdUndo::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Undo");
}

QAction * StdCmdUndo::createAction(void)
{
  QAction *pcAction;

  pcAction = new UndoAction(this,getMainWindow(),sName);
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  return pcAction;
}

//===========================================================================
// Std_Redo
//===========================================================================

DEF_STD_CMD_AC(StdCmdRedo );

StdCmdRedo::StdCmdRedo()
  :Command("Std_Redo")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Redo");
  sToolTipText  = QT_TR_NOOP("Redoes a previously undid action");
  sWhatsThis    = QT_TR_NOOP("Redoes a previously undid action");
  sStatusTip    = QT_TR_NOOP("Redoes a previously undid action");
  sPixmap       = "Redo";
  iAccel        = Qt::CTRL+Qt::Key_Y;
}

void StdCmdRedo::activated(int iMsg)
{
//  Application::Instance->slotRedo();
  getGuiApplication()->sendMsgToActiveView("Redo");
}

bool StdCmdRedo::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Redo");
}

QAction * StdCmdRedo::createAction(void)
{
  QAction *pcAction;

  pcAction = new RedoAction(this,getMainWindow(),sName);
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  return pcAction;
}

//===========================================================================
// Std_Cut
//===========================================================================
DEF_STD_CMD_A(StdCmdCut);

StdCmdCut::StdCmdCut()
  :Command("Std_Cut")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Cut");
  sToolTipText  = QT_TR_NOOP("Cut out");
  sWhatsThis    = QT_TR_NOOP("Cut out");
  sStatusTip    = QT_TR_NOOP("Cut out");
  sPixmap       = "Cut";
  iAccel        = Qt::CTRL+Qt::Key_X;
}

void StdCmdCut::activated(int iMsg)
{
  getGuiApplication()->sendMsgToActiveView("Cut");
}

bool StdCmdCut::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Cut");
}

//===========================================================================
// Std_Copy
//===========================================================================
DEF_STD_CMD_A(StdCmdCopy);

StdCmdCopy::StdCmdCopy()
  :Command("Std_Copy")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("C&opy");
  sToolTipText  = QT_TR_NOOP("Copy operation");
  sWhatsThis    = QT_TR_NOOP("Copy operation");
  sStatusTip    = QT_TR_NOOP("Copy operation");
  sPixmap       = "Copy";
  iAccel        = Qt::CTRL+Qt::Key_C;
}

void StdCmdCopy::activated(int iMsg)
{
  getGuiApplication()->sendMsgToActiveView("Copy");
}

bool StdCmdCopy::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Copy");
}

//===========================================================================
// Std_Paste
//===========================================================================
DEF_STD_CMD_A(StdCmdPaste);

StdCmdPaste::StdCmdPaste()
  :Command("Std_Paste")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Paste");
  sToolTipText  = QT_TR_NOOP("Paste operation");
  sWhatsThis    = QT_TR_NOOP("Paste operation");
  sStatusTip    = QT_TR_NOOP("Paste operation");
  sPixmap       = "Paste";
  iAccel        = Qt::CTRL+Qt::Key_V;
}

void StdCmdPaste::activated(int iMsg)
{
  getGuiApplication()->sendMsgToActiveView("Paste");
}

bool StdCmdPaste::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Paste");
}


namespace Gui {

void CreateDocCommands(void)
{
  CommandManager &rcCmdMgr = Application::Instance->commandManager();

  rcCmdMgr.addCommand(new StdCmdNew());
  rcCmdMgr.addCommand(new StdCmdOpen());

  rcCmdMgr.addCommand(new StdCmdSave());
  rcCmdMgr.addCommand(new StdCmdSaveAs());
  rcCmdMgr.addCommand(new StdCmdUndo());
  rcCmdMgr.addCommand(new StdCmdRedo());
  rcCmdMgr.addCommand(new StdCmdPrint());
  rcCmdMgr.addCommand(new StdCmdQuit());
  rcCmdMgr.addCommand(new StdCmdCut());
  rcCmdMgr.addCommand(new StdCmdCopy());
  rcCmdMgr.addCommand(new StdCmdPaste());
}

} // namespace Gui
