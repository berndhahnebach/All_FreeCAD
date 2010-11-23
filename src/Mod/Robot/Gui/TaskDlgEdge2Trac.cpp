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

#include "TaskDlgEdge2Trac.h"

using namespace RobotGui;


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgEdge2Trac::TaskDlgEdge2Trac(Robot::RobotObject *pcRobotObject,Robot::TrajectoryObject *pcTrajectoryObject)
    : TaskDialog()
{
    rob  = new TaskRobot6Axis(pcRobotObject);
    ctr  = new TaskRobotControl(pcRobotObject);
    
    trac = new TaskTrajectory(pcRobotObject,pcTrajectoryObject);
    msg  = new TaskRobotMessages(pcRobotObject);
    
    
    QObject::connect(trac ,SIGNAL(axisChanged(float,float,float,float,float,float,const Base::Placement &)),
                     rob  ,SLOT  (setAxis(float,float,float,float,float,float,const Base::Placement &)));

    Content.push_back(rob);
    Content.push_back(ctr);
    Content.push_back(trac);
    Content.push_back(msg);
}

TaskDlgEdge2Trac::~TaskDlgEdge2Trac()
{

}

//==== calls from the TaskView ===============================================================


void TaskDlgEdge2Trac::open()
{
    msg->hideGroupBox();
    ctr->hideGroupBox();
}

void TaskDlgEdge2Trac::clicked(int)
{
    
}

bool TaskDlgEdge2Trac::accept()
{
    return true;
}

bool TaskDlgEdge2Trac::reject()
{
    return true;
}

void TaskDlgEdge2Trac::helpRequested()
{

}


#include "moc_TaskDlgEdge2Trac.cpp"
