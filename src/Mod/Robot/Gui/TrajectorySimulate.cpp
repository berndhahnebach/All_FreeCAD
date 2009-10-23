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

#include "ui_TaskTrajectory.h"
#include "TrajectorySimulate.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Console.h>
#include <Gui/Selection.h>


using namespace RobotGui;
using namespace Gui;

TrajectorySimulate::TrajectorySimulate(Robot::RobotObject *pcRobotObject,Robot::TrajectoryObject *pcTrajectoryObject,QWidget *parent)
    : QDialog( parent)
{
    this->setupUi(this);
    QMetaObject::connectSlotsByName(this);

    Robot::Trajectory trac = pcTrajectoryObject->Trajectory.getValue();
    for(unsigned int i=0;i<trac.getSize();i++){
        Robot::Waypoint pt = trac.getWaypoint(i);
        

    }
}

TrajectorySimulate::~TrajectorySimulate()
{
}



#include "moc_TrajectorySimulate.cpp"
