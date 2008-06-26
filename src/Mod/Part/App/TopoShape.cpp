/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <sstream>
# include <BRep_Builder.hxx>
# include <BRepAdaptor_Curve.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <BRepAlgoAPI_Common.hxx>
# include <BRepAlgoAPI_Cut.hxx>
# include <BRepAlgoAPI_Fuse.hxx>
# include <BRepAlgoAPI_Section.hxx>
# include <BRepCheck_Analyzer.hxx>
# include <BRepBndLib.hxx>
# include <Bnd_Box.hxx>
# include <BRepTools.hxx>
# include <BRepTools_ShapeSet.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <BRepBuilderAPI_Transform.hxx>
# include <Handle_TopTools_HSequenceOfShape.hxx>
# include <TopTools_HSequenceOfShape.hxx>
# include <IGESControl_Controller.hxx>
# include <IGESControl_Writer.hxx>
# include <IGESControl_Reader.hxx>
# include <STEPControl_Writer.hxx>
# include <STEPControl_Reader.hxx>
# include <TopTools_MapOfShape.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Iterator.hxx>
# include <TopExp.hxx>
# include <Geom_BezierCurve.hxx>
# include <Geom_BezierSurface.hxx>
# include <Geom_BSplineCurve.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Geom_SurfaceOfLinearExtrusion.hxx>
# include <Geom_SurfaceOfRevolution.hxx>
# include <Geom_Circle.hxx>
# include <Geom_ConicalSurface.hxx>
# include <Geom_CylindricalSurface.hxx>
# include <Geom_Ellipse.hxx>
# include <Geom_Hyperbola.hxx>
# include <Geom_Line.hxx>
# include <Geom_Parabola.hxx>
# include <Geom_Plane.hxx>
# include <Geom_CartesianPoint.hxx>
# include <Geom_SphericalSurface.hxx>
# include <Geom_ToroidalSurface.hxx>
# include <Standard_Failure.hxx>
# include <StlAPI_Writer.hxx>
#endif

#include <Base/FileInfo.h>
#include <Base/Exception.h>

#include "TopoShape.h"

using namespace Part;

TYPESYSTEM_SOURCE(Part::TopoShape , Data::ComplexGeoData);

TopoShape::TopoShape()
{
}

TopoShape::TopoShape(const TopoDS_Shape& shape)
  : _Shape(shape)
{
}

TopoShape::TopoShape(const TopoShape& shape)
  : _Shape(shape._Shape)
{
}

TopoShape::~TopoShape()
{
}

void TopoShape::operator = (const TopoShape& sh)
{
    if (this != &sh) {
        this->_Shape = sh._Shape;
    }
}

void TopoShape::setTransform(const Base::Matrix4D& rclTrf)
{
    gp_Trsf mov;
    mov.SetValues(rclTrf[0][0],rclTrf[0][1],rclTrf[0][2],rclTrf[0][3],
                  rclTrf[1][0],rclTrf[1][1],rclTrf[1][2],rclTrf[1][3],
                  rclTrf[2][0],rclTrf[2][1],rclTrf[2][2],rclTrf[2][3],
                  0.00001,0.00001);
    mov.SetScaleFactor(rclTrf[3][3]);

    TopLoc_Location loc(mov);
    _Shape.Location(loc);
}

Base::Matrix4D TopoShape::getTransform(void) const
{
    Base::Matrix4D mtrx;
    gp_Trsf Trf = _Shape.Location().Transformation();

    gp_Mat m = Trf._CSFDB_Getgp_Trsfmatrix();
    gp_XYZ p = Trf._CSFDB_Getgp_Trsfloc();

    // set Rotation matrix
    mtrx[0][0] = m._CSFDB_Getgp_Matmatrix(0,0);
    mtrx[0][1] = m._CSFDB_Getgp_Matmatrix(0,1);
    mtrx[0][2] = m._CSFDB_Getgp_Matmatrix(0,2);

    mtrx[1][0] = m._CSFDB_Getgp_Matmatrix(1,0);
    mtrx[1][1] = m._CSFDB_Getgp_Matmatrix(1,1);
    mtrx[1][2] = m._CSFDB_Getgp_Matmatrix(1,2);

    mtrx[2][0] = m._CSFDB_Getgp_Matmatrix(2,0);
    mtrx[2][1] = m._CSFDB_Getgp_Matmatrix(2,1);
    mtrx[2][2] = m._CSFDB_Getgp_Matmatrix(2,2);

    // set pos vector
    mtrx[0][3] = p._CSFDB_Getgp_XYZx();
    mtrx[1][3] = p._CSFDB_Getgp_XYZy();
    mtrx[2][3] = p._CSFDB_Getgp_XYZz();

    // and the rest
    mtrx[3][0] = 0.0;
    mtrx[3][1] = 0.0;
    mtrx[3][2] = 0.0;
    mtrx[3][3] = Trf._CSFDB_Getgp_Trsfscale();

    return mtrx;
}

void TopoShape::read(const char *FileName)
{
    Base::FileInfo File(FileName);
  
    // checking on the file
    if (!File.isReadable())
        throw Base::Exception("File to load not existing or not readable");
    
    TopoDS_Shape aShape;

    if (File.hasExtension("igs") || File.hasExtension("iges")) {
        // read iges file
        IGESControl_Reader aReader;

        if (aReader.ReadFile((const Standard_CString)File.filePath().c_str()) != IFSelect_RetDone)
            throw Base::Exception("Error in reading IGES");
  
        // make brep
        aReader.TransferRoots();
        // one shape, who contain's all subshapes
        aShape = aReader.OneShape();
    }
    else if (File.hasExtension("stp") || File.hasExtension("step")) {
        STEPControl_Reader aReader;
        Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
        if (aReader.ReadFile((const Standard_CString)File.filePath().c_str()) != IFSelect_RetDone)
            throw Base::Exception("Error in reading STEP");

        // Root transfers
        Standard_Integer nbr = aReader.NbRootsForTransfer();
        //aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
        for (Standard_Integer n = 1; n<= nbr; n++) {
            printf("STEP: Transfering Root %d\n",n);
            /*Standard_Boolean ok =*/ aReader.TransferRoot(n);
            // Collecting resulting entities
            Standard_Integer nbs = aReader.NbShapes();
            if (nbs == 0) {
                aHSequenceOfShape.Nullify();
                throw Base::Exception("nothing to read");
            }
            else {
                for (Standard_Integer i =1; i<=nbs; i++) {
                    printf("STEP:   Transfering Shape %d\n",n);
                    aShape=aReader.Shape(i);
                    aHSequenceOfShape->Append(aShape);
                }
            }
        }
    }
    else if (File.hasExtension("brp") || File.hasExtension("brep")) {
        // read brep-file
        BRep_Builder aBuilder;
        BRepTools::Read(aShape,(const Standard_CString)File.filePath().c_str(),aBuilder);
    }
    else{
        throw Base::Exception("Unknown extension");
    }

    _Shape = aShape; 
}

bool TopoShape::exportIges(const char *filename) const
{
    // write iges file
    IGESControl_Controller::Init();
    IGESControl_Writer aWriter;
    aWriter.AddShape(this->_Shape);

    if (aWriter.Write((const Standard_CString)filename) != IFSelect_RetDone)
        return false;

    return true;
}

bool TopoShape::exportStep(const char *filename) const
{
    // write step file
    STEPControl_Writer aWriter;

    //FIXME: Does not work this way!!!
    if (aWriter.Transfer(this->_Shape, STEPControl_AsIs))
        return false;

    if (aWriter.Write((const Standard_CString)filename) != IFSelect_RetDone)
        return false;

    return true;
}

bool TopoShape::exportBrep(const char *filename) const
{
    if (!BRepTools::Write(this->_Shape,(const Standard_CString)filename))
        return false;
    return true;
}

bool TopoShape::exportStl(const char *filename) const
{
    StlAPI_Writer writer;
    writer.Write(this->_Shape,(const Standard_CString)filename);
    return true;
}

Base::BoundBox3d TopoShape::getBoundBox(void) const
{
    Base::BoundBox3d box;
    try {
        // If the shape is empty an exception may be thrown
        Bnd_Box bounds;
        BRepBndLib::Add(_Shape, bounds);
        bounds.SetGap(0.0);
        Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
        bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);

        box.MinX = xMin;
        box.MaxX = xMax;
        box.MinY = yMin;
        box.MaxY = yMax;
        box.MinZ = zMin;
        box.MaxZ = zMax;
    }
    catch (Standard_Failure) {
    }

    return box;
}

void TopoShape::Save (Base::Writer & writer) const
{
}

void TopoShape::Restore(Base::XMLReader &reader)
{
}

void TopoShape::SaveDocFile (Base::Writer &writer) const
{
}

void TopoShape::RestoreDocFile(Base::Reader &reader)
{
}

unsigned int TopoShape_RefCountShapes(const TopoDS_Shape& aShape)
{
    unsigned int size = 1; // this shape
    TopoDS_Iterator it;
    // go through all direct children
    for (it.Initialize(aShape, false, false);it.More(); it.Next()) {
        size += TopoShape_RefCountShapes(it.Value());
    }

    return size;
}

unsigned int TopoShape::getMemSize (void) const
{
    if (!_Shape.IsNull()) {
        // Count total amount of references of TopoDS_Shape objects
        unsigned int memsize = (sizeof(TopoDS_Shape)+sizeof(TopoDS_TShape)) * TopoShape_RefCountShapes(_Shape);

        // Now get a map of TopoDS_Shape objects without duplicates
        TopTools_IndexedMapOfShape M;
        TopExp::MapShapes(_Shape, M);
        for (int i=0; i<M.Extent(); i++) {
            const TopoDS_Shape& shape = M(i+1);
            // add the size of the underlying geomtric data
            Handle(TopoDS_TShape) tshape = shape.TShape();
            memsize += tshape->DynamicType()->Size();

            switch (shape.ShapeType())
            {
            case TopAbs_FACE:
                {
                    // first, last, tolerance
                    memsize += 5*sizeof(Standard_Real);
                    TopoDS_Face face = TopoDS::Face(shape);
                    BRepAdaptor_Surface surface(face);
                    switch (surface.GetType())
                    {
                    case GeomAbs_Plane:
                        memsize += sizeof(Geom_Plane);
                        break;
                    case GeomAbs_Cylinder:
                        memsize += sizeof(Geom_CylindricalSurface);
                        break;
                    case GeomAbs_Cone:
                        memsize += sizeof(Geom_ConicalSurface);
                        break;
                    case GeomAbs_Sphere:
                        memsize += sizeof(Geom_SphericalSurface);
                        break;
                    case GeomAbs_Torus:
                        memsize += sizeof(Geom_ToroidalSurface);
                        break;
                    case GeomAbs_BezierSurface:
                        memsize += sizeof(Geom_BezierSurface);
                        memsize += (surface.NbUKnots()+surface.NbVKnots()) * sizeof(Standard_Real);
                        memsize += (surface.NbUPoles()*surface.NbVPoles()) * sizeof(Geom_CartesianPoint);
                        break;
                    case GeomAbs_BSplineSurface:
                        memsize += sizeof(Geom_BSplineSurface);
                        memsize += (surface.NbUKnots()+surface.NbVKnots()) * sizeof(Standard_Real);
                        memsize += (surface.NbUPoles()*surface.NbVPoles()) * sizeof(Geom_CartesianPoint);
                        break;
                    case GeomAbs_SurfaceOfRevolution:
                        memsize += sizeof(Geom_SurfaceOfRevolution);
                        break;
                    case GeomAbs_SurfaceOfExtrusion:
                        memsize += sizeof(Geom_SurfaceOfLinearExtrusion);
                        break;
                    case GeomAbs_OtherSurface:
                        // What kind of surface should this be?
                        memsize += sizeof(Geom_Surface);
                        break;
                    default:
                        break;
                    }
                } break;
            case TopAbs_EDGE:
                {
                    // first, last, tolerance
                    memsize += 3*sizeof(Standard_Real);
                    TopoDS_Edge edge = TopoDS::Edge(shape);
                    BRepAdaptor_Curve curve(edge);
                    switch (curve.GetType())
                    {
                    case GeomAbs_Line:
                        memsize += sizeof(Geom_Line);
                        break;
                    case GeomAbs_Circle:
                        memsize += sizeof(Geom_Circle);
                        break;
                    case GeomAbs_Ellipse:
                        memsize += sizeof(Geom_Ellipse);
                        break;
                    case GeomAbs_Hyperbola:
                        memsize += sizeof(Geom_Hyperbola);
                        break;
                    case GeomAbs_Parabola:
                        memsize += sizeof(Geom_Parabola);
                        break;
                    case GeomAbs_BezierCurve:
                        memsize += sizeof(Geom_BezierCurve);
                        memsize += curve.NbKnots() * sizeof(Standard_Real);
                        memsize += curve.NbPoles() * sizeof(Geom_CartesianPoint);
                        break;
                    case GeomAbs_BSplineCurve:
                        memsize += sizeof(Geom_BSplineCurve);
                        memsize += curve.NbKnots() * sizeof(Standard_Real);
                        memsize += curve.NbPoles() * sizeof(Geom_CartesianPoint);
                        break;
                    case GeomAbs_OtherCurve:
                        // What kind of curve should this be?
                        memsize += sizeof(Geom_Curve);
                        break;
                    default:
                        break;
                    }
                } break;
            case TopAbs_VERTEX:
                {
                    // tolerance
                    memsize += sizeof(Standard_Real);
                    memsize += sizeof(Geom_CartesianPoint);
                } break;
            default:
                break;
            }
        }

        // estimated memory usage
        return memsize;
    }

    // in case the shape is invalid
    return sizeof(TopoDS_Shape);
}

bool TopoShape::isNull() const
{
    return this->_Shape.IsNull() ? true : false;
}

bool TopoShape::isValid() const
{
    BRepCheck_Analyzer aChecker(this->_Shape);
    return aChecker.IsValid() ? true : false;
}

TopoDS_Shape TopoShape::cut(TopoDS_Shape shape) const
{
    BRepAlgoAPI_Cut mkCut(this->_Shape, shape);
    return mkCut.Shape();
}

TopoDS_Shape TopoShape::common(TopoDS_Shape shape) const
{
    BRepAlgoAPI_Common mkCommon(this->_Shape, shape);
    return mkCommon.Shape();
}

TopoDS_Shape TopoShape::fuse(TopoDS_Shape shape) const
{
    BRepAlgoAPI_Fuse mkFuse(this->_Shape, shape);
    return mkFuse.Shape();
}

TopoDS_Shape TopoShape::section(TopoDS_Shape shape) const
{
    BRepAlgoAPI_Section mkSection(this->_Shape, shape);
    return mkSection.Shape();
}

TopoDS_Shape TopoShape::transform(const Base::Matrix4D& rclTrf) const
{
    gp_Trsf mov;
    mov.SetValues(rclTrf[0][0],rclTrf[0][1],rclTrf[0][2],rclTrf[0][3],
                  rclTrf[1][0],rclTrf[1][1],rclTrf[1][2],rclTrf[1][3],
                  rclTrf[2][0],rclTrf[2][1],rclTrf[2][2],rclTrf[2][3],
                  0.00001,0.00001);
    mov.SetScaleFactor(rclTrf[3][3]);
    BRepBuilderAPI_Transform mkTrf(this->_Shape, mov);
    return mkTrf.Shape();
}
