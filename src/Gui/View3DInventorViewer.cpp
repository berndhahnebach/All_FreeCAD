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
# include <qimage.h>
# include <GL/gl.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCube.h>
# include <Inventor/nodes/SoImage.h>
# include <Inventor/nodes/SoLightModel.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
# include <Inventor/nodes/SoRotationXYZ.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoSelection.h>
# include <Inventor/actions/SoBoxHighlightRenderAction.h>
# include <Inventor/events/SoEvent.h>
#endif

#include "View3DInventorViewer.h"
#include "../Base/Console.h"
#include "Tools.h"
#include <qcursor.h>

#include <Inventor/events/SoMotion3Event.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/projectors/SbSphereSheetProjector.h>
#include <Inventor/projectors/SbSpherePlaneProjector.h>
#include <Inventor/actions/SoRayPickAction.h> 
#include <Inventor/SoPickedPoint.h> 

#include "ViewProvider.h"

// build in Inventor

#include "Icons/default_background.xpm"



using namespace Gui;



/// adds an ViewProvider to the view, e.g. from a feature
void View3DInventorViewer::addViewProvider(ViewProviderInventor* pcProvider)
{
  pcViewProviderRoot->addChild(pcProvider->getRoot());
  _ViewProviderSet.insert(pcProvider);
}

void View3DInventorViewer::removeViewProvider(ViewProviderInventor* pcProvider)
{
  pcViewProviderRoot->removeChild(pcProvider->getRoot());
  _ViewProviderSet.erase(pcProvider);

}
    


View3DInventorViewer::View3DInventorViewer (QWidget *parent, const char *name, SbBool embed, Type type, SbBool build) 
  :SoQtViewer (parent, name, embed, type, build)
{
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

  axiscrossEnabled = true;
  axiscrossSize = 10;

  spinRotation.setValue(SbVec3f(0, 0, -1), 0);

  log.size = 16;
  log.position = new SbVec2s [ 16 ];
  log.time = new SbTime [ 16 ];
  log.historysize = 0;



  // Set up background scenegraph with image in it.

  bckgroundroot = new SoSeparator;
  bckgroundroot->ref();

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

//  if ( filename )
//  {
    // if file specified load this file
//    img->filename = filename;
//  }
//  else
  {
    // otherwise take the default image and scale it up to desktop size
    QImage image( default_background );

    int w = QApplication::desktop()->width() + 20;
    int h = QApplication::desktop()->height() + 20;
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

/*  const double ARROWSIZE = 2.0;

  SoTranslation * posit = new SoTranslation;
  posit->translation = SbVec3f(-2.5 * ARROWSIZE, 1.5 * ARROWSIZE, 0);

  arrowrotation = new SoRotationXYZ;
  arrowrotation->axis = SoRotationXYZ::Z;

  SoTranslation * offset = new SoTranslation;
  offset->translation = SbVec3f(ARROWSIZE/2.0, 0, 0);

  SoCube * cube = new SoCube;
  cube->width = ARROWSIZE;
  cube->height = ARROWSIZE/15.0;
*/
  /*
  this->foregroundroot->addChild(cam);
  this->foregroundroot->addChild(lm);
  this->foregroundroot->addChild(bc);
  this->foregroundroot->addChild(posit);
  this->foregroundroot->addChild(arrowrotation);
  this->foregroundroot->addChild(offset);
  this->foregroundroot->addChild(cube);
  */

  // set the ViewProvider root
  pcSelection        = new SoSelection();
  pcSelection->addFinishCallback(View3DInventorViewer::sFinishSelectionCallback,this);
  pcSelection->addSelectionCallback( View3DInventorViewer::sMadeSelection, this );
  pcSelection->addDeselectionCallback( View3DInventorViewer::sUnmadeSelection, this );

  pcViewProviderRoot = new SoSeparator();
  pcSelection->addChild(pcViewProviderRoot);
  redrawOnSelectionChange(pcSelection);
  // is not realy working with Coin3D. 
//  redrawOverlayOnSelectionChange(pcSelection);
  setSceneGraph(pcSelection);

/* // Higlighthing of the Selection with Inventor stuff
  SoBoxHighlightRenderAction *pcRenderAction = new SoBoxHighlightRenderAction();
//  SoLineHighlightRenderAction *pcRenderAction = new SoLineHighlightRenderAction();
  pcRenderAction->setLineWidth(4);
  pcRenderAction->setColor(SbColor(1,1,0));
  pcRenderAction->setLinePattern(3);

  setGLRenderAction(pcRenderAction); 
  */

  // set the transperency and antialiasing settings
//  getGLRenderAction()->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  getGLRenderAction()->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND);
//  getGLRenderAction()->setSmoothing(true);

 }

View3DInventorViewer::~View3DInventorViewer()
{
  this->bckgroundroot->unref();
  this->foregroundroot->unref();
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
  glra->apply(this->bckgroundroot);

  SbTime now = SbTime::getTimeOfDay();
  double secs = now.getValue() -  prevRedrawTime.getValue();
  prevRedrawTime = now;

  if (_bSpining) {
    SbRotation deltaRotation = spinRotation;
    deltaRotation.scaleAngle(secs * 5.0);
    reorientCamera(deltaRotation);
  }

  // Render normal scenegraph.
  SoQtViewer::actualRedraw();


  // Increase arrow angle with 1/1000 ° every frame.
  //arrowrotation->angle = arrowrotation->angle.getValue() + (0.001 / M_PI * 180);
  // Render overlay front scenegraph.
  glClear(GL_DEPTH_BUFFER_BIT);
  glra->apply(this->foregroundroot);

  drawAxisCross();
  
  if (_bSpining)
    scheduleRedraw(); 
  
} 

SbBool View3DInventorViewer::processSoEvent(const SoEvent * const ev)
{
  //Base::Console().Log("Evnt: %s\n",ev->getTypeId().getName().getString());
  bool processed = false;

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
    default:
      break;
    }
  }

  static bool MoveMode=false;
  static bool ZoomMode=false;
  static bool RotMode =false;

  const SbVec2s size(this->getGLSize());
  const SbVec2f prevnormalized = lastmouseposition;
  const SbVec2s pos(ev->getPosition());
  const SbVec2f posn((float) pos[0] / (float) SoQtMax((int)(size[0] - 1), 1),
                     (float) pos[1] / (float) SoQtMax((int)(size[1] - 1), 1));
  SbVec2s MovePos;
  lastmouseposition = posn;

  // switching the mouse modes
  if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {

    const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
    const int button = event->getButton();
    const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

    // SoDebugError::postInfo("processSoEvent", "button = %d", button);
    switch (button) {
    case SoMouseButtonEvent::BUTTON1:
      if(MoveMode && press)
      {
        RotMode = true;
        ZoomMode = false;
        MoveTime = ev->getTime();

      // Set up initial projection point for the projector object when
      // first starting a drag operation.
        spinprojector->project(lastmouseposition);
        //interactiveCountInc();
        clearLog();
        
        getWidget()->setCursor( QCursor( 13 /*ArrowCursor*/) );
        processed = true;
      }else if(MoveMode && !press){
        RotMode = false; 
        
        SbTime tmp = (ev->getTime() - MoveTime);
        if (tmp.getValue() < 0.300) 
        {
          ZoomMode = true;
          getWidget()->setCursor( QCursor( 8 /*CrossCursor*/) );
        }else{
       
          ZoomMode = false;
          getWidget()->setCursor( QCursor( 9 /*ArrowCursor*/) );

          SbViewVolume vv = getCamera()->getViewVolume(getGLAspectRatio());
          panningplane = vv.getPlane(getCamera()->focalDistance.getValue());
       
          // check on start spining
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
              _bSpining = true;
              spinRotation = rot;
              MoveMode = false;
              getWidget()->setCursor( QCursor( 0 /*CrossCursor*/) );
            }
          }
        }
        processed = true;
      }
      break;
    case SoMouseButtonEvent::BUTTON2:
      break;
    case SoMouseButtonEvent::BUTTON3:
      if(press)
      {
        // check on double click
        SbTime tmp = (ev->getTime() - CenterTime);
        if (tmp.getValue() < 0.300) 
        {
          panToCenter(panningplane, posn);
        }else{
          CenterTime = ev->getTime();
          MoveMode = true;
          _bSpining = false;
          SbViewVolume vv = getCamera()->getViewVolume(getGLAspectRatio());
          panningplane = vv.getPlane(getCamera()->focalDistance.getValue());
          getWidget()->setCursor( QCursor( 9 /*ArrowCursor*/) );
        }
      }else{
        MoveMode = false;
        RotMode = false;
        ZoomMode = false;
        getWidget()->setCursor( QCursor( 0 /*CrossCursor*/) );
      }
      processed = true;
      break;
    case SoMouseButtonEvent::BUTTON4:
      if (press) 
        View3DInventorViewer::zoom(getCamera(), 0.05f);
      break;
    case SoMouseButtonEvent::BUTTON5:
      if (press) 
        View3DInventorViewer::zoom(getCamera(), -0.05f);
      break;
    default:
      break;
    }
  }

  // Mouse Movement handling
  if (ev->getTypeId().isDerivedFrom(SoLocation2Event::getClassTypeId())) {
    const SoLocation2Event * const event = (const SoLocation2Event *) ev;

    if(MoveMode && ZoomMode){
      zoom(getCamera(),(posn[1] - prevnormalized[1]) * 20.0f);
      processed = true;
    }else if(MoveMode && RotMode) {
      addToLog(ev->getPosition(), ev->getTime());
      spin(posn);

      processed = true;
    }else if(MoveMode) {
      pan(getCamera(),getGLAspectRatio(),panningplane, posn, prevnormalized);
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




  return processed || inherited::processSoEvent(ev);
}

bool View3DInventorViewer::pickPoint(const SbVec2s& pos,SbVec3f &point,SbVec3f &norm)
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
    return NULL;

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
  for(std::set<ViewProviderInventor*>::iterator It = _ViewProviderSet.begin();It!=_ViewProviderSet.end();It++)
    for(int i = 0; i<path->getLength();i++)
      if((*It)->getRoot() == path->getNodeFromTail(i))
      {
        (*It)->selected(this,path);
        return;
      }
}

void View3DInventorViewer::sUnmadeSelection(void *viewer,SoPath *path)
{
  static_cast<View3DInventorViewer*>(viewer)->unmadeSelection(path);
}

// Callback function triggered for deselection.
void View3DInventorViewer::unmadeSelection(  SoPath * path )
{
  for(std::set<ViewProviderInventor*>::iterator It = _ViewProviderSet.begin();It!=_ViewProviderSet.end();It++)
    for(int i = 0; i<path->getLength();i++)
      if((*It)->getRoot() == path->getNodeFromTail(i))
      {
        (*It)->unselected(this,path);
        return;
      }
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



#if 0

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
