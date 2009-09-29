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
# include <Inventor/SoDB.h>
# include <Inventor/SoInput.h>
# include <Inventor/SbVec3f.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/nodes/SoSphere.h>
# include <Inventor/nodes/SoRotation.h>
# include <Inventor/actions/SoSearchAction.h>
# include <Inventor/draggers/SoJackDragger.h>
# include <Inventor/VRMLnodes/SoVRMLTransform.h>
# include <QFile>
#endif

#include "ViewProviderTrajectory.h"

#include <Mod/Robot/App/TrajectoryObject.h>
#include <App/Document.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>
#include <Base/Console.h>
#include <sstream>
using namespace Gui;
using namespace RobotGui;

PROPERTY_SOURCE(RobotGui::ViewProviderTrajectory, Gui::ViewProviderGeometryObject)

ViewProviderTrajectory::ViewProviderTrajectory()
{
    
	pcTrajectoryRoot = new Gui::SoFCSelection();
    pcTrajectoryRoot->highlightMode = Gui::SoFCSelection::OFF;
    pcTrajectoryRoot->selectionMode = Gui::SoFCSelection::SEL_OFF;
    //pcRobotRoot->style = Gui::SoFCSelection::BOX;
    pcTrajectoryRoot->ref();
}

ViewProviderTrajectory::~ViewProviderTrajectory()
{
    pcTrajectoryRoot->unref();
}

void ViewProviderTrajectory::attach(App::DocumentObject *pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);

    addDisplayMaskMode(pcTrajectoryRoot, "Waypoints");
    pcTrajectoryRoot->objectName = pcObj->getNameInDocument();
    pcTrajectoryRoot->documentName = pcObj->getDocument()->getName();
    pcTrajectoryRoot->subElementName = "Main";

}

void ViewProviderTrajectory::setDisplayMode(const char* ModeName)
{
    if ( strcmp("Waypoints",ModeName)==0 )
        setDisplayMaskMode("Waypoints");
    ViewProviderGeometryObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderTrajectory::getDisplayModes(void) const
{
    std::vector<std::string> StrList;
    StrList.push_back("Waypoints");
    return StrList;
}

void ViewProviderTrajectory::updateData(const App::Property* prop)
{
    Robot::TrajectoryObject* pcTracObj = static_cast<Robot::TrajectoryObject*>(pcObject);
    if (prop == &pcTracObj->Trajectory) {

	}else if (prop == &pcTracObj->Base) {
        Base::Placement loc = *(&pcTracObj->Base.getValue());
    }

}

