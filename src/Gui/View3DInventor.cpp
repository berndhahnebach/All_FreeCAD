/***************************************************************************
                          View3DInventor.cpp  -  description
                             -------------------
    begin                : Mon Jan 1 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2003                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2003                                                   *
 ***************************************************************************/

#include "PreCompiled.h"

#ifndef _PreComp_
# include <qvbox.h>
# include <TopoDS_Shape.hxx>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoShapeHints.h>
#endif

#include "View3DInventor.h"
#include "Document.h"
#include "../App/Label.h"

// build in Inventor
#include "Inventor/Qt/viewers/SoQtExaminerViewer.h"
#include "Inventor/OCC/SoBrepShape.h"



FCView3DInventor::FCView3DInventor( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags )
    :MDIView( pcDocument,parent, name, wflags)
{
//	_viewer = new SoQtExaminerViewer(this);
	_viewer = new View3DInventor(this);

	SetViewerDefaults();
}

FCView3DInventor::~FCView3DInventor()
{
  //delete _viewer;
	pcSepRoot->unref();

}

void FCView3DInventor::UpdatePrefs(void)
{



    // Importing the User special ++++++++++++++++++++++++++++++++++++++++++++++++
/*    SoInput in;
    SbBool ok = in.openFile(fn.latin1());
    if (!ok) throw FCException("FCView3DInventor::SetViewerDefaults() wrong file name for user special!");
    SoSeparator * pcSepUserSpecial = SoDB::readAll(&in);
	pcSepRoot->addChild(pcSepUserSpecial);
*/



}

#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>


void FCView3DInventor::SetViewerDefaults(void)
{
	// create the root group
	pcSepRoot = new SoSeparator();
	pcSepRoot->ref();

	// empty separator for user special
    SoSeparator * pcSepUserSpecial= new SoSeparator();
	pcSepRoot->addChild(pcSepUserSpecial);


	// shape aperance
	pcShapeHint = new SoShapeHints;
	pcShapeHint->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE; 

	// color management
	QColor Col1(236,212,156); // light brown
	QColor Col(128,128,128);  // grey

	QColor DifCol=Col1;
	QColor AmbCol=Col1;
	QColor SpeCol=Col;
	QColor EmCol(0,0,0);

	pcShapeMaterial = new SoMaterial;
	pcShapeMaterial->diffuseColor.setValue(((float)DifCol.red())/256,((float)DifCol.green())/256,((float)DifCol.blue())/256);
	pcShapeMaterial->ambientColor.setValue(((float)AmbCol.red())/256,((float)AmbCol.green())/256,((float)AmbCol.blue())/256);
	pcShapeMaterial->specularColor.setValue(((float)SpeCol.red())/256,((float)SpeCol.green())/256,((float)SpeCol.blue())/256);
	pcShapeMaterial->emissiveColor.setValue(((float)EmCol.red())/256,((float)EmCol.green())/256,((float)EmCol.blue())/256); 
	pcSepRoot->addChild(pcShapeMaterial);

	pcSepRoot->renderCaching = SoSeparator::ON;
	pcSepRoot->addChild(pcShapeHint);

	// create axis
	pcSepAxis = new SoSeparator();

	// x Axis
	SoMaterial *pcMat = new SoMaterial();
	pcMat->diffuseColor.setValue(1.0,0,0);
	pcSepAxis->addChild(pcMat);
	SoCylinder *pcCylinder = new SoCylinder();
	pcCylinder->height.setValue(100);
	
	SoTransform *pcTransform = new SoTransform();
	pcTransform->rotation.setValue(  SbVec3f (1,0,0), 90);
	pcSepAxis->addChild(pcTransform);

	// y axis
	pcMat = new SoMaterial();
	pcMat->diffuseColor.setValue(0,1.0,0);
	pcSepAxis->addChild(pcMat);
	pcCylinder = new SoCylinder();
	pcCylinder->height.setValue(100);

	pcSepRoot->addChild(pcSepAxis);


	// create the root for the shape
	pcSepShapeRoot = new SoSeparator();
	pcSepRoot->addChild(pcSepShapeRoot);

	_viewer->setSceneGraph(pcSepRoot);
//	_viewer->setFeedbackVisibility(true);
//	_viewer->setFeedbackSize(20);
	_viewer->setBackgroundColor(SbColor(0.2f,0.2f,0.2f));
	_viewer->viewAll();
//	_viewer->setDecoration(false);

    _viewer->show();
}


void FCView3DInventor::SetShape(void)
{
	TopoDS_Shape cShape;
    SoBrepShape cShapeConverter;


	assert(GetAppDocument());


    //TDF_Label L = GetAppDocument()->Main()->GetOCCLabel();




	// Open the argument file..
    //cShapeConverter.SetFile(fn.latin1());

	bool nurbs_enable = false;
	bool selection = false;
	bool strip_enable = false;
	bool strip_color = false;
	
	cShapeConverter.SetRenderOptions(strip_enable,strip_color,nurbs_enable,selection);
	cShapeConverter.Compute(pcSepShapeRoot);

}


void FCView3DInventor::resizeEvent ( QResizeEvent * e)
{
	MDIView::resizeEvent(e);
//	_pcFrame->resize(e->size());
}

const char *FCView3DInventor::GetName(void)
{
	return "View3DInventor";
}

bool FCView3DInventor::OnMsg(const char* pMsg)
{
	printf("Msg: %s View: %p\n",pMsg,this);

	return false;
}


void FCView3DInventor::Update(void)
{

}

/*

void MDIView::fitAll()
{
	myView->fitAll();
}


*/
void FCView3DInventor::onWindowActivated ()
{
  //myOperations->onSelectionChanged();
}

void FCView3DInventor::setCursor(const QCursor& aCursor)
{
  //_viewer->setCursor(aCursor);
}

void FCView3DInventor::dump()
{


}









	
#include "moc_View3DInventor.cpp"



