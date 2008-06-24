/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <BRepBuilderAPI_MakeFace.hxx>
#endif

#include "Geometry.h"

using namespace Part;


Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

// -------------------------------------------------

GeomCurve::GeomCurve()
{
}

GeomCurve::~GeomCurve()
{
}

TopoDS_Shape GeomCurve::toShape() const
{
    Handle_Geom_Curve c = Handle_Geom_Curve::DownCast(handle());
    BRepBuilderAPI_MakeEdge mkBuilder(c, c->FirstParameter(), c->LastParameter());
    return mkBuilder.Shape();
}

// -------------------------------------------------

GeomBezierCurve::GeomBezierCurve()
{
}

GeomBezierCurve::~GeomBezierCurve()
{
}

Handle_Geom_Geometry GeomBezierCurve::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomBSplineCurve::GeomBSplineCurve()
{
}

GeomBSplineCurve::~GeomBSplineCurve()
{
}

Handle_Geom_Geometry GeomBSplineCurve::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomCircle::GeomCircle()
{
    Handle_Geom_Circle c = new Geom_Circle(gp_Circ());
    this->myCurve = c;
}

GeomCircle::GeomCircle(const Handle_Geom_Circle& c)
{
    this->myCurve = c;
}

GeomCircle::~GeomCircle()
{
}

Handle_Geom_Geometry GeomCircle::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomEllipse::GeomEllipse()
{
}

GeomEllipse::~GeomEllipse()
{
}

Handle_Geom_Geometry GeomEllipse::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomHyperbola::GeomHyperbola()
{
}

GeomHyperbola::~GeomHyperbola()
{
}

Handle_Geom_Geometry GeomHyperbola::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomParabola::GeomParabola()
{
}

GeomParabola::~GeomParabola()
{
}

Handle_Geom_Geometry GeomParabola::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomLine::GeomLine()
{
}

GeomLine::~GeomLine()
{
}

Handle_Geom_Geometry GeomLine::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomTrimmedCurve::GeomTrimmedCurve()
{
}

GeomTrimmedCurve::GeomTrimmedCurve(const Handle_Geom_TrimmedCurve& c)
{
    this->myCurve = c;
}

GeomTrimmedCurve::~GeomTrimmedCurve()
{
}

void GeomTrimmedCurve::setHandle(const Handle_Geom_TrimmedCurve& c)
{
    this->myCurve = c;
}

Handle_Geom_Geometry GeomTrimmedCurve::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomSurface::GeomSurface()
{
}

GeomSurface::~GeomSurface()
{
}

TopoDS_Shape GeomSurface::toShape() const
{
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(handle());
    Standard_Real u1,u2,v1,v2;
    s->Bounds(u1,u2,v1,v2);
    BRepBuilderAPI_MakeFace mkBuilder(s, u1, u2, v1, v2);
    return mkBuilder.Shape();
}

// -------------------------------------------------

GeomBezierSurface::GeomBezierSurface()
{
}

GeomBezierSurface::~GeomBezierSurface()
{
}

Handle_Geom_Geometry GeomBezierSurface::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomBSplineSurface::GeomBSplineSurface()
{
}

GeomBSplineSurface::~GeomBSplineSurface()
{
}

Handle_Geom_Geometry GeomBSplineSurface::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomCylinder::GeomCylinder()
{
}

GeomCylinder::~GeomCylinder()
{
}

Handle_Geom_Geometry GeomCylinder::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomToroid::GeomToroid()
{
}

GeomToroid::~GeomToroid()
{
}

Handle_Geom_Geometry GeomToroid::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomSphere::GeomSphere()
{
}

GeomSphere::~GeomSphere()
{
}

Handle_Geom_Geometry GeomSphere::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomPlane::GeomPlane()
{
}

GeomPlane::~GeomPlane()
{
}

Handle_Geom_Geometry GeomPlane::handle() const
{
    return mySurface;
}
