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

#ifndef SKETCHER_SKETCH_H
#define SKETCHER_SKETCH_H

#include <App/PropertyStandard.h>
#include <App/PropertyFile.h>
#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/TopoShape.h>
#include "Constraint.h"

#include "freegcs/GCS.h"

#include <Base/Persistence.h>

namespace Sketcher
{

class AppSketcherExport Sketch :public Base::Persistence
{
    TYPESYSTEM_HEADER();

public:
    Sketch();
    ~Sketch();

    // from base class
    virtual unsigned int getMemSize(void) const;
    virtual void Save(Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

    /// solve the actual set up sketch
    int solve(void);
    /// delete all geometry and constraints, leave an empty sketch
    void clear(void);
    /// set the sketch up with geoms and constraints
    void setUpSketch(const std::vector<Part::Geometry *> &geo,const std::vector<Constraint *> &ConstraintList);
    /// return the actual geometry of the sketch a TopoShape
    Part::TopoShape toShape(void) const;
    /// add unspecified geometry
    int addGeometry(const Part::Geometry *geo);
    /// add unspecified geometry
    void addGeometry(const std::vector<Part::Geometry *> &geo);
    /// returns the actual geometry
    std::vector<Part::Geometry *> getGeometry(bool withConstrucionElements = false) const;
    /// get the geometry as python objects
    Py::Tuple getPyGeometry(void) const;
    /// set an geometric element to a construction element
    void setConstruction(int geoIndex,bool isConstruction=true);
    bool getConstruction(int geoIndex) const;

    /// retrieves the index of a point
    int getPointId(int geoId, PointPos pos) const;
    /// retrieves a point
    Base::Vector3d getPoint(int geoId, PointPos pos);

    /** set the datum of a distance or angle constraint to a certain value and solve
      * This can cause the solving to fail!
      */
    int setDatum(int constraintIndex, double value);

    /** initializes a point (or curve) drag by setting the current
      * sketch status as a reference
      */
    int initMove(int geoIndex, PointPos pos);

    /** move this point to a new location and solve
      * this will introduce a fix constraint on the moved point
      * to ensure its stay on the right position. But this can cause
      * the solving to fail!
      */
    int movePoint(int geoIndex, PointPos pos, Base::Vector3d toPoint);

    /// add dedicated geometry
    //@{
    /// add a point
    int addPoint(const Base::Vector3d &point);
    /// add a infinit line
    int addLine(const Part::GeomLineSegment &line);
    /// add a line segment
    int addLineSegment(const Part::GeomLineSegment &lineSegment);
    /// add a arc (circle segment)
    int addArc(const Part::GeomArcOfCircle &circleSegment);
    /// add a circle
    int addCircle(const Part::GeomCircle &circle);
    /// add a ellipse
    int addEllipse(const Part::GeomEllipse &ellipse);
    //@}


    /// constraints
    //@{
    /// add all constraints in the list
    int addConstraints(const std::vector<Constraint *> &ConstraintList);
    /// add one constraint to the sketch
    int addConstraint(const Constraint *constraint);
    /// add a fixed coordinate constraint to a point
    int addCoordinateXConstraint(int geoIndex, PointPos pos, double value);
    int addCoordinateYConstraint(int geoIndex, PointPos pos, double value);
    /// add a horizontal distance constraint to two points or line ends
    int addDistanceXConstraint(int geoId, double value);
    int addDistanceXConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2, double value);
    /// add a vertical distance constraint to two points or line ends
    int addDistanceYConstraint(int geoId, double value);
    int addDistanceYConstraint(int geoId1, PointPos pos1, int geoId2, PointPos pos2, double value);
    /// add a horizontal constraint to a geometry
    int addHorizontalConstraint(int geoIndex);
    int addHorizontalConstraint(int geoIndex1, PointPos pos1, int geoIndex2, PointPos pos2);
    /// add a vertical constraint to a geometry
    int addVerticalConstraint(int geoIndex);
    int addVerticalConstraint(int geoIndex1, PointPos pos1, int geoIndex2, PointPos pos2);
    /// add a coincident constraint to two points of two geometries
    int addPointCoincidentConstraint(int geoIndex1, PointPos pos1, int geoIndex2, PointPos pos2);
    /// add a length or distance constraint
    int addDistanceConstraint(int geoIndex1, double value);
    int addDistanceConstraint(int geoIndex1, int geoIndex2, double value);
    int addDistanceConstraint(int geoIndex1, PointPos pos1, int geoIndex2, double value);
    int addDistanceConstraint(int geoIndex1, PointPos pos1, int geoIndex2, PointPos pos2, double value);
    /// add a parallel constraint between two lines
    int addParallelConstraint(int geoIndex1, int geoIndex2);
    /// add a perpendicular constraint between two lines
    int addPerpendicularConstraint(int geoIndex1, int geoIndex2);
    /// add a tangency constraint between two geometries
    int addTangentConstraint(int geoIndex1, int geoIndex2);
    int addTangentConstraint(int geoIndex1, PointPos pos1, int geoIndex2);
    int addTangentConstraint(int geoIndex1, PointPos pos1, int geoIndex2, PointPos pos2);
    /// add a radius constraint on a circle or an arc
    int addRadiusConstraint(int geoIndex, double value);
    //@}

    enum GeoType {
        None    = 0,
        Point   = 1, // 1 Point(start), 2 Parameters(x,y)
        Line    = 2, // 2 Points(start,end), 4 Parameters(x1,y1,x2,y2)
        Arc     = 3, // 3 Points(start,end,mid), (4)+5 Parameters((x1,y1,x2,y2),x,y,r,a1,a2)
        Circle  = 4, // 1 Point(mid), 3 Parameters(x,y,r)
        Ellipse = 5
    };

    float SolveTime;

protected:
    /// container element to store and work with the geometric elements of this sketch
    struct GeoDef {
        GeoDef() : geo(0),type(None),construction(false),index(-1),
                   startPointId(-1),midPointId(-1),endPointId(-1) {}
        Part::Geometry  * geo;             // pointer to the geometry
        GeoType           type;            // type of the geometry
        bool              construction;    // defines if this element is a construction element
        int               index;           // index in the corresponding storage vector (Lines, Arcs, Circles, ...)
        int               startPointId;    // index in Points of the start point of this geometry
        int               midPointId;      // index in Points of the start point of this geometry
        int               endPointId;      // index in Points of the end point of this geometry
    };

    std::vector<GeoDef> Geoms;
    GCS::System GCSsys;

    // solving parameters
    std::vector<double*> Parameters;    // with memory allocation
    std::vector<double*> FixParameters; // with memory allocation
    std::vector<double> MoveParameters;
    std::vector<GCS::Point>  Points;
    std::vector<GCS::Line>   Lines;
    std::vector<GCS::Arc>    Arcs;
    std::vector<GCS::Circle> Circles;

    bool isInitMove;
};

} //namespace Part


#endif // SKETCHER_SKETCH_H
