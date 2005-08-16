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

#ifndef COIN_SoFCSelection_H
#define COIN_SoFCSelection_H


#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/nodes/SoLightModel.h>

class SoFullPath;


namespace Gui {



/** FreeCAD selection node
 *  \author Jürgen Riegel
 */
class GuiExport SoFCSelection : public SoSeparator {
  typedef SoSeparator inherited;

  SO_NODE_HEADER(Gui::SoFCSelection);

public:
  static void initClass(void);
  SoFCSelection(void);

  enum Modes {
    AUTO, ON, OFF
  };

  enum Styles {
    EMISSIVE, EMISSIVE_DIFFUSE
  };

  bool isHighlighted(void){return highlighted;}

  SoSFColor color;
  SoSFEnum style;
  SoSFEnum mode;

  virtual void handleEvent(SoHandleEventAction * action);
  virtual void GLRenderBelowPath(SoGLRenderAction * action);
  virtual void GLRenderInPath(SoGLRenderAction * action);
  static  void turnOffCurrentHighlight(SoGLRenderAction * action);

protected:
  virtual ~SoFCSelection();
  virtual void redrawHighlighted(SoAction * act, SbBool  flag);

private:

  static void turnoffcurrent(SoAction * action);
  void setOverride(SoGLRenderAction * action);
  static SoFullPath * currenthighlight;

//  SoFCSelectionP * pimpl;
  SbBool highlighted;
  SoColorPacker colorpacker;

};


} // namespace Gui

#endif // !COIN_SoFCSelection_H
