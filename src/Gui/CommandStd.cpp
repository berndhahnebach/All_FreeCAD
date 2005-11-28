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

  pcAction = new UndoAction(this,getMainWindow(),sName,(_eType&Cmd_Toggle) != 0);
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

  pcAction = new RedoAction(this,getMainWindow(),sName,(_eType&Cmd_Toggle) != 0);
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
  pcAction = new Action(this,getMainWindow(),sName,(_eType&Cmd_Toggle) != 0);
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

//===========================================================================
// Std_OCAFBrowser
//===========================================================================
DEF_STD_CMD_A(StdCmdOCAFBrowse);

StdCmdOCAFBrowse::StdCmdOCAFBrowse()
  :Command("Std_OCAFBrowser")
{
  sGroup        = QT_TR_NOOP("Tools");
  sMenuText     = QT_TR_NOOP("Start &raw document browser...");
  sToolTipText  = QT_TR_NOOP("Starts a new window to deeply examine the document structure");
  sWhatsThis    = QT_TR_NOOP("Starts a new window to deeply examine the document structure");
  sStatusTip    = QT_TR_NOOP("Starts a new window to deeply examine the document structure");
  sPixmap       = "Copy";
  iAccel        = Qt::CTRL+Qt::Key_B;
}

void StdCmdOCAFBrowse::activated(int iMsg)
{
#ifdef FC_USE_OCAFBROWSER
# ifdef _MSC_VER
#   pragma warning(disable: 4101)
# endif

  DebugBrowser cBrowser;
  cBrowser.DFBrowser(getGuiApplication()->activeDocument()->getDocument()->GetOCCDoc());
#else
  QMessageBox::information(getMainWindow(), "OCAFBrowser", "Because FreeCAD has been compiled without set the 'FC_USE_OCAFBROWSER' flag\n"
                                            "this feature is disabled.");
#endif
}

bool StdCmdOCAFBrowse::isActive(void)
{
  return getGuiApplication()->activeDocument() != 0;
}

namespace Gui {

void CreateStdCommands(void)
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
  rcCmdMgr.addCommand(new StdCmdOCAFBrowse());
  rcCmdMgr.addCommand(new StdCmdTipOfTheDay());
  rcCmdMgr.addCommand(new StdCmdDescription());
}

} // namespace Gui
