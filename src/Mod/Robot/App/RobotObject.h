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


#ifndef ROBOT_ROBOTOBJECT_H
#define ROBOT_ROBOTOBJECT_H

#include <App/DocumentObject.h>
#include <App/PropertyFile.h>
#include <App/PropertyGeo.h>


namespace Robot
{

class AppRobotExport RobotObject : public App::DocumentObject
{
    PROPERTY_HEADER(Robot::RobotObject);

public:
    /// Constructor
    RobotObject(void);
    virtual ~RobotObject();

    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "RobotGui::ViewProviderRobotObject";
    }
    virtual App::DocumentObjectExecReturn *execute(void) {
        return App::DocumentObject::StdReturn;
    }
    virtual short mustExecute(void) const;
    virtual PyObject *getPyObject(void);

    App::PropertyFileIncluded RobotVrmlFile;

    App::PropertyFloat Axis1,Axis2,Axis3,Axis4,Axis5,Axis6;

	App::PropertyPlacement Base;
	App::PropertyPlacement Tcp;


};

} //namespace Robot


#endif // ROBOT_ROBOTOBJECT_H
