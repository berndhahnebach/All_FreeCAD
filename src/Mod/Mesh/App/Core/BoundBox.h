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


#ifndef __BOUNDBOX_H__
#define __BOUNDBOX_H__

#ifndef _PreComp_
# include <math.h>
#endif

#include "Definitions.h"
#include "ViewProj.h"
#include "Vector3D.h"
#include "Stream.h"
#include "Tools2D.h"

// Checks if point K lays on the ray [A,B[
#define IS_ON_RAY(A,B,K)  (((A) <= (K)) && ((B) > (K)))

namespace Mesh {

class BoundBox3D;
typedef BoundBox3D * PBoundBox3D;
typedef BoundBox3D & RBoundBox3D;

class ViewProjMethod;

/** The 3D bounding box class. */
class AppMeshExport BoundBox3D
{
public:
  /**  Public attributes */
  //@{
  float MinX;
  float MinY;
  float MinZ;
  float MaxX;
  float MaxY;
  float MaxZ;
  //@}
   
  inline BoundBox3D (float fMinX = FLOAT_MAX, float fMinY = FLOAT_MAX,
                     float fMinZ = FLOAT_MAX, float fMaxX = -FLOAT_MAX,
                     float fMaxY = -FLOAT_MAX, float fMaxZ = -FLOAT_MAX);
  BoundBox3D (const BoundBox3D &rcBB) { *this = rcBB; }
  inline BoundBox3D (const Vector3D *pclVect, unsigned long ulCt);

  BoundBox3D (RVector3D rcVector, float fDistance);
  ~BoundBox3D ();
 
  // Assignment
  inline BoundBox3D& operator = (const BoundBox3D &rcBound);
 
  /** Functions for intersection, cuttíng and union of bboxes */
  //@{
  inline bool operator && (const BoundBox3D &rcBB);
  inline bool operator && (const BoundBox2D &rcBB);
  BoundBox3D operator & (RBoundBox3D rcBB);
  inline BoundBox3D& operator &= (const Vector3D &rclVect);
  BoundBox3D operator | (RBoundBox3D rcBB);
  //@}
 
  /** Test methods */
  //@{
  inline bool IsInBox (const Vector3D &rcVct) const;
  inline bool IsInBox (const BoundBox3D &rcBB) const;
  inline bool IsInBox (const BoundBox2D &rcbb) const;
  bool IsValid (void);
  //@}

  enum OCTANT {OCT_LDB = 0, OCT_RDB, OCT_LUB, OCT_RUB,
               OCT_LDF,     OCT_RDF, OCT_LUF, OCT_RUF};
  bool GetOctantFromVector (Vector3D &rclVct, OCTANT &rclOctant);
  BoundBox3D CalcOctant (BoundBox3D::OCTANT Octant);

  enum SIDE { LEFT =0, RIGHT=1, TOP=2, BOTTOM=3, FRONT=4, BACK=5, INVALID=255 };
  inline Vector3D CalcPoint (unsigned short usPoint) const;
  bool IsCutLine (CGeoLine & rclLine, float fTolerance = 0.0f);
  inline bool IsCutPlane (const Vector3D &rclBase, const Vector3D &rclNormal) const;
  

  BoundBox2D ProjectBox(const ViewProjMethod *rclP) const;

  inline Vector3D CalcCenter (void) const;
  inline float CalcDiagonalLength (void) const;
  void Flush (void);
  
  inline void Enlarge (float fLen);   
  inline void Shrink  (float fLen);   

  inline float LengthX (void) const;
  inline float LengthY (void) const;
  inline float LengthZ (void) const;
  inline void MoveX (float f);
  inline void MoveY (float f);
  inline void MoveZ (float f);
  inline void ScaleX (float f);
  inline void ScaleY (float f);
  inline void ScaleZ (float f);
 
  void Print (void);
  unsigned long GetMemSpace (void);

  friend DataStream& operator<< (DataStream& ors,
                               RBoundBox3D rclBound);
  friend DataStream& operator>> (DataStream& irs,
                               RBoundBox3D rclBound);
  DataStream& SaveData (DataStream &ofs);
  DataStream& LoadData (DataStream &ifs);
};


inline BoundBox3D::BoundBox3D (float fMinX, float fMinY, float fMinZ,
                                 float fMaxX, float fMaxY, float fMaxZ)
: MinX(fMinX), MinY(fMinY), MinZ(fMinZ),
  MaxX(fMaxX), MaxY(fMaxY), MaxZ(fMaxZ)
{
}

inline BoundBox3D::BoundBox3D (const Vector3D *pclVect, unsigned long ulCt)
: MinX(FLOAT_MAX),  MinY(FLOAT_MAX),  MinZ(FLOAT_MAX),
  MaxX(-FLOAT_MAX), MaxY(-FLOAT_MAX), MaxZ(-FLOAT_MAX)
{
  const Vector3D  *pI, *pEnd = pclVect + ulCt;
  for (pI = pclVect; pI < pEnd; pI++)
  {
    MinX = std::min<float>(MinX, pI->x);
    MinY = std::min<float>(MinY, pI->y);
    MinZ = std::min<float>(MinZ, pI->z);
    MaxX = std::max<float>(MaxX, pI->x);
    MaxY = std::max<float>(MaxY, pI->y);
    MaxZ = std::max<float>(MaxZ, pI->z);
  }
}

inline BoundBox3D& BoundBox3D::operator = (const BoundBox3D &rcBound)
{
  MinX = rcBound.MinX;
  MinY = rcBound.MinY;
  MinZ = rcBound.MinZ;
  MaxX = rcBound.MaxX;
  MaxY = rcBound.MaxY;
  MaxZ = rcBound.MaxZ;
  return *this;
}

inline BoundBox3D& BoundBox3D::operator &= (const Vector3D &rclVect)
{
  MinX = std::min<float>(MinX, rclVect.x);
  MinY = std::min<float>(MinY, rclVect.y);
  MinZ = std::min<float>(MinZ, rclVect.z);
  MaxX = std::max<float>(MaxX, rclVect.x);
  MaxY = std::max<float>(MaxY, rclVect.y);
  MaxZ = std::max<float>(MaxZ, rclVect.z);
  return *this;
}

inline bool BoundBox3D::operator && (const BoundBox3D &rcBB)
{
  return  (IS_ON_RAY (MinX, MaxX, rcBB.MinX)        ||
           IS_ON_RAY (MinX, MaxX, rcBB.MaxX)        ||
           IS_ON_RAY (rcBB.MinX,  rcBB.MaxX, MinX)  ||
           IS_ON_RAY (rcBB.MinX,  rcBB.MaxX, MaxX)) &&
          (IS_ON_RAY (MinY, MaxY, rcBB.MinY)        ||
           IS_ON_RAY (MinY, MaxY, rcBB.MaxY)        ||
           IS_ON_RAY (rcBB.MinY,  rcBB.MaxY, MinY)  ||
           IS_ON_RAY (rcBB.MinY,  rcBB.MaxY, MaxY)) &&
          (IS_ON_RAY (MinZ, MaxZ, rcBB.MinZ)        ||
           IS_ON_RAY (MinZ, MaxZ, rcBB.MaxZ)        ||
           IS_ON_RAY (rcBB.MinZ,  rcBB.MaxZ, MinZ)  ||
           IS_ON_RAY (rcBB.MinZ,  rcBB.MaxZ, MaxZ));
}

inline bool BoundBox3D::operator && (const BoundBox2D &rcBB)
{
  return  (IS_ON_RAY (MinX, MaxX, rcBB.fMinX)         ||
           IS_ON_RAY (MinX, MaxX, rcBB.fMaxX)         ||
           IS_ON_RAY (rcBB.fMinX,  rcBB.fMaxX, MinX)  ||
           IS_ON_RAY (rcBB.fMinX,  rcBB.fMaxX, MaxX)) &&
          (IS_ON_RAY (MinY, MaxY, rcBB.fMinY)         ||
           IS_ON_RAY (MinY, MaxY, rcBB.fMaxY)         ||
           IS_ON_RAY (rcBB.fMinY,  rcBB.fMaxY, MinY)  ||
           IS_ON_RAY (rcBB.fMinY,  rcBB.fMaxY, MaxY) );
}

inline Vector3D BoundBox3D::CalcPoint (unsigned short usPoint) const
{
  switch (usPoint)
  {
    case 0: return Vector3D(MinX, MinY, MaxZ);
    case 1: return Vector3D(MaxX, MinY, MaxZ);
    case 2: return Vector3D(MaxX, MaxY, MaxZ);
    case 3: return Vector3D(MinX, MaxY, MaxZ);
    case 4: return Vector3D(MinX, MinY, MinZ);
    case 5: return Vector3D(MaxX, MinY, MinZ);
    case 6: return Vector3D(MaxX, MaxY, MinZ);
    case 7: return Vector3D(MinX, MaxY, MinZ);
  }
	
  return Vector3D();
}

inline Vector3D BoundBox3D::CalcCenter (void) const
{
  return Vector3D(MinX + (MaxX - MinX) / 2.0f,
                   MinY + (MaxY - MinY) / 2.0f,
                   MinZ + (MaxZ - MinZ) / 2.0f);
}

inline float BoundBox3D::CalcDiagonalLength (void) const
{
  return (float)sqrt (((MaxX - MinX) * (MaxX - MinX)) +
                      ((MaxY - MinY) * (MaxY - MinY)) +
                      ((MaxZ - MinZ) * (MaxZ - MinZ)));
}

inline bool BoundBox3D::IsCutPlane (const Vector3D &rclBase, const Vector3D &rclNormal) const
{
  if (fabs(CalcCenter().DistanceToPlane(rclBase, rclNormal)) < CalcDiagonalLength())
  {
    float fD = CalcPoint(0).DistanceToPlane(rclBase, rclNormal);
    for (unsigned short i = 1; i < 8; i++)
    {
      if ((CalcPoint(i).DistanceToPlane(rclBase, rclNormal) * fD) < 0.0f)
        return true;
    }
  }
  return false;
}

inline void BoundBox3D::Enlarge (float fLen)
{
  MinX -= fLen; MinY -= fLen; MinZ -= fLen;
  MaxX += fLen; MaxY += fLen; MaxZ += fLen;
}

inline void BoundBox3D::Shrink (float fLen)
{
  MinX += fLen; MinY += fLen; MinZ += fLen;
  MaxX -= fLen; MaxY -= fLen; MaxZ -= fLen;
}

inline bool BoundBox3D::IsInBox (const Vector3D &rcVct) const
{
  return (IS_ON_RAY (MinX, MaxX, rcVct.x) &&
          IS_ON_RAY (MinY, MaxY, rcVct.y) &&
          IS_ON_RAY (MinZ, MaxZ, rcVct.z));
}

inline bool BoundBox3D::IsInBox (const BoundBox3D &rcBB) const
{
  return (IS_ON_RAY (MinX, MaxX, rcBB.MinX) &&
          IS_ON_RAY (MinX, MaxX, rcBB.MaxX) &&
          IS_ON_RAY (MinY, MaxY, rcBB.MinY) &&
          IS_ON_RAY (MinY, MaxY, rcBB.MaxY) &&
          IS_ON_RAY (MinZ, MaxZ, rcBB.MinZ) &&
          IS_ON_RAY (MinZ, MaxZ, rcBB.MaxZ));
}

inline bool BoundBox3D::IsInBox (const BoundBox2D &rcBB) const
{
   return ( IS_ON_RAY (MinX, MaxX, rcBB.fMinX) &&
            IS_ON_RAY (MinX, MaxX, rcBB.fMaxX) &&
            IS_ON_RAY (MinY, MaxY, rcBB.fMinY) &&
            IS_ON_RAY (MinY, MaxY, rcBB.fMaxY) );
}

inline float BoundBox3D::LengthX (void) const
{
  return MaxX - MinX;
}

inline float BoundBox3D::LengthY (void) const
{
  return MaxY - MinY;
}

inline float BoundBox3D::LengthZ (void) const
{
  return MaxZ - MinZ;
}

inline void BoundBox3D::MoveX (float f)
{
  MinX += f; MaxX += f;
}

inline void BoundBox3D::MoveY (float f)
{
  MinY += f; MaxY += f;
}

inline void BoundBox3D::MoveZ (float f)
{
  MinZ += f; MaxZ += f;
}

inline void BoundBox3D::ScaleX (float f)
{
  MinX *= f; MaxX *= f;
}

inline void BoundBox3D::ScaleY (float f)
{
  MinY *= f; MaxY *= f;
}

inline void BoundBox3D::ScaleZ (float f)
{
  MinZ *= f; MaxZ *= f;
}

} // namespace Mesh

#endif  // __BOUNDBOX_H__ 

