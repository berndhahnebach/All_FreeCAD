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
# include <stdio.h>
# include <string>
# include <math.h>
# include <limits.h>
#endif

#include "Definitions.h"
#include "BoundBox.h"


using namespace Mesh;


BoundBox3D::BoundBox3D (RVector3D rcVector, float fDistance)
{
  MinX = rcVector.x - fDistance; 
  MaxX = rcVector.x + fDistance;
  MinY = rcVector.y - fDistance; 
  MaxY = rcVector.y + fDistance;
  MinZ = rcVector.z - fDistance; 
  MaxZ = rcVector.z + fDistance;
}

BoundBox3D::~BoundBox3D ()
{
}

BoundBox3D BoundBox3D::operator & (RBoundBox3D rcBB)
{
  BoundBox3D cBBRes;
	
  cBBRes.MinX = std::max<float> (MinX, rcBB.MinX);
  cBBRes.MaxX = std::min<float> (MaxX, rcBB.MaxX);
  cBBRes.MinY = std::max<float> (MinY, rcBB.MinY);
  cBBRes.MaxY = std::min<float> (MaxY, rcBB.MaxY);
  cBBRes.MinZ = std::max<float> (MinZ, rcBB.MinZ);
  cBBRes.MaxZ = std::min<float> (MaxZ, rcBB.MaxZ);
	
  return cBBRes;
}

BoundBox3D BoundBox3D::operator | (RBoundBox3D rcBB)
{
  BoundBox3D cBBRes;
	
  cBBRes.MinX = std::min<float> (MinX, rcBB.MinX);
  cBBRes.MaxX = std::max<float> (MaxX, rcBB.MaxX);
  cBBRes.MinY = std::min<float> (MinY, rcBB.MinY);
  cBBRes.MaxY = std::max<float> (MaxY, rcBB.MaxY);
  cBBRes.MinZ = std::min<float> (MinZ, rcBB.MinZ);
  cBBRes.MaxZ = std::max<float> (MaxZ, rcBB.MaxZ);
	
  return cBBRes;
}

bool BoundBox3D::IsValid (void)
{
  return ((MinX <= MaxX) && (MinY <= MaxY) && (MinZ <= MaxZ));
}

#define HALF(A,B)  ((A)+((B-A)/2))

bool BoundBox3D::GetOctantFromVector (Vector3D &rclVct, OCTANT &rclOctant)
{
  if (!IsInBox (rclVct))
    return false;
    
  unsigned short usNdx = 0;
  if (IS_ON_RAY (HALF (MinX, MaxX), MaxX, rclVct.x))  // left/RIGHT
    usNdx |= 1;
  if (IS_ON_RAY (HALF (MinY, MaxY), MaxY, rclVct.y))  // down/UP
    usNdx |= 2;
  if (IS_ON_RAY (HALF (MinZ, MaxZ), MaxZ, rclVct.z))  // back/FRONT
    usNdx |= 4;
  rclOctant = (OCTANT) usNdx;
  return true;
}

BoundBox3D BoundBox3D::CalcOctant (OCTANT Octant)
{
  BoundBox3D cOct (*this);
	
  switch (Octant)
  {
    case OCT_LDB:
      cOct.MaxX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MaxY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MaxZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
	
    case OCT_RDB:
      cOct.MinX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MaxY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MaxZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
	
    case OCT_LUB:
      cOct.MaxX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MinY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MaxZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
	
    case OCT_RUB:
      cOct.MinX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MinY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MaxZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
	
    case OCT_LDF:
      cOct.MaxX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MaxY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MinZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
	
    case OCT_RDF:
      cOct.MinX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MaxY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MinZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
	
    case OCT_LUF:
      cOct.MaxX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MinY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MinZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
	
    case OCT_RUF:
      cOct.MinX = HALF (cOct.MinX, cOct.MaxX);
      cOct.MinY = HALF (cOct.MinY, cOct.MaxY);
      cOct.MinZ = HALF (cOct.MinZ, cOct.MaxZ);
      break;
  }
  return cOct;
}

#undef HALF

void BoundBox3D::CalcPlane (unsigned short usPlane, Vector3D& rBase, Vector3D& rNormal ) const
{
  switch (usPlane)
  { 
  //links
  case 0:
    rBase.Set(MinX, MinY, MaxZ);
    rNormal.Set(1.0f, 0.0f, 0.0f);
    break;

  // rechts
  case 1:                                   
    rBase.Set(MaxX, MinY, MaxZ);
    rNormal.Set(1.0f, 0.0f, 0.0f);
    break;

  // oben
  case 2:
    rBase.Set(MinX, MaxY, MaxZ);
    rNormal.Set(0.0f, 1.0f, 0.0f);
    break;

  // unten
  case 3:
    rBase.Set(MinX, MinY, MaxZ);
    rNormal.Set(0.0f, 1.0f, 0.0f);
    break;

  // vorne
  case 4:
    rBase.Set(MinX, MinY, MaxZ);
    rNormal.Set(0.0f, 0.0f, 1.0f);
    break;

  // hinten
  default:
    rBase.Set(MinX, MinY, MinZ);
    rNormal.Set(0.0f, 0.0f, 1.0f);
    break;
  }
}

bool BoundBox3D::IntersectPlaneWithLine (unsigned short usSide, const Vector3D& rcBase, 
                                         const Vector3D& rcDir, Vector3D& rcP0) const
{
  float k;
  Vector3D cBase, cNormal;
  Vector3D  cDir(rcDir);
  CalcPlane(usSide, cBase, cNormal);

  if ((cNormal * cDir) == 0.0f) 
    return false;  // no point of intersection
  else
  {
    k = (cNormal * (cBase - rcBase)) / (cNormal * cDir);
    cDir.Scale(k, k, k);
    rcP0 = rcBase + cDir;

    return true;
  }
}

bool BoundBox3D::IntersectWithLine ( const Vector3D& rcBase, const Vector3D& rcDir, 
                                     Vector3D& rcP0, Vector3D& rcP1 ) const
{
  Vector3D  clVectRes, clVect[6];
  unsigned short i, j;
  
  j = 0;
  // schneide jede Seitenflaeche mit der Linie
  for (i = 0; i < 6; i++)
  {

    if ( IntersectPlaneWithLine(i, rcBase, rcDir, clVectRes ) )
    {
      // pruefe, ob Schnittpunkt innerhalb BB-Grenzen
      switch (i)
      {
        case 0 :  // linke und rechte Ebene
        case 1 :
          if ((IS_ON_RAY(MinY, MaxY, clVectRes.y) &&
               IS_ON_RAY(MinZ, MaxZ, clVectRes.z)) == TRUE) 
          {
            clVect[j] = clVectRes;
            j++;
          }                                 
          break;                    
        case 2 :  // obere und untere Ebene
        case 3 :
          if ((IS_ON_RAY(MinX, MaxX, clVectRes.x) &&
               IS_ON_RAY(MinZ, MaxZ, clVectRes.z))== TRUE) 
          {
            clVect[j] = clVectRes;
            j++;                     
          }
          break;                    
        case 4 :  // vordere und hintere Ebene
        case 5 :
          if ((IS_ON_RAY(MinX, MaxX, clVectRes.x) &&
               IS_ON_RAY(MinY, MaxY, clVectRes.y)) == TRUE) 
          {
            clVect[j] = clVectRes;
            j++;
          }        
          break;                    
      }
    }
  }
  
  if (j == 2)  
  {
    rcP0 = clVect[0];
    rcP1 = clVect[1];
    return true;
  }
  else if (j > 2)  // suche 2 unterschiedliche Schnittpunkte
  {
    for (i = 1; i < j; i++)
    {
      if (clVect[i] != clVect[0])
      {
        rcP0 = clVect[0];
        rcP1 = clVect[i];
        return true;
      }
    }
  }
  
  return false; 
}

BoundBox3D::SIDE BoundBox3D::GetSideFromRay (const Vector3D &rclPt, const Vector3D &rclDir) const
{
  Vector3D cIntersection;
  return GetSideFromRay( rclPt, rclDir, cIntersection);
}

BoundBox3D::SIDE BoundBox3D::GetSideFromRay (const Vector3D &rclPt, const Vector3D &rclDir, Vector3D rcInt) const
{
  Vector3D cP0, cP1;
  if ( IntersectWithLine(rclPt, rclDir, cP0, cP1) == false )
    return INVALID;

  Vector3D  cOut;
  // same orientation
  if ( (cP1-cP0)*rclDir > 0.0f )
    cOut = cP1;
  else
    cOut = cP0;

  rcInt = cOut;

  float fMax = 1.0e-3f;
  SIDE  tSide = INVALID;

  if (fabs(cOut.x - MinX) < fMax)      // linke Ebene
  {
    fMax = float(fabs(cOut.x - MinX));
    tSide = LEFT;
  }

  if (fabs(cOut.x - MaxX) < fMax) // rechte Ebene
  {
    fMax = float(fabs(cOut.x - MaxX));
    tSide = RIGHT;
  }

  if (fabs(cOut.y - MinY) < fMax) // untere Ebene
  {
    fMax = float(fabs(cOut.y - MinY));
    tSide = BOTTOM;
  }

  if (fabs(cOut.y - MaxY) < fMax) // obere Ebene
  {
    fMax = float(fabs(cOut.y - MaxY));
    tSide = TOP;
  }

  if (fabs(cOut.z - MinZ) < fMax) // vordere Ebene
  { 
    fMax = float(fabs(cOut.z - MinZ));
    tSide = FRONT;
  }

  if (fabs(cOut.z - MaxZ) < fMax) // hintere Ebene
  {
    fMax = float(fabs(cOut.z - MaxZ));
    tSide = BACK;
  }

  return tSide;
}

void BoundBox3D::Flush (void)
{
  MinX = MinY = MinZ =  FLOAT_MAX;
  MaxX = MaxY = MaxZ = -FLOAT_MAX;
}

void BoundBox3D::Print (void)
{
  printf ("X1 : %5.2f   Y1 : %5.2f   Z1 : %5.2f\n", MinX, MinY, MinZ);
  printf ("X2 : %5.2f   Y2 : %5.2f   Z2 : %5.2f\n", MaxX, MaxY, MaxZ);
}

DataStream& operator<< (DataStream& ors, RBoundBox3D rclBound)
{
  return rclBound.SaveData (ors);
}

DataStream& operator>> (DataStream& irs, RBoundBox3D rclBound)
{
  return rclBound.LoadData (irs);
}

DataStream & BoundBox3D::SaveData (DataStream &ofs)
{
  return ofs << MinX << MaxX << MinY << MaxY << MinZ << MaxZ;
}

DataStream & BoundBox3D::LoadData (DataStream &ifs)
{
  return ifs >> MinX >> MaxX >> MinY >> MaxY >> MinZ >> MaxZ;
}

BoundBox2D BoundBox3D::ProjectBox(const ViewProjMethod *pclP) const
{
  BoundBox2D  clBB2D;

  clBB2D.SetVoid();

  for (int i = 0; i < 8; i++)
  {
    Vector3D clTrsPt = (*pclP)(CalcPoint(i));
    clBB2D &= Vector2D(clTrsPt.x, clTrsPt.y);
  }

  return clBB2D;
}
