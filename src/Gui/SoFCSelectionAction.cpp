/***************************************************************************
 * 
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
#  include <Inventor/nodes/SoCallback.h>
#  include <Inventor/nodes/SoComplexity.h>
#  include <Inventor/nodes/SoCoordinate3.h>
#  include <Inventor/nodes/SoCoordinate4.h>
#  include <Inventor/nodes/SoFont.h>
#  include <Inventor/nodes/SoProfile.h>
#  include <Inventor/nodes/SoProfileCoordinate2.h>
#  include <Inventor/nodes/SoProfileCoordinate3.h>
#  include <Inventor/nodes/SoSwitch.h>
#  include <Inventor/nodes/SoTransformation.h>
#endif

#include <Base/Console.h>

#include "SoFCSelectionAction.h"
#include "SoFCSelection.h"
#  include <Inventor/bundles/SoMaterialBundle.h>
#  include <Inventor/elements/SoSwitchElement.h>
#include "Selection.h"

#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoElements.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/elements/SoSwitchElement.h>
#include <Inventor/elements/SoUnitsElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoProfile.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoProfileCoordinate3.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransformation.h>




#define new DEBUG_CLIENTBLOCK
using namespace Gui;


SO_ACTION_SOURCE(SoFCSelectionAction);

/**
 * The order of the defined SO_ACTION_ADD_METHOD statements is very important. First the base 
 * classes and afterwards subclasses of them must be listed, otherwise the registered methods 
 * of subclasses will be overridden. For more details see the thread in the Coin3d forum 
 * https://www.coin3d.org/pipermail/coin-discuss/2004-May/004346.html.
 * This means that \c SoSwitch must be listed after \c SoGroup and \c SoFCSelection after 
 * \c SoSeparator because both classes inherits the others.
 */
void SoFCSelectionAction::initClass()
{
  SO_ACTION_INIT_CLASS(SoFCSelectionAction,SoAction);

  SO_ENABLE(SoFCSelectionAction, SoSwitchElement);

  SO_ACTION_ADD_METHOD(SoNode,nullAction);

  SO_ENABLE(SoFCSelectionAction, SoModelMatrixElement);
  SO_ENABLE(SoFCSelectionAction, SoShapeStyleElement);
  SO_ENABLE(SoFCSelectionAction, SoComplexityElement);
  SO_ENABLE(SoFCSelectionAction, SoComplexityTypeElement);
  SO_ENABLE(SoFCSelectionAction, SoCoordinateElement);
  SO_ENABLE(SoFCSelectionAction, SoFontNameElement);
  SO_ENABLE(SoFCSelectionAction, SoFontSizeElement);
  SO_ENABLE(SoFCSelectionAction, SoProfileCoordinateElement);
  SO_ENABLE(SoFCSelectionAction, SoProfileElement);
  SO_ENABLE(SoFCSelectionAction, SoSwitchElement);
  SO_ENABLE(SoFCSelectionAction, SoUnitsElement);
  SO_ENABLE(SoFCSelectionAction, SoViewVolumeElement);
  SO_ENABLE(SoFCSelectionAction, SoViewingMatrixElement);
  SO_ENABLE(SoFCSelectionAction, SoViewportRegionElement);




  SO_ACTION_ADD_METHOD(SoCallback,callDoAction);
  SO_ACTION_ADD_METHOD(SoComplexity,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate4,callDoAction);
  SO_ACTION_ADD_METHOD(SoFont,callDoAction);
  SO_ACTION_ADD_METHOD(SoGroup,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfile,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate2,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoTransformation,callDoAction);
  SO_ACTION_ADD_METHOD(SoSwitch,callDoAction);

  SO_ACTION_ADD_METHOD(SoSeparator,callDoAction);
  SO_ACTION_ADD_METHOD(SoFCSelection,callDoAction);
}


SoFCSelectionAction::SoFCSelectionAction (const SelectionChanges &SelCh)
:SelChange(SelCh)
{
  SO_ACTION_CONSTRUCTOR(SoFCSelectionAction);
}


SoFCSelectionAction::~SoFCSelectionAction()
{
}

 
void SoFCSelectionAction::beginTraversal(SoNode *node)
{
  traverse(node);
}

void SoFCSelectionAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

// ---------------------------------------------------------------

SO_ACTION_SOURCE(SoFCEnableSelectionAction);

/**
 * The order of the defined SO_ACTION_ADD_METHOD statements is very important. First the base 
 * classes and afterwards subclasses of them must be listed, otherwise the registered methods 
 * of subclasses will be overridden. For more details see the thread in the Coin3d forum 
 * https://www.coin3d.org/pipermail/coin-discuss/2004-May/004346.html.
 * This means that \c SoSwitch must be listed after \c SoGroup and \c SoFCSelection after 
 * \c SoSeparator because both classes inherits the others.
 */
void SoFCEnableSelectionAction::initClass()
{
  SO_ACTION_INIT_CLASS(SoFCEnableSelectionAction,SoAction);

  SO_ENABLE(SoFCEnableSelectionAction, SoSwitchElement);

  SO_ACTION_ADD_METHOD(SoNode,nullAction);

  SO_ENABLE(SoFCEnableSelectionAction, SoModelMatrixElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoShapeStyleElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoComplexityElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoComplexityTypeElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoCoordinateElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoFontNameElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoFontSizeElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoProfileCoordinateElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoProfileElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoSwitchElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoUnitsElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoViewVolumeElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoViewingMatrixElement);
  SO_ENABLE(SoFCEnableSelectionAction, SoViewportRegionElement);




  SO_ACTION_ADD_METHOD(SoCallback,callDoAction);
  SO_ACTION_ADD_METHOD(SoComplexity,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate4,callDoAction);
  SO_ACTION_ADD_METHOD(SoFont,callDoAction);
  SO_ACTION_ADD_METHOD(SoGroup,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfile,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate2,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoTransformation,callDoAction);
  SO_ACTION_ADD_METHOD(SoSwitch,callDoAction);

  SO_ACTION_ADD_METHOD(SoSeparator,callDoAction);
  SO_ACTION_ADD_METHOD(SoFCSelection,callDoAction);
}


SoFCEnableSelectionAction::SoFCEnableSelectionAction (const SbBool& sel)
  : selection(sel)
{
  SO_ACTION_CONSTRUCTOR(SoFCEnableSelectionAction);
}


SoFCEnableSelectionAction::~SoFCEnableSelectionAction()
{
}

 
void SoFCEnableSelectionAction::beginTraversal(SoNode *node)
{
  traverse(node);
}

void SoFCEnableSelectionAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

// ---------------------------------------------------------------

SO_ACTION_SOURCE(SoFCEnableHighlightAction);

/**
 * The order of the defined SO_ACTION_ADD_METHOD statements is very important. First the base 
 * classes and afterwards subclasses of them must be listed, otherwise the registered methods 
 * of subclasses will be overridden. For more details see the thread in the Coin3d forum 
 * https://www.coin3d.org/pipermail/coin-discuss/2004-May/004346.html.
 * This means that \c SoSwitch must be listed after \c SoGroup and \c SoFCSelection after 
 * \c SoSeparator because both classes inherits the others.
 */
void SoFCEnableHighlightAction::initClass()
{
  SO_ACTION_INIT_CLASS(SoFCEnableHighlightAction,SoAction);

  SO_ENABLE(SoFCEnableHighlightAction, SoSwitchElement);

  SO_ACTION_ADD_METHOD(SoNode,nullAction);

  SO_ENABLE(SoFCEnableHighlightAction, SoModelMatrixElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoShapeStyleElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoComplexityElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoComplexityTypeElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoCoordinateElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoFontNameElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoFontSizeElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoProfileCoordinateElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoProfileElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoSwitchElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoUnitsElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoViewVolumeElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoViewingMatrixElement);
  SO_ENABLE(SoFCEnableHighlightAction, SoViewportRegionElement);




  SO_ACTION_ADD_METHOD(SoCallback,callDoAction);
  SO_ACTION_ADD_METHOD(SoComplexity,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate4,callDoAction);
  SO_ACTION_ADD_METHOD(SoFont,callDoAction);
  SO_ACTION_ADD_METHOD(SoGroup,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfile,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate2,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoTransformation,callDoAction);
  SO_ACTION_ADD_METHOD(SoSwitch,callDoAction);

  SO_ACTION_ADD_METHOD(SoSeparator,callDoAction);
  SO_ACTION_ADD_METHOD(SoFCSelection,callDoAction);
}


SoFCEnableHighlightAction::SoFCEnableHighlightAction (const SbBool& sel)
  : highlight(sel)
{
  SO_ACTION_CONSTRUCTOR(SoFCEnableHighlightAction);
}


SoFCEnableHighlightAction::~SoFCEnableHighlightAction()
{
}

 
void SoFCEnableHighlightAction::beginTraversal(SoNode *node)
{
  traverse(node);
}

void SoFCEnableHighlightAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

// ---------------------------------------------------------------

SO_ACTION_SOURCE(SoFCSelectionColorAction);

/**
 * The order of the defined SO_ACTION_ADD_METHOD statements is very important. First the base 
 * classes and afterwards subclasses of them must be listed, otherwise the registered methods 
 * of subclasses will be overridden. For more details see the thread in the Coin3d forum 
 * https://www.coin3d.org/pipermail/coin-discuss/2004-May/004346.html.
 * This means that \c SoSwitch must be listed after \c SoGroup and \c SoFCSelection after 
 * \c SoSeparator because both classes inherits the others.
 */
void SoFCSelectionColorAction::initClass()
{
  SO_ACTION_INIT_CLASS(SoFCSelectionColorAction,SoAction);

  SO_ENABLE(SoFCSelectionColorAction, SoSwitchElement);

  SO_ACTION_ADD_METHOD(SoNode,nullAction);

  SO_ENABLE(SoFCSelectionColorAction, SoModelMatrixElement);
  SO_ENABLE(SoFCSelectionColorAction, SoShapeStyleElement);
  SO_ENABLE(SoFCSelectionColorAction, SoComplexityElement);
  SO_ENABLE(SoFCSelectionColorAction, SoComplexityTypeElement);
  SO_ENABLE(SoFCSelectionColorAction, SoCoordinateElement);
  SO_ENABLE(SoFCSelectionColorAction, SoFontNameElement);
  SO_ENABLE(SoFCSelectionColorAction, SoFontSizeElement);
  SO_ENABLE(SoFCSelectionColorAction, SoProfileCoordinateElement);
  SO_ENABLE(SoFCSelectionColorAction, SoProfileElement);
  SO_ENABLE(SoFCSelectionColorAction, SoSwitchElement);
  SO_ENABLE(SoFCSelectionColorAction, SoUnitsElement);
  SO_ENABLE(SoFCSelectionColorAction, SoViewVolumeElement);
  SO_ENABLE(SoFCSelectionColorAction, SoViewingMatrixElement);
  SO_ENABLE(SoFCSelectionColorAction, SoViewportRegionElement);




  SO_ACTION_ADD_METHOD(SoCallback,callDoAction);
  SO_ACTION_ADD_METHOD(SoComplexity,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate4,callDoAction);
  SO_ACTION_ADD_METHOD(SoFont,callDoAction);
  SO_ACTION_ADD_METHOD(SoGroup,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfile,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate2,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoTransformation,callDoAction);
  SO_ACTION_ADD_METHOD(SoSwitch,callDoAction);

  SO_ACTION_ADD_METHOD(SoSeparator,callDoAction);
  SO_ACTION_ADD_METHOD(SoFCSelection,callDoAction);
}


SoFCSelectionColorAction::SoFCSelectionColorAction (const SoSFColor& col)
  : selectionColor(col)
{
  SO_ACTION_CONSTRUCTOR(SoFCSelectionColorAction);
}


SoFCSelectionColorAction::~SoFCSelectionColorAction()
{
}

 
void SoFCSelectionColorAction::beginTraversal(SoNode *node)
{
  traverse(node);
}

void SoFCSelectionColorAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

// ---------------------------------------------------------------

SO_ACTION_SOURCE(SoFCHighlightColorAction);

/**
 * The order of the defined SO_ACTION_ADD_METHOD statements is very important. First the base 
 * classes and afterwards subclasses of them must be listed, otherwise the registered methods 
 * of subclasses will be overridden. For more details see the thread in the Coin3d forum 
 * https://www.coin3d.org/pipermail/coin-discuss/2004-May/004346.html.
 * This means that \c SoSwitch must be listed after \c SoGroup and \c SoFCSelection after 
 * \c SoSeparator because both classes inherits the others.
 */
void SoFCHighlightColorAction::initClass()
{
  SO_ACTION_INIT_CLASS(SoFCHighlightColorAction,SoAction);

  SO_ENABLE(SoFCHighlightColorAction, SoSwitchElement);

  SO_ACTION_ADD_METHOD(SoNode,nullAction);

  SO_ENABLE(SoFCHighlightColorAction, SoModelMatrixElement);
  SO_ENABLE(SoFCHighlightColorAction, SoShapeStyleElement);
  SO_ENABLE(SoFCHighlightColorAction, SoComplexityElement);
  SO_ENABLE(SoFCHighlightColorAction, SoComplexityTypeElement);
  SO_ENABLE(SoFCHighlightColorAction, SoCoordinateElement);
  SO_ENABLE(SoFCHighlightColorAction, SoFontNameElement);
  SO_ENABLE(SoFCHighlightColorAction, SoFontSizeElement);
  SO_ENABLE(SoFCHighlightColorAction, SoProfileCoordinateElement);
  SO_ENABLE(SoFCHighlightColorAction, SoProfileElement);
  SO_ENABLE(SoFCHighlightColorAction, SoSwitchElement);
  SO_ENABLE(SoFCHighlightColorAction, SoUnitsElement);
  SO_ENABLE(SoFCHighlightColorAction, SoViewVolumeElement);
  SO_ENABLE(SoFCHighlightColorAction, SoViewingMatrixElement);
  SO_ENABLE(SoFCHighlightColorAction, SoViewportRegionElement);




  SO_ACTION_ADD_METHOD(SoCallback,callDoAction);
  SO_ACTION_ADD_METHOD(SoComplexity,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate4,callDoAction);
  SO_ACTION_ADD_METHOD(SoFont,callDoAction);
  SO_ACTION_ADD_METHOD(SoGroup,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfile,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate2,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoTransformation,callDoAction);
  SO_ACTION_ADD_METHOD(SoSwitch,callDoAction);

  SO_ACTION_ADD_METHOD(SoSeparator,callDoAction);
  SO_ACTION_ADD_METHOD(SoFCSelection,callDoAction);
}


SoFCHighlightColorAction::SoFCHighlightColorAction (const SoSFColor& col)
  : highlightColor(col)
{
  SO_ACTION_CONSTRUCTOR(SoFCHighlightColorAction);
}


SoFCHighlightColorAction::~SoFCHighlightColorAction()
{
}

 
void SoFCHighlightColorAction::beginTraversal(SoNode *node)
{
  traverse(node);
}

void SoFCHighlightColorAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

// ---------------------------------------------------------------

SO_ACTION_SOURCE(SoFCDocumentAction);

/**
 * The order of the defined SO_ACTION_ADD_METHOD statements is very important. First the base 
 * classes and afterwards subclasses of them must be listed, otherwise the registered methods 
 * of subclasses will be overridden. For more details see the thread in the Coin3d forum 
 * https://www.coin3d.org/pipermail/coin-discuss/2004-May/004346.html.
 * This means that \c SoSwitch must be listed after \c SoGroup and \c SoFCSelection after 
 * \c SoSeparator because both classes inherits the others.
 */
void SoFCDocumentAction::initClass()
{
  SO_ACTION_INIT_CLASS(SoFCDocumentAction,SoAction);

  SO_ENABLE(SoFCDocumentAction, SoSwitchElement);

  SO_ACTION_ADD_METHOD(SoNode,nullAction);

  SO_ENABLE(SoFCDocumentAction, SoModelMatrixElement);
  SO_ENABLE(SoFCDocumentAction, SoShapeStyleElement);
  SO_ENABLE(SoFCDocumentAction, SoComplexityElement);
  SO_ENABLE(SoFCDocumentAction, SoComplexityTypeElement);
  SO_ENABLE(SoFCDocumentAction, SoCoordinateElement);
  SO_ENABLE(SoFCDocumentAction, SoFontNameElement);
  SO_ENABLE(SoFCDocumentAction, SoFontSizeElement);
  SO_ENABLE(SoFCDocumentAction, SoProfileCoordinateElement);
  SO_ENABLE(SoFCDocumentAction, SoProfileElement);
  SO_ENABLE(SoFCDocumentAction, SoSwitchElement);
  SO_ENABLE(SoFCDocumentAction, SoUnitsElement);
  SO_ENABLE(SoFCDocumentAction, SoViewVolumeElement);
  SO_ENABLE(SoFCDocumentAction, SoViewingMatrixElement);
  SO_ENABLE(SoFCDocumentAction, SoViewportRegionElement);




  SO_ACTION_ADD_METHOD(SoCallback,callDoAction);
  SO_ACTION_ADD_METHOD(SoComplexity,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate4,callDoAction);
  SO_ACTION_ADD_METHOD(SoFont,callDoAction);
  SO_ACTION_ADD_METHOD(SoGroup,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfile,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate2,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoTransformation,callDoAction);
  SO_ACTION_ADD_METHOD(SoSwitch,callDoAction);

  SO_ACTION_ADD_METHOD(SoSeparator,callDoAction);
  SO_ACTION_ADD_METHOD(SoFCSelection,callDoAction);
}


SoFCDocumentAction::SoFCDocumentAction (const SoSFString& docName)
  : documentName(docName)
{
  SO_ACTION_CONSTRUCTOR(SoFCDocumentAction);
}


SoFCDocumentAction::~SoFCDocumentAction()
{
}

 
void SoFCDocumentAction::beginTraversal(SoNode *node)
{
  traverse(node);
}

void SoFCDocumentAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

