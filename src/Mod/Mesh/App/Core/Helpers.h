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


#ifndef __MESH_HELPERS_HXX__
#define __MESH_HELPERS_HXX__

#include "Elements.h"

namespace Mesh {

class Vector3D;
class AppMeshExport MeshHelpPoint
{
    public:
    inline void Set (unsigned long ulCorner, unsigned long ulFacet, const Vector3D &rclPt);

    public:
    inline bool operator < (const MeshHelpPoint &rclObj) const;
    public:
    inline bool operator == (const MeshHelpPoint &rclObj) const;

    public:
    unsigned long Index (void) const
    { return _ulInd >> 2; }
    public:
    unsigned long Corner (void) const
    { return _ulInd & 3; }

    public:
    MeshPoint _clPt;
    public:
    unsigned long     _ulInd;
};

class MeshPointBuilder: public std::vector<MeshHelpPoint>
{
     public:
     inline void Add (unsigned long ulCorner, unsigned long ulFacet, const Vector3D &rclPt);
};

class AppMeshExport MeshHelpBuilderEdge
{
    public:
    unsigned long Side (void) const
    { return _ulFIndex & 3; }
    public:
    unsigned long Index (void) const
    { return _ulFIndex >> 2; }

    public:
    inline void Set (unsigned long ulInd1, unsigned long ulInd2,
                                           unsigned long ulSide, unsigned long ulFInd);

    public:
    inline bool operator < (const MeshHelpBuilderEdge &rclObj) const;
    public:
    inline bool operator == (const MeshHelpBuilderEdge &rclObj) const;
    
    public:
    inline bool operator != (const MeshHelpBuilderEdge &rclObj) const;

    public:
    unsigned long  _ulFIndex;   // Facet-Index
    public:
    unsigned long  _aulInd[2];   // Punkt-Index
};

class MeshEdgeBuilder: public std::vector<MeshHelpBuilderEdge>
{
    public:
    typedef std::vector<MeshHelpBuilderEdge>::iterator  _TIterator;

    public:
    inline void Add (unsigned long ulInd1, unsigned long ulInd2, unsigned long ulSide, unsigned long ulFInd);
};

class Vector3D;
#include "Elements.h"
inline void MeshHelpPoint::Set (unsigned long ulCorner, unsigned long ulFacet, const Vector3D &rclPt)
{
  _ulInd = (ulFacet << 2) | ulCorner;
  _clPt  = rclPt;
};

inline bool MeshHelpPoint::operator < (const MeshHelpPoint &rclObj) const
{
//   if (fabs(_clPt.x - rclObj._clPt.x) < MeshDefinitions::_fMinPointDistanceD1)
//   {
//     if (fabs(_clPt.y - rclObj._clPt.y) < MeshDefinitions::_fMinPointDistanceD1)
//     {
//       if (fabs(_clPt.z - rclObj._clPt.z) < MeshDefinitions::_fMinPointDistanceD1)
//         return FALSE;
//       else
//         return _clPt.z < rclObj._clPt.z;
//     }
//     else
//       return _clPt.y < rclObj._clPt.y;
//   }
//   else
//     return _clPt.x < rclObj._clPt.x;

  if (_clPt.x == rclObj._clPt.x)
  {
    if (_clPt.y == rclObj._clPt.y)
      return _clPt.z < rclObj._clPt.z;
    else
      return _clPt.y < rclObj._clPt.y;
  }
  else
    return _clPt.x < rclObj._clPt.x;
}

inline bool MeshHelpPoint::operator == (const MeshHelpPoint &rclObj) const
{
  return DistanceP2(_clPt, rclObj._clPt) < MeshDefinitions::_fMinPointDistanceP2;
/*
  if (fabs(_clPt.x - rclObj._clPt.x) < (MeshDefinitions::_fMinPointDistanceD1 + 1.0e-2f))
  {
    if (fabs(_clPt.y - rclObj._clPt.y) < (MeshDefinitions::_fMinPointDistanceD1 + 1.0e-2f))
    {
      if (fabs(_clPt.z - rclObj._clPt.z) < (MeshDefinitions::_fMinPointDistanceD1 + 1.0e-2f))
        return TRUE;
      else
        return FALSE;
    }
    else
      return FALSE;
  }
  else
    return FALSE;
*/
}

inline void MeshPointBuilder::Add (unsigned long ulCorner, unsigned long ulFacet, const Vector3D &rclPt)
{
  MeshHelpPoint  clObj;
  clObj.Set(ulCorner, ulFacet, rclPt);
  push_back(clObj);
}

inline void MeshHelpBuilderEdge::Set (unsigned long ulInd1, unsigned long ulInd2,
                                       unsigned long ulSide, unsigned long ulFInd)
{
  if (ulInd1 < ulInd2)
  {
    _aulInd[0] = ulInd1;
    _aulInd[1] = ulInd2; 
  }
  else
  {
    _aulInd[0] = ulInd2;
    _aulInd[1] = ulInd1; 
  }
  _ulFIndex = (ulFInd << 2) | ulSide;
}

inline bool MeshHelpBuilderEdge::operator < (const MeshHelpBuilderEdge &rclObj) const
{
  if (_aulInd[0] == rclObj._aulInd[0])
    return _aulInd[1] < rclObj._aulInd[1];
  else
    return _aulInd[0] < rclObj._aulInd[0];
}

inline bool MeshHelpBuilderEdge::operator == (const MeshHelpBuilderEdge &rclObj) const
{
  return (_aulInd[0] == rclObj._aulInd[0]) && (_aulInd[1] == rclObj._aulInd[1]);
}

inline bool MeshHelpBuilderEdge::operator != (const MeshHelpBuilderEdge &rclObj) const
{
  return (_aulInd[0] != rclObj._aulInd[0]) || (_aulInd[1] != rclObj._aulInd[1]);
}


inline void MeshEdgeBuilder::Add (unsigned long ulInd1, unsigned long ulInd2, unsigned long ulSide, unsigned long ulFInd)
{
  MeshHelpBuilderEdge  clObj;
  clObj.Set(ulInd1, ulInd2, ulSide, ulFInd);
  push_back(clObj);
}

} // namespace Mesh

#endif
