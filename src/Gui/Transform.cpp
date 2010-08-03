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
#include <QDialogButtonBox>
#include <QSignalMapper>

#include "Transform.h"
#include "Selection.h"
#include "ViewProvider.h"
#include "ui_Placement.h"
#include "Application.h"
#include "Document.h"
#include <App/PropertyGeo.h>
#include <App/DocumentObject.h>

using namespace Gui::Dialog;

namespace Gui { namespace Dialog {
class find_geometry_data
{
public:
    bool operator () (const std::pair<std::string, App::Property*>& elem) const
    {
        if (elem.first == "Points") {
            return elem.second->isDerivedFrom
                (Base::Type::fromName("Points::PropertyPointKernel"));
        }
        else if (elem.first == "Mesh") {
            return elem.second->isDerivedFrom
                (Base::Type::fromName("Mesh::PropertyMeshKernel"));
        }
        else if (elem.first == "Shape") {
            return elem.second->isDerivedFrom
                (Base::Type::fromName("Part::PropertyPartShape"));
        }

        // any other geometry type
        return elem.second->isDerivedFrom
            (Base::Type::fromName("App::PropertyGeometry"));
    }
};
class find_placement
{
public:
    bool operator () (const std::pair<std::string, App::Property*>& elem) const
    {
        if (elem.first == "Placement") {
            return elem.second->isDerivedFrom
                (Base::Type::fromName("App::PropertyPlacement"));
        }

        return false;
    }
};
}
}

/* TRANSLATOR Gui::Dialog::Transform */

Transform::Transform(QWidget* parent, Qt::WFlags fl)
  : Gui::LocationDialog(parent, fl)
{
    ui = new Ui_TransformComp(this);
    ui->resetButton->hide();
    ui->applyPlacementChange->hide();
    ui->applyIncrementalPlacement->hide();

    ui->angle->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    ui->yawAngle->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    ui->pitchAngle->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    ui->rollAngle->setSuffix(QString::fromUtf8(" \xc2\xb0"));

    ui->closeButton->setText(tr("Cancel"));
    this->setWindowTitle(tr("Transform"));

    // create a signal mapper in order to have one slot to perform the change
    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(this, SIGNAL(directionChanged()), signalMapper, SLOT(map()));
    signalMapper->setMapping(this, 0);

    int id = 1;
    QList<QDoubleSpinBox*> sb = this->findChildren<QDoubleSpinBox*>();
    for (QList<QDoubleSpinBox*>::iterator it = sb.begin(); it != sb.end(); ++it) {
        connect(*it, SIGNAL(valueChanged(double)), signalMapper, SLOT(map()));
        signalMapper->setMapping(*it, id++);
    }

    connect(signalMapper, SIGNAL(mapped(int)),
            this, SLOT(onTransformChanged(int)));

    Gui::SelectionChanges mod;
    mod.Type = Gui::SelectionChanges::SetSelection;
    onSelectionChanged(mod);
    setRotationCenter();
}

Transform::~Transform()
{
    delete ui;
}

void Transform::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == SelectionChanges::SetPreselect ||
        msg.Type == SelectionChanges::RmvPreselect)
        return; // nothing to do
    if (msg.Type == SelectionChanges::ClrSelection) {
        this->setDisabled(true);
        for (std::set<App::DocumentObject*>::iterator it = selection.begin();
             it != selection.end(); ++it)
             resetViewTransform(*it);
        selection.clear();
        return;
    }

    std::set<App::DocumentObject*> update_selection;
    std::vector<App::DocumentObject*> sel = Gui::Selection().getObjectsOfType
        (App::DocumentObject::getClassTypeId());
    for (std::vector<App::DocumentObject*>::iterator it=sel.begin();it!=sel.end();++it) {
        std::map<std::string,App::Property*> props;
        (*it)->getPropertyMap(props);
        // search for the placement property
        std::map<std::string,App::Property*>::iterator jt;
        jt = std::find_if(props.begin(), props.end(), find_geometry_data());
        if (jt != props.end()) {
            update_selection.insert(*it);
        }
    }

    // reste transform for all deselected objects
    std::vector<App::DocumentObject*> diff;
    std::back_insert_iterator< std::vector<App::DocumentObject*> > biit(diff);
    std::set_difference(selection.begin(), selection.end(),
        update_selection.begin(), update_selection.end(), biit);
    for (std::vector<App::DocumentObject*>::iterator it = diff.begin(); it != diff.end(); ++it)
         resetViewTransform(*it);
    selection = update_selection;

    this->setDisabled(selection.empty());
}

void Transform::acceptDataTransform(const Base::Matrix4D& mat, App::DocumentObject* obj)
{
    Gui::Document* doc = Gui::Application::Instance->getDocument(obj->getDocument());
    std::map<std::string,App::Property*> props;
    obj->getPropertyMap(props);
    // search for the placement property
    std::map<std::string,App::Property*>::iterator jt;
    jt = std::find_if(props.begin(), props.end(), find_placement());
    if (jt != props.end()) {
        Base::Placement local = static_cast<App::PropertyPlacement*>(jt->second)->getValue();
        Gui::ViewProvider* vp = doc->getViewProvider(obj);
        if (vp) vp->setTransformation(local.toMatrix());
    }
    else {
        // No placement found
        Gui::ViewProvider* vp = doc->getViewProvider(obj);
        if (vp) vp->setTransformation(Base::Matrix4D());
    }

    // Apply the transformation
    jt = std::find_if(props.begin(), props.end(), find_geometry_data());
    if (jt != props.end()) {
        static_cast<App::PropertyGeometry*>(jt->second)->transformGeometry(mat);
    }
}

void Transform::applyViewTransform(const Base::Placement& plm, App::DocumentObject* obj)
{
    Gui::Document* doc = Gui::Application::Instance->getDocument(obj->getDocument());
    std::map<std::string,App::Property*> props;
    obj->getPropertyMap(props);
    // search for the placement property
    std::map<std::string,App::Property*>::iterator jt;
    jt = std::find_if(props.begin(), props.end(), find_placement());
    if (jt != props.end()) {
        Base::Placement local = static_cast<App::PropertyPlacement*>(jt->second)->getValue();
        local *= plm; // in case a placement is already set
        Gui::ViewProvider* vp = doc->getViewProvider(obj);
        if (vp) vp->setTransformation(local.toMatrix());
    }
    else {
        // No placement found, so apply the transformation directly
        Gui::ViewProvider* vp = doc->getViewProvider(obj);
        if (vp) vp->setTransformation(plm.toMatrix());
    }
}

void Transform::resetViewTransform(App::DocumentObject* obj)
{
    Gui::Document* doc = Gui::Application::Instance->getDocument(obj->getDocument());
    std::map<std::string,App::Property*> props;
    obj->getPropertyMap(props);
    // search for the placement property
    std::map<std::string,App::Property*>::iterator jt;
    jt = std::find_if(props.begin(), props.end(), find_placement());
    if (jt != props.end()) {
        Base::Placement local = static_cast<App::PropertyPlacement*>(jt->second)->getValue();
        Gui::ViewProvider* vp = doc->getViewProvider(obj);
        if (vp) vp->setTransformation(local.toMatrix());
    }
    else {
        // No placement found
        Gui::ViewProvider* vp = doc->getViewProvider(obj);
        if (vp) vp->setTransformation(Base::Matrix4D());
    }
}

void Transform::showStandardButtons(bool b)
{
    ui->closeButton->setVisible(b);
    ui->oKButton->setVisible(b);
    ui->applyButton->setVisible(b);
}

void Transform::setRotationCenter()
{
    // get the global bounding box of all selected objects and use its center as
    // rotation center
    if (!selection.empty()) {
        Base::BoundBox3d bbox;
        bool first=true;
        for (std::set<App::DocumentObject*>::iterator it=selection.begin();it!=selection.end();++it) {
            std::map<std::string,App::Property*> props;
            (*it)->getPropertyMap(props);
            // search for a data property
            std::map<std::string,App::Property*>::iterator jt;
            jt = std::find_if(props.begin(), props.end(), find_geometry_data());
            if (jt != props.end()) {
                if (first)
                    bbox = (static_cast<App::PropertyGeometry*>(jt->second)->getBoundingBox());
                else
                    bbox.Add(static_cast<App::PropertyGeometry*>(jt->second)->getBoundingBox());
                first = false;
            }
        }

        ui->xCnt->setValue((bbox.MinX+bbox.MaxX)/2);
        ui->yCnt->setValue((bbox.MinY+bbox.MaxY)/2);
        ui->zCnt->setValue((bbox.MinZ+bbox.MaxZ)/2);
    }
}

void Transform::onTransformChanged(int)
{
    Base::Placement plm = this->getPlacementData();

    for (std::set<App::DocumentObject*>::iterator it=selection.begin();it!=selection.end();++it) {
        applyViewTransform(plm, *it);
    }
}

void Transform::accept()
{
    on_applyButton_clicked();
    QDialog::accept();
}

void Transform::reject()
{
    for (std::set<App::DocumentObject*>::iterator it=selection.begin();it!=selection.end();++it) {
        resetViewTransform(*it);
    }

    QDialog::reject();
}

void Transform::on_applyButton_clicked()
{
    Base::Placement plm = this->getPlacementData();
    Base::Matrix4D mat = plm.toMatrix();

    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    if (doc) {
        doc->openCommand("Transform");
        for (std::set<App::DocumentObject*>::iterator it=selection.begin();it!=selection.end();++it) {
            acceptDataTransform(mat, *it);
        }
        doc->commitCommand();
    }

    // nullify the values
    QList<QDoubleSpinBox*> sb = this->findChildren<QDoubleSpinBox*>();
    for (QList<QDoubleSpinBox*>::iterator it = sb.begin(); it != sb.end(); ++it) {
        (*it)->blockSignals(true);
        (*it)->setValue(0.0);
        (*it)->blockSignals(false);
    }
    setRotationCenter();
}

void Transform::directionActivated(int index)
{
    if (ui->directionActivated(this, index)) {
        /*emit*/ directionChanged();
    }
}

Base::Vector3f Transform::getDirection() const
{
    return ui->getDirection();
}

Base::Placement Transform::getPlacementData() const
{
    int index = ui->rotationInput->currentIndex();
    Base::Rotation rot;
    Base::Vector3d pos;
    Base::Vector3d cnt;

    pos = Base::Vector3d(ui->xPos->value(),ui->yPos->value(),ui->zPos->value());
    cnt = Base::Vector3d(ui->xCnt->value(),ui->yCnt->value(),ui->zCnt->value());

    if (index == 0) {
        Base::Vector3f dir = getDirection();
        rot.setValue(Base::Vector3d(dir.x,dir.y,dir.z),ui->angle->value()*D_PI/180.0);
    }
    else if (index == 1) {
        rot.setYawPitchRoll(
            ui->yawAngle->value(),
            ui->pitchAngle->value(),
            ui->rollAngle->value());
    }

    Base::Placement p(pos, rot, cnt);
    return p;
}

void Transform::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslate(this);
        ui->closeButton->setText(tr("Cancel"));
        this->setWindowTitle(tr("Transform"));
    }
    else {
        QDialog::changeEvent(e);
    }
}

// ---------------------------------------

TaskTransform::TaskTransform()
{
    dialog = new Transform();
    dialog->showStandardButtons(false);
    taskbox = new Gui::TaskView::TaskBox(QPixmap(), dialog->windowTitle(), true, 0);
    taskbox->groupLayout()->addWidget(dialog);
    Content.push_back(taskbox);
}

TaskTransform::~TaskTransform()
{
    // automatically deleted in the sub-class
}

bool TaskTransform::accept()
{
    dialog->accept();
    return (dialog->result() == QDialog::Accepted);
}

bool TaskTransform::reject()
{
    dialog->reject();
    return (dialog->result() == QDialog::Rejected);
}

void TaskTransform::clicked(int id)
{
    if (id == QDialogButtonBox::Apply) {
        dialog->on_applyButton_clicked();
    }
}

#include "moc_Transform.cpp"
