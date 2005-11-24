/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef COIN_SOFCBACKGROUNDGRADIENT_H
#define COIN_SOFCBACKGROUNDGRADIENT_H

#include <Inventor/nodes/SoSeparator.h>

class SoCoordinate3;
class SoMFString;
class SbVec2s;

namespace Gui {

class GuiExport SoFCBackgroundGradient : public SoSeparator {
  typedef SoSeparator inherited;

  SO_NODE_HEADER(Gui::SoFCBackgroundGradient);

public:
  static void initClass(void);
  SoFCBackgroundGradient(void);

  void setViewerSize( const SbVec2s& size );

protected:
  virtual ~SoFCBackgroundGradient();

  SoCoordinate3* coords;
};

} // namespace Gui


#endif // COIN_SOFCBACKGROUNDGRADIENT_H

