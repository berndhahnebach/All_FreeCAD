/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "FeatureMeshDefects.h"
#include "Core/Degeneration.h"
#include "Core/TopoAlgorithm.h"

using namespace Mesh;


//===========================================================================
// Defects Feature
//===========================================================================

PROPERTY_SOURCE(Mesh::FixDefects, Mesh::Feature)

FixDefects::FixDefects()
{
  ADD_PROPERTY(Source  ,(0));
}

FixDefects::~FixDefects()
{
}

int FixDefects::execute(void)
{
  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::HarmonizeNormals, Mesh::FixDefects)

HarmonizeNormals::HarmonizeNormals()
{
}

HarmonizeNormals::~HarmonizeNormals()
{
}

int HarmonizeNormals::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshFixOrientation eval(*kernel);
  eval.Fixup();
  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FlipNormals, Mesh::FixDefects)

FlipNormals::FlipNormals()
{
}

FlipNormals::~FlipNormals()
{
}

int FlipNormals::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshTopoAlgorithm cTopAlg(*kernel);
  cTopAlg.FlipNormals();
  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixNonManifolds, Mesh::FixDefects)

FixNonManifolds::FixNonManifolds()
{
}

FixNonManifolds::~FixNonManifolds()
{
}

int FixNonManifolds::execute(void)
{
  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDuplicatedFaces, Mesh::FixDefects)

FixDuplicatedFaces::FixDuplicatedFaces()
{
}

FixDuplicatedFaces::~FixDuplicatedFaces()
{
}

int FixDuplicatedFaces::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshFixDuplicateFacets eval(*kernel);
  eval.Fixup();
  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDuplicatedPoints, Mesh::FixDefects)

FixDuplicatedPoints::FixDuplicatedPoints()
{
}

FixDuplicatedPoints::~FixDuplicatedPoints()
{
}

int FixDuplicatedPoints::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshFixDuplicatePoints eval(*kernel);
  eval.Fixup();
  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDegenerations, Mesh::FixDefects)

FixDegenerations::FixDegenerations()
{
}

FixDegenerations::~FixDegenerations()
{
}

int FixDegenerations::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshFixDegeneratedFacets eval(*kernel);
  eval.Fixup();
  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDeformations, Mesh::FixDefects)

FixDeformations::FixDeformations()
{
  ADD_PROPERTY(MaxAngle  ,(0.1f));
}

FixDeformations::~FixDeformations()
{
}

int FixDeformations::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshFixDeformedFacets eval(*kernel, MaxAngle.getValue());
  eval.Fixup();
  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixIndices, Mesh::FixDefects)

FixIndices::FixIndices()
{
}

FixIndices::~FixIndices()
{
}

int FixIndices::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshEvalNeighbourhood nb(*kernel);
  if ( !nb.Evaluate() ) {
    MeshCore::MeshFixNeighbourhood fix(*kernel);
    fix.Fixup();
  }

  MeshCore::MeshEvalRangeFacet rf(*kernel);
  if ( !rf.Evaluate() ) {
    MeshCore::MeshFixRangeFacet fix(*kernel);
    fix.Fixup();
  }

  MeshCore::MeshEvalRangePoint rp(*kernel);
  if ( !rp.Evaluate() ) {
    MeshCore::MeshFixRangePoint fix(*kernel);
    fix.Fixup();
  }

  MeshCore::MeshEvalCorruptedFacets cf(*kernel);
  if ( !nb.Evaluate() ) {
    MeshCore::MeshFixCorruptedFacets fix(*kernel);
    fix.Fixup();
  }

  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FillHoles, Mesh::FixDefects)

FillHoles::FillHoles()
{
  ADD_PROPERTY(FillupHolesOfLength,(0));
}

FillHoles::~FillHoles()
{
}

int FillHoles::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshTopoAlgorithm cTopAlg(*kernel);
  cTopAlg.FillupHoles(FillupHolesOfLength.getValue());

  Mesh.setValue(kernel);

  return 0;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::RemoveComponents, Mesh::FixDefects)

RemoveComponents::RemoveComponents()
{
  ADD_PROPERTY(RemoveCompOfSize,(0));
}

RemoveComponents::~RemoveComponents()
{
}

int RemoveComponents::execute(void)
{
  Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue() );
  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel( pcFeat->Mesh.getValue() ); 

  MeshCore::MeshTopoAlgorithm cTopAlg(*kernel);
  cTopAlg.RemoveComponents(RemoveCompOfSize.getValue());

  Mesh.setValue(kernel);

  return 0;
}
