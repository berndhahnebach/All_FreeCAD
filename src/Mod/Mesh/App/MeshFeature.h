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
}

namespace Mesh
{

class Property;

/** Base class of all mesh feature classes in FreeCAD.
 * This class holds an MeshKernel object.
 * \author Werner Mayer
 */
class AppMeshExport MeshFeature : public App::Feature
{
public:
  /// Constructor
  MeshFeature(void);

  virtual void InitLabel(const TDF_Label &rcLabel);


  /** @name methodes used for recalculation (update) */
  //@{
  /** 
   *  We compute the object and topologically name it.
   *  If during the execution we found something wrong,
   *  we return the number of the failure. For example:
   *  1 - an attribute hasn't been found,
   *  2 - algorithm failed
   *  0 - no mistakes were found.
   */
  virtual Standard_Integer Execute(TFunction_Logbook& log);

  /**
   * Validation of the object label, its arguments and its results.
   */
  virtual void Validate(TFunction_Logbook& log);
  //@}


  /** @name methods for convenient handling of parameter (properties) */
  //@{
  /** Get the access to the actual mesh */
  MeshWithProperty& getMesh();
  /** Set the actual mesh */
  void setMesh(const MeshWithProperty&);
  //@}

  virtual Base::PyObjectBase *MeshFeature::GetPyObject(void);

  /// Returns the Name/Type of the feature
  virtual const char *Type(void){return "Mesh";};

protected:
  MeshWithProperty _cMesh;
};

} //namespace Mesh



#endif 
