/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef MESH_FEATURE_H
#define MESH_FEATURE_H

#include <App/Feature.h>

#include "Core/MeshKernel.h"
#include "Mesh.h"

class TFunction_Logbook;
class FeaturePy;

namespace Base{
  class PyObjectBase;
  class Reader;
  class Writer;
}

namespace Mesh
{

class Property;
class MeshFeaturePy;

/** Base class of all mesh feature classes in FreeCAD.
 * This class holds a MeshKernel object.
 * \author Werner Mayer
 */
class AppMeshExport MeshFeature : public App::Feature
{
  PROPERTY_HEADER(Mesh::MeshFeature);

public:
  /// Constructor
  MeshFeature(void);
  virtual ~MeshFeature();

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  /// Returns the Name/Type of the feature
  virtual const char *type(void){return "Mesh";};
  //@}

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::Reader &reader);

  virtual void SaveData (std::ostream &str);
  virtual void RestoreData(std::istream &str);


  /** @name methods for mesh handling */
  //@{
  /** Get the access to the actual mesh */
  virtual MeshWithProperty& getMesh();
  /** Set the actual mesh */
  virtual void setMesh(const MeshWithProperty&);
  //@}

  /// handles the MeshPy object
  virtual Base::PyObjectBase* GetPyObject(void);


protected:
  MeshWithProperty _cMesh;

private:
  MeshFeaturePy *pcMeshFeaturePy;
};

} //namespace Mesh



#endif 
