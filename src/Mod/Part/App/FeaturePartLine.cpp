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
# include <BRep_Builder.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <Geom_Line.hxx>
# include <gp_Pnt.hxx>
# include <gp_Lin.hxx>
# include <TopoDS_Edge.hxx>
#endif

#include "FeaturePartLine.h"

using namespace Part;

PROPERTY_SOURCE(Part::Line, Part::Feature)


Line::Line()
{
    ADD_PROPERTY(P1,(0.0f));
    ADD_PROPERTY(P2,(1.0f));
}

Line::~Line()
{
}

short Line::mustExecute() const
{
    if (P1.isTouched() || P2.isTouched())
        return 1;
    return Part::Feature::mustExecute();
}

App::DocumentObjectExecReturn *Line::execute(void)
{
    Base::Vector3f loc = this->Location.getValue();
    Base::Vector3f dir = this->Axis.getValue();

    // Convert into OCC representation
    gp_Lin line;
    try {
        line.SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
        line.SetDirection(gp_Dir(dir.x, dir.y, dir.z));
    }
    catch (const Standard_Failure&) {
        return new App::DocumentObjectExecReturn("Cannot create line");
    }

    // Create directly the underlying line geometry
    BRepBuilderAPI_MakeEdge makeEdge(line, this->P1.getValue(), this->P2.getValue());

    const char *error=0;
    switch ( makeEdge.Error() )
    {
    case BRepBuilderAPI_EdgeDone:
        break; // ok
    case BRepBuilderAPI_PointProjectionFailed:
        error = "Point projection failed";
        break;
    case BRepBuilderAPI_ParameterOutOfRange:
        error = "Parameter out of range";
        break;
    case BRepBuilderAPI_DifferentPointsOnClosedCurve:
        error = "Different points on closed curve";
        break;
    case BRepBuilderAPI_PointWithInfiniteParameter:
        error = "Point with infinite parameter";
        break;
    case BRepBuilderAPI_DifferentsPointAndParameter:
        error = "Different point and parameter";
        break;
    case BRepBuilderAPI_LineThroughIdenticPoints:
        error = "Line through identic points";
        break;
    }

    // Error 
    if (error)
        return new App::DocumentObjectExecReturn(error);

    TopoDS_Edge edge = makeEdge.Edge();
    this->Shape.setValue(edge);

    return App::DocumentObject::StdReturn;
}
