/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <fcntl.h>
# include <TFunction_Logbook.hxx>
# include <ios>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>

#include "Core/Algorithm.h"
#include "Core/Evaluation.h"
#include "FeatureMeshSegmentByMesh.h"


using namespace Mesh;
using namespace MeshCore;

void FeatureMeshSegmentByMesh::initFeature(void)
{
  addProperty("Link","Source");
  addProperty("Link","Tool");
}

int FeatureMeshSegmentByMesh::execute(TFunction_Logbook& log)
{
  MeshFeature *pcMesh  = dynamic_cast<MeshFeature*>(getPropertyLink("Source"));
  MeshFeature *pcTool  = dynamic_cast<MeshFeature*>(getPropertyLink("Tool"));
  if(!pcMesh || pcMesh->getStatus() != Valid || !pcTool || pcTool->getStatus() != Valid)
    return 1;

  MeshWithProperty& rMesh = pcMesh->getMesh();
  MeshWithProperty& rTool = pcTool->getMesh();

  MeshKernel& rMeshKernel = *rMesh.getKernel();
  MeshKernel& rToolMesh = *rTool.getKernel();

  // check if the toolmesh is a solid
  if ( MeshEvalSolid(rToolMesh).Validate(false) != MeshEvalSolid::Valid )
    return 1; // no solid

  std::vector<unsigned long> faces;

  MeshAlgorithm cAlg(rMeshKernel);
  cAlg.GetFacetsFromToolMesh(rToolMesh, Vector3D(0.0, 1.0f, 0.0f), faces);

  std::vector<MeshGeomFacet> tria;
  for ( std::vector<unsigned long>::iterator it = faces.begin(); it != faces.end(); ++it )
    tria.push_back( rMeshKernel.GetFacet(*it) );
  getMesh().getKernel()->operator = ( tria );

  return 0;
}

