/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <BRepFilletAPI_MakeFillet.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS.hxx>
#endif


#include "FeatureFillet.h"


using namespace Part;


PROPERTY_SOURCE(Part::Fillet, Part::Feature)

Fillet::Fillet()
{
    ADD_PROPERTY(Base,(0));
    ADD_PROPERTY(Contour,(0,0,0));
    Contour.setSize(0);
}

short Fillet::mustExecute() const
{
    if (Base.isTouched() || Contour.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Fillet::execute(void)
{
    App::DocumentObject* link = Base.getValue();
    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");
    if (!link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Part object");
    Part::Feature *base = static_cast<Part::Feature*>(Base.getValue());

    BRepFilletAPI_MakeFillet mkFillet(base->Shape.getValue());
    TopExp_Explorer xp(base->Shape.getValue(), TopAbs_EDGE);

    std::map<int, TopoDS_Edge> edgeMap;
    for (;xp.More();xp.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(xp.Current());
        edgeMap[edge.HashCode(IntegerLast())] = edge;
    }

    std::vector<FilletElement> values = Contour.getValues();
    for (std::vector<FilletElement>::iterator it = values.begin(); it != values.end(); ++it) {
        int id = it->hashval;
        double radius1 = it->radius1;
        double radius2 = it->radius2;
        std::map<int, TopoDS_Edge>::iterator ed = edgeMap.find(id);
        if (ed != edgeMap.end())
            mkFillet.Add(radius1, radius2, ed->second);
    }

    try {
        TopoDS_Shape shape = mkFillet.Shape();
        if (shape.IsNull())
            return new App::DocumentObjectExecReturn("Resulting shape is null");
        this->Shape.setValue(shape);
        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }
}
