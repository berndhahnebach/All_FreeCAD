/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef MESH_FEATURE_MESH_DEFECTS_H
#define MESH_FEATURE_MESH_DEFECTS_H

#include <App/PropertyLinks.h>
#include "MeshFeature.h"

namespace Mesh
{

/** .
 * The FixDefects class fixes defects in the attached mesh kernel.
 * @author Werner Mayer
 */
class AppMeshExport FixDefects : public Mesh::Feature
{
  PROPERTY_HEADER(Mesh::FixDefects);

public:
  /// Constructor
  FixDefects(void);
  virtual ~FixDefects();

  /** @name Properties */
  //@{
  App::PropertyLink   Source;
  virtual const MeshCore::MeshKernel& getMesh() const;
  //@}

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}

  /// returns the type name of the ViewProvider
//  virtual const char* getViewProviderName(void){return "MeshGui::ViewProviderDefects";}
};

/** .
 * The HarmonizeNormals class harmonizes the normals of the faces.
 * @author Werner Mayer
 */
class AppMeshExport HarmonizeNormals : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::HarmonizeNormals);

public:
  /// Constructor
  HarmonizeNormals(void);
  virtual ~HarmonizeNormals();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}
};

/** .
 * The FlipNormals class flips the normals of all faces.
 * @author Werner Mayer
 */
class AppMeshExport FlipNormals : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FlipNormals);

public:
  /// Constructor
  FlipNormals(void);
  virtual ~FlipNormals();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}
};

/** .
 * The FixNonManifolds class tries to fix edges with more than two faces attached.
 * @author Werner Mayer
 */
class AppMeshExport FixNonManifolds : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixNonManifolds);

public:
  /// Constructor
  FixNonManifolds(void);
  virtual ~FixNonManifolds();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}
};

/** .
 * The FixDuplicatedFaces class removes duplicated faces from the mesh.
 * @author Werner Mayer
 */
class AppMeshExport FixDuplicatedFaces : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixDuplicatedFaces);

public:
  /// Constructor
  FixDuplicatedFaces(void);
  virtual ~FixDuplicatedFaces();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}
};

/** .
 * The FixDuplicatedPoints class removes duplicated points from the mesh.
 * @author Werner Mayer
 */
class AppMeshExport FixDuplicatedPoints : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixDuplicatedPoints);

public:
  /// Constructor
  FixDuplicatedPoints(void);
  virtual ~FixDuplicatedPoints();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}
};

/** .
 * The FixDegenerations class removes degenerated faces from the mesh.
 * @author Werner Mayer
 */
class AppMeshExport FixDegenerations : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixDegenerations);

public:
  /// Constructor
  FixDegenerations(void);
  virtual ~FixDegenerations();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}
};

/** .
 * The FixIndices class tries to validate invalid indices of the internal mesh data structure.
 * @author Werner Mayer
 */
class AppMeshExport FixIndices : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixIndices);

public:
  /// Constructor
  FixIndices(void);
  virtual ~FixIndices();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}
};

} //namespace Mesh


#endif // MESH_FEATURE_MESH_DEFECTS_H 
