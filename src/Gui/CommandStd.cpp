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

#include "CommandStd.h"
#include "DlgDocTemplatesImp.h"
#include "Icons/images.cpp"
#include "Application.h"




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
  QString fn = QFileDialog::getSaveFileName(0, 0, AppWnd());
  if (!fn.isEmpty())
  {
    //doc->saveAs(fn);
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
	};

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
	*sToolTipText = "Redos a previosly Undoed action";
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

