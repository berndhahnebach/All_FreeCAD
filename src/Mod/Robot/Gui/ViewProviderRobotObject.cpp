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
# include <QFile>
#ifdef FC_OS_WIN32
# include <windows.h>
#endif

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
    Robot::RobotObject* ivObj = static_cast<Robot::RobotObject*>(pcObject);
    if (prop == &ivObj->RobotVrmlFile) {
        // read also from file
        const char* filename = ivObj->RobotVrmlFile.getValue();
        QFile file(QString::fromUtf8(filename));
        SoInput in;
        pcRobotRoot->removeAllChildren();
        if (file.open(QFile::ReadOnly)) {
            QByteArray buffer = file.readAll();
            in.setBuffer((void *)buffer.constData(), buffer.length());
            SoSeparator * node = SoDB::readAll(&in);
            if (node) pcRobotRoot->addChild(node);
        }
    }
}
