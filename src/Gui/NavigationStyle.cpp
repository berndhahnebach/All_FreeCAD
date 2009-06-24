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
# include <cfloat>
# include "InventorAll.h"
#endif

#include "NavigationStyle.h"
#include "View3DInventorViewer.h"

using namespace Gui;

NavigationStyle::NavigationStyle()
{
    this->currentmode = NavigationStyle::IDLE;
    this->prevRedrawTime = SbTime::getTimeOfDay();
    this->spinanimatingallowed = TRUE;
    this->spinsamplecounter = 0;
    this->spinincrement = SbRotation::identity();
    this->spinRotation.setValue(SbVec3f(0, 0, -1), 0);
    // FIXME: use a smaller sphere than the default one to have a larger
    // area close to the borders that gives us "z-axis rotation"?
    // 19990425 mortene.
    this->spinprojector = new SbSphereSheetProjector(SbSphere(SbVec3f(0, 0, 0), 0.8f));
    SbViewVolume volume;
    volume.ortho(-1, 1, -1, 1, -1, 1);
    this->spinprojector->setViewVolume(volume);

    this->log.size = 16;
    this->log.position = new SbVec2s [ 16 ];
    this->log.time = new SbTime [ 16 ];
    this->log.historysize = 0;
}

NavigationStyle::~NavigationStyle()
{
    delete this->spinprojector;
    delete[] this->log.position;
    delete[] this->log.time;
}

/** Rotate the camera by the given amount, then reposition it so we're
 * still pointing at the same focal point.
 */
void NavigationStyle::reorientCamera(SoCamera * cam, const SbRotation & rot)
{
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

void NavigationStyle::panCamera(SoCamera * cam, float aspectratio, const SbPlane & panplane,
                                const SbVec2f & prevpos, const SbVec2f & currpos)
{
    if (cam == NULL) return; // can happen for empty scenegraph
    if (currpos == prevpos) return; // useless invocation


    // Find projection points for the last and current mouse coordinates.
    SbViewVolume vv = cam->getViewVolume(aspectratio);
    SbLine line;
    vv.projectPointToLine(currpos, line);
    SbVec3f current_planept;
    panplane.intersect(line, current_planept);
    vv.projectPointToLine(prevpos, line);
    SbVec3f old_planept;
    panplane.intersect(line, old_planept);

    // Reposition camera according to the vector difference between the
    // projected points.
    cam->position = cam->position.getValue() - (current_planept - old_planept);
}

void NavigationStyle::pan(SoCamera* camera)
{
    // The plane we're projecting the mouse coordinates to get 3D
    // coordinates should stay the same during the whole pan
    // operation, so we should calculate this value here.
    if (camera == NULL) { // can happen for empty scenegraph
        this->panningplane = SbPlane(SbVec3f(0, 0, 1), 0);
    }
    else {
        const SbViewportRegion & vp = viewer->getViewportRegion();
        SbViewVolume vv = camera->getViewVolume(vp.getViewportAspectRatio());
        this->panningplane = vv.getPlane(camera->focalDistance.getValue());
    }
}

void NavigationStyle::panToCenter(const SbPlane & pplane, const SbVec2f & currpos)
{
    const SbViewportRegion & vp = viewer->getViewportRegion();
    float ratio = vp.getViewportAspectRatio();
    panCamera(viewer->getCamera(), ratio, pplane, SbVec2f(0.5,0.5), currpos);
}

/** Dependent on the camera type this will either shrink or expand the
 * height of the viewport (orthogonal camera) or move the camera
 * closer or further away from the focal point in the scene.
 *
 * Used from both SoGuiPlaneViewer and SoGuiExaminerViewer.
 * Implemented in the SoGuiFullViewer private class to collect common
 * code.
 */
void NavigationStyle::zoom(SoCamera * cam, float diffvalue)
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
 /*         static SbBool first = TRUE;
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

// Calculate a zoom/dolly factor from the difference of the current
// cursor position and the last.
void NavigationStyle::zoomByCursor(const SbVec2f & thispos, const SbVec2f & prevpos)
{
    // There is no "geometrically correct" value, 20 just seems to give
    // about the right "feel".
    zoom(viewer->getCamera(), (thispos[1] - prevpos[1]) * 10.0f/*20.0f*/);
}

/** Uses the sphere sheet projector to map the mouseposition onto
 * a 3D point and find a rotation from this and the last calculated point.
 */
void NavigationStyle::spin(const SbVec2f & pointerpos)
{
    if (this->log.historysize < 2) return;
    assert(this->spinprojector != NULL);

    const SbViewportRegion & vp = viewer->getViewportRegion();
    SbVec2s glsize(vp.getViewportSizePixels());
    SbVec2f lastpos;
    lastpos[0] = float(this->log.position[1][0]) / float(SoQtMax((int)(glsize[0]-1), 1));
    lastpos[1] = float(this->log.position[1][1]) / float(SoQtMax((int)(glsize[1]-1), 1));

    this->spinprojector->project(lastpos);
    SbRotation r;
    this->spinprojector->projectAndGetRotation(pointerpos, r);
    r.invert();
    this->reorientCamera(viewer->getCamera(), r);

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

NavigationStyle::ViewerMode NavigationStyle::doSpin()
{
    if (this->log.historysize >= 3) {
        SbTime stoptime = (SbTime::getTimeOfDay() - this->log.time[0]);
        if (this->spinanimatingallowed && stoptime.getValue() < 0.100) {
            const SbViewportRegion & vp = viewer->getViewportRegion();
            const SbVec2s glsize(vp.getViewportSizePixels());
            SbVec3f from = this->spinprojector->project(SbVec2f(float(this->log.position[2][0]) / float(SoQtMax(glsize[0]-1, 1)),
                                                                float(this->log.position[2][1]) / float(SoQtMax(glsize[1]-1, 1))));
            SbVec3f to = this->spinprojector->project(this->lastmouseposition);
            SbRotation rot = this->spinprojector->getRotation(from, to);

            SbTime delta = (this->log.time[0] - this->log.time[2]);
            double deltatime = delta.getValue();
            rot.invert();
            rot.scaleAngle(float(0.200 / deltatime));

            SbVec3f axis;
            float radians;
            rot.getValue(axis, radians);
            if ((radians > 0.01f) && (deltatime < 0.300)) {
                this->spinRotation = rot;
                return NavigationStyle::SPINNING;
            }
        }
    }

    return NavigationStyle::IDLE;
}

void NavigationStyle::updateSpin()
{
    SbTime now = SbTime::getTimeOfDay();
    double secs = now.getValue() -  prevRedrawTime.getValue();
    this->prevRedrawTime = now;

    if (this->isAnimating()) {
        SbRotation deltaRotation = this->spinRotation;
        deltaRotation.scaleAngle(secs * 5.0);
        this->reorientCamera(viewer->getCamera(), deltaRotation);
    }
}

/*!
  Decide if it should be possible to start a spin animation of the
  model in the viewer by releasing the mouse button while dragging.

  If the \a enable flag is \c FALSE and we're currently animating, the
  spin will be stopped.
*/
void
NavigationStyle::setAnimationEnabled(const SbBool enable)
{
    this->spinanimatingallowed = enable;
    if (!enable && this->isAnimating()) { this->stopAnimating(); }
}

/*!
  Query whether or not it is possible to start a spinning animation by
  releasing the left mouse button while dragging the mouse.
*/

SbBool
NavigationStyle::isAnimationEnabled(void) const
{
    return this->spinanimatingallowed;
}

/*!
  Query if the model in the viewer is currently in spinning mode after
  a user drag.
*/
SbBool NavigationStyle::isAnimating(void) const
{
    return this->currentmode == NavigationStyle::SPINNING;
}

/*!
 * Starts programmatically the viewer in animation mode. The given axis direction
 * is always in screen coordinates, not in world coordinates.
 */
void NavigationStyle::startAnimating(const SbVec3f& axis, float velocity)
{
    if (!isAnimationEnabled()) return;

    this->spinincrement = SbRotation::identity();
    SbRotation rot;
    rot.setValue(axis, velocity);

    viewer->setViewing(true);
    this->setViewerMode(NavigationStyle::SPINNING);
    this->spinRotation = rot;
}

void NavigationStyle::stopAnimating(void)
{
  if (this->currentmode != NavigationStyle::SPINNING) {
    return;
  }
  this->setViewerMode(viewer->isViewing() ? 
      NavigationStyle::IDLE : NavigationStyle::INTERACT);
}

// This method adds another point to the mouse location log, used for spin
// animation calculations.
void NavigationStyle::addToLog(const SbVec2s pos, const SbTime time)
{
    // In case someone changes the const size setting at the top of this
    // file too small.
    assert (this->log.size > 2 && "mouse log too small!");

    if (this->log.historysize > 0 && pos == this->log.position[0]) {
#if SOQt_DEBUG && 0 // debug
        // This can at least happen under SoQt.
        SoDebugError::postInfo("NavigationStyle::addToLog", "got position already!");
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

// This method "clears" the mouse location log, used for spin
// animation calculations.
void NavigationStyle::clearLog(void)
{
    this->log.historysize = 0;
}

// The viewer is a state machine, and all changes to the current state
// are made through this call.
void NavigationStyle::setViewerMode(const ViewerMode newmode)
{
    const ViewerMode oldmode = this->currentmode;
    if (newmode == oldmode) { return; }

    switch (newmode) {
    case DRAGGING:
        // Set up initial projection point for the projector object when
        // first starting a drag operation.
        this->spinprojector->project(this->lastmouseposition);
        //this->interactiveCountInc();
        this->clearLog();
        break;

    case SPINNING:
#if 0
        this->interactiveCountInc();
#endif
        viewer->scheduleRedraw();
        break;

    case PANNING:
        pan(viewer->getCamera());
#if 0
        this->interactiveCountInc();
#endif
        break;

    case ZOOMING:
#if 0
        this->interactiveCountInc();
#endif
        break;

    default: // include default to avoid compiler warnings.
        break;
    }

    switch (oldmode) {
    case SPINNING:
    case DRAGGING:
    case PANNING:
    case ZOOMING:
#if 0
        this->interactiveCountDec();
#endif
        break;

    default:
        break;
    }

#if 0
    this->setCursorRepresentation(newmode);
#endif
    this->currentmode = newmode;
}

NavigationStyle::ViewerMode NavigationStyle::getViewMode() const
{
    return this->currentmode;
}

SbBool NavigationStyle::processSoEvent(SoQtViewer* viewer, const SoEvent * const ev)
{
    return FALSE;
}

#if 0

SbBool ExaminerStyle::processSoEvent(const SoEvent * const ev, View3DInventorViewer* view)
{
#if 0
    // If we're in picking mode then all events must be redirected to the
    // appropriate mouse model.
    if (pcMouseModel) {
        int hd=pcMouseModel->handleEvent(ev,this->getViewportRegion());
        if (hd==AbstractMouseModel::Continue||hd==AbstractMouseModel::Restart) {
            return TRUE;
        }
        else if (hd==AbstractMouseModel::Finish) {
            pcPolygon = pcMouseModel->getPolygon();
            clipInner = pcMouseModel->isInner();
            delete pcMouseModel; pcMouseModel = 0;
            return inherited::processSoEvent(ev);
        }
        else if (hd==AbstractMouseModel::Cancel) {
            pcPolygon.clear();
            delete pcMouseModel; pcMouseModel = 0;
            return inherited::processSoEvent(ev);
        }
    }
#endif
    // Events when in "ready-to-seek" mode are ignored, except those
    // which influence the seek mode itself -- these are handled further
    // up the inheritance hierarchy.
#if 0
    if (this->isSeekMode()) { return inherited::processSoEvent(ev); }
#endif

    const SoType type(ev->getTypeId());

#if 0
    const SbVec2s size(view->getGLSize());
#else
    const SbVec2s size(100,100);
#endif
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

    // give the nodes in the foreground root the chance to handle events (e.g color bar)
    if (!processed && !this->editing) {
        SoHandleEventAction action(view->getViewportRegion());
        action.setEvent(ev);
#if 0
        action.apply(foregroundroot);
#endif
        processed = action.isHandled();
        if (processed)
            return TRUE;
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
            view->saveHomePosition();
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
            if (!view->isViewing())
                view->setViewing(true);
            break;
        default:
            break;
        }
    }

    // Mouse Button / Spaceball Button handling
    if (type.isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
        const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
        const int button = event->getButton();
        const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

        // SoDebugError::postInfo("processSoEvent", "button = %d", button);
        switch (button) {
        case SoMouseButtonEvent::BUTTON1:
            this->button1down = press;
            if (!press && ev->wasAltDown()) {
#if 0
                SbViewVolume vv = view->getCamera()->getViewVolume(view->getGLAspectRatio());
#else
                SbViewVolume vv = view->getCamera()->getViewVolume();
#endif
                this->panningplane = vv.getPlane(view->getCamera()->focalDistance.getValue());
#if 0
                if (!view->seekToPoint(pos))
                    view->panToCenter(panningplane, posn);
#endif
                processed = TRUE;
            }
            else if (press && (this->currentmode == AbstractStyle::SEEK_WAIT_MODE)) {
                newmode = AbstractStyle::SEEK_MODE;
#if 0
                this->seekToPoint(pos); // implicitly calls interactiveCountInc()
#endif
                processed = TRUE;
            }
            else if (press && (this->currentmode == AbstractStyle::IDLE)) {
                view->setViewing(true);
                processed = TRUE;
            }
            else if (!press && (this->currentmode == AbstractStyle::DRAGGING)) {
                view->setViewing(false);
                processed = TRUE;
            }
            break;
        case SoMouseButtonEvent::BUTTON2:
            // If we are in edit mode then simply ignore the RMB events
            // to pass the event to the base class.
            if (!this->editing) {
                // If we are in zoom or pan mode ignore RMB events otherwise
                // the canvas doesn't get any release events 
                if (this->currentmode != AbstractStyle::ZOOMING && 
                    this->currentmode != AbstractStyle::PANNING) {
                    if (view->isPopupMenuEnabled()) {
                        if (!press) { // release right mouse button
                            view->openPopupMenu(event->getPosition());
                        }
                    }
                }
            }
            break;
        case SoMouseButtonEvent::BUTTON3:
            if (press) {
                this->CenterTime = ev->getTime();
#if 0
                SbViewVolume vv = getCamera()->getViewVolume(getGLAspectRatio());
#else
                SbViewVolume vv = view->getCamera()->getViewVolume();
#endif
                this->panningplane = vv.getPlane(view->getCamera()->focalDistance.getValue());
            }
            else {
                SbTime tmp = (ev->getTime() - this->CenterTime);
                float dci = (float)QApplication::doubleClickInterval()/1000.0f;
                // is it just a middle click?
                if (tmp.getValue() < dci) {
#if 0
                    if (!view->seekToPoint(pos))
                        panToCenter(panningplane, posn);
#endif
                    processed = TRUE;
                }
            }
            this->button3down = press;
            break;
        case SoMouseButtonEvent::BUTTON4:
#if 0
            view->zoom(view->getCamera(), 0.05f);
#endif
            processed = TRUE;
            break;
        case SoMouseButtonEvent::BUTTON5:
#if 0
            zoom(this->getCamera(), -0.05f);
#endif
            processed = TRUE;
            break;
        default:
            break;
        }
    }

    // Mouse Movement handling
    if (type.isDerivedFrom(SoLocation2Event::getClassTypeId())) {
        const SoLocation2Event * const event = (const SoLocation2Event *) ev;
        if (this->currentmode == AbstractStyle::ZOOMING) {
#if 0
            view->zoomByCursor(posn, prevnormalized);
#endif
            processed = TRUE;
        }
        else if (this->currentmode == AbstractStyle::PANNING) {
#if 0
            pan(this->getCamera(), this->getGLAspectRatio(), this->panningplane, posn, prevnormalized);
#endif
            processed = TRUE;
        }
        else if (this->currentmode == AbstractStyle::DRAGGING) {
#if 0
            this->addToLog(event->getPosition(), event->getTime());
            this->spin(posn);
#endif
            processed = TRUE;
        }
    }

    // Spaceball & Joystick handling
    if (type.isDerivedFrom(SoMotion3Event::getClassTypeId())) {
        SoMotion3Event * const event = (SoMotion3Event *) ev;
        SoCamera * const camera = view->getCamera();
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
        if (currentmode == AbstractStyle::SPINNING) { break; }
        newmode = AbstractStyle::IDLE;

        if ((currentmode == AbstractStyle::DRAGGING)/* && (this->log.historysize >= 3)*/) {
#if 0
            SbTime stoptime = (ev->getTime() - this->log.time[0]);
#else
            SbTime stoptime = (ev->getTime());
#endif
            if (allowSpining && stoptime.getValue() < 0.100) {
#if 0
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
                    newmode = AbstractStyle::SPINNING;
                    this->spinRotation = rot;
                }
#endif
            }
        }
        break;
    case BUTTON1DOWN:
        newmode = AbstractStyle::DRAGGING;
        break;
    case BUTTON3DOWN:
    case SHIFTDOWN|BUTTON1DOWN:
        newmode = AbstractStyle::PANNING;
        break;
    case CTRLDOWN:
    case CTRLDOWN|BUTTON1DOWN:
    case CTRLDOWN|SHIFTDOWN:
    case CTRLDOWN|SHIFTDOWN|BUTTON1DOWN:
        newmode = AbstractStyle::SELECTION;
        break;
    case BUTTON1DOWN|BUTTON3DOWN:
    case CTRLDOWN|BUTTON3DOWN:
        newmode = AbstractStyle::ZOOMING;
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
        if ((currentmode != AbstractStyle::SEEK_WAIT_MODE) &&
            (currentmode != AbstractStyle::SEEK_MODE)) {
            newmode = AbstractStyle::IDLE;
        }
        break;
    }

    if (newmode != currentmode) {
        this->setMode(newmode);
    }

    // If not handled in this class, pass on upwards in the inheritance
    // hierarchy.
#if 0
    if ((this->currentmode == AbstractStyle::SELECTION || this->editing) && !processed)
        processed = inherited::processSoEvent(ev);
    else
        return TRUE;
#endif

    // check for left click without selecting something
    if (this->currentmode == AbstractStyle::SELECTION && !processed) {
        if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
            SoMouseButtonEvent * const e = (SoMouseButtonEvent *) ev;
            if (SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1)) {
                Gui::Selection().clearSelection();
            }
        }
    }

    return FALSE;
}

SbBool CADStyle::processSoEvent(const SoEvent * const ev, View3DInventorViewer* view)
{
    //Base::Console().Log("Evnt: %s\n",ev->getTypeId().getName().getString());
    bool processed = false;
#if 0
    if (!isSeekMode() && isViewing())
        setViewing(false); // by default disable viewing mode to render the scene
#endif

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
            view->saveHomePosition();
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
            if (!view->isViewing())
                view->setViewing(true);
            break;
        default:
            break;
        }
    }

    static bool MoveMode=false;
    static bool ZoomMode=false;
    static bool RotMode =false;
    static bool dCliBut3=false;

#if 0
    const SbVec2s size(this->getGLSize());
#else
    const SbVec2s size(100,100);
#endif
    const SbVec2f prevnormalized = lastmouseposition;
    const SbVec2s pos(ev->getPosition());
    const SbVec2f posn((float) pos[0] / (float) SoQtMax((int)(size[0] - 1), 1),
                     (float) pos[1] / (float) SoQtMax((int)(size[1] - 1), 1));

    lastmouseposition = posn;

    // switching the mouse modes
    if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
        const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
        const int button = event->getButton();
        const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

        // SoDebugError::postInfo("processSoEvent", "button = %d", button);
        switch (button) {
        case SoMouseButtonEvent::BUTTON1:
            if (press) {
                _bRejectSelection = false;
                if (MoveMode) {
                    RotMode = true;
                    ZoomMode = false;
                    MoveTime = ev->getTime();

#if 0
                    // Set up initial projection point for the projector object when
                    // first starting a drag operation.
                    spinprojector->project(lastmouseposition);
                    //interactiveCountInc();
                    clearLog();
#endif

                    view->setComponentCursor(SoQtCursor::getRotateCursor());
                    //getWidget()->setCursor( QCursor( Qt::PointingHandCursor ) );
                    processed = true;
                }
            }
            else {
                // if you come out of rotation dont deselect anything
                if (_bRejectSelection || MoveMode) {
                    _bRejectSelection=false;
                    processed = true;
                }
                if (MoveMode) {
                    RotMode = false; 

                    SbTime tmp = (ev->getTime() - MoveTime);
                    float dci = (float)QApplication::doubleClickInterval()/1000.0f;
                    if (tmp.getValue() < dci/*0.300*/) {
                        ZoomMode = true;
                        view->setComponentCursor(SoQtCursor::getZoomCursor());
                        //getWidget()->setCursor( QCursor( Qt::SizeVerCursor ) );
                    }
                    else {
                        ZoomMode = false;
                        view->setComponentCursor(SoQtCursor::getPanCursor());
                        //getWidget()->setCursor( QCursor( Qt::SizeAllCursor ) );

#if 0
                        SbViewVolume vv = getCamera()->getViewVolume(getGLAspectRatio());
#else
                        SbViewVolume vv = view->getCamera()->getViewVolume();
#endif
                        panningplane = vv.getPlane(view->getCamera()->focalDistance.getValue());
       
                        // check on start spining
#if 0
                        SbTime stoptime = (ev->getTime() - log.time[0]);
#else
                        SbTime stoptime = (ev->getTime());
#endif
                        if (allowSpining && stoptime.getValue() < 0.100) {
#if 0
                            const SbVec2s glsize(this->getGLSize());
#else
                            const SbVec2s glsize(100,100);
#endif
#if 0
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
#endif
                        }
                    }
                    processed = true;
                }
            }
            break;
        case SoMouseButtonEvent::BUTTON2:
            break;
        case SoMouseButtonEvent::BUTTON3:
            if (press) {
                CenterTime = ev->getTime();
                MoveMode = true;
                _bSpining = false;
                dCliBut3 = false;
#if 0
                SbViewVolume vv = getCamera()->getViewVolume(getGLAspectRatio());
#else
                SbViewVolume vv = view->getCamera()->getViewVolume();
#endif
                panningplane = vv.getPlane(view->getCamera()->focalDistance.getValue());
                // save the current cursor before overriding
                _oldCursor = view->getWidget()->cursor();
                view->setComponentCursor(SoQtCursor::getPanCursor());
                //getWidget()->setCursor( QCursor( Qt::SizeAllCursor ) );
            }
            else {
                SbTime tmp = (ev->getTime() - CenterTime);
                float dci = (float)QApplication::doubleClickInterval()/1000.0f;
                // is it just a middle click?
                if (tmp.getValue() < dci/*0.300*/) {
#if 0
                    if (!seekToPoint(pos))
                        panToCenter(panningplane, posn);
#endif
                }

                MoveMode = false;
                RotMode = false;
                ZoomMode = false;
                // restore the previous cursor
                view->getWidget()->setCursor( _oldCursor /*QCursor( Qt::ArrowCursor )*/);
                _bRejectSelection = true;
            }
            processed = true;
            break;
        case SoMouseButtonEvent::BUTTON4:
#if 0
            if (press) 
                View3DInventorViewer::zoom(getCamera(), 0.05f);
#endif
            processed = true;
            break;
        case SoMouseButtonEvent::BUTTON5:
#if 0
            if (press) 
                View3DInventorViewer::zoom(getCamera(), -0.05f);
#endif
            processed = true;
            break;
        default:
            break;
        }
    }

    // Mouse Movement handling
    if (ev->getTypeId().isDerivedFrom(SoLocation2Event::getClassTypeId())) {
        if (MoveMode && ZoomMode) {
#if 0
            zoom(getCamera(),(posn[1] - prevnormalized[1]) * 10.0f);
#endif
            processed = true;
        }
        else if (MoveMode && RotMode) {
#if 0
            addToLog(ev->getPosition(), ev->getTime());
            spin(posn);
#endif
            processed = true;
        }
        else if (MoveMode) {
#if 0
            pan(getCamera(),getGLAspectRatio(),panningplane, posn, prevnormalized);
#endif
            processed = true;
        }
        else if (_bSpining) {
            processed = true;
        }
    }

    // Spaceball & Joystick handling
    if (ev->getTypeId().isDerivedFrom(SoMotion3Event::getClassTypeId())) {
        SoMotion3Event * const event = (SoMotion3Event *) ev;
        SoCamera * const camera = view->getCamera();
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
#if 0
    if (!processed && !MoveMode && !RotMode) {
        if (pcMouseModel) {
            int hd=pcMouseModel->handleEvent(ev,this->getViewportRegion());
            if (hd==AbstractMouseModel::Continue||hd==AbstractMouseModel::Restart) {
                processed = true;
            }
            else if (hd==AbstractMouseModel::Finish) {
                pcPolygon = pcMouseModel->getPolygon();
                clipInner = pcMouseModel->isInner();
                delete pcMouseModel; pcMouseModel = 0;
            }
            else if (hd==AbstractMouseModel::Cancel) {
                pcPolygon.clear();
                delete pcMouseModel; pcMouseModel = 0;
            }
        }
    }
#endif

    // give the nodes in the foreground root the chance to handle events (e.g color bar)
    // Note: this must be done _before_ ceding to the viewer  
    if (!processed) {
        SoHandleEventAction action(view->getViewportRegion());
        action.setEvent(ev);
#if 0
        action.apply(foregroundroot);
#endif
        processed = action.isHandled();
    }

    // invokes the appropriate callback function when user interaction has started or finished
    bool bInteraction = (MoveMode||ZoomMode||RotMode|_bSpining);
#if 0
    if (bInteraction && getInteractiveCount()==0)
        interactiveCountInc();
    // must not be in seek mode because it gets decremented in setSeekMode(FALSE)
    else if (!bInteraction&&!dCliBut3&&getInteractiveCount()>0&&!isSeekMode())
        interactiveCountDec();
#endif

#if 0
    if (!processed)
        processed = inherited::processSoEvent(ev);
    else 
        return true;
#endif

    // right mouse button pressed
    if (!processed && !MoveMode && !RotMode) {
        if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
            SoMouseButtonEvent * const e = (SoMouseButtonEvent *) ev;
            if ((e->getButton() == SoMouseButtonEvent::BUTTON2) && e->getState() == SoButtonEvent::UP) {
                if (view->isPopupMenuEnabled()) {
                    if (e->getState() == SoButtonEvent::UP) {
                        view->openPopupMenu(e->getPosition());
                    }

                    // Steal all RMB-events if the viewer uses the popup-menu.
                    return true;
                }
            }
        }
    }

    // check for left click without selecting something
    if (!processed) {
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
#endif
