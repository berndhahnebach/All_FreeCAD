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
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoSeparator.h>
#endif

#include "RemoveComponents.h"
#include "ui_RemoveComponents.h"
#include "ViewProvider.h"
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
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>

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
  : QDialog(parent, fl)
{
    ui = new Ui_RemoveComponents;
    ui->setupUi(this);
    ui->spSelectComp->setRange(1, INT_MAX);
    ui->spSelectComp->setValue(10);
    ui->spDeselectComp->setRange(1, INT_MAX);
    ui->spDeselectComp->setValue(10);

    std::vector<App::DocumentObject*> meshes =
    Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
    this->_objects.insert(this->_objects.begin(), meshes.begin(), meshes.end());

    this->attachDocument(App::GetApplication().getActiveDocument());
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
    Gui::Document* doc = Gui::Application::Instance->getDocument(this->getDocument());
    Gui::MDIView* view = doc->getActiveView();
    if (view && view->getTypeId().isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        viewer->setEditing(true);
        viewer->startPicking(Gui::View3DInventorViewer::Rectangle);
        SoQtCursor::CustomCursor custom;
        custom.dim.setValue(CROSS_WIDTH, CROSS_HEIGHT);
        custom.hotspot.setValue(CROSS_HOT_X, CROSS_HOT_Y);
        custom.bitmap = cross_bitmap;
        custom.mask = cross_mask_bitmap;
        viewer->setComponentCursor(SoQtCursor(&custom));
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), selectGLCallback, this);
    }
}

void RemoveComponents::on_deselectRegion_clicked()
{
    // a rubberband to deselect a rectangle area of the meshes
    this->selectRegion = false;
    Gui::Document* doc = Gui::Application::Instance->getDocument(this->getDocument());
    Gui::MDIView* view = doc->getActiveView();
    if (view && view->getTypeId().isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        viewer->setEditing(true);
        viewer->startPicking(Gui::View3DInventorViewer::Rectangle);
        SoQtCursor::CustomCursor custom;
        custom.dim.setValue(CROSS_WIDTH, CROSS_HEIGHT);
        custom.hotspot.setValue(CROSS_HOT_X, CROSS_HOT_Y);
        custom.bitmap = cross_bitmap;
        custom.mask = cross_mask_bitmap;
        viewer->setComponentCursor(SoQtCursor(&custom));
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), selectGLCallback, this);
    }
}

struct RemoveComponents::iotaGen {
    unsigned long operator()() { return n++; }
    iotaGen() : n(0) {}
private:
    unsigned long n;
};

void RemoveComponents::on_selectAll_clicked()
{
    // select the complete meshes
    std::list<ViewProviderMesh*> views = getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        Mesh::Feature* mf = static_cast<Mesh::Feature*>((*it)->getObject());
        const Mesh::MeshObject* mo = mf->Mesh.getValuePtr();
        std::vector<unsigned long> faces(mo->countFacets());
        std::generate(faces.begin(), faces.end(), iotaGen());
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
            if (jt->size() <= (unsigned long)size)
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
            if (jt->size() >= (unsigned long)size)
                faces.insert(faces.end(), jt->begin(), jt->end());
        }

        (*it)->removeSelection(faces);
    }
}

void RemoveComponents::on_deleteButton_clicked()
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

void RemoveComponents::reject()
{
    on_deselectAll_clicked();
    QDialog::reject();
}

std::list<ViewProviderMesh*> RemoveComponents::getViewProviders() const
{
    std::list<ViewProviderMesh*> vps;
    for (std::list<App::DocumentObject*>::const_iterator it = _objects.begin(); it != _objects.end(); ++it) {
        App::Document* ad = (*it)->getDocument();
        Gui::Document* gd = Gui::Application::Instance->getDocument(ad);
        vps.push_back(static_cast<ViewProviderMesh*>(gd->getViewProvider(*it)));
    }

    return vps;
}

void RemoveComponents::selectGLCallback(void * ud, SoEventCallback * n)
{
    // When this callback function is invoked we must leave the edit mode
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
    view->setEditing(false);
    view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), selectGLCallback,ud);
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

    RemoveComponents* that = reinterpret_cast<RemoveComponents*>(ud);
    std::list<ViewProviderMesh*> views = that->getViewProviders();
    for (std::list<ViewProviderMesh*>::iterator it = views.begin(); it != views.end(); ++it) {
        ViewProviderMesh* vp = static_cast<ViewProviderMesh*>(*it);
        Gui::SoGLSelectAction gl;
        gl.x = x; gl.y = y;
        gl.w = w; gl.h = h;
        gl.apply(vp->getRoot());
        if (that->selectRegion)
            vp->addSelection(gl.indices);
        else
            vp->removeSelection(gl.indices);
    }

    view->render();
}

#include "moc_RemoveComponents.cpp"
