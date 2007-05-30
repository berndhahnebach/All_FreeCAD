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



#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include <Mod/Part/App/TopologyPy.h>

#include "Core/MeshIO.h"

#include "MeshFeature.h"
#include "MeshFeaturePy.h"
#include <App/Feature.h>

using namespace Mesh;


//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Mesh::Feature, App::AbstractFeature)

Feature::Feature()
{
  ADD_PROPERTY(Mesh, (MeshCore::MeshKernel()));
}

Feature::~Feature()
{
}

int Feature::execute(void)
{
  return 0;
}

PyObject *Feature::getPyObject(void)
{
 if(PythonObject.is(Py::_None())){
    // ref counter is set to 1
    PythonObject.set(new MeshFeaturePy(this),true);
  }
  return Py::new_reference_to(PythonObject); 
}

void Feature::onChanged(const App::Property* prop)
{
  // FIXME: What is to do if only the mesh property has changed? I think in this case no recomputation needs to be done
  // for the owner feature because the mesh is the output of the previous recomputation and not an input parameter.
  // But on the other hand all feature objects that depend on this mesh must be executed because for these ones it is
  // an input parameter.
  if ( prop == &Mesh ) {
  // call for undo/redo only but do not set to be modified
    DocumentObject::onChanged(prop);
  } else {
    // set to be modified
    AbstractFeature::onChanged(prop);
  }
}

const MeshCore::MeshKernel& Feature::getMesh() const
{
  return Mesh.getValue();
}

