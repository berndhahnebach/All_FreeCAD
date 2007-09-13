/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
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

#include <Mod/Mesh/App/WildMagic4/Wm4IntrSegment3Plane3.h>
#include <Mod/Mesh/App/WildMagic4/Wm4IntrSegment3Box3.h>
#include <Mod/Mesh/App/WildMagic4/Wm4DistVector3Triangle3.h>
#include <Mod/Mesh/App/WildMagic4/Wm4DistSegment3Triangle3.h>

#include "Elements.h"
#include "Algorithm.h"
#include "tritritest.h"

using namespace MeshCore;
using namespace Wm4;

unsigned long MeshPointArray::Get (const MeshPoint &rclPoint)
{
  iterator clIter;

  clIter = std::find(begin(), end(), rclPoint);
  if (clIter != end())
    return clIter - begin();
  else
    return ULONG_MAX;  
}

unsigned long MeshPointArray::GetOrAddIndex (const MeshPoint &rclPoint)
{
  unsigned long ulIndex;

  if ((ulIndex = Get(rclPoint)) == ULONG_MAX)
  {
    push_back(rclPoint);
    return (unsigned long)(size() - 1);
  }
  else
    return ulIndex;
}

void MeshPointArray::SetFlag (MeshPoint::TFlagType tF) const
{
  for (MeshPointArray::_TConstIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

void MeshPointArray::ResetFlag (MeshPoint::TFlagType tF) const
{
  for (MeshPointArray::_TConstIterator i = begin(); i < end(); i++) i->ResetFlag(tF);
}

void MeshPointArray::SetProperty (unsigned long ulVal) const
{
  for (_TConstIterator pP = begin(); pP != end(); pP++) pP->SetProperty(ulVal);
}

void MeshPointArray::ResetInvalid (void) const
{
  for (_TConstIterator pP = begin(); pP != end(); pP++) pP->ResetInvalid();
}

MeshPointArray& MeshPointArray::operator = (const MeshPointArray &rclPAry)
{
//  std::vector<MeshPoint>::operator=(rclPAry);
  TMeshPointArray::operator=(rclPAry);

  return *this;
}

void MeshFacetArray::Erase (_TIterator pIter)
{
  unsigned long i, *pulN;
  _TIterator  pPass, pEnd;
  unsigned long ulInd = pIter - begin();
  erase(pIter);
  pPass = begin();
  pEnd  = end();
  while (pPass < pEnd)
  {
    for (i = 0; i < 3; i++)
    {
      pulN = &pPass->_aulNeighbours[i];
      if ((*pulN > ulInd) && (*pulN != ULONG_MAX))
        (*pulN)--;
    }
    pPass++;
  }
}

void MeshFacetArray::TransposeIndices (unsigned long ulOrig, unsigned long ulNew)
{
  _TIterator  pIter = begin(), pEnd = end();

  while (pIter < pEnd)
  {
    pIter->Transpose(ulOrig, ulNew);
    ++pIter;  
  }
}

void MeshFacetArray::DecrementIndices (unsigned long ulIndex)
{
  _TIterator  pIter = begin(), pEnd = end();

  while (pIter < pEnd)
  {
    pIter->Decrement(ulIndex);
    ++pIter;  
  }
}

void MeshFacetArray::SetFlag (MeshFacet::TFlagType tF) const
{
  for (MeshFacetArray::_TConstIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

void MeshFacetArray::ResetFlag (MeshFacet::TFlagType tF) const
{
  for (MeshFacetArray::_TConstIterator i = begin(); i < end(); i++) i->ResetFlag(tF);
}

void MeshFacetArray::SetProperty (unsigned long ulVal) const
{
  for (_TConstIterator pF = begin(); pF != end(); pF++) pF->SetProperty(ulVal);
}

void MeshFacetArray::ResetInvalid (void) const
{
  for (_TConstIterator pF = begin(); pF != end(); pF++) pF->ResetInvalid();
}

MeshFacetArray& MeshFacetArray::operator = (const MeshFacetArray &rclFAry)
{
  TMeshFacetArray::operator=(rclFAry);
  return *this;
}

// -----------------------------------------------------------------

bool MeshGeomEdge::ContainedByOrIntersectBoundingBox ( const Base::BoundBox3f &rclBB ) const
{
  // Test, ob alle Eckpunkte der Edge sich auf einer der 6 Seiten der BB befinden
  if ((GetBoundBox() && rclBB) == false)
    return false;

  // Test, ob Edge-BB komplett in BB liegt
  if (rclBB.IsInBox(GetBoundBox()))
    return true;

  // Test, ob einer der Eckpunkte in BB liegt
  for (int i=0;i<2;i++)
  {
    if (rclBB.IsInBox(_aclPoints[i]))
      return true;
  }

  // "echter" Test auf Schnitt
  if (IntersectBoundingBox(rclBB))
    return true;

  return false;
}

Base::BoundBox3f MeshGeomEdge::GetBoundBox () const
{
  return Base::BoundBox3f(_aclPoints,2);
}

bool MeshGeomEdge::IntersectBoundingBox (const Base::BoundBox3f &rclBB) const
{
  const Base::Vector3f& rclP0 = _aclPoints[0];
  const Base::Vector3f& rclP1 = _aclPoints[1];

  Vector3<float> A(rclP0.x, rclP0.y, rclP0.z);
  Vector3<float> B(rclP1.x, rclP1.y, rclP1.z);

  Vector3<float> n = B - A;
  float len = n.Length();
  n.Normalize();
  Vector3<float> p = 0.5f*(A + B);

  Segment3<float> akSeg(p, n, 0.5f*len);

  Base::Vector3f clCenter  = rclBB.CalcCenter();
  Vector3<float> center(clCenter.x, clCenter.y, clCenter.z);
  Vector3<float> axis0(1.0f, 0.0f, 0.0f);
  Vector3<float> axis1(0.0f, 1.0f, 0.0f);
  Vector3<float> axis2(0.0f, 0.0f, 1.0f);
  float extent0 = 0.5f*rclBB.LengthX();
  float extent1 = 0.5f*rclBB.LengthY();
  float extent2 = 0.5f*rclBB.LengthZ();

  Box3<float> kBox(center, axis0, axis1, axis2, extent0, extent1, extent2);

  IntrSegment3Box3<float> intrsectbox(akSeg, kBox, false);
  return intrsectbox.Test();
}

// -----------------------------------------------------------------

MeshGeomFacet::MeshGeomFacet (void) 
  : _bNormalCalculated(false),
    _ucFlag(0), _ulProp(0)
{ 

}


MeshGeomFacet::MeshGeomFacet (const Base::Vector3f &v1,const Base::Vector3f &v2,const Base::Vector3f &v3)
  : _bNormalCalculated(false), 
    _ucFlag(0),
    _ulProp(0)
{
  _aclPoints[0] = v1;
  _aclPoints[1] = v2;
  _aclPoints[2] = v3;
}



bool MeshGeomFacet::IsPointOf (const Base::Vector3f &rclPoint, float fDistance) const
{
  if (DistancePlaneToPoint(rclPoint) > fDistance)
    return false;

  // force internal normal to be computed if not done yet
  Base::Vector3f clNorm(GetNormal()), clProjPt(rclPoint), clEdge;
  Base::Vector3f clP0(_aclPoints[0]), clP1(_aclPoints[1]), clP2(_aclPoints[2]);
  float     fLP, fLE;

  clNorm.Normalize();
  clProjPt.ProjToPlane(_aclPoints[0], clNorm);

    
  // Kante P0 --> P1
  clEdge = clP1 - clP0;
  fLP = clProjPt.DistanceToLine(clP0, clEdge); 
  if (fLP > 0.0f)
  {
    fLE = clP2.DistanceToLine(clP0, clEdge);
    if (fLP <= fLE)
    {
      if (clProjPt.DistanceToLine(clP2, clEdge) > fLE)
        return false;
    }
    else
      return false;
  }      

  // Kante P0 --> P2
  clEdge = clP2 - clP0;
  fLP = clProjPt.DistanceToLine(clP0, clEdge); 
  if (fLP > 0.0f)
  {
    fLE = clP1.DistanceToLine(clP0, clEdge);
    if (fLP <= fLE)
    {
      if (clProjPt.DistanceToLine(clP1, clEdge) > fLE) 
        return false;
    }
    else
      return false;
  }      

  // Kante P1 --> P2
  clEdge = clP2 - clP1;
  fLP = clProjPt.DistanceToLine(clP1, clEdge); 
  if (fLP > 0.0f)
  {
    fLE = clP0.DistanceToLine(clP1, clEdge);
    if (fLP <= fLE)
    {
      if (clProjPt.DistanceToLine(clP0, clEdge) > fLE) 
        return false;
    }
    else
      return false;
  }      

  return true;
}

bool MeshGeomFacet::IsPointOfFace (const Base::Vector3f& rclP, float fDistance) const
{
  // effektivere Implementierung als in MeshGeomFacet::IsPointOf
  //
  Base::Vector3f a(_aclPoints[0].x, _aclPoints[0].y, _aclPoints[0].z);
  Base::Vector3f b(_aclPoints[1].x, _aclPoints[1].y, _aclPoints[1].z);
  Base::Vector3f c(_aclPoints[2].x, _aclPoints[2].y, _aclPoints[2].z);
  Base::Vector3f p(rclP);

  Base::Vector3f n  = (b - a) % (c - a);
  Base::Vector3f n1 = (a - p) % (b - p);
  Base::Vector3f n2 = (c - p) % (a - p);
  Base::Vector3f n3 = (b - p) % (c - p);

  if (n * (p - a) > fDistance * n.Length())
    return false;

  if (n * (a - p) > fDistance * n.Length())
    return false;

  if (n * n1 <= 0.0f)
    return false;

  if (n * n2 <= 0.0f)
    return false;

  if (n * n3 <= 0.0f)
    return false;

  return true;
}

bool MeshGeomFacet::Weights(const Base::Vector3f& rclP, float& w0, float& w1, float& w2) const
{
  float fAreaABC = Area();
  float fAreaPBC = MeshGeomFacet(rclP,_aclPoints[1],_aclPoints[2]).Area();
  float fAreaPCA = MeshGeomFacet(rclP,_aclPoints[2],_aclPoints[0]).Area();
  float fAreaPAB = MeshGeomFacet(rclP,_aclPoints[0],_aclPoints[1]).Area();

  w0=fAreaPBC/fAreaABC;
  w1=fAreaPCA/fAreaABC;
  w2=fAreaPAB/fAreaABC;

  return fabs(w0+w1+w2-1.0f)<0.001f;
}

void MeshGeomFacet::ProjectPointToPlane (Base::Vector3f &rclPoint) const
{
  rclPoint.ProjToPlane(_aclPoints[0], GetNormal());
}

void MeshGeomFacet::ProjectFacetToPlane (MeshGeomFacet &rclFacet) const
{
  // project facet 2 onto facet 1
  IntersectPlaneWithLine( rclFacet._aclPoints[0], GetNormal(), rclFacet._aclPoints[0] );
  IntersectPlaneWithLine( rclFacet._aclPoints[1], GetNormal(), rclFacet._aclPoints[1] );
  IntersectPlaneWithLine( rclFacet._aclPoints[2], GetNormal(), rclFacet._aclPoints[2] );
}

void MeshGeomFacet::Enlarge (float fDist)
{
  Base::Vector3f  clM, clU, clV, clPNew[3];
  float      fA, fD;
  unsigned long i, ulP1, ulP2, ulP3;

  for (i = 0; i < 3; i++)
  {
    ulP1  = i;
    ulP2  = (i + 1) % 3;
    ulP3  = (i + 2) % 3;
    clU   = _aclPoints[ulP2] - _aclPoints[ulP1];
    clV   = _aclPoints[ulP3] - _aclPoints[ulP1];
    clM   = -(clU + clV);
    fA    = clM.GetAngle(-clU);
    fD    = fDist / float(sin(fA));
    clM.Normalize();
    clM.Scale(fD, fD, fD);
    clPNew[ulP1] = _aclPoints[ulP1] + clM;
  }

  _aclPoints[0] = clPNew[0];
  _aclPoints[1] = clPNew[1];
  _aclPoints[2] = clPNew[2];
}

bool MeshGeomFacet::IsDegenerated() const
{
  return (Area() < FLOAT_EPS);
}

bool MeshGeomFacet::IsDeformed() const
{
  float fCosAngle;
  Base::Vector3f u,v;

  for (int i=0; i<3; i++)
  {
    u = _aclPoints[(i+1)%3]-_aclPoints[i];
    v = _aclPoints[(i+2)%3]-_aclPoints[i];
    u.Normalize();
    v.Normalize();

    fCosAngle = u * v;

    // x < 30° => cos(x) > sqrt(3)/2 or x > 120° => cos(x) < -0.5
    if (fCosAngle > 0.86f || fCosAngle < -0.5f)
      return true;
  }

  return false;
}

bool MeshGeomFacet::IntersectBoundingBox ( const Base::BoundBox3f &rclBB ) const
{
  // the triangle's corner points
  const Base::Vector3f& v0 = _aclPoints[0];
  const Base::Vector3f& v1 = _aclPoints[1];
  const Base::Vector3f& v2 = _aclPoints[2];

  // first check if at least one point is inside the box
  if ( rclBB.IsInBox( v0 ) || rclBB.IsInBox( v1 ) || rclBB.IsInBox( v2 ) )
    return true;

  // edge lengths
  float len0 = (v0-v1).Length();
  float len1 = (v1-v2).Length();
  float len2 = (v2-v0).Length();

  // Build up the line segments
  Vector3<float> p0(0.5f*(v0.x+v1.x), 0.5f*(v0.y+v1.y), 0.5f*(v0.z+v1.z));
  Vector3<float> p1(0.5f*(v1.x+v2.x), 0.5f*(v1.y+v2.y), 0.5f*(v1.z+v2.z));
  Vector3<float> p2(0.5f*(v2.x+v0.x), 0.5f*(v2.y+v0.y), 0.5f*(v2.z+v0.z));

  Vector3<float> d0(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
  d0.Normalize();
  Vector3<float> d1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
  d1.Normalize();
  Vector3<float> d2(v0.x - v2.x, v0.y - v2.y, v0.z - v2.z);
  d2.Normalize();

  Segment3<float> akSeg0(p0, d0, len0/2.0f  );
  Segment3<float> akSeg1(p1, d1, len1/2.0f);
  Segment3<float> akSeg2(p2, d2, len2/2.0f);

  // Build up the box
  Base::Vector3f clCenter  = rclBB.CalcCenter();
  Vector3<float> center(clCenter.x, clCenter.y, clCenter.z);
  Vector3<float> axis0(1.0f, 0.0f, 0.0f);
  Vector3<float> axis1(0.0f, 1.0f, 0.0f);
  Vector3<float> axis2(0.0f, 0.0f, 1.0f);
  float extent0 = 0.5f*rclBB.LengthX();
  float extent1 = 0.5f*rclBB.LengthY();
  float extent2 = 0.5f*rclBB.LengthZ();

  Box3<float> akBox(center, axis0, axis1, axis2, extent0, extent1, extent2);

  // Check for intersection of line segments and box
  IntrSegment3Box3<float> akSec0(akSeg0, akBox, false);
  if ( akSec0.Test() )
    return true;
  IntrSegment3Box3<float> akSec1(akSeg1, akBox, false);
  if ( akSec1.Test() )
    return true;
  IntrSegment3Box3<float> akSec2(akSeg2, akBox, false);
  if ( akSec2.Test() )
    return true;

  // no intersection
  return false;
}

bool MeshGeomFacet::IntersectWithPlane (const Base::Vector3f &rclBase, const Base::Vector3f &rclNormal, Base::Vector3f &rclP1, Base::Vector3f &rclP2) const
{
  // the triangle's corner points
  const Base::Vector3f& v0 = _aclPoints[0];
  const Base::Vector3f& v1 = _aclPoints[1];
  const Base::Vector3f& v2 = _aclPoints[2];

  // edge lengths
  float len0 = (v0-v1).Length();
  float len1 = (v1-v2).Length();
  float len2 = (v2-v0).Length();

  // Build up the line segments
  Vector3<float> p0(0.5f*(v0.x+v1.x), 0.5f*(v0.y+v1.y), 0.5f*(v0.z+v1.z));
  Vector3<float> p1(0.5f*(v1.x+v2.x), 0.5f*(v1.y+v2.y), 0.5f*(v1.z+v2.z));
  Vector3<float> p2(0.5f*(v2.x+v0.x), 0.5f*(v2.y+v0.y), 0.5f*(v2.z+v0.z));

  Vector3<float> d0(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
  d0.Normalize();
  Vector3<float> d1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
  d1.Normalize();
  Vector3<float> d2(v0.x - v2.x, v0.y - v2.y, v0.z - v2.z);
  d2.Normalize();

  Segment3<float> akSeg0(p0, d0, len0/2.0f  );
  Segment3<float> akSeg1(p1, d1, len1/2.0f);
  Segment3<float> akSeg2(p2, d2, len2/2.0f);

  // Build up the plane
  Vector3<float> p(rclBase.x, rclBase.y, rclBase.z);
  Vector3<float> n(rclNormal.x, rclNormal.y, rclNormal.z);
  Plane3<float> akPln(n, p);

  // Check for intersection with plane for each line segment
  IntrSegment3Plane3<float> test0(akSeg0, akPln);
  IntrSegment3Plane3<float> test1(akSeg1, akPln);
  IntrSegment3Plane3<float> test2(akSeg2, akPln);

  Vector3<float> intr;
  if ( test0.Find() )
  {
    intr = p0 + test0.GetSegmentT() * d0; 
    rclP1.Set( intr[0], intr[1], intr[2]);

    if ( test1.Find() )
    {
      intr = p1 + test1.GetSegmentT() * d1; 
      rclP2.Set( intr[0], intr[1], intr[2]);
      return true;
    }
    else if ( test2.Find() )
    {
      intr = p2 + test2.GetSegmentT() * d2; 
      rclP2.Set( intr[0], intr[1], intr[2]);
      return true;
    }
  }
  else if ( test1.Find() )
  {
    intr = p1 + test1.GetSegmentT() * d1; 
    rclP1.Set( intr[0], intr[1], intr[2]);

    if ( test2.Find() )
    {
      intr = p2 + test2.GetSegmentT() * d2; 
      rclP2.Set( intr[0], intr[1], intr[2]);
      return true;
    }
  }

  return false;
}

bool MeshGeomFacet::Foraminate (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, Base::Vector3f &rclRes, float fMaxAngle) const
{
  const float fTolerance = 1e-06f;
  Base::Vector3f clFacetNormal = GetNormal();

  // check angle between facet normal and the line direction
  if (rclDir.GetAngle(clFacetNormal) > fMaxAngle)
    return false;

  float fDenominator = clFacetNormal * rclDir;
  float fLLenSqr     = rclDir * rclDir;
  float fNLenSqr     = clFacetNormal * clFacetNormal;

  // the line mustn't be parallel to the facet
  if ((fDenominator * fDenominator) <= (fTolerance * fLLenSqr * fNLenSqr))
    return false;

  Base::Vector3f clE0 = _aclPoints[1] - _aclPoints[0];
  Base::Vector3f clE1 = _aclPoints[2] - _aclPoints[0];
  
  Base::Vector3f clDiff0 = rclPt - _aclPoints[0];
  float fTime   = -(clFacetNormal * clDiff0) / fDenominator;
  Base::Vector3f  clDiff1 = clDiff0 + fTime * rclDir;

  float fE00 = clE0 * clE0;
  float fE01 = clE0 * clE1;
  float fE11 = clE1 * clE1;
  float fDet = float(fabs((fE00 * fE11) - (fE01 * fE01)));
  float fR0  = clE0 * clDiff1;
  float fR1  = clE1 * clDiff1;

  float fS0  = (fE11 * fR0) - (fE01 * fR1);
  float fS1  = (fE00 * fR1) - (fE01 * fR0);

  if ((fS0 >= 0.0f) && (fS1 >= 0.0f) && ((fS0 + fS1) <= fDet))
  {
    rclRes = clDiff1 + _aclPoints[0];
    return true;
  }
  else
    return false;
}

bool MeshGeomFacet::IntersectPlaneWithLine (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, Base::Vector3f &rclRes) const
{
  // berechne den Schnittpunkt Gerade <-> Ebene
  if ( fabs(rclDir * GetNormal()) < 1e-3f )
    return false; // line and plane are parallel

  float s = ( ( GetGravityPoint() - rclPt ) * GetNormal() ) 
            / ( rclDir * GetNormal() );
  rclRes = rclPt + s * rclDir;

  return true;
}

bool MeshGeomFacet::IntersectWithLine (const Base::Vector3f &rclPt, const Base::Vector3f &rclDir, Base::Vector3f &rclRes) const
{
  if ( !IntersectPlaneWithLine( rclPt, rclDir, rclRes ) )
    return false; // line and plane are parallel
  // Check if the intersection point is inside the facet
  return IsPointOfFace(rclRes, 1e-03f);
}

float MeshGeomFacet::DistanceToLineSegment (const Base::Vector3f &rclP1, const Base::Vector3f &rclP2) const
{
  // line segment
  Vector3<float> A(rclP1.x, rclP1.y, rclP1.z);
  Vector3<float> B(rclP2.x, rclP2.y, rclP2.z);

  Vector3<float> n = B - A;
  float len = n.Length();
  n.Normalize();
  Vector3<float> p = 0.5f*(A + B);

  Segment3<float> akSeg(p, n, 0.5f*len);

  // triangle
  Vector3<float>  akF0(_aclPoints[0].x, _aclPoints[0].y, _aclPoints[0].z);
  Vector3<float>  akF1(_aclPoints[1].x, _aclPoints[1].y, _aclPoints[1].z);
  Vector3<float>  akF2(_aclPoints[2].x, _aclPoints[2].y, _aclPoints[2].z);

  Triangle3<float> akTria(akF0, akF1, akF2);

  DistSegment3Triangle3<float> akDistSegTria(akSeg, akTria);
  return akDistSegTria.Get();
}

float MeshGeomFacet::DistanceToPoint (const Base::Vector3f &rclPt, Base::Vector3f &rclNt) const
{
  Vector3<float>  akPt(rclPt.x, rclPt.y, rclPt.z);
  Vector3<float>  akF0(_aclPoints[0].x, _aclPoints[0].y, _aclPoints[0].z);
  Vector3<float>  akF1(_aclPoints[1].x, _aclPoints[1].y, _aclPoints[1].z);
  Vector3<float>  akF2(_aclPoints[2].x, _aclPoints[2].y, _aclPoints[2].z);

  Triangle3<float> akTria(akF0, akF1, akF2);
  DistVector3Triangle3<float> akDistPtTria(akPt, akTria);
  
  float fDist = akDistPtTria.Get();

  // get nearest point of the facet
  Vector3<float> akNt = akDistPtTria.GetClosestPoint1();
  rclNt.Set(akNt.X(), akNt.Y(), akNt.Z());

  return fDist;
}

void MeshGeomFacet::SubSample (float fStep, std::vector<Base::Vector3f> &rclPoints) const
{
  std::vector<Base::Vector3f> clPoints;
  Base::Vector3f A = _aclPoints[0], B = _aclPoints[1], C = _aclPoints[2];
  Base::Vector3f clVecAB(B - A);
  Base::Vector3f clVecAC(C - A);
  Base::Vector3f clVecBC(C - B);

  // laengste Achse entspricht AB
  float fLenAB = clVecAB.Length();
  float fLenAC = clVecAC.Length();
  float fLenBC = clVecBC.Length();

  if (fLenAC > fLenAB)
  {
    std::swap(B, C);
    std::swap(fLenAB, fLenAC);
  }
  if (fLenBC > fLenAB)
  {
    std::swap(A, C);
    std::swap(fLenBC, fLenAB);
  }

  clVecAB = (B - A);
  clVecAC = (C - A);
  clVecBC = (C - B);
  Base::Vector3f clVecABNorm(clVecAB);
  Base::Vector3f clVecHNorm((clVecAB % clVecAC) % clVecAB);
  clVecABNorm.Normalize();
  clVecHNorm.Normalize();

  float bx = fLenAB;                
  float cy = float(sin(clVecAB.GetAngle(clVecAC)) * fLenAC);
  float cx = float(sqrt(fabs(fLenAC * fLenAC - cy * cy)));
 
  float fDetABC = bx*cy;

  for (float px = (fStep / 2.0f); px < fLenAB; px += fStep)
  {
    for (float py = (fStep / 2.0f); py < cy; py += fStep)
    {
      float u = (bx*cy + cx*py - px*cy - bx*py) / fDetABC;
      float v = (px*cy - cx*py) / fDetABC;
      float w = (bx*py) / fDetABC;

      if ((u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f) && ((u + v) < 1.0f))
      {
       // rclPoints.push_back(CBase::Vector3f(u*A + v*B + w*C));
        Base::Vector3f clV = A + (px * clVecABNorm) + (py * clVecHNorm);
        clPoints.push_back(clV);

      }
      else 
        break;
    }
  }

  // if couldn't subsample the facet take gravity center
  if (clPoints.size() == 0)
    clPoints.push_back((A + B + C) / 3.0f);

  rclPoints.insert(rclPoints.end(), clPoints.begin(), clPoints.end());
}

/**
 * Fast Triangle-Triangle Intersection Test by Tomas Möller
 * http://www.acm.org/jgt/papers/Moller97/tritri.html
 * http://www.cs.lth.se/home/Tomas_Akenine_Moller/code/
 */
bool MeshGeomFacet::IntersectWithFacet(const MeshGeomFacet &rclFacet) const
{
  float V[3][3], U[3][3];
  for (int i = 0; i < 3; i++)
  {
    V[i][0] = _aclPoints[i].x;
    V[i][1] = _aclPoints[i].y;
    V[i][2] = _aclPoints[i].z;
    U[i][0] = rclFacet._aclPoints[i].x;
    U[i][1] = rclFacet._aclPoints[i].y;
    U[i][2] = rclFacet._aclPoints[i].z;
  }

  if (tri_tri_intersect(V[0], V[1], V[2], U[0], U[1], U[2]) == 0)
    return false; // no intersections
  return true;
}

/**
 * Fast Triangle-Triangle Intersection Test by Tomas Möller
 * http://www.acm.org/jgt/papers/Moller97/tritri.html
 * http://www.cs.lth.se/home/Tomas_Akenine_Moller/code/
 */
int MeshGeomFacet::IntersectWithFacet (const MeshGeomFacet& rclFacet, Base::Vector3f& rclPt0, Base::Vector3f& rclPt1) const
{
  float V[3][3], U[3][3];
  int coplanar = 0;
  float isectpt1[3], isectpt2[3];

  for (int i = 0; i < 3; i++)
  {
    V[i][0] = _aclPoints[i].x;
    V[i][1] = _aclPoints[i].y;
    V[i][2] = _aclPoints[i].z;
    U[i][0] = rclFacet._aclPoints[i].x;
    U[i][1] = rclFacet._aclPoints[i].y;
    U[i][2] = rclFacet._aclPoints[i].z;
  }

  if (tri_tri_intersect_with_isectline(V[0], V[1], V[2], U[0], U[1], U[2], &coplanar, isectpt1, isectpt2) == 0)
    return 0; // no intersections

  rclPt0.x = isectpt1[0]; rclPt0.y = isectpt1[1]; rclPt0.z = isectpt1[2];
  rclPt1.x = isectpt2[0]; rclPt1.y = isectpt2[1]; rclPt1.z = isectpt2[2];

  if (rclPt0 == rclPt1)
    return 1;
  else
    return 2;
}

float MeshGeomFacet::CenterOfInscribedCircle(Base::Vector3f& rclCenter) const
{
  Base::Vector3f p0(_aclPoints[0].x, _aclPoints[0].y, _aclPoints[0].z);
  Base::Vector3f p1(_aclPoints[1].x, _aclPoints[1].y, _aclPoints[1].z);
  Base::Vector3f p2(_aclPoints[2].x, _aclPoints[2].y, _aclPoints[2].z);

  float a = (p1-p2).Length();
  float b = (p2-p0).Length();
  float c = (p0-p1).Length();

  // radius of the circle
  float fRadius = Area();
  fRadius *= 2.0f/(a + b + c); 

  // center of the circle
  rclCenter = (a*p0 + b*p1 + c*p2)/(a + b + c);

  return fRadius;
}

float MeshGeomFacet::CenterOfCircumCircle(Base::Vector3f& rclCenter) const
{
  Base::Vector3f p0(_aclPoints[0].x, _aclPoints[0].y, _aclPoints[0].z);
  Base::Vector3f p1(_aclPoints[1].x, _aclPoints[1].y, _aclPoints[1].z);
  Base::Vector3f p2(_aclPoints[2].x, _aclPoints[2].y, _aclPoints[2].z);

  Base::Vector3f u = (p1-p0);
  Base::Vector3f v = (p2-p1);
  Base::Vector3f w = (p0-p2);

  float uu =   (u * u);
  float vv =   (v * v);
  float ww =   (w * w);
  float uv = - (u * v);
  float vw = - (v * w);
  float uw = - (w * u);

  float w0 = (float)(2 * sqrt(uu * ww - uw * uw) * uw / (uu * ww));
  float w1 = (float)(2 * sqrt(uu * vv - uv * uv) * uv / (uu * vv));
  float w2 = (float)(2 * sqrt(vv * ww - vw * vw) * vw / (vv * ww));

  // center of the circle
  rclCenter = (w0 * p0 +w1 * p1 + w2 *p2) / (w0 + w1 + w2);

  // radius of the circle
  float fRadius = (float)(sqrt(uu * vv * ww) / (4 * Area()));

  return fRadius;
}

unsigned short MeshGeomFacet::NearestEdgeToPoint(const Base::Vector3f& rclPt) const
{
  unsigned short usSide;

  const Base::Vector3f& rcP1 = _aclPoints[0];
  const Base::Vector3f& rcP2 = _aclPoints[1];
  const Base::Vector3f& rcP3 = _aclPoints[2];

  float fD1 = FLOAT_MAX;
  float fD2 = FLOAT_MAX;
  float fD3 = FLOAT_MAX;

  // 1st edge
  Base::Vector3f clDir = rcP2 - rcP1;
  float fLen = Base::Distance(rcP2, rcP1);
  float t = ( ( rclPt - rcP1 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD1 = Base::Distance(rclPt, rcP1);
  else if ( t > 1.0f )
    fD1 = Base::Distance(rclPt, rcP2);
  else
    fD1 = ( ( ( rclPt - rcP1 ) % clDir).Length() ) / fLen;

  // 2nd edge
  clDir = rcP3 - rcP2;
  fLen = Base::Distance(rcP3, rcP2);
  t = ( ( rclPt - rcP2 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD2 = Base::Distance(rclPt, rcP2);
  else if ( t > 1.0f )
    fD2 = Base::Distance(rclPt, rcP3);
  else
    fD2 = ( ( ( rclPt - rcP2 ) % clDir).Length() ) / fLen;

  // 3rd edge
  clDir = rcP1 - rcP3;
  fLen = Base::Distance(rcP1, rcP3);
  t = ( ( rclPt - rcP3 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD3 = Base::Distance(rclPt, rcP3);
  else if ( t > 1.0f )
    fD3 = Base::Distance(rclPt, rcP1);
  else
    fD3 = ( ( ( rclPt - rcP3 ) % clDir).Length() ) / fLen;

  if ( fD1 < fD2 )
  {
    if ( fD1 < fD3 )
    {
      usSide = 0;
    }
    else
    {
      usSide = 2;
    }
  }
  else
  {
    if ( fD2 < fD3 )
    {
      usSide = 1;
    }
    else
    {
      usSide = 2;
    }
  }

  return usSide;
}

void MeshGeomFacet::NearestEdgeToPoint(const Base::Vector3f& rclPt, float& fDistance, unsigned short& usSide) const
{
  const Base::Vector3f& rcP1 = _aclPoints[0];
  const Base::Vector3f& rcP2 = _aclPoints[1];
  const Base::Vector3f& rcP3 = _aclPoints[2];

  float fD1 = FLOAT_MAX;
  float fD2 = FLOAT_MAX;
  float fD3 = FLOAT_MAX;

  // 1st edge
  Base::Vector3f clDir = rcP2 - rcP1;
  float fLen = Base::Distance(rcP2, rcP1);
  float t = ( ( rclPt - rcP1 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD1 = Base::Distance(rclPt, rcP1);
  else if ( t > 1.0f )
    fD1 = Base::Distance(rclPt, rcP2);
  else
    fD1 = ( ( ( rclPt - rcP1 ) % clDir).Length() ) / fLen;

  // 2nd edge
  clDir = rcP3 - rcP2;
  fLen = Base::Distance(rcP3, rcP2);
  t = ( ( rclPt - rcP2 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD2 = Base::Distance(rclPt, rcP2);
  else if ( t > 1.0f )
    fD2 = Base::Distance(rclPt, rcP3);
  else
    fD2 = ( ( ( rclPt - rcP2 ) % clDir).Length() ) / fLen;

  // 3rd edge
  clDir = rcP1 - rcP3;
  fLen = Base::Distance(rcP1, rcP3);
  t = ( ( rclPt - rcP3 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD3 = Base::Distance(rclPt, rcP3);
  else if ( t > 1.0f )
    fD3 = Base::Distance(rclPt, rcP1);
  else
    fD3 = ( ( ( rclPt - rcP3 ) % clDir).Length() ) / fLen;

  if ( fD1 < fD2 )
  {
    if ( fD1 < fD3 )
    {
      usSide = 0;
      fDistance = fD1;
    }
    else
    {
      usSide = 2;
      fDistance = fD3;
    }
  }
  else
  {
    if ( fD2 < fD3 )
    {
      usSide = 1;
      fDistance = fD2;
    }
    else
    {
      usSide = 2;
      fDistance = fD3;
    }
  }
}

float MeshGeomFacet::VolumeOfPrism (const MeshGeomFacet& rclF1) const
{
  Base::Vector3f P1 = this->_aclPoints[0];
  Base::Vector3f P2 = this->_aclPoints[1];
  Base::Vector3f P3 = this->_aclPoints[2];
  Base::Vector3f Q1 = rclF1._aclPoints[0];
  Base::Vector3f Q2 = rclF1._aclPoints[1];
  Base::Vector3f Q3 = rclF1._aclPoints[2];

  if ((P1-Q2).Length() < (P1-Q1).Length())
  {
    Base::Vector3f tmp = Q1;
    Q1 = Q2;
    Q2 = tmp;
  }
  if ((P1-Q3).Length() < (P1-Q1).Length())
  {
    Base::Vector3f tmp = Q1;
    Q1 = Q3;
    Q3 = tmp;
  }
  if ((P2-Q3).Length() < (P2-Q2).Length())
  {
    Base::Vector3f tmp = Q2;
    Q2 = Q3;
    Q3 = tmp;
  }

  Base::Vector3f N1 = (P2-P1) % (P3-P1);
  Base::Vector3f N2 = (P2-P1) % (Q2-P1);
  Base::Vector3f N3 = (Q2-P1) % (Q1-P1);

  float fVol=0.0f;
  fVol += float(fabs((Q3-P1) * N1));
  fVol += float(fabs((Q3-P1) * N2));
  fVol += float(fabs((Q3-P1) * N3));

  fVol /= 6.0f;

  return fVol;;
}

float MeshGeomFacet::MaximumAngle () const
{
  float fMaxAngle = 0.0f;
  
  for ( int i=0; i<3; i++ ) {
    Base::Vector3f dir1(_aclPoints[(i+1)%3]-_aclPoints[i]);
    Base::Vector3f dir2(_aclPoints[(i+2)%3]-_aclPoints[i]);
    float fAngle = dir1.GetAngle(dir2);
    if (fAngle > fMaxAngle)
      fMaxAngle = fAngle;
  }

  return fMaxAngle;
}

bool MeshGeomFacet::IsPointOfSphere(const Base::Vector3f& rP) const
{
  float radius;
  Base::Vector3f center;
  radius = CenterOfCircumCircle(center);
  radius *= radius;

  float dist = Base::DistanceP2(rP, center);
  return dist < radius;
}

bool MeshGeomFacet::IsPointOfSphere(const MeshGeomFacet& rFacet) const
{
  float radius;
  Base::Vector3f center;
  radius = CenterOfCircumCircle(center);
  radius *= radius;

  for (int i=0; i<3; i++) {
    float dist = Base::DistanceP2(rFacet._aclPoints[i], center);
    if (dist < radius)
      return true;
  }

  return false;
}

