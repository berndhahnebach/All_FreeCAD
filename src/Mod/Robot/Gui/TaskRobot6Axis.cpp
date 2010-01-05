/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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
#endif

#include <QString>
#include <QSlider>
#include "ui_TaskRobot6Axis.h"
#include "TaskRobot6Axis.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Console.h>
#include <Gui/Selection.h>


using namespace RobotGui;
using namespace Gui;

TaskRobot6Axis::TaskRobot6Axis(Robot::RobotObject *pcRobotObject,QWidget *parent)
    : TaskBox(Gui::BitmapFactory().pixmap("document-new"),tr("TaskRobot6Axis"),true, parent),
    pcRobot(pcRobotObject)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskRobot6Axis();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);

    Robot::Robot6Axis &Rob = pcRobot->getRobot();

    ui->horizontalSlider_Axis1->setMaximum(  (int ) Rob.getMaxAngle(0) );
    ui->horizontalSlider_Axis1->setMinimum(  (int ) Rob.getMinAngle(0) );
 
    ui->horizontalSlider_Axis2->setMaximum(  (int ) Rob.getMaxAngle(1) );
    ui->horizontalSlider_Axis2->setMinimum(  (int ) Rob.getMinAngle(1) );

    ui->horizontalSlider_Axis3->setMaximum(  (int ) Rob.getMaxAngle(2) );
    ui->horizontalSlider_Axis3->setMinimum(  (int ) Rob.getMinAngle(2) );

    ui->horizontalSlider_Axis4->setMaximum(  (int ) Rob.getMaxAngle(3) );
    ui->horizontalSlider_Axis4->setMinimum(  (int ) Rob.getMinAngle(3) );

    ui->horizontalSlider_Axis5->setMaximum(  (int ) Rob.getMaxAngle(4) );
    ui->horizontalSlider_Axis5->setMinimum(  (int ) Rob.getMinAngle(4) );

    ui->horizontalSlider_Axis6->setMaximum(  (int ) Rob.getMaxAngle(5) );
    ui->horizontalSlider_Axis6->setMinimum(  (int ) Rob.getMinAngle(5) );

}

TaskRobot6Axis::~TaskRobot6Axis()
{
    delete ui;
    
}

void TaskRobot6Axis::setAxis(float A1,float A2,float A3,float A4,float A5,float A6)
{
    ui->horizontalSlider_Axis1->setSliderPosition((int)A1);
    ui->lineEdit_Axis1->setText(QString::fromLatin1("%1°").arg(A1,0,'f',1));

    ui->horizontalSlider_Axis2->setSliderPosition((int)A2);
    ui->lineEdit_Axis2->setText(QString::fromLatin1("%1°").arg(A2,0,'f',1));

    ui->horizontalSlider_Axis3->setSliderPosition((int)A3);
    ui->lineEdit_Axis3->setText(QString::fromLatin1("%1°").arg(A3,0,'f',1));

    ui->horizontalSlider_Axis4->setSliderPosition((int)A4);
    ui->lineEdit_Axis4->setText(QString::fromLatin1("%1°").arg(A4,0,'f',1));

    ui->horizontalSlider_Axis5->setSliderPosition((int)A5);
    ui->lineEdit_Axis5->setText(QString::fromLatin1("%1°").arg(A5,0,'f',1));

    ui->horizontalSlider_Axis6->setSliderPosition((int)A6);
    ui->lineEdit_Axis6->setText(QString::fromLatin1("%1°").arg(A6,0,'f',1));


}



#include "moc_TaskRobot6Axis.cpp"
