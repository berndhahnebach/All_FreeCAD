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
# include <TDF_Label.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Mod/Part/App/TopologyPy.h>


#include "MeshFeature.h"
#include "MeshFeaturePy.h"

using namespace Mesh;


//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Mesh::MeshFeature, App::Feature)

MeshFeature::MeshFeature()
:pcMeshFeaturePy(0)
{
}

MeshFeature::~MeshFeature()
{
  if ( pcMeshFeaturePy )
    pcMeshFeaturePy->DecRef();
}

void MeshFeature::initFeature(void)
{
}

Standard_Integer MeshFeature::execute(void)
{
  return 0;
}


MeshWithProperty& MeshFeature::getMesh()
{
  return _cMesh;
}

void MeshFeature::setMesh(const MeshWithProperty& New)
{
  _cMesh = New;
  Touch();
}

Base::PyObjectBase *MeshFeature::GetPyObject(void)
{
  if(!pcMeshFeaturePy){
    pcMeshFeaturePy = new MeshFeaturePy(this);
  }
 
  // Increment every time when this object is returned
  pcMeshFeaturePy->IncRef();
  return pcMeshFeaturePy; 
}

