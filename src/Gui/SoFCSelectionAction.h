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


#ifndef _SoFCSelectionAction_h
#define _SoFCSelectionAction_h

//#include <Inventor/SoAction.h> 
#include <Inventor/actions/SoSubAction.h> 

namespace Gui {

class SelectionChanges;

/**
 * The SoFCOffscreenRenderer class is used for rendering scenes in offscreen buffers. 
 * @author Werner Mayer
 */
class GuiExport SoFCSelectionAction : public SoAction
{

  SO_ACTION_HEADER(SoFCSelectionAction);

public:
  SoFCSelectionAction (const SelectionChanges &SelCh);
	~SoFCSelectionAction();

  static void initClass();

  const SelectionChanges &SelChange;


protected:
  virtual void beginTraversel(SoNode *node);


private:
  static void selNode(SoAction *action,SoNode *node);
  static void callDoAction(SoAction *action,SoNode *node);
};

} // namespace Gui


#endif // _SoFCSelectionAction_h 
