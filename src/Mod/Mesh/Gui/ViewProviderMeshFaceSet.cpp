/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <algorithm>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoPolygonOffset.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <QAction>
# include <QMenu>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <Base/Tools2D.h>
#include <Base/ViewProj.h>

#include <App/Document.h>
#include <App/PropertyLinks.h>

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/SoFCSelection.h>
#include <Gui/MainWindow.h>
#include <Gui/MouseModel.h>
#include <Gui/Selection.h>
#include <Gui/WaitCursor.h>
#include <Gui/Window.h>
#include <Gui/Flag.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include <Mod/Mesh/App/Core/Algorithm.h>
#include <Mod/Mesh/App/Core/Evaluation.h>
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/Core/MeshIO.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/Triangulation.h>
#include <Mod/Mesh/App/Core/Visitor.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/MeshProperties.h>

#include "ViewProvider.h"
#include "ViewProviderMeshFaceSet.h"
#include "SoFCMeshObject.h"


using namespace MeshGui;


PROPERTY_SOURCE(MeshGui::ViewProviderMeshFaceSet, MeshGui::ViewProviderMesh)

ViewProviderMeshFaceSet::ViewProviderMeshFaceSet()
{
}

ViewProviderMeshFaceSet::~ViewProviderMeshFaceSet()
{
}

void ViewProviderMeshFaceSet::attach(App::DocumentObject *pcFeat)
{
    ViewProviderGeometryObject::attach(pcFeat);

    pcMeshNode = new SoFCMeshObjectNode;
    pcHighlight->addChild(pcMeshNode);

    pcMeshShape = new SoFCMeshObjectShape;
    pcHighlight->addChild(pcMeshShape);

    // read the threshold from the preferences
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
    int size = hGrp->GetInt("RenderTriangleLimit", -1);
    if (size > 0) pcMeshShape->MaximumTriangles = (unsigned int)(pow(10.0f,size));

    // faces
    SoGroup* pcFlatRoot = new SoGroup();
    //pShapeHints->creaseAngle = F_PI;
    pcFlatRoot->addChild(pShapeHints);
    pcFlatRoot->addChild(pcShapeMaterial);
    pcFlatRoot->addChild(pcMatBinding);
    pcFlatRoot->addChild(pcHighlight);
    addDisplayMaskMode(pcFlatRoot, "Flat");

    // points
    SoGroup* pcPointRoot = new SoGroup();
    pcPointRoot->addChild(pcPointStyle);
    pcPointRoot->addChild(pcFlatRoot);
    addDisplayMaskMode(pcPointRoot, "Point");

    // wires
    SoLightModel* pcLightModel = new SoLightModel();
    pcLightModel->model = SoLightModel::BASE_COLOR;
    SoGroup* pcWireRoot = new SoGroup();
    pcWireRoot->addChild(pcLineStyle);
    pcWireRoot->addChild(pcLightModel);
    pcWireRoot->addChild(pcShapeMaterial);
    pcWireRoot->addChild(pcHighlight);
    addDisplayMaskMode(pcWireRoot, "Wireframe");

    // faces+wires
    // Avoid any Z-buffer artefacts, so that the lines always
    // appear on top of the faces
    SoPolygonOffset* offset = new SoPolygonOffset();
    offset->styles = SoPolygonOffset::LINES;
    offset->factor = -2.0f;
    offset->units = 1.0f;
    SoGroup* pcFlatWireRoot = new SoGroup();
    pcFlatWireRoot->addChild(pcFlatRoot);
    pcFlatWireRoot->addChild(offset);
    pcFlatWireRoot->addChild(pcWireRoot);
    addDisplayMaskMode(pcFlatWireRoot, "FlatWireframe");
}

void ViewProviderMeshFaceSet::updateData(const App::Property* prop)
{
    Gui::ViewProviderGeometryObject::updateData(prop);
    if (prop->getTypeId() == Mesh::PropertyMeshKernel::getClassTypeId()) {
        const Mesh::PropertyMeshKernel* mesh = static_cast<const Mesh::PropertyMeshKernel*>(prop);
        this->pcMeshNode->mesh.setValue(mesh->getValuePtr());
        // Needs to update internal bounding box caches
        this->pcMeshShape->touch();
    }
}

void ViewProviderMeshFaceSet::showOpenEdges(bool show)
{
    if (pcOpenEdge) {
        // remove the node and destroy the data
        pcRoot->removeChild(pcOpenEdge);
        pcOpenEdge = 0;
    }

    if (show) {
        pcOpenEdge = new SoSeparator();
        pcOpenEdge->addChild(pcLineStyle);
        pcOpenEdge->addChild(pOpenColor);

        pcOpenEdge->addChild(pcMeshNode);
        pcOpenEdge->addChild(new SoFCMeshObjectBoundary);

        // add to the highlight node
        pcRoot->addChild(pcOpenEdge);
    }
}

SoShape* ViewProviderMeshFaceSet::getShapeNode() const
{
    return this->pcMeshShape;
}
