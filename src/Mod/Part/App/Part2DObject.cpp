/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
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
# include <TopoDS_Shape.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS.hxx>
# include <gp_Pln.hxx>
# include <gp_Ax1.hxx>
# include <gp_Pnt.hxx>
# include <gp_Dir.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <Geom_Plane.hxx>
# include <BRepAdaptor_Surface.hxx>
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif



#include "Part2DObject.h"


using namespace Part;


PROPERTY_SOURCE(Part::Part2DObject, Part::Feature)


Part2DObject::Part2DObject()
{
     ADD_PROPERTY_TYPE(Support,(0),   "2D",(App::PropertyType)(App::Prop_None),"Support of the 2D geometry");
}


App::DocumentObjectExecReturn *Part2DObject::execute(void)
{

    return App::DocumentObject::StdReturn;
}

Base::Placement Part2DObject::positionBySupport(const TopoDS_Face &face,const Base::Placement &Place)
{
    if (face.IsNull())
        throw Base::Exception("Null Face in Part2DObject::positionBySupport()!");

    bool Reverse = false;
    if (face.Orientation() == TopAbs_REVERSED) 
        Reverse = true;

    BRepAdaptor_Surface adapt(face);

    if (adapt.GetType() != GeomAbs_Plane)
        throw Base::Exception("No planar Face in Part2DObject::positionBySupport()!");

    gp_Pnt ObjOrg(Place.getPosition().x,Place.getPosition().y,Place.getPosition().z);
    gp_Pln plane = adapt.Plane();
    Standard_Boolean ok = plane.Direct();
    if (!ok) {
        // toggle if plane has a left-handed coordinate system
        plane.UReverse();
        Reverse = !Reverse;
    }

    gp_Ax1 Normal = plane.Axis();
    if (Reverse)
        Normal.Reverse();

    Handle (Geom_Plane) gPlane = new Geom_Plane(plane);
    GeomAPI_ProjectPointOnSurf projector(ObjOrg,gPlane);

    gp_Pnt SketchBasePoint = projector.NearestPoint();

    // check the angle against the Y Axis
    Standard_Real a = Normal.Angle(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,1,0)));

    gp_Ax3 SketchPos;
    if (fabs(a)<0.1 || fabs((fabs(a)-M_PI))< 0.1)
        SketchPos = gp_Ax3(SketchBasePoint,Normal._CSFDB_Getgp_Ax1vdir(),gp_Dir(1,0,0));
    else
        SketchPos = gp_Ax3(SketchBasePoint,Normal._CSFDB_Getgp_Ax1vdir());

    gp_Trsf Trf;
    Trf.SetTransformation(SketchPos);
    Trf.Invert();

    Base::Matrix4D mtrx;

    gp_Mat m = Trf._CSFDB_Getgp_Trsfmatrix();
    gp_XYZ p = Trf._CSFDB_Getgp_Trsfloc();
    Standard_Real scale = 1.0;

    // set Rotation matrix
    mtrx[0][0] = scale * m._CSFDB_Getgp_Matmatrix(0,0);
    mtrx[0][1] = scale * m._CSFDB_Getgp_Matmatrix(0,1);
    mtrx[0][2] = scale * m._CSFDB_Getgp_Matmatrix(0,2);

    mtrx[1][0] = scale * m._CSFDB_Getgp_Matmatrix(1,0);
    mtrx[1][1] = scale * m._CSFDB_Getgp_Matmatrix(1,1);
    mtrx[1][2] = scale * m._CSFDB_Getgp_Matmatrix(1,2);

    mtrx[2][0] = scale * m._CSFDB_Getgp_Matmatrix(2,0);
    mtrx[2][1] = scale * m._CSFDB_Getgp_Matmatrix(2,1);
    mtrx[2][2] = scale * m._CSFDB_Getgp_Matmatrix(2,2);

    // set pos vector
    mtrx[0][3] = p._CSFDB_Getgp_XYZx();
    mtrx[1][3] = p._CSFDB_Getgp_XYZy();
    mtrx[2][3] = p._CSFDB_Getgp_XYZz();

    // check the angle against the Z Axis
    //Standard_Real a = Normal.Angle(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1)));


    return Base::Placement(mtrx);
}

// Python Drawing feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
  PROPERTY_SOURCE_TEMPLATE(Part::Part2DObjectPython, Part::Part2DObject)
  template<> const char* Part::Part2DObjectPython::getViewProviderName(void) const {
    return "PartGui::ViewProvider2DObjectPython";
}
/// @endcond

// explicit template instantiation
  template class AppPartExport FeaturePythonT<Part::Part2DObject>;
}

