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


class Sketch :public Base::Persistence
{
    TYPESYSTEM_HEADER();

public:
    Sketch();
    ~Sketch();

	// from base class
    virtual unsigned int getMemSize (void) const;
	virtual void Save (Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

    /// define if you whant to use the end or start point
    enum PointPos {start,end };

    int solve(void);
    /// return the actual geometry of the sketch a TopoShape
    Part::TopoShape toShape(void);
    /// add unspecified geomtry
    int addGeometry(Part::GeomCurve *geo);
    /// returns the actual geometry 
    std::vector<Part::GeomCurve *> getGeometry(bool withConstrucionElements = false) const;
    /// get the geometry as python objects
    Py::Tuple getPyGeometry(void) const;
    /// set an geometric element to a construction element
    void setConstruction(int geoIndex,bool isConstruction=true);
    bool getConstruction(int geoIndex) const;

    /// move this point to a new location and solve
    int movePoint(int geoIndex1,PointPos Pos1,Base::Vector3d toPoint);

    /// add dedicated geometry
    //@{
    /// add a point 
    int addPoint(Base::Vector3d point);
    /// add a infinit line 
    int addLine(Part::GeomLineSegment line);
    /// add a line segment 
    int addLineSegment(Part::GeomLineSegment lineSegment);
    /// add a arc (circle segment)
    int addArc(Part::GeomTrimmedCurve circleSegment);
    /// add a circle
    int addCircle(Part::GeomCircle circle);
    /// add a ellipse
    int addEllibse(Part::GeomEllipse ellibse);
    //@}


    /// constraints
    //@{
    /// add a horizontal constraint to a geometry
    int addHorizontalConstraint(int geoIndex, const char* name=0);
    /// add a vertical constraint to a geometry
    int addVerticalConstraint(int geoIndex, const char* name=0);
    /// add a coincident constraint to two points of two geometries
    int addPointCoincidentConstraint(int geoIndex1,PointPos Pos1,int geoIndex2,PointPos Pos2, const char* name=0);
    //@}

    enum GeoType {
        Point   = 1,
        Line    = 2,
        Arc     = 3,
        Circle  = 4,
        Ellipse = 5
    };


protected:
    /// container element to store and work with the gometric elements of this sketch
    struct GeoDef {
        Part::GeomCurve * geo;                 // pointer to the geometry
        GeoType           type;                // type of the geometry
        bool              construction;        // defines if this element is a construction element
        int               parameterStartIndex; // start index for the points of this geometry
        int               pointStartIndex;     // start index for the points of this geometry
        int               lineStartIndex;      // start index of the lines of this geometry
        int               circleStartIndex;    // start index of the circle of this geometry
    };

    std::vector<GeoDef> Geoms;
    std::vector<Constraint> Const;

    // solving parameters
    std::vector<double*> Parameters;
    std::vector<point>  Points;
    std::vector<line>   Lines;
    std::vector<circle> Circles;


};

} //namespace Part


#endif // SKETCHER_SKETCH_H
