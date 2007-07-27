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

#include "DlgEvaluateMeshImp.h"

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <Base/Interpreter.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/DockWindowManager.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include <Mod/Mesh/App/Core/Evaluation.h>
#include <Mod/Mesh/App/Core/Degeneration.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/FeatureMeshDefects.h>
#include "ViewProviderDefects.h"

using namespace MeshCore;
using namespace Mesh;
using namespace MeshGui;

CleanupHandler::CleanupHandler()
 : QObject(qApp)
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

/* TRANSLATOR MeshGui::DlgEvaluateMeshImp */
/*
void DlgEvaluateMeshImp::OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason)
{
  if ( std::find(Reason.DeletedObjects.begin(), Reason.DeletedObjects.end(), _meshFeature) != Reason.DeletedObjects.end() )
  {
    removeViewProviders();

    QStringList items;
    std::vector<App::DocumentObject*> objs = _pDoc->getObjectsOfType(Mesh::Feature::getClassTypeId());
    for ( std::vector<App::DocumentObject*>::iterator jt = objs.begin(); jt != objs.end(); ++jt )
    {
      if ( _meshFeature != *jt )
        items.push_back( (*jt)->name.getValue() );
    }

    meshNameButton->clear();
    meshNameButton->insertItem(0, tr("No selection"));
    meshNameButton->insertItems(1, items);
    meshNameButton->setDisabled(items.empty());
    cleanInformation();

    _meshFeature = 0;
  }
}
*/
void DlgEvaluateMeshImp::slotDeletedObject(App::DocumentObject& Obj)
{

  if ( &Obj == _meshFeature) 
  {
    removeViewProviders();

    QStringList items;
    std::vector<App::DocumentObject*> objs = _pDoc->getObjectsOfType(Mesh::Feature::getClassTypeId());
    for ( std::vector<App::DocumentObject*>::iterator jt = objs.begin(); jt != objs.end(); ++jt )
    {
      if ( _meshFeature != *jt )
        items.push_back( (*jt)->name.getValue() );
    }

    meshNameButton->clear();
    meshNameButton->insertItem(0, tr("No selection"));
    meshNameButton->insertItems(1, items);
    meshNameButton->setDisabled(items.empty());
    cleanInformation();

    _meshFeature = 0;
  }
 
}
/*
void DlgEvaluateMeshImp::OnChange(App::Application::SubjectType &rCaller, App::Application::MessageType rcReason)
{
  if ( rcReason.Why == App::AppChanges::Del && rcReason.Doc == _pDoc)
  {
    // the view is already destroyed
    for ( std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it ) {
      delete it->second;
    }

    _vp.clear();    
    
    _pDoc->Detach(this);
    _pDoc = 0;
    _viewer = 0;
    on_refreshButton_clicked();
  }
}
*/
void DlgEvaluateMeshImp::slotDeletedDocument(App::Document& Doc)
{
    // the view is already destroyed
    for ( std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it ) {
      delete it->second;
    }

    _vp.clear();    
    
    //_pDoc->Detach(this);
    _pDoc = 0;
    _viewer = 0;
    on_refreshButton_clicked();
 
}


/**
 *  Constructs a DlgEvaluateMeshImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgEvaluateMeshImp::DlgEvaluateMeshImp( QWidget* parent, Qt::WFlags fl )
  : QDialog( parent, fl ), _meshFeature(0), _viewer(0), _pDoc(0)
{
  this->setupUi(this);
  connect( buttonHelp,  SIGNAL ( clicked() ), Gui::getMainWindow(), SLOT ( whatsThis() ));

  //App::GetApplication().Attach(this);
  Gui::Document* pGui = Gui::Application::Instance->activeDocument();
  _viewer = dynamic_cast<Gui::View3DInventor*>(pGui->getActiveView())->getViewer();
  _pDoc = pGui->getDocument();
  //_pDoc->Attach(this);

  _pDoc->signalDeletedObject.connect(boost::bind(&MeshGui::DlgEvaluateMeshImp::slotDeletedObject, this, _1));
  App::GetApplication().signalDeletedDocument.connect(boost::bind(&MeshGui::DlgEvaluateMeshImp::slotDeletedDocument, this, _1));
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

  //App::GetApplication().Detach(this);
  //if ( _pDoc )
  //  _pDoc->Detach(this);
}

void DlgEvaluateMeshImp::setMesh( Mesh::Feature* m )
{
  _meshFeature = m;
  
  on_refreshButton_clicked();

  int ct = meshNameButton->count();
  for (int i=1; i<ct; i++) {
    if ( meshNameButton->itemText(i) == _meshFeature->name.getValue() ) {
      meshNameButton->setCurrentIndex(i);
      on_meshNameButton_activated(i);
      break;
    }
  }
}

void DlgEvaluateMeshImp::setFixedMesh()
{
  std::vector<App::DocumentObject*> fixed = _pDoc->getObjectsOfType(Mesh::FixDefects::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = fixed.begin(); it != fixed.end(); ++it ) {
    Mesh::FixDefects* fix = dynamic_cast<Mesh::FixDefects*>(*it);
    if ( fix && fix->Source.getValue() == _meshFeature ) {
      setMesh(fix);
      break;
    }
  }
}

void DlgEvaluateMeshImp::addViewProvider( const char* name )
{
  removeViewProvider( name );

  ViewProviderMeshDefects* vp = (ViewProviderMeshDefects*)Base::Type::createInstanceByName( name );
  assert(vp->getTypeId().isDerivedFrom(Gui::ViewProvider::getClassTypeId()));
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
void DlgEvaluateMeshImp::removeViewProviders()
{
  if (_viewer) {
    for ( std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it ) {
      _viewer->removeViewProvider( it->second );
      delete it->second;
    }
    _vp.clear();
  }
}

void DlgEvaluateMeshImp::on_meshNameButton_activated(int i)
{
  QString item = meshNameButton->itemText(i);

  _meshFeature = 0;
  std::vector<App::DocumentObject*> objs = _pDoc->getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::iterator it = objs.begin(); it != objs.end(); ++it ) {
    if ( item == (*it)->name.getValue() ) {
      _meshFeature = (Mesh::Feature*)(*it);
      break;
    }
  }

  if ( i== 0) {
    cleanInformation();
  } else if ( !_meshFeature ) {
    on_refreshButton_clicked();
  } else {
    analyzeOrientationButton->setEnabled(true);
    analyzeDuplicatedFacesButton->setEnabled(true);
    analyzeDuplicatedPointsButton->setEnabled(true);
    analyzeNonmanifoldsButton->setEnabled(true);
    analyzeDegeneratedButton->setEnabled(true);
    analyzeIndicesButton->setEnabled(true);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue();
    textLabel4->setText( QString("%1").arg(rMesh.CountFacets()) );
    textLabel5->setText( QString("%1").arg(rMesh.CountEdges()) );
    textLabel6->setText( QString("%1").arg(rMesh.CountPoints()) );
  }
}

void DlgEvaluateMeshImp::cleanInformation()
{
  textLabel4->setText( tr("No information") );
  textLabel5->setText( tr("No information") );
  textLabel6->setText( tr("No information") );
  checkOrientationButton->setText( tr("No information") );
  checkDuplicatedFacesButton->setText( tr("No information") );
  checkDuplicatedPointsButton->setText( tr("No information") );
  checkNonmanifoldsButton->setText( tr("No information") );
  checkDegenerationButton->setText( tr("No information") );
  checkIndicesButton->setText( tr("No information") );
  analyzeOrientationButton->setDisabled(true);
  repairOrientationButton->setDisabled(true);
  analyzeDuplicatedFacesButton->setDisabled(true);
  repairDuplicatedFacesButton->setDisabled(true);
  analyzeDuplicatedPointsButton->setDisabled(true);
  repairDuplicatedPointsButton->setDisabled(true);
  analyzeNonmanifoldsButton->setDisabled(true);
  repairNonmanifoldsButton->setDisabled(true);
  analyzeDegeneratedButton->setDisabled(true);
  repairDegeneratedButton->setDisabled(true);
  analyzeIndicesButton->setDisabled(true);
  repairIndicesButton->setDisabled(true);
}

void DlgEvaluateMeshImp::on_refreshButton_clicked()
{
  QStringList items;
  App::Document* doc = App::GetApplication().getActiveDocument();

  // switch to the active document
  if (doc && doc != _pDoc) {
    if ( _pDoc )
      //_pDoc->Detach(this)
      ;
    _pDoc = doc;
    //_pDoc->Attach(this);
    removeViewProviders();
    Gui::Document* pGui = Gui::Application::Instance->activeDocument();
    _viewer = dynamic_cast<Gui::View3DInventor*>(pGui->getActiveView())->getViewer();
  }

  if ( _pDoc ) {
    std::vector<App::DocumentObject*> objs = _pDoc->getObjectsOfType(Mesh::Feature::getClassTypeId());
    for ( std::vector<App::DocumentObject*>::iterator it = objs.begin(); it != objs.end(); ++it )
    {
      items.push_back( (*it)->name.getValue() );
    }
  }

  meshNameButton->clear();
  meshNameButton->insertItem(0, tr("No selection"));
  meshNameButton->insertItems(1, items);
  meshNameButton->setDisabled(items.empty());
  cleanInformation();
}

void DlgEvaluateMeshImp::on_checkOrientationButton_clicked()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshOrientation");
  if ( it != _vp.end() )
  {
    if ( checkOrientationButton->isChecked() )
      it->second->show();
    else
      it->second->hide();
  }
}

void DlgEvaluateMeshImp::on_analyzeOrientationButton_clicked()
{
  if ( _meshFeature )
  {
    analyzeOrientationButton->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue();
    MeshEvalOrientation eval(rMesh);
    std::vector<unsigned long> inds = eval.GetIndices();
    
    if ( inds.empty() )
    {
      checkOrientationButton->setText( tr("No flipped normals") );
      removeViewProvider( "MeshGui::ViewProviderMeshOrientation" );
    }
    else
    {
      checkOrientationButton->setText( tr("%1 flipped normals").arg(inds.size()) );
      checkOrientationButton->setChecked(true);
      repairOrientationButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshOrientation" );
    }

    qApp->restoreOverrideCursor();
    analyzeOrientationButton->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::on_repairOrientationButton_clicked()
{
  if ( _meshFeature )
  {
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    doc->openCommand("Removed duplicated faces");
    Gui::Application::Instance->runCommand(
      true, "App.activeDocument().addObject(\"Mesh::HarmonizeNormals\",\"%s\").Source = App.activeDocument().%s",
      _meshFeature->name.getValue(),_meshFeature->name.getValue());
    doc->commitCommand();
    doc->getDocument()->recompute();
    
    repairOrientationButton->setEnabled(false);
    checkOrientationButton->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshOrientation" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::on_checkNonmanifoldsButton_clicked()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshNonManifolds");
  if ( it != _vp.end() )
  {
    if ( checkDegenerationButton->isChecked() )
      it->second->show();
    else
      it->second->hide();
  }
}

void DlgEvaluateMeshImp::on_analyzeNonmanifoldsButton_clicked()
{
  if ( _meshFeature )
  {
    analyzeNonmanifoldsButton->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue();
    MeshEvalTopology eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      checkNonmanifoldsButton->setText( tr("No non-manifolds") );
    }
    else
    {
      checkNonmanifoldsButton->setText( tr("%1 non-manifolds").arg(eval.CountManifolds()) );
      checkNonmanifoldsButton->setChecked(true);
      repairNonmanifoldsButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshNonManifolds" );
    }

    qApp->restoreOverrideCursor();
    analyzeNonmanifoldsButton->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::on_repairNonmanifoldsButton_clicked()
{
  QMessageBox::warning(this, tr("Non-manifolds"), tr("Cannot remove non-manifolds"));
}

void DlgEvaluateMeshImp::on_checkIndicesButton_clicked()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshIndices");
  if ( it != _vp.end() )
  {
    if ( checkIndicesButton->isChecked() )
      it->second->show();
    else
      it->second->hide();
  }
}

void DlgEvaluateMeshImp::on_analyzeIndicesButton_clicked()
{
  if ( _meshFeature )
  {
    analyzeIndicesButton->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue();
    MeshEvalNeighbourhood nb(rMesh);
    MeshEvalRangeFacet rf(rMesh);
    MeshEvalRangePoint rp(rMesh);
    MeshEvalCorruptedFacets cf(rMesh);
    
    if ( !nb.Evaluate() ) {
      checkIndicesButton->setText( tr("Invalid neighbour indices") );
      checkIndicesButton->setChecked(true);
      repairIndicesButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
    else if ( !rf.Evaluate() ) {
      checkIndicesButton->setText( tr("Invalid face indices") );
      checkIndicesButton->setChecked(true);
      repairIndicesButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
    else if ( !rp.Evaluate() ) {
      checkIndicesButton->setText( tr("Invalid point indices") );
      checkIndicesButton->setChecked(true);
      repairIndicesButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
    else if ( !cf.Evaluate() ) {
      checkIndicesButton->setText( tr("Multiple point indices") );
      checkIndicesButton->setChecked(true);
      repairIndicesButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
    else {
      checkIndicesButton->setText( tr("No invalid indices") );
      removeViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }

    qApp->restoreOverrideCursor();
    analyzeIndicesButton->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::on_repairIndicesButton_clicked()
{
  if ( _meshFeature )
  {
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    doc->openCommand("Removed duplicated faces");
    Gui::Application::Instance->runCommand(
      true, "App.activeDocument().addObject(\"Mesh::FixIndices\",\"%s\").Source = App.activeDocument().%s",
      _meshFeature->name.getValue(),_meshFeature->name.getValue());
    doc->commitCommand();
    doc->getDocument()->recompute();
    
    repairIndicesButton->setEnabled(false);
    checkIndicesButton->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshIndices" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::on_checkDegenerationButton_clicked()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDegenerations");
  if ( it != _vp.end() )
  {
    if ( checkDegenerationButton->isChecked() )
      it->second->show();
    else
      it->second->hide();
  }
}

void DlgEvaluateMeshImp::on_analyzeDegeneratedButton_clicked()
{
  if ( _meshFeature )
  {
    analyzeDegeneratedButton->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue();
    MeshEvalDegeneratedFacets eval(rMesh);
    std::vector<unsigned long> degen = eval.GetIndices();
    
    if ( degen.empty() )
    {
      checkDegenerationButton->setText( tr("No degenerations") );
      removeViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
    }
    else
    {
      checkDegenerationButton->setText( tr("%1 degenerated faces").arg(degen.size()) );
      checkDegenerationButton->setChecked(true);
      repairDegeneratedButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
    }

    qApp->restoreOverrideCursor();
    analyzeDegeneratedButton->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::on_repairDegeneratedButton_clicked()
{
  if ( _meshFeature )
  {
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    doc->openCommand("Removed duplicated faces");
    Gui::Application::Instance->runCommand(
      true, "App.activeDocument().addObject(\"Mesh::FixDegenerations\",\"%s\").Source = App.activeDocument().%s",
      _meshFeature->name.getValue(),_meshFeature->name.getValue());
    doc->commitCommand();
    doc->getDocument()->recompute();
    
    repairDegeneratedButton->setEnabled(false);
    checkDegenerationButton->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::on_checkDuplicatedFacesButton_clicked()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDuplicatedFaces");
  if ( it != _vp.end() )
  {
    if ( checkDuplicatedFacesButton->isChecked() )
      it->second->show();
    else
      it->second->hide();
  }
}

void DlgEvaluateMeshImp::on_analyzeDuplicatedFacesButton_clicked()
{
  if ( _meshFeature )
  {
    analyzeDuplicatedFacesButton->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue();
    MeshEvalDuplicateFacets eval(rMesh);
    std::vector<unsigned long> dupl = eval.GetIndices();
    
    if ( dupl.empty() )
    {
      checkDuplicatedFacesButton->setText( tr("No duplicated faces") );
      removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
    }
    else
    {
      checkDuplicatedFacesButton->setText( tr("%1 duplicated faces").arg(dupl.size()) );
      checkDuplicatedFacesButton->setChecked(true);
      repairDuplicatedFacesButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
    }

    qApp->restoreOverrideCursor();
    analyzeDuplicatedFacesButton->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::on_repairDuplicatedFacesButton_clicked()
{
  if ( _meshFeature )
  {
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    doc->openCommand("Removed duplicated faces");
    Gui::Application::Instance->runCommand(
      true, "App.activeDocument().addObject(\"Mesh::FixDuplicatedFaces\",\"%s\").Source = App.activeDocument().%s",
      _meshFeature->name.getValue(),_meshFeature->name.getValue());
    doc->commitCommand();
    doc->getDocument()->recompute();
    
    repairDuplicatedFacesButton->setEnabled(false);
    checkDuplicatedFacesButton->setChecked(false);
    removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
    setFixedMesh();
  }
}

void DlgEvaluateMeshImp::on_checkDuplicatedPointsButton_clicked()
{
  std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDuplicatedPoints");
  if ( it != _vp.end() )
  {
    if ( checkDuplicatedPointsButton->isChecked() )
      it->second->show();
    else
      it->second->hide();
  }
}

void DlgEvaluateMeshImp::on_analyzeDuplicatedPointsButton_clicked()
{
  if ( _meshFeature )
  {
    analyzeDuplicatedPointsButton->setEnabled(false);
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue();
    MeshEvalDuplicatePoints eval(rMesh);
    
    if ( eval.Evaluate() )
    {
      checkDuplicatedPointsButton->setText( tr("No duplicated points") );
      removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedPoints" );
    }
    else
    {
      checkDuplicatedPointsButton->setText( tr("Duplicated points") );
      checkDuplicatedPointsButton->setChecked(true);
      repairDuplicatedPointsButton->setEnabled(true);
      addViewProvider( "MeshGui::ViewProviderMeshDuplicatedPoints" );
    }

    qApp->restoreOverrideCursor();
    analyzeDuplicatedPointsButton->setEnabled(true);
  }
}

void DlgEvaluateMeshImp::on_repairDuplicatedPointsButton_clicked()
{
  if ( _meshFeature )
  {
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    doc->openCommand("Removed duplicated points");
    Gui::Application::Instance->runCommand(
      true, "App.activeDocument().addObject(\"Mesh::FixDuplicatedPoints\",\"%s\").Source = App.activeDocument().%s",
      _meshFeature->name.getValue(),_meshFeature->name.getValue());
    doc->commitCommand();
    doc->getDocument()->recompute();
    
    repairDuplicatedPointsButton->setEnabled(false);
    checkDuplicatedPointsButton->setChecked(false);
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
    _instance = new DockEvaluateMeshImp(Gui::getMainWindow());
    _instance->setSizeGripEnabled(false);
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
DockEvaluateMeshImp::DockEvaluateMeshImp( QWidget* parent, Qt::WFlags fl )
  : DlgEvaluateMeshImp( parent, fl )
{
  // save the current layout of the main window and restore it when we close this window
  state = Gui::getMainWindow()->saveState(1000);
  // embed this dialog into a dockable widget container
  Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
  // use Qt macro for preparing for translation stuff (but not translating yet)
  pDockMgr->addDockWindow(QT_TRANSLATE_NOOP("Gui::DockWindow", "Evaluate Mesh"), this, Qt::RightDockWidgetArea);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DockEvaluateMeshImp::~DockEvaluateMeshImp()
{
  _instance = 0;
}

/**
 * Destroys the dock window this object is embedded into without destroying itself.
 */
void DockEvaluateMeshImp::closeEvent(QCloseEvent* e)
{
  // closes the dock window and restores the former layout of the main window
  Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
  pDockMgr->removeDockWindow(this);
  Gui::getMainWindow()->restoreState(this->state, 1000);
}

/**
 * Returns an appropriate size hint for the dock window.
 */
QSize DockEvaluateMeshImp::sizeHint () const
{
  return QSize(371, 579);
}

#include "moc_DlgEvaluateMeshImp.cpp"
