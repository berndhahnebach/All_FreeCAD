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
 
#include "../../../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
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
// Std_Test1
//===========================================================================
DEF_STD_CMD(FCCmdImportTest1);

FCCmdImportTest1::FCCmdImportTest1()
	:FCCppCommand("Import_Test1")
{
	sAppModule		= "Import";
	sGroup			= "Import";
	sMenuText		= "Test1";
	sToolTipText	= "Import Test function 1";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Test1";
	iAccel			= Qt::CTRL+Qt::Key_T;
}


void FCCmdImportTest1::Activated(int iMsg)
{

//	DlgImportBoxImp cDlg(GetAppWnd(),"Import Box",true);
//	cDlg.exec();

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
/*	FCDocument *pcDoc = GetActiveOCCDocument();
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
*/
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
/* 	BRepPrimAPI_MakeBox mkBox( gp_Pnt(1, 2 ,3), 4, 5 ,6);
	TopoDS_Shape ResultShape = mkBox.Shape();


	// Build a TNaming_NamedShape using built box
	TNaming_Builder B(L);
	B.Generated(ResultShape);

	Handle(TPrsStd_AISPresentation) hcPrs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
	// Display it
	hcPrs->Display(1);
*/

}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD(FCCmdImportTest2);

FCCmdImportTest2::FCCmdImportTest2()
	:FCCppCommand("Import_Test2")
{
	sAppModule		= "Import";
	sGroup			= "Import";
	sMenuText		= "Test2";
	sToolTipText	= "Import Test function 2";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Test2";
	iAccel			= 0;
}


void FCCmdImportTest2::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;


	OpenCommand("Test Load Import");

	GetConsole().Log("Using Doc: %p\n",pcDoc);

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main()->GetOCCLabel());

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

}

//===========================================================================
// Import_Box
//===========================================================================
DEF_STD_CMD(FCCmdImportBox);

FCCmdImportBox::FCCmdImportBox()
	:FCCppCommand("Import_Box")
{
	sAppModule		= "Import";
	sGroup			= "Import";
	sMenuText		= "Box";
	sToolTipText	= "Create or change a Box feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Import_Box";
	iAccel			= 0;
}


void FCCmdImportBox::Activated(int iMsg)
{

//	DlgImportBoxImp cDlg(GetAppWnd(),"Import Box",true);
//	cDlg.exec();

}

//===========================================================================
// Import_Box
//===========================================================================
DEF_STD_CMD(FCCmdImportCut);

FCCmdImportCut::FCCmdImportCut()
	:FCCppCommand("Import_Cut")
{
	sAppModule		= "Import";
	sGroup			= "Import";
	sMenuText		= "Cut";
	sToolTipText	= "Create or change a Cut feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Import_Box";
	iAccel			= 0;
}


void FCCmdImportCut::Activated(int iMsg)
{

//	DlgImportBoxImp cDlg(GetAppWnd(),"Import Box",true);
//	cDlg.exec();

}



void CreateCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.AddCommand(new FCCmdImportTest1());
	rcCmdMgr.AddCommand(new FCCmdImportTest2());
	rcCmdMgr.AddCommand(new FCCmdImportCut());
	rcCmdMgr.AddCommand(new FCCmdImportBox());

}



