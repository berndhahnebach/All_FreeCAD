/**************************************************************************\
 *
 *  This file is part of the Coin GUI toolkit libraries.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.  See the
 *  file LICENSE.LGPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you want to use this library with software that is incompatible
 *  licensewise with the LGPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/
#ifdef _MSC_VER
#	pragma warning( disable : 4065 )
#	pragma warning( disable : 4244 )
#endif

// @configure_input@

/*!
  \class SoQtFlyViewer Inventor/Qt/viewers/SoQtFlyViewer.h
  \brief The SoQtFlyViewer class implements controls for moving
  the camera in a "flying" motion.
  \ingroup viewers

  Controls:
  <ul>

  <li>Left mouse button increases the speed.</li>

  <li>Middle mouse button decreases the speed.</li>

  <li>Left and middle mouse button together sets the speed to zero.</li>

  <li>"s" puts the viewer in seek mode. Click some geometry with the
      left mouse button to start the seek zoom animation. (Hitting "s"
      again before clicking will cancel the seek operation.)</li>

  <li>"u" puts the viewer in up-vector pick mode. Click some geometry
      with the left mouse button to set the camera's up-vector to the
      normal vector of the face you pick.
      (Hitting "u" again before clicking will cancel the pick operation.)</li>

  <li>The control key stops the flying and lets you tilt the camera by moving
      the pointer.</li>

  </ul>
*/

/*
  FIXME:
  - animate camera when setting up-vector so the scene doesn't just
    suddenly change.
*/

// includes the FreeCAD configuration
#include "../../../Config.h"

#include <soqtdefs.h>
#include <Inventor/Qt/viewers/SoQtFlyViewer.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <string.h> // strlen() etc
#include <Inventor/Qt/SoQtCursor.h>


// ************************************************************************

#ifndef DOXYGEN_SKIP_THIS

class SoQtFlyViewerP {
public:
  SoQtFlyViewerP(SoQtFlyViewer * owner)
  {
    this->searcher = NULL;
    this->publ = owner;
    this->viewermode = FLYING;
    this->currentspeed = 0.0f;
    this->maxspeed = 0.0f;
    // the speed factor ought to be adjusted according to the scene
    // graph bounding box in case of enormous models
    this->speedfactor = 1.0f;
    this->stranslation = NULL;
    this->sscale = NULL;
    this->button1down = FALSE;
    this->button3down = FALSE;
    this->lctrldown = 0;
    this->rctrldown = 0;
    this->lshiftdown = FALSE;
    this->rshiftdown = FALSE;
    this->lastrender = NULL;
  }
  ~SoQtFlyViewerP();

  enum ViewerMode {
    FLYING, TILTING, WAITING_FOR_SEEK, WAITING_FOR_UP_PICK
  };

  void constructor(SbBool build);

  void dolly(const float delta) const;
  void updateCursorRepresentation(void); // in SoNativeFlyViewer.cpp
  void setMode(ViewerMode newmode);
  int getMode(void) { return this->viewermode; }

#define SOQT_MIN_STEP     0.3f
#define SOQT_INC_FACTOR   1.6f
#define SOQT_MAX_SPEED   20.0f

  SbTime * lastrender;
  float currentspeed, maxspeed, speedfactor;

  void updateSpeedIndicator(void);

  SbBool button1down, button3down;
  int lctrldown, rctrldown;
  SbBool lshiftdown, rshiftdown;

  SoSearchAction * searcher;

  SoNode * superimposition;
  SoCoordinate3 * sgeometry;
  SoScale * sscale, * crossscale;
  SoTranslation * stranslation, * crossposition;
  SoSwitch * smaxspeedswitch, * scurrentspeedswitch, * crossswitch;
  void superimpositionevent(SoAction * action);
  static void superimposition_cb(void * closure, SoAction * action);

  SbVec2s mouseloc, tiltpos;
  SbRotation tiltrotation;
  SbVec3f tiltup;

  SoNode * getSuperimpositionNode(const char * name);
private:
  SoQtFlyViewer * publ;
  int viewermode;
};

SoQtFlyViewerP::~SoQtFlyViewerP(void)
{
  if (this->searcher)
    delete this->searcher;
  if (this->lastrender)
    delete this->lastrender;
}

#define PRIVATE(o) (o->pimpl)
#define PUBLIC(o) (o->publ)


// Common constructor code.
void
SoQtFlyViewerP::constructor(SbBool build)
{
  PUBLIC(this)->setClassName(PUBLIC(this)->getDefaultWidgetName());

  static const char * superimposed[] = {
    "#Inventor V2.1 ascii",
    "",
    "Separator {",
    "  MaterialBinding {",
    "    value OVERALL",
    "  }",
    "  OrthographicCamera {",
    "    height 1",
    "    nearDistance 0",
    "    farDistance 1",
    "  }",
    "  DEF soQt->callback Callback { }",
    "  Separator {",
    "    DEF soQt->translation Translation {",
    "      translation 0 0 0",
    "    }",
    "    DEF soQt->scale Scale {",
    "      scaleFactor 1 1 1",
    "    }",
    "    DEF soQt->geometry Coordinate3 {",
    "      point [",
    "       -0.8 -0.04 0,",
    "       -0.8  0    0,",
    "       -0.8  0.04 0,",
    "        0   -0.04 0,",
    "        0    0    0,",
    "        0    0.04 0,",
    "        0.8 -0.04 0,",
    "        0.8  0    0,",
    "        0.8  0.04 0,", 
    "        0    0.02 0,", // idx 9
    "        0.8  0.02 0,",
    "        0.8 -0.02 0,",
    "        0   -0.02 0,",
    "        0    0.01 0,", // idx 13
    "        0.4  0.01 0,",
    "        0.4 -0.01 0,",
    "        0   -0.01 0",
    "      ]",
    "    }",
    "    DEF soQt->maxspeedswitch Switch {",
    "      whichChild -3",
    // max speed indicator
    "      Material {",
    "        emissiveColor 1 0 0",
    "      }",
    "      IndexedFaceSet {",
    "        coordIndex [",
    "          12, 11, 10, 9, -1",
    "        ]",
    "      }",
    "    }",
    // the coordinate system
    "    BaseColor {",
    "      rgb 1 1 1",
    "    }",
    "    IndexedLineSet {",
    "      coordIndex [",
    "        0, 2, -1,",
    "        3, 5, -1,",
    "        6, 8, -1,",
    "        1, 7, -1",
    "      ]",
    "    }",
    // current speed indicator
    "    DEF soQt->currentspeedswitch Switch {",
    "      whichChild -3",
    "      Material {",
    "        emissiveColor 0 0 1",
    "      }",
    "      IndexedFaceSet {",
    "        coordIndex [",
    "          16, 15, 14, 13, -1",
    "        ]",
    "      }",
    "    }",
    "  }",
    // cross
    "  DEF soQt->crossswitch Switch {",
    "    whichChild -1",
    "    DEF soQt->crossposition Translation {",
    "      translation 0 0 0",
    "    }",
    "    DEF soQt->crossscale Scale {",
    "      scaleFactor 1 1 1",
    "    }",
    "    BaseColor {",
    "      rgb 1 0 0",
    "    }",
    "    Coordinate3 {",
    "      point [",
    "        0 -1  0,",
    "        0  1  0,",
    "       -1  0  0,",
    "        1  0  0",
    "      ]",
    "    }",
    "    IndexedLineSet {",
    "      coordIndex [",
    "        0, 1, -1,",
    "        2, 3, -1",
    "      ]",
    "    }",
    "  }",
    "}",
    NULL
  };

  int i, bufsize;
  for (i = bufsize = 0; superimposed[i]; i++)
    bufsize += strlen(superimposed[i]) + 1;
  char * buf = new char [bufsize + 1];
  for (i = bufsize = 0; superimposed[i]; i++) {
    strcpy(buf + bufsize, superimposed[i]);
    bufsize += strlen(superimposed[i]);
    buf[bufsize] = '\n';
    bufsize++;
  }
  SoInput * input = new SoInput;
  input->setBuffer(buf, bufsize);
  SbBool ok = SoDB::read(input, this->superimposition);
  assert(ok);
  delete input;
  delete [] buf;
  this->superimposition->ref();

  this->sscale = (SoScale *)
    this->getSuperimpositionNode("soQt->scale");
  this->stranslation = (SoTranslation *)
    this->getSuperimpositionNode("soQt->translation");
  this->sgeometry = (SoCoordinate3 *)
    this->getSuperimpositionNode("soQt->geometry");
  this->smaxspeedswitch = (SoSwitch *)
    this->getSuperimpositionNode("soQt->maxspeedswitch");
  this->scurrentspeedswitch = (SoSwitch *)
    this->getSuperimpositionNode("soQt->currentspeedswitch");
  this->crossswitch = (SoSwitch *)
    this->getSuperimpositionNode("soQt->crossswitch");
  this->crossposition = (SoTranslation *)
    this->getSuperimpositionNode("soQt->crossposition");
  this->crossscale = (SoScale *)
    this->getSuperimpositionNode("soQt->crossscale");

  SoCallback * cb = (SoCallback *)
    this->getSuperimpositionNode("soQt->callback");
  cb->setCallback(SoQtFlyViewerP::superimposition_cb, this);

  this->updateSpeedIndicator();

  PUBLIC(this)->addSuperimposition(this->superimposition);
  PUBLIC(this)->setSuperimpositionEnabled(this->superimposition,TRUE);

  if (build) {
    QWidget* viewer = PUBLIC(this)->buildWidget(PUBLIC(this)->getParentWidget());
    PUBLIC(this)->setBaseWidget(viewer);
  }
}

// This method dollies the camera back and forth in the scene.
void
SoQtFlyViewerP::dolly(const float delta) const
{
  SoCamera * const camera = PUBLIC(this)->getCamera();
  if (camera == NULL) { return; } // if there's no scenegraph, for instance

  SbPlane walkplane(PUBLIC(this)->getUpDirection(), camera->position.getValue());
  SbVec3f campos = camera->position.getValue();
  SbVec3f camvec;
  camera->orientation.getValue().multVec(SbVec3f(0, 0, -1), camvec);
  SbLine cross(campos + camvec,
                campos + camvec + PUBLIC(this)->getUpDirection());
  SbVec3f intersect;
  walkplane.intersect(cross, intersect);
  SbVec3f dir = intersect - campos;
  dir.normalize();

  camera->position = campos - dir * delta;
}

// The viewer is a state machine, and all changes to the current state
// are made through this call.
void
SoQtFlyViewerP::setMode(ViewerMode newmode)
{
  switch (newmode) {
    // Assuming interesting things will happen here..
  default:
    break;
  }

  this->viewermode = newmode;
  this->updateCursorRepresentation();
}

// This method locates a named node in the superimposed scene.
SoNode *
SoQtFlyViewerP::getSuperimpositionNode(const char * name)
{
  if (! this->searcher)
    this->searcher = new SoSearchAction;
  searcher->reset();
  searcher->setName(SbName(name));
  searcher->setInterest(SoSearchAction::FIRST);
  searcher->setSearchingAll(TRUE);
  searcher->apply(this->superimposition);
  assert(searcher->getPath());
  return searcher->getPath()->getTail();
}

void
SoQtFlyViewerP::superimpositionevent(SoAction * action)
{
  if (!action->isOfType(SoGLRenderAction::getClassTypeId())) return;
  SbViewportRegion vpRegion =
    ((SoGLRenderAction *) action)->getViewportRegion();
  SbVec2s viewport = vpRegion.getViewportSizePixels();
  float aspect = float(viewport[0]) / float(viewport[1]);
  float factorx = 1.0f/float(viewport[1]) * 220.0f;
  float factory = factorx;
  if (aspect > 1.0f) {
    this->stranslation->translation.setValue(SbVec3f(0.0f, -0.4f, 0.0f));
  } else {
    this->stranslation->translation.setValue(SbVec3f(0.0f, -0.4f / aspect, 0.0f));
    factorx /= aspect;
    factory /= aspect;
  }
  if (viewport[0] > 500)
    factorx *= 500.0f / 400.0f;
  else
    factorx *= float(viewport[0]) / 400.0f;
  this->sscale->scaleFactor.setValue(SbVec3f(factorx, factory, 1.0f));

  if (this->getMode() == TILTING) {
    assert(this->crossposition != NULL);
    assert(this->crossscale != NULL);
    float tx = float(this->tiltpos[0]-float(viewport[0])/2.0f)/(float(viewport[0]));
    float ty = float(this->tiltpos[1]-float(viewport[1])/2.0f)/(float(viewport[1]));
    if (aspect > 1.0f) tx *= aspect;
    else ty /= aspect;
    this->crossposition->translation.setValue(SbVec3f(tx, ty, 0));

    float sx = (1.0f/float(viewport[0])) * 15.0f;
    float sy = (1.0f/float(viewport[1])) * 15.0f;
    if (aspect > 1.0f) sx *= aspect;
    else sy /= aspect;
    this->crossscale->scaleFactor.setValue(SbVec3f(sx, sy, 0));
  }
}

void
SoQtFlyViewerP::superimposition_cb(void * closure, SoAction * action)
{
  assert(closure != NULL);
  ((SoQtFlyViewerP *) closure)->superimpositionevent(action);
}

void
SoQtFlyViewerP::updateSpeedIndicator(void)
{
  assert(this->sgeometry != NULL);
  SbVec3f * points = this->sgeometry->point.startEditing();
  if (points[10][0] == 0.0f)
    this->smaxspeedswitch->whichChild.setValue(SO_SWITCH_ALL);
  if (points[14][0] == 0.0f)
    this->scurrentspeedswitch->whichChild.setValue(SO_SWITCH_ALL);
  points[10][0] = this->maxspeed / (SOQT_MAX_SPEED / 0.8f);
  points[11][0] = this->maxspeed / (SOQT_MAX_SPEED / 0.8f);
  points[14][0] = this->currentspeed / (SOQT_MAX_SPEED / 0.8f);
  points[15][0] = this->currentspeed / (SOQT_MAX_SPEED / 0.8f);
  this->sgeometry->point.finishEditing();
  if (this->maxspeed == 0.0f)
    this->smaxspeedswitch->whichChild.setValue(SO_SWITCH_NONE);
  if (this->currentspeed == 0.0f)
    this->scurrentspeedswitch->whichChild.setValue(SO_SWITCH_NONE);
}

// Set cursor graphics according to mode.
void
SoQtFlyViewerP::updateCursorRepresentation(void)
{
  if (!PUBLIC(this)->isCursorEnabled()) {
    PUBLIC(this)->setComponentCursor(SoQtCursor::getBlankCursor());
    return;
  }

  switch (this->viewermode) {
  case SoQtFlyViewerP::FLYING:
    PUBLIC(this)->setComponentCursor(SoQtCursor(SoQtCursor::DEFAULT));
    break;

  case SoQtFlyViewerP::WAITING_FOR_SEEK:
    PUBLIC(this)->setComponentCursor(SoQtCursor(SoQtCursor::CROSSHAIR));
    break;

  case SoQtFlyViewerP::WAITING_FOR_UP_PICK:
    PUBLIC(this)->setComponentCursor(SoQtCursor(SoQtCursor::UPARROW));
    break;

  case SoQtFlyViewerP::TILTING:
    PUBLIC(this)->setComponentCursor(SoQtCursor::getPanCursor());
    break;

  default:
    assert(0 && "unknown mode");
    break;
  }
}

#endif // DOXYGEN_SKIP_THIS

// ************************************************************************

SOQT_OBJECT_SOURCE(SoQtFlyViewer);

// ************************************************************************

/*!
  Public constructor.
*/
SoQtFlyViewer::SoQtFlyViewer(QWidget* parent,
                                   const char * name, 
                                   SbBool embed, 
                                   SoQtFullViewer::BuildFlag flag,
                                   SoQtViewer::Type type)
  : inherited(parent, name, embed, flag, type, FALSE)
{
  PRIVATE(this) = new SoQtFlyViewerP(this);
  PRIVATE(this)->constructor(TRUE);
}

// ************************************************************************

/*!
  Protected constructor, used by viewer components derived from the
  SoQtFlyViewer.
*/
SoQtFlyViewer::SoQtFlyViewer(QWidget* parent,
                                   const char * const name,
                                   SbBool embed, 
                                   SoQtFullViewer::BuildFlag flag, 
                                   SoQtViewer::Type type, 
                                   SbBool build)
  : inherited(parent, name, embed, flag, type, FALSE)
{
  PRIVATE(this) = new SoQtFlyViewerP(this);
  PRIVATE(this)->constructor(build);
}

// ************************************************************************

/*!
  Virtual constructor.
*/
SoQtFlyViewer::~SoQtFlyViewer()
{
  if (PRIVATE(this)->superimposition != NULL) {
    this->removeSuperimposition(PRIVATE(this)->superimposition);
    PRIVATE(this)->superimposition->unref();
    PRIVATE(this)->superimposition = NULL;
  }
  delete PRIVATE(this);
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::setViewing(SbBool enable)
{
  if (enable != this->isViewing()) {
    PRIVATE(this)->maxspeed = 0.0f;
    PRIVATE(this)->currentspeed = 0.0f;
  }
  inherited::setViewing(enable);
  this->setSuperimpositionEnabled(PRIVATE(this)->superimposition, enable);
  this->scheduleRedraw();
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::resetToHomePosition(void)
{
  PRIVATE(this)->maxspeed = 0.0f;
  PRIVATE(this)->currentspeed = 0.0f;
  inherited::resetToHomePosition();
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::viewAll(void)
{
  PRIVATE(this)->maxspeed = 0.0f;
  PRIVATE(this)->currentspeed = 0.0f;
  inherited::viewAll();
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::setCamera(SoCamera * camera)
{
  PRIVATE(this)->maxspeed = 0.0f;
  PRIVATE(this)->currentspeed = 0.0f;
  inherited::setCamera(camera);
  // FIXME: do something with up-direction?
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::setCursorEnabled(SbBool enable)
{
  inherited::setCursorEnabled(enable);
  PRIVATE(this)->updateCursorRepresentation();
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::setCameraType(SoType type)
{
  PRIVATE(this)->maxspeed = 0.0f;
  PRIVATE(this)->currentspeed = 0.0f;
  inherited::setCameraType(type);
  // FIXME: what else? 20010907 mortene.
}

// ************************************************************************

// doc in super
const char *
SoQtFlyViewer::getDefaultWidgetName(void) const
{
  static const char defaultWidgetName[] = "SoQtFlyViewer";
  return defaultWidgetName;
}

// ************************************************************************

// doc in super
const char *
SoQtFlyViewer::getDefaultTitle(void) const
{
  static const char defaultTitle[] = "Fly Viewer";
  return defaultTitle;
}

// ************************************************************************

// doc in super
const char *
SoQtFlyViewer::getDefaultIconTitle(void) const
{
  static const char defaultIconTitle[] = "Fly Viewer";
  return defaultIconTitle;
}

// ************************************************************************

// Documented in superclass.
SbBool
SoQtFlyViewer::processSoEvent(const SoEvent * const event)
{
  // Let the end-user toggle between camera-interaction mode
  // ("viewing") and scenegraph-interaction mode with ALT key(s).
  if (event->getTypeId().isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
    SoKeyboardEvent * ke = (SoKeyboardEvent *)event;
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
  if (!this->isViewing()) { return inherited::processSoEvent(event); }

  // Events when in "ready-to-seek" mode are ignored, except those
  // which influence the seek mode itself -- these are handled further
  // up the inheritance hierarchy.
  if (this->isSeekMode()) { return inherited::processSoEvent(event); }


  // Keyboard handling
  if (event->isOfType(SoKeyboardEvent::getClassTypeId())) {
    // FIXME: only in fly mode
    const SoKeyboardEvent * const ke = (const SoKeyboardEvent *) event;
    switch (ke->getState()) {
    case SoButtonEvent::UP:
      switch (ke->getKey()) {
      case SoKeyboardEvent::U:
        do {
          // either to switch to up-vector pick mode, or back to fly
          // mode if pick-mode already activated (ie cancel the
          // up-vector pick operation)
          SbBool uppickmode =
            PRIVATE(this)->getMode() == SoQtFlyViewerP::WAITING_FOR_UP_PICK;
          PRIVATE(this)->setMode(uppickmode ? SoQtFlyViewerP::FLYING :
                                    SoQtFlyViewerP::WAITING_FOR_UP_PICK);
          PRIVATE(this)->maxspeed = 0.0f;
          PRIVATE(this)->currentspeed = 0.0f;
          PRIVATE(this)->updateSpeedIndicator();
          this->scheduleRedraw();
          return TRUE;
        } while (FALSE);
        break;

      case SoKeyboardEvent::S:
          PRIVATE(this)->maxspeed = 0.0f;
          PRIVATE(this)->currentspeed = 0.0f;
          PRIVATE(this)->updateSpeedIndicator();
          this->scheduleRedraw();
          return inherited::processSoEvent(event);

      case SoKeyboardEvent::LEFT_SHIFT:
        PRIVATE(this)->lshiftdown = FALSE;
        if (PRIVATE(this)->lshiftdown < 0) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "left shift key count < 0");
#endif
          PRIVATE(this)->lshiftdown = 0;
        }
        break;
      case SoKeyboardEvent::RIGHT_SHIFT:
        PRIVATE(this)->rshiftdown = FALSE;
        if (PRIVATE(this)->rshiftdown < 0) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "right shift key count < 0");
#endif
          PRIVATE(this)->rshiftdown = 0;
        }
        break;
      case SoKeyboardEvent::LEFT_CONTROL:
        PRIVATE(this)->lctrldown -= 1;
        if (PRIVATE(this)->lctrldown < 0) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "left control key count < 0");
#endif
          PRIVATE(this)->lctrldown = 0;
        }
        break;
      case SoKeyboardEvent::RIGHT_CONTROL:
        PRIVATE(this)->rctrldown -= 1;
        if (PRIVATE(this)->rctrldown < 0) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "right control key count < 0");
#endif
          PRIVATE(this)->rctrldown = 0;
        }
        break;
      default:
        break;
      }
      break;
    case SoButtonEvent::DOWN:
      switch (ke->getKey()) {
      case SoKeyboardEvent::LEFT_SHIFT:
        PRIVATE(this)->lshiftdown += 1;
        if (PRIVATE(this)->lshiftdown > 2) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "left shift key count > 2");
#endif
          PRIVATE(this)->lshiftdown = 2;
        }
        break;
      case SoKeyboardEvent::RIGHT_SHIFT:
        PRIVATE(this)->rshiftdown += 1;
        if (PRIVATE(this)->rshiftdown > 2) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "right shift key count > 2");
#endif
          PRIVATE(this)->rshiftdown = 2;
        }
        break;
      case SoKeyboardEvent::LEFT_CONTROL:
        PRIVATE(this)->lctrldown += 1;
        if (PRIVATE(this)->lctrldown > 2) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "left control key count > 2");
#endif
          PRIVATE(this)->lctrldown = 2;
        }
        break;
      case SoKeyboardEvent::RIGHT_CONTROL:
        PRIVATE(this)->rctrldown += 1;
        if (PRIVATE(this)->rctrldown > 2) {
#if SOQT_DEBUG
          SoDebugError::post("SoQtFlyViewer::processSoEvent",
                             "right control key count > 2");
#endif
          PRIVATE(this)->rctrldown = 2;
        }
        break;
      default:
        break; 
      }
      break;
    default:
      break;
    }

    if ((PRIVATE(this)->getMode() == SoQtFlyViewerP::FLYING) &&
         (PRIVATE(this)->lctrldown || PRIVATE(this)->rctrldown)) {
      PRIVATE(this)->setMode(SoQtFlyViewerP::TILTING);
      PRIVATE(this)->tiltrotation = this->getCamera()->orientation.getValue();
      PRIVATE(this)->tiltup = this->getUpDirection();
      PRIVATE(this)->tiltpos = PRIVATE(this)->mouseloc;
      PRIVATE(this)->currentspeed = 0.0f;
      PRIVATE(this)->maxspeed = 0.0f;
      PRIVATE(this)->updateSpeedIndicator();
      PRIVATE(this)->crossswitch->whichChild.setValue(SO_SWITCH_ALL);
      this->scheduleRedraw();
      // NOTE; this could be optimized to only draw the superimposition in
      // question if speed is zero.
    } else if ((PRIVATE(this)->getMode() == SoQtFlyViewerP::TILTING) &&
                !PRIVATE(this)->lctrldown && !PRIVATE(this)->rctrldown) {
      PRIVATE(this)->setMode(SoQtFlyViewerP::FLYING);
      assert(PRIVATE(this)->crossswitch != NULL);
      PRIVATE(this)->crossswitch->whichChild.setValue(SO_SWITCH_NONE);
      this->scheduleRedraw();
    }
  }

  // Mousebutton handling
  else if (event->isOfType(SoMouseButtonEvent::getClassTypeId())) {
    // FIXME: only for fly mode
    const SoMouseButtonEvent * const me = (const SoMouseButtonEvent *) event;
    switch (PRIVATE(this)->getMode()) {
    case SoQtFlyViewerP::WAITING_FOR_UP_PICK:
      if ((me->getButton() == SoMouseButtonEvent::BUTTON1) &&
           (me->getState() == SoButtonEvent::DOWN)) {
        this->findUpDirection(event->getPosition());
        PRIVATE(this)->setMode(SoQtFlyViewerP::FLYING);
        return TRUE;
      }
      break;
    case SoQtFlyViewerP::FLYING:
      switch (me->getButton()) {
      case SoMouseButtonEvent::BUTTON1:
        switch (me->getState()) {
        case SoButtonEvent::DOWN:
          PRIVATE(this)->button1down = TRUE;
          if (PRIVATE(this)->button3down) {
            PRIVATE(this)->currentspeed = 0.0f;
            PRIVATE(this)->maxspeed = 0.0f;
          } else {
            if (PRIVATE(this)->maxspeed == 0)
              PRIVATE(this)->maxspeed = SOQT_MIN_STEP;
            else if ((PRIVATE(this)->maxspeed >= -SOQT_MIN_STEP) &&
                      (PRIVATE(this)->maxspeed < 0.0f))
              PRIVATE(this)->maxspeed = 0.0f;
            else if (PRIVATE(this)->maxspeed > 0)
              PRIVATE(this)->maxspeed *= SOQT_INC_FACTOR;
            else
              PRIVATE(this)->maxspeed /= SOQT_INC_FACTOR;
            if (PRIVATE(this)->maxspeed > SOQT_MAX_SPEED)
              PRIVATE(this)->maxspeed = SOQT_MAX_SPEED;
            else if (PRIVATE(this)->maxspeed < -SOQT_MAX_SPEED)
              PRIVATE(this)->maxspeed = -SOQT_MAX_SPEED;
          }
          PRIVATE(this)->updateSpeedIndicator();
          this->scheduleRedraw();
          return TRUE;
        case SoButtonEvent::UP:
          PRIVATE(this)->button1down = FALSE;
          return TRUE;
        default:
          break;
        }
        break;
      case SoMouseButtonEvent::BUTTON3:
        switch (me->getState()) {
        case SoButtonEvent::DOWN:
          PRIVATE(this)->button3down = TRUE;
          if (PRIVATE(this)->button1down) {
            PRIVATE(this)->currentspeed = 0.0f;
            PRIVATE(this)->maxspeed = 0.0f;
          } else {
            if ((PRIVATE(this)->maxspeed > 0.0f) &&
                 (PRIVATE(this)->maxspeed <= SOQT_MIN_STEP))
              PRIVATE(this)->maxspeed = 0.0f;
            else if (PRIVATE(this)->maxspeed == 0.0f)
              PRIVATE(this)->maxspeed = -SOQT_MIN_STEP;
            else if (PRIVATE(this)->maxspeed > 0)
              PRIVATE(this)->maxspeed /= SOQT_INC_FACTOR;
            else
              PRIVATE(this)->maxspeed *= SOQT_INC_FACTOR;
            if (PRIVATE(this)->maxspeed > SOQT_MAX_SPEED)
              PRIVATE(this)->maxspeed = SOQT_MAX_SPEED;
            else if (PRIVATE(this)->maxspeed < -SOQT_MAX_SPEED)
              PRIVATE(this)->maxspeed = -SOQT_MAX_SPEED;
          }
          PRIVATE(this)->updateSpeedIndicator();
          this->scheduleRedraw();
          return TRUE;
        case SoButtonEvent::UP:
          PRIVATE(this)->button3down = FALSE;
          return TRUE;
        default:
          break;
        }
        break;
      default:
        break;
      }
    default:
      break;
    }
  }

  else if (event->isOfType(SoLocation2Event::getClassTypeId())) {
    if (PRIVATE(this)->getMode() == SoQtFlyViewerP::TILTING) {
      SbVec2s newpos = ((SoLocation2Event *) event)->getPosition();
      float dx = newpos[0] - PRIVATE(this)->tiltpos[0];
      float dy = newpos[1] - PRIVATE(this)->tiltpos[1];
      if (dx != 0.0f || dy != 0.0f) {
        SoCamera * camera = this->getCamera();
        if (camera == NULL) { return TRUE; } // probably sceneless
        SbVec3f up(PRIVATE(this)->tiltup);
        SbVec3f dir;
        SbRotation orientation(PRIVATE(this)->tiltrotation);
        orientation.multVec(SbVec3f(0.0f,0.0f,-1.0f), dir);
        SbVec3f horizon(dir.cross(up));
        horizon.normalize();
        SbRotation tilt = SbRotation(horizon, dy / 100.0f);
        SbVec3f newup;
        tilt.multVec(up, newup);
        SbRotation newOrientation = PRIVATE(this)->tiltrotation;
        newOrientation *= tilt;
        newOrientation *= SbRotation(newup, -dx / 100.0f);
        camera->orientation.setValue(newOrientation);
        this->setUpDirection(newup);
      }
    }
    PRIVATE(this)->mouseloc = ((SoLocation2Event *) event)->getPosition();
    return TRUE;
  }

  return inherited::processSoEvent(event);
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::setSeekMode(SbBool enable)
{
  // Note: this method is almost identical to the setSeekMode() in the
  // SoQtExaminerViewer, so migrate any changes.

#if SOQT_DEBUG
  if (enable == this->isSeekMode()) {
    SoDebugError::postWarning("SoQtFlyViewer::setSeekMode",
                              "seek mode already %sset", enable ? "" : "un");
    return;
  }
#endif // SOQT_DEBUG

  // FIXME: what if we're in the middle of a seek already? 20010910 mortene.
  // larsa - either stop the seek (on false) or reset timer to two new secs

  inherited::setSeekMode(enable);
  PRIVATE(this)->setMode(enable ? SoQtFlyViewerP::WAITING_FOR_SEEK :
                            SoQtFlyViewerP::FLYING);
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::actualRedraw(void)
{
  if (!this->isViewing()) {
    inherited::actualRedraw();
    return;
  }

  switch (PRIVATE(this)->getMode()) {
  case SoQtFlyViewerP::FLYING:
    do {
      SbVec2s glsize(this->getGLSize());
      SbVec2s mid(glsize[0]/2, glsize[1]/2);
      float xfrac =
        float(mid[0]-PRIVATE(this)->mouseloc[0]) / float(mid[0]);
      float yfrac =
        float(mid[1]-PRIVATE(this)->mouseloc[1]) / float(mid[1]);
      float factor = xfrac * xfrac + yfrac * yfrac;
      if (factor > 0.99f) {
        factor = 0.01f;
      } else {
        factor = 1.0f - factor;
      }

      SbTime thisrender;
      thisrender.setToTimeOfDay();
      float t = 0.0f;
      if (PRIVATE(this)->lastrender) {
        if (PRIVATE(this)->currentspeed == 0.0f)
          PRIVATE(this)->lastrender->setValue(thisrender.getValue() - 0.01);
        t = (thisrender.getValue() - PRIVATE(this)->lastrender->getValue()) * 10.0f;
        if (t >= 1.0f) t = 1.0f;
      }
      PRIVATE(this)->currentspeed +=
        (((PRIVATE(this)->currentspeed +
           PRIVATE(this)->maxspeed * factor) / 2.0f)
         - PRIVATE(this)->currentspeed) * t;

      // need some lower epsilon since the above may not get down to 0...
      if ((PRIVATE(this)->currentspeed <  0.001f) &&
           (PRIVATE(this)->currentspeed > -0.001f))
        PRIVATE(this)->currentspeed = 0.0f;
        
      PRIVATE(this)->updateSpeedIndicator();

      if (PRIVATE(this)->currentspeed != 0.0f) {
        if (PRIVATE(this)->lastrender) {
          t = (thisrender.getValue() - PRIVATE(this)->lastrender->getValue()) * 2.0f;
          if (t > 0.0f) {
            SoCamera * camera = this->getCamera();
            if (camera) { // could be a sceneless viewer
              SbVec3f up(this->getUpDirection());
              SbVec3f dir;
              SbRotation orientation(camera->orientation.getValue());
              orientation.multVec(SbVec3f(0.0f,0.0f,-1.0f), dir);
              camera->position.setValue(camera->position.getValue() +
                                        dir * (PRIVATE(this)->currentspeed * t * PRIVATE(this)->speedfactor));
              float xfactor = xfrac * xfrac / 2.0f;
              SbVec3f horizon(dir.cross(up));
              horizon.normalize();

              // FIXME: roll instead of turn if either shift button is
              // down if it seems like a good idea
              if (xfrac < 0.0f) xfactor = -xfactor;
              orientation *= SbRotation(this->getUpDirection(), xfactor * t);

              // set tilt
              SbRotation tilt = SbRotation(horizon, yfrac * yfrac * 0.002f);
              if (yfrac >= 0.0f) tilt.invert();
              orientation *= tilt;
              SbVec3f newUp;
              tilt.multVec(up, newUp);
              this->setUpDirection(newUp);
              camera->orientation.setValue(orientation);
            }
          }
        }
      }
      inherited::actualRedraw();
      if (!PRIVATE(this)->lastrender)
        PRIVATE(this)->lastrender = new SbTime;
      PRIVATE(this)->lastrender->setValue(thisrender.getValue());
      if (PRIVATE(this)->currentspeed != 0.0f ||
           PRIVATE(this)->maxspeed != 0.0f)
        this->scheduleRedraw();
    } while (FALSE);
    break;
  default:
    inherited::actualRedraw();
    break;
  }
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::rightWheelMotion(float value)
{
  PRIVATE(this)->dolly(value - this->getRightWheelValue());
  inherited::rightWheelMotion(value);
}

// ************************************************************************

// doc in super
void
SoQtFlyViewer::afterRealizeHook(void)
{
  PRIVATE(this)->updateCursorRepresentation();
  inherited::afterRealizeHook();
}

// ************************************************************************
