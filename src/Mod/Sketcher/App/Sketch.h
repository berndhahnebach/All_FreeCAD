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

#ifndef SKETCHER_SKETCH_H
#define SKETCHER_SKETCH_H

#include <App/PropertyStandard.h>
#include <App/PropertyFile.h>
#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/TopoShape.h>
#include "Constraint.h"

#include "sketchsolve_cp/solve.h"

#include <Base/Persistence.h>

namespace Sketcher
{
/*
 class GeometryList: public std::vector<Part::Geometry *>{
 public:
     explicit GeometryList(size_type _Count):std::vector<Part::Geometry *>(_Count){}
     explicit GeometryList(){}
     virtual ~GeometryList(){
         for(const_iterator it=begin();it!=end();++it)
             if(*it)
                 delete(*it);
     }
 };
*/
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

    /** solve the actual set up sketch
      * If the two fixes pointers are non null it leafs out 
      * this two parameters from the solving. Its like a fixes
      * constraint on a point
      */
    int solve(double * fixed[2]);
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

    /// move this point to a new location and solve
    int movePoint(int geoIndex1, PointPos Pos1, Base::Vector3d toPoint);

    /// add dedicated geometry
    //@{
    /// add a point 
    int addPoint(Base::Vector3d point);
    /// add a infinit line 
    int addLine(const Part::GeomLineSegment &line);
    /// add a line segment 
    int addLineSegment(const Part::GeomLineSegment &lineSegment);
    /// add a arc (circle segment)
    int addArc(const Part::GeomTrimmedCurve &circleSegment);
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
    /// add a horizontal constraint to a geometry
    int addHorizontalConstraint(int geoIndex);
    /// add a vertical constraint to a geometry
    int addVerticalConstraint(int geoIndex);
    /// add a coincident constraint to two points of two geometries
    int addPointCoincidentConstraint(int geoIndex1, PointPos Pos1, int geoIndex2, PointPos Pos2);
    /// add a length constraint to a line
    int addDistanceConstraint(int geoIndex1, double Value);
    /// add a distance between two lines (basically a point to line distance)
    int addDistanceConstraint(int geoIndex1, int geoIndex2, double Value);
    /// add a parallel constraints between two lines
    int addParallelConstraint(int geoIndex1, int geoIndex2);
    //@}

    enum GeoType {
        None    = 0,
        Point   = 1, // 1 Point(start), 2 Parameters(x,y)
        Line    = 2, // 2 Points(start,end), 4 Parameters(x1,y1,x2,y2)
        Arc     = 3,
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
        int               index;           // index in the corresponding storage vector (Lines, Circles, ...) 
        int               startPointId;    // index in Points of the start point of this geometry
        int               midPointId;      // index in Points of the start point of this geometry
        int               endPointId;      // index in Points of the end point of this geometry
    };

    std::vector<GeoDef> Geoms;
    std::vector<constraint> Const;

    // solving parameters
    std::vector<double*> Parameters;
    std::vector<double*> FixParameters;
    std::vector<point>  Points;
    std::vector<line>   Lines;
    std::vector<circle> Circles;

    // helper for the PointOnPoint constraint optimization
    struct PoPConst {
        PoPConst():startParamId(-1),endParamId(-1),midParamId(-1){}
        int startParamId;
        int endParamId;
        int midParamId;
    };

    std::map<int,PoPConst> PoPMap;

};

} //namespace Part


#endif // SKETCHER_SKETCH_H
