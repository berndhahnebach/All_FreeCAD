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

#include "Placement.h"
#include "ui_Placement.h"

using namespace Gui::Dialog;

Placement::Placement(QWidget* parent, Qt::WFlags fl)
  : Gui::LocationDialog(parent, fl)
{
    ui = new Ui_PlacementComp(this);
    ui->angle->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    ui->applyButton->setEnabled(false);
}

Placement::~Placement()
{
    delete ui;
}

void Placement::accept()
{
    QDialog::accept();
}

void Placement::directionActivated(int index)
{
    ui->directionActivated(this, index);
}

Base::Vector3f Placement::getDirection() const
{
    return ui->getDirection();
}

void Placement::setPlacement(const Base::Placement& p)
{
    ui->xPos->setValue(p.getPosition().x);
    ui->yPos->setValue(p.getPosition().y);
    ui->zPos->setValue(p.getPosition().z);

    double Y,P,R;
    p.getRotation().getYawPitchRoll(Y,P,R);
    ui->RotA->setValue(Y*180.0/D_PI);
    ui->RotB->setValue(P*180.0/D_PI);
    ui->RotC->setValue(R*180.0/D_PI);
    ui->RotA->setEnabled(false);
    ui->RotB->setEnabled(false);
    ui->RotC->setEnabled(false);

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
    Base::Placement p;
    p.setPosition(Base::Vector3d(ui->xPos->value(),ui->yPos->value(),ui->zPos->value()));
    Base::Vector3f dir = getDirection();
    p.setRotation(Base::Rotation(Base::Vector3d(dir.x,dir.y,dir.z),ui->angle->value()*D_PI/180.0));
    return p;
}
