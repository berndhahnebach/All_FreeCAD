/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <BRepBuilderAPI_MakeFace.hxx>
# include <BRepPrimAPI_MakeCone.hxx>
# include <BRepPrimAPI_MakeCylinder.hxx>
# include <BRepPrimAPI_MakeSphere.hxx>
# include <BRepPrimAPI_MakeTorus.hxx>
# include <Geom_Plane.hxx>
# include <Handle_Geom_Plane.hxx>
# include <Precision.hxx>
# include <Standard_Real.hxx>
#endif


#include "PrimitiveFeature.h"


namespace Part {
  const App::PropertyFloatConstraint::Constraints floatRange = {0.0f,1000.0f,0.1f};
  const App::PropertyFloatConstraint::Constraints angleRangeU = {0.0f,360.0f,1.0f};
  const App::PropertyFloatConstraint::Constraints angleRangeV = {-90.0f,90.0f,1.0f};
  const App::PropertyFloatConstraint::Constraints torusRangeV = {-180.0f,180.0f,1.0f};
}

using namespace Part;


PROPERTY_SOURCE(Part::Plane, Part::Feature)

Plane::Plane()
{
    ADD_PROPERTY(Length,(100.0));
    ADD_PROPERTY(Width,(100.0));
}

short Plane::mustExecute() const
{
    if (Length.isTouched() ||
        Width.isTouched() )
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Plane::execute(void)
{
    double L = this->Length.getValue();
    double W = this->Width.getValue();

    if (L < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Length of plane too small");
    if (W < Precision::Confusion())
      return new App::DocumentObjectExecReturn("Width of plane too small");

    gp_Pnt aPlanePnt(0,0,0);
    gp_Dir aPlaneDir(0,0,1);
    Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
    BRepBuilderAPI_MakeFace mkFace(aPlane, 0.0, L, 0.0, W);

    const char *error=0;
    switch (mkFace.Error())
    {
    case BRepBuilderAPI_FaceDone:
        break; // ok
    case BRepBuilderAPI_NoFace:
        error = "no face";
        break;
    case BRepBuilderAPI_NotPlanar:
        error = "not planar";
        break;
    case BRepBuilderAPI_CurveProjectionFailed:
        break;
    case BRepBuilderAPI_ParametersOutOfRange:
        error = "parameters out of range";
        break;
    case BRepBuilderAPI_SurfaceNotC2:
        error = "surface not C2";
        break;
    default:
        error = "unknown error";
        break;
    }
    // Error ?
    if (error) {
        return new App::DocumentObjectExecReturn(error);
    }

    TopoDS_Shape ResultShape = mkFace.Shape();
    this->Shape.setValue(ResultShape);

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Sphere, Part::Feature)

Sphere::Sphere(void)
{
    ADD_PROPERTY(Radius  ,(5.0));
    Radius.setConstraints(&floatRange);
    ADD_PROPERTY(Angle1  ,(-90.0f));
    Angle1.setConstraints(&angleRangeV);
    ADD_PROPERTY(Angle2  ,(90.0f));
    Angle2.setConstraints(&angleRangeV);
    ADD_PROPERTY(Angle3  ,(360.0f));
    Angle3.setConstraints(&angleRangeU);
}

short Sphere::mustExecute() const
{
    if (Radius.isTouched())
        return 1;
    if (Angle1.isTouched())
        return 1;
    if (Angle2.isTouched())
        return 1;
    if (Angle3.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Sphere::execute(void)
{
    // Build a sphere
    BRepPrimAPI_MakeSphere mkSphere(Radius.getValue(), 
                                    Angle1.getValue()/180.0f*Standard_PI,
                                    Angle2.getValue()/180.0f*Standard_PI,
                                    Angle3.getValue()/180.0f*Standard_PI);
    TopoDS_Shape ResultShape = mkSphere.Shape();
    this->Shape.setValue(ResultShape);

    return App::DocumentObject::StdReturn;
}


PROPERTY_SOURCE(Part::Cylinder, Part::Feature)

Cylinder::Cylinder(void)
{
    ADD_PROPERTY(Radius  ,(2.0));
    ADD_PROPERTY(Height  ,(10.0));
    ADD_PROPERTY(Angle   ,(360.0f));
    Angle.setConstraints(&angleRangeU);
}

short Cylinder::mustExecute() const
{
    if (Radius.isTouched())
        return 1;
    if (Height.isTouched())
        return 1;
    if (Angle.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Cylinder::execute(void)
{
    // Build a cylinder
    BRepPrimAPI_MakeCylinder mkCylr(Radius.getValue(), 
                                    Height.getValue(),
                                    Angle.getValue()/180.0f*Standard_PI);
    TopoDS_Shape ResultShape = mkCylr.Shape();
    this->Shape.setValue(ResultShape);

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Cone, Part::Feature)

Cone::Cone(void)
{
    ADD_PROPERTY(Radius1  ,(2.0));
    ADD_PROPERTY(Radius2  ,(4.0));
    ADD_PROPERTY(Height  ,(10.0));
    ADD_PROPERTY(Angle   ,(360.0f));
    Angle.setConstraints(&angleRangeU);
}

short Cone::mustExecute() const
{
    if (Radius1.isTouched())
        return 1;
    if (Radius2.isTouched())
        return 1;
    if (Height.isTouched())
        return 1;
    if (Angle.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Cone::execute(void)
{
    // Build a cone
    BRepPrimAPI_MakeCone mkCone(Radius1.getValue(),
                                Radius2.getValue(),
                                Height.getValue(),
                                Angle.getValue()/180.0f*Standard_PI);
    TopoDS_Shape ResultShape = mkCone.Shape();
    this->Shape.setValue(ResultShape);

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Torus, Part::Feature)

Torus::Torus(void)
{
    ADD_PROPERTY(Radius1  ,(10.0));
    Radius1.setConstraints(&floatRange);
    ADD_PROPERTY(Radius2  ,(2.0));
    Radius2.setConstraints(&floatRange);
    ADD_PROPERTY(Angle1  ,(-180.0f));
    Angle1.setConstraints(&torusRangeV);
    ADD_PROPERTY(Angle2  ,(180.0f));
    Angle2.setConstraints(&torusRangeV);
    ADD_PROPERTY(Angle3  ,(360.0f));
    Angle3.setConstraints(&angleRangeU);
}

short Torus::mustExecute() const
{
    if (Radius1.isTouched())
        return 1;
    if (Radius2.isTouched())
        return 1;
    if (Angle1.isTouched())
        return 1;
    if (Angle2.isTouched())
        return 1;
    if (Angle3.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Torus::execute(void)
{
    // Build a torus
    BRepPrimAPI_MakeTorus mkTorus(Radius1.getValue(),
                                  Radius2.getValue(),
                                  Angle1.getValue()/180.0f*Standard_PI,
                                  Angle2.getValue()/180.0f*Standard_PI,
                                  Angle3.getValue()/180.0f*Standard_PI);
    TopoDS_Shape ResultShape = mkTorus.Shell();
    this->Shape.setValue(ResultShape);

    return App::DocumentObject::StdReturn;
}

