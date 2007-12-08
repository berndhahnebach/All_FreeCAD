/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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
#endif


#include "FeaturePartCommon.h"
#include <BRepAlgoAPI_Common.hxx>

#include <Base/Exception.h>

using namespace Part;

PROPERTY_SOURCE(Part::Common, Part::Boolean)


Common::Common(void)
{
}

TopoDS_Shape Common::runOperation(TopoDS_Shape base, TopoDS_Shape tool) const
{
    // Let's call algorithm computing a section operation:
    BRepAlgoAPI_Common mkCommon(base, tool);
    // Let's check if the section has been successful
    if (!mkCommon.IsDone()) 
        throw Base::Exception("Intersection failed");
    return mkCommon.Shape();
}
