/***************************************************************************
 *   Copyright (c) 2005 Jürgen Riegel <juergen.riegel@web.de>              *
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

#define new DEBUG_CLIENTBLOCK
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
  //delete THIS;
}

// doc from parent
void
SoFCSelection::initClass(void)
{
  SO_NODE_INIT_CLASS(SoFCSelection,SoSeparator,"Separator");
}

/*!
  Static method that can be used to turn off the current highlight.
*/
void
SoFCSelection::turnOffCurrentHighlight(SoGLRenderAction * action)
{
  SoFCSelection::turnoffcurrent(action);
}

void SoFCSelection::doAction( SoAction *action)
{
  if ( action->getTypeId() == SoFCDocumentAction::getClassTypeId() ) {
    SoFCDocumentAction *docaction = (SoFCDocumentAction*)action;
    this->documentName = docaction->documentName;
  }

  if ( action->getTypeId() == SoFCEnableHighlightAction::getClassTypeId() ) {
    SoFCEnableHighlightAction *preaction = (SoFCEnableHighlightAction*)action;
    if ( preaction->highlight ) {
      this->highlightMode = SoFCSelection::AUTO;
    } else {
      this->highlightMode = SoFCSelection::OFF;
    }
  }

  if ( action->getTypeId() == SoFCEnableSelectionAction::getClassTypeId() ) {
    SoFCEnableSelectionAction *selaction = (SoFCEnableSelectionAction*)action;
    if ( selaction->selection ) {
      this->selectionMode = SoFCSelection::SEL_ON;
      this->style = SoFCSelection::EMISSIVE;
    } else {
      this->selectionMode = SoFCSelection::SEL_OFF;
      this->style = SoFCSelection::BOX;
      this->selected = NOTSELECTED;
    }
  }

  if ( action->getTypeId() == SoFCSelectionColorAction::getClassTypeId() ) {
    SoFCSelectionColorAction *colaction = (SoFCSelectionColorAction*)action;
    this->colorSelection = colaction->selectionColor;
  }

  if ( action->getTypeId() == SoFCHighlightColorAction::getClassTypeId() ) {
    SoFCHighlightColorAction *colaction = (SoFCHighlightColorAction*)action;
    this->colorHighlight = colaction->highlightColor;
  }

  if ( selectionMode.getValue() == SEL_ON && action->getTypeId() == SoFCSelectionAction::getClassTypeId() ) {
    SoFCSelectionAction *selaction = reinterpret_cast<SoFCSelectionAction*>(action);

    if ( selaction->SelChange.Type == SelectionChanges::AddSelection || selaction->SelChange.Type == SelectionChanges::RmvSelection ) {
      if ( documentName.getValue() == selaction->SelChange.pDocName &&
           objectName.getValue() == selaction->SelChange.pObjectName &&
           (subElementName.getValue() == selaction->SelChange.pSubName || *(selaction->SelChange.pSubName) == '\0') ) {
        if ( selaction->SelChange.Type == SelectionChanges::AddSelection )
        {
          selected = SELECTED;
        }else{
          selected = NOTSELECTED;
        }
      }
    }else if ( selaction->SelChange.Type == SelectionChanges::ClearSelection ){
      if (documentName.getValue() == selaction->SelChange.pDocName || strcmp(selaction->SelChange.pDocName,"") == 0)
        selected = NOTSELECTED;
    }
  }

  inherited::doAction( action );
}


// doc from parent
void
SoFCSelection::handleEvent(SoHandleEventAction * action)
{
  static char buf[512];
  HighlightModes mymode = (HighlightModes) this->highlightMode.getValue();

  const SoEvent * event = action->getEvent();
  if (event->isOfType(SoLocation2Event::getClassTypeId())) {
    const SoPickedPoint * pp = action->getPickedPoint();
    if (pp && pp->getPath()->containsPath(action->getCurPath())) {
      if (!highlighted) {
        if (mymode == AUTO) {
          if(Gui::Selection().setPreselect(documentName.getValue().getString()
                                           ,objectName.getValue().getString()
                                           ,subElementName.getValue().getString()
                                           ,pp->getPoint()[0]
                                           ,pp->getPoint()[1]
                                           ,pp->getPoint()[2])){
            SoFCSelection::turnoffcurrent(action);
            SoFCSelection::currenthighlight = (SoFullPath*)
              action->getCurPath()->copy();
            SoFCSelection::currenthighlight->ref();
            highlighted = TRUE;
            this->touch(); // force scene redraw
            this->redrawHighlighted(action, TRUE);
          }
        } else {
         // preselection in the viewer is disabled but we must inform all observers anyway
         if (Gui::Selection().setPreselect(documentName.getValue().getString()
                                           ,objectName.getValue().getString()
                                           ,subElementName.getValue().getString()
                                           ,pp->getPoint()[0]
                                           ,pp->getPoint()[1]
                                           ,pp->getPoint()[2]))
           highlighted = TRUE;
        }
      }
      snprintf(buf,512,"Preselected: %s.%s.%s (%f,%f,%f)",documentName.getValue().getString()
                                                      ,objectName.getValue().getString()
                                                      ,subElementName.getValue().getString()
                                                      ,pp->getPoint()[0]
                                                      ,pp->getPoint()[1]
                                                      ,pp->getPoint()[2]);

      getMainWindow()->statusBar()->message(buf,3000);
    }
    else {
      if (highlighted) {
        if (mymode == AUTO)
          SoFCSelection::turnoffcurrent(action);
        //FIXME: I think we should set 'highlighted' to false whenever no point is picked
//        else
          highlighted = FALSE;
        Gui::Selection().rmvPreselect();
      }
    }
  }else if (event->isOfType(SoKeyboardEvent ::getClassTypeId())) {
    //const SoPickedPoint * pp = action->getPickedPoint();
    SoKeyboardEvent  * const e = (SoKeyboardEvent  *) event;

    if (SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::LEFT_SHIFT) ||
        SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::RIGHT_SHIFT)   )
        bShift = true;
    if (SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::LEFT_SHIFT) ||
        SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::RIGHT_SHIFT)   )
        bShift = false;

    if (SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::LEFT_CONTROL) ||
        SoKeyboardEvent::isKeyPressEvent(e,SoKeyboardEvent::RIGHT_CONTROL)   )
        bCtrl = true;
    if (SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::LEFT_CONTROL) ||
        SoKeyboardEvent::isKeyReleaseEvent(e,SoKeyboardEvent::RIGHT_CONTROL)   )
        bCtrl = false;

     
  }else if (event->isOfType(SoMouseButtonEvent::getClassTypeId())) {
    const SoPickedPoint * pp = action->getPickedPoint();
    SoMouseButtonEvent * const e = (SoMouseButtonEvent *) event;

    //FIXME: Shouldn't we remove the preselection for newly selected objects?
    //       Otherwise the tree signals that an object is preselected even though it is hidden. (Werner)
    if (pp && pp->getPath()->containsPath(action->getCurPath())) {
      if (SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1)) {
        if(bCtrl)
        {
          if(Gui::Selection().isSelected(documentName.getValue().getString()
                                         ,objectName.getValue().getString()
                                         ,subElementName.getValue().getString()))
          {
            Gui::Selection().rmvSelection(documentName.getValue().getString()
                                          ,objectName.getValue().getString()
                                          ,subElementName.getValue().getString());
          }else{
            Gui::Selection().addSelection(documentName.getValue().getString()
                                          ,objectName.getValue().getString()
                                          ,subElementName.getValue().getString()
                                          ,pp->getPoint()[0]
                                          ,pp->getPoint()[1]
                                          ,pp->getPoint()[2]);
          }
        }else{
          Gui::Selection().clearSelection(documentName.getValue().getString());
          Gui::Selection().addSelection(documentName.getValue().getString()
                                          ,objectName.getValue().getString()
                                          ,subElementName.getValue().getString()
                                          ,pp->getPoint()[0]
                                          ,pp->getPoint()[1]
                                          ,pp->getPoint()[2]);
        }


        action->setHandled(); 
      }
    
    }
  }

  inherited::handleEvent(action);
}

// doc from parent
void
SoFCSelection::GLRenderBelowPath(SoGLRenderAction * action)
{
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
}

// doc from parent
void
SoFCSelection::GLRenderInPath(SoGLRenderAction * action)
{
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
}

/*!
  Empty method in Coin. Can be used by subclasses to be told
  when status change.
*/
void
SoFCSelection::redrawHighlighted(SoAction * /* act */, SbBool /* flag */)
{
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
}

//#undef THIS
