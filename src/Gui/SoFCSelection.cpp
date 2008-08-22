/***************************************************************************
 *   Copyright (c) 2005 J�rgen Riegel <juergen.riegel@web.de>              *
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
# include <qstatusbar.h>
# include <qstring.h>
#endif

//#include <Inventor/nodes/SoSubNodeP.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/SoFullPath.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/SoPickedPoint.h>

#include <Base/Console.h>
#include "SoFCSelection.h"
#include "MainWindow.h"
#include "Selection.h"
#include "SoFCSelectionAction.h"


using namespace Gui;

SoFullPath * Gui::SoFCSelection::currenthighlight = NULL;


// *************************************************************************

SO_NODE_SOURCE(SoFCSelection);

/*!
  Constructor.
*/
SoFCSelection::SoFCSelection()
{

  SO_NODE_CONSTRUCTOR(SoFCSelection);

  SO_NODE_ADD_FIELD(colorHighlight, (SbColor(0.1f, 0.1f, 0.8f)));
  SO_NODE_ADD_FIELD(colorSelection, (SbColor(0.1f, 0.5f, 0.1f)));
  SO_NODE_ADD_FIELD(style,          (EMISSIVE));
  SO_NODE_ADD_FIELD(highlightMode,  (AUTO));
  SO_NODE_ADD_FIELD(selectionMode,  (SEL_ON));
  SO_NODE_ADD_FIELD(selected,       (NOTSELECTED));
  SO_NODE_ADD_FIELD(documentName,   (""));
  SO_NODE_ADD_FIELD(objectName,    (""));
  SO_NODE_ADD_FIELD(subElementName, (""));

  SO_NODE_DEFINE_ENUM_VALUE(Styles, EMISSIVE);
  SO_NODE_DEFINE_ENUM_VALUE(Styles, EMISSIVE_DIFFUSE);
  SO_NODE_DEFINE_ENUM_VALUE(Styles, BOX);
  SO_NODE_SET_SF_ENUM_TYPE(style, Styles);

  SO_NODE_DEFINE_ENUM_VALUE(HighlightModes, AUTO);
  SO_NODE_DEFINE_ENUM_VALUE(HighlightModes, ON);
  SO_NODE_DEFINE_ENUM_VALUE(HighlightModes, OFF);
  SO_NODE_SET_SF_ENUM_TYPE (highlightMode, HighlightModes);

  SO_NODE_DEFINE_ENUM_VALUE(SelectionModes, SEL_ON);
  SO_NODE_DEFINE_ENUM_VALUE(SelectionModes, SEL_OFF);
  SO_NODE_SET_SF_ENUM_TYPE (selectionMode, SelectionModes);

  SO_NODE_DEFINE_ENUM_VALUE(Selected, NOTSELECTED);
  SO_NODE_DEFINE_ENUM_VALUE(Selected, SELECTED);
  SO_NODE_SET_SF_ENUM_TYPE(selected, Selected);

  highlighted = FALSE;
  bShift      = false;
  bCtrl       = false;
}

/*!
  Destructor.
*/
SoFCSelection::~SoFCSelection()
{
    // If we're being deleted and we're the current highlight,
    // NULL out that variable
    if (currenthighlight != NULL &&
	(!currenthighlight->getTail()->isOfType(SoFCSelection::getClassTypeId()))) {
	currenthighlight->unref();
	currenthighlight = NULL;
    }
  //delete THIS;
}

// doc from parent
void
SoFCSelection::initClass(void)
{
  SO_NODE_INIT_CLASS(SoFCSelection,SoSeparator,"Separator");
}

void SoFCSelection::finish()
{
  atexit_cleanup();
}

/*!
  Static method that can be used to turn off the current highlight.
*/
void
SoFCSelection::turnOffCurrentHighlight(SoGLRenderAction * action)
{
  SoFCSelection::turnoffcurrent(action);
}

void SoFCSelection::doAction(SoAction *action)
{
    if (action->getTypeId() == SoFCDocumentAction::getClassTypeId()) {
        SoFCDocumentAction *docaction = (SoFCDocumentAction*)action;
        this->documentName = docaction->documentName;
    }

    if (action->getTypeId() == SoFCDocumentObjectAction::getClassTypeId()) {
        SoFCDocumentObjectAction* objaction = static_cast<SoFCDocumentObjectAction*>(action);
        objaction->documentName  = this->documentName.getValue();
        objaction->objectName    = this->objectName.getValue();
        objaction->componentName = this->subElementName.getValue();
        objaction->setHandled();
    }

    if (action->getTypeId() == SoFCEnableHighlightAction::getClassTypeId()) {
        SoFCEnableHighlightAction *preaction = (SoFCEnableHighlightAction*)action;
        if (preaction->highlight) {
            this->highlightMode = SoFCSelection::AUTO;
        }
        else {
            this->highlightMode = SoFCSelection::OFF;
        }
    }

    if (action->getTypeId() == SoFCEnableSelectionAction::getClassTypeId()) {
        SoFCEnableSelectionAction *selaction = (SoFCEnableSelectionAction*)action;
        if (selaction->selection) {
            this->selectionMode = SoFCSelection::SEL_ON;
            this->style = SoFCSelection::EMISSIVE;
        }
        else {
            this->selectionMode = SoFCSelection::SEL_OFF;
            this->style = SoFCSelection::BOX;
            this->selected = NOTSELECTED;
        }
    }

    if (action->getTypeId() == SoFCSelectionColorAction::getClassTypeId()) {
        SoFCSelectionColorAction *colaction = (SoFCSelectionColorAction*)action;
        this->colorSelection = colaction->selectionColor;
    }

    if (action->getTypeId() == SoFCHighlightColorAction::getClassTypeId()) {
        SoFCHighlightColorAction *colaction = (SoFCHighlightColorAction*)action;
        this->colorHighlight = colaction->highlightColor;
    }

    if (selectionMode.getValue() == SEL_ON && action->getTypeId() == SoFCSelectionAction::getClassTypeId()) {
        SoFCSelectionAction *selaction = static_cast<SoFCSelectionAction*>(action);

        if (selaction->SelChange.Type == SelectionChanges::AddSelection || 
            selaction->SelChange.Type == SelectionChanges::RmvSelection) {
            if (documentName.getValue() == selaction->SelChange.pDocName &&
                objectName.getValue() == selaction->SelChange.pObjectName &&
                (subElementName.getValue() == selaction->SelChange.pSubName || 
                *(selaction->SelChange.pSubName) == '\0') ) {
                if (selaction->SelChange.Type == SelectionChanges::AddSelection) {
                    selected = SELECTED;
                }
                else {
                    selected = NOTSELECTED;
                }
            }
        }
        else if (selaction->SelChange.Type == SelectionChanges::ClrSelection) {
            if (documentName.getValue() == selaction->SelChange.pDocName ||
                strcmp(selaction->SelChange.pDocName,"") == 0)
                selected = NOTSELECTED;
        }
    }

    inherited::doAction( action );
}


// doc from parent
void
SoFCSelection::handleEvent(SoHandleEventAction * action)
{
#if 0
    static char buf[513];
    HighlightModes mymode = (HighlightModes) this->highlightMode.getValue();
    const SoEvent * event = action->getEvent();
  
    // mouse move events for preselection
    if (event->isOfType(SoLocation2Event::getClassTypeId())) {
#if 1
        // NOTE: If preselection is off then we do not check for a picked point because otherwise this search may slow
        // down extremely the system on really big data sets. In this case we just check for a picked point if the data
        // set has been selected.
        if (mymode == AUTO || mymode == ON) {
            const SoPickedPoint * pp = action->getPickedPoint();
            if (pp && pp->getPath()->containsPath(action->getCurPath())) {
                if (!highlighted) {
                    if (Gui::Selection().setPreselect(documentName.getValue().getString()
                                           ,objectName.getValue().getString()
                                           ,subElementName.getValue().getString()
                                           ,pp->getPoint()[0]
                                           ,pp->getPoint()[1]
                                           ,pp->getPoint()[2])){
                        SoFCSelection::turnoffcurrent(action);
                        SoFCSelection::currenthighlight = (SoFullPath*)action->getCurPath()->copy();
                        SoFCSelection::currenthighlight->ref();
                        highlighted = TRUE;
                        this->touch(); // force scene redraw
                        this->redrawHighlighted(action, TRUE);
                    }
                }
                
                snprintf(buf,512,"Preselected: %s.%s.%s (%f,%f,%f)",documentName.getValue().getString()
                                           ,objectName.getValue().getString()
                                           ,subElementName.getValue().getString()
                                           ,pp->getPoint()[0]
                                           ,pp->getPoint()[1]
                                           ,pp->getPoint()[2]);

                getMainWindow()->statusBar()->showMessage(QString::fromAscii(buf),3000);
            } else { // picked point
                if (highlighted) {
                    if (mymode == AUTO)
                        SoFCSelection::turnoffcurrent(action);
                    //FIXME: I think we should set 'highlighted' to false whenever no point is picked
                    //else
                    highlighted = FALSE;
                    Gui::Selection().rmvPreselect();
                }
            }
        }
#else
        const SoPickedPoint * pp = action->getPickedPoint();
        if (pp && pp->getPath()->containsPath(action->getCurPath())) {
            if (!highlighted) {
                if (mymode == AUTO || mymode == ON) {
                    if (Gui::Selection().setPreselect(documentName.getValue().getString()
                                           ,objectName.getValue().getString()
                                           ,subElementName.getValue().getString()
                                           ,pp->getPoint()[0]
                                           ,pp->getPoint()[1]
                                           ,pp->getPoint()[2])){
                        SoFCSelection::turnoffcurrent(action);
                        SoFCSelection::currenthighlight = (SoFullPath*)action->getCurPath()->copy();
                        SoFCSelection::currenthighlight->ref();
                        highlighted = TRUE;
                        this->touch(); // force scene redraw
                        this->redrawHighlighted(action, TRUE);
                    }
                } else { // mymode
                    // preselection in the viewer is disabled but we must inform all observers anyway
                    if (Gui::Selection().setPreselect(documentName.getValue().getString()
                                           ,objectName.getValue().getString()
                                           ,subElementName.getValue().getString()
                                           ,pp->getPoint()[0]
                                           ,pp->getPoint()[1]
                                           ,pp->getPoint()[2]))
                        highlighted = TRUE;
                }
            } // highlight
            snprintf(buf,512,"Preselected: %s.%s.%s (%f,%f,%f)",documentName.getValue().getString()
                                           ,objectName.getValue().getString()
                                           ,subElementName.getValue().getString()
                                           ,pp->getPoint()[0]
                                           ,pp->getPoint()[1]
                                           ,pp->getPoint()[2]);

            getMainWindow()->statusBar()->message(buf,3000);
        } else { // picked point
            if (highlighted) {
                if (mymode == AUTO)
                    SoFCSelection::turnoffcurrent(action);
                //FIXME: I think we should set 'highlighted' to false whenever no point is picked
                //else
                highlighted = FALSE;
                Gui::Selection().rmvPreselect();
            }
        }
#endif
    // key press events
    } else if (event->isOfType(SoKeyboardEvent ::getClassTypeId())) {
        SoKeyboardEvent  * const e = (SoKeyboardEvent  *) event;
        if (SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::LEFT_SHIFT)     ||
            SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::RIGHT_SHIFT)     )
            bShift = true;
        if (SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::LEFT_SHIFT)   ||
            SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::RIGHT_SHIFT)   )
            bShift = false;
        if (SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::LEFT_CONTROL)   ||
            SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::RIGHT_CONTROL)   )
            bCtrl = true;
        if (SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::LEFT_CONTROL) ||
            SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::RIGHT_CONTROL) )
            bCtrl = false;

    // mouse press events for (de)selection
    } else if (event->isOfType(SoMouseButtonEvent::getClassTypeId())) {
        SoMouseButtonEvent * const e = (SoMouseButtonEvent *) event;
        if (SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1)) {
            //FIXME: Shouldn't we remove the preselection for newly selected objects?
            //       Otherwise the tree signals that an object is preselected even though it is hidden. (Werner)
            const SoPickedPoint * pp = action->getPickedPoint();
            if (pp && pp->getPath()->containsPath(action->getCurPath())) {
                if (bCtrl) {
                    if (Gui::Selection().isSelected(documentName.getValue().getString()
                                         ,objectName.getValue().getString()
                                         ,subElementName.getValue().getString())) {
                        Gui::Selection().rmvSelection(documentName.getValue().getString()
                                          ,objectName.getValue().getString()
                                          ,subElementName.getValue().getString());
                    } else {
                        Gui::Selection().addSelection(documentName.getValue().getString()
                                          ,objectName.getValue().getString()
                                          ,subElementName.getValue().getString()
                                          ,pp->getPoint()[0]
                                          ,pp->getPoint()[1]
                                          ,pp->getPoint()[2]);

                        if (mymode == OFF) {
                            snprintf(buf,512,"Selected: %s.%s.%s (%f,%f,%f)",documentName.getValue().getString()
                                                       ,objectName.getValue().getString()
                                                       ,subElementName.getValue().getString()
                                                       ,pp->getPoint()[0]
                                                       ,pp->getPoint()[1]
                                                       ,pp->getPoint()[2]);

                            getMainWindow()->statusBar()->showMessage(QString::fromAscii(buf),3000);
                        }
                    }
                } else { // Ctrl
                    Gui::Selection().clearSelection(documentName.getValue().getString());
                    Gui::Selection().addSelection(documentName.getValue().getString()
                                          ,objectName.getValue().getString()
                                          ,subElementName.getValue().getString()
                                          ,pp->getPoint()[0]
                                          ,pp->getPoint()[1]
                                          ,pp->getPoint()[2]);
 
                    if (mymode == OFF) {
                        snprintf(buf,512,"Selected: %s.%s.%s (%f,%f,%f)",documentName.getValue().getString()
                                                   ,objectName.getValue().getString()
                                                   ,subElementName.getValue().getString()
                                                   ,pp->getPoint()[0]
                                                   ,pp->getPoint()[1]
                                                   ,pp->getPoint()[2]);

                        getMainWindow()->statusBar()->showMessage(QString::fromAscii(buf),3000);
                    }
                }

                action->setHandled(); 
            } // picked point
        } // mouse release
    }

    inherited::handleEvent(action);
#else
    // If we don't need to pick for locate highlighting,
    // then just behave as separator and return.
    // NOTE: we still have to pick for ON even though we don't have
    // to re-render, because the app needs to be notified as the mouse
    // goes over locate highlight nodes.
    if ( highlightMode.getValue() == OFF ) {
	SoSeparator::handleEvent( action );
	return;
    }

    // get event from the action
    const SoEvent *event = action->getEvent();

    //
    // If this is a mouseMotion event, then check for locate highlighting
    //
    if (event->isOfType(SoLocation2Event::getClassTypeId())) {

	// check to see if the mouse is over our geometry...
	SbBool underTheMouse = FALSE;
	const SoPickedPoint *pp = action->getPickedPoint();
	SoFullPath *pPath = (pp != NULL) ? (SoFullPath *) pp->getPath() : NULL;
	if (pPath && pPath->containsPath(action->getCurPath())) {
	    // Make sure I'm the lowest LocHL in the pick path!
	    underTheMouse = TRUE;
	    for (int i = 0; i < pPath->getLength(); i++) {
		SoNode *node = pPath->getNodeFromTail(i);
		if (node->isOfType(SoFCSelection::getClassTypeId())) {
		    if (node != this)
			underTheMouse = FALSE;
		    break; // found the lowest LocHL - look no further
		}
	    }
	}

	// Am I currently highlighted?
	if (isHighlighted(action)) {
	    if ( ! underTheMouse)
		// re-draw the object with it's normal color
		redrawHighlighted(action, FALSE);
	    else
		action->setHandled();
	}
	// Else I am not currently highlighted
	else {
	    // If under the mouse, then highlight!
	    if (underTheMouse)
		// draw this object highlighted
		redrawHighlighted(action, TRUE);
	}
    }

    // Let the base class traverse the children.
    if ( action->getGrabber() != this )
	SoSeparator::handleEvent(action);
#endif
}

// doc from parent
void
SoFCSelection::GLRenderBelowPath(SoGLRenderAction * action)
{
#if 0
  // check if preselection is active
  HighlightModes mymode = (HighlightModes) this->highlightMode.getValue();
  bool preselected = highlighted && mymode == AUTO;
  SoState * state = action->getState();
  state->push();
  if (preselected || this->highlightMode.getValue() == ON || this->selected.getValue() == SELECTED) {
    this->setOverride(action);
  }
  inherited::GLRenderBelowPath(action);
  state->pop();
#else
    // Set up state for locate highlighting (if necessary)
    GLint oldDepthFunc;
    SbBool drawHighlighted = preRender(action, oldDepthFunc);

    // now invoke the parent method
    SoSeparator::GLRenderBelowPath(action);

    // Restore old depth buffer model if needed
    if (drawHighlighted || highlighted)
	glDepthFunc((GLenum)oldDepthFunc);

    // Clean up state if needed
    if (drawHighlighted)
	action->getState()->pop();
#endif
}

// doc from parent
void
SoFCSelection::GLRenderInPath(SoGLRenderAction * action)
{
#if 0
  // check if preselection is active
  HighlightModes mymode = (HighlightModes) this->highlightMode.getValue();
  bool preselected = highlighted && mymode == AUTO;
  SoState * state = action->getState();
  state->push();
  if (preselected || this->highlightMode.getValue() == ON || this->selected.getValue() == SELECTED) {
    this->setOverride(action);
  }
  inherited::GLRenderInPath(action);
  state->pop();
#else
    // Set up state for locate highlighting (if necessary)
    GLint oldDepthFunc;
    SbBool drawHighlighted = preRender(action, oldDepthFunc);

    // now invoke the parent method
    SoSeparator::GLRenderInPath(action);

    // Restore old depth buffer model if needed
    if (drawHighlighted || highlighted)
	glDepthFunc((GLenum)oldDepthFunc);

    // Clean up state if needed
    if (drawHighlighted)
	action->getState()->pop();
#endif
}
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoWindowElement.h>

SbBool
SoFCSelection::preRender(SoGLRenderAction *action, GLint &oldDepthFunc)
//
////////////////////////////////////////////////////////////////////////
{
    // If not performing locate highlighting, just return.
    if (highlightMode.getValue() == OFF)
	return FALSE;

    SoState *state = action->getState();

    // ??? prevent caching at this level - for some reason the
    // ??? SoWindowElement::copyMatchInfo() method get called, which should
    // ??? never be called. We are not caching this node correctly yet....
    SoCacheElement::invalidate(state);

    SbBool drawHighlighted = (highlightMode.getValue() == ON || isHighlighted(action));

    if (drawHighlighted) {

	// prevent diffuse & emissive color from leaking out...
	state->push();

	SbColor col = colorHighlight.getValue();

	// Emissive Color
	SoOverrideElement::setEmissiveColorOverride(state, this, TRUE);
	SoLazyElement::setEmissive(state, &col);

	// Diffuse Color
	if (style.getValue() == EMISSIVE_DIFFUSE) {
	    SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
	    SoLazyElement::setDiffuse(state, this, 1, &col, &colorpacker);
	}
    }

    // Draw on top of other things at same z-buffer depth if:
    // [a] we're highlighted
    // [b] this is the highlighting pass. This occurs when changing from
    //     non-hilit to lit OR VICE VERSA.
    // Otherwise, leave it alone...
    if (drawHighlighted || highlighted) {
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);
	if (oldDepthFunc != GL_LEQUAL)
	    glDepthFunc(GL_LEQUAL);
    }

    return drawHighlighted;
}

/*!
  Empty method in Coin. Can be used by subclasses to be told
  when status change.
*/
#include "MainWindow.h"
#include "View3DInventor.h"
#include "View3DInventorViewer.h"
void
SoFCSelection::redrawHighlighted(SoAction *  action , SbBool  doHighlight )
{
    // If we are about to highlight, and there is something else highlighted,
    // that something else needs to unhighlight.
    if (doHighlight && currenthighlight != NULL &&
        !(*((SoFullPath *)action->getCurPath()) == *currenthighlight)) {

	SoNode *tail = currenthighlight->getTail();
	if (tail->isOfType( SoFCSelection::getClassTypeId()))
	    ((SoFCSelection *)tail)->redrawHighlighted(action, FALSE);
	else {
	    // Just get rid of the path. It's no longer valid for redraw.
	    currenthighlight->unref();
	    currenthighlight = NULL;
	}
    }

    SoPath *pathToRender;
    // save the path to ourself for later de-highlight
    if (doHighlight) {

	if (currenthighlight != NULL)
	    currenthighlight->unref();
	currenthighlight = (SoFullPath *) action->getCurPath()->copy();
	currenthighlight->ref();

	// We will be rendering this new path to highlight it
	pathToRender = currenthighlight;
	pathToRender->ref();
    }
    // delete our path if we are no longer highlighted
    else {

	// We will be rendering this old path to unhighlight it
	pathToRender = currenthighlight;
	pathToRender->ref();

	currenthighlight->unref();
	currenthighlight = NULL;
    }

    // If highlighting is forced on for this node, we don't need this special render.
    if (highlightMode.getValue() != AUTO) {
	pathToRender->unref();
	return;
    }

    SoState *state = action->getState();

    void* window;
    void* context;
    void *display;
    SoGLRenderAction *glAction;
    SoWindowElement::get(state, window, context, display, glAction);

    // If we don't have a current window, then simply return...
    //if (window == 0 || context == NULL || display == NULL || glAction == NULL)
	//return;

    glAction = static_cast<View3DInventor*>(getMainWindow()->activeWindow())
        ->getViewer()->getGLRenderAction();

#ifndef WIN32  // FIXME !!!!
    // set the current window
    glXMakeCurrent(display, window, context);
#endif
    // render into the front buffer (save the current buffering type)
    GLint whichBuffer;
    glGetIntegerv(GL_DRAW_BUFFER, &whichBuffer);
    if (whichBuffer != GL_FRONT)
	glDrawBuffer(GL_FRONT);

    highlighted = TRUE;
    glAction->apply(pathToRender);
    highlighted = FALSE;

    // restore the buffering type
    if (whichBuffer != GL_FRONT)
	glDrawBuffer((GLenum)whichBuffer);
    glFlush();

    pathToRender->unref();
}

SbBool 
SoFCSelection::readInstance  (  SoInput *  in, unsigned short  flags )
{
  // Note: The read in document name can be false, so the caller must ensure pointing to the correct documemt
  SbBool ret = inherited::readInstance(in, flags);
  return ret;
}
//
// update override state before rendering
//
void
SoFCSelection::setOverride(SoGLRenderAction * action)
{
  SoState * state = action->getState();
  if(this->selected.getValue() == SELECTED)
    SoLazyElement::setEmissive(state, &this->colorSelection.getValue());
  else
    SoLazyElement::setEmissive(state, &this->colorHighlight.getValue());
  SoOverrideElement::setEmissiveColorOverride(state, this, TRUE);

  Styles mystyle = (Styles) this->style.getValue();
  if (mystyle == SoFCSelection::EMISSIVE_DIFFUSE) {
    if(this->selected.getValue() == SELECTED)
      SoLazyElement::setDiffuse(state, this,1, &this->colorSelection.getValue(),&colorpacker);
    else
      SoLazyElement::setDiffuse(state, this,1, &this->colorHighlight.getValue(),&colorpacker);
    SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
  }
}

// private convenience method
void
SoFCSelection::turnoffcurrent(SoAction * action)
{
#if 0
  if (SoFCSelection::currenthighlight &&
      SoFCSelection::currenthighlight->getLength()) {
    SoNode * tail = SoFCSelection::currenthighlight->getTail();
    if (tail->isOfType(SoFCSelection::getClassTypeId())) {
      ((SoFCSelection*)tail)->highlighted = FALSE;
      ((SoFCSelection*)tail)->touch(); // force scene redraw
      if (action) ((SoFCSelection*)tail)->redrawHighlighted(action, FALSE);
    }
  }
  if (SoFCSelection::currenthighlight) {
    SoFCSelection::currenthighlight->unref();
    SoFCSelection::currenthighlight = NULL;
  }
#endif
      if (currenthighlight == NULL)
	return;

    SoNode *tail = currenthighlight->getTail();
    if (tail->isOfType(SoFCSelection::getClassTypeId())) {

	// don't redraw if we already are in the middle of rendering
	// (processing events during render abort might cause this)
	SoState *state = action->getState();
	if (state && state->getDepth() == 1)
	    ((SoFCSelection *)tail)->redrawHighlighted(action, FALSE);
    }
    else {
	// Just get rid of the path. It's no longer valid for redraw.
	currenthighlight->unref();
	currenthighlight = NULL;
    }

}
SbBool
SoFCSelection::isHighlighted(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *actionPath = (SoFullPath *) action->getCurPath();
    return (currenthighlight != NULL &&
	    currenthighlight->getTail() == actionPath->getTail() && // nested SoHL!
	    *currenthighlight == *actionPath);
}

//#undef THIS
