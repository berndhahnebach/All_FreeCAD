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

using namespace Gui;


SO_ACTION_SOURCE(SoFCSelectionAction);

void SoFCSelectionAction::initClass()
{
  SO_ACTION_INIT_CLASS(SoFCSelectionAction,SoAction);

  SO_ENABLE(SoFCSelectionAction, SoSwitchElement);

  SO_ACTION_ADD_METHOD(SoNode,nullAction);
  SO_ACTION_ADD_METHOD(SoFCSelection,selNode);


  SO_ACTION_ADD_METHOD(SoComplexity,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoCoordinate4,callDoAction);
  SO_ACTION_ADD_METHOD(SoFont,callDoAction);
  SO_ACTION_ADD_METHOD(SoGroup,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfile,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate2,callDoAction);
  SO_ACTION_ADD_METHOD(SoProfileCoordinate3,callDoAction);
  SO_ACTION_ADD_METHOD(SoTransformation,callDoAction);
  SO_ACTION_ADD_METHOD(SoSeparator,callDoAction);
  SO_ACTION_ADD_METHOD(SoSwitch,callDoAction);
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
  Base::Console().Log("SoFCSelectionAction::selNode()");
}
void SoFCSelectionAction::callDoAction(SoAction *action,SoNode *node)
{
  node->doAction(action);
}

