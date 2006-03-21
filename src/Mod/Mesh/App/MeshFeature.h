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

class FeaturePy;

namespace Base{
  class PyObjectBase;
  class XMLReader;
  class Writer;
}

namespace MeshCore {
  class MeshKernel;
}

namespace Mesh
{

class Property;
class MeshFeaturePy;

/** Base class of all mesh feature classes in FreeCAD.
 * This class holds a MeshKernel object.
 * \author Werner Mayer
 */
class AppMeshExport Feature : public App::AbstractFeature
{
  PROPERTY_HEADER(Mesh::Feature);

public:
  /// Constructor
  Feature(void);
  virtual ~Feature();

  /** @name Properties */
  //@{
  /** Property that holds the mesh data. */
  PropertyMeshKernel Mesh;
  /** Returns a reference to the mesh kernel object. This can be reimplemented in subclasses that
   * has a link property to another mesh feature to return the correct mesh kernel object.
   *
   * For example a curvature feature has a link to another mesh feature and a container with the curvature
   * information. Its own mesh kernel is emtpy.
   * To render the mesh with curvature information we must get the mesh kernel of the other mesh feature.
   *
   * The default implementation returns a reference to its own mesh kernel.
   */
  virtual const MeshCore::MeshKernel& getMesh() const;
  //@}

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  virtual void onChanged(App::Property* prop);
  //@}

  /// returns the type name of the ViewProvider
  virtual const char* getViewProviderName(void){return "MeshGui::ViewProviderMesh";}

  /// handles the MeshPy object
  virtual Base::PyObjectBase* GetPyObject(void);

private:
  MeshFeaturePy *pcMeshFeaturePy;
};

} //namespace Mesh



#endif 
