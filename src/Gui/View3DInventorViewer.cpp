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
# include <float.h>
# include <qapplication.h>
# include <qcursor.h>
# include <qfile.h>
# include <qfileinfo.h>
# include <qimage.h>
# include <qmessagebox.h>
# include <qpainter.h>
# include <qpen.h>
# include <qpopupmenu.h>
# ifdef FC_OS_WIN32
#  include <windows.h>
# endif
# include <GL/gl.h>
# include <Inventor/SbBox.h>
# include <Inventor/actions/SoHandleEventAction.h> 
# include <Inventor/actions/SoToVRML2Action.h>
# include <Inventor/actions/SoWriteAction.h>
# include <Inventor/manips/SoClipPlaneManip.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCallback.h> 
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoCube.h>
# include <Inventor/nodes/SoDirectionalLight.h>
# include <Inventor/nodes/SoEventCallback.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoImage.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoLightModel.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
# include <Inventor/nodes/SoRotationXYZ.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoSwitch.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoSelection.h>
# include <Inventor/actions/SoBoxHighlightRenderAction.h>
# include <Inventor/events/SoEvent.h>
# include <Inventor/events/SoKeyboardEvent.h>
# include <Inventor/events/SoLocation2Event.h>
# include <Inventor/events/SoMotion3Event.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/actions/SoRayPickAction.h>
# include <Inventor/projectors/SbSphereSheetProjector.h>
# include <Inventor/SoOffscreenRenderer.h>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/VRMLnodes/SoVRMLGroup.h>
#endif

#include <strstream>
#include <Base/Console.h>
#include <Base/Sequencer.h>
#include <Base/gzstream.h>

#include "View3DInventorViewer.h"
#include "Tools.h"
#include "SoFCBackgroundGradient.h"
#include "SoFCColorBar.h"
#include "SoFCColorLegend.h"
#include "SoFCColorGradient.h"
#include "SoFCOffscreenRenderer.h"
#include "SoFCSelection.h"
#include "SoFCInteractiveElement.h"
#include "Selection.h"
#include "SoFCSelectionAction.h"
#include "MainWindow.h"
#include "MenuManager.h"
#include "Application.h"

#include "ViewProvider.h"
// build in Inventor

//#define FC_LOGGING_CB

using namespace Gui;

/** \defgroup View3D 3d Viewer
 *
 * The 3D Viewer is the one of the major components in a CAD/CAE system.
 * Therfore a overview and some remarks to the FreeCAD 3D viewing system.
 * 
 * \section overview Overview
 */

// *************************************************************************

SOQT_OBJECT_ABSTRACT_SOURCE(View3DInventorViewer);

// *************************************************************************



void View3DInventorViewer::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason)
{
  SoFCSelectionAction cAct(Reason);

  if(Reason.Type == SelectionChanges::AddSelection || Reason.Type == SelectionChanges::RmvSelection || Reason.Type == SelectionChanges::ClearSelection)
    cAct.apply(pcViewProviderRoot);
}



/// adds an ViewProvider to the view, e.g. from a feature
void View3DInventorViewer::addViewProvider(ViewProvider* pcProvider)
{
  pcViewProviderRoot->addChild(pcProvider->getRoot());

  SoSeparator* fore = pcProvider->getFrontRoot();
  if ( fore ) foregroundroot->addChild( fore );
  SoSeparator* back = pcProvider->getBackRoot ();
  if ( back ) backgroundroot->addChild( back );
  
  _ViewProviderSet.insert(pcProvider);

  //sizeChanged( getSize() );
}

void View3DInventorViewer::removeViewProvider(ViewProvider* pcProvider)
{
  pcViewProviderRoot->removeChild(pcProvider->getRoot());
 
  SoSeparator* fore = pcProvider->getFrontRoot();
  if ( fore ) foregroundroot->removeChild( fore );
  SoSeparator* back = pcProvider->getBackRoot ();
  if ( back ) backgroundroot->removeChild( back );
  
  _ViewProviderSet.erase(pcProvider);
}

View3DInventorViewer::View3DInventorViewer (QWidget *parent, const char *name, SbBool embed, Type type, SbBool build) 
  :inherited (parent, name, embed, type, build), MenuEnabled(TRUE)
{
  Gui::Selection().Attach(this);

  // Coin should not clear the pixel-buffer, so the background image
  // is not removed.
  this->setClearBeforeRender(FALSE);
  
  // seting up the defaults for the spin roatation
  prevRedrawTime = SbTime::getTimeOfDay();
  spinanimatingallowed = TRUE;
  spinsamplecounter = 0;
  spinincrement = SbRotation::identity();

  spinprojector = new SbSphereSheetProjector(SbSphere(SbVec3f(0, 0, 0), 0.8f));
  SbViewVolume volume;
  volume.ortho(-1, 1, -1, 1, -1, 1);
  spinprojector->setViewVolume(volume);

  _bSpining = false;
  _bRejectSelection = false;

  axiscrossEnabled = true;
  axiscrossSize = 10;

  spinRotation.setValue(SbVec3f(0, 0, -1), 0);

  log.size = 16;
  log.position = new SbVec2s [ 16 ];
  log.time = new SbTime [ 16 ];
  log.historysize = 0;

  SoOrthographicCamera * cam = new SoOrthographicCamera;
  cam->position = SbVec3f(0, 0, 1);
  cam->height = 1;
  // SoImage will be at z==0.0.
  cam->nearDistance = 0.5;
  cam->farDistance = 1.5;

  // Set up background scenegraph with image in it.
  backgroundroot = new SoSeparator;
  backgroundroot->ref();
  this->backgroundroot->addChild(cam);

  // Background stuff
  pcBackGround = new SoFCBackgroundGradient;
  pcBackGround->ref();

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

  bDrawAxisCross = true;
  bAllowSpining  = true;

  this->foregroundroot->addChild(cam);
  this->foregroundroot->addChild(lm);
  this->foregroundroot->addChild(bc);

  // set the ViewProvider root
  /*
  pcSelection        = new SoSelection();
  pcSelection->addFinishCallback(View3DInventorViewer::sFinishSelectionCallback,this);
  pcSelection->addSelectionCallback( View3DInventorViewer::sMadeSelection, this );
  pcSelection->addDeselectionCallback( View3DInventorViewer::sUnmadeSelection, this );
*/
  pcViewProviderRoot = new SoSeparator();

  // increase refcount before passing it to setScenegraph(), to avoid
  // premature destruction
  pcViewProviderRoot->ref();
  // is not realy working with Coin3D. 
//  redrawOverlayOnSelectionChange(pcSelection);
  setSceneGraph(pcViewProviderRoot);

  // This is a callback node that logs all action that traverse the Inventor tree.
#if defined (FC_DEBUG) && defined(FC_LOGGING_CB)
  SoCallback * cb = new SoCallback;	 	
  cb->setCallback(interactionLoggerCB, this);
  pcViewProviderRoot->addChild(cb);
#endif


  // set the transperency and antialiasing settings
//  getGLRenderAction()->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  getGLRenderAction()->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND);
//  getGLRenderAction()->setSmoothing(true);

  // Settings
  setSeekTime(0.5);
  if ( isSeekValuePercentage() == false )
    setSeekValueAsPercentage(true);
  setSeekDistance(50);

  setBackgroundColor(SbColor(0.1f, 0.1f, 0.1f));
  setGradientBackgroud(true);

  // set some callback functions for user interaction
  addStartCallback(interactionStartCB);
  addFinishCallback(interactionFinishCB);
}

void View3DInventorViewer::setGradientBackgroud(bool b)
{
  if(b && backgroundroot->findChild(pcBackGround) == -1)
    backgroundroot->addChild( pcBackGround );
  else if(!b && backgroundroot->findChild(pcBackGround) != -1)
    backgroundroot->removeChild( pcBackGround );
  //sizeChanged(getSize());
}

void View3DInventorViewer::setGradientBackgroudColor( const SbColor& fromColor, const SbColor& toColor )
{
  pcBackGround->setColorGradient( fromColor, toColor );
}

void View3DInventorViewer::setEnabledFPSCounter(bool b)
{
#if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN)
  setenv("COIN_SHOW_FPS_COUNTER", (b?"1":"0"), 1);
#else
  b ? _putenv ("COIN_SHOW_FPS_COUNTER=1") : _putenv ("COIN_SHOW_FPS_COUNTER=0");
#endif
}

void View3DInventorViewer::stopSpinning()
{
  _bSpining = false;
}

View3DInventorViewer::~View3DInventorViewer()
{
  this->backgroundroot->unref();
  this->backgroundroot = 0;
  this->foregroundroot->unref();
  this->foregroundroot = 0;
  this->pcBackGround->unref();
  this->pcBackGround = 0;

  setSceneGraph(0);
  this->pcViewProviderRoot->unref();
  this->pcViewProviderRoot = 0;

  getMainWindow()->setPaneText(2, "");

  Gui::Selection().Detach(this);
}


void View3DInventorViewer::clearBuffer(void * userdata, SoAction * action)
{
  if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
    // do stuff specific for GL rendering here.
    glClear(GL_DEPTH_BUFFER_BIT);
  }
}

QImage View3DInventorViewer::makeScreenShot( int w, int h, float r, int c, const QColor& col ) const
{
  // if no valid color use the current background
  bool useBackground = !col.isValid();
  SbViewportRegion vp(getViewportRegion());
  vp.setWindowSize( (short)w, (short)h );
  vp.setPixelsPerInch( r );

  SoFCOffscreenRenderer renderer(vp);
  // if we use transparency then we must not set a background color
  renderer.setComponents(SoOffscreenRenderer::Components(c));
  if ( c != (int)SoOffscreenRenderer::RGB_TRANSPARENCY && c != (int)SoOffscreenRenderer::LUMINANCE_TRANSPARENCY )
  {
    if ( useBackground )
      renderer.setBackgroundColor(getBackgroundColor());
    else
      renderer.setBackgroundColor( SbColor((float)col.red()/255.0f, (float)col.green()/255.0f, (float)col.blue()/255.0f) );
  }
  else // we want a transparent background
    useBackground = false;

  SoCallback* cb = 0;
  if ( useBackground )
  {
    cb = new SoCallback;
    cb->setCallback(clearBuffer);
  }

  SoSeparator* root = new SoSeparator;
  root->ref();

  SoCamera* camera = getCamera();
  if ( useBackground )
  {
    root->addChild(backgroundroot);
    root->addChild(cb);
  }
  root->addChild(getHeadlight());
  root->addChild(camera);
  root->addChild(pcViewProviderRoot);
  if ( useBackground )
    root->addChild(cb);
  root->addChild(foregroundroot);

  QImage img;
  renderer.render( root );
  renderer.writeToImage(img);
  root->unref();

  return img;
}

bool View3DInventorViewer::makeScreenShot( const SbString& filename, const SbName& filetypeextension, int w, int h, float r, int c, const QColor& col ) const
{
  // if no valid color use the current background
  bool useBackground = !col.isValid();
  SbViewportRegion vp(getViewportRegion());
  vp.setWindowSize( (short)w, (short)h );
  vp.setPixelsPerInch( r );

  SoFCOffscreenRenderer renderer(vp);

  // if we use transparency then we must not set a background color
  renderer.setComponents(SoOffscreenRenderer::Components(c));
  if ( c != (int)SoOffscreenRenderer::RGB_TRANSPARENCY && c != (int)SoOffscreenRenderer::LUMINANCE_TRANSPARENCY )
  {
    if ( useBackground )
      renderer.setBackgroundColor(getBackgroundColor());
    else
      renderer.setBackgroundColor( SbColor((float)col.red()/255.0f, (float)col.green()/255.0f, (float)col.blue()/255.0f) );
  }
  else // we want a transparent background
    useBackground = false;

  SoCallback* cb = 0;
  if ( useBackground )
  {
    cb = new SoCallback;
    cb->setCallback(clearBuffer);
  }

  SoSeparator* root = new SoSeparator;
  root->ref();

  SoCamera* camera = getCamera();
  if ( useBackground )
  {
    root->addChild(backgroundroot);
    root->addChild(cb);
  }
  root->addChild(getHeadlight());
  root->addChild(camera);
  root->addChild(pcViewProviderRoot);
  if ( useBackground )
    root->addChild(cb);
  root->addChild(foregroundroot);

  renderer.render( root );
  bool ok = renderer.writeToImageFile( filename, filetypeextension );
  root->unref();

  return ok;
}

void View3DInventorViewer::makeScreenShot( const char* filename, int w, int h, int eBackgroundType, const char *comment ) const
{
  // if no valid color use the current background
  bool useBackground = false;
  SbViewportRegion vp(getViewportRegion());
  if(w>0 && h>0)
    vp.setWindowSize( (short)w, (short)h );

  SoFCOffscreenRenderer renderer(vp);
  SoCallback* cb = 0;

  // if we use transparency then we must not set a background color
  switch(eBackgroundType){
    case Current:
      useBackground = true;
      cb = new SoCallback;
      cb->setCallback(clearBuffer);
      break;
    case White:
      renderer.setBackgroundColor( SbColor(1.0, 1.0, 1.0) );
      break;
    case Black:
      renderer.setBackgroundColor( SbColor(0.0, 0.0, 0.0) );
      break;
    case Transparent:
      renderer.setComponents(SoOffscreenRenderer::RGB_TRANSPARENCY );
      break;
    default:
      throw Base::Exception("View3DInventorViewer::makeScreenShot(): Unknown parameter");
  }


  SoSeparator* root = new SoSeparator;
  root->ref();

  SoCamera* camera = getCamera();
  if ( useBackground )
  {
    root->addChild(backgroundroot);
    root->addChild(cb);
  }
  root->addChild(getHeadlight());
  root->addChild(camera);
  root->addChild(pcViewProviderRoot);
  if ( useBackground )
    root->addChild(cb);
  root->addChild(foregroundroot);

  // render the scene
  renderer.render( root );

  // set matrix for miba
  renderer._Matrix = camera->getViewVolume().getMatrix();

  //bool ok = renderer.writeToImageFile( filename, filetypeextension );
  renderer.writeToImageFile( filename, comment);

  root->unref();

}

bool View3DInventorViewer::dumpToFile( const char* filename, bool binary ) const
{
  bool ret = false;
  SoWriteAction wa;
  SoOutput* out = wa.getOutput();
  QFile::remove( filename );
  QFileInfo fi( filename );

  // Write VRML V2.0
  if ( fi.extension() == "wrl" || fi.extension() == "vrml" || fi.extension() == "wrz" ) {
    // If 'wrz' is set then force compression
    if ( fi.extension() == "wrz" )
      binary = true;
    
    SoToVRML2Action tovrml2;
    tovrml2.apply(pcViewProviderRoot);
    SoVRMLGroup *vrmlRoot = tovrml2.getVRML2SceneGraph();
    vrmlRoot->ref();

    if ( out->openFile( filename ) == TRUE )
    {
      out->setHeaderString("#VRML V2.0 utf8");
      wa.apply(vrmlRoot);
      vrmlRoot->unref(); // release the memory as soon as possible
      out->closeFile();

      // We want to write compressed VRML but Coin 2.4.3 doesn't do it even though
      // SoOutput::getAvailableCompressionMethods() delivers a string list that
      // contains 'GZIP'. setCompression() was called directly after opening the file, 
      // returned TRUE and no error message appeared but anyway it didn't work.
      // Strange is that reading GZIPped VRML files works.
      // So, we do the compression on our own.
      if ( binary )
      {
        std::ifstream file( filename, std::ios::in | std::ios::binary );
        if (file){
          unsigned long ulSize = 0; 
          std::streambuf* buf = file.rdbuf();
          if ( buf ) {
            unsigned long ulCurr;
            ulCurr = buf->pubseekoff(0, std::ios::cur, std::ios::in);
            ulSize = buf->pubseekoff(0, std::ios::end, std::ios::in);
            buf->pubseekoff(ulCurr, std::ios::beg, std::ios::in);
          }

          // read in the ASCII file and write back as GZIPped stream
          std::strstreambuf sbuf(ulSize);
          file >> &sbuf;
          Base::ogzstream gzip(filename);
          gzip << &sbuf;
          gzip.close();
        }
      }

      ret = true;
    }
    else
      vrmlRoot->unref();
  } else {
    // Write Inventor
    if ( out->openFile( filename ) == TRUE )
    {
      out->setBinary( binary );
      wa.apply(pcViewProviderRoot);
      ret = true;
    }
  }

  return ret;
}

void View3DInventorViewer::sizeChanged( const SbVec2s& size )
{
  // searching in the background node
//  if ( this->backgroundroot->getNumChildren() > 1 ) {
//    SoNode* child = this->backgroundroot->getChild(1);
//    if ( child && child->getTypeId() == SoFCBackgroundGradient::getClassTypeId() )
//    {
//      reinterpret_cast<SoFCBackgroundGradient*>(child)->setViewerSize( size );
//    }
//  }
//  
//  // searching in the foreground node
//  for ( int i=0; i<this->foregroundroot->getNumChildren(); i++ )
//  {
//    SoNode* child = this->foregroundroot->getChild(i);
//    if ( child && child->getTypeId().isDerivedFrom( SoFCColorBarBase::getClassTypeId() ) )
//    {
//      reinterpret_cast<SoFCColorBarBase*>(child)->setViewerSize( size );
//    }
//  }

  inherited::sizeChanged( size );
}

/**
 * Sets the SoFCInteractiveElement to \a true.
 */
void View3DInventorViewer::interactionStartCB(void * data, SoQtViewer * viewer)
{
  SoGLRenderAction * glra = viewer->getGLRenderAction();
  SoFCInteractiveElement::set(glra->getState(), viewer->getSceneGraph(), true);
}

/**
 * Sets the SoFCInteractiveElement to \a false and forces a redraw.
 */
void View3DInventorViewer::interactionFinishCB(void * data, SoQtViewer * viewer)
{
  SoGLRenderAction * glra = viewer->getGLRenderAction();
  SoFCInteractiveElement::set(glra->getState(), viewer->getSceneGraph(), false);
  viewer->render();
}

/**
 * Logs the type of the action that traverses the Inventor tree.
 */
void View3DInventorViewer::interactionLoggerCB(void * ud, SoAction* action)
{
  Base::Console().Log("%s\n", action->getTypeId().getName().getString());
}

void View3DInventorViewer::actualRedraw(void)
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
  glra->apply(this->backgroundroot);

  SbTime now = SbTime::getTimeOfDay();
  double secs = now.getValue() -  prevRedrawTime.getValue();
  prevRedrawTime = now;

  if (_bSpining) {
    SbRotation deltaRotation = spinRotation;
    deltaRotation.scaleAngle(secs * 5.0);
    reorientCamera(deltaRotation);
  }

  try {
    // Render normal scenegraph.
    inherited::actualRedraw();
  } catch ( const Base::MemoryException& e ) {
    // FIXME: If this exception appears then the background and camera position get broken somehow. (Werner 2006-02-01) 
    for ( std::set<ViewProvider*>::iterator it = _ViewProviderSet.begin(); it != _ViewProviderSet.end(); ++it )
      (*it)->hide();
    inherited::actualRedraw();
    QMessageBox::warning(getParentWidget(), e.what(), QObject::tr("Not enough memory available to display the data."));
  }


  // Render overlay front scenegraph.
  glClear(GL_DEPTH_BUFFER_BIT);
  glra->apply(this->foregroundroot);

  if(bDrawAxisCross)
    drawAxisCross();
  
  if (_bSpining)
    scheduleRedraw();  
  printDimension();
} 

void View3DInventorViewer::printDimension()
{
  SoCamera* cam = getCamera();
  if ( !cam ) return; // no camera there
  SoType t = getCamera()->getTypeId();
  if (t.isDerivedFrom(SoOrthographicCamera::getClassTypeId())) 
  {
    const SbViewportRegion& vp = getViewportRegion();
    const SbVec2s& size = vp.getWindowSize();
    short dimX, dimY; size.getValue(dimX, dimY);

    float fHeight = reinterpret_cast<SoOrthographicCamera*>(getCamera())->height.getValue();
    float fWidth = fHeight;
    if ( dimX > dimY )
    {
      fWidth *= ((float)dimX)/((float)dimY);
    }
    else if ( dimX < dimY )
    {
      fHeight *= ((float)dimY)/((float)dimX);
    }

    float fLog = float(log10(fWidth)), fFac;
    int   nExp = int(fLog);
    char  szUnit[20];
  
    if (nExp >= 6)
    {
      fFac = 1.0e+6f;
      strcpy(szUnit, "km");
    }
    else if (nExp >= 3)
    {
      fFac = 1.0e+3f;
      strcpy(szUnit, "m");
    }
    else if ((nExp >= 0) && (fLog > 0.0f))
    {
      fFac = 1.0e+0f;
      strcpy(szUnit, "mm");
    }
    else if (nExp >= -3)
    {
      fFac = 1.0e-3f;
      strcpy(szUnit, "um");
    }
    else 
    {
      fFac = 1.0e-6f;
      strcpy(szUnit, "nm");
    }
  
    QString dim; dim.sprintf("%.2f x %.2f %s", fWidth / fFac, fHeight / fFac, szUnit);
    getMainWindow()->setPaneText(2, dim);
  }
  else
    getMainWindow()->setPaneText(2, "");
}

/*!
  As ProgressBar has no chance to control the incoming Qt events of SoQt we need to override
  SoQtViewer::processEvent() to prevent the scenegraph from being selected or deselected
  while the progress bar is running.
 */
void View3DInventorViewer::processEvent(QEvent * event)
{
  if ( !Base::Sequencer().isRunning() )
    inherited::processEvent(event);
}

SbBool View3DInventorViewer::processSoEvent(const SoEvent * const ev)
{
  //Base::Console().Log("Evnt: %s\n",ev->getTypeId().getName().getString());
  bool processed = false;
  if ( !isSeekMode() && isViewing() )
    setViewing( false ); // by default disable viewing mode to render the scene

  // Keybooard handling
  if (ev->getTypeId().isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
    SoKeyboardEvent * ke = (SoKeyboardEvent *)ev;
    switch (ke->getKey()) {
    case SoKeyboardEvent::LEFT_ALT:
    case SoKeyboardEvent::RIGHT_ALT:
    case SoKeyboardEvent::LEFT_CONTROL:
    case SoKeyboardEvent::RIGHT_CONTROL:
    case SoKeyboardEvent::LEFT_SHIFT:
    case SoKeyboardEvent::RIGHT_SHIFT:
      break;
    case SoKeyboardEvent::H:
      this->saveHomePosition();
      processed = true;
      break;
    case SoKeyboardEvent::Q: // ignore 'Q' keys (to prevent app from being closed)
      processed = true;
      break;
    case SoKeyboardEvent::S:
      // processSoEvent() of the base class sets the seekMode() if needed
    case SoKeyboardEvent::HOME:
    case SoKeyboardEvent::LEFT_ARROW:
    case SoKeyboardEvent::UP_ARROW:
    case SoKeyboardEvent::RIGHT_ARROW:
    case SoKeyboardEvent::DOWN_ARROW:
      if (!isViewing())
        setViewing( true );
      break;
    default:
      break;
    }
  }

  static bool MoveMode=false;
  static bool ZoomMode=false;
  static bool RotMode =false;
  static bool dCliBut3=false;

  const SbVec2s size(this->getGLSize());
  const SbVec2f prevnormalized = lastmouseposition;
  const SbVec2s pos(ev->getPosition());
  const SbVec2f posn((float) pos[0] / (float) SoQtMax((int)(size[0] - 1), 1),
                     (float) pos[1] / (float) SoQtMax((int)(size[1] - 1), 1));
//  SbVec2s MovePos;
  lastmouseposition = posn;

  // switching the mouse modes
  if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {

    const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
    const int button = event->getButton();
    const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

    // SoDebugError::postInfo("processSoEvent", "button = %d", button);
    switch (button) {
    case SoMouseButtonEvent::BUTTON1:
      if(press)
      {
        _bRejectSelection = false;
        if(MoveMode)
        {
          RotMode = true;
          ZoomMode = false;
          MoveTime = ev->getTime();

        // Set up initial projection point for the projector object when
        // first starting a drag operation.
          spinprojector->project(lastmouseposition);
          //interactiveCountInc();
          clearLog();

          getWidget()->setCursor( QCursor( Qt::PointingHandCursor ) );
          processed = true;
        }
      }
      else
      {
        // if you come out of rotation dont deselect anything
        if(_bRejectSelection || MoveMode)
        {
          _bRejectSelection=false;
          processed = true;
        }
        if(MoveMode){
          RotMode = false; 

          SbTime tmp = (ev->getTime() - MoveTime);
          float dci = (float)QApplication::doubleClickInterval()/1000.0f;
          if (tmp.getValue() < dci/*0.300*/)
          {
            ZoomMode = true;
            getWidget()->setCursor( QCursor( Qt::SizeVerCursor ) );
          }else{
       
            ZoomMode = false;
            getWidget()->setCursor( QCursor( Qt::SizeAllCursor ) );

            SbViewVolume vv = getCamera()->getViewVolume(getGLAspectRatio());
            panningplane = vv.getPlane(getCamera()->focalDistance.getValue());
       
            // check on start spining
            SbTime stoptime = (ev->getTime() - log.time[0]);
            if (bAllowSpining && stoptime.getValue() < 0.100) {
              const SbVec2s glsize(this->getGLSize());
              SbVec3f from = spinprojector->project(SbVec2f(float(log.position[2][0]) / float(SoQtMax(glsize[0]-1, 1)),
                                                            float(log.position[2][1]) / float(SoQtMax(glsize[1]-1, 1))));
              SbVec3f to = spinprojector->project(posn);
              SbRotation rot = spinprojector->getRotation(from, to);

              SbTime delta = (log.time[0] - log.time[2]);
              double deltatime = delta.getValue();
              rot.invert();
              rot.scaleAngle(float(0.200 / deltatime));

              SbVec3f axis;
              float radians;
              rot.getValue(axis, radians);
              float dci = (float)QApplication::doubleClickInterval()/1000.0f;
              if ((radians > 0.01f) && (deltatime < dci/*0.300*/)) {
                _bSpining = true;
                spinRotation = rot;
                MoveMode = false;
                // restore the previous cursor
                getWidget()->setCursor( _oldCursor /*QCursor( Qt::ArrowCursor )*/);
              }
            }
          }
          processed = true;
        }
      }
      break;
    case SoMouseButtonEvent::BUTTON2:
      break;
    case SoMouseButtonEvent::BUTTON3:
      if(press)
      {
        // check on double click
        SbTime tmp = (ev->getTime() - CenterTime);
        float dci = (float)QApplication::doubleClickInterval()/1000.0f;
        if (tmp.getValue() < dci/*0.300*/)
        {
          dCliBut3 = true;
          if(!seekToPoint(pos))
            panToCenter(panningplane, posn);
        }else{
          CenterTime = ev->getTime();
          MoveMode = true;
          _bSpining = false;
          dCliBut3 = false;
          SbViewVolume vv = getCamera()->getViewVolume(getGLAspectRatio());
          panningplane = vv.getPlane(getCamera()->focalDistance.getValue());
          // save the current cursor before overriding
          _oldCursor = getWidget()->cursor();
          getWidget()->setCursor( QCursor( Qt::SizeAllCursor ) );
        }
      }else{
        MoveMode = false;
        RotMode = false;
        ZoomMode = false;
        // restore the previous cursor
        getWidget()->setCursor( _oldCursor /*QCursor( Qt::ArrowCursor )*/);
        _bRejectSelection = true;
      }
      processed = true;
      break;
    case SoMouseButtonEvent::BUTTON4:
      if (press) 
        View3DInventorViewer::zoom(getCamera(), 0.05f);

      processed = true;
      break;
    case SoMouseButtonEvent::BUTTON5:
      if (press) 
        View3DInventorViewer::zoom(getCamera(), -0.05f);

      processed = true;
      break;
    default:
      break;
    }
  }

  // Mouse Movement handling
  if (ev->getTypeId().isDerivedFrom(SoLocation2Event::getClassTypeId())) {
//    const SoLocation2Event * const event = (const SoLocation2Event *) ev;

    if(MoveMode && ZoomMode){
      zoom(getCamera(),(posn[1] - prevnormalized[1]) * 10.0f);
      processed = true;
    }else if(MoveMode && RotMode) {
      addToLog(ev->getPosition(), ev->getTime());
      spin(posn);

      processed = true;
    }else if(MoveMode) {
      pan(getCamera(),getGLAspectRatio(),panningplane, posn, prevnormalized);
      processed = true;

    }else if(_bSpining) {
      processed = true;
    }
  }

  // Spaceball & Joystick handling
  if (ev->getTypeId().isDerivedFrom(SoMotion3Event::getClassTypeId())) {
    SoMotion3Event * const event = (SoMotion3Event *) ev;
    SoCamera * const camera = this->getCamera();
    if (camera) {
      SbVec3f dir = event->getTranslation();
      camera->orientation.getValue().multVec(dir,dir);
      camera->position = camera->position.getValue() + dir;
      camera->orientation = 
        event->getRotation() * camera->orientation.getValue();
      processed = TRUE;
    }
  }

  // give the viewprovider the chance to handle the event
  if(!processed && !MoveMode && !RotMode)
  {
    std::set<ViewProvider*>::iterator It;
    for(It=_ViewProviderSet.begin();It!=_ViewProviderSet.end() && !processed;It++)
      processed = (*It)->handleEvent(ev,*this);
  }

  // give the nodes in the foreground root the chance to handle events (e.g color bar)
  // Note: this must be done _before_ ceding to the viewer  
  if ( !processed )
  {
    SoHandleEventAction action(getViewportRegion());
    action.setEvent(ev);
    action.apply(foregroundroot);
    processed = action.isHandled();
  }

  // right mouse button pressed
  if (!processed && !MoveMode && !RotMode)
  {
    if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
      SoMouseButtonEvent * const e = (SoMouseButtonEvent *) ev;
      if ((e->getButton() == SoMouseButtonEvent::BUTTON2) && e->getState() == SoButtonEvent::UP) {
        if (this->isPopupMenuEnabled()) {
          if (e->getState() == SoButtonEvent::UP) {
            this->openPopupMenu(e->getPosition());
          }

          // Steal all RMB-events if the viewer uses the popup-menu.
          processed = TRUE;
        }
      }
    }
  }

  // invokes the appropriate callback function when user interaction has started or finished
  bool bInteraction = (MoveMode||ZoomMode||RotMode|_bSpining);
  if (bInteraction && getInteractiveCount()==0)
    interactiveCountInc();
  // must not be in seek mode because it gets decremented in setSeekMode(FALSE)
  else if (!bInteraction&&!dCliBut3&&getInteractiveCount()>0&&!isSeekMode())
    interactiveCountDec();


  bool baseProcessed;
  if(!processed)
    baseProcessed = inherited::processSoEvent(ev);
  else 
    return true;


  // check for left click without selecting something
  if (!baseProcessed)
  {
    if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
      SoMouseButtonEvent * const e = (SoMouseButtonEvent *) ev;
      if (SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1)) {
        //Base::Console().Log("unhandled left button click!");
        Gui::Selection().clearSelection();
      }
      
    }
    
  }
  

  return false;
}

void View3DInventorViewer::setPopupMenuEnabled(const SbBool on)
{
  this->MenuEnabled = on;
}

SbBool View3DInventorViewer::isPopupMenuEnabled(void) const
{
  return this->MenuEnabled;
}

void View3DInventorViewer::openPopupMenu(const SbVec2s& position)
{
  MenuItem* StdViews = new MenuItem;
  StdViews->setCommand( "Standard views" );

  *StdViews<< "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewTop" << "Std_ViewRight"
           << "Std_ViewRear" << "Std_ViewBottom" << "Std_ViewLeft";

  MenuItem* view = new MenuItem;
  *view << "Std_ViewFitAll" << StdViews << "Separator" << "Std_ViewDockUndockFullscreen" ;

  // ask workbenches and view provider, ...
  Gui::Application::Instance->setupContextMenu("View", view);

  QPopupMenu ContextMenu(this->getGLWidget());
  MenuManager::getInstance()->setupContextMenu(view,ContextMenu);
  delete view;
  ContextMenu.exec( QCursor::pos() );
}

Base::Vector3f View3DInventorViewer::getViewDirection() const
{
  SoCamera* pCam = getCamera();
  SbViewVolume  vol = pCam->getViewVolume (); 

  // get the normal of the front clipping plane
  SbPlane nearPlane = vol.getPlane( vol.nearDist );
  SbVec3f n = nearPlane.getNormal();
  float nx, ny, nz; n.getValue(nx, ny, nz);

  Base::Vector3f viewDir(nx, ny, nz);
  viewDir.Normalize();

  return viewDir;
}

void View3DInventorViewer::getFrontClippingPlane( Base::Vector3f& rcPt, Base::Vector3f& rcNormal ) const
{
  SoCamera* pCam = getCamera();  
  SbViewVolume  vol = pCam->getViewVolume (); 

  // get the normal of the front clipping plane
  SbPlane nearPlane = vol.getPlane( vol.nearDist );
  SbVec3f n = nearPlane.getNormal();
  float nx, ny, nz; n.getValue(nx, ny, nz);
  float d = nearPlane.getDistanceFromOrigin();

  rcNormal.Set(nx, ny, nz);
  rcNormal.Normalize();
  rcPt.Set(d*rcNormal.x, d*rcNormal.y, d*rcNormal.z);
}

void View3DInventorViewer::getBackClippingPlane( Base::Vector3f& rcPt, Base::Vector3f& rcNormal ) const
{
  SoCamera* pCam = getCamera();  
  SbViewVolume  vol = pCam->getViewVolume (); 

  // get the normal of the back clipping plane
  SbPlane farPlane = vol.getPlane( vol.nearDist+vol.nearToFar );
  SbVec3f n = farPlane.getNormal();
  float nx, ny, nz; n.getValue(nx, ny, nz);
  float d = farPlane.getDistanceFromOrigin();

  rcNormal.Set(nx, ny, nz);
  rcNormal.Normalize();
  rcPt.Set(d*rcNormal.x, d*rcNormal.y, d*rcNormal.z);
}

void View3DInventorViewer::toggleClippingPlane()
{
  if ( pcViewProviderRoot->getNumChildren() > 0 && pcViewProviderRoot->getChild(0)->getTypeId() == SoClipPlaneManip::getClassTypeId() )
  {
    pcViewProviderRoot->removeChild(0);
  }
  else
  {
    pcViewProviderRoot->insertChild(new SoClipPlaneManip,0);
  }
}

bool View3DInventorViewer::hasClippingPlane() const
{
  if ( pcViewProviderRoot->getNumChildren() > 0 )
  {
    return ( pcViewProviderRoot->getChild(0)->getTypeId() == SoClipPlaneManip::getClassTypeId() );
  }

  return false;
}

/**
 * This method picks the closest point to the camera in the underlying scenegraph and returns its location and normal. 
 * If no point was picked false is returned.
 */
bool View3DInventorViewer::pickPoint(const SbVec2s& pos,SbVec3f &point,SbVec3f &norm) const
{
 // attempting raypick in the event_cb() callback method
  SoRayPickAction rp( getViewportRegion() );
  rp.setPoint(pos);
  rp.apply(getSceneManager()->getSceneGraph());
  SoPickedPoint *Point = rp.getPickedPoint();  

  if(Point)
  {
    point = Point->getObjectPoint();
    norm  = Point->getObjectNormal();
    return true;
  }else
    return false;
}

/**
 * This method is provided for convenience and does basically the same as method above unless that it
 * returns an SoPickedPoint object with additional information.
 * \note It is in the response of the client programmer to delete the returned SoPickedPoint object.
 */
SoPickedPoint* View3DInventorViewer::pickPoint(const SbVec2s& pos) const
{
  SoRayPickAction rp( getViewportRegion() );
  rp.setPoint(pos);
  rp.apply(getSceneManager()->getSceneGraph());

  // returns a copy of the point
  SoPickedPoint* pick = rp.getPickedPoint();
  return (pick ? pick->copy() : 0);
}

void View3DInventorViewer::boxZoom( const SbBox2f& box )
{
  // first move to the center of the box
  panToCenter(panningplane, box.getCenter());
  float w,h;
  box.getSize(w,h);

  // normalized coordinates are within the range [0,1]
  float max = w>h?w:h;
  if ( max < 0.001f )
    return;
  // global ln function
  zoom(getCamera(), ::log(max));
}

void View3DInventorViewer::panToCenter(const SbPlane & panningplane, const SbVec2f & currpos)
{
   pan(getCamera(),getGLAspectRatio(),panningplane, SbVec2f(0.5,0.5), currpos);
}

void View3DInventorViewer::pan(SoCamera * cam,float aspectratio, const SbPlane & panningplane, const SbVec2f & currpos, const SbVec2f & prevpos)
{
  if (cam == NULL) return; // can happen for empty scenegraph
  if (currpos == prevpos) return; // useless invocation


  // Find projection points for the last and current mouse coordinates.
  SbViewVolume vv = cam->getViewVolume(aspectratio);
  SbLine line;
  vv.projectPointToLine(currpos, line);
  SbVec3f current_planept;
  panningplane.intersect(line, current_planept);
  vv.projectPointToLine(prevpos, line);
  SbVec3f old_planept;
  panningplane.intersect(line, old_planept);

  // Reposition camera according to the vector difference between the
  // projected points.
  cam->position = cam->position.getValue() - (current_planept - old_planept);
}

// Dependent on the camera type this will either shrink or expand the
// height of the viewport (orthogonal camera) or move the camera
// closer or further away from the focal point in the scene.
//
// Used from both SoGuiPlaneViewer and SoGuiExaminerViewer.
// Implemented in the SoGuiFullViewer private class to collect common
// code.

void View3DInventorViewer::zoom(SoCamera * cam, const float diffvalue)
{
  if (cam == NULL) return; // can happen for empty scenegraph
  SoType t = cam->getTypeId();
  SbName tname = t.getName();

  // This will be in the range of <0, ->>.
  float multiplicator = float(exp(diffvalue));

  if (t.isDerivedFrom(SoOrthographicCamera::getClassTypeId())) {

    // Since there's no perspective, "zooming" in the original sense
    // of the word won't have any visible effect. So we just increase
    // or decrease the field-of-view values of the camera instead, to
    // "shrink" the projection size of the model / scene.
    SoOrthographicCamera * oc = (SoOrthographicCamera *)cam;
    oc->height = oc->height.getValue() * multiplicator;

  }
  else {
    // FrustumCamera can be found in the SmallChange CVS module (it's
    // a camera that lets you specify (for instance) an off-center
    // frustum (similar to glFrustum())
    if (!t.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()) &&
        tname != "FrustumCamera") {
 /*     static SbBool first = TRUE;
      if (first) {
        SoDebugError::postWarning("SoGuiFullViewerP::zoom",
                                  "Unknown camera type, "
                                  "will zoom by moving position, but this might not be correct.");
        first = FALSE;
      }*/
    }
    
    const float oldfocaldist = cam->focalDistance.getValue();
    const float newfocaldist = oldfocaldist * multiplicator;

    SbVec3f direction;
    cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);

    const SbVec3f oldpos = cam->position.getValue();
    const SbVec3f newpos = oldpos + (newfocaldist - oldfocaldist) * -direction;

    // This catches a rather common user interface "buglet": if the
    // user zooms the camera out to a distance from origo larger than
    // what we still can safely do floating point calculations on
    // (i.e. without getting NaN or Inf values), the faulty floating
    // point values will propagate until we start to get debug error
    // messages and eventually an assert failure from core Coin code.
    //
    // With the below bounds check, this problem is avoided.
    //
    // (But note that we depend on the input argument ''diffvalue'' to
    // be small enough that zooming happens gradually. Ideally, we
    // should also check distorigo with isinf() and isnan() (or
    // inversely; isinfite()), but those only became standardized with
    // C99.)
    const float distorigo = newpos.length();
    // sqrt(FLT_MAX) == ~ 1e+19, which should be both safe for further
    // calculations and ok for the end-user and app-programmer.
    if (distorigo > float(sqrt(FLT_MAX))) {
    }
    else {
      cam->position = newpos;
      cam->focalDistance = newfocaldist;
    }
  }
}

// Uses the sphere sheet projector to map the mouseposition unto
// a 3D point and find a rotation from this and the last calculated point.
void View3DInventorViewer::spin(const SbVec2f & pointerpos)
{
  if (this->log.historysize < 2) return;
  assert(this->spinprojector != NULL);

  SbVec2s glsize(getGLSize());
  SbVec2f lastpos;
  lastpos[0] = float(this->log.position[1][0]) / float(SoQtMax((int)(glsize[0]-1), 1));
  lastpos[1] = float(this->log.position[1][1]) / float(SoQtMax((int)(glsize[1]-1), 1));

  this->spinprojector->project(lastpos);
  SbRotation r;
  this->spinprojector->projectAndGetRotation(pointerpos, r);
  r.invert();
  this->reorientCamera(r);

  // Calculate an average angle magnitude value to make the transition
  // to a possible spin animation mode appear smooth.

  SbVec3f dummy_axis, newaxis;
  float acc_angle, newangle;
  this->spinincrement.getValue(dummy_axis, acc_angle);
  acc_angle *= this->spinsamplecounter; // weight
  r.getValue(newaxis, newangle);
  acc_angle += newangle;

  this->spinsamplecounter++;
  acc_angle /= this->spinsamplecounter;
  // FIXME: accumulate and average axis vectors aswell? 19990501 mortene.
  this->spinincrement.setValue(newaxis, acc_angle);

  // Don't carry too much baggage, as that'll give unwanted results
  // when the user quickly trigger (as in "click-drag-release") a spin
  // animation.
  if (this->spinsamplecounter > 3) this->spinsamplecounter = 3;
  
}


#if 0
void View3DInventorViewer::sFinishSelectionCallback(void *viewer,SoSelection *path)
{
  static_cast<View3DInventorViewer*>(viewer)->finishSelectionCallback(path);
}

void View3DInventorViewer::finishSelectionCallback(SoSelection *)
{
//  Base::Console().Log("SelectionCallback\n");
}


void View3DInventorViewer::sMadeSelection(void *viewer,SoPath *path)
{
  static_cast<View3DInventorViewer*>(viewer)->madeSelection(path);
}

// Callback function triggered for selection / deselection.
void View3DInventorViewer::madeSelection(  SoPath * path )
{
  for(int i = 0; i<path->getLength();i++)
  {
    SoNode *node = path->getNodeFromTail(i);
    if (node->getTypeId() == SoFCSelection::getClassTypeId()) 
    {
      SoFCSelection * SelNode = (SoFCSelection *)node;  // safe downward cast, knows the type
      Base::Console().Log("Select:%s.%s.%s \n",SelNode->documentName.getValue().getString(),
                                               SelNode->featureName.getValue().getString(),
                                               SelNode->subElementName.getValue().getString());

      SelNode->selected = SoFCSelection::SELECTED;

      Gui::Selection().addSelection(SelNode->documentName.getValue().getString(),
                                    SelNode->featureName.getValue().getString(),
                                    SelNode->subElementName.getValue().getString());
    
    }

  }



/*
  for(std::set<ViewProvider*>::iterator It = _ViewProviderSet.begin();It!=_ViewProviderSet.end();It++)
    for(int i = 0; i<path->getLength();i++)
      if((*It)->getRoot() == path->getNodeFromTail(i))
      {
        (*It)->selected(this,path);
        return;
      }
*/

}

void View3DInventorViewer::sUnmadeSelection(void *viewer,SoPath *path)
{
  static_cast<View3DInventorViewer*>(viewer)->unmadeSelection(path);
}

// Callback function triggered for deselection.
void View3DInventorViewer::unmadeSelection(  SoPath * path )
{
  for(int i = 0; i<path->getLength();i++)
  {
    SoNode *node = path->getNodeFromTail(i);
    if (node->getTypeId() == SoFCSelection::getClassTypeId()) 
    {
      SoFCSelection * SelNode = (SoFCSelection *)node;  // safe downward cast, knows the type
      Base::Console().Log("Unselect:%s.%s.%s \n",SelNode->documentName.getValue().getString(),
                                               SelNode->featureName.getValue().getString(),
                                               SelNode->subElementName.getValue().getString());
      SelNode->selected = SoFCSelection::NOTSELECTED;

      Gui::Selection().rmvSelection(SelNode->documentName.getValue().getString(),
                                    SelNode->featureName.getValue().getString(),
                                    SelNode->subElementName.getValue().getString());
    


    }

  }
}

#endif



//****************************************************************************

// Bitmap representations of an "X", a "Y" and a "Z" for the axis cross.
static GLubyte xbmp[] = { 0x11,0x11,0x0a,0x04,0x0a,0x11,0x11 };
static GLubyte ybmp[] = { 0x04,0x04,0x04,0x04,0x0a,0x11,0x11 };
static GLubyte zbmp[] = { 0x1f,0x10,0x08,0x04,0x02,0x01,0x1f };

void View3DInventorViewer::drawAxisCross(void)
{
  // FIXME: convert this to a superimposition scenegraph instead of
  // OpenGL calls. 20020603 mortene.

  // Store GL state.
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  GLfloat depthrange[2];
  glGetFloatv(GL_DEPTH_RANGE, depthrange);
  GLdouble projectionmatrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);

  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glDepthRange(0, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_BLEND); // Kills transparency.

  // Set the viewport in the OpenGL canvas. Dimensions are calculated
  // as a percentage of the total canvas size.
  SbVec2s view = getGLSize();
  const int pixelarea =
    int(float(this->axiscrossSize)/100.0f * SoQtMin(view[0], view[1]));
#if 0 // middle of canvas
  SbVec2s origin(view[0]/2 - pixelarea/2, view[1]/2 - pixelarea/2);
#endif // middle of canvas
#if 1 // lower right of canvas
  SbVec2s origin(view[0] - pixelarea, 0);
#endif // lower right of canvas
  glViewport(origin[0], origin[1], pixelarea, pixelarea);



  // Set up the projection matrix.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  const float NEARVAL = 0.1f;
  const float FARVAL = 10.0f;
  const float dim = NEARVAL * float(tan(M_PI / 8.0)); // FOV is 45° (45/360 = 1/8)
  glFrustum(-dim, dim, -dim, dim, NEARVAL, FARVAL);


  // Set up the model matrix.
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  SbMatrix mx;
  SoCamera * cam = getCamera();

  // If there is no camera (like for an empty scene, for instance),
  // just use an identity rotation.
  if (cam) { mx = cam->orientation.getValue(); }
  else { mx = SbMatrix::identity(); }

  mx = mx.inverse();
  mx[3][2] = -3.5; // Translate away from the projection point (along z axis).
  glLoadMatrixf((float *)mx);


  // Find unit vector end points.
  SbMatrix px;
  glGetFloatv(GL_PROJECTION_MATRIX, (float *)px);
  SbMatrix comb = mx.multRight(px);

  SbVec3f xpos;
  comb.multVecMatrix(SbVec3f(1,0,0), xpos);
  xpos[0] = (1 + xpos[0]) * view[0]/2;
  xpos[1] = (1 + xpos[1]) * view[1]/2;
  SbVec3f ypos;
  comb.multVecMatrix(SbVec3f(0,1,0), ypos);
  ypos[0] = (1 + ypos[0]) * view[0]/2;
  ypos[1] = (1 + ypos[1]) * view[1]/2;
  SbVec3f zpos;
  comb.multVecMatrix(SbVec3f(0,0,1), zpos);
  zpos[0] = (1 + zpos[0]) * view[0]/2;
  zpos[1] = (1 + zpos[1]) * view[1]/2;


  // Render the cross.
  {
    glLineWidth(2.0);

    enum { XAXIS, YAXIS, ZAXIS };
    int idx[3] = { XAXIS, YAXIS, ZAXIS };
    float val[3] = { xpos[2], ypos[2], zpos[2] };

    // Bubble sort.. :-}
    if (val[0] < val[1]) { SoQtSwap(val[0], val[1]); SoQtSwap(idx[0], idx[1]); }
    if (val[1] < val[2]) { SoQtSwap(val[1], val[2]); SoQtSwap(idx[1], idx[2]); }
    if (val[0] < val[1]) { SoQtSwap(val[0], val[1]); SoQtSwap(idx[0], idx[1]); }
    assert((val[0] >= val[1]) && (val[1] >= val[2])); // Just checking..

    for (int i=0; i < 3; i++) {
      glPushMatrix();
      if (idx[i] == XAXIS) {                       // X axis.
        if(isStereoViewing())
          glColor3f(0.500f, 0.125f, 0.125f);
        else
          glColor3f(0.500f, 0.5f, 0.5f);
      } else if (idx[i] == YAXIS) {                // Y axis.
        glRotatef(90, 0, 0, 1);
        if(isStereoViewing())
          glColor3f(0.400f, 0.4f, 0.4f);
        else
          glColor3f(0.125f, 0.500f, 0.125f);
      } else {                                     // Z axis.
        glRotatef(-90, 0, 1, 0);
        if(isStereoViewing())
          glColor3f(0.300f, 0.3f, 0.3f);
        else
          glColor3f(0.125f, 0.125f, 0.500f);
      }
      this->drawArrow(); 
      glPopMatrix();
    }
  }

  // Render axis notation letters ("X", "Y", "Z").
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, view[0], 0, view[1], -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLint unpack;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if(isStereoViewing())
    glColor3fv(SbVec3f(1.0f, 1.0f, 1.0f).getValue());
  else
    glColor3fv(SbVec3f(0.0f, 0.0f, 0.0f).getValue());

  glRasterPos2d(xpos[0], xpos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, xbmp);
  glRasterPos2d(ypos[0], ypos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, ybmp);
  glRasterPos2d(zpos[0], zpos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, zbmp);

  glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
  glPopMatrix();

  // Reset original state.

  // FIXME: are these 3 lines really necessary, as we push
  // GL_ALL_ATTRIB_BITS at the start? 20000604 mortene.
  glDepthRange(depthrange[0], depthrange[1]);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projectionmatrix);

  glPopAttrib();
}

// Draw an arrow for the axis representation directly through OpenGL.
void
View3DInventorViewer::drawArrow(void)
{
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glEnd();
  glDisable(GL_CULL_FACE);
  glBegin(GL_TRIANGLES);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, +0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, -0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, +0.5f / 4.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, -0.5f / 4.0f);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(1.0f - 1.0f / 3.0f, +0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, +0.5f / 4.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, -0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, -0.5f / 4.0f);
  glEnd();
}



// Rotate the camera by the given amount, then reposition it so we're
// still pointing at the same focal point.
void
View3DInventorViewer::reorientCamera(const SbRotation & rot)
{
  SoCamera * cam = getCamera();
  if (cam == NULL) return;

  // Find global coordinates of focal point.
  SbVec3f direction;
  cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  SbVec3f focalpoint = cam->position.getValue() +
    cam->focalDistance.getValue() * direction;

  // Set new orientation value by accumulating the new rotation.
  cam->orientation = rot * cam->orientation.getValue();

  // Reposition camera so we are still pointing at the same old focal point.
  cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  cam->position = focalpoint - cam->focalDistance.getValue() * direction;
}


// *************************************************************************
// Methods used for spin animation tracking.

// This method "clears" the mouse location log, used for spin
// animation calculations.
void View3DInventorViewer::clearLog(void)
{
  this->log.historysize = 0;
}

// This method adds another point to the mouse location log, used for spin
// animation calculations.
void View3DInventorViewer::addToLog(const SbVec2s pos, const SbTime time)
{
  // In case someone changes the const size setting at the top of this
  // file too small.
  assert (this->log.size > 2 && "mouse log too small!");

  if (this->log.historysize > 0 && pos == this->log.position[0]) {
#if SOQt_DEBUG && 0 // debug
    // This can at least happen under SoQt.
    SoDebugError::postInfo("SoGuiExaminerViewerP::addToLog", "got position already!");
#endif // debug
    return;
  }

  int lastidx = this->log.historysize;
  // If we've filled up the log, we should throw away the last item:
  if (lastidx == this->log.size) { lastidx--; }

  assert(lastidx < this->log.size);
  for (int i = lastidx; i > 0; i--) {
    this->log.position[i] = this->log.position[i-1];
    this->log.time[i] = this->log.time[i-1];
  }

  this->log.position[0] = pos;
  this->log.time[0] = time;
  if (this->log.historysize < this->log.size)
    this->log.historysize += 1;
}

// Draw routines
void View3DInventorViewer::drawRect( int x, int y, int w, int h, QPainter* p )
{
  if (p)
    p->drawRect( QRect( QPoint( QMIN( x, w ), QMIN( y, h ) ),
         QPoint( QMAX( x, w ), QMAX( y, h ) ) ) );
  else
  {
    QPainter p(getGLWidget());
//    p.setPen( Qt::white );
    QPen pen = p.pen();
    pen.setColor( Qt::white );
    pen.setStyle( Qt::DashLine );
    pen.setWidth( 3 );
    p.setPen( pen );
    p.setRasterOp( QPainter::XorROP );
    drawRect( x, y, w, h, &p );
  }
}

void View3DInventorViewer::drawNode ( int x, int y, int w, int h, QPainter* p )
{
  if (p)
    p->drawEllipse( x, y, w, h );
  else
  {
    QPainter p(getGLWidget());
//    p.setPen( Qt::white );
    QPen pen = p.pen();
    pen.setColor( Qt::white );
    pen.setWidth( 3 );
    p.setPen( pen );
    p.setBrush( Qt::white );
    p.setRasterOp( QPainter::XorROP );
    drawNode( x, y, w, h, &p );
  }
}

void View3DInventorViewer::drawLine ( int x1, int y1, int x2, int y2, QPainter* p )
{
  if (p)
    p->drawLine( x1, y1, x2, y2 );
  else
  {
    QPainter p(getGLWidget());
//    p.setPen( Qt::white );
    QPen pen = p.pen();
    pen.setColor( Qt::white );
    pen.setStyle( Qt::DashLine );
    pen.setWidth( 3 );
    p.setPen( pen );
    p.setRasterOp( QPainter::XorROP );
    drawLine( x1, y1, x2, y2, &p );
  }
}

void View3DInventorViewer::drawCircle ( int x, int y, int r, QPainter* p )
{
  if (p)
  {
    QPoint center(x-r/2, y-r/2);
    p->drawEllipse( center.x(), center.y(), r, r );
  }
  else
  {
    QPainter p(getGLWidget());
    p.setPen( Qt::green );
    p.setRasterOp( QPainter::XorROP );
    drawCircle( x, y, r, &p );
  }
}

void View3DInventorViewer::drawText ( int x, int y, const QString & str, QPainter* p )
{
  if (p)
    p->drawText( x, y, str);
  else
  {
    QPainter p(getGLWidget());
    p.setPen( Qt::white );
    p.setRasterOp( QPainter::XorROP );
    drawText( x, y, str, &p );
  }
}


#if 0

/*!
  Call this method to initiate a seek action towards the 3D
  intersection of the scene and the ray from the screen coordinate's
  point and in the same direction as the camera is pointing.

  Returns \c TRUE if the ray from the \a screenpos position intersect
  with any parts of the onscreen geometry, otherwise \c FALSE.
*/
SbBool
So@Gui@Viewer::seekToPoint(const SbVec2s screenpos)
{
  if (! PRIVATE(this)->camera)
    return FALSE;

  SoRayPickAction rpaction(this->getViewportRegion());
  rpaction.setPoint(screenpos);
  rpaction.setRadius(2);
  rpaction.apply(PRIVATE(this)->sceneroot);

  SoPickedPoint * picked = rpaction.getPickedPoint();
  if (!picked) {
    // FIXME: this inc seems bogus, but is needed now due to buggy
    // code in for instance the examinerviewer
    // processSoEvent(). 20020510 mortene.
#if 1
    this->interactiveCountInc(); // decremented in setSeekMode(FALSE)
#endif // FIXME
    this->setSeekMode(FALSE);
    return FALSE;
  }

  SbVec3f hitpoint;
  if (PRIVATE(this)->seektopoint) {
    hitpoint = picked->getPoint();
  } 
  else {
    SoGetBoundingBoxAction bbaction(this->getViewportRegion());
    bbaction.apply(picked->getPath());
    SbBox3f bbox = bbaction.getBoundingBox();
    hitpoint = bbox.getCenter();
  }

  PRIVATE(this)->camerastartposition = PRIVATE(this)->camera->position.getValue();
  PRIVATE(this)->camerastartorient = PRIVATE(this)->camera->orientation.getValue();

  // move point to the camera coordinate system, consider
  // transformations before camera in the scene graph
  SbMatrix cameramatrix, camerainverse;
  PRIVATE(this)->getCameraCoordinateSystem(PRIVATE(this)->camera,
                                           PRIVATE(this)->sceneroot,
                                           cameramatrix,
                                           camerainverse);
  camerainverse.multVecMatrix(hitpoint, hitpoint);

  float fd = PRIVATE(this)->seekdistance;
  if (!PRIVATE(this)->seekdistanceabs)
    fd *= (hitpoint - PRIVATE(this)->camera->position.getValue()).length()/100.0f;
  PRIVATE(this)->camera->focalDistance = fd;

  SbVec3f dir = hitpoint - PRIVATE(this)->camerastartposition;
  dir.normalize();

  // find a rotation that rotates current camera direction into new
  // camera direction.
  SbVec3f olddir;
  PRIVATE(this)->camera->orientation.getValue().multVec(SbVec3f(0, 0, -1), olddir);
  SbRotation diffrot(olddir, dir);
  PRIVATE(this)->cameraendposition = hitpoint - fd * dir;
  PRIVATE(this)->cameraendorient = PRIVATE(this)->camera->orientation.getValue() * diffrot;

  if (PRIVATE(this)->seeksensor->isScheduled()) {
    PRIVATE(this)->seeksensor->unschedule();
    this->interactiveCountDec();
  }

  PRIVATE(this)->seeksensor->setBaseTime(SbTime::getTimeOfDay());
  PRIVATE(this)->seeksensor->schedule();
  this->interactiveCountInc();

  return TRUE;
}






SbBool
SoQtExaminerViewer::processSoEvent(const SoEvent * const ev)
{
#if SOQt_DEBUG && 0 // debug
  SoDebugError::postInfo("SoQtExaminerViewer::processSoEvent",
                          "[invoked], event '%s'",
                          ev->getTypeId().getName().getString());
#endif // debug

  // Let the end-user toggle between camera-interaction mode
  // ("viewing") and scenegraph-interaction mode with ALT key(s).
  if (ev->getTypeId().isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
    SoKeyboardEvent * ke = (SoKeyboardEvent *)ev;
    switch (ke->getKey()) {
    case SoKeyboardEvent::LEFT_ALT:
    case SoKeyboardEvent::RIGHT_ALT:
      if (this->isViewing() && (ke->getState() == SoButtonEvent::DOWN)) {
        this->setViewing(FALSE);
        return TRUE;
      }
      else if (!this->isViewing() && (ke->getState() == SoButtonEvent::UP)) {
        this->setViewing(TRUE);
        return TRUE;
      }
    default:
      break;
    }
  }

  // We're in "interact" mode (ie *not* the camera modification mode),
  // so don't handle the event here. It should either be forwarded to
  // the scenegraph, or caught by SoQtViewer::processSoEvent() if
  // it's an ESC press (to switch modes).
  if (!this->isViewing()) { return inherited::processSoEvent(ev); }
    
  // Events when in "ready-to-seek" mode are ignored, except those
  // which influence the seek mode itself -- these are handled further
  // up the inheritance hierarchy.
  if (this->isSeekMode()) { return inherited::processSoEvent(ev); }

  const SoType type(ev->getTypeId());

  const SbVec2s size(this->getGLSize());
  const SbVec2f prevnormalized = lastmouseposition;
  const SbVec2s pos(ev->getPosition());
  const SbVec2f posn((float) pos[0] / (float) SoQtMax((int)(size[0] - 1), 1),
                     (float) pos[1] / (float) SoQtMax((int)(size[1] - 1), 1));

  lastmouseposition = posn;

  // Set to TRUE if any event processing happened. Note that it is not
  // necessary to restrict ourselves to only do one "action" for an
  // event, we only need this flag to see if any processing happened
  // at all.
  SbBool processed = FALSE;

  const SoGuiExaminerViewerP::ViewerMode currentmode = currentmode;
  SoGuiExaminerViewerP::ViewerMode newmode = currentmode;

  // Mismatches in state of the modifier keys happens if the user
  // presses or releases them outside the viewer window.
  if (ctrldown != ev->wasCtrlDown()) {
    ctrldown = ev->wasCtrlDown();
  }
  if (shiftdown != ev->wasShiftDown()) {
    shiftdown = ev->wasShiftDown();
  }

  // Mouse Button / Spaceball Button handling

  if (type.isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
    processed = TRUE;

    const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
    const int button = event->getButton();
    const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

    // SoDebugError::postInfo("processSoEvent", "button = %d", button);
    switch (button) {
    case SoMouseButtonEvent::BUTTON1:
      button1down = press;
      if (press && (currentmode == SoGuiExaminerViewerP::SEEK_WAIT_MODE)) {
        newmode = SoGuiExaminerViewerP::SEEK_MODE;
        this->seekToPoint(pos); // implicitly calls interactiveCountInc()
      }
      break;
    case SoMouseButtonEvent::BUTTON2:
      processed = FALSE; // pass on to superclass, so popup menu is shown
      break;
    case SoMouseButtonEvent::BUTTON3:
      button3down = press;
      break;
#ifdef HAVE_SOMOUSEBUTTONEVENT_BUTTON5
    case SoMouseButtonEvent::BUTTON4:
      if (press) SoGuiFullViewerP::zoom(this->getCamera(), 0.1f);
      break;
    case SoMouseButtonEvent::BUTTON5:
      if (press) SoGuiFullViewerP::zoom(this->getCamera(), -0.1f);
      break;
#endif // HAVE_SOMOUSEBUTTONEVENT_BUTTON5
    default:
      break;
    }
  }

  // Keyboard handling
  if (type.isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
    const SoKeyboardEvent * const event = (const SoKeyboardEvent *) ev;
    const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;
    switch (event->getKey()) {
    case SoKeyboardEvent::LEFT_CONTROL:
    case SoKeyboardEvent::RIGHT_CONTROL:
      processed = TRUE;
      ctrldown = press;
      break;
    case SoKeyboardEvent::LEFT_SHIFT:
    case SoKeyboardEvent::RIGHT_SHIFT:
      processed = TRUE;
      shiftdown = press;
      break;
    default:
      break;
    }
  }

  // Mouse Movement handling
  if (type.isDerivedFrom(SoLocation2Event::getClassTypeId())) {
    const SoLocation2Event * const event = (const SoLocation2Event *) ev;

    processed = TRUE;

    if (currentmode == SoGuiExaminerViewerP::ZOOMING) {
      zoomByCursor(posn, prevnormalized);
    }
    else if (currentmode == SoGuiExaminerViewerP::PANNING) {
      SoGuiFullViewerP::pan(this->getCamera(), this->getGLAspectRatio(),
                            panningplane, posn, prevnormalized);
    }
    else if (currentmode == SoGuiExaminerViewerP::DRAGGING) {
      addToLog(event->getPosition(), event->getTime());
      spin(posn);
    }
    else {
      processed = FALSE;
    }
  }

  // Spaceball & Joystick handling
  if (type.isDerivedFrom(SoMotion3Event::getClassTypeId())) {
    SoMotion3Event * const event = (SoMotion3Event *) ev;
    SoCamera * const camera = this->getCamera();
    if (camera) {
      if (motion3OnCamera) {
        SbVec3f dir = event->getTranslation();
        camera->orientation.getValue().multVec(dir,dir);
        camera->position = camera->position.getValue() + dir;
        camera->orientation = 
          event->getRotation() * camera->orientation.getValue();
        processed = TRUE;
      }
      else {
        // FIXME: move/rotate model
#if SOQt_DEBUG
        SoDebugError::postInfo("SoQtExaminerViewer::processSoEvent",
                               "SoMotion3Event for model movement is not implemented yet");
#endif // SOQt_DEBUG
        processed = TRUE;
      }
    }
  }

  enum {
    BUTTON1DOWN = 1 << 0,
    BUTTON3DOWN = 1 << 1,
    CTRLDOWN =    1 << 2,
    SHIFTDOWN =   1 << 3
  };
  unsigned int combo =
    (button1down ? BUTTON1DOWN : 0) |
    (button3down ? BUTTON3DOWN : 0) |
    (ctrldown ? CTRLDOWN : 0) |
    (shiftdown ? SHIFTDOWN : 0);

  switch (combo) {
  case 0:
    if (currentmode == SoGuiExaminerViewerP::SPINNING) { break; }
    newmode = SoGuiExaminerViewerP::IDLE;
    if ((currentmode == SoGuiExaminerViewerP::DRAGGING) &&
        this->isAnimationEnabled() && (log.historysize >= 3)) {
      SbTime stoptime = (ev->getTime() - log.time[0]);
      if (stoptime.getValue() < 0.100) {
        const SbVec2s glsize(this->getGLSize());
        SbVec3f from = spinprojector->project(SbVec2f(float(log.position[2][0]) / float(SoQtMax(glsize[0]-1, 1)),
                                                                     float(log.position[2][1]) / float(SoQtMax(glsize[1]-1, 1))));
        SbVec3f to = spinprojector->project(posn);
        SbRotation rot = spinprojector->getRotation(from, to);

        SbTime delta = (log.time[0] - log.time[2]);
        double deltatime = delta.getValue();
        rot.invert();
        rot.scaleAngle(float(0.200 / deltatime));

        SbVec3f axis;
        float radians;
        rot.getValue(axis, radians);
        if ((radians > 0.01f) && (deltatime < 0.300)) {
          newmode = SoGuiExaminerViewerP::SPINNING;
          spinRotation = rot;
        }
      }
    }
    break;
  case BUTTON1DOWN:
    newmode = SoGuiExaminerViewerP::DRAGGING;
    break;
  case BUTTON3DOWN:
  case CTRLDOWN|BUTTON1DOWN:
  case SHIFTDOWN|BUTTON1DOWN:
    newmode = SoGuiExaminerViewerP::PANNING;
    break;
  case BUTTON1DOWN|BUTTON3DOWN:
  case CTRLDOWN|BUTTON3DOWN:
  case CTRLDOWN|SHIFTDOWN|BUTTON1DOWN:
    newmode = SoGuiExaminerViewerP::ZOOMING;
    break;

    // There are many cases we don't handle that just falls through to
    // the default case, like SHIFTDOWN, CTRLDOWN, CTRLDOWN|SHIFTDOWN,
    // SHIFTDOWN|BUTTON3DOWN, SHIFTDOWN|CTRLDOWN|BUTTON3DOWN, etc.
    // This is a feature, not a bug. :-)
    //
    // mortene.

  default:
    // The default will make a spin stop and otherwise not do
    // anything.
    if ((currentmode != SoGuiExaminerViewerP::SEEK_WAIT_MODE) &&
        (currentmode != SoGuiExaminerViewerP::SEEK_MODE)) {
      newmode = SoGuiExaminerViewerP::IDLE;
    }
    break;
  }

  if (newmode != currentmode) {
    setMode(newmode);
  }

  // If not handled in this class, pass on upwards in the inheritance
  // hierarchy.
  return processed || inherited::processSoEvent(ev);
}

#endif
