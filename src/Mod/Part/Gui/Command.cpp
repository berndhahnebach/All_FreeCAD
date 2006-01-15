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
#	include <TNaming_NamedShape.hxx>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/FileDialog.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>

#include "DlgPartBoxImp.h"
#include "DlgPartImportStepImp.h"
#include "DlgPartImportIgesImp.h"

using Gui::FileDialog;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Part_PickCurveNet
//===========================================================================
DEF_STD_CMD(CmdPartPickCurveNet);

CmdPartPickCurveNet::CmdPartPickCurveNet()
	:Command("Part_PickCurveNet")
{
	sAppModule		= "Part";
	sGroup			  = "Part";
	sMenuText		  = "Pick a cruve network";
	sToolTipText	= "Pick a cruve network";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Test1";
	iAccel			  = 0;
}


void CmdPartPickCurveNet::activated(int iMsg)
{

//  PartGui::DlgPartBoxImp cDlg(getAppWnd(),"Part Box",true);
//	cDlg.exec();


}
//===========================================================================
// Std_Test1
//===========================================================================
DEF_STD_CMD(FCCmdPartTest1);

FCCmdPartTest1::FCCmdPartTest1()
	:Command("Part_Test1")
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

  PartGui::DlgPartBoxImp cDlg(Gui::getMainWindow(),"Part Box",true);
	cDlg.exec();


}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD(FCCmdPartTest2);

FCCmdPartTest2::FCCmdPartTest2()
	:Command("Part_Test2")
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

  App::Document *pcDoc = getDocument();
	if(!pcDoc) return;



	updateActive();
}

//===========================================================================
// Part_NewDoc
//===========================================================================
DEF_STD_CMD(CmdPartNewDoc);

CmdPartNewDoc::CmdPartNewDoc()
	:Command("Part_NewDoc")
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
	doCommand(Doc,"d = App.New()");

  updateActive();
}


//===========================================================================
// Part_Box
//===========================================================================
DEF_STD_CMD_A(FCCmdPartBox);

FCCmdPartBox::FCCmdPartBox()
	:Command("Part_Box")
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

  PartGui::DlgPartBoxImp cDlg(Gui::getMainWindow(),"Part Box",true);
	if ( cDlg.exec()== QDialog::Accepted )
  {
    openCommand("Part Box Create");
	  doCommand(Doc,"f = App.document().AddFeature(\"PartBox\",\"PartBox\")");
	  doCommand(Doc,"f.x = %f",cDlg.XLineEdit->text().toFloat());
	  doCommand(Doc,"f.y = %f",cDlg.YLineEdit->text().toFloat());
	  doCommand(Doc,"f.z = %f",cDlg.ZLineEdit->text().toFloat());
	  doCommand(Doc,"f.l = %f",cDlg.ULineEdit->text().toFloat());
	  doCommand(Doc,"f.w = %f",cDlg.VLineEdit->text().toFloat());
	  doCommand(Doc,"f.h = %f",cDlg.WLineEdit->text().toFloat());
    commitCommand();
  
    updateActive();


  }

}

bool FCCmdPartBox::isActive(void)
{
	if( getActiveGuiDocument() )
		return true;
	else
		return false;

}

//===========================================================================
// Part_Box2
//===========================================================================
DEF_STD_CMD_A(FCCmdPartBox2);

FCCmdPartBox2::FCCmdPartBox2()
	:Command("Part_Box2")
{
	sAppModule		= "Part";
	sGroup			  = "Part";
	sMenuText		  = "Box fix 1";
	sToolTipText	= "Create a Box feature without Dialog";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Part_Box";
	iAccel			  = 0;
}


void FCCmdPartBox2::activated(int iMsg)
{
  openCommand("PartBox Create");

	doCommand(Doc,"f = App.document().AddFeature(\"PartBox\",\"PartBox\")");
	doCommand(Doc,"f.x = 0.0");
	doCommand(Doc,"f.y = 0.0");
	doCommand(Doc,"f.z = 0.0");
	doCommand(Doc,"f.l = 100.0");
	doCommand(Doc,"f.w = 100.0");
	doCommand(Doc,"f.h = 100.0");

  updateActive();

  commitCommand();
}

bool FCCmdPartBox2::isActive(void)
{
	if( getActiveGuiDocument() )
		return true;
	else
		return false;

}
//===========================================================================
// Part_Box2
//===========================================================================
DEF_STD_CMD_A(FCCmdPartBox3);

FCCmdPartBox3::FCCmdPartBox3()
	:Command("Part_Box3")
{
	sAppModule		= "Part";
	sGroup			  = "Part";
	sMenuText		  = "Box fix 2";
	sToolTipText	= "Create a Box feature without Dialog";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Part_Box";
	iAccel			  = 0;
}


void FCCmdPartBox3::activated(int iMsg)
{
  openCommand("PartBox Create");

	doCommand(Doc,"f = App.document().AddFeature(\"PartBox\",\"PartBox\")");
	doCommand(Doc,"f.x = 50.0");
	doCommand(Doc,"f.y = 50.0");
	doCommand(Doc,"f.z = 50.0");
	doCommand(Doc,"f.l = 100.0");
	doCommand(Doc,"f.w = 100.0");
	doCommand(Doc,"f.h = 100.0");
 
  updateActive();

  commitCommand();
}

bool FCCmdPartBox3::isActive(void)
{
	if( getActiveGuiDocument() )
		return true;
	else
		return false;

}

//===========================================================================
// Part_Cut
//===========================================================================
DEF_STD_CMD(FCCmdPartCut);

FCCmdPartCut::FCCmdPartCut()
	:Command("Part_Cut")
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

  unsigned int n = getSelection().getNbrOfType("Part");

  if(n != 2)
  {
    Base::Console().Warning("Pleas select two shapes (Part Feature)");
    return;
  }

  string FeatName = getUniqueFeatureName("Cut");

  vector<Gui::SelectionSingelton::SelObj> Sel = getSelection().getSelection();

  openCommand("Part Cut");
	doCommand(Doc,"App.document().AddFeature(\"PartCut\",\"%s\")",FeatName.c_str());
	doCommand(Doc,"App.document().%s.Base = App.document().%s",FeatName.c_str(),Sel[0].FeatName);
	doCommand(Doc,"App.document().%s.Tool = App.document().%s",FeatName.c_str(),Sel[1].FeatName);
  updateActive();
  commitCommand();
  

}

//===========================================================================
// PartImportStep
//===========================================================================
DEF_STD_CMD_A(PartImportStep);

PartImportStep::PartImportStep()
	:Command("Part_ImportStep")
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


//  DlgPartImportStepImp cDlg(getAppWnd(),"Part import STEP",true);
//  if ( cDlg.exec() == QDialog::Accepted )

  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "STEP (*.stp *.step);;All Files (*.*)", Gui::getMainWindow() );
	if (! fn.isEmpty() )
  {
    openCommand("Part ImportSTEP Create");
	  doCommand(Doc,"f = App.document().AddFeature(\"PartImportStep\",\"PartImportStep\")");
	  doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();
  
    updateActive();


  }
}

bool PartImportStep::isActive(void)
{
	if( getActiveGuiDocument() )
		return true;
	else
		return false;

}


//===========================================================================
// PartImportIges
//===========================================================================
DEF_STD_CMD_A(PartImportIges);

PartImportIges::PartImportIges()
	:Command("Part_ImportIges")
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

//  DlgPartImportIgesImp cDlg(getAppWnd(),"Part import IGES",true);
//  if ( cDlg.exec() == QDialog::Accepted )

  
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "IGES (*.igs *.iges);;All Files (*.*)", Gui::getMainWindow() );
	if (! fn.isEmpty() )
  {
    openCommand("Part ImportIGES Create");
	  doCommand(Doc,"f = App.document().AddFeature(\"PartImportIges\",\"PartImportIges\")");
	  doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();
  
    updateActive();
  }


}

bool PartImportIges::isActive(void)
{
	if( getActiveGuiDocument() )
		return true;
	else
		return false;

}

//===========================================================================
// PartImportBrep
//===========================================================================
DEF_STD_CMD_A(PartImportBrep);

PartImportBrep::PartImportBrep()
	:Command("Part_ImportBrep")
{
	sAppModule		= "Part";
	sGroup			= "Part";
	sMenuText		= "Import BREP";
	sToolTipText	= "Import BREP file format";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Save";
	iAccel			= 0;
}


void PartImportBrep::activated(int iMsg)
{

//  DlgPartImportIgesImp cDlg(getAppWnd(),"Part import IGES",true);
//  if ( cDlg.exec() == QDialog::Accepted )

  
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "BREP (*.brp *.brep *.BREP);;All Files (*.*)", Gui::getMainWindow() );
	if (! fn.isEmpty() )
  {
    openCommand("Part ImportIGES Create");
	  
    doCommand(Doc,"f = App.document().AddFeature(\"PartImportBrep\",\"PartImportBrep\")");
	  doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());

    commitCommand();
  
    updateActive();
  }


}

bool PartImportBrep::isActive(void)
{
	if( getActiveGuiDocument() )
		return true;
	else
		return false;

}

//===========================================================================
// PartImportCurveNet
//===========================================================================
DEF_STD_CMD_A(PartImportCurveNet);

PartImportCurveNet::PartImportCurveNet()
	:Command("Part_ImportCurveNet")
{
	sAppModule		= "Part";
	sGroup			= "Part";
	sMenuText		= "Import a curve network";
	sToolTipText	= "Import a curve network";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Save";
	iAccel			= 0;
}


void PartImportCurveNet::activated(int iMsg)
{


  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "All CAD (*.stp *. step *.igs *.iges *.brp *.brep );;STEP (*.stp *. step);;IGES (*.igs *.iges);;BREP (*.brp *.brep );;All Files (*.*)", 
                                                 Gui::getMainWindow() );
	if (! fn.isEmpty() )
  {
    openCommand("Part Import Curve Net");
	  
    doCommand(Doc,"f = App.document().AddFeature(\"PartCurveNet\",\"PartCurveNet\")");
	  doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());

    commitCommand();
  
    updateActive();
  }


}

bool PartImportCurveNet::isActive(void)
{
	if( getActiveGuiDocument() )
		return true;
	else
		return false;

}



void CreatePartCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	rcCmdMgr.addCommand(new FCCmdPartTest1());
	rcCmdMgr.addCommand(new FCCmdPartTest2());

  rcCmdMgr.addCommand(new FCCmdPartCut());
	rcCmdMgr.addCommand(new FCCmdPartBox());
	rcCmdMgr.addCommand(new FCCmdPartBox2());
	rcCmdMgr.addCommand(new FCCmdPartBox3());

  rcCmdMgr.addCommand(new PartImportIges());
	rcCmdMgr.addCommand(new PartImportStep());
	rcCmdMgr.addCommand(new PartImportBrep());
	rcCmdMgr.addCommand(new PartImportCurveNet());
	rcCmdMgr.addCommand(new CmdPartPickCurveNet());

  rcCmdMgr.addCommand(new CmdPartNewDoc());

}



