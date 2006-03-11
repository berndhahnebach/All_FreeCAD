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


MeshWithProperty& Feature::getMesh()
{
  return _cMesh;
}

void Feature::setMesh(const MeshWithProperty& New)
{
  _cMesh = New;
  Touch();
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

// VC 6 hack!!!
typedef App::PropertyContainer Parent;

void Feature::Save (Base::Writer &writer)
{
  //typedef App::AbstractFeature Parent;
  // save parent
  Parent::Save(writer);

  std::string fn = name.getValue(); fn += ".bms";
  writer.addFile(fn.c_str(), this);
  //reinterpret_cast<App::AbstractFeature*>(this)->Save(indent,str);

//  MeshCore::MeshDocXML writer(*_cMesh.getKernel());

//  writer.Save(indent,str);

}

void Feature::Restore(Base::XMLReader &reader)
{
  // save parent
  Parent::Restore(reader);

//  MeshCore::MeshDocXML geter(*_cMesh.getKernel());

//  geter.Restore(reader);


}

void Feature::SaveDocFile (Base::Writer &writer)
{
  try {
    _cMesh.getKernel()->Write(writer);
  } catch( const Base::Exception& e) {
    throw e;
  }
}

void Feature::RestoreDocFile(Base::Reader &reader)
{
  try {
    _cMesh.getKernel()->Read(reader);
  } catch( const Base::MemoryException&) {
    throw Base::Exception("Invalid mesh file");
  } catch( const Base::Exception& e) {
    throw e;
  }
}

