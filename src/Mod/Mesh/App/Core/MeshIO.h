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


#ifndef MESH_IO_H
#define MESH_IO_H

#include "MeshKernel.h"

namespace Mesh {

class MeshKernel;
class FileStream;

/**
 * The MeshSTL class is able to read or write mesh object in STL format.
 */
class AppMeshExport MeshSTL
{
public:
  MeshSTL (MeshKernel &rclM);
  virtual ~MeshSTL (void) { }

  /** Loads an STL file either in binary or ASCII. 
   * Therefore the file header gets checked to decide if
   * the file is binary or not.
   */
  bool Load (FileStream &rstrIn);
  /** Loads an ASCII STL file. */
  bool LoadAscii (FileStream &rstrIn);
  /** Loads a binary STL file. */
  bool LoadBinary (FileStream &rstrIn);

  /** Saves the mesh object into an ASCII file. */
  bool SaveAscii (FileStream &rstrOut) const;
  /** Saves the mesh object into a binary file. */
  bool SaveBinary (FileStream &rstrOut) const;

protected:
  MeshKernel &_rclMesh;   /**< reference to mesh data structure */
};

/**
 * The MeshInventor class is able to read or write mesh object in 
 * OpenInventor format.
 */
class AppMeshExport MeshInventor
{
public:
  MeshInventor (MeshKernel &rclM) : _rclMesh(rclM) { }
  virtual ~MeshInventor (void) { }

  /** Loads an OpenInventor file. */
  bool Load (FileStream &rstrIn);
  /** Writes an OpenInventor file. */
  bool Save (FileStream &rstrOut) const;

protected:
  MeshKernel &_rclMesh; /**< reference to mesh data structure */
};

} // namespace Mesh

#endif // MESH_IO_H 
