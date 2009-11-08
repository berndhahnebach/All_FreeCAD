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

#include <QTimer>
#include "TrajectorySimulate.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Console.h>
#include <Gui/Selection.h>

#include <Mod/Robot/App/Waypoint.h>


using namespace RobotGui;
using namespace Gui;

TrajectorySimulate::TrajectorySimulate(Robot::RobotObject *pcRobotObject,Robot::TrajectoryObject *pcTrajectoryObject,QWidget *parent)
    : QDialog( parent),sim(pcTrajectoryObject->Trajectory.getValue(),pcRobotObject->getRobot())
{
    this->setupUi(this);
    QMetaObject::connectSlotsByName(this);

    trajectoryTable->setSortingEnabled(false);

    Robot::Trajectory trac = pcTrajectoryObject->Trajectory.getValue();
    trajectoryTable->setRowCount(trac.getSize());
    duration = trac.getDuration();

    for(unsigned int i=0;i<trac.getSize();i++){
        Robot::Waypoint pt = trac.getWaypoint(i);
        switch(pt.Type){
            case Robot::Waypoint::UNDEF: trajectoryTable->setItem(i, 0, new QTableWidgetItem(QString::fromAscii("UNDEF")));break;
            case Robot::Waypoint::CIRC: trajectoryTable->setItem(i, 0, new QTableWidgetItem(QString::fromAscii("CIRC")));break;
            case Robot::Waypoint::PTP: trajectoryTable->setItem(i, 0, new QTableWidgetItem(QString::fromAscii("PTP")));break;
            case Robot::Waypoint::LINE: trajectoryTable->setItem(i, 0, new QTableWidgetItem(QString::fromAscii("LIN")));break;
            default: trajectoryTable->setItem(i, 0, new QTableWidgetItem(QString::fromAscii("UNDEF")));break;
        }
        trajectoryTable->setItem(i, 1, new QTableWidgetItem(QString::fromUtf8(pt.Name.c_str())));
        if(pt.Cont)
            trajectoryTable->setItem(i, 2, new QTableWidgetItem(QString::fromAscii("|")));
        else
            trajectoryTable->setItem(i, 2, new QTableWidgetItem(QString::fromAscii("-")));
        trajectoryTable->setItem(i, 3, new QTableWidgetItem(QString::number(pt.Velocity)));
        trajectoryTable->setItem(i, 4, new QTableWidgetItem(QString::number(pt.Accelaration)));

    }

    QObject::connect(ButtonStepStart    ,SIGNAL(clicked()),this,SLOT(start()));
    QObject::connect(ButtonStepStop     ,SIGNAL(clicked()),this,SLOT(stop()));
    QObject::connect(ButtonStepRun      ,SIGNAL(clicked()),this,SLOT(run()));
    QObject::connect(ButtonStepBack     ,SIGNAL(clicked()),this,SLOT(back()));
    QObject::connect(ButtonStepForward  ,SIGNAL(clicked()),this,SLOT(forward()));
    QObject::connect(ButtonStepEnd      ,SIGNAL(clicked()),this,SLOT(end()));

    // set up timer
    timer = new QTimer( this );
    timer->setInterval(100);
    QObject::connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );

}

TrajectorySimulate::~TrajectorySimulate()
{
}

void TrajectorySimulate::start(void)
{
}
void TrajectorySimulate::stop(void)
{
}
void TrajectorySimulate::run(void)
{
}
void TrajectorySimulate::back(void)
{
}
void TrajectorySimulate::forward(void)
{
}
void TrajectorySimulate::end(void)
{
}

void TrajectorySimulate::timerDone(void)
{
}

void TrajectorySimulate::valueChanged ( int value )
{

}


#include "moc_TrajectorySimulate.cpp"
