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
#include <Base/Matrix.h>
using Base::Matrix4D;
#include "FeatureMeshTransform.h"

#include <App/PropertyLinks.h>
#include <App/PropertyGeo.h>

using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Transform, Mesh::Feature)


Transform::Transform(void)
{
  ADD_PROPERTY(Source  ,(0));
  ADD_PROPERTY(Position,(0));
}

int Transform::execute(void)
{

  Feature *pcFirst  = dynamic_cast<Feature*>(Source.getValue());
  if(!pcFirst || pcFirst->getStatus() != Valid)
    return 1;

  Matrix4D Matrix =Position.getValue();

 
  getMesh() = pcFirst->getMesh();
  getMesh().transform(Matrix);

  return 0;
}

