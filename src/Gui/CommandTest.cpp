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

	_putenv( "COIN_SHOW_FPS_COUNTER=1" );

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

    TDF_Label L = TDF_TagSource::NewChild(pcDoc->Main()->GetOCCLabel());

	L = L.FindChild(1);

	Handle(FCFeature) Feat = new FCFeature();
	Handle(FCParameter) Param = new FCParameter();
//	Handle(FCFunction) Func = new FCFunction();
	Feat->Set("Test");

	L.AddAttribute( Feat);
	L.AddAttribute( Param);

	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L, FCFunction::GetID());

}

bool FCCmdTest2::IsActive(void)
{
  return (GetActiveOCCDocument()!=NULL);
}



void CreateTestCommands(void)
{
	FCCommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	rcCmdMgr.AddCommand(new FCCmdTest1());
	rcCmdMgr.AddCommand(new FCCmdTest2());
}



