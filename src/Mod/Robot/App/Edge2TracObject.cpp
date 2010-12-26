/***************************************************************************
 *   Copyright (c) 2010 Jürgen Riegel (juergen.riegel@web.de)              *
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

#include "Edge2TracObject.h"
//#include <App/DocumentObjectPy.h>
//#include <Base/Placement.h>
#include <Mod/Part/App/edgecluster.h>
#include <Mod/Part/App/PartFeature.h>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include "Waypoint.h"
#include "Trajectory.h"

using namespace Robot;
using namespace App;

PROPERTY_SOURCE(Robot::Edge2TracObject, Robot::TrajectoryObject)


Edge2TracObject::Edge2TracObject()
{

    ADD_PROPERTY_TYPE( Source,      (0)   , "Edge2Trac",Prop_None,"Edges to generate the Trajectory");
    ADD_PROPERTY_TYPE( SegValue,    (0.5), "Edge2Trac",Prop_None,"Max deviation from original geometry");

}

Edge2TracObject::~Edge2TracObject()
{
}

App::DocumentObjectExecReturn *Edge2TracObject::execute(void)
{
    App::DocumentObject* link = Source.getValue();
    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");
    if (!link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Part object");
    Part::Feature *base = static_cast<Part::Feature*>(Source.getValue());
    const Part::TopoShape& TopShape = base->Shape.getShape();

    const std::vector<std::string>& SubVals = Source.getSubValuesStartsWith("Edge");
    if (SubVals.size() == 0)
        return new App::DocumentObjectExecReturn("No Edges specified");

    // container for all the edges 
    std::vector<TopoDS_Edge> edges;

    // run throug the edge name and get the real objects from the TopoShape
    for (std::vector<std::string>::const_iterator it= SubVals.begin();it!=SubVals.end();++it) {
         TopoDS_Edge edge = TopoDS::Edge(TopShape.getSubShape(it->c_str()));
         edges.push_back(edge);
    }

    // instanciate a edge cluster sorter and get the result 
    Part::Edgecluster acluster(edges);
    Part::tEdgeClusterVector aclusteroutput = acluster.GetClusters();

    if(aclusteroutput.size() == 0)
        return new App::DocumentObjectExecReturn("No Edges specified");

    // trajectory to fill
    Robot::Trajectory trac;

    // cycle trough the cluster
    for(std::vector<std::vector<TopoDS_Edge> >::const_iterator it=aclusteroutput.begin();it!=aclusteroutput.end();++it)
    {
        // cycle through the edges of the cluster
        for(std::vector<TopoDS_Edge>::const_iterator it2=it->begin();it2!=it->end();++it2)
        {
            // get start and end point
            TopoDS_Vertex V1,V2;
            TopExp::Vertices(*it2,V1,V2);
            gp_Pnt P1 = BRep_Tool::Pnt(V1);
            gp_Pnt P2 = BRep_Tool::Pnt(V2);
            // if reverse orintation, switch the points
            if ( it2->Orientation() == TopAbs_REVERSED )
            {
                 //switch the points
                 gp_Pnt tmpP = P1;
                 P1 = P2;
                 P2 = tmpP;
            }

            Waypoint wp("Pt",Base::Placement(Base::Vector3d(P2.X(),P2.Y(),P2.Z()),Base::Rotation()));
            trac.addWaypoint(wp);

        }
    }

    Trajectory.setValue(trac);
    
    return App::DocumentObject::StdReturn;
}


//short Edge2TracObject::mustExecute(void) const
//{
//    return 0;
//}

void Edge2TracObject::onChanged(const Property* prop)
{
 
    App::GeoFeature::onChanged(prop);
}
