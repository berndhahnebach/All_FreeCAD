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
#endif

#include "Widgets.h"
#include "Splashscreen.h"
#include "CommandStd.h"
#include "DlgDocTemplatesImp.h"
#include "DlgParameterImp.h"
#include "DlgMacroExecuteImp.h"
#include "DlgMacroRecordImp.h"
#include "DlgPreferencesImp.h"
#include "Icons/images.cpp"
#include "Icons/x.xpm"
#include "Application.h"
#include "../Base/Exception.h"
#include "../App/Document.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>



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
	// views
	rcCmdMgr.AddCommand(new FCCmdViewBottom());
	rcCmdMgr.AddCommand(new FCCmdViewFront());
	rcCmdMgr.AddCommand(new FCCmdViewLeft());
	rcCmdMgr.AddCommand(new FCCmdViewRear());
	rcCmdMgr.AddCommand(new FCCmdViewRight());
	rcCmdMgr.AddCommand(new FCCmdViewTop());
	rcCmdMgr.AddCommand(new FCCmdViewAxo());
	rcCmdMgr.AddCommand(new FCCmdViewFitAll());

	rcCmdMgr.AddCommand(new FCCmdDlgParameter());
	rcCmdMgr.AddCommand(new FCCmdDlgPreferences());
	rcCmdMgr.AddCommand(new FCCmdDlgMacroRecord());
	rcCmdMgr.AddCommand(new FCCmdDlgMacroExecute());

}




//===========================================================================
// Std_Open
//===========================================================================

void FCCmdOpen::CmdProfile(char** sMenuText, 
							char** sToolTipText, 
							char** sWhatsThis, 
							char** sStatusTip, 
							QPixmap &cPixmap, 
							int &iAccel)
{
	*sMenuText	  = "Open";
	*sToolTipText = "Open a Document or import Files";
	*sWhatsThis   = "Open a Document or import Files";
	*sStatusTip   = "Open a Document or import Files";
	cPixmap = QPixmap(pOpen);
	iAccel = Qt::CTRL+Qt::Key_O;
}


void FCCmdOpen::Activated(void)
{
	AppWnd()->statusBar()->message("Opening file...");

	QString f = QFileDialog::getOpenFileName( QString::null, "FreeCAD Standard (*.FCStd);;OpenCasCade (*.std)", AppWnd() );
	if ( !f.isEmpty() ) {
		// the user selected a valid existing file
		GetApplication().Open(f.latin1());
	} else {
		// the user cancelled the dialog
		AppWnd()->statusBar()->message("Opening aborted");
	}

}

//===========================================================================
// Std_New
//===========================================================================

void FCCmdNew::CmdProfile(char** sMenuText, 
							char** sToolTipText, 
							char** sWhatsThis, 
							char** sStatusTip, 
							QPixmap &cPixmap, 
							int &iAccel)
{
	*sMenuText	  = "New";
	*sToolTipText = "Create a new empty Document";
	*sWhatsThis   = "Create a new empty Document";
	*sStatusTip   = "Create a new empty Document";
	cPixmap = QPixmap(pNew);
	iAccel = Qt::CTRL+Qt::Key_N;
}


void FCCmdNew::Activated(void)
{
  DlgDocTemplatesImp cDlg(AppWnd(),"Dialog",true);
  cDlg.exec();

}

//===========================================================================
// Std_Save
//===========================================================================

void FCCmdSave::CmdProfile(char** sMenuText, 
							char** sToolTipText, 
							char** sWhatsThis, 
							char** sStatusTip, 
							QPixmap &cPixmap, 
							int &iAccel)
{
	*sMenuText	  = "Save";
	*sToolTipText = "Save the active document";
	*sWhatsThis   = "Save the active document";
	*sStatusTip   = "Save the active document";
	cPixmap = QPixmap(pSave);
	iAccel = Qt::CTRL+Qt::Key_S;
}


void FCCmdSave::Activated(void)
{
	GetApplication().Save();
}

//===========================================================================
// Std_SaveAs
//===========================================================================

void FCCmdSaveAs::CmdProfile(char** sMenuText, 
							char** sToolTipText, 
							char** sWhatsThis, 
							char** sStatusTip, 
							QPixmap &cPixmap, 
							int &iAccel)
{
	*sMenuText	  = "Save as...";
	*sToolTipText = "Save the active document under a new file name";
	*sWhatsThis   = "Save the active document under a new file name";
	*sStatusTip   = "Save the active document under a new file name";
	//cPixmap = QPixmap(Save);
	iAccel = 0;
}


void FCCmdSaveAs::Activated(void)
{
  AppWnd()->statusBar()->message("Saving file under new filename...");
  QString fn = QFileDialog::getSaveFileName(0, "FreeCAD (*.FCStd *.FCPart)", AppWnd());
  if (!fn.isEmpty())
  {
    GetApplication().SaveAs(fn.latin1());
  }
  else
  {
    AppWnd()->statusBar()->message("Saving aborted", 2000);
  }

  //statusBar()->message(IDS_STATUS_DEFAULT);

}

//===========================================================================
// Std_Print
//===========================================================================

void FCCmdPrint::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Print...";
	*sToolTipText = "Print the window";
	*sWhatsThis   = "Print the window";
	*sStatusTip   = "Print the window";
	cPixmap = QPixmap(pPrint);
	iAccel = Qt::CTRL+Qt::Key_P;;
}


void FCCmdPrint::Activated(void)
{
	AppWnd()->statusBar()->message("Printing...");
	QPrinter printer;
	if (printer.setup(AppWnd()))
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

void FCCmdQuit::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Exit";
	*sToolTipText = "Quits the application";
	*sWhatsThis   = "Quits the application";
	*sStatusTip   = "Quits the application";
	cPixmap = QPixmap(px);
	iAccel = Qt::CTRL+Qt::Key_X;
}


void FCCmdQuit::Activated(void)
{
  if (ApplicationWindow::Instance->GetActiveView() == NULL)
  {
    qApp->quit();
    return;
  }

  int iButton = QMessageBox::warning(ApplicationWindow::Instance, 
                                     "FreeCAD", "Save changes to file?", "Yes", "No", "Cancel", 0);
  if (iButton == 1) // no
    qApp->quit();
	

	//statusBar()->message(IDS_STATUS_DEFAULT);
}

//===========================================================================
// Std_Undo
//===========================================================================

void FCCmdUndo::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Undo";
	*sToolTipText = "Undo exactly one action";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pUndo);
	iAccel = Qt::CTRL+Qt::Key_Z;;
}


void FCCmdUndo::Activated(void)
{

}


//===========================================================================
// Std_Redo
//===========================================================================

void FCCmdRedo::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Redo";
	*sToolTipText = "Redoes a previosly undid action";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pRedo);
	iAccel = Qt::CTRL+Qt::Key_Y;;
}


void FCCmdRedo::Activated(void)
{

}

//===========================================================================
// Std_Cut
//===========================================================================

void FCCmdCut::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Cut";
	*sToolTipText = "Cut out";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pCut);
	iAccel = Qt::CTRL+Qt::Key_X;;
}


void FCCmdCut::Activated(void)
{

}

//===========================================================================
// Std_Copy
//===========================================================================

void FCCmdCopy::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Copy";
	*sToolTipText = "Copy operation";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pCopy);
	iAccel = Qt::CTRL+Qt::Key_C;
}


void FCCmdCopy::Activated(void)
{

}

//===========================================================================
// Std_Paste
//===========================================================================

void FCCmdPaste::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Paste";
	*sToolTipText = "Paste operation";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = Qt::CTRL+Qt::Key_V;
}


void FCCmdPaste::Activated(void)
{

}

//===========================================================================
// Std_About
//===========================================================================

void FCCmdAbout::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "About FreeCAD";
	*sToolTipText = "About FreeCAD";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
}


void FCCmdAbout::Activated(void)
{
  FCSplashAbout::Instance();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_Test1
//===========================================================================

void FCCmdTest1::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Test1";
	*sToolTipText = "Test function 1";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = Qt::CTRL+Qt::Key_T;
}


void FCCmdTest1::Activated(void)
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
/*	Handle(TSampleOcafFunction_BoxDriver) myBoxDriver = new TSampleOcafFunction_BoxDriver();
	TFunction_DriverTable::Get()->AddDriver(Standard_GUID("BoxDriver"), myBoxDriver);
	// Instanciate a TFunction_Function attribute connected to the current box driver
	// and attach it to the data structure as an attribute of the Box Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L,Standard_GUID("BoxDriver"));

	// Initialize and execute the box driver (look at the "Execute()" code)
    TFunction_Logbook log;
	myBoxDriver->Init(L);
    if (myBoxDriver->Execute(log)) MessageBox(0,"DFunction_Execute : failed","Box",MB_ICONERROR);
*/

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

void FCCmdTest2::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Test2";
	*sToolTipText = "Test function 2";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = 0;
}


void FCCmdTest2::Activated(void)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

	GetConsole().Log("Using Doc: %p\n",pcDoc);

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main()->GetOCCLabel());

	BRep_Builder aBuilder;
	TopoDS_Shape ResultShape;

	QString fn = FCFileDialog::getOpenFileName( QString::null, "BREP (*.brep *.rle)", AppWnd() );
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

void FCCmdTileHor::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Tile Hor.";
	*sToolTipText = "Tile the windows horizontal";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = 0;
}


void FCCmdTileHor::Activated(void)
{
AppWnd()->expandHorizontal();
}

//===========================================================================
// Std_TileVertical
//===========================================================================

void FCCmdTileVer::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Tile Ver.";
	*sToolTipText = "Tile the windows vertical";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = Qt::CTRL+Qt::Key_T;
}


void FCCmdTileVer::Activated(void)
{
AppWnd()->expandVertical();
}

//===========================================================================
// Std_TilePragmatic
//===========================================================================

void FCCmdTilePra::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Tile pragmatic";
	*sToolTipText = "Tile pragmatic";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = 0;
}


void FCCmdTilePra::Activated(void)
{
AppWnd()->tilePragma();
}

//===========================================================================
// Std_MDINormal
//===========================================================================

void FCCmdMDINormal::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "MDI Normal";
	*sToolTipText = "Set the standard MDI mode";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = 0;
}


void FCCmdMDINormal::Activated(void)
{
	AppWnd()->switchToChildframeMode();
}

//===========================================================================
// Std_MDIToplevel
//===========================================================================

void FCCmdMDIToplevel::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "MDI Toplevel";
	*sToolTipText = "Set the top level MDI mode";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = 0;
}


void FCCmdMDIToplevel::Activated(void)
{
	AppWnd()->switchToToplevelMode();
}

//===========================================================================
// Std_MDITabed
//===========================================================================

void FCCmdMDITabed::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "MDI tabed";
	*sToolTipText = "Set the tabed MDI mode";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(pPaste);
	iAccel = 0;
}


void FCCmdMDITabed::Activated(void)
{
	AppWnd()->switchToTabPageMode();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_ViewBottom
//===========================================================================

void FCCmdViewBottom::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Bottom";
	*sToolTipText = "Set to bottom view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_bottom);
	iAccel = 0;
}


void FCCmdViewBottom::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewBottom");
}

//===========================================================================
// Std_ViewFront
//===========================================================================

void FCCmdViewFront::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Front";
	*sToolTipText = "Set to front view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_front);
	iAccel = 0;
}


void FCCmdViewFront::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewFront");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================

void FCCmdViewLeft::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Left";
	*sToolTipText = "Set to left view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_left);
	iAccel = 0;
}


void FCCmdViewLeft::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewLeft");
}

//===========================================================================
// Std_ViewRear
//===========================================================================

void FCCmdViewRear::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Rear";
	*sToolTipText = "Set to rear view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_back);
	iAccel = 0;
}


void FCCmdViewRear::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewRear");
}

//===========================================================================
// Std_ViewBottom
//===========================================================================

void FCCmdViewRight::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Right";
	*sToolTipText = "Set to right view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_right);
	iAccel = 0;
}


void FCCmdViewRight::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewRight");
}

//===========================================================================
// Std_ViewTop
//===========================================================================

void FCCmdViewTop::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Top";
	*sToolTipText = "Set to top view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_top);
	iAccel = 0;
}


void FCCmdViewTop::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewTop");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================

void FCCmdViewAxo::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Axometric";
	*sToolTipText = "Set to axometric view";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_axo);
	iAccel = 0;
}


void FCCmdViewAxo::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewAxo");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================

void FCCmdViewFitAll::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Fit all";
	*sToolTipText = "Fits the whole content on the screen";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_fitall);
	iAccel = 0;
}


void FCCmdViewFitAll::Activated(void)
{
	AppWnd()->SendMsgToActiveView("ViewFit");
}

//===========================================================================
// Std_DlgParameter
//===========================================================================

void FCCmdDlgParameter::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Edit parameters ...";
	*sToolTipText = "Opens a Dialog to edit the parameters";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_fitall);
	iAccel = 0;
}


void FCCmdDlgParameter::Activated(void)
{
	DlgParameter cDlg(AppWnd(),"ParameterDialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgPreferences
//===========================================================================

void FCCmdDlgPreferences::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Preferences ...";
	*sToolTipText = "Opens a Dialog to edit the preferences";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_fitall);
	iAccel = 0;
}


void FCCmdDlgPreferences::Activated(void)
{
	DlgPreferencesImp cDlg(AppWnd(),"Prefernces Dialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgMacroRecord
//===========================================================================

void FCCmdDlgMacroRecord::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Macro recording ...";
	*sToolTipText = "Opens a Dialog to record a macro";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_fitall);
	iAccel = 0;
}


void FCCmdDlgMacroRecord::Activated(void)
{
	DlgMacroRecordImp cDlg(AppWnd(),"ParameterDialog",true);
	cDlg.exec();
}

//===========================================================================
// Std_DlgMacroExecute
//===========================================================================

void FCCmdDlgMacroExecute::CmdProfile(char** sMenuText,char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel)
{
	*sMenuText	  = "Execute macro ...";
	*sToolTipText = "Opens a Dialog let you execute a redordet macro";
	*sWhatsThis   = *sToolTipText;
	*sStatusTip   = *sToolTipText;
	cPixmap = QPixmap(view_fitall);
	iAccel = 0;
}


void FCCmdDlgMacroExecute::Activated(void)
{
	DlgMacroExecuteImp cDlg(AppWnd(),"Macro Execute",true);
	cDlg.exec();
}

