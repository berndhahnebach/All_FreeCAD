/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <GeomLProp_CLProps.hxx>
# include <GeomLProp_SLProps.hxx>
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
# include <gp.hxx>
# include <gp_Lin.hxx>
# include <Geom_Line.hxx>
# include <Geom_TrimmedCurve.hxx>
# include <GC_MakeLine.hxx>
# include <GC_MakeSegment.hxx>
# include <Precision.hxx>

#endif

#include <Base/Exception.h>

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
TYPESYSTEM_SOURCE_ABSTRACT(Part::Geometry,Base::Persistence);

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

// -------------------------------------------------
TYPESYSTEM_SOURCE_ABSTRACT(Part::GeomCurve,Part::Geometry);

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

bool GeomCurve::tangent(double u, gp_Dir& dir) const
{
    Handle_Geom_Curve c = Handle_Geom_Curve::DownCast(handle());
    GeomLProp_CLProps prop(c,u,1,Precision::Confusion());
    if (prop.IsTangentDefined()) {
        prop.Tangent(dir);
        return true;
    }

    return false;
}


// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomBezierCurve,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomBezierCurve::handle() const
{
    return myCurve;
}

// Persistence implementer 
unsigned int GeomBezierCurve::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomBezierCurve::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomBezierCurve::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomBSplineCurve,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomBSplineCurve::handle() const
{
    return myCurve;
}

// Persistence implementer 
unsigned int GeomBSplineCurve::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomBSplineCurve::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomBSplineCurve::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomCircle,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomCircle::handle() const
{
    return myCurve;
}

// Persistence implementer 
unsigned int GeomCircle::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomCircle::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomCircle::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomEllipse,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomEllipse::handle() const
{
    return myCurve;
}

// Persistence implementer 
unsigned int GeomEllipse::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomEllipse::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomEllipse::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomHyperbola,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomHyperbola::handle() const
{
    return myCurve;
}

// Persistence implementer 
unsigned int GeomHyperbola::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomHyperbola::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomHyperbola::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomParabola,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomParabola::handle() const
{
    return myCurve;
}

// Persistence implementer 
unsigned int GeomParabola::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomParabola::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomParabola::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomLine,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomLine::handle() const
{
    return myCurve;
}


// Persistence implementer 
unsigned int GeomLine::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomLine::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomLine::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomLineSegment,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomLineSegment::handle() const
{
    return myCurve;
}

Base::Vector3d GeomLineSegment::getStartPoint(void)
{
    Handle_Geom_TrimmedCurve this_curve = Handle_Geom_TrimmedCurve::DownCast(handle());
    gp_Pnt pnt = this_curve->StartPoint();
    return Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z());
}

Base::Vector3d GeomLineSegment::getEndPoint(void)
{
    Handle_Geom_TrimmedCurve this_curve = Handle_Geom_TrimmedCurve::DownCast(handle());
    gp_Pnt pnt = this_curve->EndPoint();
    return Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z());
}

void GeomLineSegment::setPoints(Base::Vector3d Start,Base::Vector3d End)
{
    gp_Pnt p1(Start.x,Start.y,Start.z), p2(End.x,End.y,End.z);
    Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast(handle());

    try {
        // Create line out of two points
        if (p1.Distance(p2) < Precision::Confusion()) Standard_Failure::Raise("Both points are equal");
        GC_MakeSegment ms(p1, p2);
        if (!ms.IsDone()) {
            throw Base::Exception(gce_ErrorStatusText(ms.Status()));
        }

        // get Geom_Line of line segment
        Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
            (this_curv->BasisCurve());
        Handle_Geom_TrimmedCurve that_curv = ms.Value();
        Handle_Geom_Line that_line = Handle_Geom_Line::DownCast(that_curv->BasisCurve());
        this_line->SetLin(that_line->Lin());
        this_curv->SetTrim(that_curv->FirstParameter(), that_curv->LastParameter());
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        throw Base::Exception(e->GetMessageString());
    }

}


// Persistence implementer 
unsigned int GeomLineSegment::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomLineSegment::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomLineSegment::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomOffsetCurve,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomOffsetCurve::handle() const
{
    return this->myCurve;
}

// Persistence implementer 
unsigned int GeomOffsetCurve::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomOffsetCurve::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomOffsetCurve::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomTrimmedCurve,Part::GeomCurve);

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

const Handle_Geom_Geometry& GeomTrimmedCurve::handle() const
{
    return myCurve;
}

// Persistence implementer 
unsigned int GeomTrimmedCurve::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomTrimmedCurve::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomTrimmedCurve::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE_ABSTRACT(Part::GeomSurface,Part::Geometry);

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

bool GeomSurface::tangentU(double u, double v, gp_Dir& dirU) const
{
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(handle());
    GeomLProp_SLProps prop(s,u,v,1,Precision::Confusion());
    if (prop.IsTangentUDefined()) {
        prop.TangentU(dirU);
        return true;
    }

    return false;
}

bool GeomSurface::tangentV(double u, double v, gp_Dir& dirV) const
{
    Handle_Geom_Surface s = Handle_Geom_Surface::DownCast(handle());
    GeomLProp_SLProps prop(s,u,v,1,Precision::Confusion());
    if (prop.IsTangentVDefined()) {
        prop.TangentV(dirV);
        return true;
    }

    return false;
}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomBezierSurface,Part::GeomSurface);

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

const Handle_Geom_Geometry& GeomBezierSurface::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomBezierSurface::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomBezierSurface::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomBezierSurface::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomBSplineSurface,Part::GeomSurface);

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

const Handle_Geom_Geometry& GeomBSplineSurface::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomBSplineSurface::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomBSplineSurface::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomBSplineSurface::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomCylinder,Part::GeomSurface);

GeomCylinder::GeomCylinder()
{
    Handle_Geom_CylindricalSurface s = new Geom_CylindricalSurface(gp_Cylinder());
    this->mySurface = s;
}

GeomCylinder::~GeomCylinder()
{
}

const Handle_Geom_Geometry& GeomCylinder::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomCylinder::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomCylinder::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomCylinder::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomCone,Part::GeomSurface);

GeomCone::GeomCone()
{
    Handle_Geom_ConicalSurface s = new Geom_ConicalSurface(gp_Cone());
    this->mySurface = s;
}

GeomCone::~GeomCone()
{
}

const Handle_Geom_Geometry& GeomCone::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomCone::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomCone::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomCone::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomToroid,Part::GeomSurface);

GeomToroid::GeomToroid()
{
    Handle_Geom_ToroidalSurface s = new Geom_ToroidalSurface(gp_Torus());
    this->mySurface = s;
}

GeomToroid::~GeomToroid()
{
}

const Handle_Geom_Geometry& GeomToroid::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomToroid::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomToroid::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomToroid::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomSphere,Part::GeomSurface);

GeomSphere::GeomSphere()
{
    Handle_Geom_SphericalSurface s = new Geom_SphericalSurface(gp_Sphere());
    this->mySurface = s;
}

GeomSphere::~GeomSphere()
{
}

const Handle_Geom_Geometry& GeomSphere::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomSphere::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomSphere::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomSphere::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomPlane,Part::GeomSurface);

GeomPlane::GeomPlane()
{
    Handle_Geom_Plane s = new Geom_Plane(gp_Pln());
    this->mySurface = s;
}

GeomPlane::~GeomPlane()
{
}

const Handle_Geom_Geometry& GeomPlane::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomPlane::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomPlane::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomPlane::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomOffsetSurface,Part::GeomSurface);

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

const Handle_Geom_Geometry& GeomOffsetSurface::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomOffsetSurface::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomOffsetSurface::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomOffsetSurface::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomSurfaceOfRevolution,Part::GeomSurface);

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

const Handle_Geom_Geometry& GeomSurfaceOfRevolution::handle() const
{
    return mySurface;
}

// Persistence implementer 
unsigned int GeomSurfaceOfRevolution::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomSurfaceOfRevolution::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomSurfaceOfRevolution::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}

// -------------------------------------------------

TYPESYSTEM_SOURCE(Part::GeomSurfaceOfExtrusion,Part::GeomSurface);

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

const Handle_Geom_Geometry& GeomSurfaceOfExtrusion::handle() const
{
    return mySurface;
}


// Persistence implementer 
unsigned int GeomSurfaceOfExtrusion::getMemSize (void) const               {assert(0); return 0;/* not implemented yet */}
void         GeomSurfaceOfExtrusion::Save       (Writer &/*writer*/) const {assert(0);          /* not implemented yet */}
void         GeomSurfaceOfExtrusion::Restore    (XMLReader &/*reader*/)    {assert(0);          /* not implemented yet */}
