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


#ifndef __MESH_DEFINITIONS_HXX__
#define __MESH_DEFINITIONS_HXX__

#include "MeshConfig.h"

// default values
#define MESH_MIN_PT_DIST           1.0e-5f
#define MESH_MIN_EDGE_LEN          1.0e-3f
#define MESH_MIN_EDGE_ANGLE        float(RAD(2.0))
#define MESH_REMOVE_MIN_LEN        TRUE
#define MESH_REMOVE_G3_EDGES       TRUE

namespace Mesh {

class AppMeshExport MeshDefinitions
{
public:
  MeshDefinitions (void);
  virtual ~MeshDefinitions (void)
  {}

  static float  _fMinPointDistance;
  static float  _fMinPointDistanceP2;
  static float  _fMinPointDistanceD1;

  static float  _fMinEdgeLength;
  static bool  _bRemoveMinLength;

  static float _fMinEdgeAngle;

  static void  SetMinPointDistance (float fMin);
};

} // namespace Mesh

#endif
