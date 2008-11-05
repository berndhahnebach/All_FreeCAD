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
# include <Geom_Circle.hxx>
# include <Geom_Curve.hxx>
# include <Geom_Ellipse.hxx>
# include <Geom_Hyperbola.hxx>
# include <Geom_Parabola.hxx>
# include <Geom_OffsetCurve.hxx>
# include <Geom_TrimmedCurve.hxx>
# include <Geom_Line.hxx>
# include <Geom_BezierCurve.hxx>
# include <Geom_BezierSurface.hxx>
# include <Geom_BSplineCurve.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Geom_Surface.hxx>
# include <Geom_Plane.hxx>
# include <Geom_CylindricalSurface.hxx>
# include <Geom_ConicalSurface.hxx>
# include <Geom_SphericalSurface.hxx>
# include <Geom_ToroidalSurface.hxx>
# include <Geom_OffsetSurface.hxx>
# include <Geom_SurfaceOfRevolution.hxx>
# include <Geom_SurfaceOfLinearExtrusion.hxx>
# include <gp_Circ.hxx>
# include <gp_Elips.hxx>
# include <gp_Hypr.hxx>
# include <gp_Parab.hxx>
# include <gce_ErrorType.hxx>
# include <gp_Lin.hxx>
# include <gp_Pln.hxx>
# include <gp_Cylinder.hxx>
# include <gp_Cone.hxx>
# include <gp_Sphere.hxx>
# include <gp_Torus.hxx>
# include <Standard_Real.hxx>
# include <TColgp_Array1OfPnt.hxx>
# include <TColgp_Array2OfPnt.hxx>
# include <TColStd_Array1OfReal.hxx>
# include <TColStd_Array1OfInteger.hxx>
#endif

#include "Geometry.h"

using namespace Part;


const char* gce_ErrorStatusText(gce_ErrorType et)
{
    switch (et)
    {
    case gce_Done:
        return "Construction was successful";
    case gce_ConfusedPoints:
        return "Two points are coincident";
    case gce_NegativeRadius:
        return "Radius value is negative";
    case gce_ColinearPoints:
        return "Three points are collinear";
    case gce_IntersectionError:
        return "Intersection cannot be computed";
    case gce_NullAxis:
        return "Axis is undefined";
    case gce_NullAngle:
        return "Angle value is invalid (usually null)";
    case gce_NullRadius:
        return "Radius is null";
    case gce_InvertAxis:
        return "Axis value is invalid";
    case gce_BadAngle:
        return "Angle value is invalid";
    case gce_InvertRadius:
        return "Radius value is incorrect (usually with respect to another radius)";
    case gce_NullFocusLength:
        return "Focal distance is null";
    case gce_NullVector:
        return "Vector is null";
    case gce_BadEquation:
        return "Coefficients are incorrect (applies to the equation of a geometric object)";
    default:
        return "Creation of geometry failed";
    }
}

// ---------------------------------------------------------------

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
    TColgp_Array1OfPnt poles(1,2);
    poles(1) = gp_Pnt(0.0,0.0,0.0);
    poles(2) = gp_Pnt(0.0,0.0,1.0);
    Handle_Geom_BezierCurve b = new Geom_BezierCurve(poles);
    this->myCurve = b;
}

GeomBezierCurve::GeomBezierCurve(const Handle_Geom_BezierCurve& b)
{
    this->myCurve = b;
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
    TColgp_Array1OfPnt poles(1,2);
    poles(1) = gp_Pnt(0.0,0.0,0.0);
    poles(2) = gp_Pnt(1.0,0.0,0.0);

    TColStd_Array1OfReal knots(1,2);
    knots(1) = 0.0;
    knots(2) = 1.0;

    TColStd_Array1OfInteger mults(1,2);
    mults(1) = 2;
    mults(2) = 2;

    this->myCurve = new Geom_BSplineCurve(poles, knots, mults, 1);
}

GeomBSplineCurve::GeomBSplineCurve(const Handle_Geom_BSplineCurve& b)
{
    this->myCurve = b;
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
    Handle_Geom_Ellipse e = new Geom_Ellipse(gp_Elips());
    this->myCurve = e;
}

GeomEllipse::GeomEllipse(const Handle_Geom_Ellipse& e)
{
    this->myCurve = e;
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
    Handle_Geom_Hyperbola h = new Geom_Hyperbola(gp_Hypr());
    this->myCurve = h;
}

GeomHyperbola::GeomHyperbola(const Handle_Geom_Hyperbola& h)
{
    this->myCurve = h;
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
    Handle_Geom_Parabola p = new Geom_Parabola(gp_Parab());
    this->myCurve = p;
}

GeomParabola::GeomParabola(const Handle_Geom_Parabola& p)
{
    this->myCurve = p;
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
    Handle_Geom_Line c = new Geom_Line(gp_Lin());
    this->myCurve = c;
}

GeomLine::GeomLine(const Handle_Geom_Line& c)
{
    this->myCurve = c;
}

GeomLine::~GeomLine()
{
}

Handle_Geom_Geometry GeomLine::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomLineSegment::GeomLineSegment()
{
    gp_Lin line;
    line.SetLocation(gp_Pnt(0.0,0.0,0.0));
    line.SetDirection(gp_Dir(0.0,0.0,1.0));
    Handle_Geom_Line c = new Geom_Line(line);
    this->myCurve = new Geom_TrimmedCurve(c, 0.0,1.0);
}

GeomLineSegment::~GeomLineSegment()
{
}

Handle_Geom_Geometry GeomLineSegment::handle() const
{
    return myCurve;
}

// -------------------------------------------------

GeomOffsetCurve::GeomOffsetCurve()
{
}

GeomOffsetCurve::GeomOffsetCurve(const Handle_Geom_Curve& c, double offset, const gp_Dir& dir)
{
    this->myCurve = new Geom_OffsetCurve(c, offset, dir);
}

GeomOffsetCurve::GeomOffsetCurve(const Handle_Geom_OffsetCurve& c)
{
    this->myCurve = c;
}

GeomOffsetCurve::~GeomOffsetCurve()
{
}

void GeomOffsetCurve::setHandle(const Handle_Geom_OffsetCurve& c)
{
    this->myCurve = c;
}

Handle_Geom_Geometry GeomOffsetCurve::handle() const
{
    return this->myCurve;
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
    TColgp_Array2OfPnt poles(1,2,1,2);
    poles(1,1) = gp_Pnt(0.0,0.0,0.0);
    poles(2,1) = gp_Pnt(1.0,0.0,0.0);
    poles(1,2) = gp_Pnt(0.0,1.0,0.0);
    poles(2,2) = gp_Pnt(1.0,1.0,0.0);
    this->mySurface = new Geom_BezierSurface(poles);
}

GeomBezierSurface::GeomBezierSurface(const Handle_Geom_BezierSurface& b)
{
    this->mySurface = b;
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
    TColgp_Array2OfPnt poles(1,2,1,2);
    poles(1,1) = gp_Pnt(0.0,0.0,0.0);
    poles(2,1) = gp_Pnt(1.0,0.0,0.0);
    poles(1,2) = gp_Pnt(0.0,1.0,0.0);
    poles(2,2) = gp_Pnt(1.0,1.0,0.0);

    TColStd_Array1OfReal knots(1,2);
    knots(1) = 0.0;
    knots(2) = 1.0;

    TColStd_Array1OfInteger mults(1,2);
    mults(1) = 2;
    mults(2) = 2;

    this->mySurface = new Geom_BSplineSurface(poles, knots, knots, mults, mults, 1, 1);
}

GeomBSplineSurface::GeomBSplineSurface(const Handle_Geom_BSplineSurface& b)
{
    this->mySurface = b;
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
    Handle_Geom_CylindricalSurface s = new Geom_CylindricalSurface(gp_Cylinder());
    this->mySurface = s;
}

GeomCylinder::~GeomCylinder()
{
}

Handle_Geom_Geometry GeomCylinder::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomCone::GeomCone()
{
    Handle_Geom_ConicalSurface s = new Geom_ConicalSurface(gp_Cone());
    this->mySurface = s;
}

GeomCone::~GeomCone()
{
}

Handle_Geom_Geometry GeomCone::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomToroid::GeomToroid()
{
    Handle_Geom_ToroidalSurface s = new Geom_ToroidalSurface(gp_Torus());
    this->mySurface = s;
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
    Handle_Geom_SphericalSurface s = new Geom_SphericalSurface(gp_Sphere());
    this->mySurface = s;
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
    Handle_Geom_Plane s = new Geom_Plane(gp_Pln());
    this->mySurface = s;
}

GeomPlane::~GeomPlane()
{
}

Handle_Geom_Geometry GeomPlane::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomOffsetSurface::GeomOffsetSurface()
{
}

GeomOffsetSurface::GeomOffsetSurface(const Handle_Geom_Surface& s, double offset)
{
    this->mySurface = new Geom_OffsetSurface(s, offset);
}

GeomOffsetSurface::GeomOffsetSurface(const Handle_Geom_OffsetSurface& s)
{
    this->mySurface = s;
}

GeomOffsetSurface::~GeomOffsetSurface()
{
}

void GeomOffsetSurface::setHandle(const Handle_Geom_OffsetSurface& s)
{
    mySurface = s;
}

Handle_Geom_Geometry GeomOffsetSurface::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomSurfaceOfRevolution::GeomSurfaceOfRevolution()
{
}

GeomSurfaceOfRevolution::GeomSurfaceOfRevolution(const Handle_Geom_Curve& c, const gp_Ax1& a)
{
    this->mySurface = new Geom_SurfaceOfRevolution(c,a);
}

GeomSurfaceOfRevolution::GeomSurfaceOfRevolution(const Handle_Geom_SurfaceOfRevolution& s)
{
    this->mySurface = s;
}

GeomSurfaceOfRevolution::~GeomSurfaceOfRevolution()
{
}

void GeomSurfaceOfRevolution::setHandle(const Handle_Geom_SurfaceOfRevolution& c)
{
    mySurface = c;
}

Handle_Geom_Geometry GeomSurfaceOfRevolution::handle() const
{
    return mySurface;
}

// -------------------------------------------------

GeomSurfaceOfExtrusion::GeomSurfaceOfExtrusion()
{
}

GeomSurfaceOfExtrusion::GeomSurfaceOfExtrusion(const Handle_Geom_Curve& c, const gp_Dir& d)
{
    this->mySurface = new Geom_SurfaceOfLinearExtrusion(c,d);
}

GeomSurfaceOfExtrusion::GeomSurfaceOfExtrusion(const Handle_Geom_SurfaceOfLinearExtrusion& s)
{
    this->mySurface = s;
}

GeomSurfaceOfExtrusion::~GeomSurfaceOfExtrusion()
{
}

void GeomSurfaceOfExtrusion::setHandle(const Handle_Geom_SurfaceOfLinearExtrusion& c)
{
    mySurface = c;
}

Handle_Geom_Geometry GeomSurfaceOfExtrusion::handle() const
{
    return mySurface;
}

