/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <BRepPrimAPI_MakeBox.hxx>
# include <Precision.hxx>
#endif


#include <Base/Console.h>
#include "FeaturePartBox.h"


using namespace Part;


PROPERTY_SOURCE(Part::Box, Part::Primitive)


Box::Box()
{
    ADD_PROPERTY(Length,(100.0));
    ADD_PROPERTY(Height,(100.0));
    ADD_PROPERTY(Width,(100.0));
}

short Box::mustExecute() const
{
    if (Length.isTouched() ||
        Height.isTouched() ||
        Width.isTouched() )
        return 1;
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Box::execute(void)
{
    double L = Length.getValue();
    double H = Height.getValue();
    double W = Width.getValue();

    if (L < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Length of L too small");

    if (H < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Height of H too small");

    if (W < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Width of W too small");

    try {
        // Build a box using the dimension and position attributes
        gp_Pnt pnt(Location.getValue().x,
                   Location.getValue().y,
                   Location.getValue().z);
        gp_Dir dir(Axis.getValue().x,
                   Axis.getValue().y,
                   Axis.getValue().z);
        BRepPrimAPI_MakeBox mkBox(gp_Ax2(pnt,dir), L, H, W);
        TopoDS_Shape ResultShape = mkBox.Shape();
        this->Shape.setValue(ResultShape);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}
