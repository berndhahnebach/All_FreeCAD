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


#ifndef __MESH_MESHIO_HXX__
#define __MESH_MESHIO_HXX__

#include "MeshKernel.h"

namespace Mesh {

class MeshKernel;
class FileStream;

class AppMeshExport MeshSTL
{
  public:
    public:
    MeshSTL (MeshKernel &rclM);
    public:
    virtual ~MeshSTL (void)
    {}

    public:
    bool Load (FileStream &rstrIn);

    public:
    bool LoadAscii (FileStream &rstrIn);

    public:
    bool LoadBinary (FileStream &rstrIn);

    public:
    bool SaveAscii (FileStream &rstrOut) const;

    public:
    bool SaveBinary (FileStream &rstrOut) const;

  protected:
    // help methods

    protected:
    MeshKernel &_rclMesh;   // reference to mesh data structure

};

class AppMeshExport MeshInventor
{
    public:
    MeshInventor (MeshKernel &rclM) : _rclMesh(rclM) 
    {}
    public:
    virtual ~MeshInventor (void)
    {}

    public:
    bool Load (FileStream &rstrIn);

    public:
    bool Save (FileStream &rstrOut) const;

    protected:
    MeshKernel &_rclMesh;
};

} // namespace Mesh

#endif
