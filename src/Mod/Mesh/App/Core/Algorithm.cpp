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
# include <Wm3IntrSegment3Box3.h>
# include <Wm3IntrTriangle3Triangle3.h>
# include <Wm3DistVector3Triangle3.h>
# include <Wm3DistSegment3Segment3.h>
# include <Wm3DistSegment3Triangle3.h>
#endif

#include "Algorithm.h"
#include "MeshKernel.h"
#include "Elements.h"


using namespace Mesh;
using namespace Wm3;

bool MeshFacetFunc::IntersectBoundingBox (const MeshGeomFacet &rclFacet, const BoundBox3D &rclBB)
{
  // the triangle's corner points
  const Vector3D& v0 = rclFacet._aclPoints[0];
  const Vector3D& v1 = rclFacet._aclPoints[1];
  const Vector3D& v2 = rclFacet._aclPoints[2];

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
  Vector3D clCenter  = rclBB.CalcCenter();
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

bool MeshFacetFunc::IntersectWithPlane (const MeshGeomFacet &rclFacet, const Vector3D &rclBase, const Vector3D &rclNormal, Vector3D &rclP1, Vector3D &rclP2)
{
  // the triangle's corner points
  const Vector3D& v0 = rclFacet._aclPoints[0];
  const Vector3D& v1 = rclFacet._aclPoints[1];
  const Vector3D& v2 = rclFacet._aclPoints[2];

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

bool MeshFacetFunc::Foraminate (const MeshGeomFacet &rclFacet, const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes, float fMaxAngle)
{
  const float fTolerance = 1e-06f;

  Vector3D clE0 = rclFacet._aclPoints[1] - rclFacet._aclPoints[0];
  Vector3D clE1 = rclFacet._aclPoints[2] - rclFacet._aclPoints[0];

  Vector3D clFacetNormal = rclFacet.GetNormal();

  float fDenominator = clFacetNormal * rclDir;
  float fLLenSqr     = rclDir * rclDir;
  float fNLenSqr     = rclFacet.GetNormal() * rclFacet.GetNormal();

  if (rclDir.GetAngle(clFacetNormal) > fMaxAngle)
    return false;

  if ((fDenominator * fDenominator) <= (fTolerance * fLLenSqr * fNLenSqr))
    return false;

  Vector3D clDiff0 = rclPt - rclFacet._aclPoints[0];
  float     fTime   = -(rclFacet.GetNormal() * clDiff0) / fDenominator;

  Vector3D  clDiff1 = clDiff0 + fTime * rclDir;

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
    rclRes = clDiff1 + rclFacet._aclPoints[0];
    return true;
  }
  else
    return false;
}

bool MeshFacetFunc::IntersectPlaneWithLine (const MeshGeomFacet &rclFacet, const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes)
{
  // berechne den Schnittpunkt Gerade <-> Ebene
  if ( fabs(rclDir * rclFacet.GetNormal()) < 1e-3f )
    return false; // line and plane are parallel

  float s = ( ( rclFacet.GetGravityPoint() - rclPt ) * rclFacet.GetNormal() ) 
            / ( rclDir * rclFacet.GetNormal() );
  rclRes = rclPt + s * rclDir;

  return true;
}

bool MeshFacetFunc::IntersectWithLine (const MeshGeomFacet &rclFacet, const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes)
{
  if ( !IntersectPlaneWithLine( rclFacet, rclPt, rclDir, rclRes ) )
    return false; // line and plane are parallel
  // Check if the intersection point is inside the facet
  return IsPointOf(rclFacet, rclRes, 1e-03f);
}

float MeshFacetFunc::DistanceToLineSegment (const MeshGeomFacet &rclFacet, const Vector3D &rclP1, const Vector3D &rclP2)
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
  Vector3<float>  akF0(rclFacet._aclPoints[0].x, rclFacet._aclPoints[0].y, rclFacet._aclPoints[0].z);
  Vector3<float>  akF1(rclFacet._aclPoints[1].x, rclFacet._aclPoints[1].y, rclFacet._aclPoints[1].z);
  Vector3<float>  akF2(rclFacet._aclPoints[2].x, rclFacet._aclPoints[2].y, rclFacet._aclPoints[2].z);

  Triangle3<float> akTria(akF0, akF1, akF2);

  DistSegment3Triangle3<float> akDistSegTria(akSeg, akTria);
  return akDistSegTria.Get();
}

float MeshFacetFunc::DistanceToPoint (const MeshGeomFacet &rclFacet, const Vector3D &rclPt, Vector3D &rclNt)
{
  Vector3<float>  akPt(rclPt.x, rclPt.y, rclPt.z);
  Vector3<float>  akF0(rclFacet._aclPoints[0].x, rclFacet._aclPoints[0].y, rclFacet._aclPoints[0].z);
  Vector3<float>  akF1(rclFacet._aclPoints[1].x, rclFacet._aclPoints[1].y, rclFacet._aclPoints[1].z);
  Vector3<float>  akF2(rclFacet._aclPoints[2].x, rclFacet._aclPoints[2].y, rclFacet._aclPoints[2].z);

  Triangle3<float> akTria(akF0, akF1, akF2);
  DistVector3Triangle3<float> akDistPtTria(akPt, akTria);
  
  float fDist = akDistPtTria.Get();

  // get nearest point of the facet
  Vector3<float> akNt = akDistPtTria.GetClosestPoint1();
  rclNt.Set(akNt.X(), akNt.Y(), akNt.Z());

  return fDist;
}

void MeshFacetFunc::SubSample (const MeshGeomFacet &rclFacet, float fStep, std::vector<Vector3D> &rclPoints)
{
  std::vector<Vector3D> clPoints;
  Vector3D A = rclFacet._aclPoints[0], B = rclFacet._aclPoints[1], C = rclFacet._aclPoints[2];
  Vector3D clVecAB(B - A);
  Vector3D clVecAC(C - A);
  Vector3D clVecBC(C - B);

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
  Vector3D clVecABNorm(clVecAB);
  Vector3D clVecHNorm((clVecAB % clVecAC) % clVecAB);
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
       // rclPoints.push_back(CVector3D(u*A + v*B + w*C));
        Vector3D clV = A + (px * clVecABNorm) + (py * clVecHNorm);
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

bool MeshFacetFunc::IsPointOf (const MeshGeomFacet &rclFacet, const Vector3D& rclP, float fDistance)
{
  // effektivere Implementierung als in MeshGeomFacet::IsPointOf
  //
  Vector3D a(rclFacet._aclPoints[0].x, rclFacet._aclPoints[0].y, rclFacet._aclPoints[0].z);
  Vector3D b(rclFacet._aclPoints[1].x, rclFacet._aclPoints[1].y, rclFacet._aclPoints[1].z);
  Vector3D c(rclFacet._aclPoints[2].x, rclFacet._aclPoints[2].y, rclFacet._aclPoints[2].z);
  Vector3D p(rclP);

  Vector3D n  = (b - a) % (c - a);
  Vector3D n1 = (a - p) % (b - p);
  Vector3D n2 = (c - p) % (a - p);
  Vector3D n3 = (b - p) % (c - p);

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

bool MeshFacetFunc::IntersectWithProjectedFacet(const MeshGeomFacet &rclFacet1, const MeshGeomFacet &rclFacet2)
{
  // project facet 2 onto facet 1
  MeshGeomFacet cProjFac;
  Vector3D res;
  IntersectPlaneWithLine( rclFacet1, rclFacet2._aclPoints[0], rclFacet1.GetNormal(), res);
  cProjFac._aclPoints[0] = res;

  IntersectPlaneWithLine( rclFacet1, rclFacet2._aclPoints[1], rclFacet1.GetNormal(), res);
  cProjFac._aclPoints[1] = res;
  
  IntersectPlaneWithLine( rclFacet1, rclFacet2._aclPoints[2], rclFacet1.GetNormal(), res);
  cProjFac._aclPoints[2] = res;

  Vector3<float> akU[3] = 
      {Vector3<float>(rclFacet1._aclPoints[0].x, rclFacet1._aclPoints[0].y, rclFacet1._aclPoints[0].z),
       Vector3<float>(rclFacet1._aclPoints[1].x, rclFacet1._aclPoints[1].y, rclFacet1._aclPoints[1].z),
       Vector3<float>(rclFacet1._aclPoints[2].x, rclFacet1._aclPoints[2].y, rclFacet1._aclPoints[2].z)};

  Vector3<float> akV[3] = 
      {Vector3<float>(cProjFac._aclPoints[0].x, cProjFac._aclPoints[0].y, cProjFac._aclPoints[0].z),
       Vector3<float>(cProjFac._aclPoints[1].x, cProjFac._aclPoints[1].y, cProjFac._aclPoints[1].z),
       Vector3<float>(cProjFac._aclPoints[2].x, cProjFac._aclPoints[2].y, cProjFac._aclPoints[2].z)};

  Triangle3<float> akTria1(akU);
  Triangle3<float> akTria2(akV);

  //Tests two triangles for intersection (Magic Softwate Library)
  return IntrTriangle3Triangle3<float>(akTria1, akTria2).Test();
}

bool MeshFacetFunc::IntersectWithFacet(const MeshGeomFacet &rclFacet1, const MeshGeomFacet &rclFacet2)
{
  // project facet 2 onto facet 1
  Vector3<float> akU[3] = 
      {Vector3<float>(rclFacet1._aclPoints[0].x, rclFacet1._aclPoints[0].y, rclFacet1._aclPoints[0].z),
       Vector3<float>(rclFacet1._aclPoints[1].x, rclFacet1._aclPoints[1].y, rclFacet1._aclPoints[1].z),
       Vector3<float>(rclFacet1._aclPoints[2].x, rclFacet1._aclPoints[2].y, rclFacet1._aclPoints[2].z)};

  Vector3<float> akV[3] = 
      {Vector3<float>(rclFacet2._aclPoints[0].x, rclFacet2._aclPoints[0].y, rclFacet2._aclPoints[0].z),
       Vector3<float>(rclFacet2._aclPoints[1].x, rclFacet2._aclPoints[1].y, rclFacet2._aclPoints[1].z),
       Vector3<float>(rclFacet2._aclPoints[2].x, rclFacet2._aclPoints[2].y, rclFacet2._aclPoints[2].z)};

  Triangle3<float> akTria1(akU);
  Triangle3<float> akTria2(akV);

  //Tests two triangles for intersection (Magic Softwate Library)
  return IntrTriangle3Triangle3<float>(akTria1, akTria2).Test();
}

float MeshFacetFunc::CenterOfInnerCircle(const MeshGeomFacet &rclFacet, Vector3D& rclCenter)
{
  Vector3D p0(rclFacet._aclPoints[0].x, rclFacet._aclPoints[0].y, rclFacet._aclPoints[0].z);
  Vector3D p1(rclFacet._aclPoints[1].x, rclFacet._aclPoints[1].y, rclFacet._aclPoints[1].z);
  Vector3D p2(rclFacet._aclPoints[2].x, rclFacet._aclPoints[2].y, rclFacet._aclPoints[2].z);

  float a = (p1-p2).Length();
  float b = (p2-p0).Length();
  float c = (p0-p1).Length();

  // radius of the circle
  float fRadius = MeshFacetFunc::Area(rclFacet);
  fRadius *= 2.0f/(a + b + c); 

  // center of the circle
  rclCenter = (a*p0 + b*p1 + c*p2)/(a + b + c);

  return fRadius;
}

float MeshFacetFunc::CenterOfOuterCircle(const MeshGeomFacet &rclFacet, Vector3D& rclCenter)
{
  Vector3D p0(rclFacet._aclPoints[0].x, rclFacet._aclPoints[0].y, rclFacet._aclPoints[0].z);
  Vector3D p1(rclFacet._aclPoints[1].x, rclFacet._aclPoints[1].y, rclFacet._aclPoints[1].z);
  Vector3D p2(rclFacet._aclPoints[2].x, rclFacet._aclPoints[2].y, rclFacet._aclPoints[2].z);

  Vector3D u = (p1-p0);
  Vector3D v = (p2-p1);
  Vector3D w = (p0-p2);

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
  float fRadius = (float)(sqrt(uu * vv * ww) / (4 * MeshFacetFunc::Area(rclFacet)));

  return fRadius;
}

unsigned short MeshFacetFunc::NearestEdgeToPoint(const MeshGeomFacet& rclFacet, const Vector3D& rclPt)
{
  unsigned short usSide;

  const Vector3D& rcP1 = rclFacet._aclPoints[0];
  const Vector3D& rcP2 = rclFacet._aclPoints[1];
  const Vector3D& rcP3 = rclFacet._aclPoints[2];

  float fD1 = FLOAT_MAX;
  float fD2 = FLOAT_MAX;
  float fD3 = FLOAT_MAX;

  // 1st edge
  Vector3D clDir = rcP2 - rcP1;
  float fLen = Mesh::Distance(rcP2, rcP1);
  float t = ( ( rclPt - rcP1 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD1 = Mesh::Distance(rclPt, rcP1);
  else if ( t > 1.0f )
    fD1 = Mesh::Distance(rclPt, rcP2);
  else
    fD1 = ( ( ( rclPt - rcP1 ) % clDir).Length() ) / fLen;

  // 2nd edge
  clDir = rcP3 - rcP2;
  fLen = Mesh::Distance(rcP3, rcP2);
  t = ( ( rclPt - rcP2 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD2 = Mesh::Distance(rclPt, rcP2);
  else if ( t > 1.0f )
    fD2 = Mesh::Distance(rclPt, rcP3);
  else
    fD2 = ( ( ( rclPt - rcP2 ) % clDir).Length() ) / fLen;

  // 3rd edge
  clDir = rcP1 - rcP3;
  fLen = Mesh::Distance(rcP1, rcP3);
  t = ( ( rclPt - rcP3 ) * clDir ) / ( fLen * fLen );
  if ( t < 0.0f )
    fD3 = Mesh::Distance(rclPt, rcP3);
  else if ( t > 1.0f )
    fD3 = Mesh::Distance(rclPt, rcP1);
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

//----------------------------------------------------------------------------

bool MeshEdgeFunc::ContainedByOrIntersectBoundingBox (const MeshGeomEdge &rclEdge, 
                                                          const BoundBox3D &rclBB )
{
  // Test, ob alle Eckpunkte der Edge sich auf einer der 6 Seiten der BB befinden
  if ((MeshEdgeFunc::GetBoundBox(rclEdge) && rclBB) == false)
    return false;

  // Test, ob Edge-BB komplett in BB liegt
  if (rclBB.IsInBox(MeshEdgeFunc::GetBoundBox(rclEdge)))
    return true;

  // Test, ob einer der Eckpunkte in BB liegt
  for (int i=0;i<2;i++)
  {
    if (rclBB.IsInBox(rclEdge._aclPoints[i]))
      return true;
  }

  // "echter" Test auf Schnitt
  if (MeshEdgeFunc::IntersectBoundingBox(rclEdge, rclBB))
    return true;

  return false;
}

BoundBox3D MeshEdgeFunc::GetBoundBox (const MeshGeomEdge &rclEdge)
{
  return BoundBox3D(rclEdge._aclPoints,2);
}

bool MeshEdgeFunc::IntersectBoundingBox (const MeshGeomEdge &rclEdge, const BoundBox3D &rclBB)
{
  const Vector3D& rclP0 = rclEdge._aclPoints[0];
  const Vector3D& rclP1 = rclEdge._aclPoints[1];

  Vector3<float> A(rclP0.x, rclP0.y, rclP0.z);
  Vector3<float> B(rclP1.x, rclP1.y, rclP1.z);

  Vector3<float> n = B - A;
  float len = n.Length();
  n.Normalize();
  Vector3<float> p = 0.5f*(A + B);

  Segment3<float> akSeg(p, n, 0.5f*len);

  Vector3D clCenter  = rclBB.CalcCenter();
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

//----------------------------------------------------------------------------

void MeshRefPointToFacets::Rebuild (void)
{
  clear();
  resize(_rclMesh._aclPointArray.size());

  for (MeshFacetArray::_TIterator pFIter = _rclMesh._aclFacetArray.begin(); pFIter != _rclMesh._aclFacetArray.end(); pFIter++)
  {
    operator[](pFIter->_aulPoints[0]).insert(pFIter);
    operator[](pFIter->_aulPoints[1]).insert(pFIter);
    operator[](pFIter->_aulPoints[2]).insert(pFIter);
  }
}

// ermittelt alle Nachbarn zum Facet deren Schwerpunkt unterhalb der mpx. Distanz befindet. 
// Facet deren VISIT-Flag gesetzt ist werden nicht beruecksichtig. 
/// @todo
void MeshRefPointToFacets::Neighbours (unsigned long ulFacetInd, float fMpxDist, std::vector<MeshFacetArray::_TIterator> &rclNb)
{
  rclNb.clear();
  Vector3D  clCenter = _rclMesh.GetFacet(ulFacetInd).GetGravityPoint();
  SearchNeighbours(_rclMesh._aclFacetArray.begin() + ulFacetInd, clCenter, fMpxDist * fMpxDist, rclNb);

  for (std::vector<MeshFacetArray::_TIterator>::iterator i = rclNb.begin(); i != rclNb.end(); i++)
    (*i)->ResetFlag(MeshFacet::VISIT);  
}

/// @todo
void MeshRefPointToFacets::SearchNeighbours(MeshFacetArray::_TIterator pFIter, const Vector3D &rclCenter, float fMpxDist, std::vector<MeshFacetArray::_TIterator> &rclNb)
{
  if (pFIter->IsFlag(MeshFacet::VISIT) == true)
    return;

  if (DistanceP2(rclCenter, MeshFacetFunc::ToGeomFacet(_rclMesh, *pFIter).GetGravityPoint()) > fMpxDist)
    return;

  rclNb.push_back(pFIter);
  pFIter->SetFlag(MeshFacet::VISIT);
  MeshPointArray::_TIterator        pPBegin = _rclMesh._aclPointArray.begin();
   
  for (int i = 0; i < 3; i++)
  {
    MeshPointArray::_TIterator pPIter = _rclMesh._aclPointArray.begin() + pFIter->_aulPoints[i];
    const std::set<MeshFacetArray::_TIterator> &rclFacets = operator[](pPIter - pPBegin);

    for (std::set<MeshFacetArray::_TIterator>::iterator j = rclFacets.begin(); j != rclFacets.end(); j++)
    {
      SearchNeighbours(*j, rclCenter, fMpxDist, rclNb);
    }
  }
}

//----------------------------------------------------------------------------

void MeshRefFacetToFacets::Rebuild (void)
{
  MeshFacetArray::_TIterator pFBegin = _rclMesh._aclFacetArray.begin();
  std::set<MeshFacetArray::_TIterator> clEmptySet;

  clear();

  MeshRefPointToFacets  clRPF(_rclMesh);
  
  for (MeshFacetArray::_TConstIterator pFIter = pFBegin; pFIter != _rclMesh._aclFacetArray.end(); pFIter++)
  {
    for (int i = 0; i < 3; i++)
    {
      const std::set<MeshFacetArray::_TIterator> &rclNBSet = clRPF[pFIter->_aulPoints[i]];
      operator[](pFIter - pFBegin).insert(rclNBSet.begin(), rclNBSet.end());
    }
  }
}

//----------------------------------------------------------------------------

void MeshRefPointToPoints::Rebuild (void)
{
  clear();
  resize(_rclMesh._aclPointArray.size());

  MeshPointArray::_TIterator  pBegin = _rclMesh._aclPointArray.begin();
  for (MeshFacetArray::_TIterator pFIter = _rclMesh._aclFacetArray.begin(); pFIter != _rclMesh._aclFacetArray.end(); pFIter++)
  {
    unsigned long ulP0 = pFIter->_aulPoints[0];
    unsigned long ulP1 = pFIter->_aulPoints[1];
    unsigned long ulP2 = pFIter->_aulPoints[2];

    operator[](ulP0).insert(pBegin + ulP1);
    operator[](ulP0).insert(pBegin + ulP2);
    operator[](ulP1).insert(pBegin + ulP0);
    operator[](ulP1).insert(pBegin + ulP2);
    operator[](ulP2).insert(pBegin + ulP0);
    operator[](ulP2).insert(pBegin + ulP1);
  }
}
