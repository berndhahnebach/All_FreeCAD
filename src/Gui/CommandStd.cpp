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
 
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
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


#include "DlgDocTemplatesImp.h"
#include "DlgParameterImp.h"
#include "DlgMacroExecuteImp.h"
#include "DlgMacroRecordImp.h"
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
  DlgDocTemplatesImp cDlg(GetAppWnd(),"Template Dialog",true);
  cDlg.exec();

}

//===========================================================================
// Std_Save
//===========================================================================
DEF_STD_CMD(FCCmdSave);

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
	GetActiveDocument()->Save();
}

//===========================================================================
// Std_SaveAs
//===========================================================================
DEF_STD_CMD(FCCmdSaveAs);

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

//===========================================================================
// Std_Print
//===========================================================================
DEF_STD_CMD(FCCmdPrint );

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

		///////////////////////////////////////////////////////////////////
		// TODO: Define printing by using the QPainter methods here

		painter.end();
	}
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
  ApplicationWindow::Instance->slotUndo();
}

QWidget* FCCmdUndo::GetWidget()
{
  return _pclUndoRedoWidget;
}

FCAction * FCCmdUndo::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCUndoAction(this,ApplicationWindow::Instance,sName.c_str(),_eType&Cmd_Toggle != 0);
	pcAction->setText(_pcAction->tr(sMenuText));
	pcAction->setMenuText(_pcAction->tr(sMenuText));
	pcAction->setToolTip(_pcAction->tr(sToolTipText));
	pcAction->setStatusTip(_pcAction->tr(sStatusTip));
	pcAction->setWhatsThis(_pcAction->tr(sWhatsThis));
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
  ApplicationWindow::Instance->slotRedo();
}

QWidget* FCCmdRedo::GetWidget()
{
  return _pclUndoRedoWidget;
}

FCAction * FCCmdRedo::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCRedoAction(this,ApplicationWindow::Instance,sName.c_str(),_eType&Cmd_Toggle != 0);
	pcAction->setText(_pcAction->tr(sMenuText));
	pcAction->setMenuText(_pcAction->tr(sMenuText));
	pcAction->setToolTip(_pcAction->tr(sToolTipText));
	pcAction->setStatusTip(_pcAction->tr(sStatusTip));
	pcAction->setWhatsThis(_pcAction->tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixmap));
	pcAction->setAccel(iAccel);

	return pcAction;
}

//===========================================================================
// Std_Cut
//===========================================================================
DEF_STD_CMD(FCCmdCut);

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

}

//===========================================================================
// Std_Copy
//===========================================================================
DEF_STD_CMD(FCCmdCopy );

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

}

//===========================================================================
// Std_Paste
//===========================================================================
DEF_STD_CMD(FCCmdPaste);

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
// Std_Test1
//===========================================================================
DEF_STD_CMD(FCCmdTest1);

FCCmdTest1::FCCmdTest1()
	:FCCppCommand("Std_Test1")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test1";
	sToolTipText	= "Test function 1";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Test1";
	iAccel			= Qt::CTRL+Qt::Key_T;
}


void FCCmdTest1::Activated(int iMsg)
{
  // A data structure for our box:
  // the box itself is attached to the BoxLabel label (as his name and his function attribute) 
  // its arguments (dimensions: width, length and height; and position: x, y, z) 
  // are attached to the child labels of the box:
  //
  // 0:1 Box Label ---> Name --->  Named shape ---> Function
  //       |
  //     0:1:1 -- Width Label
  //       |
  //     0:1:2 -- Length Label
  //       |
  //     0:1:3 -- Height Label
  //       |
  //     0:1:4 -- X Label
  //       |
  //     0:1:5 -- Y Label
  //       |
  //     0:1:6 -- Z Label

	// Create a new label in the data structure for the box
	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main()->GetOCCLabel());

	// Create the data structure : Set the dimensions, position and name attributes
	TDataStd_Real::Set(L.FindChild(1), 1);
	TDataStd_Real::Set(L.FindChild(2), 2);
	TDataStd_Real::Set(L.FindChild(3), 3);
	TDataStd_Real::Set(L.FindChild(4), 4);
	TDataStd_Real::Set(L.FindChild(5), 5);
	TDataStd_Real::Set(L.FindChild(6), 6);
	TDataStd_Name::Set(L, "hallo");

	// Instanciate a TSampleOcafFunction_BoxDriver and add it to the TFunction_DriverTable
//	Handle(TSampleOcafFunction_BoxDriver) myBoxDriver = new TSampleOcafFunction_BoxDriver();
//	TFunction_DriverTable::Get()->AddDriver(Standard_GUID("BoxDriver"), myBoxDriver);
	// Instanciate a TFunction_Function attribute connected to the current box driver
	// and attach it to the data structure as an attribute of the Box Label
//	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L,Standard_GUID("BoxDriver"));

	// Initialize and execute the box driver (look at the "Execute()" code)
//    TFunction_Logbook log;
//	myBoxDriver->Init(L);
//    if (myBoxDriver->Execute(log)) MessageBox(0,"DFunction_Execute : failed","Box",MB_ICONERROR);


	// Make a box
 	BRepPrimAPI_MakeBox mkBox( gp_Pnt(1, 2 ,3), 4, 5 ,6);
	TopoDS_Shape ResultShape = mkBox.Shape();


	// Build a TNaming_NamedShape using built box
	TNaming_Builder B(L);
	B.Generated(ResultShape);

	Handle(TPrsStd_AISPresentation) hcPrs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
	// Display it
	hcPrs->Display(1);


}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD(FCCmdTest2);

FCCmdTest2::FCCmdTest2()
	:FCCppCommand("Std_Test2")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test2";
	sToolTipText	= "Test function 2";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Test2";
	iAccel			= 0;
}


void FCCmdTest2::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

	GetConsole().Log("Using Doc: %p\n",pcDoc);

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main()->GetOCCLabel());

	BRep_Builder aBuilder;
	TopoDS_Shape ResultShape;

	QString fn = FCFileDialog::getOpenFileName( QString::null, "BREP (*.brep *.rle)", GetAppWnd() );
	if ( fn.isEmpty() ) return;
 
	try{
	  BRepTools::Read(ResultShape,(const Standard_CString)fn.latin1(),aBuilder);
	}
	// Boeser Fehler ;-)
	catch(...){
	  throw new FCException("Error loading BREP file");
	}  

	TNaming_Builder B(L);
	B.Generated(ResultShape);

	Handle(TPrsStd_AISPresentation) hcPrs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
	// Display it
	hcPrs->Display(1);

}

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
	sPixmap			= "TileHorizontal";
	iAccel			= 0;
}


void FCCmdTileHor::Activated(int iMsg)
{
GetAppWnd()->expandHorizontal();
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
	sPixmap			= "TileVertical";
	iAccel			= Qt::CTRL+Qt::Key_T;
}


void FCCmdTileVer::Activated(int iMsg)
{
	GetAppWnd()->expandVertical();
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
	sPixmap			= "Paste";
	iAccel			= 0;
}


void FCCmdTilePra::Activated(int iMsg)
{
	GetAppWnd()->tilePragma();
}

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

//===========================================================================
// Std_MDIToplevel
//===========================================================================
DEF_STD_CMD(FCCmdMDIToplevel);

FCCmdMDIToplevel::FCCmdMDIToplevel()
	:FCCppCommand("Std_MDIToplevel")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "MDI Toplevel";
	sToolTipText	= "Set the top level MDI mode";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "TopLevel";
	iAccel			= 0;
}


void FCCmdMDIToplevel::Activated(int iMsg)
{
	GetAppWnd()->switchToToplevelMode();
}

//===========================================================================
// Std_MDITabed
//===========================================================================
DEF_STD_CMD(FCCmdMDITabed);

FCCmdMDITabed::FCCmdMDITabed()
	:FCCppCommand("Std_MDITabed")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "MDI tabed";
	sToolTipText	= "Set the tabed MDI mode";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Paste";
	iAccel			= 0;
}


void FCCmdMDITabed::Activated(int iMsg)
{
	GetAppWnd()->switchToTabPageMode();
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
	sPixmap			= "settings";
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
	sPixmap			= "settings";
	iAccel			= 0;
}


void FCCmdDlgPreferences::Activated(int iMsg)
{
	DlgPreferencesImp cDlg(GetAppWnd(),"Prefernces Dialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgMacroRecord
//===========================================================================
DEF_STD_CMD(FCCmdDlgMacroRecord);

FCCmdDlgMacroRecord::FCCmdDlgMacroRecord()
	:FCCppCommand("Std_DlgMacroRecord")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Macro recording ...";
	sToolTipText	= "Opens a Dialog to record a macro";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Record";
	iAccel			= 0;
}


void FCCmdDlgMacroRecord::Activated(int iMsg)
{
	DlgMacroRecordImp cDlg(GetAppWnd(),"ParameterDialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgMacroExecute
//===========================================================================
DEF_STD_CMD(FCCmdDlgMacroExecute);

FCCmdDlgMacroExecute::FCCmdDlgMacroExecute()
	:FCCppCommand("Std_DlgMacroExecute")
{
	sAppModule		= "";
	sGroup			= "Standard";
	sMenuText		= "Execute macro ...";
	sToolTipText	= "Opens a Dialog let you execute a redordet macro";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Play";
	iAccel			= 0;
}


void FCCmdDlgMacroExecute::Activated(int iMsg)
{
	DlgMacroExecuteImp cDlg(GetAppWnd(),"Macro Execute",true);
	cDlg.exec();
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
	sPixmap			= "customize";
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
	sPixmap			= "settings";
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
	rcCmdMgr.AddCommand(new FCCmdMDINormal());
	rcCmdMgr.AddCommand(new FCCmdMDIToplevel());
	rcCmdMgr.AddCommand(new FCCmdMDITabed());
	rcCmdMgr.AddCommand(new FCCmdTileHor());
	rcCmdMgr.AddCommand(new FCCmdTileVer());
	rcCmdMgr.AddCommand(new FCCmdTilePra());
	rcCmdMgr.AddCommand(new FCCmdTest1());
	rcCmdMgr.AddCommand(new FCCmdTest2());
	rcCmdMgr.AddCommand(new FCCmdAbout());

	rcCmdMgr.AddCommand(new FCCmdDlgParameter());
	rcCmdMgr.AddCommand(new FCCmdDlgPreferences());
	rcCmdMgr.AddCommand(new FCCmdDlgMacroRecord());
	rcCmdMgr.AddCommand(new FCCmdDlgMacroExecute());
	rcCmdMgr.AddCommand(new FCCmdDlgCustomize());
	rcCmdMgr.AddCommand(new FCCmdDlgSettings());
	rcCmdMgr.AddCommand(new FCCmdCommandLine());
	rcCmdMgr.AddCommand(new FCCmdCreateToolOrCmdBar());
}



