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

#include "TaskDlgTrajectoryCompound.h"

#include <Gui/TaskView/TaskSelectLinkProperty.h>


using namespace RobotGui;


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgTrajectoryCompound::TaskDlgTrajectoryCompound(Robot::Edge2TracObject *obj)
    : TaskDialog(),Edge2TaskObject(obj)
{
    param  = new TaskEdge2TracParameter(obj);
    select = new Gui::TaskView::TaskSelectLinkProperty("SELECT Part::Feature SUBELEMENT Edge",&(obj->Source));

    Content.push_back(param);
    Content.push_back(select);
}

TaskDlgTrajectoryCompound::~TaskDlgTrajectoryCompound()
{

}

//==== calls from the TaskView ===============================================================


void TaskDlgTrajectoryCompound::open()
{
    select->activate();
    Edge2TaskObject->execute();
    param->setEdgeAndClusterNbr(Edge2TaskObject->NbrOfEdges,Edge2TaskObject->NbrOfCluster);

}

void TaskDlgTrajectoryCompound::clicked(int button)
{
    if(QDialogButtonBox::Apply == button)
    {
        if(select->isSelectionValid()){
            select->sendSelection2Property();
            Edge2TaskObject->execute();
            param->setEdgeAndClusterNbr(Edge2TaskObject->NbrOfCluster,Edge2TaskObject->NbrOfEdges);
        }else
            QApplication::beep();
            param->setEdgeAndClusterNbr(0,0);
    }
    
}

bool TaskDlgTrajectoryCompound::accept()
{
    if(select->isSelectionValid()){
        select->accept();
        Edge2TaskObject->execute();
        return true;
    }else
        QApplication::beep();

    return false;
}

bool TaskDlgTrajectoryCompound::reject()
{
    select->reject();
    Edge2TaskObject->execute();
    return true;
}

void TaskDlgTrajectoryCompound::helpRequested()
{

}


#include "moc_TaskDlgTrajectoryCompound.cpp"
