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
#	include <qaction.h>
#	include <BRepPrimAPI_MakeBox.hxx>
#	include <TopoDS_Shape.hxx>
#	include <TNaming_Builder.hxx>
#endif

#include "../../../Base/Exception.h"
#include "../../../App/Document.h"
#include "../../../Gui/Application.h"
#include "../../../Gui/Command.h"
#include "../../../Gui/FileDialog.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
using Gui::FileDialog;

//===========================================================================
// Import_Box
//===========================================================================
DEF_STD_CMD_A(FCCmdImportReadBREP);

FCCmdImportReadBREP::FCCmdImportReadBREP()
	:CppCommand("Import_ReadBREP")
{
	sAppModule		= "Import";
	sGroup			= "Import";
	sMenuText		= "Read BREP";
	sToolTipText	= "Read a BREP file";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool1";
	iAccel			= 0;
}


void FCCmdImportReadBREP::activated(int iMsg)
{

	openCommand("Read BREP");

	QString fn = FileDialog::getOpenFileName( QString::null, "BREP (*.brep *.rle)", getAppWnd() );
	if ( fn.isEmpty() )
	{
		abortCommand();
		return;
	}


	doCommand(Doc,"TopoShape = Import.ReadBREP(\"%s\")",fn.latin1());


	commitCommand();
}

bool FCCmdImportReadBREP::isActive(void)
{
	return getAppWnd()->activeDocument() != 0;
}

//===========================================================================
// PartImportStep
//===========================================================================
DEF_STD_CMD_A(ImportStep);

ImportStep::ImportStep()
	:CppCommand("Part_ImportStep")
{
	sAppModule		= "Part";
	sGroup			  = "Part";
	sMenuText		  = "Import STEP";
	sToolTipText	= "Create or change a Import STEP feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Save";
	iAccel			  = 0;
}


void ImportStep::activated(int iMsg)
{


//  DlgPartImportStepImp cDlg(getAppWnd(),"Part import STEP",true);
//  if ( cDlg.exec() == QDialog::Accepted )

  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "STEP (*.stp *.step);;All Files (*.*)", 
                                                 Gui::ApplicationWindow::Instance );
	if (! fn.isEmpty() )
  {
    openCommand("Part ImportSTEP Create");
	  doCommand(Doc,"f = App.DocGet().AddFeature(\"ImportStep\",\"ImportStep\")");
	  doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
//	  DoCommand(Doc,"App.DocGet().Update()");
    commitCommand();
  
    updateActive();

  }
}

bool ImportStep::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;

}


//===========================================================================
// ImportIges
//===========================================================================
DEF_STD_CMD_A(ImportIges);

ImportIges::ImportIges()
	:CppCommand("Import_Iges")
{
	sAppModule		= "Import";
	sGroup			= "Part";
	sMenuText		= "Import IGES";
	sToolTipText	= "Create or change a Import IGES feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Save";
	iAccel			= 0;
}


void ImportIges::activated(int iMsg)
{

//  DlgPartImportIgesImp cDlg(getAppWnd(),"Part import IGES",true);
//  if ( cDlg.exec() == QDialog::Accepted )

  
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "IGES (*.igs *.iges);;All Files (*.*)", 
                                                 Gui::ApplicationWindow::Instance );
	if (! fn.isEmpty() )
  {
    openCommand("ImportIGES Create");
	  doCommand(Doc,"f = App.DocGet().AddFeature(\"ImportIges\",\"ImportIges\")");
//	  doCommand(Doc,"f.FileName = \"%s\"",cDlg.FileName->text().ascii());
	  doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
	  doCommand(Doc,"App.DocGet().Update()");
    commitCommand();
  
    updateActive();
  }


}

bool ImportIges::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;

}



void CreateCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();

	rcCmdMgr.addCommand(new FCCmdImportReadBREP());
}



