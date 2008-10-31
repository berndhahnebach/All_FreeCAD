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


#ifndef PART_GEOMETRY_H
#define PART_GEOMETRY_H

#include <Handle_Geom_BezierCurve.hxx>
#include <Handle_Geom_BSplineCurve.hxx>
#include <Handle_Geom_Circle.hxx>
#include <Handle_Geom_Ellipse.hxx>
#include <Handle_Geom_Hyperbola.hxx>
#include <Handle_Geom_Parabola.hxx>
#include <Handle_Geom_Line.hxx>
#include <Handle_Geom_OffsetCurve.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <Handle_Geom_Surface.hxx>
#include <Handle_Geom_BezierSurface.hxx>
#include <Handle_Geom_BSplineSurface.hxx>
#include <Handle_Geom_CylindricalSurface.hxx>
#include <Handle_Geom_ConicalSurface.hxx>
#include <Handle_Geom_SphericalSurface.hxx>
#include <Handle_Geom_ToroidalSurface.hxx>
#include <Handle_Geom_Plane.hxx>
#include <Handle_Geom_OffsetSurface.hxx>
#include <Handle_Geom_SurfaceOfRevolution.hxx>
#include <Handle_Geom_SurfaceOfLinearExtrusion.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>

namespace Part {

class PartExport Geometry
{
public:
    virtual ~Geometry();

    virtual TopoDS_Shape toShape() const = 0;
    virtual Handle_Geom_Geometry handle() const = 0;

protected:
    Geometry();
};

class PartExport GeomCurve : public Geometry
{
public:
    GeomCurve();
    virtual ~GeomCurve();

    TopoDS_Shape toShape() const;
};

class PartExport GeomBezierCurve : public GeomCurve
{
public:
    GeomBezierCurve();
    GeomBezierCurve(const Handle_Geom_BezierCurve&);
    virtual ~GeomBezierCurve();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BezierCurve myCurve;
};

class PartExport GeomBSplineCurve : public GeomCurve
{
public:
    GeomBSplineCurve();
    GeomBSplineCurve(const Handle_Geom_BSplineCurve&);
    virtual ~GeomBSplineCurve();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BSplineCurve myCurve;
};

class PartExport GeomCircle : public GeomCurve
{
public:
    GeomCircle();
    GeomCircle(const Handle_Geom_Circle&);
    virtual ~GeomCircle();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Circle myCurve;
};

class PartExport GeomEllipse : public GeomCurve
{
public:
    GeomEllipse();
    GeomEllipse(const Handle_Geom_Ellipse&);
    virtual ~GeomEllipse();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Ellipse myCurve;
};

class PartExport GeomHyperbola : public GeomCurve
{
public:
    GeomHyperbola();
    GeomHyperbola(const Handle_Geom_Hyperbola&);
    virtual ~GeomHyperbola();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Hyperbola myCurve;
};

class PartExport GeomParabola : public GeomCurve
{
public:
    GeomParabola();
    GeomParabola(const Handle_Geom_Parabola&);
    virtual ~GeomParabola();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Parabola myCurve;
};

class PartExport GeomLine : public GeomCurve
{
public:
    GeomLine();
    GeomLine(const Handle_Geom_Line&);
    virtual ~GeomLine();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Line myCurve;
};

class PartExport GeomLineSegment : public GeomCurve
{
public:
    GeomLineSegment();
    virtual ~GeomLineSegment();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_TrimmedCurve myCurve;
};

class PartExport GeomOffsetCurve : public GeomCurve
{
public:
    GeomOffsetCurve();
    GeomOffsetCurve(const Handle_Geom_Curve&, double, const gp_Dir&);
    GeomOffsetCurve(const Handle_Geom_OffsetCurve&);
    virtual ~GeomOffsetCurve();

    void setHandle(const Handle_Geom_OffsetCurve& c);
    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_OffsetCurve myCurve;
};

class PartExport GeomTrimmedCurve : public GeomCurve
{
public:
    GeomTrimmedCurve();
    GeomTrimmedCurve(const Handle_Geom_TrimmedCurve&);
    virtual ~GeomTrimmedCurve();

    void setHandle(const Handle_Geom_TrimmedCurve&);
    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_TrimmedCurve myCurve;
};

class PartExport GeomSurface : public Geometry
{
public:
    GeomSurface();
    virtual ~GeomSurface();

    TopoDS_Shape toShape() const;
};

class PartExport GeomBezierSurface : public GeomSurface
{
public:
    GeomBezierSurface();
    GeomBezierSurface(const Handle_Geom_BezierSurface&);
    virtual ~GeomBezierSurface();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BezierSurface mySurface;
};

class PartExport GeomBSplineSurface : public GeomSurface
{
public:
    GeomBSplineSurface();
    GeomBSplineSurface(const Handle_Geom_BSplineSurface&);
    virtual ~GeomBSplineSurface();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BSplineSurface mySurface;
};

class PartExport GeomCylinder : public GeomSurface
{
public:
    GeomCylinder();
    virtual ~GeomCylinder();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_CylindricalSurface mySurface;
};

class PartExport GeomCone : public GeomSurface
{
public:
    GeomCone();
    virtual ~GeomCone();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_ConicalSurface mySurface;
};

class PartExport GeomSphere : public GeomSurface
{
public:
    GeomSphere();
    virtual ~GeomSphere();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_SphericalSurface mySurface;
};

class PartExport GeomToroid : public GeomSurface
{
public:
    GeomToroid();
    virtual ~GeomToroid();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_ToroidalSurface mySurface;
};

class PartExport GeomPlane : public GeomSurface
{
public:
    GeomPlane();
    virtual ~GeomPlane();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Plane mySurface;
};

class GeomOffsetSurface : public GeomSurface
{
public:
    GeomOffsetSurface();
    GeomOffsetSurface(const Handle_Geom_Surface&, double);
    GeomOffsetSurface(const Handle_Geom_OffsetSurface&);
    virtual ~GeomOffsetSurface();

    void setHandle(const Handle_Geom_OffsetSurface& s);
    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_OffsetSurface mySurface;
};

class GeomSurfaceOfRevolution : public GeomSurface
{
public:
    GeomSurfaceOfRevolution(const Handle_Geom_Curve&, const gp_Ax1&);
    GeomSurfaceOfRevolution(const Handle_Geom_SurfaceOfRevolution&);
    virtual ~GeomSurfaceOfRevolution();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_SurfaceOfRevolution mySurface;
};

class GeomSurfaceOfExtrusion : public GeomSurface
{
public:
    GeomSurfaceOfExtrusion(const Handle_Geom_Curve&, const gp_Dir&);
    GeomSurfaceOfExtrusion(const Handle_Geom_SurfaceOfLinearExtrusion&);
    virtual ~GeomSurfaceOfExtrusion();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_SurfaceOfLinearExtrusion mySurface;
};

}

#endif // PART_GEOMETRY_H
