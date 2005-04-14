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
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/events/SoEvent.h>
#endif

#include "View3DInventorViewer.h"
#include "../Base/Console.h"
#include "Tools.h"

#include <Inventor/events/SoMotion3Event.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

// build in Inventor

#include "Icons/default_background.xpm"

using namespace Gui;




View3DInventorViewer::View3DInventorViewer (QWidget *parent, const char *name, SbBool embed, Type type, SbBool build) 
  :SoQtViewer (parent, name, embed, type, build)
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

//  if ( filename )
//  {
    // if file specified load this file
//    img->filename = filename;
//  }
//  else
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


  // Render normal scenegraph.
  SoQtViewer::actualRedraw();


  // Increase arrow angle with 1/1000 ° every frame.
  arrowrotation->angle = arrowrotation->angle.getValue() + (0.001 / M_PI * 180);
  // Render overlay front scenegraph.
  glClear(GL_DEPTH_BUFFER_BIT);
  glra->apply(this->foregroundroot);
}

SbBool View3DInventorViewer::processSoEvent(const SoEvent * const ev)
{
  Base::Console().Log("Evnt: %s\n",ev->getTypeId().getName().getString());
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

  // Mouse Button / Spaceball Button handling

  if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {

    const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
    const int button = event->getButton();
    const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

    // SoDebugError::postInfo("processSoEvent", "button = %d", button);
    switch (button) {
    case SoMouseButtonEvent::BUTTON1:
      break;
    case SoMouseButtonEvent::BUTTON2:
      break;
    case SoMouseButtonEvent::BUTTON3:
      break;
    case SoMouseButtonEvent::BUTTON4:
      if (press) 
        View3DInventorViewer::zoom(getCamera(), 0.1f);
      break;
    case SoMouseButtonEvent::BUTTON5:
      if (press) 
        View3DInventorViewer::zoom(getCamera(), -0.1f);
      break;
    default:
      break;
    }
  }

  // Mouse Movement handling
  if (ev->getTypeId().isDerivedFrom(SoLocation2Event::getClassTypeId())) {
    const SoLocation2Event * const event = (const SoLocation2Event *) ev;

/*   if (PRIVATE(this)->currentmode == SoGuiExaminerViewerP::ZOOMING) {
      PRIVATE(this)->zoomByCursor(posn, prevnormalized);
    }
    else if (PRIVATE(this)->currentmode == SoGuiExaminerViewerP::PANNING) {
      SoGuiFullViewerP::pan(this->getCamera(), this->getGLAspectRatio(),
                            PRIVATE(this)->panningplane, posn, prevnormalized);
    }
    else if (PRIVATE(this)->currentmode == SoGuiExaminerViewerP::DRAGGING) {
      PRIVATE(this)->addToLog(event->getPosition(), event->getTime());
      PRIVATE(this)->spin(posn);
    }
    else {
      processed = FALSE;
    }*/
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
{/*
  if (this->log.historysize < 2) return;
  assert(this->spinprojector != NULL);

  SbVec2s glsize(PUBLIC(this)->getGLSize());
  SbVec2f lastpos;
  lastpos[0] = float(this->log.position[1][0]) / float(So@Gui@Max((int)(glsize[0]-1), 1));
  lastpos[1] = float(this->log.position[1][1]) / float(So@Gui@Max((int)(glsize[1]-1), 1));

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
  */
}

#if 0

SbBool
So@Gui@ExaminerViewer::processSoEvent(const SoEvent * const ev)
{
#if SO@GUI@_DEBUG && 0 // debug
  SoDebugError::postInfo("So@Gui@ExaminerViewer::processSoEvent",
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
  // the scenegraph, or caught by So@Gui@Viewer::processSoEvent() if
  // it's an ESC press (to switch modes).
  if (!this->isViewing()) { return inherited::processSoEvent(ev); }
    
  // Events when in "ready-to-seek" mode are ignored, except those
  // which influence the seek mode itself -- these are handled further
  // up the inheritance hierarchy.
  if (this->isSeekMode()) { return inherited::processSoEvent(ev); }

  const SoType type(ev->getTypeId());

  const SbVec2s size(this->getGLSize());
  const SbVec2f prevnormalized = PRIVATE(this)->lastmouseposition;
  const SbVec2s pos(ev->getPosition());
  const SbVec2f posn((float) pos[0] / (float) So@Gui@Max((int)(size[0] - 1), 1),
                     (float) pos[1] / (float) So@Gui@Max((int)(size[1] - 1), 1));

  PRIVATE(this)->lastmouseposition = posn;

  // Set to TRUE if any event processing happened. Note that it is not
  // necessary to restrict ourselves to only do one "action" for an
  // event, we only need this flag to see if any processing happened
  // at all.
  SbBool processed = FALSE;

  const SoGuiExaminerViewerP::ViewerMode currentmode = PRIVATE(this)->currentmode;
  SoGuiExaminerViewerP::ViewerMode newmode = currentmode;

  // Mismatches in state of the modifier keys happens if the user
  // presses or releases them outside the viewer window.
  if (PRIVATE(this)->ctrldown != ev->wasCtrlDown()) {
    PRIVATE(this)->ctrldown = ev->wasCtrlDown();
  }
  if (PRIVATE(this)->shiftdown != ev->wasShiftDown()) {
    PRIVATE(this)->shiftdown = ev->wasShiftDown();
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
      PRIVATE(this)->button1down = press;
      if (press && (currentmode == SoGuiExaminerViewerP::SEEK_WAIT_MODE)) {
        newmode = SoGuiExaminerViewerP::SEEK_MODE;
        this->seekToPoint(pos); // implicitly calls interactiveCountInc()
      }
      break;
    case SoMouseButtonEvent::BUTTON2:
      processed = FALSE; // pass on to superclass, so popup menu is shown
      break;
    case SoMouseButtonEvent::BUTTON3:
      PRIVATE(this)->button3down = press;
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
      PRIVATE(this)->ctrldown = press;
      break;
    case SoKeyboardEvent::LEFT_SHIFT:
    case SoKeyboardEvent::RIGHT_SHIFT:
      processed = TRUE;
      PRIVATE(this)->shiftdown = press;
      break;
    default:
      break;
    }
  }

  // Mouse Movement handling
  if (type.isDerivedFrom(SoLocation2Event::getClassTypeId())) {
    const SoLocation2Event * const event = (const SoLocation2Event *) ev;

    processed = TRUE;

    if (PRIVATE(this)->currentmode == SoGuiExaminerViewerP::ZOOMING) {
      PRIVATE(this)->zoomByCursor(posn, prevnormalized);
    }
    else if (PRIVATE(this)->currentmode == SoGuiExaminerViewerP::PANNING) {
      SoGuiFullViewerP::pan(this->getCamera(), this->getGLAspectRatio(),
                            PRIVATE(this)->panningplane, posn, prevnormalized);
    }
    else if (PRIVATE(this)->currentmode == SoGuiExaminerViewerP::DRAGGING) {
      PRIVATE(this)->addToLog(event->getPosition(), event->getTime());
      PRIVATE(this)->spin(posn);
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
      if (PRIVATE(this)->motion3OnCamera) {
        SbVec3f dir = event->getTranslation();
        camera->orientation.getValue().multVec(dir,dir);
        camera->position = camera->position.getValue() + dir;
        camera->orientation = 
          event->getRotation() * camera->orientation.getValue();
        processed = TRUE;
      }
      else {
        // FIXME: move/rotate model
#if SO@GUI@_DEBUG
        SoDebugError::postInfo("So@Gui@ExaminerViewer::processSoEvent",
                               "SoMotion3Event for model movement is not implemented yet");
#endif // SO@GUI@_DEBUG
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
    (PRIVATE(this)->button1down ? BUTTON1DOWN : 0) |
    (PRIVATE(this)->button3down ? BUTTON3DOWN : 0) |
    (PRIVATE(this)->ctrldown ? CTRLDOWN : 0) |
    (PRIVATE(this)->shiftdown ? SHIFTDOWN : 0);

  switch (combo) {
  case 0:
    if (currentmode == SoGuiExaminerViewerP::SPINNING) { break; }
    newmode = SoGuiExaminerViewerP::IDLE;
    if ((currentmode == SoGuiExaminerViewerP::DRAGGING) &&
        this->isAnimationEnabled() && (PRIVATE(this)->log.historysize >= 3)) {
      SbTime stoptime = (ev->getTime() - PRIVATE(this)->log.time[0]);
      if (stoptime.getValue() < 0.100) {
        const SbVec2s glsize(this->getGLSize());
        SbVec3f from = PRIVATE(this)->spinprojector->project(SbVec2f(float(PRIVATE(this)->log.position[2][0]) / float(So@Gui@Max(glsize[0]-1, 1)),
                                                                     float(PRIVATE(this)->log.position[2][1]) / float(So@Gui@Max(glsize[1]-1, 1))));
        SbVec3f to = PRIVATE(this)->spinprojector->project(posn);
        SbRotation rot = PRIVATE(this)->spinprojector->getRotation(from, to);

        SbTime delta = (PRIVATE(this)->log.time[0] - PRIVATE(this)->log.time[2]);
        double deltatime = delta.getValue();
        rot.invert();
        rot.scaleAngle(float(0.200 / deltatime));

        SbVec3f axis;
        float radians;
        rot.getValue(axis, radians);
        if ((radians > 0.01f) && (deltatime < 0.300)) {
          newmode = SoGuiExaminerViewerP::SPINNING;
          PRIVATE(this)->spinRotation = rot;
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
    PRIVATE(this)->setMode(newmode);
  }

  // If not handled in this class, pass on upwards in the inheritance
  // hierarchy.
  return processed || inherited::processSoEvent(ev);
}

#endif