/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2009     *
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
# include <memory>
# include <strstream>
# include <Bnd_Box.hxx>
# include <BRepBndLib.hxx>
#endif

#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Stream.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>


#include "FemMesh.h"

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMDS_VolumeTool.hxx>
#include <StdMeshers_MaxLength.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_MaxElementArea.hxx>
#include <StdMeshers_NumberOfSegments.hxx>
#include <StdMeshers_Deflection1D.hxx>
#include <StdMeshers_Regular_1D.hxx>
#include <StdMeshers_StartEndLength.hxx>
#include <StdMeshers_QuadranglePreference.hxx>
#include <StdMeshers_Quadrangle_2D.hxx>
#include <StdMeshers_QuadraticMesh.hxx>


using namespace Fem;
using namespace Base;

TYPESYSTEM_SOURCE(Fem::FemMesh , Base::Persistence);

FemMesh::FemMesh()
{
    myGen = new SMESH_Gen();
    myMesh = myGen->CreateMesh(1,false);
}

FemMesh::FemMesh(const FemMesh& mesh)
{
    myGen = new SMESH_Gen();
    myMesh = myGen->CreateMesh(1,false);
    copyMeshData(mesh);
}

FemMesh::~FemMesh()
{
    delete myMesh;
#if defined(__GNUC__)
    delete myGen; // crashes with MSVC
#endif
}

FemMesh &FemMesh::operator=(const FemMesh& mesh)
{
    if (this != &mesh) {
        copyMeshData(mesh);
    }
    return *this;
}

void FemMesh::copyMeshData(const FemMesh& mesh)
{
    const SMDS_MeshInfo& info = mesh.myMesh->GetMeshDS()->GetMeshInfo();
    int numPoly = info.NbPolygons();
    int numVolu = info.NbVolumes();
    int numTetr = info.NbTetras();
    int numHexa = info.NbHexas();
    int numPyrd = info.NbPyramids();
    int numPris = info.NbPrisms();
    int numHedr = info.NbPolyhedrons();

    SMESHDS_Mesh* meshds = this->myMesh->GetMeshDS();
    meshds->ClearMesh();

    SMDS_NodeIteratorPtr aNodeIter = mesh.myMesh->GetMeshDS()->nodesIterator();
    for (;aNodeIter->more();) {
        const SMDS_MeshNode* aNode = aNodeIter->next();
        meshds->AddNodeWithID(aNode->X(),aNode->Y(),aNode->Z(), aNode->GetID());
    }

    SMDS_EdgeIteratorPtr aEdgeIter = mesh.myMesh->GetMeshDS()->edgesIterator();
    for (;aEdgeIter->more();) {
        const SMDS_MeshEdge* aEdge = aEdgeIter->next();
        meshds->AddEdgeWithID(aEdge->GetNode(0), aEdge->GetNode(1), aEdge->GetID());
    }

    SMDS_FaceIteratorPtr aFaceIter = mesh.myMesh->GetMeshDS()->facesIterator();
    for (;aFaceIter->more();) {
        const SMDS_MeshFace* aFace = aFaceIter->next();
        switch (aFace->NbNodes()) {
            case 3:
                meshds->AddFaceWithID(aFace->GetNode(0),
                                      aFace->GetNode(1),
                                      aFace->GetNode(2),
                                      aFace->GetID());
                break;
            case 4:
                meshds->AddFaceWithID(aFace->GetNode(0),
                                      aFace->GetNode(1),
                                      aFace->GetNode(2),
                                      aFace->GetNode(3),
                                      aFace->GetID());
                break;
            case 6:
                meshds->AddFaceWithID(aFace->GetNode(0),
                                      aFace->GetNode(1),
                                      aFace->GetNode(2),
                                      aFace->GetNode(3),
                                      aFace->GetNode(4),
                                      aFace->GetNode(5),
                                      aFace->GetID());
                break;
            case 8:
                meshds->AddFaceWithID(aFace->GetNode(0),
                                      aFace->GetNode(1),
                                      aFace->GetNode(2),
                                      aFace->GetNode(3),
                                      aFace->GetNode(4),
                                      aFace->GetNode(5),
                                      aFace->GetNode(6),
                                      aFace->GetNode(7),
                                      aFace->GetID());
                break;
            default:
                {
                    std::vector<const SMDS_MeshNode*> aNodes;
                    for (int i=0; aFace->NbNodes(); i++)
                        aNodes.push_back(aFace->GetNode(0));
                    meshds->AddPolygonalFaceWithID(aNodes, aFace->GetID());
                }
                break;
        }
    }

    SMDS_VolumeIteratorPtr aVolIter = mesh.myMesh->GetMeshDS()->volumesIterator();
    for (;aVolIter->more();) {
        const SMDS_MeshVolume* aVol = aVolIter->next();
        switch (aVol->NbNodes()) {
            case 4:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetID());
                break;
            case 5:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetNode(4),
                                        aVol->GetID());
                break;
            case 6:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetNode(4),
                                        aVol->GetNode(5),
                                        aVol->GetID());
                break;
            case 8:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetNode(4),
                                        aVol->GetNode(5),
                                        aVol->GetNode(6),
                                        aVol->GetNode(7),
                                        aVol->GetID());
                break;
            case 10:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetNode(4),
                                        aVol->GetNode(5),
                                        aVol->GetNode(6),
                                        aVol->GetNode(7),
                                        aVol->GetNode(8),
                                        aVol->GetNode(9),
                                        aVol->GetID());
                break;
            case 13:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetNode(4),
                                        aVol->GetNode(5),
                                        aVol->GetNode(6),
                                        aVol->GetNode(7),
                                        aVol->GetNode(8),
                                        aVol->GetNode(9),
                                        aVol->GetNode(10),
                                        aVol->GetNode(11),
                                        aVol->GetNode(12),
                                        aVol->GetID());
                break;
            case 15:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetNode(4),
                                        aVol->GetNode(5),
                                        aVol->GetNode(6),
                                        aVol->GetNode(7),
                                        aVol->GetNode(8),
                                        aVol->GetNode(9),
                                        aVol->GetNode(10),
                                        aVol->GetNode(11),
                                        aVol->GetNode(12),
                                        aVol->GetNode(13),
                                        aVol->GetNode(14),
                                        aVol->GetID());
                break;
            case 20:
                meshds->AddVolumeWithID(aVol->GetNode(0),
                                        aVol->GetNode(1),
                                        aVol->GetNode(2),
                                        aVol->GetNode(3),
                                        aVol->GetNode(4),
                                        aVol->GetNode(5),
                                        aVol->GetNode(6),
                                        aVol->GetNode(7),
                                        aVol->GetNode(8),
                                        aVol->GetNode(9),
                                        aVol->GetNode(10),
                                        aVol->GetNode(11),
                                        aVol->GetNode(12),
                                        aVol->GetNode(13),
                                        aVol->GetNode(14),
                                        aVol->GetNode(15),
                                        aVol->GetNode(16),
                                        aVol->GetNode(17),
                                        aVol->GetNode(18),
                                        aVol->GetNode(19),
                                        aVol->GetID());
                break;
            default:
                {
                    std::vector<int> quantities;
                    std::vector<const SMDS_MeshNode*> aNodes;
                    for (int i=0; aVol->NbNodes(); i++) {
                        aNodes.push_back(aVol->GetNode(0));
                        quantities.push_back(1);
                    }
                    meshds->AddPolyhedralVolumeWithID(aNodes,
                        quantities, aVol->GetID());
                }
                break;
        }
    }
}

const SMESH_Mesh* FemMesh::getSMesh() const
{
    return myMesh;
}

SMESH_Mesh* FemMesh::getSMesh()
{
    return myMesh;
}

SMESH_Gen * FemMesh::getGenerator()
{
    return myGen;
}

void FemMesh::addHypothesis(const TopoDS_Shape & aSubShape, SMESH_Hypothesis* hyp)
{
    myMesh->AddHypothesis(aSubShape, hyp->GetID());
    SMESH_HypothesisPtr ptr(hyp);
    hypoth.push_back(ptr);
}

void FemMesh::setStanardHypotheses()
{
    if (!hypoth.empty())
        return;
    int hyp=0;
    SMESH_HypothesisPtr len(new StdMeshers_MaxLength(hyp++, 1, myGen));
    static_cast<StdMeshers_MaxLength*>(len.get())->SetLength(1.0);
    hypoth.push_back(len);

    SMESH_HypothesisPtr loc(new StdMeshers_LocalLength(hyp++, 1, myGen));
    static_cast<StdMeshers_MaxLength*>(loc.get())->SetLength(1.0);
    hypoth.push_back(loc);

    SMESH_HypothesisPtr area(new StdMeshers_MaxElementArea(hyp++, 1, myGen));
    static_cast<StdMeshers_MaxElementArea*>(area.get())->SetMaxArea(1.0);
    hypoth.push_back(area);

    SMESH_HypothesisPtr segm(new StdMeshers_NumberOfSegments(hyp++, 1, myGen));
    static_cast<StdMeshers_NumberOfSegments*>(segm.get())->SetNumberOfSegments(1);
    hypoth.push_back(segm);

    SMESH_HypothesisPtr defl(new StdMeshers_Deflection1D(hyp++, 1, myGen));
    static_cast<StdMeshers_Deflection1D*>(defl.get())->SetDeflection(0.01);
    hypoth.push_back(defl);

    SMESH_HypothesisPtr reg(new StdMeshers_Regular_1D(hyp++, 1, myGen));
    hypoth.push_back(reg);

    //SMESH_HypothesisPtr sel(new StdMeshers_StartEndLength(hyp++, 1, myGen));
    //static_cast<StdMeshers_StartEndLength*>(sel.get())->SetLength(1.0, true);
    //hypoth.push_back(sel);

    SMESH_HypothesisPtr qdp(new StdMeshers_QuadranglePreference(hyp++,1,myGen));
    hypoth.push_back(qdp);

    SMESH_HypothesisPtr q2d(new StdMeshers_Quadrangle_2D(hyp++,1,myGen));
    hypoth.push_back(q2d);

    // Apply hypothesis
    for (int i=0; i<hyp;i++)
        myMesh->AddHypothesis(myMesh->GetShapeToMesh(), i);
}

void FemMesh::compute()
{
    myGen->Compute(*myMesh, myMesh->GetShapeToMesh());
}

void FemMesh::read(const char *FileName)
{
    Base::FileInfo File(FileName);
  
    // checking on the file
    if (!File.isReadable())
        throw Base::Exception("File to load not existing or not readable");
    
    if (File.hasExtension("unv") ) {
        // read UNV file
        myMesh->UNVToMesh(File.filePath().c_str());
    }
    else if (File.hasExtension("med") ) {
        myMesh->MEDToMesh(File.filePath().c_str(),File.fileNamePure().c_str());
    }
    else if (File.hasExtension("stl") ) {
        // read brep-file
        myMesh->STLToMesh(File.filePath().c_str());
    }
    else if (File.hasExtension("dat") ) {
        // read brep-file
        myMesh->DATToMesh(File.filePath().c_str());
    }
    else{
        throw Base::Exception("Unknown extension");
    }
}

void FemMesh::write(const char *FileName) const
{
    Base::FileInfo File(FileName);

    if (File.hasExtension("unv") ) {
        // read UNV file
         myMesh->ExportUNV(File.filePath().c_str());
    }
    else if (File.hasExtension("med") ) {
         myMesh->ExportMED(File.filePath().c_str());
    }
    else if (File.hasExtension("stl") ) {
        // read brep-file
        myMesh->ExportSTL(File.filePath().c_str(),false);
    }
    else if (File.hasExtension("dat") ) {
        // read brep-file
        myMesh->ExportDAT(File.filePath().c_str());
    }
    else{
        throw Base::Exception("Unknown extension");
    }
}

// ==== Base class implementer ==============================================================

unsigned int FemMesh::getMemSize (void) const
{
    return 0;
}

void FemMesh::Save (Base::Writer &writer) const
{
    if(!writer.isForceXML()) {
        //See SaveDocFile(), RestoreDocFile()
        writer.Stream() << writer.ind() << "<FemMesh file=\"" 
                        << writer.addFile("FemMesh.unv", this)
                        << "\"/>" << std::endl;
    }
}

void FemMesh::Restore(Base::XMLReader &reader)
{
    reader.readElement("FemMesh");
    std::string file (reader.getAttribute("file") );

    if (!file.empty()) {
        // initate a file read
        reader.addFile(file.c_str(),this);
    }
}

void FemMesh::SaveDocFile (Base::Writer &writer) const
{
    // create a temporary file and copy the content to the zip stream
    Base::FileInfo fi(Base::FileInfo::getTempFileName().c_str());

    myMesh->ExportUNV(fi.filePath().c_str());
 
    Base::ifstream file(fi, std::ios::in | std::ios::binary);
    if (file){
        unsigned long ulSize = 0; 
        std::streambuf* buf = file.rdbuf();
        if (buf) {
            unsigned long ulCurr;
            ulCurr = buf->pubseekoff(0, std::ios::cur, std::ios::in);
            ulSize = buf->pubseekoff(0, std::ios::end, std::ios::in);
            buf->pubseekoff(ulCurr, std::ios::beg, std::ios::in);
        }

        // read in the ASCII file and write back to the stream
        std::strstreambuf sbuf(ulSize);
        file >> &sbuf;
        writer.Stream() << &sbuf;
    }

    file.close();
    // remove temp file
    fi.deleteFile();
}

void FemMesh::RestoreDocFile(Base::Reader &reader)
{
    // create a temporary file and copy the content from the zip stream
    Base::FileInfo fi(Base::FileInfo::getTempFileName().c_str());

    // read in the ASCII file and write back to the file stream
    Base::ofstream file(fi, std::ios::out | std::ios::binary);
    if (reader)
        reader >> file.rdbuf();
    file.close();

    // read the shape from the temp file
    myMesh->UNVToMesh(fi.filePath().c_str());

    // delete the temp file
    fi.deleteFile();
}

void FemMesh::setTransform(const Base::Matrix4D& rclTrf)
{
 /*   gp_Trsf mov;
    mov.SetValues(rclTrf[0][0],rclTrf[0][1],rclTrf[0][2],rclTrf[0][3],
                  rclTrf[1][0],rclTrf[1][1],rclTrf[1][2],rclTrf[1][3],
                  rclTrf[2][0],rclTrf[2][1],rclTrf[2][2],rclTrf[2][3],
                  0.00001,0.00001);
    TopLoc_Location loc(mov);
    _Shape.Location(loc);*/
}

Base::Matrix4D FemMesh::getTransform(void) const
{
    Base::Matrix4D mtrx;
    //gp_Trsf Trf = _Shape.Location().Transformation();

    //gp_Mat m = Trf._CSFDB_Getgp_Trsfmatrix();
    //gp_XYZ p = Trf._CSFDB_Getgp_Trsfloc();
    //Standard_Real scale = Trf._CSFDB_Getgp_Trsfscale();

    //// set Rotation matrix
    //mtrx[0][0] = scale * m._CSFDB_Getgp_Matmatrix(0,0);
    //mtrx[0][1] = scale * m._CSFDB_Getgp_Matmatrix(0,1);
    //mtrx[0][2] = scale * m._CSFDB_Getgp_Matmatrix(0,2);

    //mtrx[1][0] = scale * m._CSFDB_Getgp_Matmatrix(1,0);
    //mtrx[1][1] = scale * m._CSFDB_Getgp_Matmatrix(1,1);
    //mtrx[1][2] = scale * m._CSFDB_Getgp_Matmatrix(1,2);

    //mtrx[2][0] = scale * m._CSFDB_Getgp_Matmatrix(2,0);
    //mtrx[2][1] = scale * m._CSFDB_Getgp_Matmatrix(2,1);
    //mtrx[2][2] = scale * m._CSFDB_Getgp_Matmatrix(2,2);

    //// set pos vector
    //mtrx[0][3] = p._CSFDB_Getgp_XYZx();
    //mtrx[1][3] = p._CSFDB_Getgp_XYZy();
    //mtrx[2][3] = p._CSFDB_Getgp_XYZz();

    return mtrx;
}

Base::BoundBox3d FemMesh::getBoundBox(void) const
{
    Base::BoundBox3d box;
    try {
        // If the shape is empty an exception may be thrown
        Bnd_Box bounds;
        BRepBndLib::Add(myMesh->GetShapeToMesh(), bounds);
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

std::vector<const char*> FemMesh::getElementTypes(void) const
{
    std::vector<const char*> temp;
    temp.push_back("Vertex");
    temp.push_back("Edge");
    temp.push_back("Face");
    temp.push_back("Volume");

    return temp;
}

Data::Segment* FemMesh::getSubElement(const char* Type, unsigned long n) const
{
    // FIXME implement subelement interface 
    //std::stringstream str;
    //str << Type << n;
    //std::string temp = str.str();
    //return new ShapeSegment(getSubShape(temp.c_str()));
    return 0;
}
