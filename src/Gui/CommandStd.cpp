/** \file CommandStd.h
 *  \brief The implementation of the standrd Commands
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


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
#	include <iostream>
#	include <stdio.h>
#	include <assert.h>
#	include <fcntl.h>
#	include <ctype.h>
#	include <typeinfo>
#	include <vector>
#	include <map>
#	include <string>
#	include <list>
#	include <set>
#	include <algorithm>
#	include <stack>
#	include <queue>
#	include <string>
#	include <limits.h>
#	if defined (_POSIX_C_SOURCE)
#		undef  _POSIX_C_SOURCE
#	endif // (re-)defined in pyconfig.h
#	include <Python.h>
#	include <algorithm>
# include <qapplication.h>
#	include <qaction.h>
#	include <qcombobox.h>
#	include <qcursor.h>
#	include <qstatusbar.h>
#	include <qthread.h>
#	include <qfiledialog.h>
#	include <qpainter.h>
#	include <qprocess.h>
#	include <qprinter.h>
#	include <qmessagebox.h>
#	include <qsplashscreen.h>
#	include <qtimer.h>
#	include <qtextbrowser.h>
#	include <qvalidator.h>
#	include <BRepPrimAPI_MakeBox.hxx>
#	include <BRepTools.hxx>
#	include <TopoDS_Shape.hxx>
#	include <TNaming_Builder.hxx>
#	include <TDataStd_Real.hxx>
#	include <Handle_TPrsStd_AISPresentation.hxx>
#	include <TNaming_NamedShape.hxx>
#	include <TPrsStd_AISPresentation.hxx>
#endif

#include "../Base/Exception.h"
#include "../Base/Interpreter.h"
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

#ifdef FC_USE_OCAFBROWSER
#	include <DebugBrowser.hxx>
#endif




#include "DlgDocTemplatesImp.h"
#include "DlgParameterImp.h"
#include "DlgMacroExecuteImp.h"
#include "DlgMacroRecordImp.h"
#include "Macro.h"
#include "DlgPreferencesImp.h"
#include "DlgCustomizeImp.h"
#include "DlgSettingsImp.h"
#include "DlgActivateWindowImp.h"

using Base::Console;
using Base::Sequencer;
using namespace Gui;



//===========================================================================
// Std_Open
//===========================================================================

DEF_STD_CMD(FCCmdOpen);


FCCmdOpen::FCCmdOpen()
	:FCCppCommand("Std_Open")
{

	// seting the 
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Open";
	sToolTipText	= "Open a Document or import Files";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap         = "Open";
	iAccel      = Qt::CTRL+Qt::Key_O;

}


void FCCmdOpen::Activated(int iMsg)
{
	GetAppWnd()->statusBar()->message(QObject::tr("Opening file..."));

	QString f = QFileDialog::getOpenFileName( QString::null, "FreeCAD Standard (*.FCStd);;OpenCasCade (*.std)", GetAppWnd() );
	if ( !f.isEmpty() ) {
		// the user selected a valid existing file
//		GetApplication().Open(f.latin1());
    GetAppWnd()->AppendRecentFile(f.latin1());
	} else {
		// the user cancelled the dialog
		GetAppWnd()->statusBar()->message(QObject::tr("Opening aborted"));
	}
}

//===========================================================================
// Std_New
//===========================================================================

DEF_STD_CMD(FCCmdNew);

FCCmdNew::FCCmdNew()
	:FCCppCommand("Std_New")
{

	// seting the 
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "New";
	sToolTipText	= "Create a new empty Document";
	sWhatsThis		= "Create a new empty Document";
	sStatusTip		= "Create a new empty Document";
	sPixmap         = "New";
	iAccel      = Qt::CTRL+Qt::Key_N;

}


void FCCmdNew::Activated(int iMsg)
{
//  DlgDocTemplatesImp cDlg(this,GetAppWnd(),"Template Dialog",true);
//  cDlg.exec();

	DoCommand(FCCommand::Gui,"App.DocNew()");


}

//===========================================================================
// Std_Save
//===========================================================================
DEF_STD_CMD_A(FCCmdSave);

FCCmdSave::FCCmdSave()
	:FCCppCommand("Std_Save")
{

	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Save";
	sToolTipText	= "Save the active document";
	sWhatsThis		= "Save the active document";
	sStatusTip		= "Save the active document";
	sPixmap			= "Save";
	iAccel			= Qt::CTRL+Qt::Key_S;
}


void FCCmdSave::Activated(int iMsg)
{
	if( GetActiveDocument() )
//		DoCommand(FCCommand::Doc,"FreeCAD.DocGetActive().Save()");		
		GetActiveDocument()->Save();
	else
		DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"Save\")");
//		GetAppWnd()->SendMsgToActiveView("Save");
}

bool FCCmdSave::IsActive(void)
{
	if( GetActiveDocument() )
		return true;
	else
		return GetAppWnd()->SendHasMsgToActiveView("Save");

}

//===========================================================================
// Std_SaveAs
//===========================================================================
DEF_STD_CMD_A(FCCmdSaveAs);

FCCmdSaveAs::FCCmdSaveAs()
	:FCCppCommand("Std_SaveAs")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Save as...";
	sToolTipText	= "Save the active document under a new file name";
	sWhatsThis		= "Save the active document under a new file name";
	sStatusTip		= "Save the active document under a new file name";
	iAccel			= 0;
}


void FCCmdSaveAs::Activated(int iMsg)
{
	if( GetActiveDocument() )
  	GetActiveDocument()->SaveAs();
	else
		DoCommand(FCCommand::Gui,"FreeCADGui.SendMsgToActiveView(\"SaveAs\")");
}

bool FCCmdSaveAs::IsActive(void)
{
	if( GetActiveDocument() )
		return true;
	else
		return GetAppWnd()->SendHasMsgToActiveView("SaveAs");

}

//===========================================================================
// Std_Print
//===========================================================================
DEF_STD_CMD_A(FCCmdPrint );

FCCmdPrint::FCCmdPrint()
	:FCCppCommand("Std_Print")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Print...";
	sToolTipText	= "Print the window";
	sWhatsThis		= "Print the window";
	sStatusTip		= "Print the window";
	sPixmap			= "Print";
	iAccel			= Qt::CTRL+Qt::Key_P;;
}


void FCCmdPrint::Activated(int iMsg)
{
	GetAppWnd()->statusBar()->message("Printing...");
	QPrinter printer( QPrinter::HighResolution );
	GetAppWnd()->GetActiveView()->Print( &printer );
}

bool FCCmdPrint::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Print");
}

//===========================================================================
// Std_Quit
//===========================================================================

DEF_STD_CMD(FCCmdQuit );

FCCmdQuit::FCCmdQuit()
	:FCCppCommand("Std_Quit")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Exit";
	sToolTipText	= "Quits the application";
	sWhatsThis		= "Quits the application";
	sStatusTip		= "Quits the application";
	iAccel			= Qt::ALT+Qt::Key_F4;
}


void FCCmdQuit::Activated(int iMsg)
{
  ApplicationWindow::Instance->close();
}


//===========================================================================
// Std_Undo
//===========================================================================

DEF_STD_CMD_AC(FCCmdUndo );

FCCmdUndo::FCCmdUndo()
	:FCCppCommand("Std_Undo")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Undo";
	sToolTipText	= "Undo exactly one action";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Undo";
	iAccel			= Qt::CTRL+Qt::Key_Z;
}


void FCCmdUndo::Activated(int iMsg)
{
//  ApplicationWindow::Instance->slotUndo();
	GetAppWnd()->SendMsgToActiveView("Undo");
}

bool FCCmdUndo::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Undo");
}

QAction * FCCmdUndo::CreateAction(void)
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

DEF_STD_CMD_AC(FCCmdRedo );

FCCmdRedo::FCCmdRedo()
	:FCCppCommand("Std_Redo")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Redo";
	sToolTipText	= "Redoes a previously undid action";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Redo";
	iAccel			= Qt::CTRL+Qt::Key_Y;
}


void FCCmdRedo::Activated(int iMsg)
{
//  ApplicationWindow::Instance->slotRedo();
	GetAppWnd()->SendMsgToActiveView("Redo");
}

bool FCCmdRedo::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Redo");
}

QAction * FCCmdRedo::CreateAction(void)
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

FCCmdWorkbench::FCCmdWorkbench()
	:FCCppCommand("Std_Workbench"), pcAction(NULL)
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Workbench";
	sToolTipText	= "Switch between workbenches";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "FCIcon";
	iAccel			= 0;
}

// Todo
void FCCmdWorkbench::Activated(int iMsg)
{
  std::vector<std::string> wb = ApplicationWindow::Instance->GetWorkbenches();
  if (iMsg >= 0 && iMsg < int(wb.size()))
  {
    DoCommand(Gui, "Gui.WorkbenchActivate(\"%s\")", wb[iMsg].c_str());
  }
}

void FCCmdWorkbench::activate ( const QString& item )
{
  if (!pcAction) 
    CreateAction();
  dynamic_cast<ActionGroup*>(pcAction)->activate( item );
}

QAction * FCCmdWorkbench::CreateAction(void)
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

void FCCmdWorkbench::appendItem ( const QString& item )
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
 * Can only add to the "standard file" toolbar. This is because the command changes the workbenches
 * and so there will be several toolbars/cmdbars be deleted. If the corresponding combobox were
 * inside such a toolbar/cmdbar FreeCAD crashes.
 */
bool FCCmdWorkbench::addTo(QWidget *w)
{
  if (!w->inherits("QToolBar") || QString(w->name()) != QString("file operations"))
  {
    char szBuf[200];
    sprintf(szBuf, "Adding the command \"%s\" to this widget is not permitted!", GetName());
    QMessageBox::information(ApplicationWindow::Instance, "Warning", szBuf);
    return false;
  }

  return FCCommand::addTo(w);
}

//===========================================================================
// Std_MRU
//===========================================================================

FCCmdMRU::FCCmdMRU()
	:FCCppCommand("Std_MRU"), pcAction(0), _nMaxItems(4)
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Recent files";
	sToolTipText	= "Recent file list";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
//	sPixmap			= "";
	iAccel			= 0;
}

void FCCmdMRU::Activated(int iMsg)
{
  if (iMsg >= 0 && iMsg < int(_vMRU.size()))
  {
    try{
      DoCommand(Gui, "App.DocOpen(\"%s\")", _vMRU[iMsg].latin1());
    }catch(const Base::Exception&){
      removeRecentFile( _vMRU[iMsg] );
    }
  }
}

QAction * FCCmdMRU::CreateAction(void)
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

void FCCmdMRU::addRecentFile ( const QString& item )
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

void FCCmdMRU::removeRecentFile ( const QString& item )
{
  QStringList::Iterator it = _vMRU.find(item);
  if ( it != _vMRU.end() )
  {
    _vMRU.remove( it );
    refresh();
  }
}

void FCCmdMRU::refresh()
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

QString FCCmdMRU::recentFileItem( const QString& fn )
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

QStringList FCCmdMRU::recentFiles() const
{
  return _vMRU;
}

//===========================================================================
// Std_Cut
//===========================================================================
DEF_STD_CMD_A(FCCmdCut);

FCCmdCut::FCCmdCut()
	:FCCppCommand("Std_Cut")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Cut";
	sToolTipText	= "Cut out";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Cut";
	iAccel			= Qt::CTRL+Qt::Key_X;
}


void FCCmdCut::Activated(int iMsg)
{
	GetAppWnd()->SendMsgToActiveView("Cut");
}

bool FCCmdCut::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Cut");
}

//===========================================================================
// Std_Copy
//===========================================================================
DEF_STD_CMD_A(FCCmdCopy );

FCCmdCopy::FCCmdCopy()
	:FCCppCommand("Std_Copy")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Copy";
	sToolTipText	= "Copy operation";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Copy";
	iAccel			= Qt::CTRL+Qt::Key_C;
}


void FCCmdCopy::Activated(int iMsg)
{
	GetAppWnd()->SendMsgToActiveView("Copy");
}

bool FCCmdCopy::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Copy");
}

//===========================================================================
// Std_Paste
//===========================================================================
DEF_STD_CMD_A(FCCmdPaste);

FCCmdPaste::FCCmdPaste()
	:FCCppCommand("Std_Paste")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Paste";
	sToolTipText	= "Paste operation";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Paste";
	iAccel			= Qt::CTRL+Qt::Key_V;
}


void FCCmdPaste::Activated(int iMsg)
{
	GetAppWnd()->SendMsgToActiveView("Paste");
}
bool FCCmdPaste::IsActive(void)
{
	return GetAppWnd()->SendHasMsgToActiveView("Paste");
}

//===========================================================================
// Std_About
//===========================================================================
DEF_STD_CMD(FCCmdAbout);

FCCmdAbout::FCCmdAbout()
	:FCCppCommand("Std_About")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "About FreeCAD";
	sToolTipText	= "About FreeCAD";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
}

void FCCmdAbout::Activated(int iMsg)
{
	AboutDialog dlg( GetAppWnd() );
  dlg.exec();
}

//===========================================================================
// Std_AboutQt
//===========================================================================
DEF_STD_CMD(FCCmdAboutQt);

FCCmdAboutQt::FCCmdAboutQt()
	:FCCppCommand("Std_AboutQt")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "About &Qt";
	sToolTipText	= "About Qt";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
}

void FCCmdAboutQt::Activated(int iMsg)
{
  qApp->aboutQt();
}

//===========================================================================
// Std_TipOfTheDay
//===========================================================================
DEF_STD_CMD(FCCmdTipOfTheDay);

FCCmdTipOfTheDay::FCCmdTipOfTheDay()
	:FCCppCommand("Std_TipOfTheDay")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Tip of the day...";
	sToolTipText	= "Tip of the day";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
}

void FCCmdTipOfTheDay::Activated(int iMsg)
{
	GetAppWnd()->ShowTipOfTheDay( true );
}

//===========================================================================
// Std_WhatsThis
//===========================================================================
DEF_STD_CMD(FCCmdWhatsThis);

FCCmdWhatsThis::FCCmdWhatsThis()
	:FCCppCommand("Std_WhatsThis")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "What's This?";
	sToolTipText	= "What's This?";
//	sWhatsThis		= sToolTipText;
	sWhatsThis		= "";
	sStatusTip		= sToolTipText;
	iAccel			= Qt::SHIFT+Qt::Key_F1;
	sPixmap			= "WhatsThis";
}

void FCCmdWhatsThis::Activated(int iMsg)
{
  QWhatsThis::enterWhatsThisMode();
}

//===========================================================================
// Std_OnlineHelp
//===========================================================================

class FCCmdOnlineHelp : public FCCppCommand, public Gui::Process::ObserverType
{
public:
	FCCmdOnlineHelp();
  virtual ~FCCmdOnlineHelp();
	virtual void Activated(int iMsg);
	virtual bool IsActive(void);
  virtual void OnChange (FCSubject<Gui::Process::MessageType> &rCaller,Gui::Process::MessageType rcReason);

private:
  Gui::Process* process;
  bool fail;
  int pages;
};

FCCmdOnlineHelp::FCCmdOnlineHelp()
	:FCCppCommand("Std_OnlineHelp")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Download online help";
	sToolTipText	= "Download FreeCAD's online help";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;

  // FreeCAD's online documentation needs 763 (progress bar) 
  // steps to download.
  // This value is for initialization of the progress bar
  pages = 763;

  process = new Gui::Process;
  process->Attach(this);
}

FCCmdOnlineHelp::~FCCmdOnlineHelp()
{
  process->Detach(this);
  delete process;
}

void FCCmdOnlineHelp::Activated(int iMsg)
{
  // process is not running yet
  if (!process->isRunning())
  {
    FCParameterGrp::handle hGrp = GetApplication().GetUserParameter().GetGroup("BaseApp");
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
    GetAction()->setMenuText(QObject::tr("Stop %1").arg(sMenuText));
  else
    GetAction()->setMenuText(sMenuText);
}

bool FCCmdOnlineHelp::IsActive(void)
{
  return true;
}

void FCCmdOnlineHelp::OnChange (FCSubject<Gui::Process::MessageType> &rCaller,Gui::Process::MessageType rcReason)
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
    GetAction()->setMenuText(sMenuText);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//===========================================================================
// Std_TileHoricontal
//===========================================================================
DEF_STD_CMD_A(FCCmdTileHor);

FCCmdTileHor::FCCmdTileHor()
	:FCCppCommand("Std_TileHoricontal")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Tile &Horizontally";
	sToolTipText	= "Tile the windows horizontally";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_WindowTileHor";
	iAccel			= 0;
}


void FCCmdTileHor::Activated(int iMsg)
{
	GetAppWnd()->tileHorizontal ();
}

bool FCCmdTileHor::IsActive(void)
{
	return !( GetAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_TileVertical
//===========================================================================
DEF_STD_CMD_A(FCCmdTileVer);

FCCmdTileVer::FCCmdTileVer()
	:FCCppCommand("Std_TileVertical")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "&Tile Vertically";
	sToolTipText	= "Tile the windows vertically";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_WindowTileVer";
	iAccel			= 0;
}


void FCCmdTileVer::Activated(int iMsg)
{
	GetAppWnd()->tile();
}

bool FCCmdTileVer::IsActive(void)
{
	return !( GetAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_TilePragmatic
//===========================================================================
DEF_STD_CMD_A(FCCmdTilePra);

FCCmdTilePra::FCCmdTilePra()
	:FCCppCommand("Std_TilePragmatic")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "&Cascade";
	sToolTipText	= "Tile pragmatic";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_WindowCascade";
	iAccel			= 0;
}

void FCCmdTilePra::Activated(int iMsg)
{
	GetAppWnd()->cascade();
}

bool FCCmdTilePra::IsActive(void)
{
	return !( GetAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_Windows
//===========================================================================
DEF_STD_CMD(FCCmdWindows);

FCCmdWindows::FCCmdWindows()
	:FCCppCommand("Std_Windows")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "&Windows...";
	sToolTipText	= "Windows list";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
//	sPixmap			= "";
	iAccel			= 0;
}

void FCCmdWindows::Activated(int iMsg)
{
  Gui::Dialog::DlgActivateWindowImp dlg( GetAppWnd(), "Windows", true );
  dlg.exec();
}

/*
//===========================================================================
// Std_MDINormal
//===========================================================================
DEF_STD_CMD(FCCmdMDINormal);

FCCmdMDINormal::FCCmdMDINormal()
	:FCCppCommand("Std_MDINormal")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText	  = "MDI Normal";
	sToolTipText = "Set the standard MDI mode";
	sWhatsThis   = sToolTipText;
	sStatusTip   = sToolTipText;
	sPixmap      = "Paste";
	iAccel = 0;
}


void FCCmdMDINormal::Activated(int iMsg)
{
	GetAppWnd()->switchToChildframeMode();
}
*/

//===========================================================================
// Std_MDIToplevel
//===========================================================================
DEF_STD_CMD(FCCmdMDIToplevel);

FCCmdMDIToplevel::FCCmdMDIToplevel()
	:FCCppCommand("Std_MDIToplevel",Cmd_Toggle)
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "MDI seperate windows";
	sToolTipText	= "Set the seperate window MDI mode";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "TopLevel";
	iAccel			= 0;
}


void FCCmdMDIToplevel::Activated(int iMsg)
{
	if(iMsg){
		// switches Tab mode off 
		ToggleCommand("Std_MDITabed",false);
//		GetAppWnd()->switchToToplevelMode();
	}//else
		//GetAppWnd()->finishToplevelMode();
//		GetAppWnd()->switchToChildframeMode();
}

//===========================================================================
// Std_MDITabed
//===========================================================================
DEF_STD_CMD(FCCmdMDITabed);

FCCmdMDITabed::FCCmdMDITabed()
	:FCCppCommand("Std_MDITabed",Cmd_Toggle)
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "MDI tabed";
	sToolTipText	= "Set the tabed MDI mode";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "TopLevel";
	iAccel			= 0;
}


void FCCmdMDITabed::Activated(int iMsg)
{
	if(iMsg){
		// switches Toplevel off 
		ToggleCommand("Std_MDIToplevel",false);
//		GetAppWnd()->switchToTabPageMode();
	}//else
//		GetAppWnd()->switchToChildframeMode();
}



//===========================================================================
// Std_DlgParameter
//===========================================================================
DEF_STD_CMD(FCCmdDlgParameter);

FCCmdDlgParameter::FCCmdDlgParameter()
	:FCCppCommand("Std_DlgParameter")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Edit parameters ...";
	sToolTipText	= "Opens a Dialog to edit the parameters";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	//sPixmap			= "settings";
	iAccel			= 0;
}


void FCCmdDlgParameter::Activated(int iMsg)
{
	DlgParameter cDlg(GetAppWnd(),"ParameterDialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgPreferences
//===========================================================================
DEF_STD_CMD(FCCmdDlgPreferences);

FCCmdDlgPreferences::FCCmdDlgPreferences()
	:FCCppCommand("Std_DlgPreferences")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Preferences ...";
	sToolTipText	= "Opens a Dialog to edit the preferences";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	//sPixmap			= "settings";
	iAccel			= 0;
}


void FCCmdDlgPreferences::Activated(int iMsg)
{
	Gui::Dialog::DlgPreferencesImp cDlg(GetAppWnd(),"Preferences Dialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgMacroRecord
//===========================================================================
DEF_STD_CMD_A(FCCmdDlgMacroRecord);

FCCmdDlgMacroRecord::FCCmdDlgMacroRecord()
	:FCCppCommand("Std_DlgMacroRecord")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Macro recording ...";
	sToolTipText	= "Opens a Dialog to record a macro";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_MacroRecord";
	iAccel			= 0;
}


void FCCmdDlgMacroRecord::Activated(int iMsg)
{
	DlgMacroRecordImp cDlg(GetAppWnd(),"ParameterDialog",true);
	cDlg.exec();
}

bool FCCmdDlgMacroRecord::IsActive(void)
{
	return ! (GetAppWnd()->GetMacroMngr()->IsOpen());
}

//===========================================================================
// Std_DlgMacroExecute
//===========================================================================
DEF_STD_CMD_A(FCCmdDlgMacroExecute);

FCCmdDlgMacroExecute::FCCmdDlgMacroExecute()
	:FCCppCommand("Std_DlgMacroExecute")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Execute macro ...";
	sToolTipText	= "Opens a Dialog let you execute a redordet macro";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_MacroPlay";
	iAccel			= 0;
}


void FCCmdDlgMacroExecute::Activated(int iMsg)
{
	DlgMacroExecuteImp cDlg(GetAppWnd(),"Macro Execute",true);
	cDlg.exec();
}

bool FCCmdDlgMacroExecute::IsActive(void)
{
	return ! (GetAppWnd()->GetMacroMngr()->IsOpen());
}


//===========================================================================
// Std_MacroStop
//===========================================================================
DEF_STD_CMD_A(FCCmdMacroStop);

FCCmdMacroStop::FCCmdMacroStop()
	:FCCppCommand("Std_DlgMacroStop")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Stop macro recording";
	sToolTipText	= "Stop the a running macro recording sassion";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_MacroStop";
	iAccel			= 0;
}


void FCCmdMacroStop::Activated(int iMsg)
{
	GetAppWnd()->GetMacroMngr()->Commit();
}

bool FCCmdMacroStop::IsActive(void)
{
	return GetAppWnd()->GetMacroMngr()->IsOpen();
}


//===========================================================================
// Std_DlgCustomize
//===========================================================================
DEF_STD_CMD(FCCmdDlgCustomize);

FCCmdDlgCustomize::FCCmdDlgCustomize()
	:FCCppCommand("Std_DlgCustomize")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Customize...";
	sToolTipText	= "Customize toolbars and command bars";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	//sPixmap			= "customize";
	iAccel			= 0;
}


void FCCmdDlgCustomize::Activated(int iMsg)
{
	Gui::Dialog::DlgCustomizeImp cDlg(GetAppWnd(),"CustomizeDialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgSettings
//===========================================================================
DEF_STD_CMD(FCCmdDlgSettings);

FCCmdDlgSettings::FCCmdDlgSettings()
	:FCCppCommand("Std_DlgSettings")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Settings...";
	sToolTipText	= "Edit the program settings";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	//sPixmap			= "settings";
	iAccel			= 0;
}


void FCCmdDlgSettings::Activated(int iMsg)
{
//	FCDlgSettings cDlg(GetAppWnd(),"SettingsDialog",true);
//	cDlg.exec();
}

//===========================================================================
// Std_CommandLine
//===========================================================================
DEF_STD_CMD(FCCmdCommandLine);

FCCmdCommandLine::FCCmdCommandLine()
	:FCCppCommand("Std_CommandLine")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Start command line...";
	sToolTipText	= "Opens the command line in the console";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "CommandLine";
	iAccel			= 0;
}


void FCCmdCommandLine::Activated(int iMsg)
{
	bool show = GetAppWnd()->isMaximized ();
	bool mute = GuiConsoleObserver::bMute;

	GetAppWnd()->showMinimized () ;
	qApp->processEvents();

	GuiConsoleObserver::bMute = true;
	Base::Interpreter().RunCommandLine("Console mode");
	GuiConsoleObserver::bMute = mute;

#ifdef Q_WS_X11
	// On X11 this may not work. For further information see QWidget::showMaximized
	//
	// workaround for X11
	GetAppWnd()->hide();
	GetAppWnd()->show();
#endif

	// pop up the main window
	show ? GetAppWnd()->showMaximized () : GetAppWnd()->showNormal () ;
	qApp->processEvents();
}


//===========================================================================
// Std_CreateToolOrCmdBar
//===========================================================================
DEF_STD_CMD(FCCmdCreateToolOrCmdBar);

FCCmdCreateToolOrCmdBar::FCCmdCreateToolOrCmdBar()
	:FCCppCommand("Std_CreateToolOrCmdBar")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Create tool-/cmdbar";
	sToolTipText	= "Create toolbar and/or command bar";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "CommandLine";
	iAccel			= 0;
}


void FCCmdCreateToolOrCmdBar::Activated(int iMsg)
{
  CreateToolBarDialog dlg(GetAppWnd(),"CreateToolbarDialog",true);
  dlg.exec();
}


//===========================================================================
// Std_OCAFBrowser
//===========================================================================

class FCCmdOCAFBrowse : public FCCppCommand
{
public:
	FCCmdOCAFBrowse();
//	~FCCmdOCAFBrowse();
	virtual void Activated(int iMsg);
	virtual bool IsActive(void);
private:
#ifdef FC_USE_OCAFBROWSER
	DebugBrowser cBrowser;
#endif
};

FCCmdOCAFBrowse::FCCmdOCAFBrowse()
	:FCCppCommand("Std_OCAFBrowser")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Start raw document browser...";
	sToolTipText	= "Starts a new window to deeply examine the document structure";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Copy";
	iAccel			= Qt::CTRL+Qt::Key_B;


}

void FCCmdOCAFBrowse::Activated(int iMsg)
{
#ifdef FC_USE_OCAFBROWSER
	cBrowser.DFBrowser(GetAppWnd()->GetActiveDocument()->GetDocument()->GetOCCDoc());
#else
	QMessageBox::information(GetAppWnd(), "OCAFBrowser", "Because FreeCAD has been compiled without set the 'FC_USE_OCAFBROWSER' flag\n"
											   "this feature is disabled.");
#endif
}

bool FCCmdOCAFBrowse::IsActive(void)
{
	return GetAppWnd()->GetActiveDocument() != 0;
}



void CreateStdCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.AddCommand(new FCCmdNew());
	rcCmdMgr.AddCommand(new FCCmdOpen());

	rcCmdMgr.AddCommand(new FCCmdSave());
	rcCmdMgr.AddCommand(new FCCmdSaveAs());
	rcCmdMgr.AddCommand(new FCCmdUndo());
	rcCmdMgr.AddCommand(new FCCmdRedo());
	rcCmdMgr.AddCommand(new FCCmdPrint());
	rcCmdMgr.AddCommand(new FCCmdQuit());
	rcCmdMgr.AddCommand(new FCCmdCut());
	rcCmdMgr.AddCommand(new FCCmdCopy());
	rcCmdMgr.AddCommand(new FCCmdPaste());
//	rcCmdMgr.AddCommand(new FCCmdMDINormal());
	rcCmdMgr.AddCommand(new FCCmdMDIToplevel());
	rcCmdMgr.AddCommand(new FCCmdMDITabed());
	rcCmdMgr.AddCommand(new FCCmdTileHor());
	rcCmdMgr.AddCommand(new FCCmdTileVer());
	rcCmdMgr.AddCommand(new FCCmdTilePra());
	rcCmdMgr.AddCommand(new FCCmdAbout());
	rcCmdMgr.AddCommand(new FCCmdAboutQt());

	rcCmdMgr.AddCommand(new FCCmdDlgParameter());
	rcCmdMgr.AddCommand(new FCCmdDlgPreferences());
	rcCmdMgr.AddCommand(new FCCmdDlgMacroRecord());
	rcCmdMgr.AddCommand(new FCCmdDlgMacroExecute());
	rcCmdMgr.AddCommand(new FCCmdMacroStop());
	rcCmdMgr.AddCommand(new FCCmdDlgCustomize());
	rcCmdMgr.AddCommand(new FCCmdDlgSettings());
	rcCmdMgr.AddCommand(new FCCmdCommandLine());
	rcCmdMgr.AddCommand(new FCCmdCreateToolOrCmdBar());
	rcCmdMgr.AddCommand(new FCCmdWorkbench());
	rcCmdMgr.AddCommand(new FCCmdMRU());
	rcCmdMgr.AddCommand(new FCCmdWhatsThis());
	rcCmdMgr.AddCommand(new FCCmdOnlineHelp());
	rcCmdMgr.AddCommand(new FCCmdOCAFBrowse());
	rcCmdMgr.AddCommand(new FCCmdTipOfTheDay());
	rcCmdMgr.AddCommand(new FCCmdWindows());
}



