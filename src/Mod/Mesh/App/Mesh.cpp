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

#include "Mesh.h"
#include "MeshPy.h"

using namespace Mesh;

TYPESYSTEM_SOURCE(Mesh::PropertyNormalList, App::PropertyVectorList);
TYPESYSTEM_SOURCE(Mesh::PropertyCurvatureList , App::PropertyLists);
TYPESYSTEM_SOURCE(Mesh::PropertyMeshKernel , App::Property);

MeshCore::MeshFacetIterator MeshObject::FacetIterator() const
{
  MeshCore::MeshFacetIterator it = _pcKernel->FacetIterator();
  it.Transform(_Mtrx);
  return it;
}

MeshCore::MeshPointIterator MeshObject::PointIterator() const
{
  MeshCore::MeshPointIterator it = _pcKernel->PointIterator();
  it.Transform(_Mtrx);
  return it;
}

void MeshObject::ApplyTransform( const Base::Matrix4D& rclTrf )
{
  _Mtrx = rclTrf * _Mtrx;
}

void MeshObject::SetTransform( const Base::Matrix4D& rclTrf )
{
  _Mtrx = rclTrf;
}

// ----------------------------------------------------------------------------

void PropertyNormalList::transform(const Matrix4D &mat)
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
  Matrix4D rot;
  rot.unity();
  for (unsigned short i = 0; i < 3; i++) {
    for (unsigned short j = 0; j < 3; j++) {
      rot[i][j] = mat[i][j] / s[i];
    }
  }

  // Rotate the normal vectors
  for (int ii=0; ii<getSize(); ii++)
  {
    set1Value(ii, rot * operator[](ii));
  }
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
  if ( mode == MeanCurvature )
  {
    for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( 0.5f*(it->fMaxCurvature+it->fMinCurvature) );
    }
  }
  // Gaussian curvature
  else if ( mode == GaussCurvature )
  {
    for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( it->fMaxCurvature * it->fMinCurvature );
    }
  }
  // Maximum curvature
  else if ( mode == MaxCurvature )
  {
    for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( it->fMaxCurvature );
    }
  }
  // Minimum curvature
  else if ( mode == MinCurvature )
  {
    for ( std::vector<Mesh::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( it->fMinCurvature );
    }
  }
  // Absolute curvature
  else if ( mode == AbsCurvature )
  {
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

void PropertyCurvatureList::transform(const Matrix4D &mat)
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
  Matrix4D rot;
  rot.unity();
  for (unsigned short i = 0; i < 3; i++) {
    for (unsigned short j = 0; j < 3; j++) {
      rot[i][j] = mat[i][j] / s[i];
    }
  }

  // Rotate the principal directions
  for (int ii=0; ii<getSize(); ii++)
  {
    CurvatureInfo ci = operator[](ii);
    ci.cMaxCurvDir = rot * ci.cMaxCurvDir;
    ci.cMinCurvDir = rot * ci.cMinCurvDir;
    set1Value(ii, ci);
  }
}

void PropertyCurvatureList::Save (Base::Writer &writer)
{
  if(writer.isForceXML())
  {
  }else{
    writer << writer.ind() << "<CurvatureList file=\"" << writer.addFile(getName(), this) << "\"/>" << std::endl;
  }
}

void PropertyCurvatureList::Restore(Base::XMLReader &reader)
{
  reader.readElement("CurvatureList");
  std::string file (reader.getAttribute("file") );

  if(file == "")
  {
  }else{
    // initate a file read
    reader.addFile(file.c_str(),this);
  }
}

void PropertyCurvatureList::SaveDocFile (Base::Writer &writer)
{
  try {
    unsigned long uCt = getSize();
    writer.write((const char*)&uCt, sizeof(unsigned long));
    writer.write((const char*)&(_lValueList[0]), uCt*sizeof(CurvatureInfo));
  } catch( const Base::Exception& e) {
    throw e;
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
  } catch( const Base::Exception& e) {
    throw e;
  }
}

// ----------------------------------------------------------------------------

PropertyMeshKernel::PropertyMeshKernel()
:_pcMesh(new MeshCore::MeshKernel())
{

}

PropertyMeshKernel::~PropertyMeshKernel()
{
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

PyObject *PropertyMeshKernel::getPyObject(void)
{
  return new MeshPy(_pcMesh);
}

void PropertyMeshKernel::setPyObject(PyObject *value)
{
  if( PyObject_TypeCheck(value, &(MeshPy::Type)) ) {
   	MeshPy  *pcObject = (MeshPy*)value;
    // Copy the content, do NOT replace the pointer
    setValue(*pcObject->getMesh());
  }
}

void PropertyMeshKernel::Save (Base::Writer &writer)
{
  if( writer.isForceXML() )
  {
    writer << writer.ind() << "<Mesh>" << std::endl;
    MeshCore::MeshDocXML saver(*_pcMesh);
    saver.Save(writer);
  }else{
    writer << writer.ind() << "<Mesh file=\"" << writer.addFile("MeshKernel.bms", this) << "\"/>" << std::endl;
  }
}

void PropertyMeshKernel::Restore(Base::XMLReader &reader)
{
  reader.readElement("Mesh");
  std::string file (reader.getAttribute("file") );

  if(file == "")
  {
    // read XML
    MeshCore::MeshDocXML restorer(*_pcMesh);
    restorer.Restore(reader);
  }else{
    // initate a file read
    reader.addFile(file.c_str(),this);
  }
}

void PropertyMeshKernel::SaveDocFile (Base::Writer &writer)
{
  try {
    _pcMesh->Write( writer );
  } catch( const Base::Exception& e) {
    throw e;
  }
}

void PropertyMeshKernel::RestoreDocFile(Base::Reader &reader)
{
  try {
    _pcMesh->Read( reader );
  } catch( const Base::Exception& e) {
    throw e;
  }
}

// ----------------------------------------------------------------------------
#if 0
MeshPropertyNormal::MeshPropertyNormal(int size)
{
  Normales.resize(size);
}


void MeshPropertyNormal::resizePoints(void)
{
  setInvalid();
  Normales.clear();
}


void MeshPropertyNormal::transform(const Matrix4D& mat)
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
  Matrix4D rot;
  rot.unity();
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      rot[i][j] = mat[i][j] / s[i];

  // Rotate the normal vectors
  for (std::vector<Vector3D>::iterator it = Normales.begin(); it != Normales.end(); ++it)
  {
    *it = rot * (*it);
  }
}

// ----------------------------------------------------------------------------

MeshPropertyCurvature::MeshPropertyCurvature(int size)
{
  Curvature.resize(size);
}

MeshPropertyCurvature::~MeshPropertyCurvature()
{
}

void MeshPropertyCurvature::resizePoints() 
{
  setInvalid();
  Curvature.clear();
}

void MeshPropertyCurvature::resizeFaces()
{
  setInvalid();
  Curvature.clear();
}

void MeshPropertyCurvature::transform(const Matrix4D &mat)
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
  Matrix4D rot;
  rot.unity();
  for (unsigned short i = 0; i < 3; i++)
    for (unsigned short j = 0; j < 3; j++)
      rot[i][j] = mat[i][j] / s[i];

  // Rotate the principal directions
  for (std::vector<fCurvature>::iterator it = Curvature.begin(); it != Curvature.end(); ++it)
  {
    it->cMaxCurvDir = rot * it->cMaxCurvDir;
    it->cMinCurvDir = rot * it->cMinCurvDir;
  }
}

std::vector<float> MeshPropertyCurvature::getCurvature( MeshPropertyCurvature::Mode tMode) const
{
  std::vector<float> aCurvatures;
  if ( tMode == MaxCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      aCurvatures.push_back( it->fMaxCurvature );
    }
  }
  else if ( tMode == MinCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      aCurvatures.push_back( it->fMinCurvature );
    }
  }
  else if ( tMode == MeanCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      aCurvatures.push_back( 0.5f * ( it->fMaxCurvature + it->fMinCurvature ) );
    }
  }
  else if ( tMode == GaussCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      aCurvatures.push_back( it->fMaxCurvature * it->fMinCurvature );
    }
  }

  return aCurvatures;
}

std::vector<Vector3D> MeshPropertyCurvature::getCurvatureDir( Mode tMode) const
{
  std::vector<Vector3D> aCurvatures;
  if ( tMode == MaxCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      aCurvatures.push_back( it->cMaxCurvDir );
    }
  }
  else if ( tMode == MinCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      aCurvatures.push_back( it->cMinCurvDir );
    }
  }

  return aCurvatures;
}

std::vector<Vector3D> MeshPropertyCurvature::getAbsCurvatureDir( Mode tMode) const
{
  std::vector<Vector3D> aCurvatures;
  if ( tMode == MaxCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      if ( fabs(it->fMaxCurvature) > fabs(it->fMinCurvature) )
        aCurvatures.push_back( it->cMaxCurvDir );
      else
        aCurvatures.push_back( it->cMinCurvDir );
    }
  }
  else if ( tMode == MinCurvature )
  {
    for ( std::vector<fCurvature>::const_iterator it = Curvature.begin(); it != Curvature.end(); ++it )
    {
      if ( fabs(it->fMaxCurvature) > fabs(it->fMinCurvature) )
        aCurvatures.push_back( it->cMinCurvDir );
      else
        aCurvatures.push_back( it->cMaxCurvDir );
    }
  }

  return aCurvatures;
}

//*************************************************************************************************************

MeshWithProperty::MeshWithProperty()
{
  _Mesh = new MeshKernel();
//  Base::Console().Log("Create MeshWithProperty: %p \n",this);
}


MeshWithProperty::MeshWithProperty(const MeshWithProperty& New)
{
  _Mesh = new MeshKernel(*(New._Mesh));
//  Base::Console().Log("Create MeshWithProperty: %p \n",this);

}

 	/// destructor
MeshWithProperty::~MeshWithProperty(void)
{ 
  delete _Mesh;
//  Base::Console().Log("Destroy MeshWithProperty: %p \n",this);
}

void MeshWithProperty::operator= ( const MeshWithProperty& New)
{
  *_Mesh = *New._Mesh;
  
  DataWithPropertyBag::operator= (New);
}

void MeshWithProperty::clear(void)
{
  DataWithPropertyBag::clear();
  _Mesh->Clear();
}

void MeshWithProperty::transform(const Matrix4D &rclMat)
{
  DataWithPropertyBag::transform(rclMat);
  *_Mesh *= rclMat;
}

#endif
