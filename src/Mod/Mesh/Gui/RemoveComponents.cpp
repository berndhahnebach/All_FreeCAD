/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <algorithm>
# include <climits>
# include <boost/bind.hpp>
# include <QPushButton>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoSeparator.h>
#endif

#include "RemoveComponents.h"
#include "ui_RemoveComponents.h"
#include "ViewProvider.h"
#include <Base/Console.h>
#include <Base/Tools.h>
#include <App/Application.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelectionAction.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Core/Algorithm.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>
#include <Mod/Mesh/App/Core/Tools.h>

using namespace MeshGui;

#define CROSS_WIDTH 16
#define CROSS_HEIGHT 16
#define CROSS_HOT_X 7
#define CROSS_HOT_Y 7

static unsigned char cross_bitmap[] = {
  0xc0, 0x03, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02,
  0x40, 0x02, 0x40, 0x02, 0x7f, 0xfe, 0x01, 0x80,
  0x01, 0x80, 0x7f, 0xfe, 0x40, 0x02, 0x40, 0x02,
  0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0xc0, 0x03
};

static unsigned char cross_mask_bitmap[] = {
 0xc0,0x03,0xc0,0x03,0xc0,0x03,0xc0,0x03,0xc0,0x03,0xc0,0x03,0xff,0xff,0xff,
 0xff,0xff,0xff,0xff,0xff,0xc0,0x03,0xc0,0x03,0xc0,0x03,0xc0,0x03,0xc0,0x03,
 0xc0,0x03
};

RemoveComponents::RemoveComponents(QWidget* parent, Qt::WFlags fl)
  : QWidget(parent, fl),  _interactiveMode(0)
{
    ui = new Ui_RemoveComponents;
    ui->setupUi(this);
    ui->spSelectComp->setRange(1, INT_MAX);
    ui->spSelectComp->setValue(10);
    ui->spDeselectComp->setRange(1, INT_MAX);
    ui->spDeselectComp->setValue(10);
}

RemoveComponents::~RemoveComponents()
{
    // no need to delete child widgets, Qt does it all for us
    delete ui;
}

void RemoveComponents::cancelObservation()
{
    close();
}

void RemoveComponents::on_selectRegion_clicked()
{
    // a rubberband to select a rectangle area of the meshes
    this->selectRegion = true;
    Gui::View3DInventorViewer* viewer = this->getViewer();
    if (viewer) {
        stopInteractiveCallback(viewer);
        startInteractiveCallback(viewer, selectGLCallback);
        // set cross cursor
        viewer->startPicking(Gui::View3DInventorViewer::Rectangle);
        SoQtCursor::CustomCursor custom;
        custom.dim.setValue(CROSS_WIDTH, CROSS_HEIGHT);
        custom.hotspot.setValue(CROSS_HOT_X, CROSS_HOT_Y);
        custom.bitmap = cross_bitmap;
        custom.mask = cross_mask_bitmap;
        viewer->setComponentCursor(SoQtCursor(&custom));
    }
}

void RemoveComponents::on_deselectRegion_clicked()
{
    // a rubberband to deselect a rectangle area of the meshes
    this->selectRegion = false;
    Gui::View3DInventorViewer* viewer = this->getViewer();
    if (viewer) {
        stopInteractiveCallback(viewer);
        startInteractiveCallback(viewer, selectGLCallback);
        // set cross cursor
        viewer->startPicking(Gui::View3DInventorViewer::Rectangle);
        SoQtCursor::CustomCursor custom;
        custom.dim.setValue(CROSS_WIDTH, CROSS_HEIGHT);
        custom.hotspot.setValue(CROSS_HOT_X, CROSS_HOT_Y);
        custom.bitmap = cross_bitmap;
        custom.mask = cross_mask_bitmap;
        viewer->setComponentCursor(SoQtCursor(&custom));
    }
}

void RemoveComponents::on_selectAll_clicked()
{
    // select the complete meshes
    std::list<ViewProviderMesh*> views = getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        Mesh::Feature* mf = static_cast<Mesh::Feature*>((*it)->getObject());
        const Mesh::MeshObject* mo = mf->Mesh.getValuePtr();
        std::vector<unsigned long> faces(mo->countFacets());
        std::generate(faces.begin(), faces.end(), Base::iotaGen<unsigned long>(0));
        (*it)->addSelection(faces);
    }
}

void RemoveComponents::on_deselectAll_clicked()
{
    // deselect all meshes
    std::list<ViewProviderMesh*> views = getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        (*it)->clearSelection();
    }
}

void RemoveComponents::on_selectComponents_clicked()
{
    // select components upto a certain size
    int size = ui->spSelectComp->value();
    std::list<ViewProviderMesh*> views = getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        Mesh::Feature* mf = static_cast<Mesh::Feature*>((*it)->getObject());
        const Mesh::MeshObject* mo = mf->Mesh.getValuePtr();

        std::vector<std::vector<unsigned long> > segm;
        MeshCore::MeshComponents comp(mo->getKernel());
        comp.SearchForComponents(MeshCore::MeshComponents::OverEdge,segm);

        std::vector<unsigned long> faces;
        for (std::vector<std::vector<unsigned long> >::iterator jt = segm.begin(); jt != segm.end(); ++jt) {
            if (jt->size() < (unsigned long)size)
                faces.insert(faces.end(), jt->begin(), jt->end());
        }

        (*it)->addSelection(faces);
    }
}

void RemoveComponents::on_deselectComponents_clicked()
{
    // deselect components from a certain size on
    int size = ui->spDeselectComp->value();
    std::list<ViewProviderMesh*> views = getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        Mesh::Feature* mf = static_cast<Mesh::Feature*>((*it)->getObject());
        const Mesh::MeshObject* mo = mf->Mesh.getValuePtr();

        std::vector<std::vector<unsigned long> > segm;
        MeshCore::MeshComponents comp(mo->getKernel());
        comp.SearchForComponents(MeshCore::MeshComponents::OverEdge,segm);

        std::vector<unsigned long> faces;
        for (std::vector<std::vector<unsigned long> >::iterator jt = segm.begin(); jt != segm.end(); ++jt) {
            if (jt->size() > (unsigned long)size)
                faces.insert(faces.end(), jt->begin(), jt->end());
        }

        (*it)->removeSelection(faces);
    }
}

void RemoveComponents::deleteSelection()
{
    // delete all selected faces
    bool selected = false;
    std::list<ViewProviderMesh*> views = getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        Mesh::Feature* mf = static_cast<Mesh::Feature*>((*it)->getObject());
        unsigned long ct = MeshCore::MeshAlgorithm(mf->Mesh.getValue().getKernel()).
            CountFacetFlag(MeshCore::MeshFacet::SELECTED);
        if (ct > 0) {
            selected = true;
            break;
        }
    }
    if (!selected) return; // nothing todo

    Gui::Document* doc = Gui::Application::Instance->getDocument(this->getDocument());
    doc->openCommand("Delete selection");
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        (*it)->deleteSelection();
    }
    doc->commitCommand();
}

void RemoveComponents::invertSelection()
{
    std::list<ViewProviderMesh*> views = getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        Mesh::Feature* mf = static_cast<Mesh::Feature*>((*it)->getObject());
        const Mesh::MeshObject* mo = mf->Mesh.getValuePtr();
        const MeshCore::MeshFacetArray& faces = mo->getKernel().GetFacets();
        unsigned long num_notsel = std::count_if(faces.begin(), faces.end(),
            std::bind2nd(MeshCore::MeshIsNotFlag<MeshCore::MeshFacet>(),
            MeshCore::MeshFacet::SELECTED));
        std::vector<unsigned long> notselect;
        notselect.reserve(num_notsel);
        MeshCore::MeshFacetArray::_TConstIterator beg = faces.begin();
        MeshCore::MeshFacetArray::_TConstIterator end = faces.end();
        for (MeshCore::MeshFacetArray::_TConstIterator jt = beg; jt != end; ++jt) {
            if (!jt->IsFlag(MeshCore::MeshFacet::SELECTED))
                notselect.push_back(jt-beg);
        }
        (*it)->setSelection(notselect);
    }
}

void RemoveComponents::on_selectTriangle_clicked()
{
    // a rubberband to select a rectangle area of the meshes
    this->selectRegion = true;
    Gui::View3DInventorViewer* viewer = this->getViewer();
    if (viewer) {
        stopInteractiveCallback(viewer);
        startInteractiveCallback(viewer, pickFaceCallback);
    }
}

void RemoveComponents::on_deselectTriangle_clicked()
{
    // a rubberband to select a rectangle area of the meshes
    this->selectRegion = false;
    Gui::View3DInventorViewer* viewer = this->getViewer();
    if (viewer) {
        stopInteractiveCallback(viewer);
        startInteractiveCallback(viewer, pickFaceCallback);
    }
}

void RemoveComponents::reject()
{
    if (_interactiveMode) {
        Gui::View3DInventorViewer* viewer = this->getViewer();
        if (viewer)
            stopInteractiveCallback(viewer);
    }
    on_deselectAll_clicked();
}

std::list<ViewProviderMesh*> RemoveComponents::getViewProviders() const
{
    std::list<ViewProviderMesh*> vps;
    for (RemoveComponents::const_iterator it = this->begin(); it != this->end(); ++it) {
        App::Document* ad = (*it)->getDocument();
        Gui::Document* gd = Gui::Application::Instance->getDocument(ad);
        vps.push_back(static_cast<ViewProviderMesh*>(gd->getViewProvider(*it)));
    }

    return vps;
}

Gui::View3DInventorViewer* RemoveComponents::getViewer() const
{
    App::Document* app = this->getDocument();
    if (!app) return 0;
    Gui::Document* doc = Gui::Application::Instance->getDocument(app);
    if (!doc) return 0;
    Gui::MDIView* view = doc->getActiveView();
    if (view && view->getTypeId().isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        return viewer;
    }

    return 0;
}

void RemoveComponents::startInteractiveCallback(Gui::View3DInventorViewer* viewer,SoEventCallbackCB *cb)
{
    if (this->_interactiveMode)
        return;
    viewer->setEditing(true);
    viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), cb, this);
    this->_interactiveMode = cb;
}

void RemoveComponents::stopInteractiveCallback(Gui::View3DInventorViewer* viewer)
{
    if (!this->_interactiveMode)
        return;
    if (viewer->isEditing()) {
        viewer->setEditing(false);
        viewer->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), this->_interactiveMode, this);
        this->_interactiveMode = 0;
    }
}

void RemoveComponents::selectGLCallback(void * ud, SoEventCallback * n)
{
    // When this callback function is invoked we must leave the edit mode
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
    RemoveComponents* that = reinterpret_cast<RemoveComponents*>(ud);
    that->stopInteractiveCallback(view);
    n->setHandled();
    std::vector<SbVec2f> clPoly = view->getPickedPolygon();
    if (clPoly.size() != 1)
        return;
    const SoEvent* ev = n->getEvent();

    // get the pixel coordinates
    SbVec2f pos = clPoly[0];
    float pX,pY; pos.getValue(pX,pY);
    const SbVec2s& sz = view->getViewportRegion().getViewportSizePixels();
    float fRatio = view->getViewportRegion().getViewportAspectRatio();
    if (fRatio > 1.0f) {
        pX = (pX - 0.5f) / fRatio + 0.5f;
        pos.setValue(pX,pY);
    }
    else if (fRatio < 1.0f) {
        pY = (pY - 0.5f) * fRatio + 0.5f;
        pos.setValue(pX,pY);
    }

    short x1 = (short)(pX * sz[0] + 0.5f);
    short y1 = (short)(pY * sz[1] + 0.5f);
    SbVec2s loc = ev->getPosition();
    short x2 = loc[0];
    short y2 = loc[1];

    short x = (x1+x2)/2;
    short y = (y1+y2)/2;
    short w = (x2-x1);
    short h = (y2-y1);
    if (w<0) w = -w;
    if (h<0) h = -h;

    SbVec3f pnt, dir;
    view->getNearPlane(pnt, dir);
    Base::Vector3f point (pnt[0],pnt[1],pnt[2]);
    Base::Vector3f normal(dir[0],dir[1],dir[2]);

    std::list<ViewProviderMesh*> views = that->getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        ViewProviderMesh* vp = static_cast<ViewProviderMesh*>(*it);
        Gui::SoGLSelectAction gl;
        gl.x = x; gl.y = y;
        gl.w = w; gl.h = h;
        gl.apply(vp->getRoot());

        std::vector<unsigned long> faces;
        const Mesh::MeshObject& mesh = static_cast<Mesh::Feature*>((*it)->getObject())->Mesh.getValue();
        const MeshCore::MeshKernel& kernel = mesh.getKernel();

        // if the selected are is split into several segments then identify that
        // with the nearest triangle to the user
        if (that->ui->frontTriangles->isChecked()) {
            // get the nearest facet to the user (using front clipping plane)
            MeshCore::MeshNearestIndexToPlane<MeshCore::MeshFaceIterator> p =
                std::for_each(gl.indices.begin(), gl.indices.end(),
                    MeshCore::MeshNearestIndexToPlane<MeshCore::MeshFaceIterator>
                    (kernel, point, normal));

            // succeeded
            if (p.nearest_index != ULONG_MAX) {
                // set VISIT flag to all outer facets
                MeshCore::MeshAlgorithm alg(kernel);
                alg.SetFacetFlag(MeshCore::MeshFacet::VISIT);
                alg.ResetFacetsFlag(gl.indices, MeshCore::MeshFacet::VISIT);

                MeshCore::MeshTopFacetVisitor visitor(faces);
                kernel.VisitNeighbourFacets(visitor, p.nearest_index);
                // append also the start facet
                faces.push_back(p.nearest_index);
            }
        }
        else {
            // simply get all triangles under the rubberband
            faces.insert(faces.end(), gl.indices.begin(), gl.indices.end());
        }

        // if set filter out all triangles which do not point into user direction
        if (that->ui->screenTriangles->isChecked()) {
            std::vector<unsigned long> screen;
            screen.reserve(faces.size());
            MeshCore::MeshFacetIterator it_f(kernel);
            for (std::vector<unsigned long>::iterator it = faces.begin(); it != faces.end(); ++it) {
                it_f.Set(*it);
                if (it_f->GetNormal() * normal > 0.0f) {
                    screen.push_back(*it);
                }
            }

            faces.swap(screen);
        }

        if (that->selectRegion)
            vp->addSelection(faces);
        else
            vp->removeSelection(faces);
    }

    view->render();
}

void RemoveComponents::pickFaceCallback(void * ud, SoEventCallback * n)
{
    // handle only mouse button events
    if (n->getEvent()->isOfType(SoMouseButtonEvent::getClassTypeId())) {
        const SoMouseButtonEvent * mbe = static_cast<const SoMouseButtonEvent*>(n->getEvent());
        Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

        // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
        n->getAction()->setHandled();
        if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
            const SoPickedPoint * point = n->getPickedPoint();
            if (point == NULL) {
                Base::Console().Message("No facet picked.\n");
                return;
            }

            n->setHandled();

            // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
            // really from the mesh we render and not from any other geometry
            Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
            if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMesh::getClassTypeId()))
                return;
            ViewProviderMesh* that = static_cast<ViewProviderMesh*>(vp);
            RemoveComponents* dlg = reinterpret_cast<RemoveComponents*>(ud);
            std::list<ViewProviderMesh*> views = dlg->getViewProviders();
            if (std::find(views.begin(), views.end(), that) == views.end())
                return;
            const SoDetail* detail = point->getDetail(/*that->getShapeNode()*/);
            if (detail && detail->getTypeId() == SoFaceDetail::getClassTypeId()) {
                // get the boundary to the picked facet
                unsigned long uFacet = static_cast<const SoFaceDetail*>(detail)->getFaceIndex();
                std::vector<unsigned long> faces; faces.push_back(uFacet);
                if (dlg->selectRegion) {
                    if (dlg->ui->cbSelectComp->isChecked())
                        that->selectComponent(uFacet);
                    else
                        that->selectFacet(uFacet);
                }
                else {
                    if (dlg->ui->cbDeselectComp->isChecked())
                        that->deselectComponent(uFacet);
                    else
                        that->removeSelection(faces);
                }
            }
        }
    }
}

// ---------------------------------------

RemoveComponentsDialog::RemoveComponentsDialog(const std::vector<Mesh::Feature*>& mesh,
                                               QWidget* parent, Qt::WFlags fl)
  : QDialog(parent, fl)
{
    widget = new RemoveComponents(this);
    this->setWindowTitle(widget->windowTitle());

    QVBoxLayout* hboxLayout = new QVBoxLayout(this);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Ok);
    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(TaskRemoveComponents::tr("Delete"));
    buttonBox->addButton(TaskRemoveComponents::tr("Invert"),
        QDialogButtonBox::ActionRole);
    
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(clicked(QAbstractButton*)));

    hboxLayout->addWidget(widget);
    hboxLayout->addWidget(buttonBox);

    //std::vector<App::DocumentObject*> meshes =
    //Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());

    std::for_each(mesh.begin(), mesh.end(), 
        boost::bind(&RemoveComponents::addToObservation, widget, _1));
    widget->attachDocument(App::GetApplication().getActiveDocument());
}

RemoveComponentsDialog::~RemoveComponentsDialog()
{
}

void RemoveComponentsDialog::reject()
{
    widget->reject();
    QDialog::reject();
}

void RemoveComponentsDialog::clicked(QAbstractButton* btn)
{
    QDialogButtonBox* buttonBox = qobject_cast<QDialogButtonBox*>(sender());
    QDialogButtonBox::StandardButton id = buttonBox->standardButton(btn);
    if (id == QDialogButtonBox::Ok) {
        widget->deleteSelection();
    }
    else if (id == QDialogButtonBox::Close) {
        this->reject();
    }
    else if (id == QDialogButtonBox::NoButton) {
        widget->invertSelection();
    }
}

// ---------------------------------------

TaskRemoveComponents::TaskRemoveComponents(const std::vector<Mesh::Feature*>& mesh)
{
    widget = new RemoveComponents();
    taskbox = new Gui::TaskView::TaskBox(
        QPixmap(), widget->windowTitle(), false, 0);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);

    std::for_each(mesh.begin(), mesh.end(), 
        boost::bind(&RemoveComponents::addToObservation, widget, _1));
    widget->attachDocument(App::GetApplication().getActiveDocument());
}

TaskRemoveComponents::~TaskRemoveComponents()
{
    // automatically deleted in the sub-class
}

void TaskRemoveComponents::modifyStandardButtons(QDialogButtonBox* box)
{
    QPushButton* btn = box->button(QDialogButtonBox::Ok);
    btn->setText(tr("Delete"));
    box->addButton(tr("Invert"), QDialogButtonBox::ActionRole);
}

bool TaskRemoveComponents::accept()
{
    return false;
}

void TaskRemoveComponents::clicked(int id)
{
    if (id == QDialogButtonBox::Ok) {
        widget->deleteSelection();
    }
    else if (id == QDialogButtonBox::Close) {
        widget->reject();
    }
    else if (id == QDialogButtonBox::NoButton) {
        widget->invertSelection();
    }
}

#include "moc_RemoveComponents.cpp"
