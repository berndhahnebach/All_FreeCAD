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
//# include <qvbox.h>
//# include <TopoDS_Shape.hxx>
# include <TNaming_NamedShape.hxx>
# ifdef FC_OS_WIN32
#   include <windows.h>
# endif
# include <qapplication.h>
# include <qimage.h>
# include <GL/gl.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCone.h>
# include <Inventor/nodes/SoCube.h>
# include <Inventor/nodes/SoCylinder.h>
# include <Inventor/nodes/SoImage.h>
# include <Inventor/nodes/SoLightModel.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoRotationXYZ.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/nodes/SoTranslation.h>
#endif

#include "View3DInventorEx.h"
#include "Document.h"
#include "../App/Label.h"
#include "../App/Feature.h"
#include "../Base/Console.h"

// build in Inventor
#include "Inventor/Qt/viewers/SoQtExaminerViewer.h"
#include "Inventor/OCC/SoBrepShape.h"

#include "View3DInventorExamples.h"
#include "Tools.h"
#include "ViewProvider.h"

#include "Icons/default_background.xpm"

using namespace Gui;

View3DInventorEx::View3DInventorEx( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags )
    :MDIView( pcDocument,parent, name, wflags)
{
  pcActViewProvider = 0l;
  
  _viewer = new Gui::MyExaminerViewer(this/*,"FreeCAD.png"*/);

  setViewerDefaults();
}

View3DInventorEx::~View3DInventorEx()
{
  //delete _viewer;
  pcSepRoot->unref();

  delete _viewer;
}

void View3DInventorEx::updatePrefs(void)
{



    // Importing the User special ++++++++++++++++++++++++++++++++++++++++++++++++
/*    SoInput in;
    SbBool ok = in.openFile(fn.latin1());
    if (!ok) throw FCException("FCView3DInventor::SetViewerDefaults() wrong file name for user special!");
    SoSeparator * pcSepUserSpecial = SoDB::readAll(&in);
  pcSepRoot->addChild(pcSepUserSpecial);
*/
}


void View3DInventorEx::setViewerDefaults(void)
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
//  pcSepShapeRoot = new SoSeparator();
//  pcSepRoot->addChild(pcSepShapeRoot);

//  _viewer->setSceneGraph(pcSepRoot);


  _viewer->setFeedbackVisibility(true);
  _viewer->setFeedbackSize(15);
//  _viewer->setBackgroundColor(SbColor(0.2f,0.2f,0.2f));
  _viewer->viewAll();
  _viewer->setDecoration(false);

  _viewer->setSceneGraph(pcSepRoot);


//  _viewer->setSceneGraph(new SoCone);
//  _viewer->show();

  _viewer->show();
}

/*
void View3DInventorEx::SetShape(void)
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
*/
void View3DInventorEx::onUpdate(void)
{

  App::Feature *pcActFeature = getAppDocument()->GetActiveFeature();

  if(pcActFeature)
  {
    std::string cName = pcActFeature->Type();
    // check if still the same ViewProvider
    if(cName == cViewProviderName && pcActViewProvider)
    {
      // if the same just calculate the new representation
      SoNode* Node = pcActViewProvider->create(pcActFeature);
      _viewer->setSceneGraph(Node);
    }else{
      // if not create the new ViewProvider
      if (pcActViewProvider) 
        delete pcActViewProvider;
      pcActViewProvider = ViewProviderInventorFactory().Produce(pcActFeature->Type());
      if(pcActViewProvider)
      {
        // if succesfully created set the right name an calculate the view
        cViewProviderName = pcActFeature->Type();
        SoNode* Node = pcActViewProvider->create(pcActFeature);
        _viewer->setSceneGraph(Node);
      }else{
        Base::Console().Warning("View3DInventorEx::onUpdate() no view provider for the Feature %s found\n",pcActFeature->Type());
        cViewProviderName = "";
      }
    }

  }


/*
  TDF_Label L = getAppDocument()->GetActive();

  if(! L.IsNull())
  {
    Handle(TNaming_NamedShape) ShapeToViewName;
    if (!( L.FindAttribute(TNaming_NamedShape::GetID(),ShapeToViewName) ))
      return;

    // Now, let's get the TopoDS_Shape of these TNaming_NamedShape:
    TopoDS_Shape ShapeToView  = ShapeToViewName->Get();

    bool nurbs_enable = false;
    bool selection = true;
    bool strip_enable = false;
    bool strip_color = false;

    SoBrepShape cShapeConverter;

    cShapeConverter.SetDeflection(0.5);
    cShapeConverter.SetRenderOptions(strip_enable,strip_color,nurbs_enable,selection);
    cShapeConverter.SetShape(ShapeToView);

    SoSeparator * SepShapeRoot=new SoSeparator();
    if(cShapeConverter.Compute2(SepShapeRoot))
      _viewer->setSceneGraph(SepShapeRoot);
    else
      Base::Console().Error("View3DInventorEx::Update() Cannot compute Inventor representation for the actual shape");
  }
*/
}


void View3DInventorEx::resizeEvent ( QResizeEvent * e)
{
  MDIView::resizeEvent(e);
//  _pcFrame->resize(e->size());
}

const char *View3DInventorEx::getName(void)
{
  return "View3DInventorEx";
}

// buffer acrobatics for inventor ****************************************************
static char * buffer;
static size_t buffer_size = 0;
static std::string cReturnString;

static void *
buffer_realloc(void * bufptr, size_t size)
{
  buffer = (char *)realloc(bufptr, size);
  buffer_size = size;
  return buffer;
}

static const std::string&
buffer_writeaction(SoNode * root)
{
  SoOutput out;
  buffer = (char *)malloc(1024);
  buffer_size = 1024;
  out.setBuffer(buffer, buffer_size, buffer_realloc);

  SoWriteAction wa(&out);
  wa.apply(root);

  cReturnString =buffer;
  free(buffer);
  return cReturnString;
}

// **********************************************************************************

bool View3DInventorEx::onMsg(const char* pMsg, const char** ppReturn)
{
  if(strcmp("SetStereoOn",pMsg) == 0 ){
    _viewer->setStereoViewing(true);
    return true;
  }else if(strcmp("SetStereoOff",pMsg) == 0 ){
    _viewer->setStereoViewing(false);
    return true;
  }else if(strcmp("SetDecorationOn",pMsg) == 0 ){
    _viewer->setDecoration(true);
    return true;
  }else if(strcmp("SetDecorationOff",pMsg) == 0 ){
    _viewer->setDecoration(false);
    return true;
  }else if(strcmp("SetFeedbackOff",pMsg) == 0 ){
    _viewer->setFeedbackVisibility(true);
    return true;
  }else if(strcmp("SetFeedbackOn",pMsg) == 0 ){
    _viewer->setFeedbackVisibility(false);
    return true;
  }else if(strcmp("Example1",pMsg) == 0 ){
    SoSeparator * root = new SoSeparator;;
    Texture3D(root);
    _viewer->setSceneGraph(root);
    return true;
  }else if(strcmp("Example2",pMsg) == 0 ){
    SoSeparator * root = new SoSeparator;;
    LightManip(root);
    _viewer->setSceneGraph(root);
    return true;
  }else if(strcmp("Example3",pMsg) == 0 ){
    SoSeparator * root = new SoSeparator;;
    AnimationTexture(root);
    _viewer->setSceneGraph(root);
    return true;
  }else if(strcmp("GetCamera",pMsg) == 0 ){
    SoCamera * Cam = _viewer->getCamera();
    *ppReturn = buffer_writeaction(Cam).c_str();
    return true;
  }else if(strcmp("ViewFit",pMsg) == 0 ){
    _viewer->viewAll();
    return true;
  }
  return false;

  return false;
}


bool View3DInventorEx::onHasMsg(const char* pMsg)
{
  if(strcmp("SetStereoOn",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetStereoOff",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetStereoOff",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetDecorationOn",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetDecorationOff",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetFeedbackOff",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetFeedbackOn",pMsg) == 0 ){
    return true;
  }else if(strcmp("Example1",pMsg) == 0 ){
    return true;
  }else if(strcmp("Example2",pMsg) == 0 ){
    return true;
  }else if(strcmp("Example3",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewFit",pMsg) == 0 ){
    return true;
  }else if(strcmp("GetCamera",pMsg) == 0 ){
    return true;
  }
  return false;
}


/*

void MDIView::fitAll()
{
  myView->fitAll();
}


*/
void View3DInventorEx::onWindowActivated ()
{
  //myOperations->onSelectionChanged();
}

void View3DInventorEx::setCursor(const QCursor& aCursor)
{
  //_viewer->setCursor(aCursor);
}

void View3DInventorEx::dump()
{


}

// *************************************************************************

MyExaminerViewer::MyExaminerViewer(QWidget *parent, const char * filename)
  : SoQtExaminerViewer(parent)
{
  // Coin should not clear the pixel-buffer, so the background image
  // is not removed.
  this->setClearBeforeRender(FALSE);


  // Set up background scenegraph with image in it.

  this->bckgroundroot = new SoSeparator;
  this->bckgroundroot->ref();

  SoOrthographicCamera * cam = new SoOrthographicCamera;
  cam->position = SbVec3f(0, 0, 1);
  cam->height = 1;
  // SoImage will be at z==0.0.
  cam->nearDistance = 0.5;
  cam->farDistance = 1.5;

  this->bckgroundroot->addChild(cam);


  SoImage * img = new SoImage;
  img->vertAlignment = SoImage::HALF;
  img->horAlignment = SoImage::CENTER;

  if ( filename )
  {
    // if file specified load this file
    img->filename = filename;
  }
  else
  {
    // otherwise take the default image and scale it up to desktop size
    QImage image( default_background );

    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();
    Tools::convert( image.smoothScale(w, h), img->image );
  }

  this->bckgroundroot->addChild(img);



  // Set up foreground, overlayed scenegraph.

  this->foregroundroot = new SoSeparator;
  this->foregroundroot->ref();

  SoLightModel * lm = new SoLightModel;
  lm->model = SoLightModel::BASE_COLOR;

  SoBaseColor * bc = new SoBaseColor;
  bc->rgb = SbColor(1, 1, 0);

  cam = new SoOrthographicCamera;
  cam->position = SbVec3f(0, 0, 5);
  cam->height = 10;
  cam->nearDistance = 0;
  cam->farDistance = 10;

  const double ARROWSIZE = 2.0;

  SoTranslation * posit = new SoTranslation;
  posit->translation = SbVec3f(-2.5 * ARROWSIZE, 1.5 * ARROWSIZE, 0);

  arrowrotation = new SoRotationXYZ;
  arrowrotation->axis = SoRotationXYZ::Z;

  SoTranslation * offset = new SoTranslation;
  offset->translation = SbVec3f(ARROWSIZE/2.0, 0, 0);

  SoCube * cube = new SoCube;
  cube->width = ARROWSIZE;
  cube->height = ARROWSIZE/15.0;
/*
  this->foregroundroot->addChild(cam);
  this->foregroundroot->addChild(lm);
  this->foregroundroot->addChild(bc);
  this->foregroundroot->addChild(posit);
  this->foregroundroot->addChild(arrowrotation);
  this->foregroundroot->addChild(offset);
  this->foregroundroot->addChild(cube);
  */
}

MyExaminerViewer::~MyExaminerViewer()
{
  this->bckgroundroot->unref();
  this->foregroundroot->unref();
}

void MyExaminerViewer::actualRedraw(void)
{
  // Must set up the OpenGL viewport manually, as upon resize
  // operations, Coin won't set it up until the SoGLRenderAction is
  // applied again. And since we need to do glClear() before applying
  // the action..
  const SbViewportRegion vp = this->getViewportRegion();
  SbVec2s origin = vp.getViewportOriginPixels();
  SbVec2s size = vp.getViewportSizePixels();
  glViewport(origin[0], origin[1], size[0], size[1]);

  const SbColor col = this->getBackgroundColor();
  glClearColor(col[0], col[1], col[2], 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render our scenegraph with the image.
  SoGLRenderAction * glra = this->getGLRenderAction();
  glra->apply(this->bckgroundroot);


  // Render normal scenegraph.
  SoQtExaminerViewer::actualRedraw();


  // Increase arrow angle with 1/1000 ° every frame.
  arrowrotation->angle = arrowrotation->angle.getValue() + (0.001 / M_PI * 180);
  // Render overlay front scenegraph.
  glClear(GL_DEPTH_BUFFER_BIT);
  glra->apply(this->foregroundroot);
}

#include "moc_View3DInventorEx.cpp" 
