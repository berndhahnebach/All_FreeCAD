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

using namespace Robot;
using namespace App;

PROPERTY_SOURCE(Robot::RobotObject, App::DocumentObject)


RobotObject::RobotObject() 
{
    ADD_PROPERTY_TYPE(RobotVrmlFile,(0),"",Prop_None,"Included file with the VRML representation of the robot");

    ADD_PROPERTY_TYPE(Axis1,(0.0),"",Prop_None,"Axis 1 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis2,(0.0),"",Prop_None,"Axis 2 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis3,(0.0),"",Prop_None,"Axis 3 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis4,(0.0),"",Prop_None,"Axis 4 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis5,(0.0),"",Prop_None,"Axis 5 angle of the robot in degre");
    ADD_PROPERTY_TYPE(Axis6,(0.0),"",Prop_None,"Axis 6 angle of the robot in degre");

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
