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

void DlgEvaluateMeshImp::slotCreatedObject(App::DocumentObject& Obj)
{
    // add new mesh object to the list
    if (Obj.getTypeId().isDerivedFrom(Mesh::Feature::getClassTypeId())) {
        QString label = QString::fromUtf8(Obj.Label.getValue());
        QString name = QString::fromAscii(Obj.getNameInDocument());
        meshNameButton->addItem(label, name);
    }
}

void DlgEvaluateMeshImp::slotDeletedObject(App::DocumentObject& Obj)
{
    // remove mesh objects from the list
    if (Obj.getTypeId().isDerivedFrom(Mesh::Feature::getClassTypeId())) {
        int index = meshNameButton->findData(QString::fromAscii(Obj.getNameInDocument()));
        if (index > 0) {
            meshNameButton->removeItem(index);
            meshNameButton->setDisabled(meshNameButton->count() < 2);
        }
    }

    // is it the current mesh object then clear everything
    if (&Obj == _meshFeature) {
        removeViewProviders();
        _meshFeature = 0;
        meshNameButton->setCurrentIndex(0);
        cleanInformation();
    }
}

void DlgEvaluateMeshImp::slotChangedObject(App::DocumentObject& Obj, App::Property& Prop)
{
    // if the current mesh object was modified update everything
    if (&Obj == _meshFeature && Prop.getTypeId() == Mesh::PropertyMeshKernel::getClassTypeId()) {
        removeViewProviders();
        cleanInformation();
        showInformation();
    }
    else if (Obj.getTypeId().isDerivedFrom(Mesh::Feature::getClassTypeId())) {
        // if the label has changed update the entry in the list
        if (Prop.getTypeId() == App::PropertyString::getClassTypeId() &&
            QString::compare(QLatin1String(Prop.getName()), QLatin1String("Label")) == 0) {
                QString label = QString::fromUtf8(Obj.Label.getValue());
                QString name = QString::fromAscii(Obj.getNameInDocument());
                int index = meshNameButton->findData(name);
                meshNameButton->setItemText(index, label);
        }
    }
}

void DlgEvaluateMeshImp::slotCreatedDocument(App::Document& Doc)
{
}

void DlgEvaluateMeshImp::slotDeletedDocument(App::Document& Doc)
{
    if (&Doc == getDocument()) {
        // the view is already destroyed
        for (std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it) {
            delete it->second;
        }

        _vp.clear();    

        // try to attach to the active document
        this->detachDocument();
        this->_view = 0;
        on_refreshButton_clicked();
    }
}

/**
 *  Constructs a DlgEvaluateMeshImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgEvaluateMeshImp::DlgEvaluateMeshImp(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent, fl), _meshFeature(0), _view(0)
{
    this->setupUi(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);
    line_3->setFrameShape(QFrame::HLine);
    line_3->setFrameShadow(QFrame::Sunken);
    line_4->setFrameShape(QFrame::HLine);
    line_4->setFrameShadow(QFrame::Sunken);
    line_5->setFrameShape(QFrame::HLine);
    line_5->setFrameShadow(QFrame::Sunken);
    line_6->setFrameShape(QFrame::HLine);
    line_6->setFrameShadow(QFrame::Sunken);
    line_7->setFrameShape(QFrame::HLine);
    line_7->setFrameShadow(QFrame::Sunken);
    line_8->setFrameShape(QFrame::HLine);
    line_8->setFrameShadow(QFrame::Sunken);

    connect(buttonHelp,  SIGNAL (clicked()),
            Gui::getMainWindow(), SLOT (whatsThis()));

    // try to attach to the active document
    this->on_refreshButton_clicked();
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgEvaluateMeshImp::~DlgEvaluateMeshImp()
{
    // no need to delete child widgets, Qt does it all for us
    for (std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it) {
        if (_view)
            _view->getViewer()->removeViewProvider(it->second);
        delete it->second;
    }

    _vp.clear();
}

void DlgEvaluateMeshImp::setMesh(Mesh::Feature* m)
{
    App::Document& doc = m->getDocument();
    if (&doc != getDocument())
        attachDocument(&doc);
  
    refreshList();

    int ct = meshNameButton->count();
    QString objName = QString::fromAscii(m->getNameInDocument());
    for (int i=1; i<ct; i++) {
        if (meshNameButton->itemData(i).toString() == objName) {
            meshNameButton->setCurrentIndex(i);
            on_meshNameButton_activated(i);
            break;
        }
    }
}

void DlgEvaluateMeshImp::addViewProvider(const char* name)
{
    removeViewProvider(name);

    if (_view) {
        ViewProviderMeshDefects* vp = static_cast<ViewProviderMeshDefects*>(Base::Type::createInstanceByName(name));
        assert(vp->getTypeId().isDerivedFrom(Gui::ViewProvider::getClassTypeId()));
        vp->attach(_meshFeature);
        _view->getViewer()->addViewProvider( vp );
        vp->showDefects();
        _vp[name] = vp;
    }
}

void DlgEvaluateMeshImp::removeViewProvider(const char* name)
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find( name );
    if (it != _vp.end()) {
        if (_view)
            _view->getViewer()->removeViewProvider( it->second );
        delete it->second;
        _vp.erase(it);
    }
}

void DlgEvaluateMeshImp::removeViewProviders()
{
    for (std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.begin(); it != _vp.end(); ++it) {
        if (_view)
            _view->getViewer()->removeViewProvider( it->second );
        delete it->second;
    }
    _vp.clear();
}

void DlgEvaluateMeshImp::on_meshNameButton_activated(int i)
{
    QString item = meshNameButton->itemData(i).toString();

    _meshFeature = 0;
    std::vector<App::DocumentObject*> objs = getDocument()->getObjectsOfType(Mesh::Feature::getClassTypeId());
    for (std::vector<App::DocumentObject*>::iterator it = objs.begin(); it != objs.end(); ++it) {
        if (item == QLatin1String((*it)->getNameInDocument())) {
            _meshFeature = (Mesh::Feature*)(*it);
            break;
        }
    }

    if (i== 0) {
        cleanInformation();
    }
    else {
        showInformation();
    }
}

void DlgEvaluateMeshImp::refreshList()
{
    QList<QPair<QString, QString> > items;
    if (this->getDocument()) {
        std::vector<App::DocumentObject*> objs = this->getDocument()->getObjectsOfType(Mesh::Feature::getClassTypeId());
        for (std::vector<App::DocumentObject*>::iterator it = objs.begin(); it != objs.end(); ++it) {
            items.push_back(qMakePair(QString::fromUtf8((*it)->Label.getValue()),
                                      QString::fromAscii((*it)->getNameInDocument())));
        }
    }

    meshNameButton->clear();
    meshNameButton->addItem(tr("No selection"));
    for (QList<QPair<QString, QString> >::iterator it = items.begin(); it != items.end(); ++it)
        meshNameButton->addItem(it->first, it->second);
    meshNameButton->setDisabled(items.empty());
    cleanInformation();
}

void DlgEvaluateMeshImp::showInformation()
{
    analyzeOrientationButton->setEnabled(true);
    analyzeDuplicatedFacesButton->setEnabled(true);
    analyzeDuplicatedPointsButton->setEnabled(true);
    analyzeNonmanifoldsButton->setEnabled(true);
    analyzeDegeneratedButton->setEnabled(true);
    analyzeIndicesButton->setEnabled(true);
    analyzeSelfIntersectionButton->setEnabled(true);
    analyzeAllTogether->setEnabled(true);

    const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
    textLabel4->setText(QString::fromAscii("%1").arg(rMesh.CountFacets()));
    textLabel5->setText(QString::fromAscii("%1").arg(rMesh.CountEdges()));
    textLabel6->setText(QString::fromAscii("%1").arg(rMesh.CountPoints()));
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
    checkSelfIntersectionButton->setText( tr("No information") );
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
    analyzeSelfIntersectionButton->setDisabled(true);
    repairSelfIntersectionButton->setDisabled(true);
    analyzeAllTogether->setDisabled(true);
    repairAllTogether->setDisabled(true);
}

void DlgEvaluateMeshImp::on_refreshButton_clicked()
{
    // Connect to application and active document
    Gui::Document* gui = Gui::Application::Instance->activeDocument();
    App::Document* doc = gui->getDocument();

    // switch to the active document
    if (doc && doc != this->getDocument()) {
        attachDocument(doc);
        removeViewProviders();
        _view = dynamic_cast<Gui::View3DInventor*>(gui->getActiveView());
    }
    refreshList();
}

void DlgEvaluateMeshImp::on_checkOrientationButton_clicked()
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshOrientation");
    if (it != _vp.end()) {
        if (checkOrientationButton->isChecked())
            it->second->show();
        else
            it->second->hide();
    }
}

void DlgEvaluateMeshImp::on_analyzeOrientationButton_clicked()
{
    if (_meshFeature) {
        analyzeOrientationButton->setEnabled(false);
        qApp->processEvents();
        qApp->setOverrideCursor(Qt::WaitCursor);

        const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
        MeshEvalOrientation eval(rMesh);
        std::vector<unsigned long> inds = eval.GetIndices();
    
        if (inds.empty()) {
            checkOrientationButton->setText( tr("No flipped normals") );
            checkOrientationButton->setChecked(false);
            repairOrientationButton->setEnabled(false);
            removeViewProvider( "MeshGui::ViewProviderMeshOrientation" );
        }
        else {
            checkOrientationButton->setText( tr("%1 flipped normals").arg(inds.size()) );
            checkOrientationButton->setChecked(true);
            repairOrientationButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
            addViewProvider( "MeshGui::ViewProviderMeshOrientation" );
        }

        qApp->restoreOverrideCursor();
        analyzeOrientationButton->setEnabled(true);
    }
}

void DlgEvaluateMeshImp::on_repairOrientationButton_clicked()
{
    if (_meshFeature) {
        const char* docName = App::GetApplication().getDocumentName(&_meshFeature->getDocument());
        const char* objName = _meshFeature->getNameInDocument();
        Gui::Document* doc = Gui::Application::Instance->getDocument(docName);
        doc->openCommand("Harmonize normals");
        try {
            Gui::Application::Instance->runCommand(
                true, "App.getDocument(\"%s\").getObject(\"%s\").harmonizeNormals()\n"
                    , docName, objName);
        }
        catch (const Base::Exception& e) {
            QMessageBox::warning(this, tr("Orientation"), QString::fromLatin1(e.what()));
        }

        doc->commitCommand();
        doc->getDocument()->recompute();
    
        repairOrientationButton->setEnabled(false);
        checkOrientationButton->setChecked(false);
        removeViewProvider( "MeshGui::ViewProviderMeshOrientation" );
    }
}

void DlgEvaluateMeshImp::on_checkNonmanifoldsButton_clicked()
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshNonManifolds");
    if (it != _vp.end()) {
        if (checkNonmanifoldsButton->isChecked())
            it->second->show();
        else
            it->second->hide();
    }
}

void DlgEvaluateMeshImp::on_analyzeNonmanifoldsButton_clicked()
{
    if (_meshFeature) {
        analyzeNonmanifoldsButton->setEnabled(false);
        qApp->processEvents();
        qApp->setOverrideCursor(Qt::WaitCursor);

        const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
        MeshEvalTopology eval(rMesh);
    
        if (eval.Evaluate()) {
          checkNonmanifoldsButton->setText( tr("No non-manifolds") );
          checkNonmanifoldsButton->setChecked(false);
          repairNonmanifoldsButton->setEnabled(false);
          removeViewProvider( "MeshGui::ViewProviderMeshNonManifolds" );
        }
        else {
          checkNonmanifoldsButton->setText( tr("%1 non-manifolds").arg(eval.CountManifolds()) );
          checkNonmanifoldsButton->setChecked(true);
          repairNonmanifoldsButton->setEnabled(true);
          repairAllTogether->setEnabled(true);
          addViewProvider( "MeshGui::ViewProviderMeshNonManifolds" );
        }

        qApp->restoreOverrideCursor();
        analyzeNonmanifoldsButton->setEnabled(true);
    }
}

void DlgEvaluateMeshImp::on_repairNonmanifoldsButton_clicked()
{
    if (_meshFeature) {
        const char* docName = App::GetApplication().getDocumentName(&_meshFeature->getDocument());
        const char* objName = _meshFeature->getNameInDocument();
        Gui::Document* doc = Gui::Application::Instance->getDocument(docName);
        doc->openCommand("Remove non-manifolds");
        try {
            Gui::Application::Instance->runCommand(
                true, "App.getDocument(\"%s\").getObject(\"%s\").removeNonManifolds()\n"
                    , docName, objName);
        } 
        catch (const Base::Exception& e) {
            QMessageBox::warning(this, tr("Non-manifolds"), QString::fromLatin1(e.what()));
        }
        catch (...) {
            QMessageBox::warning(this, tr("Non-manifolds"), tr("Cannot remove non-manifolds"));
        }

        doc->commitCommand();
        doc->getDocument()->recompute();
    
        repairNonmanifoldsButton->setEnabled(false);
        checkNonmanifoldsButton->setChecked(false);
        removeViewProvider( "MeshGui::ViewProviderMeshNonManifolds" );
    }
}

void DlgEvaluateMeshImp::on_checkIndicesButton_clicked()
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshIndices");
    if (it != _vp.end()) {
        if (checkIndicesButton->isChecked())
            it->second->show();
        else
            it->second->hide();
    }
}

void DlgEvaluateMeshImp::on_analyzeIndicesButton_clicked()
{
    if (_meshFeature) {
        analyzeIndicesButton->setEnabled(false);
        qApp->processEvents();
        qApp->setOverrideCursor(Qt::WaitCursor);

        const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
        MeshEvalRangeFacet rf(rMesh);
        MeshEvalRangePoint rp(rMesh);
        MeshEvalCorruptedFacets cf(rMesh);
        MeshEvalNeighbourhood nb(rMesh);
        
        if (!rf.Evaluate()) {
            checkIndicesButton->setText( tr("Invalid face indices") );
            checkIndicesButton->setChecked(true);
            repairIndicesButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
           addViewProvider( "MeshGui::ViewProviderMeshIndices" );
        }
        else if (!rp.Evaluate()) {
            checkIndicesButton->setText( tr("Invalid point indices") );
            checkIndicesButton->setChecked(true);
            repairIndicesButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
            addViewProvider( "MeshGui::ViewProviderMeshIndices" );
        }
        else if (!cf.Evaluate()) {
            checkIndicesButton->setText( tr("Multiple point indices") );
            checkIndicesButton->setChecked(true);
            repairIndicesButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
            addViewProvider( "MeshGui::ViewProviderMeshIndices" );
        }
        else if (!nb.Evaluate()) {
            checkIndicesButton->setText( tr("Invalid neighbour indices") );
            checkIndicesButton->setChecked(true);
            repairIndicesButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
            addViewProvider( "MeshGui::ViewProviderMeshIndices" );
        }
        else {
            checkIndicesButton->setText( tr("No invalid indices") );
            checkIndicesButton->setChecked(false);
            repairIndicesButton->setEnabled(false);
            removeViewProvider( "MeshGui::ViewProviderMeshIndices" );
        }

        qApp->restoreOverrideCursor();
        analyzeIndicesButton->setEnabled(true);
    }
}

void DlgEvaluateMeshImp::on_repairIndicesButton_clicked()
{
    if (_meshFeature) {
        const char* docName = App::GetApplication().getDocumentName(&_meshFeature->getDocument());
        const char* objName = _meshFeature->getNameInDocument();
        Gui::Document* doc = Gui::Application::Instance->getDocument(docName);
        doc->openCommand("Fix indices");
        try {
            Gui::Application::Instance->runCommand(
                true, "App.getDocument(\"%s\").getObject(\"%s\").fixIndices()\n"
                    , docName, objName);
        }
        catch (const Base::Exception& e) {
            QMessageBox::warning(this, tr("Indices"), QString::fromLatin1(e.what()));
        }

        doc->commitCommand();
        doc->getDocument()->recompute();
    
        repairIndicesButton->setEnabled(false);
        checkIndicesButton->setChecked(false);
        removeViewProvider( "MeshGui::ViewProviderMeshIndices" );
    }
}

void DlgEvaluateMeshImp::on_checkDegenerationButton_clicked()
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDegenerations");
    if (it != _vp.end()) {
        if (checkDegenerationButton->isChecked())
            it->second->show();
        else
            it->second->hide();
    }
}

void DlgEvaluateMeshImp::on_analyzeDegeneratedButton_clicked()
{
    if (_meshFeature) {
        analyzeDegeneratedButton->setEnabled(false);
        qApp->processEvents();
        qApp->setOverrideCursor(Qt::WaitCursor);

        const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
        MeshEvalDegeneratedFacets eval(rMesh);
        std::vector<unsigned long> degen = eval.GetIndices();
        
        if (degen.empty()) {
            checkDegenerationButton->setText( tr("No degenerations") );
            checkDegenerationButton->setChecked(false);
            repairDegeneratedButton->setEnabled(false);
            removeViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
        }
        else {
            checkDegenerationButton->setText( tr("%1 degenerated faces").arg(degen.size()) );
            checkDegenerationButton->setChecked(true);
            repairDegeneratedButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
            addViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
        }

        qApp->restoreOverrideCursor();
        analyzeDegeneratedButton->setEnabled(true);
    }
}

void DlgEvaluateMeshImp::on_repairDegeneratedButton_clicked()
{
    if (_meshFeature) {
        const char* docName = App::GetApplication().getDocumentName(&_meshFeature->getDocument());
        const char* objName = _meshFeature->getNameInDocument();
        Gui::Document* doc = Gui::Application::Instance->getDocument(docName);
        doc->openCommand("Remove degenerated faces");
        try {
            Gui::Application::Instance->runCommand(
                true, "App.getDocument(\"%s\").getObject(\"%s\").fixDegenerations()\n"
                    , docName, objName);
        }
        catch (const Base::Exception& e) {
            QMessageBox::warning(this, tr("Degenerations"), QString::fromLatin1(e.what()));
        }

        doc->commitCommand();
        doc->getDocument()->recompute();
    
        repairDegeneratedButton->setEnabled(false);
        checkDegenerationButton->setChecked(false);
        removeViewProvider( "MeshGui::ViewProviderMeshDegenerations" );
    }
}

void DlgEvaluateMeshImp::on_checkDuplicatedFacesButton_clicked()
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDuplicatedFaces");
    if (it != _vp.end()) {
        if (checkDuplicatedFacesButton->isChecked())
            it->second->show();
        else
            it->second->hide();
    }
}

void DlgEvaluateMeshImp::on_analyzeDuplicatedFacesButton_clicked()
{
    if (_meshFeature) {
        analyzeDuplicatedFacesButton->setEnabled(false);
        qApp->processEvents();
        qApp->setOverrideCursor(Qt::WaitCursor);

        const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
        MeshEvalDuplicateFacets eval(rMesh);
        std::vector<unsigned long> dupl = eval.GetIndices();
    
        if (dupl.empty()) {
            checkDuplicatedFacesButton->setText( tr("No duplicated faces") );
            checkDuplicatedFacesButton->setChecked(false);
            repairDuplicatedFacesButton->setEnabled(false);
            removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
        }
        else {
            checkDuplicatedFacesButton->setText( tr("%1 duplicated faces").arg(dupl.size()) );
            checkDuplicatedFacesButton->setChecked(true);
            repairDuplicatedFacesButton->setEnabled(true);
            repairAllTogether->setEnabled(true);

            addViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
        }

        qApp->restoreOverrideCursor();
        analyzeDuplicatedFacesButton->setEnabled(true);
    }
}

void DlgEvaluateMeshImp::on_repairDuplicatedFacesButton_clicked()
{
    if (_meshFeature) {
        const char* docName = App::GetApplication().getDocumentName(&_meshFeature->getDocument());
        const char* objName = _meshFeature->getNameInDocument();
        Gui::Document* doc = Gui::Application::Instance->getDocument(docName);
        doc->openCommand("Remove duplicated faces");
        try {
            Gui::Application::Instance->runCommand(
                true, "App.getDocument(\"%s\").getObject(\"%s\").removeDuplicatedFacets()\n"
                    , docName, objName);
        }
        catch (const Base::Exception& e) {
            QMessageBox::warning(this, tr("Duplicated faces"), QString::fromLatin1(e.what()));
        }

        doc->commitCommand();
        doc->getDocument()->recompute();
    
        repairDuplicatedFacesButton->setEnabled(false);
        checkDuplicatedFacesButton->setChecked(false);
        removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedFaces" );
    }
}

void DlgEvaluateMeshImp::on_checkDuplicatedPointsButton_clicked()
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshDuplicatedPoints");
    if (it != _vp.end()) {
        if ( checkDuplicatedPointsButton->isChecked() )
            it->second->show();
        else
            it->second->hide();
    }
}

void DlgEvaluateMeshImp::on_analyzeDuplicatedPointsButton_clicked()
{
    if (_meshFeature) {
        analyzeDuplicatedPointsButton->setEnabled(false);
        qApp->processEvents();
        qApp->setOverrideCursor(Qt::WaitCursor);

        const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
        MeshEvalDuplicatePoints eval(rMesh);
    
        if (eval.Evaluate()) {
            checkDuplicatedPointsButton->setText( tr("No duplicated points") );
            checkDuplicatedPointsButton->setChecked(false);
            repairDuplicatedPointsButton->setEnabled(false);
            removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedPoints" );
        }
        else {
            checkDuplicatedPointsButton->setText( tr("Duplicated points") );
            checkDuplicatedPointsButton->setChecked(true);
            repairDuplicatedPointsButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
            addViewProvider( "MeshGui::ViewProviderMeshDuplicatedPoints" );
        }

        qApp->restoreOverrideCursor();
        analyzeDuplicatedPointsButton->setEnabled(true);
    }
}

void DlgEvaluateMeshImp::on_repairDuplicatedPointsButton_clicked()
{
    if (_meshFeature) {
        const char* docName = App::GetApplication().getDocumentName(&_meshFeature->getDocument());
        const char* objName = _meshFeature->getNameInDocument();
        Gui::Document* doc = Gui::Application::Instance->getDocument(docName);
        doc->openCommand("Remove duplicated points");
        try {
            Gui::Application::Instance->runCommand(
                true, "App.getDocument(\"%s\").getObject(\"%s\").removeDuplicatedPoints()\n"
                    , docName, objName);
        }
        catch (const Base::Exception& e) {
            QMessageBox::warning(this, tr("Duplicated points"), QString::fromLatin1(e.what()));
        }

        doc->commitCommand();
        doc->getDocument()->recompute();
    
        repairDuplicatedPointsButton->setEnabled(false);
        checkDuplicatedPointsButton->setChecked(false);
        removeViewProvider( "MeshGui::ViewProviderMeshDuplicatedPoints" );
    }
}

void DlgEvaluateMeshImp::on_checkSelfIntersectionButton_clicked()
{
    std::map<std::string, ViewProviderMeshDefects*>::iterator it = _vp.find("MeshGui::ViewProviderMeshSelfIntersections");
    if (it != _vp.end()) {
        if ( checkSelfIntersectionButton->isChecked() )
            it->second->show();
        else
            it->second->hide();
    }
}

void DlgEvaluateMeshImp::on_analyzeSelfIntersectionButton_clicked()
{
    if (_meshFeature) {
        analyzeSelfIntersectionButton->setEnabled(false);
        qApp->processEvents();
        qApp->setOverrideCursor(Qt::WaitCursor);

        const MeshKernel& rMesh = _meshFeature->Mesh.getValue().getKernel();
        MeshEvalSelfIntersection eval(rMesh);
    
        if (eval.Evaluate()) {
            checkSelfIntersectionButton->setText( tr("No self-intersections") );
            checkSelfIntersectionButton->setChecked(false);
            repairSelfIntersectionButton->setEnabled(false);
            removeViewProvider( "MeshGui::ViewProviderMeshSelfIntersections" );
        }
        else {
            checkSelfIntersectionButton->setText( tr("Self-intersections") );
            checkSelfIntersectionButton->setChecked(true);
            repairSelfIntersectionButton->setEnabled(true);
            repairAllTogether->setEnabled(true);
            addViewProvider( "MeshGui::ViewProviderMeshSelfIntersections" );
        }

        qApp->restoreOverrideCursor();
        analyzeSelfIntersectionButton->setEnabled(true);
    }
}

void DlgEvaluateMeshImp::on_repairSelfIntersectionButton_clicked()
{
    if (_meshFeature) {
        const char* docName = App::GetApplication().getDocumentName(&_meshFeature->getDocument());
        const char* objName = _meshFeature->getNameInDocument();
        Gui::Document* doc = Gui::Application::Instance->getDocument(docName);
        doc->openCommand("Fix self-intersections");
        try {
            Gui::Application::Instance->runCommand(
                true, "App.getDocument(\"%s\").getObject(\"%s\").fixSelfIntersections()\n"
                    , docName, objName);
        }
        catch (const Base::Exception& e) {
            QMessageBox::warning(this, tr("Self-intersections"), QString::fromLatin1(e.what()));
        }

        doc->commitCommand();
        doc->getDocument()->recompute();
    
        repairSelfIntersectionButton->setEnabled(false);
        checkSelfIntersectionButton->setChecked(false);
        removeViewProvider( "MeshGui::ViewProviderMeshSelfIntersections" );
    }
}

void DlgEvaluateMeshImp::on_analyzeAllTogether_clicked()
{
    on_analyzeOrientationButton_clicked();
    on_analyzeDuplicatedFacesButton_clicked();
    on_analyzeDuplicatedPointsButton_clicked();
    on_analyzeNonmanifoldsButton_clicked();
    on_analyzeDegeneratedButton_clicked();
    on_analyzeIndicesButton_clicked();
    on_analyzeSelfIntersectionButton_clicked();
}

void DlgEvaluateMeshImp::on_repairAllTogether_clicked()
{
    if (repairSelfIntersectionButton->isEnabled())
        on_repairSelfIntersectionButton_clicked();
    if (repairDuplicatedPointsButton->isEnabled())
        on_repairDuplicatedPointsButton_clicked();
    if (repairDuplicatedFacesButton->isEnabled())
        on_repairDuplicatedFacesButton_clicked();
    if (repairDegeneratedButton->isEnabled())
        on_repairDegeneratedButton_clicked();
    if (repairIndicesButton->isEnabled())
        on_repairIndicesButton_clicked();
    if (repairNonmanifoldsButton->isEnabled())
        on_repairNonmanifoldsButton_clicked();
    if (repairOrientationButton->isEnabled())
        on_repairOrientationButton_clicked();
}

// -------------------------------------------------------------

DockEvaluateMeshImp* DockEvaluateMeshImp::_instance=0;

DockEvaluateMeshImp* DockEvaluateMeshImp::instance()
{
    // not initialized?
    if(!_instance) {
        _instance = new DockEvaluateMeshImp(Gui::getMainWindow());
        _instance->setSizeGripEnabled(false);
    }

    return _instance;
}

void DockEvaluateMeshImp::destruct ()
{
    if (_instance != 0) {
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
    // embed this dialog into a dockable widget container
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    // use Qt macro for preparing for translation stuff (but not translating yet)
    QDockWidget* dw = pDockMgr->addDockWindow(QT_TR_NOOP("Evaluate Mesh"), this, Qt::RightDockWidgetArea);
    //dw->setAttribute(Qt::WA_DeleteOnClose);
    dw->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    dw->show();
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
    // closes the dock window
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    pDockMgr->removeDockWindow(this);
}

/**
 * Returns an appropriate size hint for the dock window.
 */
QSize DockEvaluateMeshImp::sizeHint () const
{
    return QSize(371, 579);
}

#include "moc_DlgEvaluateMeshImp.cpp"

