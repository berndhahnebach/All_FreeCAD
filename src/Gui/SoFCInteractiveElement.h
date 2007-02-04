/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_SOFCINTERACTIVEELEMENT_H
#define GUI_SOFCINTERACTIVEELEMENT_H

#include <Inventor/elements/SoReplacedElement.h>


namespace Gui {
/**
 * @author Werner Mayer
 */
class GuiExport SoFCInteractiveElement : public SoReplacedElement {
  typedef SoReplacedElement inherited;

  SO_ELEMENT_HEADER(SoFCInteractiveElement);

public:
  static void initClass(void);

  virtual void init(SoState * state);
  static void set(SoState * const state, SoNode * const node, SbBool mode);
  static SbBool get(SoState * const state);
  static const SoFCInteractiveElement * getInstance(SoState * state);

protected:
  virtual ~SoFCInteractiveElement();
  virtual void setElt(SbBool mode);

private:
  SbBool interactiveMode;
};

} // namespace Gui

#endif // GUI_SOFCINTERACTIVEELEMENT_H
