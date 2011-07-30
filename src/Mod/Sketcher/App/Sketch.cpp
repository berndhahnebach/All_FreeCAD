/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2010     *
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
#include <Mod/Part/App/ArcOfCirclePy.h>
#include <Mod/Part/App/CirclePy.h>
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

TYPESYSTEM_SOURCE(Sketcher::Sketch, Base::Persistence)

const int IntGeoOffset(0); 

Sketch::Sketch()
: GCSsys(), isInitMove(false)
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
    Points.clear();
    Lines.clear();
    Arcs.clear();
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

    GCSsys.clear();
    isInitMove = false;
}

void Sketch::setUpSketch(const std::vector<Part::Geometry *> &geo, const std::vector<Constraint *> &ConstraintList)
{
    clear();

    for (std::vector<Part::Geometry *>::const_iterator it = geo.begin(); it != geo.end(); ++it) {
        if ((*it)->getTypeId()== GeomLineSegment::getClassTypeId()) { // add a line
            const GeomLineSegment *lineSeg = dynamic_cast<const GeomLineSegment*>((*it));
            // create the definition struct for that geom
            addLineSegment(*lineSeg);
        } else if ((*it)->getTypeId()== GeomCircle::getClassTypeId()) {
            const GeomCircle *circle = dynamic_cast<const GeomCircle*>((*it));
            addCircle(*circle);
        } else if ((*it)->getTypeId()== GeomArcOfCircle::getClassTypeId()) {
            const GeomArcOfCircle *aoc = dynamic_cast<const GeomArcOfCircle*>((*it));
            addArc(*aoc);
        } else {
            Base::Exception("Sketch::setUpSketch(): Unknown or unsupported type added to a sketch");
        }
    }

    addConstraints(ConstraintList);

    GCSsys.clearLevel(-1);
    GCSsys.initSolution(Parameters);

}

// Geometry adding ==========================================================

int Sketch::addGeometry(const Part::Geometry *geo)
{
    if (geo->getTypeId()== GeomLineSegment::getClassTypeId()) { // add a line
        const GeomLineSegment *lineSeg = dynamic_cast<const GeomLineSegment*>(geo);
        // create the definition struct for that geom
        return addLineSegment(*lineSeg);
    } else if (geo->getTypeId()== GeomCircle::getClassTypeId()) { // add a circle
        const GeomCircle *circle = dynamic_cast<const GeomCircle*>(geo);
        // create the definition struct for that geom
        return addCircle(*circle);
    } else if (geo->getTypeId()== GeomArcOfCircle::getClassTypeId()) { // add an arc
        const GeomArcOfCircle *aoc = dynamic_cast<const GeomArcOfCircle*>(geo);
        // create the definition struct for that geom
        return addArc(*aoc);
    } else {
        Base::Exception("Sketch::addGeometry(): Unknown or unsupported type added to a sketch");
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
    int paramStartIndex = Parameters.size();
    Parameters.push_back(new double(newPoint.x));
    Parameters.push_back(new double(newPoint.y));
 
    // set the points for later constraints
    GCS::Point p1;
    p1.x = Parameters[paramStartIndex+0];
    p1.y = Parameters[paramStartIndex+1];
    def.startPointId = Points.size();
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
    GeomLineSegment *lineSeg = static_cast<GeomLineSegment*>(lineSegment.clone());
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = lineSeg;
    def.type = Line;
    def.construction = false;

    // get the points from the line
    Base::Vector3d start = lineSeg->getStartPoint();
    Base::Vector3d end   = lineSeg->getEndPoint();

    // the points for later constraints
    GCS::Point p1, p2;

    Parameters.push_back(new double(start.x));
    Parameters.push_back(new double(start.y));
    p1.x = Parameters[Parameters.size()-2];
    p1.y = Parameters[Parameters.size()-1];

    Parameters.push_back(new double(end.x));
    Parameters.push_back(new double(end.y));
    p2.x = Parameters[Parameters.size()-2];
    p2.y = Parameters[Parameters.size()-1];

    // add the points
    def.startPointId = Points.size();
    def.endPointId = Points.size()+1;
    Points.push_back(p1);
    Points.push_back(p2);

    // set the line for later constraints
    GCS::Line l;
    l.p1 = p1;
    l.p2 = p2;
    def.index = Lines.size();
    Lines.push_back(l);

    // store complete set
    Geoms.push_back(def);
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addArc(const Part::GeomArcOfCircle &circleSegment)
{
    // create our own copy
    GeomArcOfCircle *aoc = static_cast<GeomArcOfCircle*>(circleSegment.clone());
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = aoc;
    def.type = Arc;
    def.construction = false;

    Base::Vector3d center   = aoc->getCenter();
    Base::Vector3d startPnt = aoc->getStartPoint();
    Base::Vector3d endPnt   = aoc->getEndPoint();
    double radius           = aoc->getRadius();
    double startAngle, endAngle;
    aoc->getRange(startAngle, endAngle);
 
    GCS::Point p1, p2, p3;

    Parameters.push_back(new double(startPnt.x));
    Parameters.push_back(new double(startPnt.y));
    p1.x = Parameters[Parameters.size()-2];
    p1.y = Parameters[Parameters.size()-1];

    Parameters.push_back(new double(endPnt.x));
    Parameters.push_back(new double(endPnt.y));
    p2.x = Parameters[Parameters.size()-2];
    p2.y = Parameters[Parameters.size()-1];

    Parameters.push_back(new double(center.x));
    Parameters.push_back(new double(center.y));
    p3.x = Parameters[Parameters.size()-2];
    p3.y = Parameters[Parameters.size()-1];

    def.startPointId = Points.size();
    Points.push_back(p1);
    def.endPointId = Points.size();
    Points.push_back(p2);
    def.midPointId = Points.size();
    Points.push_back(p3);

    Parameters.push_back(new double(radius));
    double *r = Parameters[Parameters.size()-1];
    Parameters.push_back(new double(startAngle));
    double *a1 = Parameters[Parameters.size()-1];
    Parameters.push_back(new double(endAngle));
    double *a2 = Parameters[Parameters.size()-1];

    // set the arc for later constraints
    GCS::Arc a;
    a.start      = p1;
    a.end        = p2;
    a.center     = p3;
    a.rad        = r;
    a.startAngle = a1;
    a.endAngle   = a2;
    def.index = Arcs.size();
    Arcs.push_back(a);

    // store complete set
    Geoms.push_back(def);
    
    // arcs require an ArcRules constraint for the end points
    GCSsys.addConstraintArcRules(a);

    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addCircle(const GeomCircle &cir)
{
    // create our own copy
    GeomCircle *circ = static_cast<GeomCircle*>(cir.clone());
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = circ;
    def.type = Circle;
    def.construction = false;

    Base::Vector3d center = circ->getCenter();
    double radius         = circ->getRadius();
 
    GCS::Point p1;

    Parameters.push_back(new double(center.x));
    Parameters.push_back(new double(center.y));
    p1.x = Parameters[Parameters.size()-2];
    p1.y = Parameters[Parameters.size()-1];

    Parameters.push_back(new double(radius));

    def.midPointId = Points.size();
    Points.push_back(p1);

    // add the radius parameter
    double *r = Parameters[Parameters.size()-1];

    // set the circle for later constraints
    GCS::Circle c;
    c.center = p1;
    c.rad    = r;
    def.index = Circles.size();
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
        } else if (it->type == Arc) {
            GeomArcOfCircle *aoc = dynamic_cast<GeomArcOfCircle*>(it->geo);
            tuple[i] = Py::Object(new ArcOfCirclePy(aoc));
        } else if (it->type == Circle) {
            GeomCircle *circle = dynamic_cast<GeomCircle*>(it->geo);
            tuple[i] = Py::Object(new CirclePy(circle));
        } else if (it->type == Point) {
            Base::Vector3d temp(*(Points[Geoms[i].startPointId].x),*(Points[Geoms[i].startPointId].y),0);
            tuple[i] = Py::Object(new VectorPy(temp));
        } else {
            assert(0); // not implemented type in the sketch!
        }
    }
    return tuple;
}

void Sketch::setConstruction(int geoId, bool isConstruction)
{
    assert(geoId < int(Geoms.size()));

    Geoms[geoId].construction = isConstruction;
}

bool Sketch::getConstruction(int geoId) const
{
    assert(geoId < int(Geoms.size()));

    return Geoms[geoId].construction;
}

// constraint adding ==========================================================

int Sketch::addConstraint(const Constraint *constraint)
{
    // constraints on nothing makes no sense 
    assert(int(Geoms.size()) > 0);
    int rtn = -1;
    switch (constraint->Type) {
        case DistanceX:
            if (constraint->FirstPos == none) // horizontal length of a line
                rtn = addDistanceXConstraint(constraint->First,constraint->Value);
            else if (constraint->Second == -1) // point on fixed x-coordinate
                rtn = addCoordinateXConstraint(constraint->First,constraint->FirstPos,constraint->Value);
            else if (constraint->SecondPos != none) // point to point horizontal distance
                rtn = addDistanceXConstraint(constraint->First,constraint->FirstPos,
                                             constraint->Second,constraint->SecondPos,constraint->Value);
            break;
        case DistanceY:
            if (constraint->FirstPos == none) // vertical length of a line
                rtn = addDistanceYConstraint(constraint->First,constraint->Value);
            else if (constraint->Second == -1) // point on fixed y-coordinate
                rtn = addCoordinateYConstraint(constraint->First,constraint->FirstPos,constraint->Value);
            else if (constraint->SecondPos != none) // point to point vertical distance
                rtn = addDistanceYConstraint(constraint->First,constraint->FirstPos,
                                             constraint->Second,constraint->SecondPos,constraint->Value);
            break;
        case Horizontal:
            if (constraint->Second == -1) // horizontal line
                rtn = addHorizontalConstraint(constraint->First);
            else // two points on the same horizontal line
                rtn = addHorizontalConstraint(constraint->First,constraint->FirstPos,
                                              constraint->Second,constraint->SecondPos);
            break;
        case Vertical:
            if (constraint->Second == -1) // vertical line
                rtn = addVerticalConstraint(constraint->First);
            else // two points on the same vertical line
                rtn = addVerticalConstraint(constraint->First,constraint->FirstPos,
                                            constraint->Second,constraint->SecondPos);
            break;
        case Coincident:
            rtn = addPointCoincidentConstraint(constraint->First,constraint->FirstPos,constraint->Second,constraint->SecondPos);
            break;
        case Parallel:
            rtn = addParallelConstraint(constraint->First,constraint->Second);
            break;
        case Perpendicular:
            rtn = addPerpendicularConstraint(constraint->First,constraint->Second);
            break;
        case Tangent:
            if (constraint->SecondPos != none) // tangency at common point
                rtn = addTangentConstraint(constraint->First,constraint->FirstPos,
                                           constraint->Second,constraint->SecondPos);
            else if (constraint->Second != -1) {
                if (constraint->FirstPos != none) // "First" is a tangency point
                    rtn = addTangentConstraint(constraint->First,constraint->FirstPos,
                                               constraint->Second);
                else // simple tangency
                    rtn = addTangentConstraint(constraint->First,constraint->Second);
            }
            break;
        case Distance:
            if (constraint->SecondPos != none) // point to point distance
                rtn = addDistanceConstraint(constraint->First,constraint->FirstPos,
                                            constraint->Second,constraint->SecondPos,
                                            constraint->Value);
            else if (constraint->Second != -1) {
                if (constraint->FirstPos != none) // point to line distance
                    rtn = addDistanceConstraint(constraint->First,constraint->FirstPos,
                                                constraint->Second,constraint->Value);
                else // line to line distance (not implemented yet)
                    rtn = addDistanceConstraint(constraint->First,constraint->Second,constraint->Value);
            }
            else // line length
                rtn = addDistanceConstraint(constraint->First,constraint->Value);
            break;
        case Angle:
            break;
        case Radius:
            rtn = addRadiusConstraint(constraint->First, constraint->Value);
            break;
        case None:
            break;
    }
    return rtn;
}

int Sketch::addConstraints(const std::vector<Constraint *> &ConstraintList)
{
    // constraints on nothing makes no sense 
    assert(int(Geoms.size()) > 0 || ConstraintList.size() == 0);

    int rtn = -1;
    for (std::vector<Constraint *>::const_iterator it = ConstraintList.begin();it!=ConstraintList.end();++it)
        rtn = addConstraint (*it);

    return rtn;
}

int Sketch::addCoordinateXConstraint(int geoId, PointPos pos, double value)
{
    int pointId = getPointId(geoId, pos);
 
    if (pointId >= 0 && pointId < int(Points.size())) {
        double *val = new double(value);
        FixParameters.push_back(val);
        GCS::Point &p = Points[pointId];
        return GCSsys.addConstraintCoordinateX(p, val);
    }
    return -1;
}

int Sketch::addCoordinateYConstraint(int geoId, PointPos pos, double value)
{
    int pointId = getPointId(geoId, pos);
 
    if (pointId >= 0 && pointId < int(Points.size())) {
        double *val = new double(value);
        FixParameters.push_back(val);
        GCS::Point &p = Points[pointId];
        return GCSsys.addConstraintCoordinateY(p, val);
    }
    return -1;
}

int Sketch::addDistanceXConstraint(int geoId, double value)
{
    assert(geoId < int(Geoms.size()));
    assert(Geoms[geoId].type == Line);

    GCS::Line &l = Lines[Geoms[geoId].index];

    FixParameters.push_back(new double(value));
    double *diff = FixParameters[FixParameters.size()-1];

    return GCSsys.addConstraintDifference(l.p1.x, l.p2.x, diff);
}

int Sketch::addDistanceYConstraint(int geoId, double value)
{
    assert(geoId < int(Geoms.size()));
    assert(Geoms[geoId].type == Line);

    GCS::Line &l = Lines[Geoms[geoId].index];

    FixParameters.push_back(new double(value));
    double *diff = FixParameters[FixParameters.size()-1];

    return GCSsys.addConstraintDifference(l.p1.y, l.p2.y, diff);
}

int Sketch::addDistanceXConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2, double value)
{
    int pointId1 = getPointId(geoId1, pos1);
    int pointId2 = getPointId(geoId2, pos2);

    if (pointId1 >= 0 && pointId1 < int(Points.size()) &&
        pointId2 >= 0 && pointId2 < int(Points.size())) {
        GCS::Point &p1 = Points[pointId1];
        GCS::Point &p2 = Points[pointId2];

        FixParameters.push_back(new double(value));
        double *difference = FixParameters[FixParameters.size()-1];

        return GCSsys.addConstraintDifference(p1.x, p2.x, difference);
    }
    return -1;
}

int Sketch::addDistanceYConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2, double value)
{
    int pointId1 = getPointId(geoId1, pos1);
    int pointId2 = getPointId(geoId2, pos2);

    if (pointId1 >= 0 && pointId1 < int(Points.size()) &&
        pointId2 >= 0 && pointId2 < int(Points.size())) {
        GCS::Point &p1 = Points[pointId1];
        GCS::Point &p2 = Points[pointId2];

        FixParameters.push_back(new double(value));
        double *difference = FixParameters[FixParameters.size()-1];

        return GCSsys.addConstraintDifference(p1.y, p2.y, difference);
    }
    return -1;
}

// horizontal line constraint
int Sketch::addHorizontalConstraint(int geoId)
{
    assert(geoId < int(Geoms.size()));
    assert(Geoms[geoId].type == Line);

    GCS::Line &l = Lines[Geoms[geoId].index];
    return GCSsys.addConstraintHorizontal(l);
}

// two points on a horizontal line constraint
int Sketch::addHorizontalConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2)
{
    int pointId1 = getPointId(geoId1, pos1);
    int pointId2 = getPointId(geoId2, pos2);

    if (pointId1 >= 0 && pointId1 < int(Points.size()) &&
        pointId2 >= 0 && pointId2 < int(Points.size())) {
        GCS::Point &p1 = Points[pointId1];
        GCS::Point &p2 = Points[pointId2];
        return GCSsys.addConstraintHorizontal(p1,p2);
    }
    return -1;
}

// vertical line constraint
int Sketch::addVerticalConstraint(int geoId)
{
    assert(geoId < int(Geoms.size()));
    assert(Geoms[geoId].type == Line);

    GCS::Line &l = Lines[Geoms[geoId].index];
    return GCSsys.addConstraintVertical(l);
}

// two points on a vertical line constraint
int Sketch::addVerticalConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2)
{
    int pointId1 = getPointId(geoId1, pos1);
    int pointId2 = getPointId(geoId2, pos2);

    if (pointId1 >= 0 && pointId1 < int(Points.size()) &&
        pointId2 >= 0 && pointId2 < int(Points.size())) {
        GCS::Point &p1 = Points[pointId1];
        GCS::Point &p2 = Points[pointId2];
        return GCSsys.addConstraintVertical(p1,p2);
    }
    return -1;
}

int Sketch::addPointCoincidentConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2)
{
    int pointId1 = getPointId(geoId1, pos1);
    int pointId2 = getPointId(geoId2, pos2);

    if (pointId1 >= 0 && pointId1 < int(Points.size()) &&
        pointId2 >= 0 && pointId2 < int(Points.size())) {
        GCS::Point &p1 = Points[pointId1];
        GCS::Point &p2 = Points[pointId2];
        return GCSsys.addConstraintP2PCoincident(p1, p2);
    }
    return -1;
}

int Sketch::addParallelConstraint(int geoId1, int geoId2)
{
    assert(geoId1 < int(Geoms.size()));
    assert(geoId2 < int(Geoms.size()));
    assert(Geoms[geoId1].type == Line);
    assert(Geoms[geoId2].type == Line);

    GCS::Line &l1 = Lines[Geoms[geoId1].index];
    GCS::Line &l2 = Lines[Geoms[geoId2].index];
    return GCSsys.addConstraintParallel(l1, l2);
}

const char* nameFromType(Sketch::GeoType type)
{
    switch(type) {
    case Sketch::Point:
        return "point";
    case Sketch::Line:
        return "line";
    case Sketch::Arc:
        return "arc";
    case Sketch::Circle:
        return "circle";
    case Sketch::Ellipse:
        return "ellipse";
    case Sketch::None:
    default:
        return "unknown";
    }
}

int Sketch::addPerpendicularConstraint(int geoId1, int geoId2)
{
    assert(geoId1 < int(Geoms.size()));
    assert(geoId2 < int(Geoms.size()));

    if (Geoms[geoId2].type == Line) {
        if (Geoms[geoId1].type == Line) {
            GCS::Line &l1 = Lines[Geoms[geoId1].index];
            GCS::Line &l2 = Lines[Geoms[geoId2].index];
            return GCSsys.addConstraintPerpendicular(l1, l2);
        }
        else
            std::swap(geoId1, geoId2);
    }

    if (Geoms[geoId1].type == Line) {
        GCS::Line &l = Lines[Geoms[geoId1].index];
        if (Geoms[geoId2].type == Arc) {
            GCS::Arc &a = Arcs[Geoms[geoId2].index];
            //return GCSsys.addConstraintPerpendicular(l, a);
            Base::Console().Warning("Perpendicular constraints between lines and arcs are not implemented yet.\n");
            return -1;
        } else if (Geoms[geoId2].type == Circle) {
            GCS::Circle &c = Circles[Geoms[geoId2].index];
            //return GCSsys.addConstraintPerpendicular(l, c);
            Base::Console().Warning("Perpendicular constraints between lines and circles are not implemented yet.\n");
            return -1;
        }
    }

    Base::Console().Warning("Perpendicular constraints between %s and %s are not supported.\n",
        nameFromType(Geoms[geoId1].type), nameFromType(Geoms[geoId2].type));
    return -1;
}

// simple tangency constraint
int Sketch::addTangentConstraint(int geoId1, int geoId2)
{
    // accepts the following combinations:
    // 1) Line1, Line2/Circle2/Arc2
    // 2) Circle1, Line2 (converted to case #1)
    //    Circle1, Circle2/Arc2 (not implemented yet)
    // 3) Arc1, Line2 (converted to case #1)
    //    Arc1, Circle2/Arc2 (not implemented yet)
    assert(geoId1 < int(Geoms.size()));
    assert(geoId2 < int(Geoms.size()));

    if (Geoms[geoId2].type == Line) {
        if (Geoms[geoId1].type == Line) {
            GCS::Line &l1 = Lines[Geoms[geoId1].index];
            GCS::Line &l2 = Lines[Geoms[geoId2].index];
            return GCSsys.addConstraintParallel(l1, l2);
        }
        else
            std::swap(geoId1, geoId2);
    }

    if (Geoms[geoId1].type == Line) {
        GCS::Line &l = Lines[Geoms[geoId1].index];
        if (Geoms[geoId2].type == Arc) {
            GCS::Arc &a = Arcs[Geoms[geoId2].index];
            return GCSsys.addConstraintTangent(l, a);
        } else if (Geoms[geoId2].type == Circle) {
            GCS::Circle &c = Circles[Geoms[geoId2].index];
            return GCSsys.addConstraintTangent(l, c);
        }
    } else
        Base::Console().Warning("Tangency constraints between circles and arcs are not implemented yet.\n");

    return -1;
}

// tangency at specific point constraint
int Sketch::addTangentConstraint(int geoId1, PointPos pos1, int geoId2)
{
    // accepts the following combinations:
    // 1) Line1, start/end/mid, Line2
    // 2) Line1, start/end/mid, Circle2
    // 3) Line1, start/end/mid, Arc2
    // 4) Arc1, start/end, Line2
    // 5) Arc1, start/end, Circle2 (not implemented yet)
    // 6) Arc1, start/end, Arc2 (not implemented yet)

    int pointId1 = getPointId(geoId1, pos1);
    assert(geoId2 < int(Geoms.size()));

    if (pointId1 < 0 || pointId1 >= int(Points.size()))
        return addTangentConstraint(geoId1, geoId2);

    GCS::Point &p1 = Points[pointId1];
    if (Geoms[geoId1].type == Line) {
        GCS::Line &l1 = Lines[Geoms[geoId1].index];
        if (Geoms[geoId2].type == Line) {
            GCS::Line &l2 = Lines[Geoms[geoId2].index];
            GCSsys.addConstraintPointOnLine(p1, l2);
            return GCSsys.addConstraintParallel(l1, l2);
        }
        else if (Geoms[geoId2].type == Arc) {
            GCS::Arc &a2 = Arcs[Geoms[geoId2].index];
            GCSsys.addConstraintPointOnArc(p1, a2);
            return GCSsys.addConstraintTangent(l1, a2);
        }
        else if (Geoms[geoId2].type == Circle) {
            GCS::Circle &c2 = Circles[Geoms[geoId2].index];
            GCSsys.addConstraintPointOnCircle(p1, c2);
            return GCSsys.addConstraintTangent(l1, c2);
        }
    }
    else if (Geoms[geoId1].type == Arc) {
        GCS::Arc &a1 = Arcs[Geoms[geoId1].index];
        if (Geoms[geoId2].type == Line) {
            GCS::Line &l2 = Lines[Geoms[geoId2].index];
            GCSsys.addConstraintPointOnLine(p1, l2);
            return GCSsys.addConstraintTangent(l2, a1);
        }
        else if (Geoms[geoId2].type == Arc) {
            //GCS::Arcs &a2 = Arcs[Geoms[geoId2].index];
            //GCSsys.addConstraintPointOnArc(p1, a2);
            //return GCSsys.addConstraintTangent(a1, a2);
            Base::Console().Warning("Tangency constraints between circles and arcs are not implemented yet.\n");
        }
        else if (Geoms[geoId2].type == Circle) {
            //GCS::Circle &c2 = Circles[Geoms[geoId2].index];
            //GCSsys.addConstraintPointOnCircle(p1, c2);
            //return GCSsys.addConstraintTangent(a1, c2);
            Base::Console().Warning("Tangency constraints between circles and arcs are not implemented yet.\n");
        }
    }
    return -1;
}

// tangency at common point constraint
int Sketch::addTangentConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2)
{
    // accepts the following combinations:
    // 1) Line1, start/end/mid, Line2, start/end/mid
    // 2) Line1, start/end/mid, Arc2, start/end
    // 3) Arc1, start/end, Line2, start/end/mid (converted to case #2)
    // 4) Arc1, start/end, Arc2, start/end (not implemented yet)

    int pointId1 = getPointId(geoId1, pos1);
    int pointId2 = getPointId(geoId2, pos2);

    if (pointId1 < 0 || pointId1 >= int(Points.size()) ||
        pointId2 < 0 || pointId2 >= int(Points.size()))
        return -1;


    GCS::Point &p1 = Points[pointId1];
    GCS::Point &p2 = Points[pointId2];
    if (Geoms[geoId2].type == Line) {
        if (Geoms[geoId1].type == Line) {
            GCS::Line &l1 = Lines[Geoms[geoId1].index];
            GCS::Line &l2 = Lines[Geoms[geoId2].index];
            GCSsys.addConstraintP2PCoincident(p1, p2);
            return GCSsys.addConstraintParallel(l1, l2);
        }
        else {
            std::swap(geoId1, geoId2);
            std::swap(pos1, pos2);
            std::swap(pointId1, pointId2);
            p1 = Points[pointId1];
            p2 = Points[pointId2];
        }
    }

    if (Geoms[geoId1].type == Line) {
        GCS::Line &l1 = Lines[Geoms[geoId1].index];
        if (Geoms[geoId2].type == Line) {
            GCS::Line &l2 = Lines[Geoms[geoId2].index];
            GCSsys.addConstraintP2PCoincident(p1, p2);
            return GCSsys.addConstraintParallel(l1, l2);
        }
        else if (Geoms[geoId2].type == Arc) {
            GCS::Arc &a2 = Arcs[Geoms[geoId2].index];
            if (pos2 == start) {
                if (pos1 == start)
                    return GCSsys.addConstraintLine2Arc(l1.p2, l1.p1, a2);
                else if (pos1 == end)
                    return GCSsys.addConstraintLine2Arc(l1.p1, l1.p2, a2);
                else if (pos1 == mid) {
                    GCSsys.addConstraintP2PCoincident(p1, p2);
                    return GCSsys.addConstraintTangent(l1, a2);
                }
            }
            else if (pos2 == end) {
                if (pos1 == start)
                    return GCSsys.addConstraintArc2Line(a2, l1.p1, l1.p2);
                else if (pos1 == end)
                    return GCSsys.addConstraintArc2Line(a2, l1.p2, l1.p1);
                else if (pos1 == mid) {
                    GCSsys.addConstraintP2PCoincident(p1, p2);
                    return GCSsys.addConstraintTangent(l1, a2);
                }
            }
            else
                return -1;
        }
    }
    else if (Geoms[geoId1].type == Arc) {
        //GCS::Arc &a1 = Arcs[Geoms[geoId1].index];
        if (Geoms[geoId2].type == Arc) {
            //GCS::Arcs &a2 = Arcs[Geoms[geoId2].index];
            //GCSsys.addConstraintPointOnArc(p1, a2);
            //return GCSsys.addConstraintTangent(a1, a2);
            Base::Console().Warning("Tangency constraints between arcs are not implemented yet.\n");
        }
    }
    return -1;
}

// line length constraint
int Sketch::addDistanceConstraint(int geoId, double value)
{
    assert(geoId < int(Geoms.size()));
    assert(Geoms[geoId].type == Line);

    GCS::Line &l = Lines[Geoms[geoId].index];

    // add the parameter for the length
    FixParameters.push_back(new double(value));
    double *distance = FixParameters[FixParameters.size()-1];

    return GCSsys.addConstraintP2PDistance(l.p1, l.p2, distance);
}

// line to line distance constraint
int Sketch::addDistanceConstraint(int geoId1, int geoId2, double value)
{
    assert(geoId1 < int(Geoms.size()));
    assert(geoId2 < int(Geoms.size()));

    assert(Geoms[geoId1].type == Line);
    assert(Geoms[geoId2].type == Line);

    Base::Console().Warning("Line to line distance constraints are not implemented yet.\n");

    return -1;
}

// point to line distance constraint
int Sketch::addDistanceConstraint(int geoId1, PointPos pos1, int geoId2, double value)
{
    int pointId1 = getPointId(geoId1, pos1);
    assert(geoId2 < int(Geoms.size()));
    assert(Geoms[geoId2].type == Line);

    if (pointId1 >= 0 && pointId1 < int(Points.size())) {
        GCS::Point &p1 = Points[pointId1];
        GCS::Line &l2 = Lines[Geoms[geoId2].index];

        // add the parameter for the distance
        FixParameters.push_back(new double(value));
        double *distance = FixParameters[FixParameters.size()-1];

        return GCSsys.addConstraintP2LDistance(p1, l2, distance);
    }
    return -1;
}

// point to point distance constraint
int Sketch::addDistanceConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2, double value)
{
    int pointId1 = getPointId(geoId1, pos1);
    int pointId2 = getPointId(geoId2, pos2);

    if (pointId1 >= 0 && pointId1 < int(Points.size()) &&
        pointId2 >= 0 && pointId2 < int(Points.size())) {
        GCS::Point &p1 = Points[pointId1];
        GCS::Point &p2 = Points[pointId2];

        // add the parameter for the distance
        FixParameters.push_back(new double(value));
        double *distance = FixParameters[FixParameters.size()-1];

        return GCSsys.addConstraintP2PDistance(p1, p2, distance);
    }
    return -1;
}

int Sketch::addRadiusConstraint(int geoId, double value)
{
    assert(geoId < int(Geoms.size()));
    assert(Geoms[geoId].type == Circle || Geoms[geoId].type == Arc);

    if (Geoms[geoId].type == Circle) {
        GCS::Circle &c = Circles[Geoms[geoId].index];
        // add the parameter for the radius
        FixParameters.push_back(new double(value));
        double *radius = FixParameters[FixParameters.size()-1];
        return GCSsys.addConstraintCircleRadius(c, radius);
    }
    else if (Geoms[geoId].type == Arc) {
        GCS::Arc &a = Arcs[Geoms[geoId].index];
        // add the parameter for the radius
        FixParameters.push_back(new double(value));
        double *radius = FixParameters[FixParameters.size()-1];
        return GCSsys.addConstraintArcRadius(a, radius);
    }
    return -1;
}

// solving ==========================================================

int Sketch::solve() {

    Base::TimeInfo start_time;

    // solving with freegcs
    int ret = GCSsys.solve();
    // if successfully solve write the parameter back
    if (ret == GCS::Success) {
        GCSsys.applySolution();
        int i=0;
        for (std::vector<GeoDef>::const_iterator it=Geoms.begin();it!=Geoms.end();++it,i++) {
            try {
                if (it->type == Line) {
                    GeomLineSegment *lineSeg = dynamic_cast<GeomLineSegment*>(it->geo);
                    lineSeg->setPoints(Vector3d(*Lines[it->index].p1.x,
                                                *Lines[it->index].p1.y,
                                                0.0),
                                       Vector3d(*Lines[it->index].p2.x,
                                                *Lines[it->index].p2.y,
                                                0.0)
                                      );
                } else if (it->type == Arc) {
                    GCS::Arc &myArc = Arcs[it->index];
                    // the following 4 lines are redundant since these equations are already included in the arc constraints
//                    *myArc.start.x = *myArc.center.x + *myArc.rad * cos(*myArc.startAngle);
//                    *myArc.start.y = *myArc.center.y + *myArc.rad * sin(*myArc.startAngle);
//                    *myArc.end.x = *myArc.center.x + *myArc.rad * cos(*myArc.endAngle);
//                    *myArc.end.y = *myArc.center.y + *myArc.rad * sin(*myArc.endAngle);
                    GeomArcOfCircle *aoc = dynamic_cast<GeomArcOfCircle*>(it->geo);
                    aoc->setCenter(Vector3d(*Points[it->midPointId].x,
                                            *Points[it->midPointId].y,
                                            0.0)
                                   );
                    aoc->setRadius(*myArc.rad);
                    aoc->setRange(*myArc.startAngle, *myArc.endAngle);
                } else if (it->type == Circle) {
                    GeomCircle *circ = dynamic_cast<GeomCircle*>(it->geo);
                    circ->setCenter(Vector3d(*Points[it->midPointId].x,
                                             *Points[it->midPointId].y,
                                             0.0)
                                   );
                    circ->setRadius(*Circles[it->index].rad);
                }
            } catch (Base::Exception e) {
                Base::Console().Error("Solve: Error build geometry(%d): %s\n",i,e.what());
                return -1;
            }
        }
    } else {
        //Base::Console().Log("NotSolved ");
    }
    Base::TimeInfo end_time;
    //Base::Console().Log("T:%s\n",Base::TimeInfo::diffTime(start_time,end_time).c_str());
    SolveTime = Base::TimeInfo::diffTimeF(start_time,end_time);
    return ret;
}

int Sketch::initMove(int geoId, PointPos pos)
{
    assert(geoId >= 0 && geoId < int(Geoms.size()));

    GCSsys.clearLevel(-1);

    // lists of fixed parameters, points, etc.
    std::vector<double*> fixedParams;
    if (Geoms[geoId].type == Line) {
        if (pos == start || pos == end) {
            MoveParameters.resize(2); // x,y
            GCS::Point p0;
            p0.x = &MoveParameters[0];
            p0.y = &MoveParameters[1];
            if (pos == start) {
                GCS::Point &p = Points[Geoms[geoId].startPointId];
                *p0.x = *p.x;
                *p0.y = *p.y;
                GCSsys.addConstraintP2PCoincident(p0,p,-1);
            } else if (pos == end) {
                GCS::Point &p = Points[Geoms[geoId].endPointId];
                *p0.x = *p.x;
                *p0.y = *p.y;
                GCSsys.addConstraintP2PCoincident(p0,p,-1);
            }
        } else if (pos == none || pos == mid) {
            MoveParameters.resize(6); // x1,y1,x2,y2,dx,dy
            GCS::Point p1, p2;
            p1.x = &MoveParameters[0];
            p1.y = &MoveParameters[1];
            p2.x = &MoveParameters[2];
            p2.y = &MoveParameters[3];
            GCS::Line &l = Lines[Geoms[geoId].index];
            *p1.x = *l.p1.x;
            *p1.y = *l.p1.y;
            *p2.x = *l.p2.x;
            *p2.y = *l.p2.y;
             MoveParameters[4] = *l.p2.x - *l.p1.x;
             MoveParameters[5] = *l.p2.y - *l.p1.y;
             GCSsys.addConstraintP2PCoincident(p1,l.p1,-1);
             GCSsys.addConstraintP2PCoincident(p2,l.p2,-1);
        }
    } else if (Geoms[geoId].type == Circle) {
        if (pos == mid || pos == none) {
            if (pos == none)
                MoveParameters.resize(4); // cx,cy,x,y
            else
                MoveParameters.resize(2); // cx,cy
            GCS::Point &center = Points[Geoms[geoId].midPointId];
            GCS::Point p0;
            p0.x = &MoveParameters[0];
            p0.y = &MoveParameters[1];
            *p0.x = *center.x;
            *p0.y = *center.y;
            GCSsys.addConstraintP2PCoincident(p0,center,-1);
            if (pos == none) {
                GCS::Circle &c = Circles[Geoms[geoId].index];
                GCS::Point p1;
                p1.x = &MoveParameters[2];
                p1.y = &MoveParameters[3];
                *p1.x = *center.x;
                *p1.y = *center.y + *c.rad;
                GCSsys.addConstraintPointOnCircle(p1,c,-1);
            }
        }
    } else if (Geoms[geoId].type == Arc) {
        if (pos == start || pos == end || pos == mid || pos == none) {
            if (pos == mid)
                MoveParameters.resize(2); // cx,cy
            else
                MoveParameters.resize(4); // cx,cy,x,y
            GCS::Point &center = Points[Geoms[geoId].midPointId];
            GCS::Point p0;
            p0.x = &MoveParameters[0];
            p0.y = &MoveParameters[1];
            *p0.x = *center.x;
            *p0.y = *center.y;
            GCSsys.addConstraintP2PCoincident(p0,center,-1);
            if (pos == start || pos == end) {
                GCS::Point &p = (pos == start) ? Points[Geoms[geoId].startPointId]
                                               : Points[Geoms[geoId].endPointId];;
                GCS::Point p1;
                p1.x = &MoveParameters[2];
                p1.y = &MoveParameters[3];
                *p1.x = *p.x;
                *p1.y = *p.y;
                GCSsys.addConstraintP2PCoincident(p1,p,-1);
            } else if (pos == none) {
                GCS::Arc &a = Arcs[Geoms[geoId].index];
                GCS::Point p1;
                p1.x = &MoveParameters[2];
                p1.y = &MoveParameters[3];
                *p1.x = *center.x;
                *p1.y = *center.y + *a.rad;
                GCSsys.addConstraintPointOnArc(p1,a,-1);
            }
        }

    }

    GCSsys.initSolution(Parameters);
    isInitMove = true;
    return 0;
}

int Sketch::movePoint(int geoId, PointPos pos, Base::Vector3d toPoint)
{
    // index out of bounds?
    assert(geoId < int(Geoms.size()));

    if (!isInitMove)
        initMove(geoId, pos);

    // lists of fixed parameters, points, etc.
    std::vector<double*> fixedParams;
    if (Geoms[geoId].type == Line) {
        if (pos == start || pos == end) {
            MoveParameters[0] = toPoint.x;
            MoveParameters[1] = toPoint.y;
        } else if (pos == none || pos == mid) {
            double dx =  MoveParameters[4]/2;
            double dy =  MoveParameters[5]/2;
            MoveParameters[0] = toPoint.x - dx;
            MoveParameters[1] = toPoint.y - dy;
            MoveParameters[2] = toPoint.x + dx;
            MoveParameters[3] = toPoint.y + dy;
        }
    } else if (Geoms[geoId].type == Circle) {
        if (pos == mid || pos == none) {
            if (pos == none) {
                MoveParameters[2] = toPoint.x;
                MoveParameters[3] = toPoint.y;
            } else {
                MoveParameters[0] = toPoint.x;
                MoveParameters[1] = toPoint.y;
            }
        }
    } else if (Geoms[geoId].type == Arc) {
        if (pos == start || pos == end || pos == mid || pos == none) {
            if (pos == mid) {
                MoveParameters[0] = toPoint.x;
                MoveParameters[1] = toPoint.y;
            } else {
                MoveParameters[2] = toPoint.x;
                MoveParameters[3] = toPoint.y;
            }
        }

    }

    return solve();
}

int Sketch::setDatum(int constraintIndex, double value)
{
    return -1;
}

int Sketch::getPointId(int geoId, PointPos pos) const
{
    assert(geoId < int(Geoms.size()));
    switch(pos) {
        case start:
            return Geoms[geoId].startPointId;
        case end:
            return Geoms[geoId].endPointId;
        case mid:
            return Geoms[geoId].midPointId;
        case none:
            break;
    }
    return -1;
}

Base::Vector3d Sketch::getPoint(int geoId, PointPos pos)
{
    int pointId = getPointId(geoId, pos);
    if (pointId != -1)
        return Base::Vector3d(*Points[pointId].x, *Points[pointId].y, 0);

    return Base::Vector3d();
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
        Base::Console().Warning("Left over edges in Sketch. Only closed structures will be propagated at the moment!\n");

#endif

    return result;
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

