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
#	include <BRepTools.hxx>
#	include <Handle_TPrsStd_AISPresentation.hxx>
#	include <TNaming_NamedShape.hxx>
#	include <TPrsStd_AISPresentation.hxx>
#endif

#include "../../../Base/Exception.h"
#include "../../../App/Document.h"
#include "../../../App/Label.h"
#include "../../../Gui/Application.h"
#include "../../../Gui/Command.h"
#include "../../../Gui/Widgets.h"


#include "../../../Gui/Icons/images.cpp"

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>

#include "DlgPartBoxImp.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_Test1
//===========================================================================
DEF_STD_CMD(FCCmdPartTest1);

FCCmdPartTest1::FCCmdPartTest1()
	:FCCppCommand("Part_Test1")
{
	sAppModule		= "Part";
	sGroup			= "Part";
	sMenuText		= "Test1";
	sToolTipText	= "Part Test function 1";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Test1";
	iAccel			= Qt::CTRL+Qt::Key_T;
}


void FCCmdPartTest1::Activated(int iMsg)
{

	DlgPartBoxImp cDlg(GetAppWnd(),"Part Box",true);
	cDlg.exec();

}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD(FCCmdPartTest2);

FCCmdPartTest2::FCCmdPartTest2()
	:FCCppCommand("Part_Test2")
{
	sAppModule		= "Part";
	sGroup			= "Part";
	sMenuText		= "Test2";
	sToolTipText	= "Part Test function 2";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Test2";
	iAccel			= 0;
}


void FCCmdPartTest2::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;


	OpenCommand("Test Load Part");

	GetConsole().Log("Using Doc: %p\n",pcDoc);

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main());

	L = L.FindChild(1);

	BRep_Builder aBuilder;
	TopoDS_Shape ResultShape;

	QString fn = FCFileDialog::getOpenFileName( QString::null, "BREP (*.brep *.rle)", GetAppWnd() );
	if ( fn.isEmpty() )
	{
		AbortCommand();
		return;
	}
 
	try{
	  BRepTools::Read(ResultShape,(const Standard_CString)fn.latin1(),aBuilder);
	}
	// Boeser Fehler ;-)
	catch(...){
		AbortCommand();
	  throw new FCException("Error loading BREP file");
	}

	TNaming_Builder B(L);
	B.Generated(ResultShape);

	Handle(TPrsStd_AISPresentation) hcPrs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID());
	// Display it
	hcPrs->Display(1);

	UpdateActive();
}

//===========================================================================
// Part_Box
//===========================================================================
DEF_STD_CMD(FCCmdPartBox);

FCCmdPartBox::FCCmdPartBox()
	:FCCppCommand("Part_Box")
{
	sAppModule		= "Part";
	sGroup			= "Part";
	sMenuText		= "Box";
	sToolTipText	= "Create or change a Box feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Part_Box";
	iAccel			= 0;
}


void FCCmdPartBox::Activated(int iMsg)
{

	DlgPartBoxImp cDlg(GetAppWnd(),"Part Box",true);
	cDlg.exec();

}

//===========================================================================
// Part_Box
//===========================================================================
DEF_STD_CMD(FCCmdPartCut);

FCCmdPartCut::FCCmdPartCut()
	:FCCppCommand("Part_Cut")
{
	sAppModule		= "Part";
	sGroup			= "Part";
	sMenuText		= "Cut";
	sToolTipText	= "Create or change a Cut feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Part_Box";
	iAccel			= 0;
}


void FCCmdPartCut::Activated(int iMsg)
{

	DlgPartBoxImp cDlg(GetAppWnd(),"Part Box",true);
	cDlg.exec();

}



void CreateCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.AddCommand(new FCCmdPartTest1());
	rcCmdMgr.AddCommand(new FCCmdPartTest2());
	rcCmdMgr.AddCommand(new FCCmdPartCut());
	rcCmdMgr.AddCommand(new FCCmdPartBox());

}



