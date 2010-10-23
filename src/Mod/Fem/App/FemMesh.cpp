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
# include <strstream>
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


using namespace Fem;
using namespace Base;

TYPESYSTEM_SOURCE(Fem::FemMesh , Base::Persistence);

FemMesh::FemMesh()
{
    myGen = new SMESH_Gen();
    myMesh = myGen->CreateMesh(1,false);
}

FemMesh::FemMesh(const FemMesh& Mesh)
{
    myGen = new SMESH_Gen();
    myMesh = myGen->CreateMesh(1,false);
    myMesh->ShapeToMesh(Mesh.myMesh->GetShapeToMesh());
}

FemMesh::~FemMesh()
{
    delete myMesh;
    //delete myGen; // crashes
}

FemMesh &FemMesh::operator=(const FemMesh& Trac)
{
    return *this;
}

const SMESH_Mesh* FemMesh::getSMesh() const
{
    return myMesh;
}

SMESH_Mesh* FemMesh::getSMesh()
{
    return myMesh;
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
    //try {
    //    // If the shape is empty an exception may be thrown
    //    Bnd_Box bounds;
    //    BRepBndLib::Add(_Shape, bounds);
    //    bounds.SetGap(0.0);
    //    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    //    bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);

    //    box.MinX = xMin;
    //    box.MaxX = xMax;
    //    box.MinY = yMin;
    //    box.MaxY = yMax;
    //    box.MinZ = zMin;
    //    box.MaxZ = zMax;
    //}
    //catch (Standard_Failure) {
    //}

    return box;
}

std::vector<const char*> FemMesh::getElementTypes(void) const
{
    // FIXME implement subelement interface 
    std::vector<const char*> temp;
    //temp.push_back("Vertex");
    //temp.push_back("Edge");
    //temp.push_back("Face");

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
