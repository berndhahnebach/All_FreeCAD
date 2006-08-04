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
#include <Base/Vector3D.h>
#include <App/Material.h>

namespace Base {
  class XMLReader;
  class Writer;
}

namespace MeshCore {

class MeshKernel;

/**
 * The MeshSTL class is able to read or write mesh object in STL format.
 */
class AppMeshExport LoadMeshSTL
{
public:
  LoadMeshSTL (MeshKernel &rclM): _rclMesh(rclM){};
  virtual ~LoadMeshSTL (void) { }

  /** Loads an STL file either in binary or ASCII. 
   * Therefore the file header gets checked to decide if
   * the file is binary or not.
   */
  //bool Load (FileStream &rstrIn);
  bool Load (std::istream &rstrIn);
  /** Loads an ASCII STL file. */
  //bool LoadAscii (FileStream &rstrIn);
  bool LoadAscii (std::istream &rstrIn);
  /** Loads a binary STL file. */
  //bool LoadBinary (FileStream &rstrIn);
  bool LoadBinary (std::istream &rstrIn);

protected:
  MeshKernel &_rclMesh;   /**< reference to mesh data structure */
};

class AppMeshExport SaveMeshSTL
{
public:
  SaveMeshSTL (const MeshKernel &rclM): _rclMesh(rclM){};
  virtual ~SaveMeshSTL (void) { }

  /** Saves the mesh object into an ASCII file. */
  //bool SaveAscii (FileStream &rstrOut) const;
  bool SaveAscii (std::ostream &rstrOut) const;
  /** Saves the mesh object into a binary file. */
  //bool SaveBinary (FileStream &rstrOut) const;
  bool SaveBinary (std::ostream &rstrOut) const;

protected:
  const MeshKernel &_rclMesh;   /**< reference to mesh data structure */
};





/**
 * The MeshInventor class is able to read or write mesh object in 
 * OpenInventor format.
 */
class AppMeshExport SaveMeshInventor
{
public:
  SaveMeshInventor (const MeshKernel &rclM) : _rclMesh(rclM) { }
  virtual ~SaveMeshInventor (void) { }

  /** Writes an OpenInventor file. */
  bool Save (std::ostream &rstrOut) const;

protected:
  const MeshKernel &_rclMesh; /**< reference to mesh data structure */
};

class AppMeshExport LoadMeshInventor
{
public:
  LoadMeshInventor (MeshKernel &rclM) : _rclMesh(rclM) { }
  virtual ~LoadMeshInventor (void) { }

  /** Loads an OpenInventor file. */
  bool Load (std::istream &rstrIn);

protected:
  MeshKernel &_rclMesh; /**< reference to mesh data structure */
};

/**
 * The MeshInventor class is able to read or write mesh object in 
 * OpenInventor format.
 */
class AppMeshExport MeshDocXML
{
public:
  MeshDocXML (MeshKernel &rclM) : _rclMesh(rclM) { }
  virtual ~MeshDocXML (void) { }

  void Save (Base::Writer &writer) const;
  void Restore(Base::XMLReader &reader);

protected:
  MeshKernel &_rclMesh; /**< reference to mesh data structure */
};

struct AppMeshExport VRMLViewpointData
{
  Base::Vector3f clVRefPln;
  Base::Vector3f clVRefUp;
  Base::Vector3f clVRefPt;
  Base::Vector3f clPRefPt;
  double    dVPlnDist;
  double    dUmin;
  double    dUmax;
  double    dVmin;
  double    dVmax;
  std::string  clName;
};



struct AppMeshExport VRMLInfo
{
  std::string _clFileName;
  std::string _clAuthor;
  std::string _clDate;
  std::string _clCompany;
  std::string _clAnnotation;
  std::string _clPicFileName;
  App::Color  _clColor;
  bool     _bSaveViewPoints;
  bool     _bSavePicture;
  std::vector<std::string> _clComments;
  std::vector<VRMLViewpointData> _clViewpoints;
};


class AppMeshExport SaveMeshVRML
{
  public:
    SaveMeshVRML (const MeshKernel &rclM);
    SaveMeshVRML (const MeshKernel &rclM, VRMLInfo* pclVRMLInfo);
    virtual ~SaveMeshVRML (void){}

    bool Save (std::ofstream &rstrOut, const App::Material &rclMat) const;
    bool Save (std::ofstream &rstrOut, const std::vector<App::Color> &raclColor, const App::Material &rclMat, bool bColorPerVertex = true) const;

  protected:
    void WriteVRMLHeaderInfo(std::ofstream &rstrOut) const;
    void WriteVRMLAnnotations(std::ofstream &rstrOut) const;
    void WriteVRMLViewpoints(std::ofstream &rstrOut) const;
    void WriteVRMLColorbar(std::ofstream &rstrOut) const;

    const MeshKernel &_rclMesh;   // reference to mesh data structure
    VRMLInfo* _pclVRMLInfo;
};


class AppMeshExport SaveMeshNastran
{
  public:
    SaveMeshNastran (const MeshKernel &rclM);
    virtual ~SaveMeshNastran (void){}

    bool Load (std::ifstream &rstrIn);

    bool Save (std::ofstream &rstrOut);

  protected:
    const MeshKernel &_rclMesh;   // reference to mesh data structure
};

class AppMeshExport LoadMeshNastran
{
  public:
    LoadMeshNastran (MeshKernel &rclM);
    virtual ~LoadMeshNastran (void){}

    bool Load (std::ifstream &rstrIn);

  protected:
    MeshKernel &_rclMesh;   // reference to mesh data structure
};


class AppMeshExport LoadMeshCadmouldFE
{
  public:
    LoadMeshCadmouldFE (MeshKernel &rclM);
    virtual ~LoadMeshCadmouldFE (void){}

    bool Load (std::ifstream &rstrIn);

  protected:
    MeshKernel &_rclMesh;   // reference to mesh data structure
};

class AppMeshExport SaveMeshCadmouldFE
{
  public:
    SaveMeshCadmouldFE (const MeshKernel &rclM);
    virtual ~SaveMeshCadmouldFE (void){}

    bool Save (std::ofstream &rstrOut);

  protected:
    const MeshKernel &_rclMesh;   // reference to mesh data structure
};



} // namespace MeshCore

#endif // MESH_IO_H 
