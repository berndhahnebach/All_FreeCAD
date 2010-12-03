/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#ifndef GUI_UTILITIES_H
#define GUI_UTILITIES_H

#include <Base/ViewProj.h>
#include <Inventor/SbViewVolume.h>

class SbViewVolume;

namespace Gui {

/**
 */
class GuiExport ViewVolumeProjection : public Base::ViewProjMethod
{
public:
    ViewVolumeProjection (const SbViewVolume &vv);
    virtual ~ViewVolumeProjection(){};

    Base::Vector3f operator()(const Base::Vector3f &rclPt) const;
    Base::Vector3f inverse (const Base::Vector3f &rclPt) const;

    Base::Matrix4D getProjectionMatrix () const;

protected:
    SbViewVolume viewVolume;
};

} // namespace Gui

#endif // GUI_UTILITIES_H
