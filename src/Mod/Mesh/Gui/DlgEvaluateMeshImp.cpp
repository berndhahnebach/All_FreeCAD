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
# include <qlineedit.h>
# include <qmessagebox.h>
# include <qpushbutton.h>
#endif

#include <Gui/MainWindow.h>
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
  connect( buttonHelp,  SIGNAL ( clicked() ), Gui::getMainWindow(), SLOT ( whatsThis() ));
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
      textLabelOrientation->setText( tr("No wrong oriented faces found") );
    }
    else
    {
      textLabelOrientation->setText( tr("One or more wrong oriented faces found") );
      repairOrientation->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeOrientation->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairOrientation()
{
  if ( _meshFeature )
  {
    qApp->setOverrideCursor(Qt::WaitCursor);

    MeshKernel cMesh = _meshFeature->getMesh();
    MeshFixNormals eval(cMesh);
    
    if ( eval.Fixup() )
    {
      _meshFeature->Mesh.setValue( cMesh );
      textLabelOrientation->setText( tr("Wrong oriented faces fixed") );
      repairOrientation->setEnabled(false);
    }
    else
    {
      textLabelOrientation->setText( tr("Still wrong oriented faces found") );
    }

    qApp->restoreOverrideCursor();
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
      textLabelNonmanifolds->setText( tr("No non-manifolds found") );
    }
    else
    {
      textLabelNonmanifolds->setText( tr("%1 non-manifolds found").arg(eval.CountManifolds()) );
      repairNonmanifolds->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeNonmanifolds->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairNonManifolds()
{
  QMessageBox::warning(this, "Non-manifolds", "Cannot remove non-manifolds");
}

void DlgEvaluateMeshImp::onAnalyzeIndices()
{
  if ( _meshFeature )
  {
    analyzeIndices->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalNeighbourhood nb(rMesh);
    MeshEvalRangeFacet rf(rMesh);
    MeshEvalRangePoint rp(rMesh);
    MeshEvalCorruptedFacets cf(rMesh);
    
    if ( !nb.Evaluate() ) {
      textLabelIndices->setText( tr("Invalid neighbour indices") );
      repairIndices->setEnabled(true);
    }
    else if ( !rf.Evaluate() ) {
      textLabelIndices->setText( tr("Invalid face indices") );
      repairIndices->setEnabled(true);
    }
    else if ( !rp.Evaluate() ) {
      textLabelIndices->setText( tr("Invalid point indices") );
      repairIndices->setEnabled(true);
    }
    else if ( !cf.Evaluate() ) {
      textLabelIndices->setText( tr("Multiple point indices") );
      repairIndices->setEnabled(true);
    }
    else {
      textLabelIndices->setText( tr("No invalid indices found") );
    }

    qApp->restoreOverrideCursor();
    analyzeIndices->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairIndices()
{
  if ( _meshFeature )
  {
    qApp->setOverrideCursor(Qt::WaitCursor);

    MeshKernel cMesh = _meshFeature->getMesh();
    MeshFixNeighbourhood eval(cMesh);
    
    if ( eval.Fixup() )
    {
      _meshFeature->Mesh.setValue( cMesh );
      textLabelIndices->setText( tr("Invalid neighbourhood fixed") );
      repairIndices->setEnabled(false);
    }
    else
    {
      textLabelIndices->setText( tr("Still invalid neighbourhood") );
    }

    qApp->restoreOverrideCursor();
  }
}

void DlgEvaluateMeshImp::onAnalyzeDegenerations()
{
  if ( _meshFeature )
  {
    analyzeDegenerated->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->getMesh();
    MeshEvalDegeneratedFacets eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      textLabelDegeneration->setText( tr("No degenerated faces found") );
    }
    else
    {
      textLabelDegeneration->setText( tr("Degenerated faces found") );
      repairDegenerated->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeDegenerated->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairDegenerations()
{
  if ( _meshFeature )
  {
    qApp->setOverrideCursor(Qt::WaitCursor);

    MeshKernel cMesh = _meshFeature->getMesh();
    MeshFixDegeneratedFacets eval(cMesh);
    
    if ( eval.Fixup() )
    {
      _meshFeature->Mesh.setValue( cMesh );
      textLabelDegeneration->setText( tr("Degenerated faces fixed") );
      repairDegenerated->setEnabled(false);
    }
    else
    {
      textLabelDegeneration->setText( tr("Still degenerated faces found") );
    }

    qApp->restoreOverrideCursor();
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
    MeshEvalDuplicateFacets eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      textLabelDuplicatedFaces->setText( tr("No duplicated faces found") );
    }
    else
    {
      textLabelDuplicatedFaces->setText( tr("Duplicated faces found") );
      repairDupFaces->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeDupFaces->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairDuplicatedFaces()
{
  if ( _meshFeature )
  {
    qApp->setOverrideCursor(Qt::WaitCursor);

    MeshKernel cMesh = _meshFeature->getMesh();
    MeshFixDuplicateFacets eval(cMesh);
    
    if ( eval.Fixup() )
    {
      _meshFeature->Mesh.setValue( cMesh );
      textLabelDuplicatedFaces->setText( tr("Duplicated faces removed") );
      analyzeDupFaces->setEnabled(false);
    }
    else
    {
      textLabelDuplicatedFaces->setText( tr("Still duplicated faces found") );
    }

    qApp->restoreOverrideCursor();
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
    MeshEvalDuplicatePoints eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      textLabelDuplPoints->setText( tr("No duplicated points found") );
    }
    else
    {
      textLabelDuplPoints->setText( tr("Duplicated points found") );
      textLabelDuplPoints->setEnabled(true);
    }

    qApp->restoreOverrideCursor();
    analyzeDupPts->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairDuplicatedPoints()
{
  if ( _meshFeature )
  {
    qApp->setOverrideCursor(Qt::WaitCursor);

    MeshKernel cMesh = _meshFeature->getMesh();
    MeshFixDuplicatePoints eval(cMesh);
    
    if ( eval.Fixup() )
    {
      _meshFeature->Mesh.setValue( cMesh );
      textLabelDuplPoints->setText( tr("Duplicated points removed") );
      analyzeDupPts->setEnabled(false);
    }
    else
    {
      textLabelDuplPoints->setText( tr("Still duplicated points found") );
    }

    qApp->restoreOverrideCursor();
  }
}

#include "DlgEvaluateMesh.cpp"
#include "moc_DlgEvaluateMesh.cpp"
