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
#	include <iostream.h>
#	include <stdio.h>
#	include <assert.h>
#	include <fcntl.h>
#	include <ctype.h>
#	include <typeinfo>
#	include <vector>
#	include <map>
#	include <string>
#	include <list>
#	include <set>
#	include <algorithm>
#	include <stack>
#	include <queue>
#	include <string>
#	if defined (_POSIX_C_SOURCE)
#		undef  _POSIX_C_SOURCE
#	endif // (re-)defined in pyconfig.h
#	include <Python.h>
#	include <limits.h>
#	include <algorithm>
#	include <qaction.h>
#	include <qcursor.h>
#	include <qstatusbar.h>
#	include <qfiledialog.h>
#	include <qprinter.h>
#	include <qmessagebox.h>
#	include <qthread.h>
#	include <BRepPrimAPI_MakeBox.hxx>
#	include <BRepTools.hxx>
#	include <TopoDS_Shape.hxx>
#	include <TNaming_Builder.hxx>
#	include <TDataStd_Real.hxx>
#	include <Handle_TPrsStd_AISPresentation.hxx>
#	include <TNaming_NamedShape.hxx>
#	include <TPrsStd_AISPresentation.hxx>
#	include <Inventor/nodes/SoMaterial.h>
#	include <Inventor/nodes/SoShapeHints.h>
#endif

#include "../Base/Exception.h"
#include "../Base/Interpreter.h"
#include "../App/Document.h"
#include "../App/Label.h"
#include "../App/Feature.h"
#include "../App/Function.h"
#include "../App/Parameter.h"
#include "Application.h"
#include "Document.h"
#include "Command.h"
// build in Inventor
#include "Inventor/Qt/viewers/SoQtExaminerViewer.h"
#include "Inventor/Qt/SoQt.h"
#include "Inventor/OCC/SoBrepShape.h"

#include "Widgets.h"
#include "Macro.h"




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_Test1
//===========================================================================
DEF_STD_CMD_A(FCCmdTest1);

FCCmdTest1::FCCmdTest1()
	:FCCppCommand("Std_Test1")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test1";
	sToolTipText	= "Test function 1";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool1";
	iAccel			= Qt::CTRL+Qt::Key_T;
}


void FCCmdTest1::Activated(int iMsg)
{
	//FCDocument *pcDoc = GetActiveOCCDocument();
	//if(!pcDoc) return;

	//OpenCommand("Test1 - Box");


#if 0
	// get open file name
	QString fn = FCFileDialog::getOpenFileName( QString::null, "Inventor (*.iv)", GetAppWnd() );
	if ( fn.isEmpty() ) return;

    // Open the argument file..
    SoInput in;
    SbBool ok = in.openFile(fn.latin1());
    if (!ok) { return; }

    // ..and import it.
    SoSeparator * root = SoDB::readAll(&in);
#else
	// get open file name
	QString fn = FCFileDialog::getOpenFileName( QString::null, "Inventor (*.brep)", GetAppWnd() );
	if ( fn.isEmpty() ) return;

    // Open the argument file..
    SoBrepShape in;
    in.SetFile(fn.latin1());

	SoSeparator * root = new SoSeparator();
	SoShapeHints * hints = new SoShapeHints;
	hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE; 

	// color management
	QColor Col1(236,212,156);
	QColor Col(128,128,128);

	QColor DifCol=Col1;
	QColor AmbCol=Col1;
	QColor SpeCol=Col;
	QColor EmCol(0,0,0);

	SoMaterial *aMat = new SoMaterial;
	aMat->diffuseColor.setValue(((float)DifCol.red())/256,((float)DifCol.green())/256,((float)DifCol.blue())/256);
	aMat->ambientColor.setValue(((float)AmbCol.red())/256,((float)AmbCol.green())/256,((float)AmbCol.blue())/256);
	aMat->specularColor.setValue(((float)SpeCol.red())/256,((float)SpeCol.green())/256,((float)SpeCol.blue())/256);
	aMat->emissiveColor.setValue(((float)EmCol.red())/256,((float)EmCol.green())/256,((float)EmCol.blue())/256); 
	root->addChild(aMat);

	root->renderCaching = SoSeparator::ON;
	root->addChild(hints);

	putenv( "COIN_SHOW_FPS_COUNTER=1" );

	bool nurbs_enable = false;
	bool selection = false;
	bool strip_enable = false;
	bool strip_color = false;

	in.SetRenderOptions(strip_enable,strip_color,nurbs_enable,selection);
	in.Compute(root);
	
#endif

    if (root == NULL) { return ; }
    root->ref();
  
    // Use the ExaminerViewer, for a nice interface for 3D model
    // inspection.
    SoQtExaminerViewer * viewer = new SoQtExaminerViewer();
    viewer->setSceneGraph(root);
	viewer->setFeedbackVisibility(true);
	viewer->setFeedbackSize(20);
	viewer->setBackgroundColor(SbColor(0.2f,0.2f,0.2f));

    viewer->show();
  
    // Pop up the main window.
//    SoQt::show(mainwin);
    // Loop until exit.
//   SoQt::mainLoop();
  
    // Clean up resources.
    //delete viewer;
    //root->unref();



    //CommitCommand();

	//UpdateActive();

}

bool FCCmdTest1::IsActive(void)
{
  //return (GetActiveOCCDocument()!=NULL);
  return true;
}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD_A(FCCmdTest2);

FCCmdTest2::FCCmdTest2()
	:FCCppCommand("Std_Test2")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test2";
	sToolTipText	= "Test function 2";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool2";
	iAccel			= 0;
}

#include <TFunction_Function.hxx>

void FCCmdTest2::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

	GetConsole().Log("Using Doc: %p\n",pcDoc);

    TDF_Label L = pcDoc->Main();

	L = L.FindChild(1);

//	Handle(FCFeature) Feat = new FCFeature();
	Handle(FCParameter) Param = new FCParameter();
//	Handle(FCFunction) Func = new FCFunction();
//	Feat->Set("Test");

//	L.AddAttribute( Feat);
	L.AddAttribute( Param);

	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L, FCFunction::GetID());

}

bool FCCmdTest2::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}

//===========================================================================
// Std_Test3
//===========================================================================
DEF_STD_CMD_A(FCCmdTest3);

FCCmdTest3::FCCmdTest3()
	:FCCppCommand("Std_Test3")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test3";
	sToolTipText	= "Test function 3";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool3";
	iAccel			= 0;
}

void FCCmdTest3::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

}


bool FCCmdTest3::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}

//===========================================================================
// Std_Test4
//===========================================================================

DEF_STD_CMD_A(FCCmdTest4);

FCCmdTest4::FCCmdTest4()
	:FCCppCommand("Std_Test4")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test4";
	sToolTipText	= "Test function 4";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool4";
	iAccel			= 0;
}

void FCCmdTest4::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

}


bool FCCmdTest4::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}

//===========================================================================
// Std_Test5
//===========================================================================
DEF_STD_CMD_A(FCCmdTest5);

FCCmdTest5::FCCmdTest5()
	:FCCppCommand("Std_Test5")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test5";
	sToolTipText	= "Test function 5";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool5";
	iAccel			= 0;
}

void FCCmdTest5::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

}


bool FCCmdTest5::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}


//===========================================================================
// Std_Test6
//===========================================================================
DEF_STD_CMD_A(FCCmdTest6);

FCCmdTest6::FCCmdTest6()
	:FCCppCommand("Std_Test6")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test6";
	sToolTipText	= "Test function 6";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool6";
	iAccel			= 0;
}

void FCCmdTest6::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

}


bool FCCmdTest6::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}


//===========================================================================
// Std_Test7
//===========================================================================
DEF_STD_CMD_A(FCCmdTest7);

FCCmdTest7::FCCmdTest7()
	:FCCppCommand("Std_Test7")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test7";
	sToolTipText	= "Test function 7";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool7";
	iAccel			= 0;
}

void FCCmdTest7::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

}


bool FCCmdTest7::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}


//===========================================================================
// Std_Test8
//===========================================================================
DEF_STD_CMD_A(FCCmdTest8);

FCCmdTest8::FCCmdTest8()
	:FCCppCommand("Std_Test8")
{
	sAppModule		= "";
	sGroup			= "Standard-Test";
	sMenuText		= "Test8";
	sToolTipText	= "Test function 8";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool8";
	iAccel			= 0;
}

void FCCmdTest8::Activated(int iMsg)
{

	FCDocument *pcDoc = GetActiveOCCDocument();
	if(!pcDoc) return;

}


bool FCCmdTest8::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}


void CreateTestCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.AddCommand(new FCCmdTest1());
	rcCmdMgr.AddCommand(new FCCmdTest2());
	rcCmdMgr.AddCommand(new FCCmdTest3());
	rcCmdMgr.AddCommand(new FCCmdTest4());
	rcCmdMgr.AddCommand(new FCCmdTest5());
	rcCmdMgr.AddCommand(new FCCmdTest6());
	rcCmdMgr.AddCommand(new FCCmdTest7());
	rcCmdMgr.AddCommand(new FCCmdTest8());
}



