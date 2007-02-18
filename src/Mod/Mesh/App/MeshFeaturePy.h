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


#ifndef MESH_FEATURE_PY_H
#define MESH_FEATURE_PY_H

#include <Base/PyObjectBase.h>
#include <App/FeaturePy.h>

namespace Base{
  class PyObjectBase;
}

namespace Mesh
{

class Feature;
class MeshPy;

//===========================================================================
// MeshFeaturePy - Python wrapper 
//===========================================================================

// The DocTypeStd python class 
class AppMeshExport MeshFeaturePy :public App::FeaturePy
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~MeshFeaturePy();

public:
  MeshFeaturePy(Feature *pcFeature, PyTypeObject *T = &Type);
  static PyObject *PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------

  virtual PyObject *_repr(void);  				// the representation
  PyObject *_getattr(char *attr);					// __getattr__ function
  int _setattr(char *attr, PyObject *value);		// __setattr__ function
  PYFUNCDEF_D(MeshFeaturePy,countPoints)
  PYFUNCDEF_D(MeshFeaturePy,countFacets)
  PYFUNCDEF_D(MeshFeaturePy,hasNonUnifomOrientedFacets)
  PYFUNCDEF_D(MeshFeaturePy,countNonUnifomOrientedFacets)
  PYFUNCDEF_D(MeshFeaturePy,isSolid)
  PYFUNCDEF_D(MeshFeaturePy,hasNonManifolds)

private:
  Feature     *_pcFeature;
};

} //namespace Mesh



#endif // MESH_FEATURE_PY_H 
