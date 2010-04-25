/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
#include <QSignalMapper>
#include <QDockWidget>

#include "Placement.h"
#include "ui_Placement.h"
#include <Gui/DockWindowManager.h>
#include <Gui/Application.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <App/PropertyGeo.h>
#include <Base/Console.h>

using namespace Gui::Dialog;

namespace Gui { namespace Dialog {
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

/* TRANSLATOR Gui::Dialog::Placement */

Placement::Placement(QWidget* parent, Qt::WFlags fl)
  : Gui::LocationDialog(parent, fl)
{
    ui = new Ui_PlacementComp(this);
    ui->angle->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    ui->yawAngle->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    ui->pitchAngle->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    ui->rollAngle->setSuffix(QString::fromUtf8(" \xc2\xb0"));

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
            this, SLOT(onPlacementChanged(int)));
}

Placement::~Placement()
{
    delete ui;
}

void Placement::onPlacementChanged(int)
{
    if (ui->applyPlacementChange->isChecked()) {
        Base::Placement plm = this->getPlacement();
        if (receivers(SIGNAL(placementChanged(QVariant))) > 0) {
            QVariant data = QVariant::fromValue<Base::Placement>(plm);
            /*emit*/ placementChanged(data);
        }
        else {
            applyPlacement(plm);
        }
    }
}

void Placement::applyPlacement(const Base::Placement& p)
{
    std::vector<App::DocumentObject*> sel = Gui::Selection().getObjectsOfType
        (App::DocumentObject::getClassTypeId());
    if (!sel.empty()) {
        for (std::vector<App::DocumentObject*>::iterator it=sel.begin();it!=sel.end();++it) {
            std::map<std::string,App::Property*> props;
            (*it)->getPropertyMap(props);
            // search for the placement property
            std::map<std::string,App::Property*>::iterator jt;
            jt = std::find_if(props.begin(), props.end(), find_placement());
            if (jt != props.end()) {
                Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(*it);
                if (vp) vp->setTransformation(p.toMatrix());
            }
        }
    }
    else {
        Base::Console().Warning("No object selected.\n");
    }
}

void Placement::on_applyPlacementChange_toggled(bool on)
{
    ui->applyButton->setDisabled(on);
    if (on) onPlacementChanged(0);
}

void Placement::on_applyIncrementalPlacement_toggled(bool on)
{
    if (on) {
        this->pm = getPlacementData();
        QList<QDoubleSpinBox*> sb = this->findChildren<QDoubleSpinBox*>();
        for (QList<QDoubleSpinBox*>::iterator it = sb.begin(); it != sb.end(); ++it) {
            (*it)->blockSignals(true);
            (*it)->setValue(0.0);
            (*it)->blockSignals(false);
        }
    }
    else {
        Base::Placement p = getPlacementData();
        this->pm = p * this->pm;
        this->blockSignals(true);
        setPlacementData(this->pm);
        this->blockSignals(false);
    }
}

void Placement::accept()
{
    if (!ui->applyPlacementChange->isChecked())
        on_applyButton_clicked();
    QDialog::accept();
}

void Placement::on_applyButton_clicked()
{
    Base::Placement plm = this->getPlacement();
    if (receivers(SIGNAL(placementChanged(QVariant))) > 0) {
        QVariant data = QVariant::fromValue<Base::Placement>(plm);
        /*emit*/ placementChanged(data);
    }
    else {
        applyPlacement(plm);
    }
}

void Placement::on_resetButton_clicked()
{
    QList<QDoubleSpinBox*> sb = this->findChildren<QDoubleSpinBox*>();
    for (QList<QDoubleSpinBox*>::iterator it = sb.begin(); it != sb.end(); ++it) {
        (*it)->blockSignals(true);
        (*it)->setValue(0.0);
        (*it)->blockSignals(false);
    }

    onPlacementChanged(0);
}

void Placement::directionActivated(int index)
{
    if (ui->directionActivated(this, index)) {
        /*emit*/ directionChanged();
    }
}

Base::Vector3f Placement::getDirection() const
{
    return ui->getDirection();
}

void Placement::setPlacement(const Base::Placement& p)
{
    this->pm = p;
    setPlacementData(this->pm);
}

void Placement::setPlacementData(const Base::Placement& p)
{

    ui->xPos->setValue(p.getPosition().x);
    ui->yPos->setValue(p.getPosition().y);
    ui->zPos->setValue(p.getPosition().z);

    double Y,P,R;
    p.getRotation().getYawPitchRoll(Y,P,R);
    ui->yawAngle->setValue(Y);
    ui->pitchAngle->setValue(P);
    ui->rollAngle->setValue(R);

    // check if the user-defined direction is already there
    double angle;
    Base::Vector3d axis;
    p.getRotation().getValue(axis, angle);
    ui->angle->setValue(angle*180.0/D_PI);
    Base::Vector3f dir((float)axis.x,(float)axis.y,(float)axis.z);
    for (int i=0; i<ui->direction->count()-1; i++) {
        QVariant data = ui->direction->itemData (i);
        if (data.canConvert<Base::Vector3f>()) {
            const Base::Vector3f val = data.value<Base::Vector3f>();
            if (val == dir) {
                ui->direction->setCurrentIndex(i);
                return;
            }
        }
    }

    // add a new item before the very last item
    QString display = QString::fromAscii("(%1,%2,%3)")
        .arg(dir.x)
        .arg(dir.y)
        .arg(dir.z);
    ui->direction->insertItem(ui->direction->count()-1, display,
        QVariant::fromValue<Base::Vector3f>(dir));
    ui->direction->setCurrentIndex(ui->direction->count()-2);
}

Base::Placement Placement::getPlacement() const
{
    Base::Placement p = getPlacementData();
    if (ui->applyIncrementalPlacement->isChecked())
        p = p * this->pm;
    return p;
}

Base::Placement Placement::getPlacementData() const
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

void Placement::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslate(this);
    }
    else {
        QDialog::changeEvent(e);
    }
}

// ----------------------------------------------

DockablePlacement::DockablePlacement(QWidget* parent, Qt::WFlags fl) : Placement(parent, fl)
{
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    QDockWidget* dw = pDockMgr->addDockWindow(QT_TR_NOOP("Placement"),
        this, Qt::BottomDockWidgetArea);
    dw->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    dw->show();
}

DockablePlacement::~DockablePlacement()
{
}

void DockablePlacement::accept()
{
    // closes the dock window
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    pDockMgr->removeDockWindow(this);
    Placement::accept();
}

void DockablePlacement::reject()
{
    // closes the dock window
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    pDockMgr->removeDockWindow(this);
    Placement::reject();
}

#include "moc_Placement.cpp"
