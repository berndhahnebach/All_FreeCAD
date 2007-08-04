/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <q3dragobject.h>
# include <qfileinfo.h>
# include <qsplitter.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
#endif

#include "SplitView3DInventor.h"
#include "View3DInventorViewer.h"
#include "Document.h"
#include "Application.h"


using namespace Gui;

SplitView3DInventor::SplitView3DInventor( int views, Gui::Document* pcDocument, QWidget* parent, Qt::WFlags wflags )
    :MDIView( pcDocument,parent, wflags)
{
  // important for highlighting 
  setMouseTracking(true);
  
  // attach Parameter Observer
  hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/View");
  hGrp->Attach(this);

  QSplitter* mainSplitter=0;

  if ( views <= 3 ) {
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    _viewer.push_back(new View3DInventorViewer(mainSplitter));
    _viewer.push_back(new View3DInventorViewer(mainSplitter));
    if (views==3)
      _viewer.push_back(new View3DInventorViewer(mainSplitter));
  } else {
    mainSplitter = new QSplitter(Qt::Vertical, this);
    QSplitter *topSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
    QSplitter *botSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
    _viewer.push_back(new View3DInventorViewer(topSplitter));
    _viewer.push_back(new View3DInventorViewer(topSplitter));
    for (int i=2;i<views;i++)
      _viewer.push_back(new View3DInventorViewer(botSplitter));
    topSplitter->setOpaqueResize( true );
    botSplitter->setOpaqueResize( true );
  }

  mainSplitter->show();
  setCentralWidget(mainSplitter);

  setViewerDefaults();
}

SplitView3DInventor::~SplitView3DInventor()
{
  hGrp->Detach(this);
  for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
    delete *it;
  }
}

View3DInventorViewer* SplitView3DInventor::getViewer(unsigned int n) const
{
  return ( _viewer.size() > n ? _viewer[n] : 0);
}

void SplitView3DInventor::setViewerDefaults(void)
{
  float stereoOffset = hGrp->GetFloat("EyeDistance",65.0);
  bool drawAxisCross = hGrp->GetBool("CornerCoordSystem",true);
  bool allowSpinning = hGrp->GetBool("UseAutoRotation",true);
  bool gradientBackG = hGrp->GetBool("Gradient",true);
  bool smoothing     = hGrp->GetBool("UseAntialiasing",false);
  bool showFPSCount  = hGrp->GetBool("ShowFPS",false);
  bool cameraType    = hGrp->GetBool("Orthographic",false);
  long col = hGrp->GetInt("BackgroundColor",0);
  float r,g,b;
  r = (col & 0xff) / 255.0;
  g = ((col >> 8) & 0xff) / 255.0;
  b = ((col >> 16) & 0xff) / 255.0;
  long col2 = hGrp->GetInt("BackgroundColor2",13467519); // default color (lila)
  long col3 = hGrp->GetInt("BackgroundColor3",16777215); // default color (white)
  float r2,g2,b2,r3,g3,b3;
  r2 = (col2 & 0xff) / 255.0; g2 = ((col2 >> 8) & 0xff) / 255.0; b2 = ((col2 >> 16) & 0xff) / 255.0;
  r3 = (col3 & 0xff) / 255.0; g3 = ((col3 >> 8) & 0xff) / 255.0; b3 = ((col3 >> 16) & 0xff) / 255.0;

  for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
    (*it)->setStereoOffset(stereoOffset);
    (*it)->bDrawAxisCross = drawAxisCross;
    (*it)->bAllowSpining =  allowSpinning;
    (*it)->setGradientBackgroud(gradientBackG);
    (*it)->setBackgroundColor(SbColor(r, g, b));
    (*it)->setGradientBackgroudColor( SbColor(r2, g2, b2), SbColor(r3, g3, b3) );
    (*it)->getGLRenderAction()->setSmoothing(smoothing);
    (*it)->setEnabledFPSCounter(showFPSCount);
    (*it)->setCameraType( (cameraType ? SoOrthographicCamera::getClassTypeId() : SoPerspectiveCamera::getClassTypeId()));
  }
}

/// Observer message from the ParameterGrp
void SplitView3DInventor::OnChange(ParameterGrp::SubjectType &rCaller,ParameterGrp::MessageType Reason)
{
  setViewerDefaults();
}

void SplitView3DInventor::onUpdate(void)
{
  update();  
}

const char *SplitView3DInventor::getName(void) const
{
  return "SplitView3DInventor";
}

bool SplitView3DInventor::onMsg(const char* pMsg, const char** ppReturn)
{
  if(strcmp("ViewFit",pMsg) == 0 ){
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it )
      (*it)->viewAll();
    return true;
  }else if(strcmp("ViewBottom",pMsg) == 0 ){
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
      SoCamera* cam = (*it)->getCamera();
      cam->orientation.setValue(-1, 0, 0, 0);
      (*it)->viewAll();
    }
    return true;
  }else if(strcmp("ViewFront",pMsg) == 0 ){
    float root = (float)(sqrt(2.0)/2.0);
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
      SoCamera* cam = (*it)->getCamera();
      cam->orientation.setValue(-root, 0, 0, -root);
      (*it)->viewAll();
    }
    return true;
  }else if(strcmp("ViewLeft",pMsg) == 0 ){
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
      SoCamera* cam = (*it)->getCamera();
      cam->orientation.setValue(0.5, 0.5, 0.5, 0.5);
      (*it)->viewAll();
    }
    return true;
  }else if(strcmp("ViewRear",pMsg) == 0 ){
    float root = (float)(sqrt(2.0)/2.0);
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
      SoCamera* cam = (*it)->getCamera();
      cam->orientation.setValue(0, root, root, 0);
      (*it)->viewAll();
    }
    return true;
  }else if(strcmp("ViewRight",pMsg) == 0 ){
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
      SoCamera* cam = (*it)->getCamera();
      cam->orientation.setValue(-0.5, 0.5, 0.5, -0.5);
      (*it)->viewAll();
    }
    return true;
  }else if(strcmp("ViewTop",pMsg) == 0 ){
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
      SoCamera* cam = (*it)->getCamera();
      cam->orientation.setValue(0, 0, 0, 1);
      (*it)->viewAll();
    }
    return true;
  }else if(strcmp("ViewAxo",pMsg) == 0 ){
    float root = (float)(sqrt(3.0)/4.0);
    for ( std::vector<View3DInventorViewer*>::iterator it = _viewer.begin(); it != _viewer.end(); ++it ) {
      SoCamera* cam = (*it)->getCamera();
      cam->orientation.setValue(-0.333333f, -0.166666f, -0.333333f, -root);
      (*it)->viewAll();
    }
    return true;
  }

  return false;
}

bool SplitView3DInventor::onHasMsg(const char* pMsg) const
{
  if(strcmp("ViewFit",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewBottom",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewFront",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewLeft",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewRear",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewRight",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewTop",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewAxo",pMsg) == 0 ){
    return true;
  }
  return false;
}

void SplitView3DInventor::setCursor(const QCursor& aCursor)
{
  //_viewer->getWidget()->setCursor(aCursor);
}
