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
// Std_Workbench
//===========================================================================

/* TRANSLATOR Gui::StdCmdWorkbench */

StdCmdWorkbench::StdCmdWorkbench()
  :Command("Std_Workbench"), pcAction(NULL)
{
  sGroup        = QT_TR_NOOP("View");
  sMenuText     = QT_TR_NOOP("Workbench");
  sToolTipText  = QT_TR_NOOP("Switch between workbenches");
  sWhatsThis    = QT_TR_NOOP("Switch between workbenches");
  sStatusTip    = QT_TR_NOOP("Switch between workbenches");
  sPixmap       = "FCIcon";
  iAccel        = 0;
}

/**
 * Activates the workbench at the position \a pos.
 */
void StdCmdWorkbench::activated( int pos )
{
  QStringList wb = Application::Instance->workbenches();
  if (pos >= 0 && pos < int(wb.size()))
    activate( wb[pos] );
}

/**
 * Activates the workbench with the name \a item.
 */
void StdCmdWorkbench::activate ( const QString& item )
{
  try{
    doCommand(Gui, "Gui.ActivateWorkbench(\"%s\")", item.latin1());
  }
  catch(const Base::Exception&)
  {
    // just do nothing because for sure the exception has already been reported
  }
}

void StdCmdWorkbench::notify( const QString& item )
{
  if ( pcAction )
    dynamic_cast<WorkbenchGroup*>(pcAction)->activate( item );
}

/**
 * Creates the QAction object containing all workbenches.
 */
QAction * StdCmdWorkbench::createAction(void)
{
  pcAction = new WorkbenchGroup( getMainWindow(), sName, true );
  pcAction->setExclusive( true );
  pcAction->setUsesDropDown( true );
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  // fill up with workbench actions
  refresh();

  return pcAction;
}

/**
 * Appends a new workbench with name \a item to the list of workbenches.
 */
void StdCmdWorkbench::addWorkbench ( const QString& item )
{
  if ( pcAction )
  {
    QAction* action = new WorkbenchAction( this, pcAction, item );
    action->setText( item ); // native literal
    action->setMenuText( item );
    action->setToggleAction( true );
    QPixmap px = Application::Instance->workbenchIcon( item );
    if ( px.isNull() )
      action->setIconSet(Gui::BitmapFactory().pixmap(App::Application::Config()["AppIcon"].c_str()));
    else
      action->setIconSet( px );
    pcAction->add( action );
  }
}

/**
 * Refreshes the list of available workbenches.
 */
void StdCmdWorkbench::refresh ()
{
  if ( pcAction )
  {
    // remove all workbench actions and rebuild it
    QObjectList *childs = pcAction->queryList( "Gui::WorkbenchAction" );
    QObjectListIt it( *childs ); // iterate over the actions
    QObject *obj;

    while ( (obj = it.current()) != 0 ) 
    {
      pcAction->removeChild( obj );
      delete obj;
      ++it;
    }
    delete childs; // delete the list, not the objects  

    // sort the workbenches
    QStringList items = Application::Instance->workbenches();
    items.sort();
    for (QStringList::Iterator item = items.begin(); item!=items.end(); ++item)
      addWorkbench( *item );
  }
}

/** 
 * Can only be added to the "file operations" toolbar or any other kind of widgets. This is because this 
 * command changes the workbenches and so there will several toolbars/cmdbars be deleted. If the 
 * corresponding combobox were inside such a toolbar/cmdbar FreeCAD would crash.
 */
bool StdCmdWorkbench::addTo(QWidget *w)
{
  if (w->inherits("QToolBar") && QString(w->name()) != QString("file operations"))
  {
#ifdef FC_DEBUG
    char szBuf[200];
    sprintf(szBuf, "Adding the command \"%s\" to this widget is not permitted!", getName());
    QMessageBox::information(getMainWindow(), "Warning", szBuf);
#else
    Base::Console().Log("Cannot add '%s' to this widget.\n", sName);
#endif
    return false;
  }

  // either add to the "file operations" toolbar or another widget type (e.g. popup menu)
  return Command::addTo(w);
}

//===========================================================================
// Std_MRU
//===========================================================================

/* TRANSLATOR Gui::StdCmdMRU */

StdCmdMRU::StdCmdMRU()
  :Command("Std_MRU"), pcAction(0), _nMaxItems(4)
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("Recent files");
  sToolTipText  = QT_TR_NOOP("Recent file list");
  sWhatsThis    = QT_TR_NOOP("Recent file list");
  sStatusTip    = QT_TR_NOOP("Recent file list");
  //	sPixmap			= "";
  iAccel        = 0;
}

/**
 * Opens the recent file at position \a iMsg in the menu.
 * If the file does not exist or cannot be loaded this item is removed
 * from the list.
 */
void StdCmdMRU::activated(int iMsg)
{
  if (iMsg < 0 || iMsg >= int(_vMRU.size()))
    return; // not in range

  QString f = _vMRU[iMsg];
  QFileInfo fi(f);
  if ( !fi.exists() || !fi.isFile())
  {
    removeRecentFile( f );
    // rebuild the recent file list
    dynamic_cast<MRUActionGroup*>(pcAction)->setRecentFiles(_vMRU);
  }
  else
  {
    getGuiApplication()->open( f.latin1() );
  }
}

/**
 * Creates the QAction object containing the recent files.
 */
QAction * StdCmdMRU::createAction(void)
{
  pcAction = new MRUActionGroup( this, getMainWindow(),sName, false );
  pcAction->setUsesDropDown( true );
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  // load recent file list
  StdCmdMRU::load();
  dynamic_cast<MRUActionGroup*>(pcAction)->setRecentFiles( _vMRU );

  return pcAction;
}

/**
 * Adds the recent file item with name \a item.
 */
void StdCmdMRU::addRecentFile ( const QString& item )
{
  if ( _vMRU.contains( item ) )
    removeRecentFile( item ); // already inserted

  if ( _nMaxItems > (int)_vMRU.size() )
  {
    _vMRU.prepend( item );
  }
  else if ( _nMaxItems > 0 )
  {
    _vMRU.remove ( _vMRU.last() );
    _vMRU.prepend( item );
  }
}

/**
 * Removes the recent file item with name \a item.
 */
void StdCmdMRU::removeRecentFile ( const QString& item )
{
  QStringList::Iterator it = _vMRU.find(item);
  if ( it != _vMRU.end() )
  {
    _vMRU.remove( it );
  }
}

/** Refreshes the recent file list. */
void StdCmdMRU::refreshRecentFileWidgets()
{
  MRUActionGroup* recfiles = dynamic_cast<MRUActionGroup*>(pcAction);
  if ( recfiles )
    recfiles->setRecentFiles( _vMRU );
}

/** 
 * Returns a list of the recent files.
 */
QStringList StdCmdMRU::recentFiles() const
{
  return _vMRU;
}

void StdCmdMRU::load()
{
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences");
  if (hGrp->HasGroup("RecentFiles"))
  {
    hGrp = hGrp->GetGroup("RecentFiles");
    StdCmdMRU* pCmd = dynamic_cast<StdCmdMRU*>(Application::Instance->commandManager().getCommandByName("Std_MRU"));
    if (pCmd)
    {
      int maxCnt = hGrp->GetInt("RecentFiles", 4);
      pCmd->setMaxCount( maxCnt );
      std::vector<std::string> MRU = hGrp->GetASCIIs("MRU");

      // append the items in reverse mode to prevent the order
      for (std::vector<std::string>::reverse_iterator it = MRU.rbegin(); it!=MRU.rend();++it)
      {
        pCmd->addRecentFile( it->c_str() );
      }
    }
  }
}

void StdCmdMRU::save()
{
  // save recent file list
  Command* pCmd = Application::Instance->commandManager().getCommandByName("Std_MRU");
  if (pCmd)
  {
    char szBuf[200];
    int i=0;
    ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("RecentFiles");
    hGrp->Clear();
    hGrp->SetInt("RecentFiles", ((StdCmdMRU*)pCmd)->maxCount());

    QStringList files = ((StdCmdMRU*)pCmd)->recentFiles();
    if ( files.size() > 0 )
    {
      for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it, i++ )
      {
        sprintf(szBuf, "MRU%d", i);
        hGrp->SetASCII(szBuf, (*it).latin1());
      }
    }
  }
}


//===========================================================================
// Std_About
//===========================================================================

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

QAction * StdCmdAbout::createAction(void)
{
  QAction *pcAction;

  QString exe = App::Application::Config()["ExeName"].c_str();
  pcAction = new Action(this,getMainWindow(),sName);
  pcAction->setText( QObject::tr(sMenuText).arg(exe) );
  pcAction->setMenuText( QObject::tr(sMenuText).arg(exe) );
  pcAction->setToolTip( QObject::tr(sToolTipText).arg(exe) );
  pcAction->setStatusTip( QObject::tr(sStatusTip).arg(exe) );
  pcAction->setWhatsThis( QObject::tr(sWhatsThis).arg(exe) );
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  return pcAction;
}

void StdCmdAbout::activated(int iMsg)
{
  AboutDialog dlg( getMainWindow() );
  dlg.exec();
}

void StdCmdAbout::languageChange()
{
  if ( _pcAction )
  {
    QString exe = App::Application::Config()["ExeName"].c_str();
    _pcAction->setText( QObject::tr(sMenuText).arg(exe) );
    _pcAction->setMenuText( QObject::tr(sMenuText).arg(exe) );
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
  sWhatsThis    = QT_TR_NOOP("Tip of the day");
  sStatusTip    = QT_TR_NOOP("Tip of the day");
  sHelpUrl      = "TipOfTheDay.html";
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
  Gui::Dialog::DlgParameterImp cDlg(getMainWindow(),"ParameterDialog",true);
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
  Gui::Dialog::DlgPreferencesImp cDlg(getMainWindow(),"Preferences Dialog",true);
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
  sPixmap       = "Std_MacroRecord";
  iAccel        = 0;
}

void StdCmdDlgMacroRecord::activated(int iMsg)
{
  Gui::Dialog::DlgMacroRecordImp cDlg(getMainWindow(),"ParameterDialog",true);
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
  sPixmap       = "Std_MacroPlay";
  iAccel        = 0;
}

void StdCmdDlgMacroExecute::activated(int iMsg)
{
  Gui::Dialog::DlgMacroExecuteImp cDlg(getMainWindow(),"Macro Execute",true);
  cDlg.exec();
}

bool StdCmdDlgMacroExecute::isActive(void)
{
  return ! (getGuiApplication()->macroManager()->isOpen());
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
  sPixmap       = "Std_MacroStop";
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
  Gui::Dialog::DlgCustomizeImp cDlg(getMainWindow(),"CustomizeDialog",true);
  cDlg.exec();
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
  sPixmap       = "CommandLine";
  iAccel        = 0;
}

void StdCmdCommandLine::activated(int iMsg)
{
  bool show = getMainWindow()->isMaximized ();
  ConsoleMsgFlags ret = Base::Console().SetEnabledMsgType("MessageBox",ConsoleMsgType::MsgType_Wrn|
                                                                       ConsoleMsgType::MsgType_Err, false);

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
  Base::Console().SetEnabledMsgType("MessageBox", ret, true);
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
  rcCmdMgr.addCommand(new StdCmdMacroStop());
  rcCmdMgr.addCommand(new StdCmdDlgCustomize());
  rcCmdMgr.addCommand(new StdCmdCommandLine());
  rcCmdMgr.addCommand(new StdCmdWorkbench());
  rcCmdMgr.addCommand(new StdCmdMRU());
  rcCmdMgr.addCommand(new StdCmdWhatsThis());
  rcCmdMgr.addCommand(new StdCmdOnlineHelp());
  rcCmdMgr.addCommand(new StdCmdTipOfTheDay());
  rcCmdMgr.addCommand(new StdCmdDescription());
}

} // namespace Gui
