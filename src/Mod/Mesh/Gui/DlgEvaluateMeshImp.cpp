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
# include <qapplication.h>
# include <qcursor.h>
# include <qlabel.h>
# include <qpushbutton.h>
#endif

#include "../App/Core/Evaluation.h"
#include "../App/Core/Degeneration.h"
#include "../App/MeshFeature.h"
#include "DlgEvaluateMeshImp.h"

using namespace MeshCore;
using namespace Mesh;
using namespace MeshGui;

/* TRANSLATOR Gui::Dialog::DlgEvaluateMeshImp */

/**
 *  Constructs a DlgEvaluateMeshImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgEvaluateMeshImp::DlgEvaluateMeshImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgEvaluateMesh( parent, name, modal, fl ), _meshFeature(0)
{
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgEvaluateMeshImp::~DlgEvaluateMeshImp()
{
  // no need to delete child widgets, Qt does it all for us
}

void DlgEvaluateMeshImp::setMesh( Mesh::Feature* m )
{
  _meshFeature = m;
  onRefreshInfo();
}

void DlgEvaluateMeshImp::onRefreshInfo()
{
  if ( _meshFeature )
  {
    lineEditName->setText( _meshFeature->name.getValue() );
    const MeshKernel& rMesh = _meshFeature->getMesh();
    textLabel4->setText( QString("%1").arg(rMesh.CountFacets()) );
    textLabel6->setText( QString("%1").arg(rMesh.CountPoints()) );
  
    unsigned long ulCtEd=0;
    unsigned long openEdges = 0, closedEdges = 0;

    const MeshFacetArray& rFacets = rMesh.GetFacets();
    for (MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); it++)
    {
      for (int i = 0; i < 3; i++)
      {
        if (it->_aulNeighbours[i] == ULONG_MAX)
          openEdges++;
        else
          closedEdges++;
      }
    }

    ulCtEd = openEdges + (closedEdges / 2);
    textLabel5->setText( QString("%1").arg(ulCtEd) );
  }
}

void DlgEvaluateMeshImp::onAnalyzeOrientation()
{
  if ( _meshFeature )
  {
    analyzeOrientation->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalNormals eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      textLabelOrientation->setText( "No wrong oriented faces found" );
    }
    else
    {
      textLabelOrientation->setText( QString("One or more wrong oriented faces found") );
      repairOrientation->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeOrientation->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onAnalyzeNonManifolds()
{
  if ( _meshFeature )
  {
    analyzeNonmanifolds->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalTopology eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      textLabelNonmanifolds->setText( "No non-manifolds found" );
    }
    else
    {
      textLabelNonmanifolds->setText( QString("%1 non-manifolds found").arg(eval.CountManifolds()) );
      repairNonmanifolds->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeNonmanifolds->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onAnalyzeTopology()
{
  if ( _meshFeature )
  {
    analyzeTopology->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalNeighbourhood eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      textLabelTopology->setText( "No invalid neighbourhood found" );
    }
    else
    {
      textLabelTopology->setText( QString("Invalid neighbourhood") );
      repairTopology->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeTopology->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onAnalyzeDegenerations()
{
  if ( _meshFeature )
  {
    analyzeDegenrated->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalDegenerations eval(rMesh);
    unsigned long ct = eval.CountEdgeTooSmall(FLOAT_EPS);
    
    if ( ct == 0 )
    {
      textLabelDegeneration->setText( "No degenrated faces found." );
    }
    else
    {
      textLabelDegeneration->setText( QString("%1 degenerated faces found").arg(ct) );
      repairDegenrated->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeDegenrated->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onAnalyzeDuplicatedFaces()
{
  if ( _meshFeature )
  {
    analyzeDupFaces->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalDegenerations eval(rMesh);
    
    if ( !eval.HasDegeneration(MeshEvalDegenerations::DuplicatedFacets) )
    {
      textLabelDuplicatedFaces->setText( "No duplicated faces found." );
    }
    else
    {
      textLabelDuplicatedFaces->setText( QString("Duplicated faces found") );
      repairDupFaces->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeDupFaces->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onAnalyzeDuplicatedPoints()
{
  if ( _meshFeature )
  {
    analyzeDupPts->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalDegenerations eval(rMesh);
    
    if ( !eval.HasDegeneration(MeshEvalDegenerations::DuplicatedPoints) )
    {
      textLabelDuplPoints->setText( "No duplicated points found." );
    }
    else
    {
      textLabelDuplPoints->setText( QString("Duplicated points found") );
      textLabelDuplPoints->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeDupPts->setEnabled(true);
  }
}

#include "DlgEvaluateMesh.cpp"
#include "moc_DlgEvaluateMesh.cpp"
