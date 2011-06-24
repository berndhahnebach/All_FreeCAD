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
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/details/SoLineDetail.h>
#endif

#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoWindowElement.h>

#include <Inventor/SoFullPath.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/SoPickedPoint.h>

#include "View3DInventor.h"
#include "View3DInventorViewer.h"

#include <Base/Console.h>
#include "SoFCUnifiedSelection.h"
#include "MainWindow.h"
#include "Selection.h"
#include "ViewProvider.h"
#include "SoFCInteractiveElement.h"

// For 64-bit system the method using the front buffer doesn't work at all for lines.
// Thus, use the method which forces a redraw every time. This is a bit slower but at
// least it works.
#if defined(_OCC64) // is set by configure or cmake
# define NO_FRONTBUFFER
#endif

using namespace Gui;

SoFullPath * Gui::SoFCUnifiedSelection::currenthighlight = NULL;


// *************************************************************************

SO_NODE_SOURCE(SoFCUnifiedSelection);

/*!
  Constructor.
*/
SoFCUnifiedSelection::SoFCUnifiedSelection()
{
    SO_NODE_CONSTRUCTOR(SoFCUnifiedSelection);

    SO_NODE_ADD_FIELD(colorHighlight, (SbColor(0.8f, 0.1f, 0.1f)));
    SO_NODE_ADD_FIELD(colorSelection, (SbColor(0.1f, 0.8f, 0.1f)));
    SO_NODE_ADD_FIELD(style,          (EMISSIVE));
    SO_NODE_ADD_FIELD(highlightMode,  (AUTO));
    SO_NODE_ADD_FIELD(selectionMode,  (SEL_ON));
    SO_NODE_ADD_FIELD(selected,       (NOTSELECTED));
    SO_NODE_ADD_FIELD(documentName,   (""));
    SO_NODE_ADD_FIELD(objectName,     (""));
    SO_NODE_ADD_FIELD(subElementName, (""));

    SO_NODE_DEFINE_ENUM_VALUE(Styles, EMISSIVE);
    SO_NODE_DEFINE_ENUM_VALUE(Styles, EMISSIVE_DIFFUSE);
    SO_NODE_DEFINE_ENUM_VALUE(Styles, BOX);
    SO_NODE_SET_SF_ENUM_TYPE(style,   Styles);

    SO_NODE_DEFINE_ENUM_VALUE(HighlightModes, AUTO);
    SO_NODE_DEFINE_ENUM_VALUE(HighlightModes, ON);
    SO_NODE_DEFINE_ENUM_VALUE(HighlightModes, OFF);
    SO_NODE_SET_SF_ENUM_TYPE (highlightMode, HighlightModes);

    SO_NODE_DEFINE_ENUM_VALUE(SelectionModes, SEL_ON);
    SO_NODE_DEFINE_ENUM_VALUE(SelectionModes, SEL_OFF);
    SO_NODE_SET_SF_ENUM_TYPE (selectionMode,  SelectionModes);

    SO_NODE_DEFINE_ENUM_VALUE(Selected, NOTSELECTED);
    SO_NODE_DEFINE_ENUM_VALUE(Selected, SELECTED);
    SO_NODE_SET_SF_ENUM_TYPE(selected,  Selected);

    highlighted = FALSE;
    bShift      = FALSE;
    bCtrl       = FALSE;

    selected = NOTSELECTED;
}

/*!
  Destructor.
*/
SoFCUnifiedSelection::~SoFCUnifiedSelection()
{
    //// If we're being deleted and we're the current highlight,
    //// NULL out that variable
    //if (currenthighlight != NULL &&
    //    (!currenthighlight->getTail()->isOfType(SoFCUnifiedSelection::getClassTypeId()))) {
    //    currenthighlight->unref();
    //    currenthighlight = NULL;
    //}
    ////delete THIS;
}

// doc from parent
void
SoFCUnifiedSelection::initClass(void)
{
    SO_NODE_INIT_CLASS(SoFCUnifiedSelection,SoSeparator,"Separator");
}

void SoFCUnifiedSelection::finish()
{
    atexit_cleanup();
}


const SoPickedPoint*
SoFCUnifiedSelection::getPickedPoint(SoHandleEventAction* action) const
{
    // To identify the picking of lines in a concave area we have to 
    // get all intersection points. If we have two or more intersection
    // points where the first is of a face and the second of a line with
    // almost similar coordinates we use the second point, instead.
    const SoPickedPointList & points = action->getPickedPointList();
    if (points.getLength() == 0)
        return 0;
    else if (points.getLength() == 1)
        return points[0];
    const SoPickedPoint* pp0 = points[0];
    const SoPickedPoint* pp1 = points[1];
    const SoDetail* det0 = pp0->getDetail();
    const SoDetail* det1 = pp1->getDetail();
    if (det0 && det0->isOfType(SoFaceDetail::getClassTypeId()) &&
        det1 && det1->isOfType(SoLineDetail::getClassTypeId())) {
        const SbVec3f& pt0 = pp0->getPoint();
        const SbVec3f& pt1 = pp1->getPoint();
        if (pt0.equals(pt1, 0.01f))
            return pp1;
    }

    return pp0;
}

// doc from parent
void
SoFCUnifiedSelection::handleEvent(SoHandleEventAction * action)
{
    static char buf[513];
    HighlightModes mymode = (HighlightModes) this->highlightMode.getValue();
    const SoEvent * event = action->getEvent();

    // If we don't need to pick for locate highlighting,
    // then just behave as separator and return.
    // NOTE: we still have to pick for ON even though we don't have
    // to re-render, because the app needs to be notified as the mouse
    // goes over locate highlight nodes.
    //if (highlightMode.getValue() == OFF) {
    //    inherited::handleEvent( action );
    //    return;
    //}

    
    //
    // If this is a mouseMotion event, then check for locate highlighting
    //
    if (event->isOfType(SoLocation2Event::getClassTypeId())) {
        // check to see if the mouse is over our geometry...
        const SoPickedPoint * pp = this->getPickedPoint(action);
        SoFullPath *pPath = (pp != NULL) ? (SoFullPath *) pp->getPath() : NULL;
        ViewProvider *vp = 0;
        if (pPath && pPath->containsPath(action->getCurPath())) {
            // Make sure I'm the lowest LocHL in the pick path!
            for (int i = 0; i < pPath->getLength(); i++) {
                SoNode *node = pPath->getNodeFromTail(i);
                if (node->isOfType(SoSeparator::getClassTypeId())) {
                    std::map<SoSeparator*,ViewProvider*>::iterator it = viewer->_ViewProviderMap.find(static_cast<SoSeparator*>(node));
                    if(it != viewer->_ViewProviderMap.end()){
                        vp=it->second;
                        break;
                    }
                 }
            }
        }

        if(vp && vp->useNewSelectionModel()){
            const char * e = vp->getElement(pp);

            vp->getSelectionShape(0);
        }
    }
    // key press events
    else if (event->isOfType(SoKeyboardEvent ::getClassTypeId())) {
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
    }
    //// mouse press events for (de)selection (only if selection is enabled on this node)
    //else if (event->isOfType(SoMouseButtonEvent::getClassTypeId()) && 
    //         selectionMode.getValue() == SoFCUnifiedSelection::SEL_ON) {
    //    SoMouseButtonEvent * const e = (SoMouseButtonEvent *) event;
    //    if (SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1)) {
    //        //FIXME: Shouldn't we remove the preselection for newly selected objects?
    //        //       Otherwise the tree signals that an object is preselected even though it is hidden. (Werner)
    //        const SoPickedPoint * pp = this->getPickedPoint(action);
    //        if (pp && pp->getPath()->containsPath(action->getCurPath())) {
    //            if (bCtrl) {
    //                if (Gui::Selection().isSelected(documentName.getValue().getString()
    //                                     ,objectName.getValue().getString()
    //                                     ,subElementName.getValue().getString())) {
    //                    Gui::Selection().rmvSelection(documentName.getValue().getString()
    //                                      ,objectName.getValue().getString()
    //                                      ,subElementName.getValue().getString());
    //                }
    //                else {
    //                    Gui::Selection().addSelection(documentName.getValue().getString()
    //                                      ,objectName.getValue().getString()
    //                                      ,subElementName.getValue().getString()
    //                                      ,pp->getPoint()[0]
    //                                      ,pp->getPoint()[1]
    //                                      ,pp->getPoint()[2]);

    //                    if (mymode == OFF) {
    //                        snprintf(buf,512,"Selected: %s.%s.%s (%f,%f,%f)",documentName.getValue().getString()
    //                                                   ,objectName.getValue().getString()
    //                                                   ,subElementName.getValue().getString()
    //                                                   ,pp->getPoint()[0]
    //                                                   ,pp->getPoint()[1]
    //                                                   ,pp->getPoint()[2]);

    //                        getMainWindow()->statusBar()->showMessage(QString::fromAscii(buf),3000);
    //                    }
    //                }
    //            }
    //            else { // Ctrl
    //                if (!Gui::Selection().isSelected(documentName.getValue().getString()
    //                                     ,objectName.getValue().getString()
    //                                     ,subElementName.getValue().getString())) {
    //                    Gui::Selection().clearSelection(documentName.getValue().getString());
    //                    Gui::Selection().addSelection(documentName.getValue().getString()
    //                                          ,objectName.getValue().getString()
    //                                          ,subElementName.getValue().getString()
    //                                          ,pp->getPoint()[0]
    //                                          ,pp->getPoint()[1]
    //                                          ,pp->getPoint()[2]);
    //                }
    //                else {
    //                    Gui::Selection().clearSelection(documentName.getValue().getString());
    //                    Gui::Selection().addSelection(documentName.getValue().getString()
    //                                          ,objectName.getValue().getString()
    //                                          ,0
    //                                          ,pp->getPoint()[0]
    //                                          ,pp->getPoint()[1]
    //                                          ,pp->getPoint()[2]);
    //                }
 
    //                if (mymode == OFF) {
    //                    snprintf(buf,512,"Selected: %s.%s.%s (%f,%f,%f)",documentName.getValue().getString()
    //                                               ,objectName.getValue().getString()
    //                                               ,subElementName.getValue().getString()
    //                                               ,pp->getPoint()[0]
    //                                               ,pp->getPoint()[1]
    //                                               ,pp->getPoint()[2]);

    //                    getMainWindow()->statusBar()->showMessage(QString::fromAscii(buf),3000);
    //                }
    //            }

    //            action->setHandled(); 
    //        } // picked point
    //    } // mouse release
    //}

    //// Let the base class traverse the children.
    //if (action->getGrabber() != this)
        inherited::handleEvent(action);
}

