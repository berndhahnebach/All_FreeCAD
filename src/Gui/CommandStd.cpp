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
#include "WhatsThis.h"
#include "DlgUndoRedo.h"
#include "BitmapFactory.h"
#include "View.h"

#include "DlgDocTemplatesImp.h"
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
  :CppCommand("Std_Open")
{
  // seting the
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
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
  

  QStringList FileList = QFileDialog::getOpenFileNames( EndingList.c_str(),QString::null, getAppWnd() );

  for ( QStringList::Iterator it = FileList.begin(); it != FileList.end(); ++it ) {
     getAppWnd()->open((*it).latin1());
  }
}

//===========================================================================
// Std_New
//===========================================================================

DEF_STD_CMD(StdCmdNew);

StdCmdNew::StdCmdNew()
  :CppCommand("Std_New")
{
  // seting the 
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&New");
  sToolTipText  = QT_TR_NOOP("Create a new empty Document");
  sWhatsThis    = QT_TR_NOOP("Create a new empty Document");
  sStatusTip    = QT_TR_NOOP("Create a new empty Document");
  sPixmap       = "New";
  iAccel        = Qt::CTRL+Qt::Key_N;
}

void StdCmdNew::activated(int iMsg)
{
//  DlgDocTemplatesImp cDlg(this,getAppWnd(),"Template Dialog",true);
//  cDlg.exec();

  doCommand(Command::Gui,"App.New()");
}

//===========================================================================
// Std_Save
//===========================================================================
DEF_STD_CMD_A(StdCmdSave);

StdCmdSave::StdCmdSave()
  :CppCommand("Std_Save")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Save");
  sToolTipText  = QT_TR_NOOP("Save the active document");
  sWhatsThis    = QT_TR_NOOP("Save the active document");
  sStatusTip    = QT_TR_NOOP("Save the active document");
  sPixmap       = "Save";
  iAccel        = Qt::CTRL+Qt::Key_S;
}

void StdCmdSave::activated(int iMsg)
{
  if( getActiveDocument() )
//    DoCommand(Command::Doc,"FreeCAD.DocGetActive().Save()");		
    getActiveDocument()->save();
  else
    doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Save\")");
//    getAppWnd()->SendMsgToActiveView("Save");
}

bool StdCmdSave::isActive(void)
{
  if( getActiveDocument() )
    return true;
  else
    return getAppWnd()->sendHasMsgToActiveView("Save");
}

//===========================================================================
// Std_SaveAs
//===========================================================================
DEF_STD_CMD_A(StdCmdSaveAs);

StdCmdSaveAs::StdCmdSaveAs()
  :CppCommand("Std_SaveAs")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Save &As...");
  sToolTipText  = QT_TR_NOOP("Save the active document under a new file name");
  sWhatsThis    = QT_TR_NOOP("Save the active document under a new file name");
  sStatusTip    = QT_TR_NOOP("Save the active document under a new file name");
  iAccel        = 0;
}

void StdCmdSaveAs::activated(int iMsg)
{
  if( getActiveDocument() )
    getActiveDocument()->saveAs();
  else
    doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SaveAs\")");
}

bool StdCmdSaveAs::isActive(void)
{
  if( getActiveDocument() )
    return true;
  else
    return getAppWnd()->sendHasMsgToActiveView("SaveAs");
}

//===========================================================================
// Std_Print
//===========================================================================
DEF_STD_CMD_A(StdCmdPrint );

StdCmdPrint::StdCmdPrint()
  :CppCommand("Std_Print")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Print...");
  sToolTipText  = QT_TR_NOOP("Print the window");
  sWhatsThis    = QT_TR_NOOP("Print the window");
  sStatusTip    = QT_TR_NOOP("Print the window");
  sPixmap       = "Print";
  iAccel        = Qt::CTRL+Qt::Key_P;;
}

void StdCmdPrint::activated(int iMsg)
{
  if ( getAppWnd()->activeView() )
  {
    getAppWnd()->statusBar()->message("Printing...");
    QPrinter printer( QPrinter::HighResolution );
    getAppWnd()->activeView()->print( &printer );
  }
}

bool StdCmdPrint::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Print");
}

//===========================================================================
// Std_Quit
//===========================================================================

DEF_STD_CMD(StdCmdQuit );

StdCmdQuit::StdCmdQuit()
  :CppCommand("Std_Quit")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("E&xit");
  sToolTipText  = QT_TR_NOOP("Quits the application");
  sWhatsThis    = QT_TR_NOOP("Quits the application");
  sStatusTip    = QT_TR_NOOP("Quits the application");
  iAccel        = Qt::ALT+Qt::Key_F4;
}

void StdCmdQuit::activated(int iMsg)
{
  ApplicationWindow::Instance->close();
}

//===========================================================================
// Std_Undo
//===========================================================================

DEF_STD_CMD_AC(StdCmdUndo);

StdCmdUndo::StdCmdUndo()
  :CppCommand("Std_Undo")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Undo");
  sToolTipText  = QT_TR_NOOP("Undo exactly one action");
  sWhatsThis    = QT_TR_NOOP("Undo exactly one action");
  sStatusTip    = QT_TR_NOOP("Undo exactly one action");
  sPixmap       = "Undo";
  iAccel        = Qt::CTRL+Qt::Key_Z;
}

void StdCmdUndo::activated(int iMsg)
{
//  ApplicationWindow::Instance->slotUndo();
  getAppWnd()->sendMsgToActiveView("Undo");
}

bool StdCmdUndo::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Undo");
}

QAction * StdCmdUndo::createAction(void)
{
  QAction *pcAction;

  pcAction = new UndoAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
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
  :CppCommand("Std_Redo")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Redo");
  sToolTipText  = QT_TR_NOOP("Redoes a previously undid action");
  sWhatsThis    = QT_TR_NOOP("Redoes a previously undid action");
  sStatusTip    = QT_TR_NOOP("Redoes a previously undid action");
  sPixmap       = "Redo";
  iAccel        = Qt::CTRL+Qt::Key_Y;
}

void StdCmdRedo::activated(int iMsg)
{
//  ApplicationWindow::Instance->slotRedo();
  getAppWnd()->sendMsgToActiveView("Redo");
}

bool StdCmdRedo::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Redo");
}

QAction * StdCmdRedo::createAction(void)
{
  QAction *pcAction;

  pcAction = new RedoAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
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

StdCmdWorkbench::StdCmdWorkbench()
  :CppCommand("Std_Workbench"), pcAction(NULL)
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
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
  QStringList wb = ApplicationWindow::Instance->workbenches();
  if (pos >= 0 && pos < int(wb.size()))
    activate( wb[pos] );
}

/**
 * Activates the workbench with the name \a item.
 */
void StdCmdWorkbench::activate ( const QString& item )
{
  try{
    doCommand(Gui, "Gui.WorkbenchActivate(\"%s\")", item.latin1());
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
  pcAction = new WorkbenchGroup( ApplicationWindow::Instance, sName.c_str(), true );
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
 
  // sort the workbenches
  QStringList items = ApplicationWindow::Instance->workbenches();
  items.sort();
  for (QStringList::Iterator it = items.begin(); it!=items.end(); ++it)
    appendItem( *it );

  return pcAction;
}

/**
 * Append a new workbench with \a item to the list of workbenches.
 */
void StdCmdWorkbench::appendItem ( const QString& item )
{
  if ( pcAction )
  {
    QAction* action = new WorkbenchAction( this, pcAction, item );
    action->setText(QObject::tr(item));
    action->setMenuText(QObject::tr(item));
    action->setToggleAction( true );
    action->setIconSet(Gui::BitmapFactory().pixmap("FCIcon"));
    pcAction->add( action );
  }
}

/** 
 * Can only be added to the "standard file" toolbar. This is because this command changes the workbenches
 * and so there will several toolbars/cmdbars be deleted. If the corresponding combobox were
 * inside such a toolbar/cmdbar FreeCAD would crash.
 */
bool StdCmdWorkbench::addTo(QWidget *w)
{
  if (!w->inherits("QToolBar") || QString(w->name()) != QString("file operations"))
  {
    char szBuf[200];
    sprintf(szBuf, "Adding the command \"%s\" to this widget is not permitted!", getName());
    QMessageBox::information(ApplicationWindow::Instance, "Warning", szBuf);
    return false;
  }

  return Command::addTo(w);
}

//===========================================================================
// Std_MRU
//===========================================================================

StdCmdMRU::StdCmdMRU()
  :CppCommand("Std_MRU"), pcAction(0), _nMaxItems(4)
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
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
    getAppWnd()->open( f.latin1() );
  }
}

/**
 * Creates the QAction object containing the recent files.
 */
QAction * StdCmdMRU::createAction(void)
{
  pcAction = new MRUActionGroup( this, ApplicationWindow::Instance,sName.c_str(), false );
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
  FCParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences");
  if (hGrp->HasGroup("RecentFiles"))
  {
    hGrp = hGrp->GetGroup("RecentFiles");
    StdCmdMRU* pCmd = dynamic_cast<StdCmdMRU*>(ApplicationWindow::Instance->commandManager().getCommandByName("Std_MRU"));
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
  Command* pCmd = ApplicationWindow::Instance->commandManager().getCommandByName("Std_MRU");
  if (pCmd)
  {
    char szBuf[200];
    int i=0;
    FCParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("RecentFiles");
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
  :CppCommand("Std_Cut")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Cut");
  sToolTipText  = QT_TR_NOOP("Cut out");
  sWhatsThis    = QT_TR_NOOP("Cut out");
  sStatusTip    = QT_TR_NOOP("Cut out");
  sPixmap       = "Cut";
  iAccel        = Qt::CTRL+Qt::Key_X;
}

void StdCmdCut::activated(int iMsg)
{
  getAppWnd()->sendMsgToActiveView("Cut");
}

bool StdCmdCut::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Cut");
}

//===========================================================================
// Std_Copy
//===========================================================================
DEF_STD_CMD_A(StdCmdCopy);

StdCmdCopy::StdCmdCopy()
  :CppCommand("Std_Copy")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("C&opy");
  sToolTipText  = QT_TR_NOOP("Copy operation");
  sWhatsThis    = QT_TR_NOOP("Copy operation");
  sStatusTip    = QT_TR_NOOP("Copy operation");
  sPixmap       = "Copy";
  iAccel        = Qt::CTRL+Qt::Key_C;
}

void StdCmdCopy::activated(int iMsg)
{
  getAppWnd()->sendMsgToActiveView("Copy");
}

bool StdCmdCopy::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Copy");
}

//===========================================================================
// Std_Paste
//===========================================================================
DEF_STD_CMD_A(StdCmdPaste);

StdCmdPaste::StdCmdPaste()
  :CppCommand("Std_Paste")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Paste");
  sToolTipText  = QT_TR_NOOP("Paste operation");
  sWhatsThis    = QT_TR_NOOP("Paste operation");
  sStatusTip    = QT_TR_NOOP("Paste operation");
  sPixmap       = "Paste";
  iAccel        = Qt::CTRL+Qt::Key_V;
}

void StdCmdPaste::activated(int iMsg)
{
  getAppWnd()->sendMsgToActiveView("Paste");
}

bool StdCmdPaste::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Paste");
}

//===========================================================================
// Std_About
//===========================================================================
DEF_STD_CMD(StdCmdAbout);

StdCmdAbout::StdCmdAbout()
  :CppCommand("Std_About")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&About FreeCAD");
  sToolTipText  = QT_TR_NOOP("About FreeCAD");
  sWhatsThis    = QT_TR_NOOP("About FreeCAD");
  sStatusTip    = QT_TR_NOOP("About FreeCAD");
  sPixmap       = "FCIcon";
}

void StdCmdAbout::activated(int iMsg)
{
  AboutDialog dlg( getAppWnd() );
  dlg.exec();
}

//===========================================================================
// Std_AboutQt
//===========================================================================
DEF_STD_CMD(StdCmdAboutQt);

StdCmdAboutQt::StdCmdAboutQt()
  :CppCommand("Std_AboutQt")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
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
  :CppCommand("Std_TipOfTheDay")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Tip of the day...");
  sToolTipText  = QT_TR_NOOP("Tip of the day");
  sWhatsThis    = QT_TR_NOOP("Tip of the day");
  sStatusTip    = QT_TR_NOOP("Tip of the day");
  sHelpUrl      = "TipOfTheDay.html";
}

void StdCmdTipOfTheDay::activated(int iMsg)
{
  getAppWnd()->showTipOfTheDay( true );
}

//===========================================================================
// Std_WhatsThis
//===========================================================================
DEF_STD_CMD(StdCmdWhatsThis);

StdCmdWhatsThis::StdCmdWhatsThis()
  :CppCommand("Std_WhatsThis")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
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
  :CppCommand("Std_DlgParameter")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("E&dit parameters ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog to edit the parameters");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog to edit the parameters");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog to edit the parameters");
  //sPixmap     = "settings";
  iAccel        = 0;
}

void StdCmdDlgParameter::activated(int iMsg)
{
  Gui::Dialog::DlgParameterImp cDlg(getAppWnd(),"ParameterDialog",true);
  cDlg.exec();
}

//===========================================================================
// Std_DlgPreferences
//===========================================================================
DEF_STD_CMD(StdCmdDlgPreferences);

StdCmdDlgPreferences::StdCmdDlgPreferences()
  :CppCommand("Std_DlgPreferences")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Preferences ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog to edit the preferences");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog to edit the preferences");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog to edit the preferences");
  //sPixmap     = "settings";
  iAccel        = 0;
}

void StdCmdDlgPreferences::activated(int iMsg)
{
  Gui::Dialog::DlgPreferencesImp cDlg(getAppWnd(),"Preferences Dialog",true);
  cDlg.exec();
}

//===========================================================================
// Std_DlgMacroRecord
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroRecord);

StdCmdDlgMacroRecord::StdCmdDlgMacroRecord()
  :CppCommand("Std_DlgMacroRecord")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Macro recording ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog to record a macro");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog to record a macro");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog to record a macro");
  sPixmap       = "Std_MacroRecord";
  iAccel        = 0;
}

void StdCmdDlgMacroRecord::activated(int iMsg)
{
  Gui::Dialog::DlgMacroRecordImp cDlg(getAppWnd(),"ParameterDialog",true);
  cDlg.exec();
}

bool StdCmdDlgMacroRecord::isActive(void)
{
  return ! (getAppWnd()->macroManager()->isOpen());
}

//===========================================================================
// Std_DlgMacroExecute
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroExecute);

StdCmdDlgMacroExecute::StdCmdDlgMacroExecute()
  :CppCommand("Std_DlgMacroExecute")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("E&xecute macro ...");
  sToolTipText  = QT_TR_NOOP("Opens a Dialog let you execute a recorded macro");
  sWhatsThis    = QT_TR_NOOP("Opens a Dialog let you execute a recorded macro");
  sStatusTip    = QT_TR_NOOP("Opens a Dialog let you execute a recorded macro");
  sPixmap       = "Std_MacroPlay";
  iAccel        = 0;
}

void StdCmdDlgMacroExecute::activated(int iMsg)
{
  Gui::Dialog::DlgMacroExecuteImp cDlg(getAppWnd(),"Macro Execute",true);
  cDlg.exec();
}

bool StdCmdDlgMacroExecute::isActive(void)
{
  return ! (getAppWnd()->macroManager()->isOpen());
}

//===========================================================================
// Std_MacroStop
//===========================================================================
DEF_STD_CMD_A(StdCmdMacroStop);

StdCmdMacroStop::StdCmdMacroStop()
  :CppCommand("Std_DlgMacroStop")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("S&top macro recording");
  sToolTipText  = QT_TR_NOOP("Stop the a running macro recording sassion");
  sWhatsThis    = QT_TR_NOOP("Stop the a running macro recording sassion");
  sStatusTip    = QT_TR_NOOP("Stop the a running macro recording sassion");
  sPixmap       = "Std_MacroStop";
  iAccel        = 0;
}

void StdCmdMacroStop::activated(int iMsg)
{
  getAppWnd()->macroManager()->commit();
}

bool StdCmdMacroStop::isActive(void)
{
  return getAppWnd()->macroManager()->isOpen();
}

//===========================================================================
// Std_DlgCustomize
//===========================================================================
DEF_STD_CMD(StdCmdDlgCustomize);

StdCmdDlgCustomize::StdCmdDlgCustomize()
  :CppCommand("Std_DlgCustomize")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Cu&stomize...");
  sToolTipText  = QT_TR_NOOP("Customize toolbars and commandbars");
  sWhatsThis    = QT_TR_NOOP("Customize toolbars and commandbars");
  sStatusTip    = QT_TR_NOOP("Customize toolbars and commandbars");
  //sPixmap     = "customize";
  iAccel        = 0;
}

void StdCmdDlgCustomize::activated(int iMsg)
{
  Gui::Dialog::DlgCustomizeImp cDlg(getAppWnd(),"CustomizeDialog",true);
  cDlg.exec();
}

//===========================================================================
// Std_CommandLine
//===========================================================================
DEF_STD_CMD(StdCmdCommandLine);

StdCmdCommandLine::StdCmdCommandLine()
  :CppCommand("Std_CommandLine")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Start command &line...");
  sToolTipText  = QT_TR_NOOP("Opens the command line in the console");
  sWhatsThis    = QT_TR_NOOP("Opens the command line in the console");
  sStatusTip    = QT_TR_NOOP("Opens the command line in the console");
  sPixmap       = "CommandLine";
  iAccel        = 0;
}

void StdCmdCommandLine::activated(int iMsg)
{
  bool show = getAppWnd()->isMaximized ();
  bool mute = GuiConsoleObserver::bMute;

  // pop up the Gui command window
  GUIConsole Wnd;

  getAppWnd()->showMinimized () ;
  qApp->processEvents();

  // create temporary console sequencer
  Base::ConsoleSequencer* seq = new Base::ConsoleSequencer;
  GuiConsoleObserver::bMute = true;
  Base::Interpreter().runCommandLine("Console mode");
  GuiConsoleObserver::bMute = mute;
  delete seq;

#ifdef Q_WS_X11
  // On X11 this may not work. For further information see QWidget::showMaximized
  //
  // workaround for X11
  getAppWnd()->hide();
  getAppWnd()->show();
#endif

  // pop up the main window
  show ? getAppWnd()->showMaximized () : getAppWnd()->showNormal () ;
  qApp->processEvents();
}

//===========================================================================
// Std_OCAFBrowser
//===========================================================================
DEF_STD_CMD_A(StdCmdOCAFBrowse);

StdCmdOCAFBrowse::StdCmdOCAFBrowse()
  :CppCommand("Std_OCAFBrowser")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
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
  cBrowser.DFBrowser(getAppWnd()->activeDocument()->getDocument()->GetOCCDoc());
#else
  QMessageBox::information(getAppWnd(), "OCAFBrowser", "Because FreeCAD has been compiled without set the 'FC_USE_OCAFBROWSER' flag\n"
                                        "this feature is disabled.");
#endif
}

bool StdCmdOCAFBrowse::isActive(void)
{
  return getAppWnd()->activeDocument() != 0;
}

namespace Gui {

void CreateStdCommands(void)
{
  CommandManager &rcCmdMgr = ApplicationWindow::Instance->commandManager();

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
