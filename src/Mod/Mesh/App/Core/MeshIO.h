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
 * The MeshInput class is able to read a mesh object from a input stream
 * in various formats.
 */
class AppMeshExport MeshInput
{
public:
  MeshInput (MeshKernel &rclM): _rclMesh(rclM){};
  virtual ~MeshInput (void) { }

  /** Loads an STL file either in binary or ASCII format. 
   * Therefore the file header gets checked to decide if the file is binary or not.
   */
  bool LoadSTL (std::istream &rstrIn);
  /** Loads an ASCII STL file. */
  bool LoadAsciiSTL (std::istream &rstrIn);
  /** Loads a binary STL file. */
  bool LoadBinarySTL (std::istream &rstrIn);
  /** Loads an OBJ Mesh file. */
  bool LoadOBJ (std::istream &rstrIn);
  /** Loads the mesh object from an XML file. */
  void LoadXML (Base::XMLReader &reader);
  /** Loads an OpenInventor file. */
  bool LoadInventor (std::istream &rstrIn);
  /** Loads a Nastran file. */
  bool LoadNastran (std::istream &rstrIn);
  /** Loads a Cadmould FE file. */
  bool LoadCadmouldFE (std::ifstream &rstrIn);

protected:
  MeshKernel &_rclMesh;   /**< reference to mesh data structure */
};

/**
 * The MeshOutput class is able to write a mesh object to an ouput stream
 * on various formats.
 */
class AppMeshExport MeshOutput
{
public:
  MeshOutput (const MeshKernel &rclM): _rclMesh(rclM){};
  virtual ~MeshOutput (void) { }

  /** Saves the mesh object into an ASCII STL file. */
  bool SaveAsciiSTL (std::ostream &rstrOut) const;
  /** Saves the mesh object into a binary STL file. */
  bool SaveBinarySTL (std::ostream &rstrOut) const;
  /** Saves the mesh object into an XML file. */
  void SaveXML (Base::Writer &writer) const;
  /** Writes an OpenInventor file. */
  bool SaveInventor (std::ostream &rstrOut) const;
  /** Writes a VRML file. */
  bool SaveVRML (std::ostream &rstrOut) const;
  /** Writes a Nastran file. */
  bool SaveNastran (std::ostream &rstrOut) const;
  /** Writes a Cadmould FE file. */
  bool SaveCadmouldFE (std::ostream &rstrOut) const;

protected:
  const MeshKernel &_rclMesh;   /**< reference to mesh data structure */
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

    bool Save (std::ostream &rstrOut, const App::Material &rclMat) const;
    bool Save (std::ostream &rstrOut, const std::vector<App::Color> &raclColor, const App::Material &rclMat, bool bColorPerVertex = true) const;

  protected:
    void WriteVRMLHeaderInfo(std::ostream &rstrOut) const;
    void WriteVRMLAnnotations(std::ostream &rstrOut) const;
    void WriteVRMLViewpoints(std::ostream &rstrOut) const;

    const MeshKernel &_rclMesh;   // reference to mesh data structure
    VRMLInfo* _pclVRMLInfo;
};


} // namespace MeshCore

#endif // MESH_IO_H 
