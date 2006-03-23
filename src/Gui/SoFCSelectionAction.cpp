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
  SO_ACTION_ADD_METHOD(SoFCSelection,selNode);
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

void SoFCSelectionAction::selNode(SoAction *action,SoNode *node)
{
  node->doAction(action);
  //Base::Console().Log("SoFCSelectionAction::selNode()");
}

void SoFCSelectionAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

