/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
#	include <qaction.h>
# include <qlineedit.h>
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
#include "../../../Gui/FileDialog.h"


#include "../../../Gui/Icons/images.cpp"

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>

#include "DlgPartBoxImp.h"
#include "DlgPartImportStepImp.h"
#include "DlgPartImportIgesImp.h"

using Gui::FileDialog;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_Test1
//===========================================================================
DEF_STD_CMD(FCCmdPartTest1);

FCCmdPartTest1::FCCmdPartTest1()
	:CppCommand("Part_Test1")
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


void FCCmdPartTest1::activated(int iMsg)
{

	DlgPartBoxImp cDlg(getAppWnd(),"Part Box",true);
	cDlg.exec();


}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD(FCCmdPartTest2);

FCCmdPartTest2::FCCmdPartTest2()
	:CppCommand("Part_Test2")
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


void FCCmdPartTest2::activated(int iMsg)
{

  App::Document *pcDoc = getActiveOCCDocument();
	if(!pcDoc) return;


	openCommand("Test Load Part");

	Base::Console().Log("Using Doc: %p\n",pcDoc);

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main());

	L = L.FindChild(1);

	BRep_Builder aBuilder;
	TopoDS_Shape ResultShape;

	QString fn = FileDialog::getOpenFileName( QString::null, "BREP (*.brep *.rle)", getAppWnd() );
	if ( fn.isEmpty() )
	{
		abortCommand();
		return;
	}
 
	try{
	  BRepTools::Read(ResultShape,(const Standard_CString)fn.latin1(),aBuilder);
	}
	// Boeser Fehler ;-)
	catch(...){
		abortCommand();
    throw new Base::Exception("Error loading BREP file");
	}

	TNaming_Builder B(L);
	B.Generated(ResultShape);

	Handle(TPrsStd_AISPresentation) hcPrs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID());
	// Display it
	hcPrs->Display(1);

	updateActive();
}

//===========================================================================
// Part_NewDoc
//===========================================================================
DEF_STD_CMD(CmdPartNewDoc);

CmdPartNewDoc::CmdPartNewDoc()
	:CppCommand("Part_NewDoc")
{
	sAppModule		= "Part";
	sGroup			  = "Part";
	sMenuText		  = "New document";
	sToolTipText	= "Create a empty part document";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "New";
	iAccel			  = 0;
}

void CmdPartNewDoc::activated(int iMsg)
{
	doCommand(Doc,"d = App.DocNew()");

  updateActive();
}


//===========================================================================
// Part_Box
//===========================================================================
DEF_STD_CMD_A(FCCmdPartBox);

FCCmdPartBox::FCCmdPartBox()
	:CppCommand("Part_Box")
{
	sAppModule		= "Part";
	sGroup			  = "Part";
	sMenuText		  = "Box";
	sToolTipText	= "Create or change a Box feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Part_Box";
	iAccel			  = 0;
}


void FCCmdPartBox::activated(int iMsg)
{

	DlgPartBoxImp cDlg(getAppWnd(),"Part Box",true);
	if ( cDlg.exec()== QDialog::Accepted )
  {
    openCommand("Part Box Create");
	  doCommand(Doc,"f = App.DocGet().AddFeature(\"PartBox\")");
	  doCommand(Doc,"f.x = %f",cDlg.XLineEdit->text().toFloat());
	  doCommand(Doc,"f.y = %f",cDlg.YLineEdit->text().toFloat());
	  doCommand(Doc,"f.z = %f",cDlg.ZLineEdit->text().toFloat());
	  doCommand(Doc,"f.l = %f",cDlg.ULineEdit->text().toFloat());
	  doCommand(Doc,"f.w = %f",cDlg.VLineEdit->text().toFloat());
	  doCommand(Doc,"f.h = %f",cDlg.WLineEdit->text().toFloat());
//	  DoCommand(Doc,"App.DocGet().Update()");
    commitCommand();
  
    updateActive();


  }

}

bool FCCmdPartBox::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;

}

//===========================================================================
// Part_Box2
//===========================================================================
DEF_STD_CMD_A(FCCmdPartBox2);

FCCmdPartBox2::FCCmdPartBox2()
	:CppCommand("Part_Box2")
{
	sAppModule		= "Part";
	sGroup			  = "Part";
	sMenuText		  = "Box2";
	sToolTipText	= "Create or change a Box feature without Dialog";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Part_Box";
	iAccel			  = 0;
}


void FCCmdPartBox2::activated(int iMsg)
{
  openCommand("PartBox Create");

	doCommand(Doc,"f = App.DocGet().AddFeature(\"PartBox\")");
	doCommand(Doc,"f.x = 0.0");
	doCommand(Doc,"f.y = 0.0");
	doCommand(Doc,"f.z = 0.0");
	doCommand(Doc,"f.l = 100.0");
	doCommand(Doc,"f.w = 100.0");
	doCommand(Doc,"f.h = 100.0");
 // DoCommand(Doc,"App.DocGet().Update()");

  updateActive();

  commitCommand();
}

bool FCCmdPartBox2::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;

}

//===========================================================================
// Part_Cut
//===========================================================================
DEF_STD_CMD(FCCmdPartCut);

FCCmdPartCut::FCCmdPartCut()
	:CppCommand("Part_Cut")
{
	sAppModule    = "Part";
	sGroup			  = "Part";
	sMenuText		  = "Cut";
	sToolTipText  = "Create or change a Cut feature";
	sWhatsThis    = sToolTipText;
	sStatusTip    = sToolTipText;
	sPixmap       = "Part_Box";
	iAccel        = 0;
}


void FCCmdPartCut::activated(int iMsg)
{

	DlgPartBoxImp cDlg(getAppWnd(),"Part Box",true);
	cDlg.exec();

}

//===========================================================================
// PartImportStep
//===========================================================================
DEF_STD_CMD_A(PartImportStep);

PartImportStep::PartImportStep()
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


void PartImportStep::activated(int iMsg)
{


  DlgPartImportStepImp cDlg(getAppWnd(),"Part import STEP",true);
  if ( cDlg.exec() == QDialog::Accepted )
  {
    openCommand("Part ImportSTEP Create");
	  doCommand(Doc,"f = App.DocGet().AddFeature(\"PartImportStep\")");
	  doCommand(Doc,"f.FileName = \"%s\"",cDlg.FileName->text().ascii());
//	  DoCommand(Doc,"App.DocGet().Update()");
    commitCommand();
  
    updateActive();


  }
}

bool PartImportStep::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;

}


//===========================================================================
// PartImportIges
//===========================================================================
DEF_STD_CMD_A(PartImportIges);

PartImportIges::PartImportIges()
	:CppCommand("Part_ImportIges")
{
	sAppModule		= "Part";
	sGroup			= "Part";
	sMenuText		= "Import IGES";
	sToolTipText	= "Create or change a Import IGES feature";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Save";
	iAccel			= 0;
}


void PartImportIges::activated(int iMsg)
{

  DlgPartImportIgesImp cDlg(getAppWnd(),"Part import IGES",true);
  if ( cDlg.exec() == QDialog::Accepted )
  {
    openCommand("Part ImportIGES Create");
	  doCommand(Doc,"f = App.DocGet().AddFeature(\"PartImportIges\")");
	  doCommand(Doc,"f.FileName = \"%s\"",cDlg.FileName->text().ascii());
	  doCommand(Doc,"App.DocGet().Update()");
    commitCommand();
  
    updateActive();
  }


}

bool PartImportIges::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;

}



void CreateCommands(void)
{
  Gui::CommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.addCommand(new FCCmdPartTest1());
	rcCmdMgr.addCommand(new FCCmdPartTest2());
	rcCmdMgr.addCommand(new FCCmdPartCut());
	rcCmdMgr.addCommand(new FCCmdPartBox());
	rcCmdMgr.addCommand(new FCCmdPartBox2());
	rcCmdMgr.addCommand(new PartImportIges());
	rcCmdMgr.addCommand(new PartImportStep());
	rcCmdMgr.addCommand(new CmdPartNewDoc());

}



