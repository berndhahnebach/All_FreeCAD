/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2010     *
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
# include <TopoDS_Compound.hxx>
#endif

#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/TimeInfo.h>
#include <Base/Console.h>

#include <Base/VectorPy.h>

#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/GeometryCurvePy.h>
#include <Mod/Part/App/LinePy.h>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>


#include "Sketch.h"
#include "Constraint.h"
#include <math.h>

#include <iostream>


using namespace Sketcher;
using namespace Base;
using namespace Part;

#define POINT_CONSTRAIN_OPTIMIZE 1

TYPESYSTEM_SOURCE(Sketcher::Sketch, Base::Persistence)

const int IntGeoOffset(0); 

Sketch::Sketch()
{
    //// add the root point at 0,0
    //addPoint(Base::Vector3d());
    //// add x,y axis
    //Part::GeomLineSegment axis;
    //axis.setPoints(Base::Vector3d(0,0,0),Base::Vector3d(100,0,0));
    //addLineSegment(axis);
    //axis.setPoints(Base::Vector3d(0,0,0),Base::Vector3d(0,100,0));
    //addLineSegment(axis);

    // set them to construction elements

}
Sketch::~Sketch()
{
    clear();
}

void Sketch::clear(void)
{
    // clear all internal data sets
    Const.clear();
    Points.clear();
    Lines.clear();
    Circles.clear();

    // deleting the doubles allocated with new
    for (std::vector<double*>::iterator it = Parameters.begin(); it != Parameters.end(); ++it)
        if (*it) delete *it;
    Parameters.clear();
    for (std::vector<double*>::iterator it = FixParameters.begin(); it != FixParameters.end(); ++it)
        if (*it) delete *it;
    FixParameters.clear();

    // deleting the geometry copied into this sketch
    for (std::vector<GeoDef>::iterator it = Geoms.begin(); it != Geoms.end(); ++it)
        if (it->geo) delete it->geo;
    Geoms.clear();
}

void Sketch::setUpSketch(const std::vector<Part::Geometry *> &geo, const std::vector<Constraint *> &ConstraintList)
{
    clear();
    int rtn = -1;


    // pass 1: first check the PointCoincidentConstraint ===============================================

    for (std::vector<Constraint *>::const_iterator it = ConstraintList.begin(); it != ConstraintList.end(); ++it) {
        switch ((*it)->Type) {
           case Coincident: addPointCoincidentConstraint((*it)->First,(*it)->FirstPos,(*it)->Second,(*it)->SecondPos,(*it)->Name.c_str()); break;
           default: break;
        }
    }

    // pass 2: building up internal geo list ===========================================================
    for (std::vector<Part::Geometry *>::const_iterator it = geo.begin(); it != geo.end(); ++it) {
        if ((*it)->getTypeId()== GeomLineSegment::getClassTypeId()) { // add a line
            const GeomLineSegment *lineSeg = dynamic_cast<const GeomLineSegment*>((*it));
            // create the definition struct for that geom
            addLineSegment(*lineSeg);
        }
        else if ((*it)->getTypeId()== GeomCircle::getClassTypeId()) {
            const GeomCircle *circle = dynamic_cast<const GeomCircle*>((*it));
            addCircle(*circle);
        }
        else {
            Base::Exception("Sketch::addGeometry(): Unknown or unsoported type added to a sketch");
        }
    }

    // pass 3 building up constraints =================================================================
    // constraints on nothing makes no sense 
    assert((int)Geoms.size() > 0 || ConstraintList.size() == 0);

    rtn = -1;
    for (std::vector<Constraint *>::const_iterator it = ConstraintList.begin(); it != ConstraintList.end(); ++it) {
        // constraints on nothing makes no sense 
        assert((int)Geoms.size() > 0);
        switch ((*it)->Type) {
           case Horizontal: addHorizontalConstraint((*it)->First,(*it)->Name.c_str()); break;
           case Vertical  : addVerticalConstraint((*it)->First,(*it)->Name.c_str());   break;
           case Parallel  : addParallelConstraint((*it)->First,(*it)->Second,(*it)->Name.c_str());  break;
           case Distance  :
               if ((*it)->Second == -1)
                    rtn = addDistanceConstraint((*it)->First,(*it)->Value, (*it)->Name.c_str());
               else
                   rtn = addDistanceConstraint((*it)->First,(*it)->Second,(*it)->Value, (*it)->Name.c_str());
               break;
           case Angle: break;
           case None :  break;
           case Coincident: break;
        }
    }

    // clear the optimize data structure
    PoPMap.clear();
}

// Geometry adding ==========================================================

int Sketch::addGeometry(const Part::Geometry *geo)
{
    if (geo->getTypeId()== GeomLineSegment::getClassTypeId()) { // add a line
        const GeomLineSegment *lineSeg = dynamic_cast<const GeomLineSegment*>(geo);
        // create the definition struct for that geom
        return addLineSegment(*lineSeg);
    } else {
        Base::Exception("Sketch::addGeometry(): Unknown or unsoported type added to a sketch");
        return 0; 
    }
}

void Sketch::addGeometry(const std::vector<Part::Geometry *> &geo)
{
    for (std::vector<Part::Geometry *>::const_iterator it = geo.begin();it!=geo.end();++it)
        addGeometry(*it);
}

int Sketch::addPoint(Base::Vector3d newPoint)
{
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = 0;
    def.type = Point;
    def.construction = false;

    // set the parameter for the solver
    def.parameterStartIndex = Parameters.size();
    Parameters.push_back(new double(newPoint.x));
    Parameters.push_back(new double(newPoint.y));
 
    // set the points for later constraints
    point p1;
    p1.x = Parameters[def.parameterStartIndex+0];
    p1.y = Parameters[def.parameterStartIndex+1];
    def.pointStartIndex = Points.size();
    Points.push_back(p1);

    // store complete set
    Geoms.push_back(def);
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addLine(const Part::GeomLineSegment &line)
{

    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addLineSegment(const Part::GeomLineSegment &lineSegment)
{
    // create our own copy
    GeomLineSegment *lineSeg = new GeomLineSegment(lineSegment);
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = lineSeg;
    def.type = Line;
    def.construction = false;

    // get the points from the line
    Base::Vector3d start = lineSeg->getStartPoint();
    Base::Vector3d end   = lineSeg->getEndPoint();

    // the points for later constraints
    point p1, p2;
    def.parameterStartIndex = Parameters.size();

    // check if for the start point is already a constraint point present
    if (PoPMap[Geoms.size()].StartPointIndex != -1) {
        // if yes, use the coincident point
        p1.x = Parameters[PoPMap[Geoms.size()].StartPointIndex+0];
        p1.y = Parameters[PoPMap[Geoms.size()].StartPointIndex+1];
        // set the values
        *(Parameters[PoPMap[Geoms.size()].StartPointIndex+0]) = start.x;
        *(Parameters[PoPMap[Geoms.size()].StartPointIndex+1]) = start.y;
    } else {
        // otherwise set the parameter for the solver
        Parameters.push_back(new double(start.x));
        Parameters.push_back(new double(start.y));
        // set the points for later constraints
        p1.x = Parameters[Parameters.size()-2];
        p1.y = Parameters[Parameters.size()-1];
    }

    // check if for the end point is already a constraint point present
    if (PoPMap[Geoms.size()].EndPointIndex != -1) {
        // if yes, use the coincident point
        p2.x = Parameters[PoPMap[Geoms.size()].EndPointIndex+0];
        p2.y = Parameters[PoPMap[Geoms.size()].EndPointIndex+1];
        // set the values
        *(Parameters[PoPMap[Geoms.size()].EndPointIndex+0]) = end.x;
        *(Parameters[PoPMap[Geoms.size()].EndPointIndex+1]) = end.y;
    } else {
        Parameters.push_back(new double(end.x));
        Parameters.push_back(new double(end.y));
        // set the points for later constraints
        p2.x = Parameters[Parameters.size()-2];
        p2.y = Parameters[Parameters.size()-1];
    }

    // add the points
    def.pointStartIndex = Points.size();
    Points.push_back(p1);
    Points.push_back(p2);

    // set the line for later constraints
    line l;
    l.p1 = p1;
    l.p2 = p2;
    def.lineStartIndex = Lines.size();
    Lines.push_back(l);

    // store complete set
    Geoms.push_back(def);
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addArc(const Part::GeomTrimmedCurve &circleSegment)
{

    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addCircle(const GeomCircle &cir)
{
    // create our own copy
    GeomCircle *circ = new GeomCircle(cir);
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = circ;
    def.type = Circle;
    def.construction = false;

    // get the point from the line
    Base::Vector3d center = circ->getCenter();
    double radius         = circ->getRadius();
 
    // set the parameter for the solver
    def.parameterStartIndex = Parameters.size();
    Parameters.push_back(new double(center.x));
    Parameters.push_back(new double(center.y));
    Parameters.push_back(new double(radius));

    // set the points for later constraints
    point p1;
    p1.x = Parameters[def.parameterStartIndex+0];
    p1.y = Parameters[def.parameterStartIndex+1];
    def.pointStartIndex = Points.size();
    Points.push_back(p1);

    // add the radius parameter
    double *r = Parameters[def.parameterStartIndex+2];

    // set the circel for later constraints
    circle c;
    c.center = p1;
    c.rad    = r;
    def.circleStartIndex = Circles.size();
    Circles.push_back(c);

    // store complete set
    Geoms.push_back(def);
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addEllipse(const Part::GeomEllipse &ellibse)
{
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

std::vector<Part::Geometry *> Sketch::getGeometry(bool withConstrucionElements) const
{
    std::vector<Part::Geometry *> temp(Geoms.size()-IntGeoOffset);
    int i=0;
    std::vector<GeoDef>::const_iterator it=Geoms.begin();
    // skip the default elements
    it += IntGeoOffset;
         
    for (;it!=Geoms.end();++it,i++)
        if (!it->construction || withConstrucionElements)
          temp[i] = it->geo->clone();

    return temp;
}

Py::Tuple Sketch::getPyGeometry(void) const
{
    Py::Tuple tuple(Geoms.size());
    int i=0;
    std::vector<GeoDef>::const_iterator it=Geoms.begin();
    // skip the default elements
    it += IntGeoOffset;

    for (;it!=Geoms.end();++it,i++) {
        if (it->type == Line) {
            GeomLineSegment *lineSeg = dynamic_cast<GeomLineSegment*>(it->geo);
            tuple[i] = Py::Object(new LinePy(lineSeg));
        } else if (it->type == Point) {
            Base::Vector3d temp(*(Points[Geoms[i].pointStartIndex].x),*(Points[Geoms[i].pointStartIndex].y),0);
            tuple[i] = Py::Object(new VectorPy(temp));
        } else {
            assert(0); // not implemented type in the sketch!
        }
    }
    return tuple;
}

void Sketch::setConstruction(int geoIndex,bool isConstruction)
{
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());

    Geoms[geoIndex].construction = isConstruction;
}

bool  Sketch::getConstruction(int geoIndex) const
{
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());

    return Geoms[geoIndex].construction;
}

// constraint adding ==========================================================

int Sketch::addConstraint(const Constraint *constraint)
{
    // constraints on nothing makes no sense 
    assert((int)Geoms.size()>0  );
    int rtn = -1;
    switch (constraint->Type) {
       case Horizontal:
           rtn = addHorizontalConstraint(constraint->First,constraint->Name.c_str());
           break;
       case Vertical:
           rtn = addVerticalConstraint(constraint->First,constraint->Name.c_str());
           break;
       case Coincident:
           rtn = addPointCoincidentConstraint(constraint->First,constraint->FirstPos,constraint->Second,constraint->SecondPos,constraint->Name.c_str());
           break;
       case Parallel:
           rtn = addParallelConstraint(constraint->First,constraint->Second,constraint->Name.c_str());
           break;
       case Distance:
           if (constraint->Second == -1)
                rtn = addDistanceConstraint(constraint->First,constraint->Value, constraint->Name.c_str());
           else
               rtn = addDistanceConstraint(constraint->First,constraint->Second,constraint->Value, constraint->Name.c_str());
           break;
       case Angle:
           break;
       case None:
           break;
    }

    return rtn;
}

int Sketch::addConstraints(const std::vector<Constraint *> &ConstraintList)
{
    // constraints on nothing makes no sense 
    assert((int)Geoms.size()>0 || ConstraintList.size() == 0 );

    int rtn = -1;
    for (std::vector<Constraint *>::const_iterator it = ConstraintList.begin();it!=ConstraintList.end();++it)
        rtn = addConstraint (*it);

    return rtn;
}

int Sketch::addHorizontalConstraint(int geoIndex, const char* name)
{
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex].type == Line);

    ConstrainDef constrain;
    constrain.constrain.type = horizontal;
    constrain.constrain.line1 = Lines[Geoms[geoIndex].lineStartIndex];
    if (name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}

int Sketch::addVerticalConstraint(int geoIndex, const char* name)
{
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex].type == Line);

    // create the constraint and fill it up
    ConstrainDef constrain;
    constrain.constrain.type = vertical;
    constrain.constrain.line1 = Lines[Geoms[geoIndex].lineStartIndex];
    if (name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}

int Sketch::addPointCoincidentConstraint(int geoIndex1, PointPos Pos1, int geoIndex2, PointPos Pos2, const char* name)
{
    // this optimization alters point on point constraints for e.g Line segments
    // to one point. That means the Lines segments get altered to a polyline. 

    // create one points for the constraint
    int parameterStartIndex = Parameters.size();
    Parameters.push_back(new double(0));
    Parameters.push_back(new double(0));

    // save the index belonging to the geo id for later usage in build up geo
    switch(Pos1) {
        case start: PoPMap[geoIndex1].StartPointIndex = parameterStartIndex; break;
        case end  : PoPMap[geoIndex1].EndPointIndex = parameterStartIndex; break;
        case mid  : PoPMap[geoIndex1].MidPointIndex = parameterStartIndex; break;
        case none : break;
    }
    switch(Pos2) {
        case start: PoPMap[geoIndex2].StartPointIndex = parameterStartIndex; break;
        case end  : PoPMap[geoIndex2].EndPointIndex = parameterStartIndex; break;
        case mid  : PoPMap[geoIndex2].MidPointIndex = parameterStartIndex; break;
        case none : break;
    }

    return Const.size()-1;
}

int Sketch::addParallelConstraint(int geoIndex1,int geoIndex2, const char* name)
{   
    // index out of bounds?
    assert(geoIndex1 < (int)Geoms.size());
    assert(geoIndex2 < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex1].type == Line );
    assert(Geoms[geoIndex2].type == Line );

    // creat the constraint and fill it up
    ConstrainDef constrain;
    constrain.constrain.type = parallel;
    constrain.constrain.line1 = Lines[Geoms[geoIndex1].lineStartIndex];
    constrain.constrain.line2 = Lines[Geoms[geoIndex2].lineStartIndex];

    if (name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}

int Sketch::addDistanceConstraint(int geoIndex1, double Value, const char* name)
{   
    // index out of bounds?
    assert(geoIndex1 < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex1].type == Line );

    // creat the constraint and fill it up
    ConstrainDef constrain;
    constrain.constrain.type = P2PDistance;
    constrain.constrain.point1 = Points[Geoms[geoIndex1].pointStartIndex];
    constrain.constrain.point2 = Points[Geoms[geoIndex1].pointStartIndex+1];

    // add the parameter for the length
    FixParameters.push_back(new double(Value));
    constrain.constrain.parameter = FixParameters[FixParameters.size()-1];

    if (name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}

int Sketch::addDistanceConstraint(int geoIndex1, int geoIndex2, double Value, const char* name)
{   
    // index out of bounds?
    assert(geoIndex1 < (int)Geoms.size());
    assert(geoIndex2 < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex1].type == Line );
    assert(Geoms[geoIndex2].type == Line );

    // creat the constraint and fill it up
    ConstrainDef constrain;
    constrain.constrain.type = P2LDistance;
    constrain.constrain.line1 = Lines[Geoms[geoIndex1].lineStartIndex];
    constrain.constrain.point1 = Points[Geoms[geoIndex2].pointStartIndex];

    // add the parameter for the length
    Parameters.push_back(new double(Value));
    constrain.constrain.parameter = Parameters[Parameters.size()-1];

    if (name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}


// solving ==========================================================

void _redirectPoint(point &c, double &fixedValue1, double &fixedValue2, double * fixed[2])
{
    if (fixed[0] && c.x == fixed[0]) {
        // copy the fixes values
        //fixedValue1 = *(c.x);
        //fixedValue2 = *(c.y);
        // point the constraint to the fixed values
        c.x = &(fixedValue1);
        c.y = &(fixedValue2);
    }
}

int Sketch::solve(double * fixed[2]) {

    Base::TimeInfo start_time;
    //Base::Console().Log("solv: Start solving (C:%d;G%d) ",Const.size(),Geoms.size());
    Solver s;
    double fixedValue1=0,fixedValue2=0;

    if (fixed[0]) {
        fixedValue1 = *(fixed[0]);
        fixedValue2 = *(fixed[1]);
    }

    // copy the constraints and handling the fixed constraint ################
    std::vector<constraint> constraints(Const.size());
    int i=0;
    for (std::vector<ConstrainDef>::iterator it=Const.begin();it!=Const.end();++it,i++) {
        constraints[i] = it->constrain;
        if (fixed[0]) {
            // exchange the fixed points in the constraints
            _redirectPoint(constraints[i].point1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].point1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line1.p1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line1.p2,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line2.p1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line2.p2,fixedValue1,fixedValue2,fixed);
        }
    }

    // solving with solvesketch ############################################
    int ret = s.solve(&Parameters[0],Parameters.size(),&constraints[0],Const.size(),0);

    // set back the fixed parameters no matter what the solver did with it
    if (fixed[0]) {
        *(fixed[0]) = fixedValue1;
        *(fixed[1]) = fixedValue2;
    }

    // if successfully solve write the parameter back
    if (ret == 0) {
        int i=0;
        for (std::vector<GeoDef>::const_iterator it=Geoms.begin();it!=Geoms.end();++it,i++) {
            try {
                if (it->type == Line) {
                    GeomLineSegment *lineSeg = dynamic_cast<GeomLineSegment*>(it->geo);
                    lineSeg->setPoints(
                             Vector3d(
                                 *Points[it->pointStartIndex+0].x,
                                 *Points[it->pointStartIndex+0].y,
                                 0.0),
                             Vector3d(
                                 *Points[it->pointStartIndex+1].x,
                                 *Points[it->pointStartIndex+1].y,
                                 0.0)
                         );
                }
            } catch (Base::Exception e) {
                Base::Console().Error("Solv: Error build geometry(%d): %s\n",i,e.what());
                return -1;
            }

        }
    }else{
        //Base::Console().Log("NotSolved ");
    }
    Base::TimeInfo end_time;
    //Base::Console().Log("T:%s\n",Base::TimeInfo::diffTime(start_time,end_time).c_str());
    SolveTime = Base::TimeInfo::diffTimeF(start_time,end_time);
    return ret;
}

int Sketch::solve(void) 
{
    double * fixed[2]={0,0};
    return solve(fixed);
}

int Sketch::movePoint(int geoIndex1, PointPos Pos1, Base::Vector3d toPoint)
{
    // list of the two fixed parameters (point)
    double * fixed[2]={0,0};

    if (Pos1 == start) {
        *(Points[Geoms[geoIndex1].pointStartIndex].x) = toPoint.x;
        *(Points[Geoms[geoIndex1].pointStartIndex].y) = toPoint.y;
        fixed [0] = Points[Geoms[geoIndex1].pointStartIndex].x;
        fixed [1] = Points[Geoms[geoIndex1].pointStartIndex].y;
    } else {
        *(Points[Geoms[geoIndex1].pointStartIndex+1].x) = toPoint.x;
        *(Points[Geoms[geoIndex1].pointStartIndex+1].y) = toPoint.y;
        fixed [0] = Points[Geoms[geoIndex1].pointStartIndex+1].x;
        fixed [1] = Points[Geoms[geoIndex1].pointStartIndex+1].y;
    }

    return solve(fixed);
}

TopoShape Sketch::toShape(void) const
{
    TopoShape result;
    std::vector<GeoDef>::const_iterator it=Geoms.begin();

    // skip the default elements
    it += IntGeoOffset;
#if 0

    bool first = true;
    for (;it!=Geoms.end();++it) {
        if (!it->construction) {
            TopoDS_Shape sh = it->geo->toShape();
            if (first) {
                first = false;
                result._Shape = sh;
            } else {
                result._Shape = result.fuse(sh);
            }
        }
    }
    return result;
#else
    std::list<TopoDS_Edge> edge_list;
    std::list<TopoDS_Wire> wires;

    // collecting all (non constructive) edges out of the sketch
    for (;it!=Geoms.end();++it) {
        if (!it->construction) {
            edge_list.push_back(TopoDS::Edge(it->geo->toShape()));
        }
    }

    // sort them together to wires
    while (edge_list.size() > 0) {
        BRepBuilderAPI_MakeWire mkWire;
        // add and erase first edge
        mkWire.Add(edge_list.front());
        edge_list.erase(edge_list.begin());

        TopoDS_Wire new_wire = mkWire.Wire(); // current new wire

        // try to connect each edge to the wire, the wire is complete if no more egdes are connectible
        bool found = false;
        do {
            found = false;
            for (std::list<TopoDS_Edge>::iterator pE = edge_list.begin(); pE != edge_list.end(); ++pE) {
                mkWire.Add(*pE);
                if (mkWire.Error() != BRepBuilderAPI_DisconnectedWire) {
                    // edge added ==> remove it from list
                    found = true;
                    edge_list.erase(pE);
                    new_wire = mkWire.Wire();
                    break;
                }
            }
        }
        while (found);
        wires.push_back(new_wire);
    }
    if (wires.size() == 1)
        result = *wires.begin();
    else if (wires.size() > 1) {
        // FIXME: The right way here would be to determine the outer and inner wires and 
        // generate a face with holes (inner wires have to be taged REVERSE or INNER).
        // thats the only way to transport a somwhat more complex sketch...
        //result = *wires.begin();
        
        // I think a compound can be used as container because it is just a collection of
        // shapes and doesn't need too much information about the topology.
        // The actual knowledge how to create a prism from several wires should go to the Pad
        // feature (Werner).
        BRep_Builder builder;
        TopoDS_Compound comp;
        builder.MakeCompound(comp);
        for (std::list<TopoDS_Wire>::iterator wt = wires.begin(); wt != wires.end(); ++wt)
            builder.Add(comp, *wt);
        result._Shape = comp;
    }
    // FIXME: if free edges are left over its probably better to 
    // create a compound with the closed structures and let the 
    // features decide what to do with it...
    if (edge_list.size() > 0)
        Base::Console().Warning("Left offer edges in Sketch. Only closed structures will be propagated at the moment!");

#endif

    return result;
}

void Sketch::getGeoVertexIndex(int VertexId,int &GeoId,int &PointPos)
{
    PointPos = VertexId%2 + 1;
    GeoId = (int)VertexId/2;
}

// Persistance implementer -------------------------------------------------

unsigned int Sketch::getMemSize(void) const
{
    return 0;
}

void Sketch::Save(Writer &writer) const
{

}

void Sketch::Restore(XMLReader &reader)
{
 
}

