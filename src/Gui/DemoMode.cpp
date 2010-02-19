/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <cmath>
# include <float.h>
# include <climits>
# include <QTimer>
#endif

#include "DemoMode.h"
#include "ui_DemoMode.h"

#include "Application.h"
#include "Document.h"
#include "MainWindow.h"
#include "View3DInventor.h"
#include "View3DInventorViewer.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DemoMode */

DemoMode::DemoMode(QWidget* parent, Qt::WFlags fl)
  : QDialog(/*parent*/0, fl|Qt::WindowStaysOnTopHint), ui(new Ui_DemoMode)
{
    // create widgets
    ui->setupUi(this);
    timer = new QTimer(this);
    timer->setInterval(30000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onAutoPlay()));
}

/** Destroys the object and frees any allocated resources */
DemoMode::~DemoMode()
{
    delete ui;
}

void DemoMode::accept()
{
    on_fullscreen_toggled(false);
    on_stopButton_clicked();
    QDialog::accept();
}

void DemoMode::reject()
{
    on_fullscreen_toggled(false);
    on_stopButton_clicked();
    QDialog::reject();
}

Gui::View3DInventor* DemoMode::activeView() const
{
    Document* doc = Application::Instance->activeDocument();
    if (doc) {
        MDIView* view = doc->getActiveView();
        if (doc && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
            return static_cast<Gui::View3DInventor*>(view);
        }
    }

    return 0;
}

float DemoMode::getSpeed(int v) const
{
    float speed = ((float)v)/50.0f; // let 2.0 be the maximum speed
    return speed;
}

SbVec3f DemoMode::getDirection() const
{
    SbVec3f vec((float)ui->xSlider->value(),
                (float)ui->ySlider->value(),
                (float)ui->zSlider->value());
    if (vec.length() < FLT_EPSILON)
        vec.setValue(1,1,1);
    vec.normalize();
    return vec;
}

void DemoMode::on_xSlider_valueChanged(int)
{
    Gui::View3DInventor* view = activeView();
    if (view && view->getViewer()->isAnimating()) {
        view->getViewer()->startAnimating(getDirection(),getSpeed
            (ui->speedSlider->value()));
    }
}

void DemoMode::on_ySlider_valueChanged(int)
{
    Gui::View3DInventor* view = activeView();
    if (view && view->getViewer()->isAnimating()) {
        view->getViewer()->startAnimating(getDirection(),getSpeed
            (ui->speedSlider->value()));
    }
}

void DemoMode::on_zSlider_valueChanged(int)
{
    Gui::View3DInventor* view = activeView();
    if (view && view->getViewer()->isAnimating()) {
        view->getViewer()->startAnimating(getDirection(),getSpeed
            (ui->speedSlider->value()));
    }
}

void DemoMode::on_speedSlider_valueChanged(int v)
{
    Gui::View3DInventor* view = activeView();
    if (view && view->getViewer()->isAnimating()) {
        view->getViewer()->startAnimating(getDirection(),getSpeed(v));
    }
}

void DemoMode::on_playButton_clicked()
{
    Gui::View3DInventor* view = activeView();
    if (view) {
        view->getViewer()->startAnimating(getDirection(),getSpeed(ui->speedSlider->value()));
    }
}

void DemoMode::on_stopButton_clicked()
{
    Gui::View3DInventor* view = activeView();
    if (view)
        view->getViewer()->stopAnimating();
}

void DemoMode::on_fullscreen_toggled(bool on)
{
    Gui::View3DInventor* view = activeView();
    if (view) {
        view->setCurrentViewMode(on ? MDIView::/*TopLevel*/FullScreen : MDIView::Child);
        this->activateWindow();
    }
}

void DemoMode::onAutoPlay()
{
    Gui::View3DInventor* view = activeView();
    if (view && !view->getViewer()->isAnimating()) {
        view->getViewer()->startAnimating(getDirection(),getSpeed
            (ui->speedSlider->value()));
    }
}

void DemoMode::on_timerCheck_toggled(bool on)
{
    if (on)
        timer->start();
    else
        timer->stop();
}

#include "moc_DemoMode.cpp"
