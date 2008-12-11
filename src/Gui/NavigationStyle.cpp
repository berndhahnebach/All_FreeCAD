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
#if 0
#ifndef _PreComp_
# include "InventorAll.h"
#endif

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

// The viewer is a state machine, and all changes to the current state
// are made through this call.
void AbstractStyle::setMode(const ViewerMode newmode)
{
    const ViewerMode oldmode = this->currentmode;
    if (newmode == oldmode) { return; }

    switch (newmode) {
    case DRAGGING:
        // Set up initial projection point for the projector object when
        // first starting a drag operation.
#if 0
        this->spinprojector->project(this->lastmouseposition);
        this->interactiveCountInc();
        this->clearLog();
#endif
        break;

    case SPINNING:
#if 0
        this->interactiveCountInc();
        this->scheduleRedraw();
#endif
        break;

    case PANNING:
        {
            // The plane we're projecting the mouse coordinates to get 3D
            // coordinates should stay the same during the whole pan
            // operation, so we should calculate this value here.
#if 0
            SoCamera * cam = this->getCamera();
            if (cam == NULL) { // can happen for empty scenegraph
                this->panningplane = SbPlane(SbVec3f(0, 0, 1), 0);
            }
            else {
                SbViewVolume vv = cam->getViewVolume(this->getGLAspectRatio());
                this->panningplane = vv.getPlane(cam->focalDistance.getValue());
            }
#endif
        }
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

ExaminerStyle::ExaminerStyle()
{
}

ExaminerStyle::~ExaminerStyle()
{
}

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

CADStyle::CADStyle()
{
}

CADStyle::~CADStyle()
{
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
