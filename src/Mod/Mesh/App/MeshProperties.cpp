/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Writer.h>
#include <Base/Reader.h>

#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Iterator.h"

#include "MeshProperties.h"
#include "Mesh.h"
#include "MeshPy.h"

using namespace Mesh;

TYPESYSTEM_SOURCE(Mesh::PropertyNormalList, App::PropertyVectorList);
TYPESYSTEM_SOURCE(Mesh::PropertyCurvatureList , App::PropertyLists);
TYPESYSTEM_SOURCE(Mesh::PropertyMeshKernel , App::Property);

void PropertyNormalList::transform(const Base::Matrix4D &mat)
{
    // A normal vector is only a direction with unit length, so we only need to rotate it
    // (no translations or scaling)

    // Extract scale factors (assumes an orthogonal rotation matrix)
    // Use the fact that the length of the row vectors of R are all equal to 1
    // And that scaling is applied after rotating
    double s[3];
    s[0] = sqrt(mat[0][0] * mat[0][0] + mat[0][1] * mat[0][1] + mat[0][2] * mat[0][2]);
    s[1] = sqrt(mat[1][0] * mat[1][0] + mat[1][1] * mat[1][1] + mat[1][2] * mat[1][2]);
    s[2] = sqrt(mat[2][0] * mat[2][0] + mat[2][1] * mat[2][1] + mat[2][2] * mat[2][2]);

    // Set up the rotation matrix: zero the translations and make the scale factors = 1
    Base::Matrix4D rot;
    rot.unity();
    for (unsigned short i = 0; i < 3; i++) {
        for (unsigned short j = 0; j < 3; j++) {
            rot[i][j] = mat[i][j] / s[i];
        }
    }

    aboutToSetValue();

    // Rotate the normal vectors
    for (int ii=0; ii<getSize(); ii++) {
        set1Value(ii, rot * operator[](ii));
    }

    hasSetValue();

}

// ----------------------------------------------------------------------------

PropertyCurvatureList::PropertyCurvatureList()
{

}

PropertyCurvatureList::~PropertyCurvatureList()
{

}

void PropertyCurvatureList::setValue(const CurvatureInfo& lValue)
{
    aboutToSetValue();
    _lValueList.resize(1);
    _lValueList[0]=lValue;
    hasSetValue();
}

std::vector<float> PropertyCurvatureList::getCurvature( int mode ) const
{
    const std::vector<Mesh::CurvatureInfo>& fCurvInfo = getValues();
    std::vector<float> fValues;
    fValues.reserve(fCurvInfo.size());

    // Mean curvature
    if (mode == MeanCurvature) {
        for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
        {
            fValues.push_back( 0.5f*(it->fMaxCurvature+it->fMinCurvature) );
        }
    }
    // Gaussian curvature
    else if (mode == GaussCurvature) {
        for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
        {
            fValues.push_back( it->fMaxCurvature * it->fMinCurvature );
        }
    }
    // Maximum curvature
    else if (mode == MaxCurvature) {
        for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
        {
          fValues.push_back( it->fMaxCurvature );
        }
    }
    // Minimum curvature
    else if (mode == MinCurvature) {
        for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
        {
          fValues.push_back( it->fMinCurvature );
        }
    }
    // Absolute curvature
    else if (mode == AbsCurvature) {
        for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
        {
            if ( fabs(it->fMaxCurvature) > fabs(it->fMinCurvature) )
                fValues.push_back( it->fMaxCurvature );
            else
                fValues.push_back( it->fMinCurvature );
        }
    }

    return fValues;
}

void PropertyCurvatureList::transform(const Base::Matrix4D &mat)
{
    // The principal direction is only a vector with unit length, so we only need to rotate it
    // (no translations or scaling)
    
    // Extract scale factors (assumes an orthogonal rotation matrix)
    // Use the fact that the length of the row vectors of R are all equal to 1
    // And that scaling is applied after rotating
    double s[3];
    s[0] = sqrt(mat[0][0] * mat[0][0] + mat[0][1] * mat[0][1] + mat[0][2] * mat[0][2]);
    s[1] = sqrt(mat[1][0] * mat[1][0] + mat[1][1] * mat[1][1] + mat[1][2] * mat[1][2]);
    s[2] = sqrt(mat[2][0] * mat[2][0] + mat[2][1] * mat[2][1] + mat[2][2] * mat[2][2]);
    
    // Set up the rotation matrix: zero the translations and make the scale factors = 1
    Base::Matrix4D rot;
    rot.unity();
    for (unsigned short i = 0; i < 3; i++) {
        for (unsigned short j = 0; j < 3; j++) {
            rot[i][j] = mat[i][j] / s[i];
        }
    }

    aboutToSetValue();

    // Rotate the principal directions
    for (int ii=0; ii<getSize(); ii++)
    {
        CurvatureInfo ci = operator[](ii);
        ci.cMaxCurvDir = rot * ci.cMaxCurvDir;
        ci.cMinCurvDir = rot * ci.cMinCurvDir;
        _lValueList[ii] = ci;
    }

    hasSetValue();
}

void PropertyCurvatureList::Save (Base::Writer &writer) const
{
    if(writer.isForceXML()) {
    }
    else {
        writer.Stream() << writer.ind() << "<CurvatureList file=\"" << 
        writer.addFile(getName(), this) << "\"/>" << std::endl;
    }
}

void PropertyCurvatureList::Restore(Base::XMLReader &reader)
{
    reader.readElement("CurvatureList");
    std::string file (reader.getAttribute("file") );
    
    if (file == "") {
    }
    else {
        // initate a file read
        reader.addFile(file.c_str(),this);
    }
}

void PropertyCurvatureList::SaveDocFile (Base::Writer &writer) const
{
    try {
        unsigned long uCt = getSize();
        writer.Stream().write((const char*)&uCt, sizeof(unsigned long));
        writer.Stream().write((const char*)&(_lValueList[0]), uCt*sizeof(CurvatureInfo));
    } catch( const Base::Exception&) {
        throw;
    }
}

void PropertyCurvatureList::RestoreDocFile(Base::Reader &reader)
{
    try {
        _lValueList.clear();
        unsigned long uCt=ULONG_MAX;
        reader.read((char*)&uCt, sizeof(unsigned long));
        _lValueList.resize(uCt);
        reader.read((char*)&(_lValueList[0]), uCt*sizeof(CurvatureInfo));
    } catch( const Base::Exception&) {
        throw;
    }
}

App::Property *PropertyCurvatureList::Copy(void) const
{
    PropertyCurvatureList *p= new PropertyCurvatureList();
    p->_lValueList = _lValueList;
    return p;
}

void PropertyCurvatureList::Paste(const App::Property &from)
{
    aboutToSetValue();
    _lValueList = dynamic_cast<const PropertyCurvatureList&>(from)._lValueList;
    hasSetValue();
}

// ----------------------------------------------------------------------------

PropertyMeshKernel::PropertyMeshKernel()
  : _pcMesh(new MeshCore::MeshKernel())
{

}

PropertyMeshKernel::~PropertyMeshKernel()
{
    delete _pcMesh;
}

void PropertyMeshKernel::setValue(const MeshCore::MeshKernel& m)
{
    aboutToSetValue();
    _pcMesh->operator=( m );
    hasSetValue();
}

void PropertyMeshKernel::setValue(MeshCore::MeshKernel* m)
{
    aboutToSetValue();
    delete (_pcMesh);
    _pcMesh = m;
    hasSetValue();
}

const MeshCore::MeshKernel& PropertyMeshKernel::getValue(void)const 
{
    return *_pcMesh;
}

unsigned int PropertyMeshKernel::getMemSize (void) const
{
    unsigned int size = 0;
    size += _pcMesh->GetMemSize();
    
    return size;
}

//FIXME: This is the provisional solution to allow to transform meshes.
//At a later stage we want to use a so called Placement object instead.
void PropertyMeshKernel::applyTransformation( const Base::Matrix4D& rclTrf )
{
    aboutToSetValue();
    _pcMesh->Transform(rclTrf);
    hasSetValue();
}

void PropertyMeshKernel::deletePointIndices( const std::vector<unsigned long>& inds )
{
    aboutToSetValue();
    _pcMesh->DeletePoints(inds);
    hasSetValue();
}

void PropertyMeshKernel::deleteFacetIndices( const std::vector<unsigned long>& inds )
{
    aboutToSetValue();
    _pcMesh->DeleteFacets(inds);
    hasSetValue();
}

void PropertyMeshKernel::setPointIndices( const std::vector<std::pair<unsigned long, Base::Vector3f> >& inds )
{
    aboutToSetValue();
    for (std::vector<std::pair<unsigned long, Base::Vector3f> >::const_iterator it = inds.begin(); it != inds.end(); ++it)
        _pcMesh->SetPoint(it->first, it->second);
    hasSetValue();
}

void PropertyMeshKernel::append( const std::vector<MeshCore::MeshFacet>& rFaces, const std::vector<Base::Vector3f>& rPoints)
{
    aboutToSetValue();
    _pcMesh->AddFacets(rFaces, rPoints);
    hasSetValue();
}

/**
 * Clears the mesh kernel and frees any allocated memory.
 * \note If you replace the current with an empty mesh kernel with setValue() 
 * then the memory doesn't get freed.
 */
void PropertyMeshKernel::clear()
{
    aboutToSetValue();
    _pcMesh->Clear();
    hasSetValue();
}

PyObject *PropertyMeshKernel::getPyObject(void)
{
    return new MeshPy(_pcMesh);
}

void PropertyMeshKernel::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(MeshPy::Type))) {
        MeshPy  *pcObject = (MeshPy*)value;
        // Copy the content, do NOT replace the pointer
        setValue(pcObject->getMesh());
    } else {
        std::string error = std::string("type must be 'Mesh', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

void PropertyMeshKernel::Save (Base::Writer &writer) const
{
    if (writer.isForceXML()) {
        writer.Stream() << writer.ind() << "<Mesh>" << std::endl;
        MeshCore::MeshOutput saver(*_pcMesh);
        saver.SaveXML(writer);
    }
    else {
        writer.Stream() << writer.ind() << "<Mesh file=\"" << 
        writer.addFile("MeshKernel.bms", this) << "\"/>" << std::endl;
    }
}

void PropertyMeshKernel::Restore(Base::XMLReader &reader)
{
    reader.readElement("Mesh");
    std::string file (reader.getAttribute("file") );
    
    if (file == "") {
        // read XML
        MeshCore::MeshInput restorer(*_pcMesh);
        restorer.LoadXML(reader);
    } 
    else {
        // initate a file read
        reader.addFile(file.c_str(),this);
    }
}

void PropertyMeshKernel::SaveDocFile (Base::Writer &writer) const
{
    try {
        _pcMesh->Write( writer.Stream() );
    } catch( const Base::Exception&) {
        throw;
    }
}

void PropertyMeshKernel::RestoreDocFile(Base::Reader &reader)
{
    try {
        _pcMesh->Read( reader );
    } catch( const Base::Exception&) {
        throw;
    }
}

App::Property *PropertyMeshKernel::Copy(void) const
{
    //NOTE: We must not copy the pointer to the mesh but the mesh data itself 
    PropertyMeshKernel *p= new PropertyMeshKernel();
    *(p->_pcMesh) = *_pcMesh;
    return p;
}

void PropertyMeshKernel::Paste(const App::Property &from)
{
    //NOTE: We must not restore the pointer to the mesh but the mesh data itself 
    aboutToSetValue();
    const PropertyMeshKernel& p = dynamic_cast<const PropertyMeshKernel&>(from);
    *_pcMesh = *p._pcMesh;
    hasSetValue();
}
