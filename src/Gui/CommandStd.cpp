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
# include <iostream>
# include <stdio.h>
# include <assert.h>
# include <fcntl.h>
# include <ctype.h>
# include <typeinfo>
# include <vector>
# include <map>
# include <string>
# include <list>
# include <set>
# include <algorithm>
# include <stack>
# include <queue>
# include <string>
# include <limits.h>
# if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
# endif // (re-)defined in pyconfig.h
# include <Python.h>
# include <algorithm>
# include <qapplication.h>
# include <qaction.h>
# include <qcombobox.h>
# include <qcursor.h>
# include <qstatusbar.h>
# include <qthread.h>
# include <qfiledialog.h>
# include <qpainter.h>
# include <qprocess.h>
# include <qprinter.h>
# include <qmessagebox.h>
# include <qsplashscreen.h>
# include <qtimer.h>
# include <qtextbrowser.h>
# include <qvalidator.h>
# include <BRepPrimAPI_MakeBox.hxx>
# include <BRepTools.hxx>
# include <TopoDS_Shape.hxx>
# include <TNaming_Builder.hxx>
# include <TDataStd_Real.hxx>
# include <Handle_TPrsStd_AISPresentation.hxx>
# include <TNaming_NamedShape.hxx>
# include <TPrsStd_AISPresentation.hxx>
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
#include "DlgActivateWindowImp.h"
#include "Widgets.h"

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
  sGroup        = "Standard";
  sMenuText     = "Open";
  sToolTipText  = "Open a Document or import Files";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Open";
  iAccel        = Qt::CTRL+Qt::Key_O;
}

void StdCmdOpen::activated(int iMsg)
{
  getAppWnd()->statusBar()->message(QObject::tr("Opening file..."));

  QString f = QFileDialog::getOpenFileName( QString::null, "FreeCAD Standard (*.FCStd);;OpenCasCade (*.std)", getAppWnd() );
  if ( !f.isEmpty() ) {
    // the user selected a valid existing file
//    GetApplication().Open(f.latin1());
    getAppWnd()->AppendRecentFile(f.latin1());
  } else {
    // the user cancelled the dialog
    getAppWnd()->statusBar()->message(QObject::tr("Opening aborted"));
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
  sGroup        = "Standard";
  sMenuText     = "New";
  sToolTipText  = "Create a new empty Document";
  sWhatsThis    = "Create a new empty Document";
  sStatusTip    = "Create a new empty Document";
  sPixmap       = "New";
  iAccel        = Qt::CTRL+Qt::Key_N;
}

void StdCmdNew::activated(int iMsg)
{
//  DlgDocTemplatesImp cDlg(this,getAppWnd(),"Template Dialog",true);
//  cDlg.exec();

  doCommand(Command::Gui,"App.DocNew()");
}

//===========================================================================
// Std_Save
//===========================================================================
DEF_STD_CMD_A(StdCmdSave);

StdCmdSave::StdCmdSave()
  :CppCommand("Std_Save")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Save";
  sToolTipText  = "Save the active document";
  sWhatsThis    = "Save the active document";
  sStatusTip    = "Save the active document";
  sPixmap       = "Save";
  iAccel        = Qt::CTRL+Qt::Key_S;
}

void StdCmdSave::activated(int iMsg)
{
  if( getActiveDocument() )
//    DoCommand(Command::Doc,"FreeCAD.DocGetActive().Save()");		
    getActiveDocument()->Save();
  else
    doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Save\")");
//    getAppWnd()->SendMsgToActiveView("Save");
}

bool StdCmdSave::isActive(void)
{
  if( getActiveDocument() )
    return true;
  else
    return getAppWnd()->SendHasMsgToActiveView("Save");
}

//===========================================================================
// Std_SaveAs
//===========================================================================
DEF_STD_CMD_A(StdCmdSaveAs);

StdCmdSaveAs::StdCmdSaveAs()
  :CppCommand("Std_SaveAs")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Save as...";
  sToolTipText  = "Save the active document under a new file name";
  sWhatsThis    = "Save the active document under a new file name";
  sStatusTip    = "Save the active document under a new file name";
  iAccel        = 0;
}

void StdCmdSaveAs::activated(int iMsg)
{
  if( getActiveDocument() )
    getActiveDocument()->SaveAs();
  else
    doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SaveAs\")");
}

bool StdCmdSaveAs::isActive(void)
{
  if( getActiveDocument() )
    return true;
  else
    return getAppWnd()->SendHasMsgToActiveView("SaveAs");
}

//===========================================================================
// Std_Print
//===========================================================================
DEF_STD_CMD_A(StdCmdPrint );

StdCmdPrint::StdCmdPrint()
  :CppCommand("Std_Print")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Print...";
  sToolTipText  = "Print the window";
  sWhatsThis    = "Print the window";
  sStatusTip    = "Print the window";
  sPixmap       = "Print";
  iAccel        = Qt::CTRL+Qt::Key_P;;
}

void StdCmdPrint::activated(int iMsg)
{
  if ( getAppWnd()->GetActiveView() )
  {
    getAppWnd()->statusBar()->message("Printing...");
    QPrinter printer( QPrinter::HighResolution );
    getAppWnd()->GetActiveView()->Print( &printer );
  }
}

bool StdCmdPrint::isActive(void)
{
  return getAppWnd()->SendHasMsgToActiveView("Print");
}

//===========================================================================
// Std_Quit
//===========================================================================

DEF_STD_CMD(StdCmdQuit );

StdCmdQuit::StdCmdQuit()
  :CppCommand("Std_Quit")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Exit";
  sToolTipText  = "Quits the application";
  sWhatsThis    = "Quits the application";
  sStatusTip    = "Quits the application";
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
  sGroup        = "Standard";
  sMenuText     = "Undo";
  sToolTipText  = "Undo exactly one action";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Undo";
  iAccel        = Qt::CTRL+Qt::Key_Z;
}

void StdCmdUndo::activated(int iMsg)
{
//  ApplicationWindow::Instance->slotUndo();
  getAppWnd()->SendMsgToActiveView("Undo");
}

bool StdCmdUndo::isActive(void)
{
  return getAppWnd()->SendHasMsgToActiveView("Undo");
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
  sGroup        = "Standard";
  sMenuText     = "Redo";
  sToolTipText  = "Redoes a previously undid action";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Redo";
  iAccel        = Qt::CTRL+Qt::Key_Y;
}

void StdCmdRedo::activated(int iMsg)
{
//  ApplicationWindow::Instance->slotRedo();
  getAppWnd()->SendMsgToActiveView("Redo");
}

bool StdCmdRedo::isActive(void)
{
  return getAppWnd()->SendHasMsgToActiveView("Redo");
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
  sGroup        = "Standard";
  sMenuText     = "Workbench";
  sToolTipText  = "Switch between workbenches";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "FCIcon";
  iAccel        = 0;
}

/**
 * Activates the workbench at the position \a iMsg.
 */
void StdCmdWorkbench::activated(int iMsg)
{
  std::vector<std::string> wb = ApplicationWindow::Instance->GetWorkbenches();
  if (iMsg >= 0 && iMsg < int(wb.size()))
  {
    doCommand(Gui, "Gui.WorkbenchActivate(\"%s\")", wb[iMsg].c_str());
  }
}

/**
 * Activates the workbench with the name \a item.
 */
void StdCmdWorkbench::activate ( const QString& item )
{
  if (!pcAction) 
    createAction();
  dynamic_cast<ActionGroup*>(pcAction)->activate( item );
}

/**
 * Creates the QAction object containing all workbenches.
 */
QAction * StdCmdWorkbench::createAction(void)
{
  pcAction = new ActionGroup( this, ApplicationWindow::Instance, sName.c_str(), true );
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
 
  std::vector<std::string> items = ApplicationWindow::Instance->GetWorkbenches();
  for (std::vector<std::string>::iterator it = items.begin(); it!=items.end(); ++it)
    appendItem(it->c_str());

  return pcAction;
}

/**
 * Append a new workbench with \a item to the list of workbenches.
 */
void StdCmdWorkbench::appendItem ( const QString& item )
{
  if ( pcAction )
  {
    QAction* action = new QAction( pcAction, item );
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
  sGroup        = "Standard";
  sMenuText     = "Recent files";
  sToolTipText  = "Recent file list";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  if (iMsg >= 0 && iMsg < int(_vMRU.size()))
  {
    try{
      doCommand(Gui, "App.DocOpen(\"%s\")", _vMRU[iMsg].latin1());
    }catch(const Base::Exception&){
      removeRecentFile( _vMRU[iMsg] );
    }
  }
}

/**
 * Creates the QAction object containing the recent files.
 */
QAction * StdCmdMRU::createAction(void)
{
  pcAction = new ActionGroup(this,ApplicationWindow::Instance,sName.c_str(), false );
  pcAction->setUsesDropDown( true );
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  addRecentFile( "Test 123" );
  addRecentFile( "Test 231" );
  addRecentFile( "Test 231" );
  addRecentFile( "Test 132" );
  addRecentFile( "Test 321" );
  addRecentFile( "Test 213" );

  return pcAction;
}

/**
 * Adds the recent file item with name \a item.
 */
void StdCmdMRU::addRecentFile ( const QString& item )
{
  if ( _vMRU.contains( item ) )
    return; // already inserted

  if ( _nMaxItems > (int)_vMRU.size() )
  {
    _vMRU.prepend( item );
  }
  else
  {
    _vMRU.remove ( _vMRU.last() );
    _vMRU.prepend( item );
  }

  refresh();
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
    refresh();
  }
}

/**
 * Refreshes the list of the known recent files.
 */
void StdCmdMRU::refresh()
{
  if ( pcAction )
  {
    dynamic_cast<ActionGroup*>(pcAction)->clear();

    for ( QStringList::Iterator it = _vMRU.begin(); it != _vMRU.end(); ++it )
    {
      QAction* action = new QAction( pcAction, *it );
      action->setText(QObject::tr( *it ));
      QString name = recentFileItem( *it );
      action->setMenuText(QObject::tr(name));
      dynamic_cast<ActionGroup*>(pcAction)->addAction( action );
    }
  }
}

/** 
 * Creates the text of an item for the recent files menu.
 */
QString StdCmdMRU::recentFileItem( const QString& fn )
{
  int ct = _vMRU.findIndex( fn ) + 1;

  QString file(fn);

  int npos = file.findRev('/');
  if ( npos != -1 )
  {
    QString fn = file.right(file.length()-npos-1);
    QString path = file.left(npos);
    QString cur  = QDir::currentDirPath();
    if (path != cur)
      fn = file;
    fn.prepend(QString("&%1 ").arg(ct));
    return fn;
  }

  file.prepend(QString("&%1 ").arg(ct));
  return file;
}

/** 
 * Returns a list of the recent files.
 */
QStringList StdCmdMRU::recentFiles() const
{
  return _vMRU;
}

//===========================================================================
// Std_Cut
//===========================================================================
DEF_STD_CMD_A(StdCmdCut);

StdCmdCut::StdCmdCut()
  :CppCommand("Std_Cut")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Cut";
  sToolTipText  = "Cut out";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Cut";
  iAccel        = Qt::CTRL+Qt::Key_X;
}

void StdCmdCut::activated(int iMsg)
{
  getAppWnd()->SendMsgToActiveView("Cut");
}

bool StdCmdCut::isActive(void)
{
  return getAppWnd()->SendHasMsgToActiveView("Cut");
}

//===========================================================================
// Std_Copy
//===========================================================================
DEF_STD_CMD_A(StdCmdCopy);

StdCmdCopy::StdCmdCopy()
  :CppCommand("Std_Copy")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Copy";
  sToolTipText  = "Copy operation";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Copy";
  iAccel        = Qt::CTRL+Qt::Key_C;
}

void StdCmdCopy::activated(int iMsg)
{
  getAppWnd()->SendMsgToActiveView("Copy");
}

bool StdCmdCopy::isActive(void)
{
  return getAppWnd()->SendHasMsgToActiveView("Copy");
}

//===========================================================================
// Std_Paste
//===========================================================================
DEF_STD_CMD_A(StdCmdPaste);

StdCmdPaste::StdCmdPaste()
  :CppCommand("Std_Paste")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Paste";
  sToolTipText  = "Paste operation";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Paste";
  iAccel        = Qt::CTRL+Qt::Key_V;
}

void StdCmdPaste::activated(int iMsg)
{
  getAppWnd()->SendMsgToActiveView("Paste");
}

bool StdCmdPaste::isActive(void)
{
  return getAppWnd()->SendHasMsgToActiveView("Paste");
}

//===========================================================================
// Std_About
//===========================================================================
DEF_STD_CMD(StdCmdAbout);

StdCmdAbout::StdCmdAbout()
  :CppCommand("Std_About")
{
  sAppModule  = "";
  sGroup      = "Standard";
  sMenuText   = "About FreeCAD";
  sToolTipText  = "About FreeCAD";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  sGroup        = "Standard";
  sMenuText     = "About &Qt";
  sToolTipText  = "About Qt";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  sGroup        = "Standard";
  sMenuText     = "Tip of the day...";
  sToolTipText  = "Tip of the day";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
}

void StdCmdTipOfTheDay::activated(int iMsg)
{
  getAppWnd()->ShowTipOfTheDay( true );
}

//===========================================================================
// Std_WhatsThis
//===========================================================================
DEF_STD_CMD(StdCmdWhatsThis);

StdCmdWhatsThis::StdCmdWhatsThis()
  :CppCommand("Std_WhatsThis")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "What's This?";
  sToolTipText  = "What's This?";
  //	sWhatsThis		= sToolTipText;
  sWhatsThis    = "";
  sStatusTip    = sToolTipText;
  iAccel        = Qt::SHIFT+Qt::Key_F1;
  sPixmap       = "WhatsThis";
}

void StdCmdWhatsThis::activated(int iMsg)
{
  QWhatsThis::enterWhatsThisMode();
}

//===========================================================================
// Std_OnlineHelp
//===========================================================================

class StdCmdOnlineHelp : public CppCommand, public Gui::Process::ObserverType
{
public:
  StdCmdOnlineHelp();
  virtual ~StdCmdOnlineHelp();
  virtual void activated(int iMsg);
  virtual bool isActive(void);
  virtual void OnChange (FCSubject<Gui::Process::MessageType> &rCaller,Gui::Process::MessageType rcReason);

private:
  Gui::Process* process;
  bool fail;
  int pages;
};

StdCmdOnlineHelp::StdCmdOnlineHelp()
  :CppCommand("Std_OnlineHelp")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Download online help";
  sToolTipText  = "Download FreeCAD's online help";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;

  // FreeCAD's online documentation needs 763 (progress bar) 
  // steps to download.
  // This value is for initialization of the progress bar
  pages = 763;

  process = new Gui::Process;
  process->Attach(this);
}

StdCmdOnlineHelp::~StdCmdOnlineHelp()
{
  process->Detach(this);
  delete process;
}

void StdCmdOnlineHelp::activated(int iMsg)
{
  // process is not running yet
  if (!process->isRunning())
  {
    FCParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
    hGrp = hGrp->GetGroup("Preferences")->GetGroup("OnlineHelp");
    std::string url = hGrp->GetASCII("DownloadURL", "http://free-cad.sourceforge.net/index.html");
    std::string prx = hGrp->GetASCII("ProxyText", "");
    bool bUseProxy  = hGrp->GetBool ("UseProxy", false);

    if (bUseProxy)
      process->setEnvironment("http_proxy", prx.c_str());
    else
      process->unsetEnvironment("http_proxy");

    process->clearArguments();
    process->setExecutable("wget");
    *process << "-r" << "-k" << "-E" << url.c_str();
    process->start();
    fail = false;
  }
  else // kill the process now
  {
    if (process->isRunning())
    {
      process->tryTerminate();
      QTimer::singleShot( 2000, process, SLOT( kill() ) );
    }
  }

  if (process->isRunning())
    getAction()->setMenuText(QObject::tr("Stop %1").arg(sMenuText));
  else
    getAction()->setMenuText(sMenuText);
}

bool StdCmdOnlineHelp::isActive(void)
{
  return true;
}

void StdCmdOnlineHelp::OnChange (FCSubject<Gui::Process::MessageType> &rCaller,Gui::Process::MessageType rcReason)
{
  if (&rCaller != process)
    return;

  switch (rcReason)
  {
    // 'started' signal
    case Gui::Process::processStarted:
    {
      Console().Message("Download started...\n");
      Sequencer().start("Download online help", 0);
    } break;

    // 'exited' signal
    case Gui::Process::processExited:
    {
      if (!fail)
      {
        Base::Console().Message("Download finished.\n");
        QMessageBox::information(ApplicationWindow::Instance, "Download Online help", "Download finished.");
      }

      Sequencer().stop();
    } break;

    // 'failed' signal
    case Gui::Process::processFailed:
    {
#ifdef FC_OS_WIN32
      QString msg = Gui::Process::systemWarning( GetLastError(), process->executable().latin1() );
      Console().Warning("%s\n", msg.latin1());
#endif
    } break;

    // 'killed' signal
    case Gui::Process::processKilled:
    {
      Console().Warning("Download was canceled.\n");
      Sequencer().stop();
    } break;

    // 'output' signal or 'error output' signal
    case Gui::Process::receivedStdout:
    case Gui::Process::receivedStderr:
    {
      try
      {
        Sequencer().next();
        QString msg = process->message();

        // search for an error message
        int pos;
        if ( (pos = msg.find("failed: ")) != -1 )
        {
          int pos2 = msg.find('.', pos);
          QString substr = msg.mid(pos+8, pos2-pos-8+1);
          fail = true;
          Sequencer().stop();
          Console().Error("%s\n", substr.latin1());

          if (process->isRunning())
          {
            process->tryTerminate();
            QTimer::singleShot( 2000, process, SLOT( kill() ) );
          }
        }
      } 
      catch (const Base::Exception&)
      {
        // if ESC is pressed simply ignore this exception
      }
    } break;

    // 'wroteStdin' signal
    case Gui::Process::wroteStdin:
      break;

    // 'launch' signal
    case Gui::Process::launchFinished:
      break;
  }

  // resets menu text
  if (!process->isRunning())
    getAction()->setMenuText(sMenuText);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//===========================================================================
// Std_TileHoricontal
//===========================================================================
DEF_STD_CMD_A(StdCmdTileHor);

StdCmdTileHor::StdCmdTileHor()
  :CppCommand("Std_TileHoricontal")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Tile &Horizontally";
  sToolTipText  = "Tile the windows horizontally";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_WindowTileHor";
  iAccel        = 0;
}

void StdCmdTileHor::activated(int iMsg)
{
  getAppWnd()->tileHorizontal ();
}

bool StdCmdTileHor::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_TileVertical
//===========================================================================
DEF_STD_CMD_A(StdCmdTileVer);

StdCmdTileVer::StdCmdTileVer()
  :CppCommand("Std_TileVertical")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "&Tile Vertically";
  sToolTipText  = "Tile the windows vertically";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_WindowTileVer";
  iAccel        = 0;
}

void StdCmdTileVer::activated(int iMsg)
{
  getAppWnd()->tile();
}

bool StdCmdTileVer::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_TilePragmatic
//===========================================================================
DEF_STD_CMD_A(StdCmdTilePra);

StdCmdTilePra::StdCmdTilePra()
  :CppCommand("Std_TilePragmatic")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "&Cascade";
  sToolTipText  = "Tile pragmatic";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_WindowCascade";
  iAccel        = 0;
}

void StdCmdTilePra::activated(int iMsg)
{
  getAppWnd()->cascade();
}

bool StdCmdTilePra::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_Windows
//===========================================================================
DEF_STD_CMD(StdCmdWindows);

StdCmdWindows::StdCmdWindows()
  :CppCommand("Std_Windows")
{
  sAppModule    = "";
  sGroup      = "Standard";
  sMenuText   = "&Windows...";
  sToolTipText  = "Windows list";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  //sPixmap     = "";
  iAccel        = 0;
}

void StdCmdWindows::activated(int iMsg)
{
  Gui::Dialog::DlgActivateWindowImp dlg( getAppWnd(), "Windows", true );
  dlg.exec();
}

/*
//===========================================================================
// Std_MDINormal
//===========================================================================
DEF_STD_CMD(StdCmdMDINormal);

StdCmdMDINormal::StdCmdMDINormal()
  :CppCommand("Std_MDINormal")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "MDI Normal";
  sToolTipText  = "Set the standard MDI mode";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Paste";
  iAccel        = 0;
}


void StdCmdMDINormal::Activated(int iMsg)
{
  getAppWnd()->switchToChildframeMode();
}
*/

//===========================================================================
// Std_MDIToplevel
//===========================================================================
DEF_STD_CMD(StdCmdMDIToplevel);

StdCmdMDIToplevel::StdCmdMDIToplevel()
  :CppCommand("Std_MDIToplevel",Cmd_Toggle)
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "MDI seperate windows";
  sToolTipText  = "Set the seperate window MDI mode";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "TopLevel";
  iAccel        = 0;
}

void StdCmdMDIToplevel::activated(int iMsg)
{
  if(iMsg){
    // switches Tab mode off 
    toggleCommand("Std_MDITabed",false);
//    getAppWnd()->switchToToplevelMode();
  }//else
    //getAppWnd()->finishToplevelMode();
//    getAppWnd()->switchToChildframeMode();
}

//===========================================================================
// Std_MDITabed
//===========================================================================
DEF_STD_CMD(StdCmdMDITabbed);

StdCmdMDITabbed::StdCmdMDITabbed()
  :CppCommand("Std_MDITabed",Cmd_Toggle)
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "MDI tabed";
  sToolTipText  = "Set the tabed MDI mode";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "TopLevel";
  iAccel        = 0;
}

void StdCmdMDITabbed::activated(int iMsg)
{
  if(iMsg){
    // switches Toplevel off 
    toggleCommand("Std_MDIToplevel",false);
//    getAppWnd()->switchToTabPageMode();
  }//else
//    getAppWnd()->switchToChildframeMode();
}


//===========================================================================
// Std_DlgParameter
//===========================================================================
DEF_STD_CMD(StdCmdDlgParameter);

StdCmdDlgParameter::StdCmdDlgParameter()
  :CppCommand("Std_DlgParameter")
{
  sAppModule  = "";
  sGroup      = "Standard";
  sMenuText   = "Edit parameters ...";
  sToolTipText  = "Opens a Dialog to edit the parameters";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  sAppModule  = "";
  sGroup      = "Standard";
  sMenuText   = "Preferences ...";
  sToolTipText  = "Opens a Dialog to edit the preferences";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  sAppModule  = "";
  sGroup      = "Standard";
  sMenuText   = "Macro recording ...";
  sToolTipText  = "Opens a Dialog to record a macro";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  return ! (getAppWnd()->GetMacroMngr()->isOpen());
}

//===========================================================================
// Std_DlgMacroExecute
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroExecute);

StdCmdDlgMacroExecute::StdCmdDlgMacroExecute()
  :CppCommand("Std_DlgMacroExecute")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Execute macro ...";
  sToolTipText  = "Opens a Dialog let you execute a redordet macro";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  return ! (getAppWnd()->GetMacroMngr()->isOpen());
}

//===========================================================================
// Std_MacroStop
//===========================================================================
DEF_STD_CMD_A(StdCmdMacroStop);

StdCmdMacroStop::StdCmdMacroStop()
  :CppCommand("Std_DlgMacroStop")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Stop macro recording";
  sToolTipText  = "Stop the a running macro recording sassion";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_MacroStop";
  iAccel        = 0;
}

void StdCmdMacroStop::activated(int iMsg)
{
  getAppWnd()->GetMacroMngr()->commit();
}

bool StdCmdMacroStop::isActive(void)
{
  return getAppWnd()->GetMacroMngr()->isOpen();
}

//===========================================================================
// Std_DlgCustomize
//===========================================================================
DEF_STD_CMD(StdCmdDlgCustomize);

StdCmdDlgCustomize::StdCmdDlgCustomize()
  :CppCommand("Std_DlgCustomize")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Customize...";
  sToolTipText  = "Customize toolbars and commandbars";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
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
  sGroup        = "Standard";
  sMenuText     = "Start command line...";
  sToolTipText  = "Opens the command line in the console";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "CommandLine";
  iAccel        = 0;
}

void StdCmdCommandLine::activated(int iMsg)
{
  bool show = getAppWnd()->isMaximized ();
  bool mute = GuiConsoleObserver::bMute;

  getAppWnd()->showMinimized () ;
  qApp->processEvents();

  GuiConsoleObserver::bMute = true;
  Base::Interpreter().RunCommandLine("Console mode");
  GuiConsoleObserver::bMute = mute;

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

class StdCmdOCAFBrowse : public CppCommand
{
public:
  StdCmdOCAFBrowse();
//~StdCmdOCAFBrowse();
  virtual void activated(int iMsg);
  virtual bool isActive(void);
private:
#ifdef FC_USE_OCAFBROWSER
  DebugBrowser cBrowser;
#endif
};

StdCmdOCAFBrowse::StdCmdOCAFBrowse()
  :CppCommand("Std_OCAFBrowser")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "Start raw document browser...";
  sToolTipText  = "Starts a new window to deeply examine the document structure";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Copy";
  iAccel        = Qt::CTRL+Qt::Key_B;
}

void StdCmdOCAFBrowse::activated(int iMsg)
{
#ifdef FC_USE_OCAFBROWSER
  cBrowser.DFBrowser(getAppWnd()->GetActiveDocument()->GetDocument()->GetOCCDoc());
#else
  QMessageBox::information(getAppWnd(), "OCAFBrowser", "Because FreeCAD has been compiled without set the 'FC_USE_OCAFBROWSER' flag\n"
                                        "this feature is disabled.");
#endif
}

bool StdCmdOCAFBrowse::isActive(void)
{
  return getAppWnd()->GetActiveDocument() != 0;
}

namespace Gui {

void CreateStdCommands(void)
{
  CommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

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
  //rcCmdMgr.addCommand(new StdCmdMDINormal());
  rcCmdMgr.addCommand(new StdCmdMDIToplevel());
  rcCmdMgr.addCommand(new StdCmdMDITabbed());
  rcCmdMgr.addCommand(new StdCmdTileHor());
  rcCmdMgr.addCommand(new StdCmdTileVer());
  rcCmdMgr.addCommand(new StdCmdTilePra());
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
  rcCmdMgr.addCommand(new StdCmdWindows());
}

} // namespace Gui
