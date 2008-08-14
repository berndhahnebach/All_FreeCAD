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

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Interpreter.h>
#include <Base/Sequencer.h>
#include <App/Document.h>
#include "Action.h"
#include "Application.h"
#include "Document.h"
#include "Splashscreen.h"
#include "Command.h"
#include "MainWindow.h"
#include "WhatsThis.h"
#include "DlgUndoRedo.h"
#include "BitmapFactory.h"
#include "View.h"

#include "DlgParameterImp.h"
#include "DlgMacroExecuteImp.h"
#include "DlgMacroRecordImp.h"
#include "Macro.h"
#include "DlgPreferencesImp.h"
#include "DlgCustomizeImp.h"
#include "Widgets.h"
#include "NetworkRetriever.h"
#include "OnlineDocumentation.h"
#include "GuiConsole.h"

using Base::Console;
using Base::Sequencer;
using namespace Gui;




//===========================================================================
// Std_Workbench
//===========================================================================

/* TRANSLATOR Gui::StdCmdWorkbench */

DEF_STD_CMD_AC(StdCmdWorkbench);

StdCmdWorkbench::StdCmdWorkbench()
  : Command("Std_Workbench")
{
  sGroup        = QT_TR_NOOP("View");
  sMenuText     = QT_TR_NOOP("Workbench");
  sToolTipText  = QT_TR_NOOP("Switch between workbenches");
  sWhatsThis    = QT_TR_NOOP("Switch between workbenches");
  sStatusTip    = QT_TR_NOOP("Switch between workbenches");
  sPixmap       = "FCIcon";
  iAccel        = 0;
}

void StdCmdWorkbench::activated(int iMsg)
{
  try {
    QList<QAction*> items = ((WorkbenchGroup*)_pcAction)->actions();
    doCommand(Gui, "Gui.activateWorkbench(\"%s\")", (const char*)items[iMsg]->objectName().toAscii());
  } catch(const Base::PyException& e) {
    QString msg(QLatin1String(e.what()));
    // ignore '<type 'exceptions.*Error'>' prefixes
    QRegExp rx;
    rx.setPattern(QLatin1String("^\\s*<type 'exceptions.\\w*'>:\\s*"));
    int pos = rx.indexIn(msg);
    if ( pos != -1 )
      msg = msg.mid(rx.matchedLength());
    QMessageBox::critical(getMainWindow(), QObject::tr("Cannot load workbench"), msg); 
  } catch(...) {
    QMessageBox::critical(getMainWindow(), QObject::tr("Cannot load workbench"), 
      QObject::tr("A general error occurred while loading the workbench")); 
  }
}

bool StdCmdWorkbench::isActive(void)
{
  return true;
}

Action * StdCmdWorkbench::createAction(void)
{
  Action *pcAction;

  pcAction = new WorkbenchGroup(this,getMainWindow());
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIcon(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setShortcut(iAccel);

  return pcAction;
}

//===========================================================================
// Std_RecentFiles
//===========================================================================

/* TRANSLATOR Gui::StdCmdRecentFiles */

DEF_STD_CMD_C(StdCmdRecentFiles)

StdCmdRecentFiles::StdCmdRecentFiles()
  :Command("Std_RecentFiles")
{
    sGroup        = QT_TR_NOOP("File");
    sMenuText     = QT_TR_NOOP("Recent files");
    sToolTipText  = QT_TR_NOOP("Recent file list");
    sWhatsThis    = QT_TR_NOOP("Recent file list");
    sStatusTip    = QT_TR_NOOP("Recent file list");
    iAccel        = 0;
}

/**
 * Opens the recent file at position \a iMsg in the menu.
 * If the file does not exist or cannot be loaded this item is removed
 * from the list.
 */
void StdCmdRecentFiles::activated(int iMsg)
{
    RecentFilesAction* act = qobject_cast<RecentFilesAction*>(_pcAction);
    if (act) act->activateFile( iMsg );
}

/**
 * Creates the QAction object containing the recent files.
 */
Action * StdCmdRecentFiles::createAction(void)
{
  RecentFilesAction* pcAction = new RecentFilesAction(this, getMainWindow());
  pcAction->setObjectName(QLatin1String("recentFiles"));
  pcAction->setDropDownMenu(true);
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  return pcAction;
}

//===========================================================================
// Std_About
//===========================================================================

/* TRANSLATOR Gui::StdCmdAbout */
DEF_STD_CMD_ACL(StdCmdAbout)

StdCmdAbout::StdCmdAbout()
  :Command("Std_About")
{
  sGroup        = QT_TR_NOOP("Help");
  sMenuText     = QT_TR_NOOP("&About %1");
  sToolTipText  = QT_TR_NOOP("About %1");
  sWhatsThis    = QT_TR_NOOP("About %1");
  sStatusTip    = QT_TR_NOOP("About %1");
  sPixmap       = App::Application::Config()["AppIcon"].c_str();
}

Action * StdCmdAbout::createAction(void)
{
  Action *pcAction;

  QString exe = QString::fromUtf8(App::Application::Config()["ExeName"].c_str());
  pcAction = new Action(this,getMainWindow());
  pcAction->setText( QObject::tr(sMenuText).arg(exe) );
  pcAction->setToolTip( QObject::tr(sToolTipText).arg(exe) );
  pcAction->setStatusTip( QObject::tr(sStatusTip).arg(exe) );
  pcAction->setWhatsThis( QObject::tr(sWhatsThis).arg(exe) );
  if(sPixmap)
    pcAction->setIcon(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setShortcut(iAccel);

  return pcAction;
}

bool StdCmdAbout::isActive()
{
  return true;
}

/**
 * Shows information about the application.
 */
void StdCmdAbout::activated(int iMsg)
{
  Gui::Dialog::AboutDialog dlg( getMainWindow() );
  dlg.exec();
}

void StdCmdAbout::languageChange()
{
  if ( _pcAction )
  {
      QString exe = QString::fromUtf8(App::Application::Config()["ExeName"].c_str());
    _pcAction->setText( QObject::tr(sMenuText).arg(exe) );
    _pcAction->setToolTip( QObject::tr(sToolTipText).arg(exe) );
    _pcAction->setStatusTip( QObject::tr(sStatusTip).arg(exe) );
    _pcAction->setWhatsThis( QObject::tr(sWhatsThis).arg(exe) );
  }
}

//===========================================================================
// Std_AboutQt
//===========================================================================
DEF_STD_CMD(StdCmdAboutQt);

StdCmdAboutQt::StdCmdAboutQt()
  :Command("Std_AboutQt")
{
  sGroup        = QT_TR_NOOP("Help");
  sMenuText     = QT_TR_NOOP("About &Qt");
  sToolTipText  = QT_TR_NOOP("About Qt");
  sWhatsThis    = QT_TR_NOOP("About Qt");
  sStatusTip    = QT_TR_NOOP("About Qt");
}

void StdCmdAboutQt::activated(int iMsg)
{
  qApp->aboutQt();
}

//===========================================================================
// Std_TipOfTheDay
//===========================================================================
DEF_STD_CMD(StdCmdTipOfTheDay);

StdCmdTipOfTheDay::StdCmdTipOfTheDay()
  :Command("Std_TipOfTheDay")
{
  sGroup        = QT_TR_NOOP("Help");
  sMenuText     = QT_TR_NOOP("&Tip of the day...");
  sToolTipText  = QT_TR_NOOP("Tip of the day");
  sWhatsThis    = QT_TR_NOOP("<p>Opens a dialog that shows a list of useful tips.</p>"
    "See <a href=\"index.php@TipOfTheDay.html\"><b>Tip of the day</b></a> for more details.");
  sStatusTip    = QT_TR_NOOP("Tip of the day");
}

void StdCmdTipOfTheDay::activated(int iMsg)
{
  getMainWindow()->showTipOfTheDay( true );
}

//===========================================================================
// Std_WhatsThis
//===========================================================================
DEF_STD_CMD(StdCmdWhatsThis);

StdCmdWhatsThis::StdCmdWhatsThis()
  :Command("Std_WhatsThis")
{
  sGroup        = QT_TR_NOOP("Help");
  sMenuText     = QT_TR_NOOP("&What's This?");
  sToolTipText  = QT_TR_NOOP("What's This");
  //	sWhatsThis		= sToolTipText;
  sWhatsThis    = "";
  sStatusTip    = QT_TR_NOOP("What's This");
  iAccel        = Qt::SHIFT+Qt::Key_F1;
  sPixmap       = "WhatsThis";
}

void StdCmdWhatsThis::activated(int iMsg)
{
  QWhatsThis::enterWhatsThisMode();
}

//===========================================================================
// Std_DlgParameter
//===========================================================================
DEF_STD_CMD(StdCmdDlgParameter);

StdCmdDlgParameter::StdCmdDlgParameter()
  :Command("Std_DlgParameter")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("E&dit parameters ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog to edit the parameters");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog to edit the parameters");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog to edit the parameters");
  //sPixmap     = "settings";
  iAccel        = 0;
}

void StdCmdDlgParameter::activated(int iMsg)
{
  Gui::Dialog::DlgParameterImp cDlg(getMainWindow());
  cDlg.exec();
}

//===========================================================================
// Std_DlgPreferences
//===========================================================================
DEF_STD_CMD(StdCmdDlgPreferences);

StdCmdDlgPreferences::StdCmdDlgPreferences()
  :Command("Std_DlgPreferences")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("&Preferences ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog to edit the preferences");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog to edit the preferences");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog to edit the preferences");
  //sPixmap     = "settings";
  iAccel        = 0;
}

void StdCmdDlgPreferences::activated(int iMsg)
{
  Gui::Dialog::DlgPreferencesImp cDlg(getMainWindow());
  cDlg.exec();
}

//===========================================================================
// Std_DlgMacroRecord
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroRecord);

StdCmdDlgMacroRecord::StdCmdDlgMacroRecord()
  :Command("Std_DlgMacroRecord")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("&Macro recording ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog to record a macro");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog to record a macro");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog to record a macro");
  sPixmap       = "macro-record";
  iAccel        = 0;
}

void StdCmdDlgMacroRecord::activated(int iMsg)
{
  Gui::Dialog::DlgMacroRecordImp cDlg(getMainWindow());
  cDlg.exec();
}

bool StdCmdDlgMacroRecord::isActive(void)
{
  return ! (getGuiApplication()->macroManager()->isOpen());
}

//===========================================================================
// Std_DlgMacroExecute
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroExecute);

StdCmdDlgMacroExecute::StdCmdDlgMacroExecute()
  :Command("Std_DlgMacroExecute")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("E&xecute macro ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog let you execute a recorded macro");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog let you execute a recorded macro");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog let you execute a recorded macro");
  sPixmap       = "macro-execute";
  iAccel        = 0;
}

void StdCmdDlgMacroExecute::activated(int iMsg)
{
  Gui::Dialog::DlgMacroExecuteImp cDlg(getMainWindow());
  cDlg.exec();
}

bool StdCmdDlgMacroExecute::isActive(void)
{
  return ! (getGuiApplication()->macroManager()->isOpen());
}

//===========================================================================
// Std_DlgMacroExecuteDirect
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroExecuteDirect);

StdCmdDlgMacroExecuteDirect::StdCmdDlgMacroExecuteDirect()
  :Command("Std_DlgMacroExecuteDirect")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("Execute &editor");
  sToolTipText  = QT_TR_NOOP("Execute the macro/script in the editor");
  sWhatsThis    = QT_TR_NOOP("Execute the macro/script in the editor");
  sStatusTip    = QT_TR_NOOP("Execute the macro/script in the editor");
  sPixmap       = "macro-execute";
  iAccel        = 0;
}

void StdCmdDlgMacroExecuteDirect::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"Run\")");
}

bool StdCmdDlgMacroExecuteDirect::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Run");
}

//===========================================================================
// Std_MacroStop
//===========================================================================
DEF_STD_CMD_A(StdCmdMacroStop);

StdCmdMacroStop::StdCmdMacroStop()
  :Command("Std_DlgMacroStop")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("S&top macro recording");
  sToolTipText  = QT_TR_NOOP("Stop the a running macro recording sassion");
  sWhatsThis    = QT_TR_NOOP("Stop the a running macro recording sassion");
  sStatusTip    = QT_TR_NOOP("Stop the a running macro recording sassion");
  sPixmap       = "macro-stop";
  iAccel        = 0;
}

void StdCmdMacroStop::activated(int iMsg)
{
  getGuiApplication()->macroManager()->commit();
}

bool StdCmdMacroStop::isActive(void)
{
  return getGuiApplication()->macroManager()->isOpen();
}

//===========================================================================
// Std_DlgCustomize
//===========================================================================
DEF_STD_CMD(StdCmdDlgCustomize);

StdCmdDlgCustomize::StdCmdDlgCustomize()
  :Command("Std_DlgCustomize")
{
    sGroup        = QT_TR_NOOP("Tools");
    sMenuText     = QT_TR_NOOP("Cu&stomize...");
    sToolTipText  = QT_TR_NOOP("Customize toolbars and commandbars");
    sWhatsThis    = QT_TR_NOOP("Customize toolbars and commandbars");
    sStatusTip    = QT_TR_NOOP("Customize toolbars and commandbars");
    //sPixmap     = "customize";
    iAccel        = 0;
}

void StdCmdDlgCustomize::activated(int iMsg)
{
    static QPointer<QDialog> dlg = 0;
    if (!dlg)
        dlg = new Gui::Dialog::DlgCustomizeImp(getMainWindow());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

//===========================================================================
// Std_CommandLine
//===========================================================================
DEF_STD_CMD(StdCmdCommandLine);

StdCmdCommandLine::StdCmdCommandLine()
  :Command("Std_CommandLine")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("Start command &line...");
  sToolTipText  = QT_TR_NOOP("Opens the command line in the console");
  sWhatsThis    = QT_TR_NOOP("Opens the command line in the console");
  sStatusTip    = QT_TR_NOOP("Opens the command line in the console");
  sPixmap       = "utilities-terminal";
  iAccel        = 0;
}

void StdCmdCommandLine::activated(int iMsg)
{
  bool show = getMainWindow()->isMaximized ();

  // pop up the Gui command window
  GUIConsole Wnd;

  getMainWindow()->showMinimized () ;
  qApp->processEvents();

  // create temporary console sequencer
  Base::ConsoleSequencer* seq = new Base::ConsoleSequencer;
  Base::Interpreter().runCommandLine("Console mode");
  delete seq;

#ifdef Q_WS_X11
  // On X11 this may not work. For further information see QWidget::showMaximized
  //
  // workaround for X11
  getMainWindow()->hide();
  getMainWindow()->show();
#endif

  // pop up the main window
  show ? getMainWindow()->showMaximized () : getMainWindow()->showNormal () ;
  qApp->processEvents();
}

//===========================================================================
// Std_OnlineHelp
//===========================================================================

DEF_STD_CMD(StdCmdOnlineHelp);

StdCmdOnlineHelp::StdCmdOnlineHelp()
  :Command("Std_OnlineHelp")
{
    sGroup        = QT_TR_NOOP("Help");
    sMenuText     = QT_TR_NOOP("Help");
    sToolTipText  = QT_TR_NOOP("Show help to the application");
    sWhatsThis    = QT_TR_NOOP("Help");
    sStatusTip    = QT_TR_NOOP("Help");
    sPixmap       = "help";
}

void StdCmdOnlineHelp::activated(int iMsg)
{
    std::string url = App::Application::Config()["AppHomePath"]+ "doc/FreeCAD.chm";
#if QT_VERSION >= 0x040200
    bool ok = QDesktopServices::openUrl(QString::fromUtf8(url.c_str()));
#elif defined(Q_WS_WIN)
    std::wstring wstr = Base::FileInfo(url).toStdWString();
    bool ok = (reinterpret_cast<int>(ShellExecuteW(NULL, NULL, wstr.c_str(), NULL,
                                                   NULL, SW_SHOWNORMAL)) > 32);
#else

#endif
    if (!ok) {
        QMessageBox::critical(getMainWindow(), QObject::tr("File not found"),
            QObject::tr("Cannot open file %1").arg(QString::fromUtf8(url.c_str())));
    }
}

//===========================================================================
// Std_OnlineHelpPython
//===========================================================================

DEF_STD_CMD(StdCmdOnlineHelpPython);

StdCmdOnlineHelpPython::StdCmdOnlineHelpPython()
  :Command("Std_OnlineHelpPython")
{
    sGroup        = QT_TR_NOOP("Help");
    sMenuText     = QT_TR_NOOP("Python Manuals");
    sToolTipText  = QT_TR_NOOP("Show the Python documentation");
    sWhatsThis    = QT_TR_NOOP("Show the Python documentation");
    sStatusTip    = QT_TR_NOOP("Show the Python documentation");
    sPixmap       = "python";
}

void StdCmdOnlineHelpPython::activated(int iMsg)
{
    std::string url = App::Application::Config()["AppHomePath"]+ "doc/Python25.chm";
#if QT_VERSION >= 0x040200
    bool ok = QDesktopServices::openUrl(QString::fromUtf8(url.c_str()));
#elif defined(Q_WS_WIN)
    std::wstring wstr = Base::FileInfo(url).toStdWString();
    bool ok = (reinterpret_cast<int>(ShellExecuteW(NULL, NULL, wstr.c_str(), NULL,
                                                   NULL, SW_SHOWNORMAL)) > 32);
#else

#endif
    if (!ok) {
        QMessageBox::critical(getMainWindow(), QObject::tr("File not found"),
            QObject::tr("Cannot open file %1").arg(QString::fromUtf8(url.c_str())));
    }
}

//===========================================================================
// Std_OnlineHelpWebsite
//===========================================================================

DEF_STD_CMD(StdCmdOnlineHelpWebsite);

StdCmdOnlineHelpWebsite::StdCmdOnlineHelpWebsite()
  :Command("Std_OnlineHelpWebsite")
{
    sGroup        = QT_TR_NOOP("Help");
    sMenuText     = QT_TR_NOOP("Help Website");
    sToolTipText  = QT_TR_NOOP("The website where the help is maintained");
    sWhatsThis    = QT_TR_NOOP("Help Website");
    sStatusTip    = QT_TR_NOOP("Help Website");
    sPixmap       = "help";
}

void StdCmdOnlineHelpWebsite::activated(int iMsg)
{
    ParameterGrp::handle hURLGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/OnlineHelp");
    std::string url = hURLGrp->GetASCII("DownloadURL", "http://juergen-riegel.net/FreeCAD/Docu/index.php?title=Online_Help_Toc");
    OpenURLInBrowser(url.c_str());
}

//===========================================================================
// Std_FreeCADWebsite
//===========================================================================

DEF_STD_CMD(StdCmdFreeCADWebsite);

StdCmdFreeCADWebsite::StdCmdFreeCADWebsite()
  :Command("Std_FreeCADWebsite")
{
    sGroup        = QT_TR_NOOP("Help");
    sMenuText     = QT_TR_NOOP("FreeCAD Website");
    sToolTipText  = QT_TR_NOOP("The FreeCAD website");
    sWhatsThis    = QT_TR_NOOP("FreeCAD Website");
    sStatusTip    = QT_TR_NOOP("FreeCAD Website");
    sPixmap       = "help";
}

void StdCmdFreeCADWebsite::activated(int iMsg)
{
    OpenURLInBrowser("http://juergen-riegel.net/FreeCAD/Docu/index.php?title=Main_Page");
}

//===========================================================================
// Std_PythonWebsite
//===========================================================================

DEF_STD_CMD(StdCmdPythonWebsite);

StdCmdPythonWebsite::StdCmdPythonWebsite()
  :Command("Std_PythonWebsite")
{
    sGroup        = QT_TR_NOOP("Help");
    sMenuText     = QT_TR_NOOP("Python Website");
    sToolTipText  = QT_TR_NOOP("The official Python website");
    sWhatsThis    = QT_TR_NOOP("Python Website");
    sStatusTip    = QT_TR_NOOP("Python Website");
    sPixmap       = "python";
}

void StdCmdPythonWebsite::activated(int iMsg)
{
    OpenURLInBrowser("http://python.org");
}

namespace Gui {

void CreateStdCommands(void)
{
    CommandManager &rcCmdMgr = Application::Instance->commandManager();

    rcCmdMgr.addCommand(new StdCmdAbout());
    rcCmdMgr.addCommand(new StdCmdAboutQt());

    rcCmdMgr.addCommand(new StdCmdDlgParameter());
    rcCmdMgr.addCommand(new StdCmdDlgPreferences());
    rcCmdMgr.addCommand(new StdCmdDlgMacroRecord());
    rcCmdMgr.addCommand(new StdCmdDlgMacroExecute());
    rcCmdMgr.addCommand(new StdCmdDlgMacroExecuteDirect());
    rcCmdMgr.addCommand(new StdCmdMacroStop());
    rcCmdMgr.addCommand(new StdCmdDlgCustomize());
    rcCmdMgr.addCommand(new StdCmdCommandLine());
    rcCmdMgr.addCommand(new StdCmdWorkbench());
    rcCmdMgr.addCommand(new StdCmdRecentFiles());
    rcCmdMgr.addCommand(new StdCmdWhatsThis());
    rcCmdMgr.addCommand(new StdCmdPythonHelp());
    rcCmdMgr.addCommand(new StdCmdOnlineHelp());
    rcCmdMgr.addCommand(new StdCmdOnlineHelpPython());
    rcCmdMgr.addCommand(new StdCmdOnlineHelpWebsite());
    rcCmdMgr.addCommand(new StdCmdFreeCADWebsite());
    rcCmdMgr.addCommand(new StdCmdPythonWebsite());
    //rcCmdMgr.addCommand(new StdCmdDownloadOnlineHelp());
    rcCmdMgr.addCommand(new StdCmdTipOfTheDay());
    rcCmdMgr.addCommand(new StdCmdDescription());
}

} // namespace Gui
