/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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
#endif
#if 0
#include "NavigationStyle.h"
#include "View3DInventorViewer.h"

using namespace Gui;

AbstractStyle::AbstractStyle()
{
}

AbstractStyle::~AbstractStyle()
{
}

SbBool AbstractStyle::processSoEvent(const SoEvent * const ev, View3DInventorViewer* view)
{
    return FALSE;
}

ExaminerStyle::ExaminerStyle()
{
}

ExaminerStyle::~ExaminerStyle()
{
}

SbBool ExaminerStyle::processSoEvent(const SoEvent * const ev, View3DInventorViewer* view)
{
    // Events when in "ready-to-seek" mode are ignored, except those
    // which influence the seek mode itself -- these are handled further
    // up the inheritance hierarchy.
//    if (this->isSeekMode()) { return inherited::processSoEvent(ev); }

    const SoType type(ev->getTypeId());

    const SbVec2s size;//(view->getGLSize());
    const SbVec2f prevnormalized = this->lastmouseposition;
    const SbVec2s pos(ev->getPosition());
    const SbVec2f posn((float) pos[0] / (float) SoQtMax((int)(size[0] - 1), 1),
                       (float) pos[1] / (float) SoQtMax((int)(size[1] - 1), 1));

    this->lastmouseposition = posn;
#if 0
    // Set to TRUE if any event processing happened. Note that it is not
    // necessary to restrict ourselves to only do one "action" for an
    // event, we only need this flag to see if any processing happened
    // at all.
    SbBool processed = FALSE;

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
      // If we are in edit mode then simply ignore the RMB events
      // to pass the event to the base class.
      if (this->editing) {
        processed = FALSE;
      }
      else {
        // If we are in zoom or pan mode ignore RMB events otherwise
        // the canvas doesn't get any release events 
        if (this->currentmode != View3DInventorViewer::ZOOMING && 
          this->currentmode != View3DInventorViewer::PANNING) {
          if (this->isPopupMenuEnabled()) {
            if (!press) { // release right mouse button
              this->openPopupMenu(event->getPosition());
            }
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
  if ((this->currentmode == View3DInventorViewer::SELECTION || this->editing) && !processed)
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
#endif
  return FALSE;
}
#endif
