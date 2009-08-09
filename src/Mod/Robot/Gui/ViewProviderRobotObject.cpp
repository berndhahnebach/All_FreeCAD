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
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/actions/SoSearchAction.h>
# include <Inventor/VRMLnodes/SoVRMLTransform.h>
# include <QFile>
#endif

#include "ViewProviderRobotObject.h"

#include <Mod/Robot/App/RobotObject.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <App/Document.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>
#include <sstream>
using namespace Gui;
using namespace RobotGui;

PROPERTY_SOURCE(RobotGui::ViewProviderRobotObject, Gui::ViewProviderDocumentObject)

ViewProviderRobotObject::ViewProviderRobotObject()
{
	pcRobotRoot = new Gui::SoFCSelection();
    pcRobotRoot->highlightMode = Gui::SoFCSelection::OFF;
    pcRobotRoot->selectionMode = Gui::SoFCSelection::SEL_OFF;
    //pcRobotRoot->style = Gui::SoFCSelection::BOX;
    pcRobotRoot->ref();
	Axis1Node = Axis2Node = Axis3Node = Axis4Node = Axis5Node = Axis6Node = 0;
}

ViewProviderRobotObject::~ViewProviderRobotObject()
{
    pcRobotRoot->unref();
}

void ViewProviderRobotObject::attach(App::DocumentObject *pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);
    addDisplayMaskMode(pcRobotRoot, "VRML");
    pcRobotRoot->objectName = pcObj->getNameInDocument();
    pcRobotRoot->documentName = pcObj->getDocument()->getName();
    pcRobotRoot->subElementName = "Main";
}

void ViewProviderRobotObject::setDisplayMode(const char* ModeName)
{
    if ( strcmp("VRML",ModeName)==0 )
        setDisplayMaskMode("VRML");
    ViewProviderDocumentObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderRobotObject::getDisplayModes(void) const
{
    std::vector<std::string> StrList;
    StrList.push_back("VRML");
    return StrList;
}

void ViewProviderRobotObject::updateData(const App::Property* prop)
{
    Robot::RobotObject* robObj = static_cast<Robot::RobotObject*>(pcObject);
    if (prop == &robObj->RobotVrmlFile) {
        // read also from file
        const char* filename = robObj->RobotVrmlFile.getValue();
        QFile file(QString::fromUtf8(filename));
        SoInput in;
        pcRobotRoot->removeAllChildren();
        if (file.open(QFile::ReadOnly)) {
            QByteArray buffer = file.readAll();
            in.setBuffer((void *)buffer.constData(), buffer.length());
            SoSeparator * node = SoDB::readAll(&in);
            if (node) pcRobotRoot->addChild(node);
        }
		// search for the conection points +++++++++++++++++++++++++++++++++++++++++++++++++
		Axis1Node = Axis2Node = Axis3Node = Axis4Node = Axis5Node = Axis6Node = 0;
		SoSearchAction searchAction;
		SoPath * path;

		// Axis 1
		searchAction.setName("FREECAD_AXIS1");
		searchAction.setInterest(SoSearchAction::FIRST);
		searchAction.setSearchingAll(FALSE);
		searchAction.apply(pcRobotRoot);
		path = searchAction.getPath();
		if(path){
			SoNode* node = path->getTail();
			std::string typeName = node->getTypeId().getName();
			if (!node || node->getTypeId() != SoVRMLTransform::getClassTypeId())
				throw; // should not happen
			Axis1Node = static_cast<SoVRMLTransform *>(node);
		}
		// Axis 2
		searchAction.setName("FREECAD_AXIS2");
		searchAction.setInterest(SoSearchAction::FIRST);
		searchAction.setSearchingAll(FALSE);
		searchAction.apply(pcRobotRoot);
		path = searchAction.getPath();
		if(path){
			SoNode* node = path->getTail();
			std::string typeName = node->getTypeId().getName();
			if (!node || node->getTypeId() != SoVRMLTransform::getClassTypeId())
				throw; // should not happen
			Axis2Node = static_cast<SoVRMLTransform *>(node);
		}
		// Axis 3
		searchAction.setName("FREECAD_AXIS3");
		searchAction.setInterest(SoSearchAction::FIRST);
		searchAction.setSearchingAll(FALSE);
		searchAction.apply(pcRobotRoot);
		path = searchAction.getPath();
		if(path){
			SoNode* node = path->getTail();
			std::string typeName = node->getTypeId().getName();
			if (!node || node->getTypeId() != SoVRMLTransform::getClassTypeId())
				throw; // should not happen
			Axis3Node = static_cast<SoVRMLTransform *>(node);
		}
		// Axis 4
		searchAction.setName("FREECAD_AXIS4");
		searchAction.setInterest(SoSearchAction::FIRST);
		searchAction.setSearchingAll(FALSE);
		searchAction.apply(pcRobotRoot);
		path = searchAction.getPath();
		if(path){
			SoNode* node = path->getTail();
			std::string typeName = node->getTypeId().getName();
			if (!node || node->getTypeId() != SoVRMLTransform::getClassTypeId())
				throw; // should not happen
			Axis4Node = static_cast<SoVRMLTransform *>(node);
		}
		// Axis 5
		searchAction.setName("FREECAD_AXIS5");
		searchAction.setInterest(SoSearchAction::FIRST);
		searchAction.setSearchingAll(FALSE);
		searchAction.apply(pcRobotRoot);
		path = searchAction.getPath();
		if(path){
			SoNode* node = path->getTail();
			std::string typeName = node->getTypeId().getName();
			if (!node || node->getTypeId() != SoVRMLTransform::getClassTypeId())
				throw; // should not happen
			Axis5Node = static_cast<SoVRMLTransform *>(node);
		}
		// Axis 6
		searchAction.setName("FREECAD_AXIS6");
		searchAction.setInterest(SoSearchAction::FIRST);
		searchAction.setSearchingAll(FALSE);
		searchAction.apply(pcRobotRoot);
		path = searchAction.getPath();
		if(path){
			SoNode* node = path->getTail();
			std::string typeName = node->getTypeId().getName();
			if (!node || node->getTypeId() != SoVRMLTransform::getClassTypeId())
				throw; // should not happen
			Axis6Node = static_cast<SoVRMLTransform *>(node);
		}
    }else if (prop == &robObj->Axis1) {
		if(Axis1Node)
			Axis1Node->rotation.setValue(SbVec3f(0.0,1.0,0.0),robObj->Axis1.getValue()*(M_PI/180));
    }else if (prop == &robObj->Axis2) {
		if(Axis2Node)
			Axis2Node->rotation.setValue(SbVec3f(0.0,1.0,0.0),robObj->Axis2.getValue()*(M_PI/180));
    }else if (prop == &robObj->Axis3) {
		if(Axis3Node)
			Axis3Node->rotation.setValue(SbVec3f(0.0,1.0,0.0),robObj->Axis3.getValue()*(M_PI/180));
    }else if (prop == &robObj->Axis4) {
		if(Axis4Node)
			Axis4Node->rotation.setValue(SbVec3f(0.0,1.0,0.0),robObj->Axis4.getValue()*(M_PI/180));
    }else if (prop == &robObj->Axis5) {
		if(Axis5Node)
			Axis5Node->rotation.setValue(SbVec3f(0.0,1.0,0.0),robObj->Axis5.getValue()*(M_PI/180));
    }else if (prop == &robObj->Axis6) {
		if(Axis6Node)
			Axis6Node->rotation.setValue(SbVec3f(0.0,1.0,0.0),robObj->Axis6.getValue()*(M_PI/180));
	}

}
