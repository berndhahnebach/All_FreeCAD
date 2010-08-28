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


#include "PreCompiled.h"
#ifndef _PreComp_
# include <gp.hxx>
# include <gp_Trsf.hxx>
# include <BRepBuilderAPI_Transform.hxx>
#endif


#include "FeatureMirroring.h"


using namespace Part;

const char* Mirroring::PlaneEnums[]= {"XY","XZ","YZ",NULL};

PROPERTY_SOURCE(Part::Mirroring, Part::Feature)

Mirroring::Mirroring()
{
    ADD_PROPERTY(Base,(0));
    ADD_PROPERTY(Plane,((long)0));
    Plane.setEnums(PlaneEnums);
}

short Mirroring::mustExecute() const
{
    if (Base.isTouched() ||
        Plane.isTouched() )
        return 1;
    return 0;
}

Base::Matrix4D Mirroring::getTransformation() const
{
    return Base::Matrix4D();
}

App::DocumentObjectExecReturn *Mirroring::execute(void)
{
    App::DocumentObject* link = Base.getValue();
    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");
    if (!link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Part object");
    Part::Feature *base = static_cast<Part::Feature*>(Base.getValue());
    long planeenum = this->Plane.getValue();

    try {
        gp_Trsf mat;
        if (planeenum == 0)
            mat.SetMirror(gp::XOY());
        else if (planeenum == 1)
            mat.SetMirror(gp::ZOX());
        else
            mat.SetMirror(gp::YOZ());
        const TopoDS_Shape& shape = base->Shape.getValue();
        TopLoc_Location loc = shape.Location();
        gp_Trsf placement = loc.Transformation();
        mat = placement * mat;
        BRepBuilderAPI_Transform mkTrf(shape, mat);
        this->Shape.setValue(mkTrf.Shape());
        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }
}
