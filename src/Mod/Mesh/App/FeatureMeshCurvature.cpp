/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <fcntl.h>
# include <ios>
# include <Wm3Vector3.h>
# include <Wm3MeshCurvature.h>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Matrix.h>
#include <Base/Sequencer.h>
#include "FeatureMeshCurvature.h"

#include "Core/Elements.h"
#include "Core/Iterator.h"



using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Curvature, Mesh::Feature)



Curvature::Curvature(void)
{
  ADD_PROPERTY(Source,(0));
  ADD_PROPERTY(CurvMaxDir,(Base::Vector3D()));
  ADD_PROPERTY(CurvMinDir,(Base::Vector3D()));
  ADD_PROPERTY(CurvMaxVal,(0.0f));
  ADD_PROPERTY(CurvMinVal,(0.0f));
}

int Curvature::execute(void)
{
  Feature *pcFeat  = dynamic_cast<Feature*>(Source.getValue());
  if(!pcFeat || pcFeat->getStatus() != Valid)
    return 1;
 
  // get all points
  MeshWithProperty& rMesh = pcFeat->getMesh();
  MeshKernel* pMesh = rMesh.getKernel();
  std::vector< Wm3::Vector3<float> > aPnts;
  MeshPointIterator cPIt( *pMesh );
  for ( cPIt.Init(); cPIt.More(); cPIt.Next() )
  {
    Wm3::Vector3<float> cP( cPIt->x, cPIt->y, cPIt->z );
    aPnts.push_back( cP );
  }

  // get all point connections
  std::vector<int> aIdx;
  const std::vector<MeshFacet>& raFts = pMesh->GetFacets();
  for ( std::vector<MeshFacet>::const_iterator it = raFts.begin(); it != raFts.end(); ++it )
  {
    for (int i=0; i<3; i++)
    {
      int idx = (int)it->_aulPoints[i];
      aIdx.push_back( (int)it->_aulPoints[i] );
    }
  }

  // compute vertex based curvatures
  Wm3::MeshCurvature<float> meshCurv(pMesh->CountPoints(), &(aPnts[0]), pMesh->CountFacets(), &(aIdx[0]));

  // get curvature information now
  const Wm3::Vector3<float>* aMaxCurvDir = meshCurv.GetMaxDirections();
  const Wm3::Vector3<float>* aMinCurvDir = meshCurv.GetMinDirections();
  const float* aMaxCurv = meshCurv.GetMaxCurvatures();
  const float* aMinCurv = meshCurv.GetMinCurvatures();

  CurvMaxVal.setSize(pMesh->CountPoints()); CurvMinVal.setSize(pMesh->CountPoints());
  CurvMaxDir.setSize(pMesh->CountPoints()); CurvMinDir.setSize(pMesh->CountPoints());
  for ( unsigned long i=0; i<pMesh->CountPoints(); i++ )
  {
    CurvMaxVal.set1Value(i, aMaxCurv[i]);
    CurvMinVal.set1Value(i, aMinCurv[i]);
    CurvMaxDir.set1Value(i, Vector3D( aMaxCurvDir[i].X(), aMaxCurvDir[i].Y(), aMaxCurvDir[i].Z() ) );
    CurvMinDir.set1Value(i, Vector3D( aMinCurvDir[i].X(), aMinCurvDir[i].Y(), aMinCurvDir[i].Z() ) );
  }

  return 0;
}

void Curvature::transform(const Base::Matrix4D &mat)
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
  for (int ii=0; ii<CurvMaxDir.getSize(); ii++)
  {
    CurvMaxDir.set1Value(ii, rot * CurvMaxDir[ii]);
    CurvMinDir.set1Value(ii, rot * CurvMinDir[ii]);
  }
}

MeshWithProperty& Curvature::getMesh()
{
  Feature *pcFeat  = dynamic_cast<Feature*>(Source.getValue());
  if(!pcFeat || pcFeat->getStatus() != Valid)
    return Feature::getMesh();

  return pcFeat->getMesh();
}
