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
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoMarkerSet.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <QFile>
#endif

#include "ViewProviderFemMesh.h"

#include <Mod/Fem/App/FemMeshObject.h>
#include <Mod/Fem/App/FemMesh.h>
#include <App/Document.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>
#include <Base/Console.h>
#include <sstream>
using namespace Gui;
using namespace FemGui;
using namespace Fem;

PROPERTY_SOURCE(FemGui::ViewProviderFemMesh, Gui::ViewProviderGeometryObject)

ViewProviderFemMesh::ViewProviderFemMesh()
{
    
	pcFemMeshRoot = new Gui::SoFCSelection();
    pcFemMeshRoot->highlightMode = Gui::SoFCSelection::OFF;
    pcFemMeshRoot->selectionMode = Gui::SoFCSelection::SEL_OFF;
    //pcFemRoot->style = Gui::SoFCSelection::BOX;
    pcFemMeshRoot->ref();

    pcCoords = new SoCoordinate3();
    pcCoords->ref();
    pcDrawStyle = new SoDrawStyle();
    pcDrawStyle->ref();
    pcDrawStyle->style = SoDrawStyle::LINES;
    pcDrawStyle->lineWidth = 2;

    pcLines = new SoLineSet;
    pcLines->ref();


}

ViewProviderFemMesh::~ViewProviderFemMesh()
{
    pcFemMeshRoot->unref();
    pcCoords->unref();
    pcDrawStyle->unref();
    pcLines->unref();

}

void ViewProviderFemMesh::attach(App::DocumentObject *pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);

    // Draw trajectory lines
    SoSeparator* linesep = new SoSeparator;
    SoBaseColor * basecol = new SoBaseColor;
    basecol->rgb.setValue( 1.0f, 0.5f, 0.0f );
    linesep->addChild(basecol);
    linesep->addChild(pcCoords);
    linesep->addChild(pcLines);

    // Draw markers
    SoBaseColor * markcol = new SoBaseColor;
    markcol->rgb.setValue( 1.0f, 1.0f, 0.0f );
    SoMarkerSet* marker = new SoMarkerSet;
    marker->markerIndex=SoMarkerSet::CROSS_5_5;
    linesep->addChild(markcol);
    linesep->addChild(marker);

    pcFemMeshRoot->addChild(linesep);

    addDisplayMaskMode(pcFemMeshRoot, "Mesh");
    pcFemMeshRoot->objectName = pcObj->getNameInDocument();
    pcFemMeshRoot->documentName = pcObj->getDocument()->getName();
    pcFemMeshRoot->subElementName = "Main";

}

void ViewProviderFemMesh::setDisplayMode(const char* ModeName)
{
    if ( strcmp("Mesh",ModeName)==0 )
        setDisplayMaskMode("Mesh");
    ViewProviderGeometryObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderFemMesh::getDisplayModes(void) const
{
    std::vector<std::string> StrList;
    StrList.push_back("Mesh");
    return StrList;
}

void ViewProviderFemMesh::updateData(const App::Property* prop)
{
 //   Fem::FemMeshObject* pcTracObj = static_cast<Fem::FemMeshObject*>(pcObject);
 //   if (prop == &pcTracObj->FemMesh) {
 //       const FemMesh &trak = pcTracObj->FemMesh.getValue();

 //       pcCoords->point.deleteValues(0);
 //       pcCoords->point.setNum(trak.getSize());

 //       for(unsigned int i=0;i<trak.getSize();++i){
 //           Base::Vector3d pos = trak.getWaypoint(i).EndPos.getPosition();
 //           pcCoords->point.set1Value(i,pos.x,pos.y,pos.z);

 //       }
 //       pcLines->numVertices.set1Value(0, trak.getSize());
	//}else if (prop == &pcTracObj->Base) {
 //       Base::Placement loc = *(&pcTracObj->Base.getValue());
 //   }

}

