/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <BRepLib.hxx>
# include <BRepPrimAPI_MakeCone.hxx>
# include <BRepPrimAPI_MakeCylinder.hxx>
# include <BRepPrimAPI_MakeSphere.hxx>
# include <BRepPrimAPI_MakeTorus.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <BRepBuilderAPI_MakeWire.hxx>
# include <BRepBuilderAPI_GTransform.hxx>
# include <gp_GTrsf.hxx>
# include <GCE2d_MakeSegment.hxx>
# include <Geom_Plane.hxx>
# include <Geom_CylindricalSurface.hxx>
# include <Geom2d_Line.hxx>
# include <Geom2d_TrimmedCurve.hxx>
# include <Handle_Geom_Plane.hxx>
# include <Handle_Geom_CylindricalSurface.hxx>
# include <Handle_Geom2d_Line.hxx>
# include <Handle_Geom2d_TrimmedCurve.hxx>
# include <Precision.hxx>
# include <Standard_Real.hxx>
# include <TopoDS_Solid.hxx>
#endif


#include "PrimitiveFeature.h"


namespace Part {
    const App::PropertyFloatConstraint::Constraints floatRange = {0.0f,1000.0f,0.1f};
    const App::PropertyFloatConstraint::Constraints angleRangeU = {0.0f,360.0f,1.0f};
    const App::PropertyFloatConstraint::Constraints angleRangeV = {-90.0f,90.0f,1.0f};
    const App::PropertyFloatConstraint::Constraints torusRangeV = {-180.0f,180.0f,1.0f};
}

using namespace Part;


PROPERTY_SOURCE_ABSTRACT(Part::Primitive, Part::Feature)

Primitive::Primitive(void) 
{
}

Primitive::~Primitive()
{
}

short Primitive::mustExecute(void) const
{
    return Feature::mustExecute();
}

void Primitive::onChanged(const App::Property* prop)
{
    if (!isRestoring()) {
        // Do not support sphere, ellipsoid and torus because the creation
        // takes too long and thus is not feasible
        std::string grp = (prop->getGroup() ? prop->getGroup() : "");
        if (grp == "Plane" || grp == "Cylinder" || grp == "Cone"){
            try {
                App::DocumentObjectExecReturn *ret = recompute();
                delete ret;
            }
            catch (...) {
            }
        }
    }
    Part::Feature::onChanged(prop);
}

PROPERTY_SOURCE(Part::Plane, Part::Primitive)

Plane::Plane()
{
    ADD_PROPERTY_TYPE(Length,(100.0f),"Plane",App::Prop_None,"The length of the plane");
    ADD_PROPERTY_TYPE(Width ,(100.0f),"Plane",App::Prop_None,"The width of the plane");
}

short Plane::mustExecute() const
{
    if (Length.isTouched() ||
        Width.isTouched() )
        return 1;
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Plane::execute(void)
{
    double L = this->Length.getValue();
    double W = this->Width.getValue();

    if (L < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Length of plane too small");
    if (W < Precision::Confusion())
      return new App::DocumentObjectExecReturn("Width of plane too small");

    gp_Pnt pnt(0.0,0.0,0.0);
    gp_Dir dir(0.0,0.0,1.0);
    Handle_Geom_Plane aPlane = new Geom_Plane(pnt, dir);
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
#if OCC_HEX_VERSION < 0x060500
    case BRepBuilderAPI_SurfaceNotC2:
        error = "surface not C2";
        break;
#endif
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

PROPERTY_SOURCE(Part::Sphere, Part::Primitive)

Sphere::Sphere(void)
{
    ADD_PROPERTY_TYPE(Radius,(5.0),"Sphere",App::Prop_None,"The radius of the sphere");
    Radius.setConstraints(&floatRange);
    ADD_PROPERTY_TYPE(Angle1,(-90.0f),"Sphere",App::Prop_None,"The angle of the sphere");
    Angle1.setConstraints(&angleRangeV);
    ADD_PROPERTY_TYPE(Angle2,(90.0f),"Sphere",App::Prop_None,"The angle of the sphere");
    Angle2.setConstraints(&angleRangeV);
    ADD_PROPERTY_TYPE(Angle3,(360.0f),"Sphere",App::Prop_None,"The angle of the sphere");
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
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Sphere::execute(void)
{
    // Build a sphere
    try {
        BRepPrimAPI_MakeSphere mkSphere(Radius.getValue(),
                                        Angle1.getValue()/180.0f*Standard_PI,
                                        Angle2.getValue()/180.0f*Standard_PI,
                                        Angle3.getValue()/180.0f*Standard_PI);
        TopoDS_Shape ResultShape = mkSphere.Shape();
        this->Shape.setValue(ResultShape);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Ellipsoid, Part::Primitive)

Ellipsoid::Ellipsoid(void)
{
    ADD_PROPERTY_TYPE(Radius1,(2.0),"Ellipsoid",App::Prop_None,"The radius of the ellipsoid");
    Radius1.setConstraints(&floatRange);
    ADD_PROPERTY_TYPE(Radius2,(4.0),"Ellipsoid",App::Prop_None,"The radius of the ellipsoid");
    Radius2.setConstraints(&floatRange);
    ADD_PROPERTY_TYPE(Angle1,(-90.0f),"Ellipsoid",App::Prop_None,"The angle of the ellipsoid");
    Angle1.setConstraints(&angleRangeV);
    ADD_PROPERTY_TYPE(Angle2,(90.0f),"Ellipsoid",App::Prop_None,"The angle of the ellipsoid");
    Angle2.setConstraints(&angleRangeV);
    ADD_PROPERTY_TYPE(Angle3,(360.0f),"Ellipsoid",App::Prop_None,"The angle of the ellipsoid");
    Angle3.setConstraints(&angleRangeU);
}

short Ellipsoid::mustExecute() const
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
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Ellipsoid::execute(void)
{
    // Build a sphere
    try {
        gp_Pnt pnt(0.0,0.0,0.0);
        gp_Dir dir(0.0,0.0,1.0);
        gp_Ax2 ax2(pnt,dir);
        BRepPrimAPI_MakeSphere mkSphere(ax2,
                                        Radius2.getValue(), 
                                        Angle1.getValue()/180.0f*Standard_PI,
                                        Angle2.getValue()/180.0f*Standard_PI,
                                        Angle3.getValue()/180.0f*Standard_PI);
        Standard_Real scale = Radius1.getValue()/Radius2.getValue();
        gp_Dir xDir = ax2.XDirection();
        gp_Dir yDir = ax2.YDirection();
        gp_GTrsf mat;
        mat.SetValue(1,1,xDir.X());
        mat.SetValue(2,1,xDir.Y());
        mat.SetValue(3,1,xDir.Z());
        mat.SetValue(1,2,yDir.X());
        mat.SetValue(2,2,yDir.Y());
        mat.SetValue(3,2,yDir.Z());
        mat.SetValue(1,3,dir.X()*scale);
        mat.SetValue(2,3,dir.Y()*scale);
        mat.SetValue(3,3,dir.Z()*scale);
        BRepBuilderAPI_GTransform mkTrsf(mkSphere.Shape(), mat);
        TopoDS_Shape ResultShape = mkTrsf.Shape();
        this->Shape.setValue(ResultShape);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Cylinder, Part::Primitive)

Cylinder::Cylinder(void)
{
    ADD_PROPERTY_TYPE(Radius,(2.0),"Cylinder",App::Prop_None,"The radius of the cylinder");
    ADD_PROPERTY_TYPE(Height,(10.0f),"Cylinder",App::Prop_None,"The height of the cylinder");
    ADD_PROPERTY_TYPE(Angle,(360.0f),"Cylinder",App::Prop_None,"The angle of the cylinder");
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
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Cylinder::execute(void)
{
    // Build a cylinder
    try {
        BRepPrimAPI_MakeCylinder mkCylr(Radius.getValue(),
                                        Height.getValue(),
                                        Angle.getValue()/180.0f*Standard_PI);
        TopoDS_Shape ResultShape = mkCylr.Shape();
        this->Shape.setValue(ResultShape);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Cone, Part::Primitive)

Cone::Cone(void)
{
    ADD_PROPERTY_TYPE(Radius1,(2.0),"Cone",App::Prop_None,"The radius of the cone");
    ADD_PROPERTY_TYPE(Radius2,(4.0),"Cone",App::Prop_None,"The radius of the cone");
    ADD_PROPERTY_TYPE(Height,(10.0),"Cone",App::Prop_None,"The height of the cone");
    ADD_PROPERTY_TYPE(Angle,(360.0),"Cone",App::Prop_None,"The angle of the cone");
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
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Cone::execute(void)
{
    try {
        // Build a cone
        BRepPrimAPI_MakeCone mkCone(Radius1.getValue(),
                                    Radius2.getValue(),
                                    Height.getValue(),
                                    Angle.getValue()/180.0f*Standard_PI);
        TopoDS_Shape ResultShape = mkCone.Shape();
        this->Shape.setValue(ResultShape);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Torus, Part::Primitive)

Torus::Torus(void)
{
    ADD_PROPERTY_TYPE(Radius1,(10.0),"Torus",App::Prop_None,"The radius of the torus");
    Radius1.setConstraints(&floatRange);
    ADD_PROPERTY_TYPE(Radius2,(2.0),"Torus",App::Prop_None,"The radius of the torus");
    Radius2.setConstraints(&floatRange);
    ADD_PROPERTY_TYPE(Angle1,(-180.0),"Torus",App::Prop_None,"The angle of the torus");
    Angle1.setConstraints(&torusRangeV);
    ADD_PROPERTY_TYPE(Angle2,(180.0),"Torus",App::Prop_None,"The angle of the torus");
    Angle2.setConstraints(&torusRangeV);
    ADD_PROPERTY_TYPE(Angle3,(360.0),"Torus",App::Prop_None,"The angle of the torus");
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
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Torus::execute(void)
{
    try {
        // Build a torus
        BRepPrimAPI_MakeTorus mkTorus(Radius1.getValue(),
                                      Radius2.getValue(),
                                      Angle1.getValue()/180.0f*Standard_PI,
                                      Angle2.getValue()/180.0f*Standard_PI,
                                      Angle3.getValue()/180.0f*Standard_PI);
        const TopoDS_Solid& ResultShape = mkTorus.Solid();
        this->Shape.setValue(ResultShape);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Part::Helix, Part::Primitive)

Helix::Helix(void)
{
    ADD_PROPERTY_TYPE(Pitch, (1.0),"Helix",App::Prop_None,"The pitch of the helix");
    Pitch.setConstraints(&floatRange);
    ADD_PROPERTY_TYPE(Height,(2.0),"Helix",App::Prop_None,"The height of the helix");
    Height.setConstraints(&floatRange);
    ADD_PROPERTY_TYPE(Radius,(1.0),"Helix",App::Prop_None,"The radius of the helix");
    Radius.setConstraints(&floatRange);
}

short Helix::mustExecute() const
{
    if (Pitch.isTouched())
        return 1;
    if (Height.isTouched())
        return 1;
    if (Radius.isTouched())
        return 1;
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Helix::execute(void)
{
    try {
        Standard_Real myPitch  = Pitch.getValue();
        Standard_Real myHeight = Height.getValue();
        Standard_Real myRadius = Radius.getValue();

        if (myPitch < Precision::Confusion())
            return new App::DocumentObjectExecReturn("Pitch of helix too small");

        if (myHeight < Precision::Confusion())
            return new App::DocumentObjectExecReturn("Height of helix too small");

        if (myRadius < Precision::Confusion())
            return new App::DocumentObjectExecReturn("Radius of helix too small");

        gp_Ax2 cylAx2(gp_Pnt(0.0,0.0,0.0) , gp::DZ());
        Handle(Geom_CylindricalSurface) cyl = new Geom_CylindricalSurface(cylAx2 , myRadius);

        gp_Pnt2d aPnt(0, 0);
        gp_Dir2d aDir(2. * PI, myPitch);
        gp_Ax2d aAx2d(aPnt, aDir);

        Handle(Geom2d_Line) line = new Geom2d_Line(aAx2d);
        gp_Pnt2d beg = line->Value(0);
        gp_Pnt2d end = line->Value(2.0*PI*(myHeight/myPitch));
        Handle(Geom2d_TrimmedCurve) segm = GCE2d_MakeSegment(beg , end);

        TopoDS_Edge edgeOnSurf = BRepBuilderAPI_MakeEdge(segm , cyl);
        TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edgeOnSurf);
        BRepLib::BuildCurves3d(wire);
        this->Shape.setValue(wire);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}
