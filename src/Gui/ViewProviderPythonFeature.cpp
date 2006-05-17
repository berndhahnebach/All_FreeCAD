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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qpixmap.h>
#endif

#include "ViewProviderPythonFeature.h"
#include "Tree.h"


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderPythonFeature, Gui::ViewProviderDocumentObject)


      
ViewProviderPythonFeature::ViewProviderPythonFeature()
{
}

ViewProviderPythonFeature::~ViewProviderPythonFeature()
{
}

QPixmap ViewProviderPythonFeature::getIcon() const
{
  const char * Python_Feature_xpm[] = {
    "14 15 7 1",
    "# c #000000",
    "b c #0000ff",
    "d c #00ff00",
    "a c #808000",
    ". c None",
    "e c #ff0000",
    "c c #ffff00",
    "...###........",
    "..#aaa#.......",
    "..#b#ba##.....",
    ".##c#c#aa#....",
    "#aa#a#aaaa#...",
    "#aaaaaadaa#...",
    "#dddddd#aa#...",
    "##de#d##da#...",
    ".#ee###da#..##",
    ".#e##ddaa#.#a#",
    "..##ddaa#..#a#",
    "..#ddaa####aa#",
    "..#ddaa#aaaa#.",
    "..#ddaaaaaad#.",
    "...#ddddddd#.."};
  QPixmap px(Python_Feature_xpm);
  return px;
}
