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

#include "MeshCurvature.h"
#include "Mesh.h"
#include "Core/MeshKernel.h"
#include "Core/Tools.h"
#include "Core/Iterator.h"
#include "Core/Visitor.h"
#include "Core/Approximation.h"

#include <Base/Sequencer.h>

#ifndef _PreComp_
# include <Wm3Vector3.h>
# include <Wm3MeshCurvature.h>
#endif

using namespace Mesh;
using namespace MeshCore;

MeshCurvaturePropertyError::MeshCurvaturePropertyError(const char * sMessage)
  : Base::Exception(sMessage)
{
}

MeshCurvaturePropertyError::MeshCurvaturePropertyError(void)
{
  _sErrMsg = "No MeshCurvatureProperty found";
}

MeshKernelError::MeshKernelError(const char * sMessage)
  : Base::Exception(sMessage)
{
}

MeshKernelError::MeshKernelError(void)
{
  _sErrMsg = "No MeshKernel found";
}

// -------------------------------------------------------------------------------

MeshCurvature::MeshCurvature( MeshWithProperty& rclMesh )
  : _rclMesh(rclMesh), _curvature(0L)
{
  bool found = false;
  std::list<App::PropertyBag*> bag = _rclMesh.GetAllOfType("Curvature");
  for ( std::list<App::PropertyBag*>::iterator it = bag.begin(); it != bag.end(); ++it )
  {
    if ( typeid(**it) == typeid(MeshPropertyCurvature) )
    {
      found = true;
      break;
    }
  }

  if ( !found )
    throw MeshCurvaturePropertyError();

  MeshKernel* pMesh = _rclMesh.getKernel();
  if ( !pMesh )
    throw MeshKernelError(); // no mesh kernel
}

// -------------------------------------------------------------------------------

MeshCurvaturePerVertex::MeshCurvaturePerVertex( MeshWithProperty& rclMesh )
  :MeshCurvature( rclMesh )
{
  MeshPropertyCurvature *prop = dynamic_cast<MeshPropertyCurvature*>(_rclMesh.Get("VertexCurvature") );
  if ( !prop )
    throw MeshCurvaturePropertyError();
  _curvature = prop;
}

MeshCurvaturePerVertex::~MeshCurvaturePerVertex()
{
}

bool MeshCurvaturePerVertex::Compute()
{
  // get all points
  MeshKernel* pMesh = _rclMesh.getKernel();
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
      if ( idx < 0)
        throw;
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
  for ( unsigned long i=0; i<pMesh->CountPoints(); i++ )
  {
    MeshPropertyCurvature::fCurvature info;
    info.fMaxCurvature = aMaxCurv[i];
    info.fMinCurvature = aMinCurv[i];
    info.cMaxCurvDir = Vector3D( aMaxCurvDir[i].X(), aMaxCurvDir[i].Y(), aMaxCurvDir[i].Z() );
    info.cMinCurvDir = Vector3D( aMinCurvDir[i].X(), aMinCurvDir[i].Y(), aMinCurvDir[i].Z() );
    _curvature->setValue(i, info);
  }

  return true;
}

// -------------------------------------------------------------------------------

MeshCurvaturePerFace::MeshCurvaturePerFace( MeshWithProperty& rclMesh )
  :MeshCurvature( rclMesh )
{
  MeshPropertyCurvature *prop = dynamic_cast<MeshPropertyCurvature*>(_rclMesh.Get("FaceCurvature") );
  if ( !prop )
    throw MeshCurvaturePropertyError();
  _curvature = prop;
}

MeshCurvaturePerFace::~MeshCurvaturePerFace()
{
}

unsigned long MeshCurvaturePerFace::RequiredPoints() const
{
  return 12;
}

void MeshCurvaturePerFace::ExtractCurvature(unsigned long pos, const std::vector<Vector3D>& points, 
                                            const Vector3D& normal )
{
  MeshSurfaceFit cSurfaceFitter;   // Surface-Fitter
  float fCurv1, fCurv2;            // Principal curvature
  float dDistance;                 // Quality of fit
  Vector3D cCurvDir1, cCurvDir2;   // Directions of principal curvature

  // Initialize
  cSurfaceFitter.Clear();
  if ( points.size() >= RequiredPoints() )
  {
    cSurfaceFitter.AddPoint(points);
    cSurfaceFitter.Fit();

    if (cSurfaceFitter.GetCurvatureInfo(0.0, 0.0, 0.0, fCurv1, fCurv2, cCurvDir1, cCurvDir2, dDistance) == true)
    {
    }
  }

  // Harmonize normals of surface and curvature directions
  // by cross product
  //
  if (cSurfaceFitter.GetNormal() * normal < 0.0f)
  {
    fCurv1 *= -1.0;
    fCurv2 *= -1.0;
    cCurvDir1 = cCurvDir1 * -1.0;
    cCurvDir2 = cCurvDir2 * -1.0;
  }

  // sort the two curvatures by value
  if (fabs(fCurv1) < fabs(fCurv2))
  {
    float tmp;
    Vector3D tmpVec;

    tmp = fCurv1;
    fCurv1 = fCurv2;
    fCurv2 = tmp;

    tmpVec = cCurvDir1;
    cCurvDir1 = cCurvDir2;
    cCurvDir2 = tmpVec;
  }

  // override curvature info
  MeshPropertyCurvature::fCurvature info;
  info.fMaxCurvature = fCurv1;
  info.fMinCurvature = fCurv2;
  info.cMaxCurvDir = cCurvDir1;
  info.cMinCurvDir = cCurvDir2;
  _curvature->setValue(pos, info);
}

bool MeshCurvaturePerFace::Compute()
{
  MeshKernel* pMesh = _rclMesh.getKernel();
  unsigned long ulMinPoints=RequiredPoints();

  // iterate over all facets
  MeshSearchNeighbours cMeshNeighbours(*pMesh, 1000.0f);

  unsigned long uCntFaces = pMesh->CountFacets();
  MeshFacetIterator cIt( *pMesh );

  Base::Sequencer().start("Calc curvature...", uCntFaces);

  for ( unsigned long i=0; i<uCntFaces; i++ )
  {
    cIt.Set( i );
    std::vector<Vector3D> clResult;
    cMeshNeighbours.NeighboursFromFacet(i, /*_fRadius*/2.0f, ulMinPoints, clResult);

    // make sure we have enough points
    if ( clResult.size() >= ulMinPoints )
    {
      ExtractCurvature( i, clResult, cIt->GetNormal() );
    }

    Base::Sequencer().next();
  }

  Base::Sequencer().stop();

  return true;
}
