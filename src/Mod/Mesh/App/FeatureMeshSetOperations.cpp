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
# include <ios>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>

#include "Core/MeshKernel.h"
#include "Core/Algorithm.h"
#include "Core/Evaluation.h"
#include "Core/Iterator.h"
#include "Core/Visitor.h"

#include "Core/SetOperations.h"

#include "FeatureMeshSetOperations.h"


using namespace Mesh;
using namespace std;

PROPERTY_SOURCE(Mesh::SetOperations, Mesh::Feature)


SetOperations::SetOperations(void)
{
  ADD_PROPERTY(Source1  ,(0));
  ADD_PROPERTY(Source2  ,(0));
  ADD_PROPERTY(OperationType, ("union"));
}

int SetOperations::execute(void)
{
  Mesh::Feature *mesh1  = dynamic_cast<Mesh::Feature*>(Source1.getValue());
  Mesh::Feature *mesh2  = dynamic_cast<Mesh::Feature*>(Source2.getValue());

  if ((mesh1 != NULL) && (mesh2 != NULL))
  {
    const MeshCore::MeshKernel& meshKernel1 = mesh1->getMesh();
    const MeshCore::MeshKernel& meshKernel2 = mesh2->getMesh();

    MeshCore::MeshKernel *pcKernel = new MeshCore::MeshKernel(); // Result Meshkernel

    MeshCore::SetOperations::OperationType type;
    string ot(OperationType.getValue());
    if (ot == "union")
      type = MeshCore::SetOperations::Union;
    else if (ot == "intersection")
      type = MeshCore::SetOperations::Intersect;
    else if (ot == "difference")
      type = MeshCore::SetOperations::Difference;
    else
    {
      type = MeshCore::SetOperations::Union;
      //throw new exception("operation type must be: union, intersection or difference");
    }
    
    MeshCore::SetOperations setOp(meshKernel1, meshKernel2, *pcKernel, type, 1.0e-5);

    setOp.Do();
    
    Mesh.setValue(pcKernel);


  }
  else
  { // Error mesh property
  }

    

  

  return 0;
}

