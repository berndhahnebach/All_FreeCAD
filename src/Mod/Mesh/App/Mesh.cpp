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

#include "Mesh.h"
#include "Core/MeshKernel.h"

using namespace Mesh;


// ----------------------------------------------------------------------------

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

