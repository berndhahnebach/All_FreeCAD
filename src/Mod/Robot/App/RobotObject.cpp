/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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

#include "RobotObject.h"
#include <App/DocumentObjectPy.h>
#include <Base/Placement.h>

using namespace Robot;
using namespace App;

PROPERTY_SOURCE(Robot::RobotObject, App::DocumentObject)


RobotObject::RobotObject()
:block(false)
{
    ADD_PROPERTY_TYPE(RobotVrmlFile,(0),"Robot definition",Prop_None,"Included file with the VRML representation of the robot");

    ADD_PROPERTY_TYPE(Axis1,(0.0),"Robot kinematic",Prop_None,"Axis 1 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis2,(0.0),"Robot kinematic",Prop_None,"Axis 2 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis3,(0.0),"Robot kinematic",Prop_None,"Axis 3 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis4,(0.0),"Robot kinematic",Prop_None,"Axis 4 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis5,(0.0),"Robot kinematic",Prop_None,"Axis 5 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis6,(0.0),"Robot kinematic",Prop_None,"Axis 6 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Error,("") ,"Robot kinematic",Prop_None,"Robot error while moving");

    ADD_PROPERTY_TYPE(Tcp,(Base::Placement()),"Robot kinematic",Prop_None,"Tcp of the robot");
    ADD_PROPERTY_TYPE(Base,(Base::Placement()),"Robot kinematic",Prop_None,"Actuall base frame of the robot");
    ADD_PROPERTY_TYPE(Base,(Base::Placement()),"Robot definition",Prop_None,"Position of the robot in the simulation");

}

RobotObject::~RobotObject()
{
}

short RobotObject::mustExecute(void) const
{
    return 0;
}

PyObject *RobotObject::getPyObject()
{
    if (PythonObject.is(Py::_None())){
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this),true);
    }
    return Py::new_reference_to(PythonObject); 
}

void RobotObject::onChanged(const Property* prop)
{
    static bool block = false;
    if(prop == &Axis1 && !block){
        robot.setAxis(0,Axis1.getValue());
        block = true;
        Tcp.setValue(robot.getTcp());
        block = false;
    }
    if(prop == &Axis2 && !block){
        robot.setAxis(0,Axis2.getValue());
        block = true;
        Tcp.setValue(robot.getTcp());
        block = false;
    }
    if(prop == &Axis3 && !block){
        robot.setAxis(0,Axis3.getValue());
        block = true;
        Tcp.setValue(robot.getTcp());
        block = false;
    }
    if(prop == &Axis4 && !block){
        robot.setAxis(0,Axis4.getValue());
        block = true;
        Tcp.setValue(robot.getTcp());
        block = false;
    }
    if(prop == &Axis5 && !block){
        robot.setAxis(0,Axis5.getValue());
        block = true;
        Tcp.setValue(robot.getTcp());
        block = false;
    }
    if(prop == &Axis6 && !block){
        robot.setAxis(0,Axis6.getValue());
        block = true;
        Tcp.setValue(robot.getTcp());
        block = false;
    }
    if(prop == &Tcp && !block){
        robot.setTo(Tcp.getValue());
        block = true;
        Axis1.setValue(robot.getAxis(0));
        Axis2.setValue(robot.getAxis(1));
        Axis3.setValue(robot.getAxis(2));
        Axis4.setValue(robot.getAxis(3));
        Axis5.setValue(robot.getAxis(4));
        Axis6.setValue(robot.getAxis(5));
        block = false;
    }
    App::DocumentObject::onChanged(prop);
}
