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
:pcMeshFeaturePy(0)
{
  ADD_PROPERTY(Mesh, (MeshCore::MeshKernel()));
}

Feature::~Feature()
{
  if ( pcMeshFeaturePy )
    pcMeshFeaturePy->DecRef();
}

int Feature::execute(void)
{
  return 0;
}

Base::PyObjectBase *Feature::GetPyObject(void)
{
  if(!pcMeshFeaturePy){
    pcMeshFeaturePy = new MeshFeaturePy(this);
  }
 
  // Increment every time when this object is returned
  pcMeshFeaturePy->IncRef();
  return pcMeshFeaturePy; 
}

void Feature::onChanged(App::Property* prop)
{
  // Ignore some properties
  if ( prop == &Mesh )
    return;
  else
    AbstractFeature::onChanged(prop);
}

const MeshCore::MeshKernel& Feature::getMesh() const
{
  return Mesh.getValue();
}

