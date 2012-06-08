/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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


#ifndef PART_FEATUREPARTCOMMON_H
#define PART_FEATUREPARTCOMMON_H

#include <App/PropertyLinks.h>
#include "FeaturePartBoolean.h"

namespace Part
{

class Common : public Boolean
{
    PROPERTY_HEADER(Part::Common);

public:
    Common();

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
protected:
    BRepAlgoAPI_BooleanOperation* makeOperation(const TopoDS_Shape&, const TopoDS_Shape&) const;
    //@}
};

class MultiCommon : public Part::Feature
{
    PROPERTY_HEADER(Part::MultiCommon);

public:
    MultiCommon();

    App::PropertyLinkList Shapes;
    PropertyShapeHistory History;

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "PartGui::ViewProviderMultiCommon";
    }

};

}

#endif // PART_FEATUREPARTCOMMON_H
