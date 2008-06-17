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
# include <Geom_Circle.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Edge.hxx>
#endif

#include "FeaturePartCircle.h"

using namespace Part;

PROPERTY_SOURCE(Part::Circle, Part::Feature)


Circle::Circle()
{
    ADD_PROPERTY(Angle0,(0.0f));
    ADD_PROPERTY(Angle1,(2*F_PI));
    ADD_PROPERTY(Circ,(gp_Circ()));
}

Circle::~Circle()
{
}

short Circle::mustExecute() const
{
    if (Angle0.isTouched() ||
        Angle1.isTouched() ||
        Circ.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Circle::execute(void)
{
    Handle_Geom_Circle hCircle = new Geom_Circle (Circ.getValue());
    BRepBuilderAPI_MakeEdge clMakeEdge(hCircle, this->Angle0.getValue(), this->Angle1.getValue());
    TopoDS_Edge edge = TopoDS::Edge(clMakeEdge.Shape());
    this->Shape.setValue(edge);

    return App::DocumentObject::StdReturn;
}
