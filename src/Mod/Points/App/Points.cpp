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
# include <math.h>
# include <iostream>
#endif

#include <Base/Exception.h>
#include <Base/Matrix.h>
#include <Base/Persistance.h>
#include <Base/Writer.h>

#include "Points.h"

using namespace Points;
using namespace std;

TYPESYSTEM_SOURCE(Points::PropertyGreyValue, App::PropertyFloat);
TYPESYSTEM_SOURCE(Points::PropertyGreyValueList, App::PropertyFloatList);
TYPESYSTEM_SOURCE(Points::PropertyNormalList, App::PropertyVectorList);
TYPESYSTEM_SOURCE(Points::PropertyCurvatureList , App::PropertyLists);

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
  const std::vector<Points::CurvatureInfo>& fCurvInfo = getValues();
  std::vector<float> fValues;
  fValues.reserve(fCurvInfo.size());

  // Mean curvature
  if ( mode == MeanCurvature )
  {
    for ( std::vector<Points::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( 0.5f*(it->fMaxCurvature+it->fMinCurvature) );
    }
  }
  // Gaussian curvature
  else if ( mode == GaussCurvature )
  {
    for ( std::vector<Points::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( it->fMaxCurvature * it->fMinCurvature );
    }
  }
  // Maximum curvature
  else if ( mode == MaxCurvature )
  {
    for ( std::vector<Points::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( it->fMaxCurvature );
    }
  }
  // Minimum curvature
  else if ( mode == MinCurvature )
  {
    for ( std::vector<Points::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
    {
      fValues.push_back( it->fMinCurvature );
    }
  }
  // Absolute curvature
  else if ( mode == AbsCurvature )
  {
    for ( std::vector<Points::CurvatureInfo>::const_iterator it=fCurvInfo.begin();it!=fCurvInfo.end(); ++it )
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
  writer.addFile("Curvature", this);
}

void PropertyCurvatureList::Restore(Base::XMLReader &reader)
{
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










/*

Point::Point ()
{
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}

Point::Point (float fX, float fY, float fZ)
{
    x = fX;
    y = fY;
    z = fZ;
}

Point::Point (float Coord[3])
{
    x = Coord[0];
    y = Coord[1];
    z = Coord[2];
}

Point::Point (const Point& rPt)
{
    x = rPt.x;
    y = rPt.y;
    z = rPt.z;
}

float& Point::operator[] (int i) const
{
    return (float&) *(&x + i);
}

Point::operator float* ()
{
    return &x;
}

Point& Point::operator= (const Point& rPt)
{
    x = rPt.x;
    y = rPt.y;
    z = rPt.z;
    return *this;
}

bool Point::operator== (const Point& rPt) const
{
    return ( x == rPt.x && y == rPt.y && z == rPt.z );
}

bool Point::operator!= (const Point& rPt) const
{
    return ( x != rPt.x || y != rPt.y || z != rPt.z );
}
*/


void PointsPropertyNormal::transform(const Matrix4D &mat)
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
  for (unsigned short i = 0; i < 3; i++)
    for (unsigned short j = 0; j < 3; j++)
      rot[i][j] = mat[i][j] / s[i];

  // Rotate the normal vectors
  for (std::vector<Vector3D>::iterator it = aVertexNormal.begin(); it != aVertexNormal.end(); ++it)
  {
    *it = rot * (*it);
  }
}

// ----------------------------------------------------------------------------

PointsPropertyCurvature::PointsPropertyCurvature(int size)
{
  Curvature.resize(size);
}

PointsPropertyCurvature::~PointsPropertyCurvature()
{
}

void PointsPropertyCurvature::resizePoints() 
{
  setInvalid();
  Curvature.clear();
}

void PointsPropertyCurvature::transform(const Matrix4D &mat)
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

std::vector<float> PointsPropertyCurvature::getCurvature( PointsPropertyCurvature::Mode tMode) const
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

std::vector<Vector3D> PointsPropertyCurvature::getCurvatureDir( Mode tMode) const
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

std::vector<Vector3D> PointsPropertyCurvature::getAbsCurvatureDir( Mode tMode) const
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

// ----------------------------------------------------------------------------

void PointsWithProperty::transform(const Matrix4D &rclMat)
{
  DataWithPropertyBag::transform(rclMat);
  for (std::vector<Vector3D>::iterator it = _Points.begin(); it != _Points.end(); ++it)
  {
    *it = rclMat * (*it);
  }
}

void PointsWithProperty::Save (Base::Writer &writer)
{
  writer << writer.ind() << "<Points Count=\"" << _Points.size() << "\">" << std::endl;

  writer.incInd();
  for (PointKernel::const_iterator itp = _Points.begin(); itp != _Points.end(); ++itp)
  {
    writer << writer.ind() << "<P "
                            << "x=\"" <<  itp->x << "\" "
                            << "y=\"" <<  itp->y << "\" "
                            << "z=\"" <<  itp->z << "\"/>" 
                            << std::endl;
  }
  writer.decInd();
  writer << writer.ind() << "</Points>" << std::endl;

}

void PointsWithProperty::Restore(Base::XMLReader &reader)
{ 
  int Cnt,i;
 
  reader.readElement("Points");
  Cnt = reader.getAttributeAsInteger("Count");

  _Points.resize(Cnt);
  for(i=0 ;i<Cnt ;i++)
  {
    reader.readElement("P");
    _Points[i].x = (float) reader.getAttributeAsFloat("x");
    _Points[i].y = (float) reader.getAttributeAsFloat("y");
    _Points[i].z = (float) reader.getAttributeAsFloat("z");
     

  }
  reader.readEndElement("Points");

}

