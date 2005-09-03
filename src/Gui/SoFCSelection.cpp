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
#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/SoPickedPoint.h>

#include "SoFCSelection.h"
#include "Application.h"

using namespace Gui;

SoFullPath * Gui::SoFCSelection::currenthighlight = NULL;


/*
namespace Gui {
class SoFCSelectionP {
public:
  SbBool highlighted;
  SoColorPacker colorpacker;
};
}
*/

//#undef THIS
//#define THIS this->pimpl

// *************************************************************************

SO_NODE_SOURCE(SoFCSelection);

/*!
  Constructor.
*/
SoFCSelection::SoFCSelection()
{
//  THIS = new SoFCSelectionP;
//  SO_NODE_INTERNAL_CONSTRUCTOR(SoFCSelection);

  SO_NODE_CONSTRUCTOR(SoFCSelection);

  SO_NODE_ADD_FIELD(colorHighlight, (SbColor(0.1f, 0.1f, 0.5f)));
  SO_NODE_ADD_FIELD(colorSelection, (SbColor(0.1f, 0.5f, 0.1f)));
  SO_NODE_ADD_FIELD(style,          (EMISSIVE));
  SO_NODE_ADD_FIELD(mode,           (AUTO));
  SO_NODE_ADD_FIELD(selected,       (OFF));
  SO_NODE_ADD_FIELD(documentName,   (""));
  SO_NODE_ADD_FIELD(featureName,    (""));
  SO_NODE_ADD_FIELD(subElementName, (""));

  SO_NODE_DEFINE_ENUM_VALUE(Styles, EMISSIVE);
  SO_NODE_DEFINE_ENUM_VALUE(Styles, EMISSIVE_DIFFUSE);
  SO_NODE_SET_SF_ENUM_TYPE(style, Styles);

  SO_NODE_DEFINE_ENUM_VALUE(Modes, AUTO);
  SO_NODE_DEFINE_ENUM_VALUE(Modes, ON);
  SO_NODE_DEFINE_ENUM_VALUE(Modes, OFF);
  SO_NODE_SET_SF_ENUM_TYPE(mode, Modes);

  SO_NODE_DEFINE_ENUM_VALUE(Selected, NOTSELECTED);
  SO_NODE_DEFINE_ENUM_VALUE(Selected, SELECTED);
  SO_NODE_SET_SF_ENUM_TYPE(selected, Selected);

  highlighted = FALSE;
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

// doc from parent
void
SoFCSelection::handleEvent(SoHandleEventAction * action)
{
  Modes mymode = (Modes) this->mode.getValue();
  if (mymode == AUTO) {
    const SoEvent * event = action->getEvent();
    if (event->isOfType(SoLocation2Event::getClassTypeId())) {
      const SoPickedPoint * pp = action->getPickedPoint();
      if (pp && pp->getPath()->containsPath(action->getCurPath())) {
        if (!highlighted) {
          SoFCSelection::turnoffcurrent(action);
          SoFCSelection::currenthighlight = (SoFullPath*)
            action->getCurPath()->copy();
          SoFCSelection::currenthighlight->ref();
          highlighted = TRUE;
          QString msg("Preselection: ");
          msg += documentName.getValue().getString();
          msg += ".";
          msg += featureName.getValue().getString();
          msg += ".";
          msg += subElementName.getValue().getString();
          Gui::ApplicationWindow::Instance->statusBar()->message(msg);
          this->touch(); // force scene redraw
          this->redrawHighlighted(action, TRUE);
        }
      }
      else {
        if (highlighted) {
          SoFCSelection::turnoffcurrent(action);
        }
      }
    }
  }
  inherited::handleEvent(action);
}

// doc from parent
void
SoFCSelection::GLRenderBelowPath(SoGLRenderAction * action)
{
  SoState * state = action->getState();
  state->push();
  if (highlighted || this->mode.getValue() == ON || this->selected.getValue() == SELECTED) {
    this->setOverride(action);
  }
  inherited::GLRenderBelowPath(action);
  state->pop();
}

// doc from parent
void
SoFCSelection::GLRenderInPath(SoGLRenderAction * action)
{
  SoState * state = action->getState();
  state->push();
  if (highlighted || this->mode.getValue() == ON || this->selected.getValue() == SELECTED) {
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
