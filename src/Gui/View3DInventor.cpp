/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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

using namespace Gui;


View3DInventor::View3DInventor( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags )
    :MDIView( pcDocument,parent, name, wflags)
{
//  _viewer = new SoQtExaminerViewer(this);
  _viewer = new MyView3DInventor(this);

  setViewerDefaults();
}

View3DInventor::~View3DInventor()
{
  //delete _viewer;
  pcSepRoot->unref();
}

void View3DInventor::updatePrefs(void)
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


void View3DInventor::setViewerDefaults(void)
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
  //  _viewer->setFeedbackVisibility(true);
  //  _viewer->setFeedbackSize(20);
  _viewer->setBackgroundColor(SbColor(0.2f,0.2f,0.2f));
  _viewer->viewAll();
//  _viewer->setDecoration(false);

  _viewer->show();
}

void View3DInventor::setShape(void)
{
  TopoDS_Shape cShape;
    SoBrepShape cShapeConverter;


  assert(getAppDocument());


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

void View3DInventor::resizeEvent ( QResizeEvent * e)
{
  MDIView::resizeEvent(e);
//  _pcFrame->resize(e->size());
}

const char *View3DInventor::getName(void)
{
  return "View3DInventor";
}

bool View3DInventor::onMsg(const char* pMsg,const char** ppReturn)
{
  printf("Msg: %s View: %p\n",pMsg,this);

  return false;
}


void View3DInventor::onUpdate(void)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::View3DInventor::onUpdate()");
#endif

}

/*

void MDIView::fitAll()
{
  myView->fitAll();
}


*/
void View3DInventor::onWindowActivated ()
{
  //myOperations->onSelectionChanged();
}

void View3DInventor::setCursor(const QCursor& aCursor)
{
  //_viewer->setCursor(aCursor);
}

void View3DInventor::dump()
{

}

#include "moc_View3DInventor.cpp"
