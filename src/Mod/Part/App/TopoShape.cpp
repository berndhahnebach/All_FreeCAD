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
# include <cstdlib>
# include <sstream>
# include <Bnd_Box.hxx>
# include <BRep_Builder.hxx>
# include <BRep_Tool.hxx>
# include <BRepAdaptor_Curve.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <BRepAlgoAPI_Common.hxx>
# include <BRepAlgoAPI_Cut.hxx>
# include <BRepAlgoAPI_Fuse.hxx>
# include <BRepAlgoAPI_Section.hxx>
# include <BRepBndLib.hxx>
# include <BRepBuilderAPI_GTransform.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <BRepBuilderAPI_MakePolygon.hxx>
# include <BRepBuilderAPI_MakeVertex.hxx>
# include <BRepBuilderAPI_NurbsConvert.hxx>
# include <BRepBuilderAPI_FaceError.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <BRepBuilderAPI_Transform.hxx>
# include <BRepCheck_Analyzer.hxx>
# include <BRepFilletAPI_MakeFillet.hxx>
# include <BRepMesh.hxx>
# include <BRepMesh_IncrementalMesh.hxx>
# include <BRepMesh_Triangle.hxx>
# include <BRepMesh_Edge.hxx>
# include <BRepOffsetAPI_MakeThickSolid.hxx>
# include <BRepOffsetAPI_MakePipe.hxx>
# include <BRepOffsetAPI_MakePipeShell.hxx>
# include <BRepOffsetAPI_Sewing.hxx>
# include <BRepPrimAPI_MakePrism.hxx>
# include <BRepPrimAPI_MakeRevol.hxx>
# include <BRepTools.hxx>
# include <BRepTools_ReShape.hxx>
# include <BRepTools_ShapeSet.hxx>
# include <Handle_TopTools_HSequenceOfShape.hxx>
# include <TopTools_HSequenceOfShape.hxx>
# include <Interface_Static.hxx>
# include <IGESControl_Controller.hxx>
# include <IGESControl_Writer.hxx>
# include <IGESControl_Reader.hxx>
# include <STEPControl_Writer.hxx>
# include <STEPControl_Reader.hxx>
# include <TopTools_MapOfShape.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Compound.hxx>
# include <TopoDS_Iterator.hxx>
# include <TopoDS_Vertex.hxx>
# include <TopExp.hxx>
# include <TopExp_Explorer.hxx>
# include <TopTools_ListIteratorOfListOfShape.hxx>
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
# include <Poly_Triangulation.hxx>
# include <Standard_Failure.hxx>
# include <StlAPI_Writer.hxx>
# include <Standard_Failure.hxx>
# include <gp_GTrsf.hxx>
# include <ShapeAnalysis_Shell.hxx>
# include <ShapeBuild_ReShape.hxx>
# include <ShapeFix_Face.hxx>
# include <ShapeFix_Shell.hxx>
# include <ShapeFix_Solid.hxx>
# include <ShapeUpgrade_ShellSewing.hxx>
#endif
# include <Poly_Polygon3D.hxx>
# include <Poly_PolygonOnTriangulation.hxx>
# include <BRepMesh.hxx>
# include <BRepBuilderAPI_Sewing.hxx>
# include <ShapeFix_Shape.hxx>
# include <XSControl_WorkSession.hxx>
# include <Transfer_TransientProcess.hxx>
# include <APIHeaderSection_MakeHeader.hxx>

#include <Base/Builder3D.h>
#include <Base/FileInfo.h>
#include <Base/Exception.h>

#include "TopoShape.h"
#include "TopoShapeFacePy.h"
#include "TopoShapeEdgePy.h"
#include "TopoShapeVertexPy.h"
#include "ProgressIndicator.h"

using namespace Part;

const char* BRepBuilderAPI_FaceErrorText(BRepBuilderAPI_FaceError et)
{
    switch (et)
    {
    case BRepBuilderAPI_FaceDone:
        return "Construction was successful";
    case BRepBuilderAPI_NoFace:
        return "No face";
    case BRepBuilderAPI_NotPlanar:
        return "Face is not planar";
    case BRepBuilderAPI_CurveProjectionFailed:
        return "Curve projection failed";
    case BRepBuilderAPI_ParametersOutOfRange:
        return "Parameters out of range";
    case BRepBuilderAPI_SurfaceNotC2:
        return "Surface not C2-continous";
    default:
        return "Unknown creation error";
    }
}
// ------------------------------------------------

TYPESYSTEM_SOURCE(Part::ShapeSegment , Data::Segment);

std::string ShapeSegment::getName() const
{
    return std::string();
}

// ------------------------------------------------

TYPESYSTEM_SOURCE(Part::TopoShape , Data::ComplexGeoData);

TopoShape::TopoShape()
{
}

TopoShape::~TopoShape()
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

std::vector<const char*> TopoShape::getElementTypes(void) const
{
    std::vector<const char*> temp(3);
    temp.push_back("Vertex");
    temp.push_back("Edge");
    temp.push_back("Face");

    return temp;
}

Data::Segment* TopoShape::getSubElement(const char* Type, unsigned long n) const
{
    std::stringstream str;
    str << Type << n;
    std::string temp = str.str();
    return new ShapeSegment(getSubShape(temp.c_str()));
}

TopoDS_Shape TopoShape::getSubShape(const char* Type) const
{
    if (!Type) return TopoDS_Shape();
    std::string shapetype(Type);
    if (shapetype.size() > 4 && shapetype.substr(0,4) == "Face") {
        int index=std::atoi(&shapetype[4]);
        TopTools_IndexedMapOfShape anIndices;
        TopExp::MapShapes(this->_Shape, TopAbs_FACE, anIndices);
        return anIndices.FindKey(index);
    }
    else if (shapetype.size() > 4 && shapetype.substr(0,4) == "Edge") {
        int index=std::atoi(&shapetype[4]);
        TopTools_IndexedMapOfShape anIndices;
        TopExp::MapShapes(this->_Shape, TopAbs_EDGE, anIndices);
        return anIndices.FindKey(index);
    }
    else if (shapetype.size() > 6 && shapetype.substr(0,6) == "Vertex") {
        int index=std::atoi(&shapetype[6]);
        TopTools_IndexedMapOfShape anIndices;
        TopExp::MapShapes(this->_Shape, TopAbs_VERTEX, anIndices);
        return anIndices.FindKey(index);
    }

    return TopoDS_Shape();
}

PyObject * TopoShape::getPySubShape(const char* Type) const
{
    // get the shape
    TopoDS_Shape Shape = getSubShape(Type);
    // destinquish the return type
    std::string shapetype(Type);
    if (shapetype.size() > 4 && shapetype.substr(0,4) == "Face") 
        return new TopoShapeFacePy(new TopoShape(Shape));
    else if (shapetype.size() > 4 && shapetype.substr(0,4) == "Edge") 
        return new TopoShapeEdgePy(new TopoShape(Shape));
    else if (shapetype.size() > 6 && shapetype.substr(0,6) == "Vertex") 
        return new TopoShapeVertexPy(new TopoShape(Shape));
    else 
        return 0;

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
    TopLoc_Location loc(mov);
    _Shape.Location(loc);
}

Base::Matrix4D TopoShape::getTransform(void) const
{
    Base::Matrix4D mtrx;
    gp_Trsf Trf = _Shape.Location().Transformation();

    gp_Mat m = Trf._CSFDB_Getgp_Trsfmatrix();
    gp_XYZ p = Trf._CSFDB_Getgp_Trsfloc();
    Standard_Real scale = Trf._CSFDB_Getgp_Trsfscale();

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

    return mtrx;
}

void TopoShape::read(const char *FileName)
{
    Base::FileInfo File(FileName);
  
    // checking on the file
    if (!File.isReadable())
        throw Base::FileException("File to load not existing or not readable", FileName);
    
    if (File.hasExtension("igs") || File.hasExtension("iges")) {
        // read iges file
        importIges(File.filePath().c_str());
    }
    else if (File.hasExtension("stp") || File.hasExtension("step")) {
        importStep(File.filePath().c_str());
    }
    else if (File.hasExtension("brp") || File.hasExtension("brep")) {
        // read brep-file
        importBrep(File.filePath().c_str());
    }
    else{
        throw Base::Exception("Unknown extension");
    }
}

/*!
 Example code to get the labels for each face in an IGES file.
 \code
#include <Handle_XSControl_WorkSession.hxx>
#include <Handle_XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSControl_TransferReader.hxx>
#include <Handle_IGESData_IGESModel.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESEntity.hxx>

IGESControl_Reader aReader;
...
// Gets the labels of all face items if defined in the IGES file
Handle_XSControl_WorkSession ws = aReader.WS();
Handle_XSControl_TransferReader tr = ws->TransferReader();

std::string name;
Handle(IGESData_IGESModel) aModel = aReader.IGESModel();
Standard_Integer all = aModel->NbEntities();

TopExp_Explorer ex;
for (ex.Init(this->_Shape, TopAbs_FACE); ex.More(); ex.Next())
{
    const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
    Handle_Standard_Transient ent = tr->EntityFromShapeResult(aFace, 1);
    if (!ent.IsNull()) {
        int i = aModel->Number(ent);
        if (i > 0) {
            Handle_IGESData_IGESEntity ie = aModel->Entity(i);
            if (ie->HasShortLabel())
                name = ie->ShortLabel()->ToCString();
        }
    }
}
\endcode
*/
void TopoShape::importIges(const char *FileName)
{
    try {
        // read iges file
        IGESControl_Reader aReader;
        if (aReader.ReadFile((const Standard_CString)FileName) != IFSelect_RetDone)
            throw Base::Exception("Error in reading IGES");

        Handle_Message_ProgressIndicator pi = new ProgressIndicator(100);
        pi->NewScope(100, "Reading IGES file...");
        pi->Show();
        aReader.WS()->MapReader()->SetProgress(pi);

        // make brep
        aReader.ClearShapes();
        aReader.TransferRoots();
        // one shape that contains all subshapes
        this->_Shape = aReader.OneShape();
        pi->EndScope();
    }
    catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        throw Base::Exception(aFail->GetMessageString());
    }
}

void TopoShape::importStep(const char *FileName)
{
    try {
        STEPControl_Reader aReader;
        if (aReader.ReadFile((const Standard_CString)FileName) != IFSelect_RetDone)
            throw Base::Exception("Error in reading STEP");

        Handle_Message_ProgressIndicator pi = new ProgressIndicator(100);
        aReader.WS()->MapReader()->SetProgress(pi);
        pi->NewScope(100, "Reading STEP file...");
        pi->Show();

        // Root transfers
        aReader.TransferRoots();
        // one shape that contains all subshapes
        this->_Shape = aReader.OneShape();
        pi->EndScope();
    }
    catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        throw Base::Exception(aFail->GetMessageString());
    }
}

void TopoShape::importBrep(const char *FileName)
{
    try {
        // read brep-file
        BRep_Builder aBuilder;
        TopoDS_Shape aShape;
    #if OCC_HEX_VERSION >= 0x060300
        Handle_Message_ProgressIndicator pi = new ProgressIndicator(100);
        pi->NewScope(100, "Reading BREP file...");
        pi->Show();
        BRepTools::Read(aShape,(const Standard_CString)FileName,aBuilder,pi);
        pi->EndScope();
    #else
        BRepTools::Read(aShape,(const Standard_CString)FileName,aBuilder);
    #endif
        this->_Shape = aShape;
    }
    catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        throw Base::Exception(aFail->GetMessageString());
    }
}

void TopoShape::write(const char *FileName) const
{
    Base::FileInfo File(FileName);
    
    if (File.hasExtension("igs") || File.hasExtension("iges")) {
        // write iges file
        exportIges(File.filePath().c_str());
    }
    else if (File.hasExtension("stp") || File.hasExtension("step")) {
        exportStep(File.filePath().c_str());
    }
    else if (File.hasExtension("brp") || File.hasExtension("brep")) {
        // read brep-file
        exportBrep(File.filePath().c_str());
    }
    else if (File.hasExtension("stl")) {
        // read brep-file
        exportStl(File.filePath().c_str());
    }
    else{
        throw Base::Exception("Unknown extension");
    }
}

void TopoShape::exportIges(const char *filename) const
{
    Interface_Static::SetCVal("write.iges.unit","IN");
    try {
        // write iges file
        IGESControl_Controller::Init();
        IGESControl_Writer aWriter;
        //IGESControl_Writer aWriter(Interface_Static::CVal("write.iges.unit"), 1);
        aWriter.AddShape(this->_Shape);
        aWriter.ComputeModel();
        if (aWriter.Write((const Standard_CString)filename) != IFSelect_RetDone)
            throw Base::Exception("Writing of IGES failed");
    }
    catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        throw Base::Exception(aFail->GetMessageString());
    }
}

void TopoShape::exportStep(const char *filename) const
{
    try {
        // write step file
        STEPControl_Writer aWriter;

        Handle_Message_ProgressIndicator pi = new ProgressIndicator(100);
        aWriter.WS()->MapReader()->SetProgress(pi);
        pi->NewScope(100, "Writing STEP file...");
        pi->Show();

        if (aWriter.Transfer(this->_Shape, STEPControl_AsIs) != IFSelect_RetDone)
            throw Base::Exception("Error in transferring STEP");

        APIHeaderSection_MakeHeader makeHeader(aWriter.Model());
        makeHeader.SetName(new TCollection_HAsciiString(filename));
        makeHeader.SetAuthorValue (1, new TCollection_HAsciiString("FreeCAD"));
        makeHeader.SetOrganizationValue (1, new TCollection_HAsciiString("FreeCAD"));
        makeHeader.SetOriginatingSystem(new TCollection_HAsciiString("FreeCAD"));
        makeHeader.SetDescriptionValue(1, new TCollection_HAsciiString("FreeCAD Model"));

        if (aWriter.Write((const Standard_CString)filename) != IFSelect_RetDone)
            throw Base::Exception("Writing of STEP failed");
        pi->EndScope();
    }
    catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        throw Base::Exception(aFail->GetMessageString());
    }
}

void TopoShape::exportBrep(const char *filename) const
{
    if (!BRepTools::Write(this->_Shape,(const Standard_CString)filename))
        throw Base::Exception("Writing of BREP failed");
}

void TopoShape::exportStl(const char *filename) const
{
    StlAPI_Writer writer;
    //writer.RelativeMode() = false;
    //writer.SetDeflection(0.1);
    writer.Write(this->_Shape,(const Standard_CString)filename);
}

void TopoShape::exportFaceSet(double dev, double ca, std::ostream& str) const
{
    Base::InventorBuilder builder(str);
    TopExp_Explorer ex;

    BRepMesh_IncrementalMesh MESH(this->_Shape,dev);
    for (ex.Init(this->_Shape, TopAbs_FACE); ex.More(); ex.Next()) {
        // get the shape and mesh it
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        Standard_Integer nbNodesInFace,nbTriInFace;
        std::vector<Base::Vector3f> vertices;
        std::vector<int> indices;

        // doing the meshing and checking the result
        TopLoc_Location aLoc;
        Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
        if (aPoly.IsNull()) continue;

        // getting the transformation of the shape/face
        gp_Trsf myTransf;
        Standard_Boolean identity = true;
        if (!aLoc.IsIdentity()) {
            identity = false;
            myTransf = aLoc.Transformation();
        }

        // getting size and create the array
        nbNodesInFace = aPoly->NbNodes();
        nbTriInFace = aPoly->NbTriangles();
        vertices.resize(nbNodesInFace);
        indices.resize(4*nbTriInFace);

        // check orientation
        TopAbs_Orientation orient = aFace.Orientation();

        // cycling through the poly mesh
        const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
        const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
        for (int i=1;i<=nbTriInFace;i++) {
            // Get the triangle
            Standard_Integer N1,N2,N3;
            Triangles(i).Get(N1,N2,N3);

            // change orientation of the triangles
            if (orient != TopAbs_FORWARD) {
                Standard_Integer tmp = N1;
                N1 = N2;
                N2 = tmp;
            }

            gp_Pnt V1 = Nodes(N1);
            gp_Pnt V2 = Nodes(N2);
            gp_Pnt V3 = Nodes(N3);

            // transform the vertices to the place of the face
            if (!identity) {
                V1.Transform(myTransf);
                V2.Transform(myTransf);
                V3.Transform(myTransf);
            }

            vertices[N1-1].Set((float)(V1.X()),(float)(V1.Y()),(float)(V1.Z()));
            vertices[N2-1].Set((float)(V2.X()),(float)(V2.Y()),(float)(V2.Z()));
            vertices[N3-1].Set((float)(V3.X()),(float)(V3.Y()),(float)(V3.Z()));

            int j = i - 1;
            N1--; N2--; N3--;
            indices[4*j] = N1; indices[4*j+1] = N2; indices[4*j+2] = N3; indices[4*j+3] = -1;
        }

        builder.addIndexedFaceSet(vertices, indices, (float)ca);
    } // end of face loop
}

void TopoShape::exportLineSet(std::ostream& str) const
{
    Base::InventorBuilder builder(str);
    // get a indexed map of edges
    TopTools_IndexedMapOfShape M;
    TopExp::MapShapes(this->_Shape, TopAbs_EDGE, M);

    // build up map edge->face
    TopTools_IndexedDataMapOfShapeListOfShape edge2Face;
    TopExp::MapShapesAndAncestors(this->_Shape, TopAbs_EDGE, TopAbs_FACE, edge2Face);
    for (int i=0; i<M.Extent(); i++)
    {
        const TopoDS_Edge& aEdge = TopoDS::Edge(M(i+1));
        gp_Trsf myTransf;
        Standard_Boolean identity = true;
        TopLoc_Location aLoc;

        // try to triangulate the edge
        Handle(Poly_Polygon3D) aPoly = BRep_Tool::Polygon3D(aEdge, aLoc);

        std::vector<Base::Vector3f> vertices;
        Standard_Integer nbNodesInFace;

        // triangulation succeeded?
        if (!aPoly.IsNull()) {
            if (!aLoc.IsIdentity()) {
                identity = false;
                myTransf = aLoc.Transformation();
            }
            nbNodesInFace = aPoly->NbNodes();
            vertices.resize(nbNodesInFace);

            const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();

            gp_Pnt V;
            for (Standard_Integer i=0;i < nbNodesInFace;i++) {
                V = Nodes(i+1);
                V.Transform(myTransf);
                vertices[i].Set((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
            }
        }
        else {
            // the edge has not its own triangulation, but then a face the edge is attached to
            // must provide this triangulation

            // Look for one face in our map (it doesn't care which one we take)
            const TopoDS_Face& aFace = TopoDS::Face(edge2Face.FindFromKey(aEdge).First());

            // take the face's triangulation instead
            Handle(Poly_Triangulation) aPolyTria = BRep_Tool::Triangulation(aFace,aLoc);
            if (!aLoc.IsIdentity()) {
                identity = false;
                myTransf = aLoc.Transformation();
            }

            if (aPolyTria.IsNull()) break;

            // this holds the indices of the edge's triangulation to the actual points
            Handle(Poly_PolygonOnTriangulation) aPoly = BRep_Tool::PolygonOnTriangulation(aEdge, aPolyTria, aLoc);
            if (aPoly.IsNull())
                continue; // polygon does not exist

            // getting size and create the array
            nbNodesInFace = aPoly->NbNodes();
            vertices.resize(nbNodesInFace);

            const TColStd_Array1OfInteger& indices = aPoly->Nodes();
            const TColgp_Array1OfPnt& Nodes = aPolyTria->Nodes();

            gp_Pnt V;
            int pos = 0;
            // go through the index array
            for (Standard_Integer i=indices.Lower();i <= indices.Upper();i++) {
                V = Nodes(indices(i));
                V.Transform(myTransf);
                vertices[pos++].Set((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
            }
        }

        builder.addLineSet(vertices, 2, 0, 0, 0);
    }
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
                    const TopoDS_Face& face = TopoDS::Face(shape);
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
                    const TopoDS_Edge& edge = TopoDS::Edge(shape);
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

bool TopoShape::isClosed() const
{
    return BRep_Tool::IsClosed(this->_Shape) ? true : false;
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

TopoDS_Shape TopoShape::makePipe(const TopoDS_Shape& profile) const
{
    if (this->_Shape.IsNull())
        Standard_Failure::Raise("Cannot sweep along empty spine");
    if (this->_Shape.ShapeType() != TopAbs_WIRE)
        Standard_Failure::Raise("Spine shape is not a wire");
    if (profile.IsNull())
        Standard_Failure::Raise("Cannot sweep empty profile");
    BRepOffsetAPI_MakePipe mkPipe(TopoDS::Wire(this->_Shape), profile);
    return mkPipe.Shape();
}

TopoDS_Shape TopoShape::makePipeShell(const TopTools_ListOfShape& profiles, const Standard_Boolean make_solid) const
{
    if (this->_Shape.IsNull())
        Standard_Failure::Raise("Cannot sweep along empty spine");
    if (this->_Shape.ShapeType() != TopAbs_WIRE)
        Standard_Failure::Raise("Spine shape is not a wire");

    BRepOffsetAPI_MakePipeShell mkPipeShell(TopoDS::Wire(this->_Shape));
    TopTools_ListIteratorOfListOfShape it;
    for (it.Initialize(profiles); it.More(); it.Next()) {
        mkPipeShell.Add(TopoDS_Shape(it.Value()));
    }

    if (!mkPipeShell.IsReady()) Standard_Failure::Raise("shape is not ready to build");
    else mkPipeShell.Build();

    if (make_solid)	mkPipeShell.MakeSolid();

    return mkPipeShell.Shape();
}

TopoDS_Shape TopoShape::makePrism(const gp_Vec& vec) const
{
    if (this->_Shape.IsNull()) Standard_Failure::Raise("cannot sweep empty shape");
    BRepPrimAPI_MakePrism mkPrism(this->_Shape, vec);
    return mkPrism.Shape();
}

TopoDS_Shape TopoShape::revolve(const gp_Ax1& axis, double d) const
{
    if (this->_Shape.IsNull()) Standard_Failure::Raise("cannot sweep empty shape");
    BRepPrimAPI_MakeRevol mkRevol(this->_Shape, axis,d);
    return mkRevol.Shape();
}

TopoDS_Shape TopoShape::makeThickSolid(const TopTools_ListOfShape& remFace,
                                       Standard_Real offset, Standard_Real tolerance) const
{
    BRepOffsetAPI_MakeThickSolid mkThick(this->_Shape, remFace, offset, tolerance);
    return mkThick.Shape();
}

TopoDS_Shape TopoShape::transformGeometry(const Base::Matrix4D& rclTrf) const
{
    // There is a strange behaviour of the gp_Trsf class if rclTrf has
    // a negative determinant.
    gp_GTrsf mat;
    if (rclTrf.determinant() < 0.0) {
        //mat.SetValues(-rclTrf[0][0],rclTrf[0][1],rclTrf[0][2],rclTrf[0][3],
        //              -rclTrf[1][0],rclTrf[1][1],rclTrf[1][2],rclTrf[1][3],
        //              -rclTrf[2][0],rclTrf[2][1],rclTrf[2][2],rclTrf[2][3],
        //              0.00001,0.00001);
        mat.SetValue(1,1,-rclTrf[0][0]);
        mat.SetValue(2,1,-rclTrf[1][0]);
        mat.SetValue(3,1,-rclTrf[2][0]);
        mat.SetValue(1,2,rclTrf[0][1]);
        mat.SetValue(2,2,rclTrf[1][1]);
        mat.SetValue(3,2,rclTrf[2][1]);
        mat.SetValue(1,3,rclTrf[0][2]);
        mat.SetValue(2,3,rclTrf[1][2]);
        mat.SetValue(3,3,rclTrf[2][2]);
        mat.SetValue(1,4,rclTrf[0][3]);
        mat.SetValue(2,4,rclTrf[1][3]);
        mat.SetValue(3,4,rclTrf[2][3]);
    }
    else {
        //mat.SetValues(rclTrf[0][0],rclTrf[0][1],rclTrf[0][2],rclTrf[0][3],
        //              rclTrf[1][0],rclTrf[1][1],rclTrf[1][2],rclTrf[1][3],
        //              rclTrf[2][0],rclTrf[2][1],rclTrf[2][2],rclTrf[2][3],
        //              0.00001,0.00001);
        mat.SetValue(1,1,rclTrf[0][0]);
        mat.SetValue(2,1,rclTrf[1][0]);
        mat.SetValue(3,1,rclTrf[2][0]);
        mat.SetValue(1,2,rclTrf[0][1]);
        mat.SetValue(2,2,rclTrf[1][1]);
        mat.SetValue(3,2,rclTrf[2][1]);
        mat.SetValue(1,3,rclTrf[0][2]);
        mat.SetValue(2,3,rclTrf[1][2]);
        mat.SetValue(3,3,rclTrf[2][2]);
        mat.SetValue(1,4,rclTrf[0][3]);
        mat.SetValue(2,4,rclTrf[1][3]);
        mat.SetValue(3,4,rclTrf[2][3]);
    }

    // geometric transformation
    BRepBuilderAPI_GTransform mkTrf(this->_Shape, mat);
    return mkTrf.Shape();
}

void TopoShape::transformShape(const Base::Matrix4D& rclTrf)
{
    // There is a strange behaviour of the gp_Trsf class if rclTrf has
    // a negative determinant.
    gp_Trsf mat;
    if (rclTrf.determinant() < 0.0) {
        mat.SetValues(-rclTrf[0][0],rclTrf[0][1],rclTrf[0][2],rclTrf[0][3],
                      -rclTrf[1][0],rclTrf[1][1],rclTrf[1][2],rclTrf[1][3],
                      -rclTrf[2][0],rclTrf[2][1],rclTrf[2][2],rclTrf[2][3],
                      0.00001,0.00001);
    }
    else {
        mat.SetValues(rclTrf[0][0],rclTrf[0][1],rclTrf[0][2],rclTrf[0][3],
                      rclTrf[1][0],rclTrf[1][1],rclTrf[1][2],rclTrf[1][3],
                      rclTrf[2][0],rclTrf[2][1],rclTrf[2][2],rclTrf[2][3],
                      0.00001,0.00001);
    }

    // location transformation
    BRepBuilderAPI_Transform mkTrf(this->_Shape, mat);
    this->_Shape = mkTrf.Shape();
}

TopoDS_Shape TopoShape::mirror(const gp_Ax2& ax2) const
{
    gp_Trsf mat;
    mat.SetMirror(ax2);
    TopLoc_Location loc = this->_Shape.Location();
    gp_Trsf placement = loc.Transformation();
    mat = placement * mat;
    BRepBuilderAPI_Transform mkTrf(this->_Shape, mat);
    return mkTrf.Shape();
}

TopoDS_Shape TopoShape::toNurbs() const
{
    BRepBuilderAPI_NurbsConvert mkNurbs(this->_Shape);
    return mkNurbs.Shape();
}

TopoDS_Shape TopoShape::replaceShape(const std::vector< std::pair<TopoDS_Shape,TopoDS_Shape> >& s) const
{
    BRepTools_ReShape reshape;
    std::vector< std::pair<TopoDS_Shape,TopoDS_Shape> >::const_iterator it;
    for (it = s.begin(); it != s.end(); ++it)
        reshape.Replace(it->first, it->second);
    return reshape.Apply(this->_Shape, TopAbs_SHAPE);
}

TopoDS_Shape TopoShape::removeShape(const std::vector<TopoDS_Shape>& s) const
{
    BRepTools_ReShape reshape;
    for (std::vector<TopoDS_Shape>::const_iterator it = s.begin(); it != s.end(); ++it)
        reshape.Remove(*it);
    return reshape.Apply(this->_Shape, TopAbs_SHAPE);
}

void TopoShape::sewShape()
{
    //ShapeFix_Shape fixer(this->_Shape);
    //fixer.Perform();

    BRepBuilderAPI_Sewing sew;
    sew.Add(this->_Shape/*fixer.Shape()*/);
    sew.Perform();

    //shape = ShapeUpgrade_ShellSewing().ApplySewing(shape);
    this->_Shape = sew.SewedShape();
}

bool TopoShape::fix()
{
    bool ret = false;
    if (this->_Shape.IsNull())
        return false;
    TopAbs_ShapeEnum type = this->_Shape.ShapeType();
    if (type == TopAbs_SOLID) {
        ShapeFix_Solid fix(TopoDS::Solid(this->_Shape));
        ret = fix.Perform() ? true : false;
        this->_Shape = fix.Shape();
    }
    else if (type == TopAbs_SHELL) {
        //ShapeAnalysis_Shell eval;
        //eval.LoadShells(this->_Shape);
        ShapeFix_Shell fix(TopoDS::Shell(this->_Shape));
        ret = fix.Perform() ? true : false;
        this->_Shape = fix.Shape();
    }
    else if (type == TopAbs_FACE) {
        ShapeFix_Face fix(TopoDS::Face(this->_Shape));
        ret = fix.Perform() ? true : false;
        this->_Shape = fix.Result();
    }
    else {
        ShapeFix_Shape fix(this->_Shape);
        ret = fix.Perform() ? true : false;
        this->_Shape = fix.Shape();
    }

    return ret;
}

namespace Part {
struct Vertex
{
    Standard_Real x,y,z;
    Standard_Integer i;

    Vertex(Standard_Real X, Standard_Real Y, Standard_Real Z)
        : x(X),y(Y),z(Z)
    {
    }
    Vertex(const gp_Pnt& p)
        : x(p.X()),y(p.Y()),z(p.Z())
    {
    }

    gp_Pnt toPoint() const
    { return gp_Pnt(x,y,z); }

    bool operator < (const Vertex &rclPt) const
    {
        if (fabs ( this->x - rclPt.x) >= MESH_MIN_PT_DIST)
            return this->x < rclPt.x;
        if (fabs ( this->y - rclPt.y) >= MESH_MIN_PT_DIST)
            return this->y < rclPt.y;
        if (fabs ( this->z - rclPt.z) >= MESH_MIN_PT_DIST)
            return this->z < rclPt.z;
        return false; // points are considered to be equal
    }

private:
    // use the same value as used inside the Mesh module
    static const double MESH_MIN_PT_DIST;
};
}

//const double Vertex::MESH_MIN_PT_DIST = 1.0e-6;
const double Vertex::MESH_MIN_PT_DIST = gp::Resolution();

#include <StlTransfer.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshExplorer.hxx>

void TopoShape::getFaces(std::vector<Base::Vector3d> &aPoints,
                         std::vector<FacetTopo> &aTopo,
                         float accuracy, uint16_t flags) const
{
#if 1
    std::set<Vertex> vertices;
    Standard_Real x1, y1, z1;
    Standard_Real x2, y2, z2;
    Standard_Real x3, y3, z3;

    Handle_StlMesh_Mesh aMesh = new StlMesh_Mesh();
    StlTransfer::BuildIncrementalMesh(this->_Shape, accuracy, aMesh);
    StlMesh_MeshExplorer xp(aMesh);
    for (Standard_Integer nbd=1;nbd<=aMesh->NbDomains();nbd++) {
        for (xp.InitTriangle (nbd); xp.MoreTriangle (); xp.NextTriangle ()) {
            xp.TriangleVertices (x1,y1,z1,x2,y2,z2,x3,y3,z3);
            Data::ComplexGeoData::FacetTopo face;
            std::set<Vertex>::iterator it;

            // 1st vertex
            Vertex v1(x1,y1,z1);
            it = vertices.find(v1);
            if (it == vertices.end()) {
                v1.i = vertices.size();
                face.I1 = v1.i;
                vertices.insert(v1);
            }
            else {
                face.I1 = it->i;
            }

            // 2nd vertex
            Vertex v2(x2,y2,z2);
            it = vertices.find(v2);
            if (it == vertices.end()) {
                v2.i = vertices.size();
                face.I2 = v2.i;
                vertices.insert(v2);
            }
            else {
                face.I2 = it->i;
            }

            // 3rd vertex
            Vertex v3(x3,y3,z3);
            it = vertices.find(v3);
            if (it == vertices.end()) {
                v3.i = vertices.size();
                face.I3 = v3.i;
                vertices.insert(v3);
            }
            else {
                face.I3 = it->i;
            }

            // make sure that we don't insert invalid facets
            if (face.I1 != face.I2 &&
                face.I2 != face.I3 &&
                face.I3 != face.I1)
                aTopo.push_back(face);
        }
    }

    std::vector<gp_Pnt> points;
    points.resize(vertices.size());
    for (std::set<Vertex>::iterator it = vertices.begin(); it != vertices.end(); ++it)
        points[it->i] = it->toPoint();
    for (std::vector<gp_Pnt>::iterator it = points.begin(); it != points.end(); ++it)
        aPoints.push_back(Base::Vector3d(it->X(),it->Y(),it->Z()));
#endif
#if 0
    BRepMesh::Mesh (this->_Shape, accuracy);
    std::set<Vertex> vertices;
    for (TopExp_Explorer xp(this->_Shape,TopAbs_FACE); xp.More(); xp.Next()) {
        TopoDS_Face face = TopoDS::Face(xp.Current());
        TopAbs_Orientation orient = face.Orientation();
        // change orientation of the triangles
        Standard_Boolean reversed = false;
        if (orient != TopAbs_FORWARD) {
            reversed = true;
        }
        TopLoc_Location aLoc;
        Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(face, aLoc);
        if (aPoly.IsNull()) continue;

        // getting the transformation of the shape/face
        gp_Trsf myTransf;
        Standard_Boolean identity = true;
        if(!aLoc.IsIdentity())  {
            identity = false;
            myTransf = aLoc.Transformation();
        }

        // cycling through the poly mesh
        const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
        for (Standard_Integer i=1;i<=Nodes.Length();i++) {
            Standard_Real X1, Y1, Z1;
            gp_Pnt p = Nodes.Value(i);
            p.Transform(myTransf);
            p.Coord (X1, Y1, Z1);
        }

        const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
        try {
            for (Standard_Integer i=1;i<=Triangles.Length();i++) {
                Standard_Integer V1, V2, V3;
                Poly_Triangle triangle = Triangles.Value(i);
                triangle.Get(V1, V2, V3);
                if (reversed)
                    std::swap(V1,V2);
                gp_Pnt P1, P2, P3;
                Data::ComplexGeoData::FacetTopo face;
                std::set<Vertex>::iterator it;

                // 1st vertex
                P1 = Nodes(V1);
                P1.Transform(myTransf);
                Vertex v1(P1);
                it = vertices.find(v1);
                if (it == vertices.end()) {
                    v1.i = vertices.size();
                    face.I1 = v1.i;
                    vertices.insert(v1);
                }
                else {
                    face.I1 = it->i;
                }

                // 2nd vertex
                P2 = Nodes(V2);
                P2.Transform(myTransf);
                Vertex v2(P2);
                it = vertices.find(v2);
                if (it == vertices.end()) {
                    v2.i = vertices.size();
                    face.I2 = v2.i;
                    vertices.insert(v2);
                }
                else {
                    face.I2 = it->i;
                }
                
                // 3rd vertex
                P3 = Nodes(V3);
                P3.Transform(myTransf);
                Vertex v3(P3);
                it = vertices.find(v3);
                if (it == vertices.end()) {
                    v3.i = vertices.size();
                    face.I3 = v3.i;
                    vertices.insert(v3);
                }
                else {
                    face.I3 = it->i;
                }

                // make sure that we don't insert invalid facets
                if (face.I1 != face.I2 &&
                    face.I2 != face.I3 &&
                    face.I3 != face.I1)
                    aTopo.push_back(face);
            }
        }
        catch(Standard_Failure) {
        }
    }

    std::map<Standard_Integer,gp_Pnt> points;
    for (std::set<Vertex>::iterator it = vertices.begin(); it != vertices.end(); ++it)
        points[it->i] = it->toPoint();
    for (std::map<Standard_Integer,gp_Pnt>::iterator it = points.begin(); it != points.end(); ++it)
        aPoints.push_back(Base::Vector3d(it->second.X(),it->second.Y(),it->second.Z()));
#endif
}

void TopoShape::setFaces(const std::vector<Base::Vector3d> &Points,
                         const std::vector<FacetTopo> &Topo, float Accuracy)
{
    gp_XYZ p1, p2, p3;
    TopoDS_Vertex Vertex1, Vertex2, Vertex3;
    TopoDS_Face newFace;
    TopoDS_Wire newWire;
    BRepBuilderAPI_Sewing aSewingTool;
    Standard_Real x1, y1, z1;
    Standard_Real x2, y2, z2;
    Standard_Real x3, y3, z3;

    aSewingTool.Init(Accuracy,Standard_True);

    TopoDS_Compound aComp;
    BRep_Builder BuildTool;
    BuildTool.MakeCompound(aComp);

    unsigned int ctPoints = Points.size();
    for (std::vector<FacetTopo>::const_iterator it = Topo.begin(); it != Topo.end(); ++it) {
        if (it->I1 >= ctPoints || it->I2 >= ctPoints || it->I3 >= ctPoints)
            continue;
        x1 = Points[it->I1].x; y1 = Points[it->I1].y; z1 = Points[it->I1].z;
        x2 = Points[it->I2].x; y2 = Points[it->I2].y; z2 = Points[it->I2].z;
        x3 = Points[it->I3].x; y3 = Points[it->I3].y; z3 = Points[it->I3].z;

        p1.SetCoord(x1,y1,z1);
        p2.SetCoord(x2,y2,z2);
        p3.SetCoord(x3,y3,z3);

        if ((!(p1.IsEqual(p2,0.0))) && (!(p1.IsEqual(p3,0.0)))) {
            Vertex1 = BRepBuilderAPI_MakeVertex(p1);
            Vertex2 = BRepBuilderAPI_MakeVertex(p2);
            Vertex3 = BRepBuilderAPI_MakeVertex(p3);

            newWire = BRepBuilderAPI_MakePolygon(Vertex1, Vertex2, Vertex3, Standard_True);
            if (!newWire.IsNull()) {
                newFace = BRepBuilderAPI_MakeFace(newWire);
                if (!newFace.IsNull())
                    BuildTool.Add(aComp, newFace);
            }
        }
    }

    aSewingTool.Load(aComp);
    aSewingTool.Perform();
    _Shape = aSewingTool.SewedShape();
    // TopAbs_Orientation o = _Shape.Orientation();
    _Shape.Reverse(); // seems that we have to reverse the orientation
    if (_Shape.IsNull())
        _Shape = aComp;
}
