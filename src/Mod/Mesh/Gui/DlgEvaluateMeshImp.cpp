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
# include <qcheckbox.h>
# include <qcursor.h>
# include <qlabel.h>
# include <qlineedit.h>
# include <qmessagebox.h>
# include <qpushbutton.h>
#endif

#include "DlgEvaluateMeshImp.h"

#include <Base/Interpreter.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/DockWindow.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "../App/Core/Evaluation.h"
#include "../App/Core/Degeneration.h"
#include "../App/MeshFeature.h"
#include "../App/FeatureMeshDefects.h"
#include "ViewProviderDefects.h"

using namespace MeshCore;
using namespace Mesh;
using namespace MeshGui;

CleanupHandler::CleanupHandler()
 : QObject(qApp, "CleanupHandler")
{
  // connect to lstWindowClosed signal
  connect( qApp, SIGNAL(lastWindowClosed()), this, SLOT(cleanup()) );
}

// The lastWindowClosed signal will be emitted recursively and before the cleanup slot is finished
// therefore all code inside this function must handle this case!
void CleanupHandler::cleanup()
{
  DockEvaluateMeshImp::destruct();
}

// -------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::DlgEvaluateMeshImp */

void DlgEvaluateMeshImp::OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason)
{
  if ( std::find(Reason.DeletedObjects.begin(), Reason.DeletedObjects.end(), _meshFeature) != Reason.DeletedObjects.end() )
  {
    for ( std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it ) {
      _viewer->removeViewProvider( it->second );
      delete it->second;
    }

    _vp.clear();

    _meshFeature = 0;
    cleanInformation();
  }
}

void DlgEvaluateMeshImp::OnChange(App::Application::SubjectType &rCaller, App::Application::MessageType rcReason)
{
  if ( rcReason.Why == App::AppChanges::Del && rcReason.Doc == _pDoc)
  {
    // the view is already destroyed
    for ( std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it ) {
      delete it->second;
      _viewer = 0;
    }

    _vp.clear();    
    
    _pDoc->Detach(this);
    _pDoc = 0;
    cleanInformation();
  }
}

/**
 *  Constructs a DlgEvaluateMeshImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgEvaluateMeshImp::DlgEvaluateMeshImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgEvaluateMesh( parent, name, modal, fl ), _meshFeature(0), _viewer(0), _pDoc(0)
{
  connect( buttonHelp,  SIGNAL ( clicked() ), Gui::getMainWindow(), SLOT ( whatsThis() ));

  App::GetApplication().Attach(this);
  Gui::Document* pGui = Gui::Application::Instance->activeDocument();
  _viewer = dynamic_cast<Gui::View3DInventor*>(pGui->getActiveView())->getViewer();
  _pDoc = pGui->getDocument();
  _pDoc->Attach(this);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgEvaluateMeshImp::~DlgEvaluateMeshImp()
{
  // no need to delete child widgets, Qt does it all for us
  for ( std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it ) {
    _viewer->removeViewProvider( it->second );
    delete it->second;
  }

  _vp.clear();

  App::GetApplication().Detach(this);
  if ( _pDoc )
    _pDoc->Detach(this);
}

void DlgEvaluateMeshImp::setMesh( Mesh::Feature* m )
{
  _meshFeature = m;
  onRefreshInfo();
}

void DlgEvaluateMeshImp::setFixedMesh()
{
  std::vector<App::DocumentObject*> fixed = _pDoc->getObjectsOfType(Mesh::FixDefects::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = fixed.begin(); it != fixed.end(); ++it ) {
    Mesh::FixDefects* fix = dynamic_cast<Mesh::FixDefects*>(*it);
    if ( fix && fix->Source.getValue() == _meshFeature ) {
      setMesh(fix);
      Gui::Selection().addSelection(_pDoc->getName(), fix->name.getValue());
      break;
    }
  }
}

void DlgEvaluateMeshImp::addViewProvider( const char* name )
{
  removeViewProvider( name );

  ViewProviderMeshDefects* vp = (ViewProviderMeshDefects*)Base::Type::createInstanceByName( name );
  vp->attach( _meshFeature );
  _viewer->addViewProvider( vp );
  vp->showDefects();
  _vp[name] = vp;
}

void DlgEvaluateMeshImp::removeViewProvider( const char* name )
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find( name );
  if ( it != _vp.end() ) {
    _viewer->removeViewProvider( it->second );
    delete it->second;
    _vp.erase( it );
  }
}

void DlgEvaluateMeshImp::cleanInformation()
{
  lineEditName->setText( tr("No information") );
  textLabel4->setText( tr("No information") );
  textLabel5->setText( tr("No information") );
  textLabel6->setText( tr("No information") );
  textLabelOrientation->setText( tr("No information") );
  textLabelDuplicatedFaces->setText( tr("No information") );
  textLabelDuplPoints->setText( tr("No information") );
  textLabelNonmanifolds->setText( tr("No information") );
  textLabelDegeneration->setText( tr("No information") );
  textLabelIndices->setText( tr("No information") );
  pushButtonRefresh->setDisabled(true);
  analyzeOrientation->setDisabled(true);
  repairOrientation->setDisabled(true);
  analyzeDupFaces->setDisabled(true);
  repairDupFaces->setDisabled(true);
  analyzeDupPts->setDisabled(true);
  repairDupPts->setDisabled(true);
  analyzeNonmanifolds->setDisabled(true);
  repairNonmanifolds->setDisabled(true);
  analyzeDegenerated->setDisabled(true);
  repairDegenerated->setDisabled(true);
  analyzeIndices->setDisabled(true);
  repairIndices->setDisabled(true);
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

void DlgEvaluateMeshImp::onCheckOrientation()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshOrientation");
  if ( it != _vp.end() )
  {
    if ( textLabelOrientation->isChecked() )
      it->second->show();
    else
      it->second->hide();
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
    MeshEvalOrientation eval(rMesh);
    std::vector<unsigned long> inds = eval.GetIndices();
    
    if ( inds.empty() )
    {
      textLabelOrientation->setText( tr("No flipped normals found") );
    }
    else
    {
      textLabelOrientation->setText( tr("%1 flipped normals found").arg(inds.size()) );
      textLabelOrientation->setChecked(true);
      repairOrientation->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshOrientation" );
    }

    qApp->restoreOverrideCursor();
    analyzeOrientation->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairOrientation()
{
  if ( _meshFeature )
  {
    Gui::Selection().clearSelection(_pDoc->getName());
    Gui::Selection().addSelection(_pDoc->getName(), _meshFeature->name.getValue());
    Base::Interpreter().runString("Gui.RunCommand(\"Mesh_HarmonizeNormals\")");
    repairOrientation->setEnabled(false);
    textLabelOrientation->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshOrientation" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::onCheckNonManifolds()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshNonManifolds");
  if ( it != _vp.end() )
  {
    if ( textLabelNonmanifolds->isChecked() )
      it->second->show();
    else
      it->second->hide();
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
      textLabelNonmanifolds->setChecked(true);
      repairNonmanifolds->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshNonManifolds" );
    }

    qApp->restoreOverrideCursor();
    analyzeNonmanifolds->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairNonManifolds()
{
  QMessageBox::warning(this, "Non-manifolds", "Cannot remove non-manifolds");
}

void DlgEvaluateMeshImp::onCheckIndices()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshIndices");
  if ( it != _vp.end() )
  {
    if ( textLabelIndices->isChecked() )
      it->second->show();
    else
      it->second->hide();
  }
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
      textLabelIndices->setChecked(true);
      repairIndices->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
    else if ( !rf.Evaluate() ) {
      textLabelIndices->setText( tr("Invalid face indices") );
      textLabelIndices->setChecked(true);
      repairIndices->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
    else if ( !rp.Evaluate() ) {
      textLabelIndices->setText( tr("Invalid point indices") );
      textLabelIndices->setChecked(true);
      repairIndices->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
    else if ( !cf.Evaluate() ) {
      textLabelIndices->setText( tr("Multiple point indices") );
      textLabelIndices->setChecked(true);
      repairIndices->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
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
    Gui::Selection().clearSelection(_pDoc->getName());
    Gui::Selection().addSelection(_pDoc->getName(), _meshFeature->name.getValue());
    Base::Interpreter().runString("Gui.RunCommand(\"Mesh_FixIndices\")");
    repairIndices->setEnabled(false);
    textLabelIndices->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshIndices" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::onCheckDegenerations()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDegenerations");
  if ( it != _vp.end() )
  {
    if ( textLabelDegeneration->isChecked() )
      it->second->show();
    else
      it->second->hide();
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
    std::vector<unsigned long> degen = eval.GetIndices();
    
    if ( degen.empty() )
    {
      textLabelDegeneration->setText( tr("No degenerations found") );
    }
    else
    {
      textLabelDegeneration->setText( tr("%1 degenerated faces found").arg(degen.size()) );
      textLabelDegeneration->setChecked(true);
      repairDegenerated->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
    }

    qApp->restoreOverrideCursor();
    analyzeDegenerated->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairDegenerations()
{
  if ( _meshFeature )
  {
    Gui::Selection().clearSelection(_pDoc->getName());
    Gui::Selection().addSelection(_pDoc->getName(), _meshFeature->name.getValue());
    Base::Interpreter().runString("Gui.RunCommand(\"Mesh_FixDegenerations\")");
    repairDegenerated->setEnabled(false);
    textLabelDegeneration->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::onCheckDuplicatedFaces()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDuplicatedFaces");
  if ( it != _vp.end() )
  {
    if ( textLabelDuplicatedFaces->isChecked() )
      it->second->show();
    else
      it->second->hide();
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
    std::vector<unsigned long> dupl = eval.GetIndices();
    
    if ( dupl.empty() )
    {
      textLabelDuplicatedFaces->setText( tr("No duplicated faces found") );
    }
    else
    {
      textLabelDuplicatedFaces->setText( tr("%1 duplicated faces found").arg(dupl.size()) );
      textLabelDuplicatedFaces->setChecked(true);
      repairDupFaces->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
    }

    qApp->restoreOverrideCursor();
    analyzeDupFaces->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairDuplicatedFaces()
{
  if ( _meshFeature )
  {
    Gui::Selection().clearSelection(_pDoc->getName());
    Gui::Selection().addSelection(_pDoc->getName(), _meshFeature->name.getValue());
    Base::Interpreter().runString("Gui.RunCommand(\"Mesh_FixDuplicateFaces\")");
    repairDupFaces->setEnabled(false);
    textLabelDuplicatedFaces->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::onCheckDuplicatedPoints()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDuplicatedPoints");
  if ( it != _vp.end() )
  {
    if ( textLabelDuplPoints->isChecked() )
      it->second->show();
    else
      it->second->hide();
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
      textLabelDuplPoints->setChecked(true);
      repairDupPts->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshDuplicatedPoints" );
    }

    qApp->restoreOverrideCursor();
    analyzeDupPts->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::onRepairDuplicatedPoints()
{
  if ( _meshFeature )
  {
    Gui::Selection().clearSelection(_pDoc->getName());
    Gui::Selection().addSelection(_pDoc->getName(), _meshFeature->name.getValue());
    Base::Interpreter().runString("Gui.RunCommand(\"Mesh_FixDuplicatePoints\")");
    repairDupPts->setEnabled(false);
    textLabelDuplPoints->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedPoints" );
    setFixedMesh();
  }
}

// -------------------------------------------------------------

DockEvaluateMeshImp* DockEvaluateMeshImp::_instance=0;

DockEvaluateMeshImp* DockEvaluateMeshImp::instance()
{
  // not initialized?
  if(!_instance)
  {
    _instance = new DockEvaluateMeshImp( Gui::getMainWindow(), "Evaluate Mesh");
    _instance->setSizeGripEnabled(false);
    // embed this dialog into a dockable widget container
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    Gui::DockContainer* pDockDlg = new Gui::DockContainer( Gui::getMainWindow(), "Evaluate Mesh" );
    pDockMgr->addDockWindow("Evaluate Mesh", pDockDlg, Qt::DockRight );

    // do not allow to hide
    pDockDlg->dockWindow()->setCloseMode(QDockWindow::Never);
    pDockDlg->setDockedWidget(_instance);

    // try to set an appropriate width
    pDockDlg->setFixedExtentWidth( _instance->width() );
    pDockDlg->show();
  }

  return _instance;
}

void DockEvaluateMeshImp::destruct ()
{
  if ( _instance != 0 )
  {
    DockEvaluateMeshImp *pTmp = _instance;
    _instance = 0;
    delete pTmp;
  }
}

bool DockEvaluateMeshImp::hasInstance()
{
  return _instance != 0;
}

/**
 *  Constructs a DockEvaluateMeshImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DockEvaluateMeshImp::DockEvaluateMeshImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgEvaluateMeshImp( parent, name, false, fl )
{
}

/**
 *  Destroys the object and frees any allocated resources
 */
DockEvaluateMeshImp::~DockEvaluateMeshImp()
{
  _instance = 0;
}

/**
 * Destroys the dock window this object is embedded into thus it destroys itself.
 */
void DockEvaluateMeshImp::reject()
{
  Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
  pDockMgr->removeDockedWidget(this);
}

/**
 * Returns an appropriate size hint for the dock window.
 */
QSize DockEvaluateMeshImp::sizeHint () const
{
  return QSize(371, 579);
}

#include "DlgEvaluateMesh.cpp"
#include "moc_DlgEvaluateMesh.cpp"
#include "moc_DlgEvaluateMeshImp.cpp"
