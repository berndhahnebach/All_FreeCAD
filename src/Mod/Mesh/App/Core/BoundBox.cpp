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
