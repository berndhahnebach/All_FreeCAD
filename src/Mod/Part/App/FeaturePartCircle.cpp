/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <gp_Circ.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Edge.hxx>
#endif

#include "FeaturePartCircle.h"

using namespace Part;

PROPERTY_SOURCE(Part::Circle, Part::Feature)


Circle::Circle()
{
    ADD_PROPERTY(Radius,(2.0f));
    ADD_PROPERTY(Angle1,(0.0f));
    ADD_PROPERTY(Angle2,(2*F_PI));
}

Circle::~Circle()
{
}

short Circle::mustExecute() const
{
    if (Angle1.isTouched() ||
        Angle2.isTouched() ||
        Radius.isTouched())
        return 1;
    return Part::Feature::mustExecute();
}

App::DocumentObjectExecReturn *Circle::execute(void)
{
    Base::Vector3f loc = this->Location.getValue();
    Base::Vector3f dir = this->Axis.getValue();

    gp_Ax1 axis;
    axis.SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    axis.SetDirection(gp_Dir(dir.x, dir.y, dir.z));
    gp_Circ circle;
    circle.SetAxis(axis);
    circle.SetRadius(this->Radius.getValue());
    
    BRepBuilderAPI_MakeEdge clMakeEdge(circle, this->Angle1.getValue(), this->Angle2.getValue());
    TopoDS_Edge edge = TopoDS::Edge(clMakeEdge.Shape());
    this->Shape.setValue(edge);

    return App::DocumentObject::StdReturn;
}
