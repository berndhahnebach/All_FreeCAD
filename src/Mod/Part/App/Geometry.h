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
#include <Handle_Geom_TrimmedCurve.hxx>
#include <Handle_Geom_Surface.hxx>
#include <Handle_Geom_BezierSurface.hxx>
#include <Handle_Geom_BSplineSurface.hxx>
#include <Handle_Geom_CylindricalSurface.hxx>
#include <Handle_Geom_SphericalSurface.hxx>
#include <Handle_Geom_ToroidalSurface.hxx>
#include <Handle_Geom_Plane.hxx>
#include <TopoDS_Shape.hxx>

namespace Part {

class Geometry
{
public:
    virtual ~Geometry();

    virtual TopoDS_Shape toShape() const = 0;
    virtual Handle_Geom_Geometry handle() const = 0;

protected:
    Geometry();
};

class GeomCurve : public Geometry
{
public:
    GeomCurve();
    virtual ~GeomCurve();

    TopoDS_Shape toShape() const;
};

class GeomBezierCurve : public GeomCurve
{
public:
    GeomBezierCurve();
    virtual ~GeomBezierCurve();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BezierCurve myCurve;
};

class GeomBSplineCurve : public GeomCurve
{
public:
    GeomBSplineCurve();
    virtual ~GeomBSplineCurve();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BSplineCurve myCurve;
};

class GeomCircle : public GeomCurve
{
public:
    GeomCircle();
    GeomCircle(const Handle_Geom_Circle&);
    virtual ~GeomCircle();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Circle myCurve;
};

class GeomEllipse : public GeomCurve
{
public:
    GeomEllipse();
    virtual ~GeomEllipse();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Ellipse myCurve;
};

class GeomHyperbola : public GeomCurve
{
public:
    GeomHyperbola();
    virtual ~GeomHyperbola();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Hyperbola myCurve;
};

class GeomParabola : public GeomCurve
{
public:
    GeomParabola();
    virtual ~GeomParabola();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Parabola myCurve;
};

class GeomLine : public GeomCurve
{
public:
    GeomLine();
    virtual ~GeomLine();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Line myCurve;
};

class GeomTrimmedCurve : public GeomCurve
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

class GeomSurface : public Geometry
{
public:
    GeomSurface();
    virtual ~GeomSurface();

    TopoDS_Shape toShape() const;
};

class GeomBezierSurface : public GeomSurface
{
public:
    GeomBezierSurface();
    virtual ~GeomBezierSurface();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BezierSurface mySurface;
};

class GeomBSplineSurface : public GeomSurface
{
public:
    GeomBSplineSurface();
    virtual ~GeomBSplineSurface();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_BSplineSurface mySurface;
};

class GeomCylinder : public GeomSurface
{
public:
    GeomCylinder();
    virtual ~GeomCylinder();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_CylindricalSurface mySurface;
};

class GeomSphere : public GeomSurface
{
public:
    GeomSphere();
    virtual ~GeomSphere();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_SphericalSurface mySurface;
};

class GeomToroid : public GeomSurface
{
public:
    GeomToroid();
    virtual ~GeomToroid();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_ToroidalSurface mySurface;
};

class GeomPlane : public GeomSurface
{
public:
    GeomPlane();
    virtual ~GeomPlane();

    Handle_Geom_Geometry handle() const;

private:
    Handle_Geom_Plane mySurface;
};

}

#endif // PART_GEOMETRY_H
