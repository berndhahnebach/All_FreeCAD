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
#endif

#include "../../../Base/Exception.h"
#include "../../../App/Document.h"
#include "../../../Gui/Application.h"
#include "../../../Gui/Command.h"


#include "../../../Gui/Icons/images.cpp"
#include "../../../Gui/Icons/x.xpm"

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//===========================================================================
// Import_Box
//===========================================================================
DEF_STD_CMD_A(FCCmdImportReadBREP);

FCCmdImportReadBREP::FCCmdImportReadBREP()
	:FCCppCommand("Import_ReadBREP")
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


void FCCmdImportReadBREP::Activated(int iMsg)
{

	OpenCommand("Read BREP");

	QString fn = FCFileDialog::getOpenFileName( QString::null, "BREP (*.brep *.rle)", GetAppWnd() );
	if ( fn.isEmpty() )
	{
		AbortCommand();
		return;
	}


	DoCommand(Doc,"TopoShape = Import.ReadBREP(\"%s\")",fn.latin1());


	CommitCommand();
}

bool FCCmdImportReadBREP::IsActive(void)
{
	return GetAppWnd()->GetActiveDocument() != 0;
}



void CreateCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.AddCommand(new FCCmdImportReadBREP());
}



