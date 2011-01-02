/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
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



#ifndef PART_PART2DOBJECT_H
#define PART_PART2DOBJECT_H

#include <App/PropertyStandard.h>

#include "PartFeature.h"

namespace Part
{


class AppPartExport Part2DObject :public Part::Feature
{
    PROPERTY_HEADER(Part::Part2DObject);

public:
    Part2DObject();

    /// if the 2DObject lies on the Face of an other object this links to it
    App::PropertyLinkSub        Support;


    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "PartGui::ViewProvider2DObject";
    }
    //@}

};

} //namespace Part


#endif // PART_PART2DOBJECT_H
