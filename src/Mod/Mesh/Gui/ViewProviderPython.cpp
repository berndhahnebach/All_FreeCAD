/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <Inventor/SoPickedPoint.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoIndexedLineSet.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPolygonOffset.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoSeparator.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <Base/Tools.h>
#include <Base/ViewProj.h>

#include <App/Document.h>
#include <App/PropertyLinks.h>

#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/SoFCSelection.h>
#include <Gui/SoFCSelectionAction.h>
#include <Gui/MainWindow.h>
#include <Gui/MouseModel.h>
#include <Gui/Selection.h>
#include <Gui/Window.h>
#include <Gui/WaitCursor.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include <Mod/Mesh/App/Core/Algorithm.h>
#include <Mod/Mesh/App/Core/Evaluation.h>
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/Core/MeshIO.h>
#include <Mod/Mesh/App/Core/Triangulation.h>
#include <Mod/Mesh/App/Core/Visitor.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include "ViewProviderPython.h"
#include "SoFCIndexedFaceSet.h"


using namespace MeshGui;

PROPERTY_SOURCE(MeshGui::ViewProviderPython, Gui::ViewProviderPythonGeometry)


//**************************************************************************
// Construction/Destruction

App::PropertyFloatConstraint::Constraints ViewProviderPython::floatRange = {1.0f,64.0f,1.0f};
App::PropertyFloatConstraint::Constraints ViewProviderPython::angleRange = {0.0f,180.0f,1.0f};
App::PropertyIntegerConstraint::Constraints ViewProviderPython::intPercent = {0,100,1};
const char* ViewProviderPython::LightingEnums[]= {"One side","Two side",NULL};

ViewProviderPython::ViewProviderPython() : pcOpenEdge(0)
{
    ADD_PROPERTY(LineTransparency,(0));
    LineTransparency.setConstraints(&intPercent);
    ADD_PROPERTY(LineWidth,(1.0f));
    LineWidth.setConstraints(&floatRange);
    ADD_PROPERTY(PointSize,(2.0f));
    PointSize.setConstraints(&floatRange);
    ADD_PROPERTY(CreaseAngle,(0.0f));
    CreaseAngle.setConstraints(&angleRange);
    ADD_PROPERTY(OpenEdges,(false));
    ADD_PROPERTY(Lighting,(1));
    Lighting.setEnums(LightingEnums);
    ADD_PROPERTY(LineColor,(0,0,0));

    pOpenColor = new SoBaseColor();
    setOpenEdgeColorFrom(ShapeColor.getValue());
    pOpenColor->ref();

    pcLineStyle = new SoDrawStyle();
    pcLineStyle->ref();
    pcLineStyle->style = SoDrawStyle::LINES;
    pcLineStyle->lineWidth = LineWidth.getValue();

    pcPointStyle = new SoDrawStyle();
    pcPointStyle->ref();
    pcPointStyle->style = SoDrawStyle::POINTS;
    pcPointStyle->pointSize = PointSize.getValue();

    pShapeHints = new SoShapeHints;
    pShapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    pShapeHints->ref();

    pcMatBinding = new SoMaterialBinding;
    pcMatBinding->value = SoMaterialBinding::OVERALL;
    pcMatBinding->ref();

    pLineColor = new SoMaterial;
    pLineColor->ref();
    LineColor.touch();

    // read the correct shape color from the preferences
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");

    // Mesh color
    App::Color color = ShapeColor.getValue();
    unsigned long current = color.getPackedValue();
    unsigned long setting = hGrp->GetUnsigned("MeshColor", current);
    if (current != setting) {
        color.setPackedValue((uint32_t)setting);
        ShapeColor.setValue(color);
    }
    Transparency.setValue(hGrp->GetInt("MeshTransparency", 0));

    // Line color
    color = LineColor.getValue();
    current = color.getPackedValue();
    setting = hGrp->GetUnsigned("LineColor", current);
    if (current != setting) {
        color.setPackedValue((uint32_t)setting);
        LineColor.setValue(color);
    }
    LineTransparency.setValue(hGrp->GetInt("LineTransparency", 0));

    bool twoside = hGrp->GetBool("TwoSideRendering", false);
    if (twoside) Lighting.setValue(1);
    else Lighting.setValue((long)0);

    bool normal_per_vertex = hGrp->GetBool("VertexPerNormals", false);
    if (normal_per_vertex) {
        double angle = hGrp->GetFloat("CreaseAngle", 0.0);
        CreaseAngle.setValue(angle);
    }
}

ViewProviderPython::~ViewProviderPython()
{
    pOpenColor->unref();
    pcLineStyle->unref();
    pcPointStyle->unref();
    pShapeHints->unref();
    pcMatBinding->unref();
    pLineColor->unref();
}

void ViewProviderPython::onChanged(const App::Property* prop)
{
    if (prop == &LineTransparency) {
        float trans = LineTransparency.getValue()/100.0f;
        pLineColor->transparency = trans;
    }
    else if (prop == &LineWidth) {
        pcLineStyle->lineWidth = LineWidth.getValue();
    }
    else if (prop == &PointSize) {
        pcPointStyle->pointSize = PointSize.getValue();
    }
    else if (prop == &CreaseAngle) {
        pShapeHints->creaseAngle = (F_PI*CreaseAngle.getValue())/180.0;
    }
    else if (prop == &OpenEdges) {
        showOpenEdges(OpenEdges.getValue());
    }
    else if (prop == &Lighting) {
        if (Lighting.getValue() == 0) {
            pShapeHints->vertexOrdering = SoShapeHints::UNKNOWN_ORDERING;
        }
        else {
            pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
        }
    }
    else if (prop == &LineColor) {
        const App::Color& c = LineColor.getValue();
        pLineColor->diffuseColor.setValue(c.r,c.g,c.b);
    }
    else {
        // Set the inverse color for open edges
        if (prop == &ShapeColor) {
            setOpenEdgeColorFrom(ShapeColor.getValue());
        }
        else if (prop == &ShapeMaterial) {
            setOpenEdgeColorFrom(ShapeMaterial.getValue().diffuseColor);
        }
        ViewProviderPythonGeometry::onChanged(prop);
    }
}

void ViewProviderPython::attach(App::DocumentObject *pcFeat)
{
    // call parent attach method
    ViewProviderPythonGeometry::attach(pcFeat);

    // faces
    SoGroup* pcFlatRoot = new SoGroup();
    pcFlatRoot->addChild(pShapeHints);
    pcFlatRoot->addChild(pcShapeMaterial);
    pcFlatRoot->addChild(pcMatBinding);
    pcFlatRoot->addChild(pcHighlight);
    addDisplayMaskMode(pcFlatRoot, "Shaded");

    // points
    SoGroup* pcPointRoot = new SoGroup();
    pcPointRoot->addChild(pcPointStyle);
    pcPointRoot->addChild(pcFlatRoot);
    addDisplayMaskMode(pcPointRoot, "Points");

    // wires
    SoLightModel* pcLightModel = new SoLightModel();
    pcLightModel->model = SoLightModel::BASE_COLOR;
    SoGroup* pcWireRoot = new SoGroup();
    pcWireRoot->addChild(pcLineStyle);
    pcWireRoot->addChild(pcLightModel);
    //pcWireRoot->addChild(pcShapeMaterial);
    pcWireRoot->addChild(pLineColor);
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
    addDisplayMaskMode(pcFlatWireRoot, "Flat Lines");

    pcMeshCoord = new SoCoordinate3;
    pcHighlight->addChild(pcMeshCoord);

    pcMeshFaces = new SoFCIndexedFaceSet;
    pcHighlight->addChild(pcMeshFaces);

    // read the threshold from the preferences
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
    int size = hGrp->GetInt("RenderTriangleLimit", -1);
    if (size > 0) static_cast<SoFCIndexedFaceSet*>(pcMeshFaces)->renderTriangleLimit = (unsigned int)(pow(10.0f,size));
}

void ViewProviderPython::setDisplayMode(const char* ModeName)
{
    if (strcmp("Shaded",ModeName)==0)
        setDisplayMaskMode("Shaded");
    else if (strcmp("Points",ModeName)==0)
        setDisplayMaskMode("Points");
    else if (strcmp("Flat Lines",ModeName)==0)
        setDisplayMaskMode("Flat Lines");
    else if (strcmp("Wireframe",ModeName)==0)
        setDisplayMaskMode("Wireframe");

    ViewProviderPythonGeometry::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderPython::getDisplayModes(void) const
{
    // get the modes of the father
    std::vector<std::string> StrList = ViewProviderPythonGeometry::getDisplayModes();

    // add your own modes
    StrList.push_back("Shaded");
    StrList.push_back("Wireframe");
    StrList.push_back("Flat Lines");
    StrList.push_back("Points");

    return StrList;
}

void ViewProviderPython::updateData(const App::Property* prop)
{
    Gui::ViewProviderPythonGeometry::updateData(prop);
    if (prop->getTypeId() == Mesh::PropertyMeshKernel::getClassTypeId()) {
        const Mesh::PropertyMeshKernel* mesh = static_cast<const Mesh::PropertyMeshKernel*>(prop);
        createMesh(mesh->getValue().getKernel());
        showOpenEdges(OpenEdges.getValue());
    }
}

void ViewProviderPython::createMesh(const MeshCore::MeshKernel& rcMesh)
{
    // set the point coordinates
    const MeshCore::MeshPointArray& cP = rcMesh.GetPoints();
    pcMeshCoord->point.setNum(rcMesh.CountPoints());
    SbVec3f* verts = pcMeshCoord->point.startEditing();
    unsigned long i=0;
    for (MeshCore::MeshPointArray::_TConstIterator it = cP.begin(); it != cP.end(); ++it, i++) {
        verts[i].setValue(it->x, it->y, it->z);
    }
    pcMeshCoord->point.finishEditing();

    // set the face indices
    unsigned long j=0;
    const MeshCore::MeshFacetArray& cF = rcMesh.GetFacets();
    pcMeshFaces->coordIndex.setNum(4*rcMesh.CountFacets());
    int32_t* indices = pcMeshFaces->coordIndex.startEditing();
    for (MeshCore::MeshFacetArray::_TConstIterator it = cF.begin(); it != cF.end(); ++it, j++) {
        for (int i=0; i<3; i++) {
            indices[4*j+i] = it->_aulPoints[i];
        }
        indices[4*j+3] = SO_END_FACE_INDEX;
    }
    pcMeshFaces->coordIndex.finishEditing();
}

void ViewProviderPython::showOpenEdges(bool show)
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

        pcOpenEdge->addChild(pcMeshCoord);
        SoIndexedLineSet* lines = new SoIndexedLineSet;
        pcOpenEdge->addChild(lines);

        // add to the highlight node
        pcRoot->addChild(pcOpenEdge);

        // Build up the lines with indices to the list of vertices 'pcMeshCoord'
        int index=0;
        const MeshCore::MeshKernel& rMesh = static_cast<Mesh::Feature*>(pcObject)->Mesh.getValue().getKernel();
        const MeshCore::MeshFacetArray& rFaces = rMesh.GetFacets();
        for (MeshCore::MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it) {
            for (int i=0; i<3; i++) {
                if (it->_aulNeighbours[i] == ULONG_MAX) {
                    lines->coordIndex.set1Value(index++,it->_aulPoints[i]);
                    lines->coordIndex.set1Value(index++,it->_aulPoints[(i+1)%3]);
                    lines->coordIndex.set1Value(index++,SO_END_LINE_INDEX);
                }
            }
        }
    }
}

void ViewProviderPython::setOpenEdgeColorFrom(const App::Color& c)
{
    float r=1.0f-c.r; r = r < 0.5f ? 0.0f : 1.0f;
    float g=1.0f-c.g; g = g < 0.5f ? 0.0f : 1.0f;
    float b=1.0f-c.b; b = b < 0.5f ? 0.0f : 1.0f;
    pOpenColor->rgb.setValue(r, g, b);
}
