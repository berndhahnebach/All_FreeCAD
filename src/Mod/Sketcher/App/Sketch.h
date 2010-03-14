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

    int solve(void);
    int addGeometry(Part::GeomCurve *geo);

    /// add dedicated geometry
    //@{
    /// add a line segment 
    int addLineSegment(Part::GeomLineSegment lineSegment);
    //@}

    /// constraints
    //@{
    /// add a horizontal constraint to a geometry
    int addHorizontalConstraint(int geoIndex, const char* name=0);
    /// add a vertical constraint to a geometry
    int addVerticalConstraint(int geoIndex, const char* name=0);
    //@}

    enum GeoType {
        Point = 1,
        Line  = 2
    };


protected:
    /// container element to store and work with the gometric elements of this sketch
    struct GeoDef {
        Part::GeomCurve * geo;                 // pointer to the geometry
        GeoType           type;                // type of the geometry
        int               parameterStartIndex; // start index for the points of this geometry
        int               pointStartIndex;     // start index for the points of this geometry
        int               lineStartIndex;      // start indes of the lines of this geometry
    };

    std::vector<GeoDef> Geoms;
    std::vector<Sketch::Constraint> Const;

    // solving parameters
    std::vector<double*> Parameters;
    std::vector<point>  Points;
    std::vector<line>   Lines;


};

} //namespace Part


#endif // SKETCHER_SKETCH_H
