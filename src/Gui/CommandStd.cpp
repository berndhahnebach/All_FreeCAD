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
#	include <qstatusbar.h>
#	include <qfiledialog.h>
#	include <qprinter.h>
#	include <qmessagebox.h>
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
#include "Application.h"
#include "Document.h"
#include "Widgets.h"
#include "Splashscreen.h"
#include "Command.h"

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
#include "Icons/images.cpp"
#include "Icons/x.xpm"





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
	int iAccel      = Qt::CTRL+Qt::Key_O;

}


void FCCmdOpen::Activated(int iMsg)
{
	GetAppWnd()->statusBar()->message("Opening file...");

	QString f = QFileDialog::getOpenFileName( QString::null, "FreeCAD Standard (*.FCStd);;OpenCasCade (*.std)", GetAppWnd() );
	if ( !f.isEmpty() ) {
		// the user selected a valid existing file
		GetApplication().Open(f.latin1());
    GetAppWnd()->AppendRecentFile(f.latin1());
	} else {
		// the user cancelled the dialog
		GetAppWnd()->statusBar()->message("Opening aborted");
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
	int iAccel      = Qt::CTRL+Qt::Key_N;

}


void FCCmdNew::Activated(int iMsg)
{
  DlgDocTemplatesImp cDlg(this,GetAppWnd(),"Template Dialog",true);
  cDlg.exec();

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
  	GetActiveDocument()->SaveAs();

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
	QPrinter printer;
	if (printer.setup(GetAppWnd()))
	{
		QPainter painter;
		painter.begin(&printer);

		GetAppWnd()->GetActiveView()->Print(painter);

		painter.end();
	}
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
	iAccel			= Qt::CTRL+Qt::Key_X;
}


void FCCmdQuit::Activated(int iMsg)
{
  ApplicationWindow::Instance->close();
}


//===========================================================================
// Std_Undo
//===========================================================================

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
  _pclUndoRedoWidget = new FCUndoRedoDlg(ApplicationWindow::Instance, "Undo");
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

QWidget* FCCmdUndo::GetWidget()
{
  return _pclUndoRedoWidget;
}

FCAction * FCCmdUndo::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCUndoAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
  pcAction->setText(QObject::tr(sMenuText));
	pcAction->setMenuText(QObject::tr(sMenuText));
	pcAction->setToolTip(QObject::tr(sToolTipText));
	pcAction->setStatusTip(QObject::tr(sStatusTip));
	pcAction->setWhatsThis(QObject::tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixmap));
	pcAction->setAccel(iAccel);

	return pcAction;
}

//===========================================================================
// Std_Redo
//===========================================================================

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
	_pclUndoRedoWidget = new FCUndoRedoDlg(ApplicationWindow::Instance, "Redo");
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

QWidget* FCCmdRedo::GetWidget()
{
  return _pclUndoRedoWidget;
}

FCAction * FCCmdRedo::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCRedoAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(QObject::tr(sMenuText));
	pcAction->setMenuText(QObject::tr(sMenuText));
	pcAction->setToolTip(QObject::tr(sToolTipText));
	pcAction->setStatusTip(QObject::tr(sStatusTip));
	pcAction->setWhatsThis(QObject::tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixmap));
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
//	sPixmap			= "";
	iAccel			= 0;
}


void FCCmdWorkbench::Activated(int iMsg)
{
  std::vector<std::string> wb = ApplicationWindow::Instance->GetWorkbenches();
  if (iMsg >= 0 && iMsg < int(wb.size()))
  {
    //char szBuf[200];
    //sprintf(szBuf, "Gui.WorkbenchActivate(\"%s\")", wb[iMsg].c_str());
    DoCommand(Gui, "Gui.WorkbenchActivate(\"%s\")", wb[iMsg].c_str());
    UpdateAction(iMsg);
  }
}

void FCCmdWorkbench::UpdateAction(int i)
{
  if (!pcAction) 
    CreateAction();
  pcAction->activate(i);
}

void FCCmdWorkbench::UpdateAction(const char* item)
{
  if (!pcAction) 
    CreateAction();
  pcAction->activate(QString(item));
}

FCAction * FCCmdWorkbench::CreateAction(void)
{
	pcAction = new FCMultiAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(QObject::tr(sMenuText));
	pcAction->setMenuText(QObject::tr(sMenuText));
	pcAction->setToolTip(QObject::tr(sToolTipText));
	pcAction->setStatusTip(QObject::tr(sStatusTip));
	pcAction->setWhatsThis(QObject::tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixmap));
	pcAction->setAccel(iAccel);

  pcAction->setItems(ApplicationWindow::Instance->GetWorkbenches());

	return pcAction;
}

void FCCmdWorkbench::AddItem (const char* item)
{
  if (pcAction)
    pcAction->insertItem(item);
}

void FCCmdWorkbench::RemItem (const char* item)
{
  if (pcAction)
    pcAction->removeItem(item);
}

void FCCmdWorkbench::Clear()
{
  if (pcAction)
    pcAction->clear();
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
	:FCCppCommand("Std_MRU"), pcAction(NULL), nMaxItems(4)
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
      DoCommand(Gui, "App.DocOpen(\"%s\")", _vMRU[iMsg].c_str());
    }catch(const FCException&){
      RemItem(_vMRU[iMsg].c_str());
    }
  }
}

FCAction * FCCmdMRU::CreateAction(void)
{
	pcAction = new FCMultiAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(QObject::tr(sMenuText));
	pcAction->setMenuText(QObject::tr(sMenuText));
	pcAction->setToolTip(QObject::tr(sToolTipText));
	pcAction->setStatusTip(QObject::tr(sStatusTip));
	pcAction->setWhatsThis(QObject::tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixmap));
	pcAction->setAccel(iAccel);

	return pcAction;
}

void FCCmdMRU::AddItem (const char* item)
{
  if (std::find(_vMRU.begin(), _vMRU.end(), item) == _vMRU.end())
  {
    if (nMaxItems > (int)_vMRU.size())
    {
      _vMRU.push_back(item);
      if (pcAction)
        pcAction->insertItem(GetFileName(item));
    }
  }
}

void FCCmdMRU::RemItem (const char* item)
{
  if (pcAction)
    pcAction->clear();
  std::vector<std::string>::iterator it = std::find(_vMRU.begin(), _vMRU.end(), item);
  if (it!=_vMRU.end())
    _vMRU.erase(it);
  for (it=_vMRU.begin(); it!=_vMRU.end(); ++it)
    pcAction->insertItem(GetFileName(it->c_str()));
}

void FCCmdMRU::Clear()
{
  if (pcAction)
    pcAction->clear();
  _vMRU.clear();
}

QString FCCmdMRU::GetFileName(const char* name)
{
  std::vector<std::string>::iterator it = std::find(_vMRU.begin(), _vMRU.end(), name);
  int ct = (it-_vMRU.begin())+1;

  QString file(name);

  int npos = file.findRev('/');
  if (npos != -1)
  {
    QString fn = file.right(file.length()-npos-1);
    QString path = file.left(npos);
    QString cur  = QDir::currentDirPath();
    if (path != cur)
      fn = file;
    fn.prepend(QObject::tr("%1 ").arg(ct));
    return fn;
  }

  file.prepend(QObject::tr("%1 ").arg(ct));
  return file;
}

std::vector<std::string> FCCmdMRU::GetItems() const
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
  FCSplashAbout::Instance();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//===========================================================================
// Std_TileHoricontal
//===========================================================================
DEF_STD_CMD(FCCmdTileHor);

FCCmdTileHor::FCCmdTileHor()
	:FCCppCommand("Std_TileHoricontal")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Tile Hor.";
	sToolTipText	= "Tile the windows horizontal";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_WindowTileHor";
	iAccel			= 0;
}


void FCCmdTileHor::Activated(int iMsg)
{
	GetAppWnd()->tileVertically ();
}

//===========================================================================
// Std_TileVertical
//===========================================================================
DEF_STD_CMD(FCCmdTileVer);

FCCmdTileVer::FCCmdTileVer()
	:FCCppCommand("Std_TileVertical")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Tile Ver.";
	sToolTipText	= "Tile the windows vertical";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_WindowTileVer";
	iAccel			= Qt::CTRL+Qt::Key_T;
}


void FCCmdTileVer::Activated(int iMsg)
{
	GetAppWnd()->tileAnodine ();
}

//===========================================================================
// Std_TilePragmatic
//===========================================================================
DEF_STD_CMD(FCCmdTilePra);

FCCmdTilePra::FCCmdTilePra()
	:FCCppCommand("Std_TilePragmatic")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Tile pragmatic";
	sToolTipText	= "Tile pragmatic";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_WindowCascade";
	iAccel			= 0;
}

void FCCmdTilePra::Activated(int iMsg)
{
	GetAppWnd()->tilePragma();
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
		GetAppWnd()->switchToToplevelMode();
	}else
		//GetAppWnd()->finishToplevelMode();
		GetAppWnd()->switchToChildframeMode();
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
		GetAppWnd()->switchToTabPageMode();
	}else
		GetAppWnd()->switchToChildframeMode();
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
	FCDlgPreferencesImp cDlg(GetAppWnd(),"Prefernces Dialog",true);
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
	sToolTipText	= "Customize toolbars and button groups";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	//sPixmap			= "customize";
	iAccel			= 0;
}


void FCCmdDlgCustomize::Activated(int iMsg)
{
	FCDlgCustomize cDlg(GetAppWnd(),"CustomizeDialog",true);
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
	bool b = GetAppWnd()->isMaximized ();
	GetAppWnd()->showMinimized () ;
	// set the Gui console observer mute
	FCGuiConsoleObserver::bMute = true;
	GetInterpreter().RunCommandLine("Console mode");
	// unmute the Gui console observer 
	FCGuiConsoleObserver::bMute = true;
	// pop up the main window
	GetAppWnd()->showMaximized () ;
	// restore the former mode
	if (!b) GetAppWnd()->showNormal () ;

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
  FCDlgCreateToolOrCmdBar dlg(GetAppWnd(),"CreateToolbarDialog",true);
  dlg.exec();
}


//===========================================================================
// Std_OCAFBrowser
//===========================================================================
#ifdef FC_USE_OCAFBROWSER

class FCCmdOCAFBrowse : public FCCppCommand
{
public:
	FCCmdOCAFBrowse();
//	~FCCmdOCAFBrowse();
	virtual void Activated(int iMsg);
	virtual bool IsActive(void);
private:
	DebugBrowser cBrowser;
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
	iAccel			= Qt::CTRL+Qt::Key_C;


}

void FCCmdOCAFBrowse::Activated(int iMsg)
{
	cBrowser.DFBrowser(GetAppWnd()->GetActiveDocument()->GetDocument()->GetOCCDoc());
}

bool FCCmdOCAFBrowse::IsActive(void)
{
	return GetAppWnd()->GetActiveDocument() != 0;
}

#endif


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

#	ifdef FC_USE_OCAFBROWSER
		rcCmdMgr.AddCommand(new FCCmdOCAFBrowse());
#	endif

}



