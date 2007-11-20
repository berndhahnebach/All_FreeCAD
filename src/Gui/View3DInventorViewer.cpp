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
# ifdef FC_OS_WIN32
#  include <windows.h>
# endif
# ifdef FC_OS_MACOSX
# include <OpenGL/gl.h>
# else
# include <GL/gl.h>
# endif
# include <Inventor/SbBox.h>
# include <Inventor/actions/SoGetBoundingBoxAction.h>
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
#include "MouseModel.h"

#include "ViewProvider.h"
// build in Inventor

//#define FC_LOGGING_CB

#define new DEBUG_CLIENTBLOCK

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
  SoSeparator* root = pcProvider->getRoot();
  if ( root ) pcViewProviderRoot->addChild( root );
  SoSeparator* fore = pcProvider->getFrontRoot();
  if ( fore ) foregroundroot->addChild( fore );
  SoSeparator* back = pcProvider->getBackRoot ();
  if ( back ) backgroundroot->addChild( back );
  
  _ViewProviderSet.insert(pcProvider);
}

void View3DInventorViewer::removeViewProvider(ViewProvider* pcProvider)
{
  SoSeparator* root = pcProvider->getRoot();
  if ( root ) pcViewProviderRoot->removeChild( root );
  SoSeparator* fore = pcProvider->getFrontRoot();
  if ( fore ) foregroundroot->removeChild( fore );
  SoSeparator* back = pcProvider->getBackRoot ();
  if ( back ) backgroundroot->removeChild( back );
  
  _ViewProviderSet.erase(pcProvider);
}

View3DInventorViewer::View3DInventorViewer (QWidget *parent, const char *name, SbBool embed, Type type, SbBool build) 
  : inherited (parent, name, embed, type, build), MenuEnabled(TRUE), pcMouseModel(0),_iMouseModel(1), editing(FALSE)
{
  Gui::Selection().Attach(this);

  // Coin should not clear the pixel-buffer, so the background image
  // is not removed.
  this->setClearBeforeRender(FALSE);
  
  // setting up the defaults for the spin rotation
  initialize();

  _bRejectSelection = false;

  SoOrthographicCamera * cam = new SoOrthographicCamera;
  cam->position = SbVec3f(0, 0, 1);
  cam->height = 1;
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
  pcViewProviderRoot = new SoSeparator();

  // increase refcount before passing it to setScenegraph(), to avoid
  // premature destruction
  pcViewProviderRoot->ref();
  // is not really working with Coin3D. 
//  redrawOverlayOnSelectionChange(pcSelection);
  setSceneGraph(pcViewProviderRoot);
  // Event callback node
  pEventCallback = new SoEventCallback();
  pEventCallback->setUserData(this);
  pEventCallback->ref();
  pcViewProviderRoot->addChild(pEventCallback);

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
  setViewing(false);

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
}

void View3DInventorViewer::setGradientBackgroudColor( const SbColor& fromColor, const SbColor& toColor )
{
  pcBackGround->setColorGradient( fromColor, toColor );
}

void View3DInventorViewer::setEnabledFPSCounter(bool b)
{
#if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN) || defined(FC_OS_MACOSX)
  setenv("COIN_SHOW_FPS_COUNTER", (b?"1":"0"), 1);
#else
  b ? _putenv ("COIN_SHOW_FPS_COUNTER=1") : _putenv ("COIN_SHOW_FPS_COUNTER=0");
#endif
}

View3DInventorViewer::~View3DInventorViewer()
{
  // cleanup
  this->backgroundroot->unref();
  this->backgroundroot = 0;
  this->foregroundroot->unref();
  this->foregroundroot = 0;
  this->pcBackGround->unref();
  this->pcBackGround = 0;

  setSceneGraph(0);
  this->pEventCallback->unref();
  this->pEventCallback = 0;
  this->pcViewProviderRoot->unref();
  this->pcViewProviderRoot = 0;

  finalize();

  // Note: When closing the application the main window doesn't exists any more.
  if (getMainWindow()) 
    getMainWindow()->setPaneText(2, "");

  Gui::Selection().Detach(this);
}

void View3DInventorViewer::initialize()
{
  this->currentmode = View3DInventorViewer::IDLE;

  this->prevRedrawTime = SbTime::getTimeOfDay();
  this->spinanimatingallowed = TRUE;
  this->spinsamplecounter = 0;
  this->spinincrement = SbRotation::identity();

  // FIXME: use a smaller sphere than the default one to have a larger
  // area close to the borders that gives us "z-axis rotation"?
  // 19990425 mortene.
  this->spinprojector = new SbSphereSheetProjector(SbSphere(SbVec3f(0, 0, 0), 0.8f));
  SbViewVolume volume;
  volume.ortho(-1, 1, -1, 1, -1, 1);
  this->spinprojector->setViewVolume(volume);

  this->axiscrossEnabled = TRUE;
  this->axiscrossSize = 10;

  this->spinRotation.setValue(SbVec3f(0, 0, -1), 0);

  this->log.size = 16;
  this->log.position = new SbVec2s [ 16 ];
  this->log.time = new SbTime [ 16 ];
  this->log.historysize = 0;
  this->button1down = FALSE;
  this->button3down = FALSE;
  this->ctrldown = FALSE;
  this->shiftdown = FALSE;
}

void View3DInventorViewer::finalize()
{
  delete this->spinprojector;
  delete[] this->log.position;
  delete[] this->log.time;
}

void View3DInventorViewer::clearBuffer(void * userdata, SoAction * action)
{
  if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
    // do stuff specific for GL rendering here.
    glClear(GL_DEPTH_BUFFER_BIT);
  }
}

void View3DInventorViewer::makeScreenShot( const char* filename, int w, int h, int eBackgroundType, const char *comment ) const
{
  // if no valid color use the current background
  bool useBackground = false;
  SbViewportRegion vp(getViewportRegion());
  if(w>0 && h>0)
    vp.setWindowSize( (short)w, (short)h );

  //NOTE: To support pixels per inch we must use SbViewportRegion::setPixelsPerInch( ppi );
  //The default value is 72.0.
  //If we need to support grayscale images with must either use SoOffscreenRenderer::LUMINANCE or 
  //SoOffscreenRenderer::LUMINANCE_TRANSPARENCY. 
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
      renderer.setComponents(SoFCOffscreenRenderer::RGB_TRANSPARENCY );
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

void View3DInventorViewer::startPicking( View3DInventorViewer::ePickMode mode )
{
  if (pcMouseModel)
    return;
  
  switch (mode)
  {
  case Lasso:
    pcMouseModel = new PolyPickerMouseModel();
    break;
  case Rectangle:
    pcMouseModel = new SelectionMouseModel();
    break;
  case BoxZoom:
    pcMouseModel = new BoxZoomMouseModel();
    break;
  case Circle:
    pcMouseModel = new CirclePickerMouseModel();
    break;
  default:
    break;
  }

  if ( pcMouseModel )
    pcMouseModel->grabMouseModel(this);
}

void View3DInventorViewer::stopPicking()
{
  pcPolygon.clear();
  delete pcMouseModel; 
  pcMouseModel = 0;
}

bool View3DInventorViewer::isPicking() const
{
    return (pcMouseModel ? true : false);
}

bool View3DInventorViewer::dumpToFile( const char* filename, bool binary ) const
{
  bool ret = false;
  SoWriteAction wa;
  SoOutput* out = wa.getOutput();
  QFile::remove( filename );
  QFileInfo fi( filename );

  // Write VRML V2.0
  if ( fi.completeSuffix() == "wrl" || fi.completeSuffix() == "vrml" || fi.completeSuffix() == "wrz" ) {
    // If 'wrz' is set then force compression
    if ( fi.completeSuffix() == "wrz" )
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

// Documented in superclass. Overrides this method to be able to draw
// the axis cross, if selected, and to keep a continuous animation
// upon spin.
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

  if (this->isAnimating()) {
    SbRotation deltaRotation = this->spinRotation;
    deltaRotation.scaleAngle(secs * 5.0);
    this->reorientCamera(deltaRotation);
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

  if (bDrawAxisCross) { this->drawAxisCross(); }

  // Immediately reschedule to get continous spin animation.
  if (this->isAnimating()) { this->scheduleRedraw(); }

  printDimension();
}

void View3DInventorViewer::setSeekMode(SbBool on)
{
  // Overrides this method to make sure any animations are stopped
  // before we go into seek mode.

  // Note: this method is almost identical to the setSeekMode() in the
  // SoQtFlyViewer and SoQtPlaneViewer, so migrate any changes.

  if (this->isAnimating()) { this->stopAnimating(); }
  inherited::setSeekMode(on);
  this->setMode(on ? View3DInventorViewer::SEEK_WAIT_MODE :
                     (this->isViewing() ?
                     View3DInventorViewer::IDLE : View3DInventorViewer::INTERACT));
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

    float fHeight = static_cast<SoOrthographicCamera*>(getCamera())->height.getValue();
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
  switch(_iMouseModel)
  {
    case 0:return processSoEvent2(ev);break;
    case 1:return processSoEvent1(ev);break;
    default:return processSoEvent1(ev);
  }
}


SbBool View3DInventorViewer::processSoEvent2(const SoEvent * const ev)
{
  // If we're in picking mode then all events must be redirected to the
  // appropriate mouse model.
  if (pcMouseModel) {
    int hd=pcMouseModel->handleEvent(ev,this->getViewportRegion());
    if (hd==AbstractMouseModel::Continue||hd==AbstractMouseModel::Restart) {
      return TRUE;
    } else if (hd==AbstractMouseModel::Finish) {
      pcPolygon = pcMouseModel->getPolygon();
      delete pcMouseModel; pcMouseModel = 0;
    } else if (hd==AbstractMouseModel::Cancel) {
      pcPolygon.clear();
      delete pcMouseModel; pcMouseModel = 0;
    }
  }
  // Events when in "ready-to-seek" mode are ignored, except those
  // which influence the seek mode itself -- these are handled further
  // up the inheritance hierarchy.
  if (this->isSeekMode()) { return inherited::processSoEvent(ev); }

  const SoType type(ev->getTypeId());

  const SbVec2s size(this->getGLSize());
  const SbVec2f prevnormalized = this->lastmouseposition;
  const SbVec2s pos(ev->getPosition());
  const SbVec2f posn((float) pos[0] / (float) SoQtMax((int)(size[0] - 1), 1),
                     (float) pos[1] / (float) SoQtMax((int)(size[1] - 1), 1));

  this->lastmouseposition = posn;

  // Set to TRUE if any event processing happened. Note that it is not
  // necessary to restrict ourselves to only do one "action" for an
  // event, we only need this flag to see if any processing happened
  // at all.
  SbBool processed = FALSE;

  // If we are in editing mode then all events must be redirected to the view providers
  if (this->editing) {
    std::set<ViewProvider*>::iterator It;
    for (It=_ViewProviderSet.begin();It!=_ViewProviderSet.end() && !processed;It++)
      processed = (*It)->handleEvent(ev,*this);
  }

  if (processed)
    return TRUE;

  const ViewerMode currentmode = this->currentmode;
  ViewerMode newmode = currentmode;

  // Mismatches in state of the modifier keys happens if the user
  // presses or releases them outside the viewer window.
  if (this->ctrldown != ev->wasCtrlDown()) {
    this->ctrldown = ev->wasCtrlDown();
  }
  if (this->shiftdown != ev->wasShiftDown()) {
    this->shiftdown = ev->wasShiftDown();
  }

  // Keyboard handling
  if (type.isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
    const SoKeyboardEvent * const event = (const SoKeyboardEvent *) ev;
    const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;
    switch (event->getKey()) {
    case SoKeyboardEvent::LEFT_CONTROL:
    case SoKeyboardEvent::RIGHT_CONTROL:
      this->ctrldown = press;
      break;
    case SoKeyboardEvent::LEFT_SHIFT:
    case SoKeyboardEvent::RIGHT_SHIFT:
      this->shiftdown = press;
      break;
    case SoKeyboardEvent::H:
      processed = TRUE;
      this->saveHomePosition();
      break;
    case SoKeyboardEvent::Q: // ignore 'Q' keys (to prevent app from being closed)
      processed = TRUE;
      break;
    case SoKeyboardEvent::S:
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

  // Mouse Button / Spaceball Button handling
  if (type.isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
    processed = TRUE;

    const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
    const int button = event->getButton();
    const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

    // SoDebugError::postInfo("processSoEvent", "button = %d", button);
    switch (button) {
    case SoMouseButtonEvent::BUTTON1:
      this->button1down = press;
      if (press && (this->currentmode == View3DInventorViewer::SEEK_WAIT_MODE)) {
        newmode = View3DInventorViewer::SEEK_MODE;
        this->seekToPoint(pos); // implicitly calls interactiveCountInc()
      } else if (press && (this->currentmode == View3DInventorViewer::IDLE)) {
        setViewing(true);
      } else if (!press && (this->currentmode == View3DInventorViewer::DRAGGING)) {
        setViewing(false);
      } else if (this->currentmode == View3DInventorViewer::SELECTION) {
        processed = FALSE;
      }
      break;
    case SoMouseButtonEvent::BUTTON2:
      // If we are in zoom or pan mode ignore RMB events otherwise
      // the canvas doesn't get any release events 
      if (this->currentmode != View3DInventorViewer::ZOOMING && 
        this->currentmode != View3DInventorViewer::PANNING) {
        if (this->isPopupMenuEnabled()) {
          if (!press) { // release right mouse button
            this->openPopupMenu(event->getPosition());
          }
        }
      } break;
    case SoMouseButtonEvent::BUTTON3:
      this->button3down = press;
      break;
    case SoMouseButtonEvent::BUTTON4:
      if (press) zoom(this->getCamera(), 0.05f);
      break;
    case SoMouseButtonEvent::BUTTON5:
      if (press) zoom(this->getCamera(), -0.05f);
      break;
    default:
      break;
    }
  }

  // Mouse Movement handling
  if (type.isDerivedFrom(SoLocation2Event::getClassTypeId())) {
    const SoLocation2Event * const event = (const SoLocation2Event *) ev;

    processed = TRUE;

    if (this->currentmode == View3DInventorViewer::ZOOMING) {
      this->zoomByCursor(posn, prevnormalized);
    } else if (this->currentmode == View3DInventorViewer::PANNING) {
      pan(this->getCamera(), this->getGLAspectRatio(), this->panningplane, posn, prevnormalized);
    } else if (this->currentmode == View3DInventorViewer::DRAGGING) {
      this->addToLog(event->getPosition(), event->getTime());
      this->spin(posn);
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
      SbVec3f dir = event->getTranslation();
      camera->orientation.getValue().multVec(dir,dir);
      camera->position = camera->position.getValue() + dir;
      camera->orientation = 
        event->getRotation() * camera->orientation.getValue();
      processed = TRUE;
    }
  }

  enum {
    BUTTON1DOWN = 1 << 0,
    BUTTON3DOWN = 1 << 1,
    CTRLDOWN =    1 << 2,
    SHIFTDOWN =   1 << 3
  };
  unsigned int combo =
    (this->button1down ? BUTTON1DOWN : 0) |
    (this->button3down ? BUTTON3DOWN : 0) |
    (this->ctrldown ? CTRLDOWN : 0) |
    (this->shiftdown ? SHIFTDOWN : 0);

  switch (combo) {
  case 0:
    if (currentmode == View3DInventorViewer::SPINNING) { break; }
    newmode = View3DInventorViewer::IDLE;


    if ((currentmode == View3DInventorViewer::DRAGGING) && (this->log.historysize >= 3)) {
      SbTime stoptime = (ev->getTime() - this->log.time[0]);
      if (bAllowSpining && stoptime.getValue() < 0.100) {
        const SbVec2s glsize(this->getGLSize());
        SbVec3f from = this->spinprojector->project(SbVec2f(float(this->log.position[2][0]) / float(SoQtMax(glsize[0]-1, 1)),
                                                            float(this->log.position[2][1]) / float(SoQtMax(glsize[1]-1, 1))));
        SbVec3f to = this->spinprojector->project(posn);
        SbRotation rot = this->spinprojector->getRotation(from, to);

        SbTime delta = (this->log.time[0] - this->log.time[2]);
        double deltatime = delta.getValue();
        rot.invert();
        rot.scaleAngle(float(0.200 / deltatime));

        SbVec3f axis;
        float radians;
        rot.getValue(axis, radians);
        if ((radians > 0.01f) && (deltatime < 0.300)) {
          newmode = View3DInventorViewer::SPINNING;
          this->spinRotation = rot;
        }
      }
    }
    break;
  case BUTTON1DOWN:
    newmode = View3DInventorViewer::DRAGGING;
    break;
  case BUTTON3DOWN:
  case SHIFTDOWN|BUTTON1DOWN:
    newmode = View3DInventorViewer::PANNING;
    break;
  case CTRLDOWN:
  case CTRLDOWN|BUTTON1DOWN:
  case CTRLDOWN|SHIFTDOWN:
  case CTRLDOWN|SHIFTDOWN|BUTTON1DOWN:
    newmode = View3DInventorViewer::SELECTION;
    break;
  case BUTTON1DOWN|BUTTON3DOWN:
  case CTRLDOWN|BUTTON3DOWN:
    newmode = View3DInventorViewer::ZOOMING;
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
    if ((currentmode != View3DInventorViewer::SEEK_WAIT_MODE) &&
        (currentmode != View3DInventorViewer::SEEK_MODE)) {
      newmode = View3DInventorViewer::IDLE;
    }
    break;
  }

  if (newmode != currentmode) {
    this->setMode(newmode);
  }

  // give the nodes in the foreground root the chance to handle events (e.g color bar)
  // Note: this must be done _before_ ceding to the base class  
  if (!processed)
  {
    SoHandleEventAction action(getViewportRegion());
    action.setEvent(ev);
    action.apply(foregroundroot);
    processed = action.isHandled();
  }

  // If not handled in this class, pass on upwards in the inheritance
  // hierarchy.
  if (this->currentmode == View3DInventorViewer::SELECTION && !processed)
    processed = inherited::processSoEvent(ev);
  else
    return TRUE;

  // check for left click without selecting something
  if (this->currentmode == View3DInventorViewer::SELECTION && !processed) {
    if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
      SoMouseButtonEvent * const e = (SoMouseButtonEvent *) ev;
      if (SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1)) {
        Gui::Selection().clearSelection();
      }      
    }
  }

  return FALSE;
}

SbBool View3DInventorViewer::processSoEvent1(const SoEvent * const ev)
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
      //if (isEditing()) // in edit mode do not do interactions 
      //    break;
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
    if (pcMouseModel) {
      int hd=pcMouseModel->handleEvent(ev,this->getViewportRegion());
      if (hd==AbstractMouseModel::Continue||hd==AbstractMouseModel::Restart) {
        processed = true;
      } else if (hd==AbstractMouseModel::Finish) {
        pcPolygon = pcMouseModel->getPolygon();
        delete pcMouseModel; pcMouseModel = 0;
      } else if (hd==AbstractMouseModel::Cancel) {
        pcPolygon.clear();
        delete pcMouseModel; pcMouseModel = 0;
      }
    }
    if (!processed) {
      std::set<ViewProvider*>::iterator It;
      for(It=_ViewProviderSet.begin();It!=_ViewProviderSet.end() && !processed;It++)
        processed = (*It)->handleEvent(ev,*this);
    }
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

  // invokes the appropriate callback function when user interaction has started or finished
  bool bInteraction = (MoveMode||ZoomMode||RotMode|_bSpining);
  if (bInteraction && getInteractiveCount()==0)
    interactiveCountInc();
  // must not be in seek mode because it gets decremented in setSeekMode(FALSE)
  else if (!bInteraction&&!dCliBut3&&getInteractiveCount()>0&&!isSeekMode())
    interactiveCountDec();


  if(!processed)
    processed = inherited::processSoEvent(ev);
  else 
    return true;


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
          return true;
        }
      }
    }
  }

  // check for left click without selecting something
  if (!processed)
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
  // ask workbenches and view provider, ...
  MenuItem* view = new MenuItem;
  Gui::Application::Instance->setupContextMenu("View", view);

  QMenu ContextMenu(this->getGLWidget());
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
    SoClipPlaneManip* clip = new SoClipPlaneManip;

    SoGetBoundingBoxAction action(this->getViewportRegion());
    action.apply(this->getSceneGraph());
    SbBox3f box = action.getBoundingBox();

    if (!box.isEmpty()) {
      // adjust to overall bounding box of the scene
      clip->setValue(box, SbVec3f(0.0f,0.0f,1.0f), 1.0f);
    }

    pcViewProviderRoot->insertChild(clip,0);
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
  //return (pick ? pick->copy() : 0); // needs the same instance of CRT under MS Windows
  return (pick ? new SoPickedPoint(*pick) : 0);
}

void View3DInventorViewer::boxZoom(const SbBox2s& box)
{
    SoCamera* cam = this->getCamera();
    if (!cam) return; // no camera 
    SbViewVolume vv = cam->getViewVolume(getGLAspectRatio());

    short sizeX,sizeY;
    box.getSize(sizeX, sizeY);
    SbVec2s size = this->getGLSize();

    // The bbox must not be empty i.e. width and length is zero, but it is possible that
    // either width or length is zero
    if (sizeX == 0 && sizeY == 0) 
        return;

    // Get the new center in normalized pixel coordinates
    short xmin,xmax,ymin,ymax;
    box.getBounds(xmin,ymin,xmax,ymax);
    const SbVec2f center((float) ((xmin+xmax)/2) / (float) SoQtMax((int)(size[0] - 1), 1),
                         (float) (size[1]-(ymin+ymax)/2) / (float) SoQtMax((int)(size[1] - 1), 1));

    SbPlane plane = vv.getPlane(cam->focalDistance.getValue());
    pan(cam,getGLAspectRatio(),plane, SbVec2f(0.5,0.5), center);

    // Set height or height angle of the camera
    float scaleX = (float)sizeX/(float)size[0];
    float scaleY = (float)sizeY/(float)size[1];
    float scale = std::max<float>(scaleX, scaleY);
    if (cam && cam->getTypeId() == SoOrthographicCamera::getClassTypeId()) {
        float height = static_cast<SoOrthographicCamera*>(cam)->height.getValue() * scale;
        static_cast<SoOrthographicCamera*>(cam)->height = height;
    } else if (cam && cam->getTypeId() == SoPerspectiveCamera::getClassTypeId()) {
        float height = static_cast<SoPerspectiveCamera*>(cam)->heightAngle.getValue() / 2.0f;
        height = 2.0f * atan(tan(height) * scale);
        static_cast<SoPerspectiveCamera*>(cam)->heightAngle = height;
    }
}

void View3DInventorViewer::viewAll()
{
  // call the default implementation first to make sure everything is visible
  SoQtViewer::viewAll();
  
  // Get the bounding box of the scene
  SoGetBoundingBoxAction action(this->getViewportRegion());
  action.apply(this->getSceneGraph());
  SbBox3f box = action.getBoundingBox();
  if (box.isEmpty()) return;

#if 0
  // check whether the box is very wide or tall, if not do nothing
  float box_width, box_height, box_depth;
  box.getSize( box_width, box_height, box_depth );
  if ( box_width < 5.0f*box_height && box_width < 5.0f*box_depth && 
       box_height < 5.0f*box_width && box_height < 5.0f*box_depth && 
       box_depth < 5.0f*box_width && box_depth < 5.0f*box_height )
    return;
#endif

  SoCamera* cam = this->getCamera();
  if (!cam) return;

  SbViewVolume  vol = cam->getViewVolume();
  if (vol.ulf == vol.llf)
    return; // empty frustum (no view up vector defined)
  SbVec2f s = vol.projectBox(box);
  SbVec2s size = getSize();

  SbVec3f pt1, pt2, pt3, tmp;
  vol.projectPointToLine( SbVec2f(0.0f,0.0f), pt1, tmp );
  vol.projectPointToLine( SbVec2f(s[0],0.0f), pt2, tmp );
  vol.projectPointToLine( SbVec2f(0.0f,s[1]), pt3, tmp );

  float cam_width = (pt2-pt1).length();
  float cam_height = (pt3-pt1).length();

  // add a small border
  cam_height = 1.02f * std::max<float>((cam_width*(float)size[1])/(float)size[0],cam_height);

  float aspect = cam->aspectRatio.getValue();

  if (cam->getTypeId() == SoPerspectiveCamera::getClassTypeId()) {
    // set the new camera position dependent on the occupied space of projected bounding box
    //SbVec3f direction = cam->position.getValue() - box.getCenter();
    //float movelength = direction.length();
    //direction.normalize();
    //float fRatio = getViewportRegion().getViewportAspectRatio();
    //if ( fRatio > 1.0f ) {
    //  float factor = std::max<float>(s[0]/fRatio,s[1]);
    //  movelength = factor * movelength;
    //} else {
    //  float factor = std::max<float>(s[0],s[1]/fRatio);
    //  movelength = factor * movelength;
    //}
    //cam->position.setValue(box.getCenter() + direction * movelength);
  } else if (cam->getTypeId() == SoOrthographicCamera::getClassTypeId()) {
    SoOrthographicCamera* ocam = (SoOrthographicCamera *)cam;  // safe downward cast, knows the type
    if (aspect < 1.0f)
      ocam->height = cam_height / aspect;
    else
      ocam->height = cam_height;
  }
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

// Draw routines
void View3DInventorViewer::drawRect(int x1, int y1, int x2, int y2)
{
    // Make current context
    SbVec2s view = this->getGLSize();
    this->glLockNormal();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, view[0], 0, view[1], -1, 1);

    // Store GL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    GLfloat depthrange[2];
    glGetFloatv(GL_DEPTH_RANGE, depthrange);
    GLdouble projectionmatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);

    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_TRUE);
    glDepthRange(0,0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);

    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glDrawBuffer(GL_FRONT);
    glLineWidth(3.0f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x3F3F);
    glColor4f(1.0, 1.0, 0.0, 0.0);
    glViewport(0, 0, view[0], view[1]);

    glBegin(GL_LINE_LOOP);
        glVertex3i(x1, view[1]-y1, 0);
        glVertex3i(x2, view[1]-y1, 0);
        glVertex3i(x2, view[1]-y2, 0);
        glVertex3i(x1, view[1]-y2, 0);
    glEnd();

    glFlush();
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_COLOR_LOGIC_OP);

    // Reset original state
    glDepthRange(depthrange[0], depthrange[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projectionmatrix);

    glPopAttrib();
    glPopMatrix();
    
    // Release the context
    this->glUnlockNormal();
}

void View3DInventorViewer::drawLine (int x1, int y1, int x2, int y2)
{
    // Make current context
    SbVec2s view = this->getGLSize();
    this->glLockNormal();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, view[0], 0, view[1], -1, 1);

    // Store GL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    GLfloat depthrange[2];
    glGetFloatv(GL_DEPTH_RANGE, depthrange);
    GLdouble projectionmatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);

    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_TRUE);
    glDepthRange(0,0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);

    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glDrawBuffer(GL_FRONT);
    glLineWidth(3.0f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x3F3F);
    glColor4f(1.0, 1.0, 0.0, 0.0);
    glViewport(0, 0, view[0], view[1]);

    glBegin(GL_LINE_LOOP);
        glVertex3i(x1, view[1]-y1, 0);
        glVertex3i(x2, view[1]-y2, 0);
    glEnd();

    glFlush();
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_COLOR_LOGIC_OP);

    // Reset original state
    glDepthRange(depthrange[0], depthrange[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projectionmatrix);

    glPopAttrib();
    glPopMatrix();
    
    // Release the context
    this->glUnlockNormal();
}

void View3DInventorViewer::drawCircle (int x, int y, int r)
{
#if 0 //TODO
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
#endif
}

void View3DInventorViewer::drawText (int x, int y, const QString & str)
{
#if 0 //TODO
  if (p)
    p->drawText( x, y, str);
  else
  {
    QPainter p(getGLWidget());
    p.setPen( Qt::white );
    p.setRasterOp( QPainter::XorROP );
    drawText( x, y, str, &p );
  }
#endif
}

/*!
  Query if the model in the viewer is currently in spinning mode after
  a user drag.
*/
SbBool View3DInventorViewer::isAnimating(void) const
{
  return this->currentmode == View3DInventorViewer::SPINNING;
}

void View3DInventorViewer::stopAnimating(void)
{
  if (this->currentmode != View3DInventorViewer::SPINNING) {
    return;
  }
  this->setMode(this->isViewing() ? View3DInventorViewer::IDLE : View3DInventorViewer::INTERACT);
}

/*!
  Decide whether or not the mouse pointer cursor should be visible in
  the rendering canvas.
*/
void View3DInventorViewer::setCursorEnabled(SbBool enable)
{
  inherited::setCursorEnabled(enable);
  this->setCursorRepresentation(this->currentmode);
}

void View3DInventorViewer::afterRealizeHook(void)
{
  inherited::afterRealizeHook();
  this->setCursorRepresentation(this->currentmode);
}

// Documented in superclass. This method overridden from parent class
// to make sure the mouse pointer cursor is updated.
void View3DInventorViewer::setViewing(SbBool enable)
{
  if (this->isViewing() == enable) {
    return;
  }

  this->setMode(enable ? View3DInventorViewer::IDLE : View3DInventorViewer::INTERACT);
  inherited::setViewing(enable);
}

// The viewer is a state machine, and all changes to the current state
// are made through this call.
void View3DInventorViewer::setMode(const ViewerMode newmode)
{
  const ViewerMode oldmode = this->currentmode;
  if (newmode == oldmode) { return; }

  switch (newmode) {
  case DRAGGING:
    // Set up initial projection point for the projector object when
    // first starting a drag operation.
    this->spinprojector->project(this->lastmouseposition);
    this->interactiveCountInc();
    this->clearLog();
    break;

  case SPINNING:
    this->interactiveCountInc();
    this->scheduleRedraw();
    break;

  case PANNING:
    {
      // The plane we're projecting the mouse coordinates to get 3D
      // coordinates should stay the same during the whole pan
      // operation, so we should calculate this value here.
      SoCamera * cam = this->getCamera();
      if (cam == NULL) { // can happen for empty scenegraph
        this->panningplane = SbPlane(SbVec3f(0, 0, 1), 0);
      }
      else {
        SbViewVolume vv = cam->getViewVolume(this->getGLAspectRatio());
        this->panningplane = vv.getPlane(cam->focalDistance.getValue());
      }
    }
    this->interactiveCountInc();
    break;

  case ZOOMING:
    this->interactiveCountInc();
    break;

  default: // include default to avoid compiler warnings.
    break;
  }

  switch (oldmode) {
  case SPINNING:
  case DRAGGING:
  case PANNING:
  case ZOOMING:
    this->interactiveCountDec();
    break;

  default:
    break;
  }

  this->setCursorRepresentation(newmode);
  this->currentmode = newmode;
}

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
  SbVec2s view = this->getGLSize();
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
  SoCamera * cam = this->getCamera();

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
        if (isStereoViewing())
          glColor3f(0.500f, 0.5f, 0.5f);
        else
          glColor3f(0.500f, 0.125f, 0.125f);
      } else if (idx[i] == YAXIS) {                // Y axis.
        glRotatef(90, 0, 0, 1);
        if (isStereoViewing())
          glColor3f(0.400f, 0.4f, 0.4f);
        else
          glColor3f(0.125f, 0.500f, 0.125f);
      } else {                                     // Z axis.
        glRotatef(-90, 0, 1, 0);
        if (isStereoViewing())
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
void View3DInventorViewer::drawArrow(void)
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
void View3DInventorViewer::reorientCamera(const SbRotation & rot)
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

// ************************************************************************

// Calculate a zoom/dolly factor from the difference of the current
// cursor position and the last.
void View3DInventorViewer::zoomByCursor(const SbVec2f & thispos, const SbVec2f & prevpos)
{
  // There is no "geometrically correct" value, 20 just seems to give
  // about the right "feel".
  zoom(this->getCamera(), (thispos[1] - prevpos[1]) * 10.0f);
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

// ************************************************************************

#define HAND_WITH 24
#define HAND_HEIGHT 24
#define HAND_HOT_X 9
#define HAND_HOT_Y 0

static unsigned char hand_bitmap[] = {
 0x00,0x03,0x00,0x80,0x04,0x00,0x80,0x04,0x00,0x80,0x04,0x00,0x80,0x04,0x00,
 0x80,0x1c,0x00,0x80,0xe4,0x00,0x80,0x24,0x01,0x80,0x24,0x07,0x8e,0x24,0x09,
 0x92,0x24,0x09,0xa4,0x00,0x09,0xc4,0x00,0x08,0x08,0x00,0x08,0x08,0x00,0x08,
 0x10,0x00,0x08,0x10,0x00,0x04,0x20,0x00,0x04,0x20,0x00,0x04,0x40,0x00,0x02,
 0x80,0x00,0x02,0x00,0x01,0x01,0x00,0xff,0x01,0x00,0x00,0x00,0x00,0xab,0xab,
 0xab,0xab,0xab,0xab,0xab,0xab,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
 0x00,0x1b,0x00,0xee,0x04,0xee };

static unsigned char hand_mask_bitmap[] = {
 0x00,0x03,0x00,0x80,0x07,0x00,0x80,0x07,0x00,0x80,0x07,0x00,0x80,0x07,0x00,
 0x80,0x1f,0x00,0x80,0xff,0x00,0x80,0xff,0x01,0x80,0xff,0x07,0x8e,0xff,0x0f,
 0x9e,0xff,0x0f,0xbc,0xff,0x0f,0xfc,0xff,0x0f,0xf8,0xff,0x0f,0xf8,0xff,0x0f,
 0xf0,0xff,0x0f,0xf0,0xff,0x07,0xe0,0xff,0x07,0xe0,0xff,0x07,0xc0,0xff,0x03,
 0x80,0xff,0x03,0x00,0xff,0x01,0x00,0xff,0x01,0x00,0x00,0x00,0x00,0xab,0xab,
 0xab,0xab,0xab,0xab,0xab,0xab,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,
 0x00,0x1b,0x00,0xd5,0x07,0x1c };

// Set cursor graphics according to mode.
void View3DInventorViewer::setCursorRepresentation(int modearg)
{
  if (!this->isCursorEnabled()) {
    this->setComponentCursor(SoQtCursor::getBlankCursor());
    return;
  }

  switch (modearg) {
  case View3DInventorViewer::IDLE:
  case View3DInventorViewer::INTERACT:
    if (isEditing())
      this->getWidget()->setCursor(this->editCursor);
    else
      this->setComponentCursor(SoQtCursor(SoQtCursor::DEFAULT));
    break;

  case View3DInventorViewer::DRAGGING:
  case View3DInventorViewer::SPINNING:
    this->setComponentCursor(SoQtCursor::getRotateCursor());
    break;

  case View3DInventorViewer::ZOOMING:
    {
    this->setComponentCursor(SoQtCursor::getZoomCursor());
    }break;

  case View3DInventorViewer::SEEK_MODE:
  case View3DInventorViewer::SEEK_WAIT_MODE:
    this->setComponentCursor(SoQtCursor(SoQtCursor::CROSSHAIR));
    break;

  case View3DInventorViewer::PANNING:
    this->setComponentCursor(SoQtCursor::getPanCursor());
    break;

  case View3DInventorViewer::SELECTION:
    {
      SoQtCursor::CustomCursor custom;
      custom.dim.setValue(HAND_WITH, HAND_HEIGHT);
      custom.hotspot.setValue(HAND_HOT_X, HAND_HOT_Y);
      custom.bitmap = hand_bitmap;
      custom.mask = hand_mask_bitmap;
      this->setComponentCursor(SoQtCursor(&custom));
    }
    break;

  default: assert(0); break;
  }
}

void View3DInventorViewer::setEditing(SbBool edit) 
{ 
  this->editing = edit; 
  this->setComponentCursor(SoQtCursor(SoQtCursor::DEFAULT));
  this->editCursor = QCursor();
}

void View3DInventorViewer::setEditingCursor (const SoQtCursor& cursor)
{
  //Note: Coin caches the pointer to the CustomCursor instance with
  //the QCursor instance in a dictionary. So we must not store the
  //SoQtCursor object here but the QCursor object, otherwise we might
  //restore the wrong QCursor from the dictionary. 
  this->setComponentCursor(cursor);
  this->editCursor = this->getWidget()->cursor();
}

void View3DInventorViewer::addEventCallback(SoType eventtype, SoEventCallbackCB * cb, void* userdata)
{
    pEventCallback->addEventCallback(eventtype, cb, userdata);
}

void View3DInventorViewer::removeEventCallback(SoType eventtype, SoEventCallbackCB * cb, void* userdata)
{
    pEventCallback->removeEventCallback(eventtype, cb, userdata);
}

ViewProvider* View3DInventorViewer::getViewProviderByPath(SoPath * path) const
{
    for (std::set<ViewProvider*>::const_iterator it = _ViewProviderSet.begin(); it != _ViewProviderSet.end(); it++) {
        for (int i = 0; i<path->getLength();i++) {
            SoNode *node = path->getNode(i);
            if ((*it)->getRoot() == node) {
                return (*it);
            }
        }
    }

    return 0;
}

std::vector<ViewProvider*> View3DInventorViewer::getViewProvidersOfType(const Base::Type& typeId) const
{
    std::vector<ViewProvider*> views;
    for (std::set<ViewProvider*>::const_iterator it = _ViewProviderSet.begin(); it != _ViewProviderSet.end(); it++) {
        if ((*it)->getTypeId().isDerivedFrom(typeId)) {
            views.push_back(*it);
        }
    }
    return views;
}


