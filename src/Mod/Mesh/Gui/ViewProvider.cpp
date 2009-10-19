/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QMenu>
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
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/SoFCSelection.h>
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

#include "ViewProvider.h"
#include "SoFCIndexedFaceSet.h"
#include "SoFCMeshObject.h"


using namespace MeshGui;

using Mesh::Feature;
using MeshCore::MeshKernel;
using MeshCore::MeshPointIterator;
using MeshCore::MeshFacetIterator;
using MeshCore::MeshGeomFacet;
using MeshCore::MeshFacet;

PROPERTY_SOURCE(MeshGui::ViewProviderExport, Gui::ViewProviderDocumentObject)

ViewProviderExport::ViewProviderExport()
{
}

ViewProviderExport::~ViewProviderExport()
{
}

std::vector<std::string> ViewProviderExport::getDisplayModes(void) const
{
    std::vector<std::string> mode;
    mode.push_back("");
    return mode;
}

const char* ViewProviderExport::getDefaultDisplayMode() const
{
    return "";
}

QIcon ViewProviderExport::getIcon() const
{
    const char * Mesh_Feature_xpm[] = {
        "22 22 6 1",
        ". c None",
        "# c #000000",
        "c c #ffff00",
        "a c #808080",
        "b c #c0c0c0",
        "f c #008000",
        ".............##.......",
        ".............###......",
        ".............#f##.....",
        ".#....####...#ff##....",
        ".##.##....#..#fff##...",
        ".###.........#ffff##..",
        ".####........#fffff##.",
        ".#####.......#ffffff##",
        ".............#########",
        ".####.................",
        "#abab##########.......",
        "#babababababab#.......",
        "#ababababababa#.......",
        "#babab################",
        "#abab##cccccccccccc##.",
        "#bab##cccccccccccc##..",
        "#ab##cccccccccccc##...",
        "#b##cccccccccccc##....",
        "###cccccccccccc##.....",
        "##cccccccccccc##......",
        "###############.......",
        "......................"};
    QPixmap px(Mesh_Feature_xpm);
    return px;
}

// ------------------------------------------------------

App::PropertyFloatConstraint::Constraints ViewProviderMesh::floatRange = {1.0f,64.0f,1.0f};
App::PropertyFloatConstraint::Constraints ViewProviderMesh::angleRange = {0.0f,180.0f,1.0f};
App::PropertyIntegerConstraint::Constraints ViewProviderMesh::intPercent = {0,100,1};
const char* ViewProviderMesh::LightingEnums[]= {"One side","Two side",NULL};

PROPERTY_SOURCE(MeshGui::ViewProviderMesh, Gui::ViewProviderGeometryObject)

ViewProviderMesh::ViewProviderMesh() : pcOpenEdge(0), m_bEdit(false)
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
    ADD_PROPERTY(LineColor,(ShapeColor.getValue()));
    //ADD_PROPERTY(BacksideColor,(0.8f,0.8f,0.8f));

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

    //pBackHints = new SoShapeHints;
    //pBackHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    //pBackHints->ref();

    pcMatBinding = new SoMaterialBinding;
    pcMatBinding->value = SoMaterialBinding::OVERALL;
    pcMatBinding->ref();

    pLineColor = new SoMaterial;
    pLineColor->ref();
    LineColor.touch();

    //pcBacksideMaterial = new SoMaterial;
    //pcBacksideMaterial->ref();
    //BacksideColor.touch();

    // read the correct shape color from the preferences
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
    App::Color color = ShapeColor.getValue();
    unsigned long current = color.getPackedValue();
    unsigned long setting = hGrp->GetUnsigned("MeshColor", current);
    if (current != setting) {
        color.setPackedValue((uint32_t)setting);
        ShapeColor.setValue(color);
    }

    bool twoside = hGrp->GetBool("TwoSideRendering", false);
    if (twoside) Lighting.setValue(1);
    else Lighting.setValue((long)0);

    //color = BacksideColor.getValue();
    //current = color.getPackedValue();
    //setting = hGrp->GetUnsigned("BackfaceColor", current);
    //if (current != setting) {
    //    color.setPackedValue((uint32_t)setting);
    //    BacksideColor.setValue(color);
    //}

    bool normal_per_vertex = hGrp->GetBool("VertexPerNormals", false);
    if (normal_per_vertex) {
        double angle = hGrp->GetFloat("CreaseAngle", 0.0);
        CreaseAngle.setValue(angle);
    }
}

ViewProviderMesh::~ViewProviderMesh()
{
    pOpenColor->unref();
    pcLineStyle->unref();
    pcPointStyle->unref();
    pShapeHints->unref();
    //pBackHints->unref();
    pcMatBinding->unref();
    pLineColor->unref();
    //pcBacksideMaterial->unref();
}

void ViewProviderMesh::onChanged(const App::Property* prop)
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
        //pBackHints->creaseAngle = (F_PI*CreaseAngle.getValue())/180.0;
    }
    else if (prop == &OpenEdges) {
        showOpenEdges(OpenEdges.getValue());
    }
    else if (prop == &Lighting) {
        if (Lighting.getValue() == 0) {
            pShapeHints->vertexOrdering = SoShapeHints::UNKNOWN_ORDERING;
            //pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
            //pcBacksideMaterial->diffuseColor = pcShapeMaterial->diffuseColor;
        }
        else {
            pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
        //    const App::Color& c = BacksideColor.getValue();
        //    pcBacksideMaterial->diffuseColor.setValue(c.r,c.g,c.b);
        }
    }
    else if (prop == &LineColor) {
        const App::Color& c = LineColor.getValue();
        pLineColor->diffuseColor.setValue(c.r,c.g,c.b);
    }
    /*else if (prop == &BacksideColor && Lighting.getValue() != 0) {
        const App::Color& c = BacksideColor.getValue();
        pcBacksideMaterial->diffuseColor.setValue(c.r,c.g,c.b);
    }*/
    else {
        // Set the inverse color for open edges
        if (prop == &ShapeColor) {
            setOpenEdgeColorFrom(ShapeColor.getValue());
        }
        else if (prop == &ShapeMaterial) {
            setOpenEdgeColorFrom(ShapeMaterial.getValue().diffuseColor);
        }
        ViewProviderGeometryObject::onChanged(prop);
    }
}

void ViewProviderMesh::setOpenEdgeColorFrom(const App::Color& c)
{
    float r=1.0f-c.r; r = r < 0.5f ? 0.0f : 1.0f;
    float g=1.0f-c.g; g = g < 0.5f ? 0.0f : 1.0f;
    float b=1.0f-c.b; b = b < 0.5f ? 0.0f : 1.0f;
    pOpenColor->rgb.setValue(r, g, b);
}

SoShape* ViewProviderMesh::getShapeNode() const
{
    return 0;
}

/** 
 * Extracts the mesh data from the feature \a pcFeature and creates
 * an Inventor node \a SoNode with these data. 
 */
void ViewProviderMesh::attach(App::DocumentObject *pcFeat)
{
    ViewProviderGeometryObject::attach(pcFeat);

    // faces
    SoGroup* pcFlatRoot = new SoGroup();
    /*
    {
        // front face
        SoSeparator* sep = new SoSeparator();
        pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
        pShapeHints->shapeType = SoShapeHints::SOLID;
        sep->addChild(pShapeHints);
        sep->addChild(pcShapeMaterial);
        sep->addChild(pcMatBinding);
        sep->addChild(pcHighlight);
        pcFlatRoot->addChild(sep);
    }
    {
        // back face
        SoSeparator* sep = new SoSeparator();
        pBackHints->vertexOrdering = SoShapeHints::CLOCKWISE;
        pBackHints->shapeType = SoShapeHints::SOLID;
        sep->addChild(pBackHints);
        sep->addChild(pcBacksideMaterial);
        sep->addChild(pcHighlight);
        pcFlatRoot->addChild(sep);
    }
    */
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
    addDisplayMaskMode(pcFlatWireRoot, "FlatWireframe");

    // Turns on backface culling
    //SoShapeHints * wirehints = new SoShapeHints;
    //wirehints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
    //wirehints->shapeType = SoShapeHints::SOLID;
    //wirehints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
    //SoGroup* pcHiddenLineRoot = new SoGroup();
    //pcHiddenLineRoot->addChild(wirehints);
    //pcHiddenLineRoot->addChild(pcWireRoot);
    //addDisplayMaskMode(pcHiddenLineRoot, "HiddenLine");
}

QIcon ViewProviderMesh::getIcon() const
{
#if 1
    static QIcon icon = Gui::BitmapFactory().pixmap("Tree_Mesh");
    return icon;
#else
    static const char * const Mesh_Feature_xpm[] = {
        "16 16 4 1",
        ".	c None",
        "#	c #000000",
        "s	c #BEC2FC",
        "g	c #00FF00",
        ".......##.......",
        "....#######.....",
        "..##ggg#ggg#....",
        "##ggggg#gggg##..",
        "#g#ggg#gggggg##.",
        "#gg#gg#gggg###s.",
        "#gg#gg#gg##gg#s.",
        "#ggg#####ggg#ss.",
        "#gggg##gggg#ss..",
        ".#g##g#gggg#s...",
        ".##ggg#ggg#ss...",
        ".##gggg#g#ss....",
        "..s#####g#s.....",
        "....sss##ss.....",
        "........ss......",
        "................"};
    QPixmap px(Mesh_Feature_xpm);
    return px;
#endif
}

void ViewProviderMesh::setDisplayMode(const char* ModeName)
{
    if (strcmp("Shaded",ModeName)==0)
        setDisplayMaskMode("Flat");
    else if (strcmp("Points",ModeName)==0)
        setDisplayMaskMode("Point");
    else if (strcmp("Flat Lines",ModeName)==0)
        setDisplayMaskMode("FlatWireframe");
    else if (strcmp("Wireframe",ModeName)==0)
        setDisplayMaskMode("Wireframe");

    ViewProviderGeometryObject::setDisplayMode(ModeName);
}

std::vector<std::string> ViewProviderMesh::getDisplayModes(void) const
{
    std::vector<std::string> StrList;

    // add your own modes
    StrList.push_back("Shaded");
    StrList.push_back("Wireframe");
    StrList.push_back("Flat Lines");
    StrList.push_back("Points");

    return StrList;
}

bool ViewProviderMesh::setEdit(int ModNum)
{
    if (m_bEdit) return true;
    m_bEdit = true;
    return true;
}

void ViewProviderMesh::unsetEdit(void)
{
    m_bEdit = false;
}

const char* ViewProviderMesh::getEditModeName(void)
{
    return "Polygon picking";
}

bool ViewProviderMesh::createToolMesh(const std::vector<SbVec2f>& rclPoly, const SbViewVolume& vol,
                                      const Base::Vector3f& rcNormal, std::vector<MeshCore::MeshGeomFacet>& aFaces)
{
    float fX, fY, fZ;
    SbVec3f pt1, pt2, pt3, pt4;
    MeshGeomFacet face;
    std::vector<Base::Vector3f> top, bottom, polygon;

    for (std::vector<SbVec2f>::const_iterator it = rclPoly.begin(); it != rclPoly.end(); ++it) {
        // the following element
        std::vector<SbVec2f>::const_iterator nt = it + 1;
        if (nt == rclPoly.end())
            nt = rclPoly.begin();
        else if (*it == *nt)
            continue; // two adjacent vertices are equal

        vol.projectPointToLine(*it, pt1, pt2);
        vol.projectPointToLine(*nt, pt3, pt4);

        // 1st facet
        pt1.getValue(fX, fY, fZ);
        face._aclPoints[0].Set(fX, fY, fZ);
        pt4.getValue(fX, fY, fZ);
        face._aclPoints[1].Set(fX, fY, fZ);
        pt3.getValue(fX, fY, fZ);
        face._aclPoints[2].Set(fX, fY, fZ);
        if (face.Area() > 0)
            aFaces.push_back(face);

        // 2nd facet
        pt1.getValue(fX, fY, fZ);
        face._aclPoints[0].Set(fX, fY, fZ);
        pt2.getValue(fX, fY, fZ);
        face._aclPoints[1].Set(fX, fY, fZ);
        pt4.getValue(fX, fY, fZ);
        face._aclPoints[2].Set(fX, fY, fZ);
        if (face.Area() > 0)
            aFaces.push_back(face);

        if (it+1 < rclPoly.end()) {
            pt1.getValue(fX, fY, fZ);
            top.push_back( Base::Vector3f(fX, fY, fZ) );
            pt2.getValue(fX, fY, fZ);
            bottom.push_back( Base::Vector3f(fX, fY, fZ) );
            // polygon we need to triangulate (in x,y-plane)
            it->getValue(fX, fY);
            polygon.push_back( Base::Vector3f(fX, fY, 0.0f) );
        }
    }

    // now create the lids
    std::vector<MeshGeomFacet> aLid;
    MeshCore::EarClippingTriangulator cTria;
    cTria.SetPolygon(polygon);
    bool ok = cTria.TriangulatePolygon();
  
    std::vector<MeshFacet> faces = cTria.GetFacets();
    for (std::vector<MeshFacet>::iterator itF = faces.begin(); itF != faces.end(); ++itF) {
        MeshGeomFacet topFacet;
        topFacet._aclPoints[0] = top[itF->_aulPoints[0]];
        topFacet._aclPoints[1] = top[itF->_aulPoints[1]];
        topFacet._aclPoints[2] = top[itF->_aulPoints[2]];
        if (topFacet.GetNormal() * rcNormal < 0) {
            std::swap(topFacet._aclPoints[1], topFacet._aclPoints[2]);
            topFacet.CalcNormal();
        }
        aFaces.push_back(topFacet);

        MeshGeomFacet botFacet;
        botFacet._aclPoints[0] = bottom[itF->_aulPoints[0]];
        botFacet._aclPoints[1] = bottom[itF->_aulPoints[1]];
        botFacet._aclPoints[2] = bottom[itF->_aulPoints[2]];
        if (botFacet.GetNormal() * rcNormal > 0) {
            std::swap(botFacet._aclPoints[1], botFacet._aclPoints[2]);
            botFacet.CalcNormal();
        }
        aFaces.push_back(botFacet);
    }

    return ok;
}

void ViewProviderMesh::showOpenEdges(bool show)
{
}

void ViewProviderMesh::clipMeshCallback(void * ud, SoEventCallback * n)
{
    // show the wait cursor because this could take quite some time
    Gui::WaitCursor wc;

    // When this callback function is invoked we must in either case leave the edit mode
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
    view->setEditing(false);
    view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), clipMeshCallback,ud);
    n->setHandled();

    SbBool clip_inner;
    std::vector<SbVec2f> clPoly = view->getPickedPolygon(&clip_inner);
    if (clPoly.size() < 3)
        return;
    if (clPoly.front() != clPoly.back())
        clPoly.push_back(clPoly.front());

    std::vector<Gui::ViewProvider*> views = view->getViewProvidersOfType(ViewProviderMesh::getClassTypeId());
    for (std::vector<Gui::ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
        ViewProviderMesh* that = static_cast<ViewProviderMesh*>(*it);
        if (that->m_bEdit) {
            that->unsetEdit();
            that->cutMesh(clPoly, *view, clip_inner);
        }
    }

    view->render();
}

void ViewProviderMesh::partMeshCallback(void * ud, SoEventCallback * cb)
{
    // show the wait cursor because this could take quite some time
    Gui::WaitCursor wc;

    // When this callback function is invoked we must in either case leave the edit mode
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(cb->getUserData());
    view->setEditing(false);
    view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), partMeshCallback,ud);
    cb->setHandled();

    SbBool clip_inner;
    std::vector<SbVec2f> clPoly = view->getPickedPolygon(&clip_inner);
    if (clPoly.size() < 3)
        return;
    if (clPoly.front() != clPoly.back())
        clPoly.push_back(clPoly.front());

    // get the normal of the front clipping plane
    SbVec3f b,n;
    view->getNearPlane(b, n);
    Base::Vector3f cPoint(b[0],b[1],b[2]), cNormal(n[0],n[1],n[2]);
    SoCamera* pCam = view->getCamera();  
    SbViewVolume  vol = pCam->getViewVolume(); 

    // create a tool shape from these points
    std::vector<MeshCore::MeshGeomFacet> aFaces;
    if (!ViewProviderMesh::createToolMesh(clPoly, vol, cNormal, aFaces))
        Base::Console().Message("The picked polygon seems to have self-overlappings. This could lead to strange results.");

    MeshCore::MeshKernel toolMesh;
    bool locked = Base::Sequencer().setLocked(true);
    toolMesh = aFaces;
    Base::Sequencer().setLocked(locked);

    // Open a transaction object for the undo/redo stuff
    Gui::Application::Instance->activeDocument()->openCommand("Split");

    try {
        std::vector<Gui::ViewProvider*> views = view->getViewProvidersOfType(ViewProviderMesh::getClassTypeId());
        for (std::vector<Gui::ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
            ViewProviderMesh* that = static_cast<ViewProviderMesh*>(*it);
            if (that->m_bEdit) {
                that->unsetEdit();
                that->splitMesh(toolMesh, cNormal, clip_inner);
            }
        }
    }
    catch(...) {
        // Don't rethrow any exception
    }

    // Close the transaction
    Gui::Application::Instance->activeDocument()->commitCommand();
    view->render();
}

void ViewProviderMesh::segmMeshCallback(void * ud, SoEventCallback * cb)
{
    // show the wait cursor because this could take quite some time
    Gui::WaitCursor wc;

    // When this callback function is invoked we must in either case leave the edit mode
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(cb->getUserData());
    view->setEditing(false);
    view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), segmMeshCallback,ud);
    cb->setHandled();

    SbBool clip_inner;
    std::vector<SbVec2f> clPoly = view->getPickedPolygon(&clip_inner);
    if (clPoly.size() < 3)
        return;
    if (clPoly.front() != clPoly.back())
        clPoly.push_back(clPoly.front());

    // get the normal of the front clipping plane
    SbVec3f b,n;
    view->getNearPlane(b, n);
    Base::Vector3f cPoint(b[0],b[1],b[2]), cNormal(n[0],n[1],n[2]);
    SoCamera* pCam = view->getCamera();  
    SbViewVolume  vol = pCam->getViewVolume(); 

    // create a tool shape from these points
    std::vector<MeshCore::MeshGeomFacet> aFaces;
    if (!ViewProviderMesh::createToolMesh(clPoly, vol, cNormal, aFaces))
        Base::Console().Message("The picked polygon seems to have self-overlappings. This could lead to strange results.");

    MeshCore::MeshKernel toolMesh;
    bool locked = Base::Sequencer().setLocked(true);
    toolMesh = aFaces;
    Base::Sequencer().setLocked(locked);

    // Open a transaction object for the undo/redo stuff
    Gui::Application::Instance->activeDocument()->openCommand("Segment");

    try {
        std::vector<Gui::ViewProvider*> views = view->getViewProvidersOfType(ViewProviderMesh::getClassTypeId());
        for (std::vector<Gui::ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
            ViewProviderMesh* that = static_cast<ViewProviderMesh*>(*it);
            if (that->m_bEdit) {
                that->unsetEdit();
                that->segmentMesh(toolMesh, cNormal, clip_inner);
            }
        }
    }
    catch(...) {
        // Don't rethrow any exception
    }

    // Close the transaction
    Gui::Application::Instance->activeDocument()->commitCommand();
    view->render();
}

namespace MeshGui {
template <class T>
struct iotaGen {
public:
    T operator()() { return n++; }
    iotaGen(T v) : n(v) {}

private:
    T n;
};
}

void ViewProviderMesh::getFacetsFromPolygon(const std::vector<SbVec2f>& picked,
                                            Gui::View3DInventorViewer &Viewer,
                                            SbBool inner,
                                            std::vector<unsigned long>& indices) const
{
    // get the normal of the front clipping plane
    SbVec3f b,n;
    Viewer.getNearPlane(b, n);
    Base::Vector3f cPoint(b[0],b[1],b[2]), cNormal(n[0],n[1],n[2]);
    SoCamera* pCam = Viewer.getCamera();  
    SbViewVolume  vol = pCam->getViewVolume(); 

    // create a tool shape from these points
    std::vector<MeshCore::MeshGeomFacet> aFaces;
    bool ok = ViewProviderMesh::createToolMesh(picked, vol, cNormal, aFaces);

    // Get the attached mesh property
    Mesh::PropertyMeshKernel& meshProp = static_cast<Mesh::Feature*>(pcObject)->Mesh;

    // Get the facet indices inside the tool mesh
    MeshCore::MeshKernel cToolMesh;
    bool locked = Base::Sequencer().setLocked(true);
    cToolMesh = aFaces;
    Base::Sequencer().setLocked(locked);
    MeshCore::MeshFacetGrid cGrid(meshProp.getValue().getKernel());
    MeshCore::MeshAlgorithm cAlg(meshProp.getValue().getKernel());
    cAlg.GetFacetsFromToolMesh(cToolMesh, cNormal, cGrid, indices);
    if (!inner) {
        // get the indices that are completely outside
        std::vector<unsigned long> complete(meshProp.getValue().countFacets());
        std::generate(complete.begin(), complete.end(), iotaGen<unsigned long>(0));
        std::sort(indices.begin(), indices.end());
        std::vector<unsigned long> complementary;
        std::back_insert_iterator<std::vector<unsigned long> > biit(complementary);
        std::set_difference(complete.begin(), complete.end(), indices.begin(), indices.end(), biit);
        indices = complementary;
    }

    if (!ok) // note: the mouse grabbing needs to be released
        Base::Console().Message("The picked polygon seems to have self-overlappings. This could lead to strange results.");
}

void ViewProviderMesh::cutMesh(const std::vector<SbVec2f>& picked, 
                               Gui::View3DInventorViewer &Viewer, SbBool inner)
{
    // Get the facet indices inside the tool mesh
    std::vector<unsigned long> indices;
    getFacetsFromPolygon(picked, Viewer, inner, indices);

    // Get the attached mesh property
    Mesh::PropertyMeshKernel& meshProp = static_cast<Mesh::Feature*>(pcObject)->Mesh;

    //Remove the facets from the mesh and open a transaction object for the undo/redo stuff
    Gui::Application::Instance->activeDocument()->openCommand("Cut");
    meshProp.deleteFacetIndices( indices );
    Gui::Application::Instance->activeDocument()->commitCommand();
    ((Mesh::Feature*)pcObject)->purgeTouched();
}

void ViewProviderMesh::splitMesh(const MeshCore::MeshKernel& toolMesh, const Base::Vector3f& normal, SbBool clip_inner)
{
    // Get the attached mesh property
    Mesh::PropertyMeshKernel& meshProp = static_cast<Mesh::Feature*>(pcObject)->Mesh;
    const MeshCore::MeshKernel& meshPropKernel = meshProp.getValue().getKernel();

    // Get the facet indices inside the tool mesh
    std::vector<unsigned long> indices;
    MeshCore::MeshFacetGrid cGrid(meshPropKernel);
    MeshCore::MeshAlgorithm cAlg(meshPropKernel);
    cAlg.GetFacetsFromToolMesh(toolMesh, normal, cGrid, indices);
    if (!clip_inner) {
        // get the indices that are completely outside
        std::vector<unsigned long> complete(meshPropKernel.CountFacets());
        std::generate(complete.begin(), complete.end(), iotaGen<unsigned long>(0));
        std::sort(indices.begin(), indices.end());
        std::vector<unsigned long> complementary;
        std::back_insert_iterator<std::vector<unsigned long> > biit(complementary);
        std::set_difference(complete.begin(), complete.end(), indices.begin(), indices.end(), biit);
        indices = complementary;
    }

    // Remove the facets from the mesh and create a new one
    Mesh::MeshObject* kernel = meshProp.getValue().meshFromSegment(indices);
    meshProp.deleteFacetIndices(indices);
    Mesh::Feature* splitMesh = static_cast<Mesh::Feature*>(App::GetApplication().getActiveDocument()
        ->addObject("Mesh::Feature",pcObject->getNameInDocument()));
    // Note: deletes also kernel
    splitMesh->Mesh.setValuePtr(kernel);
    static_cast<Mesh::Feature*>(pcObject)->purgeTouched();
}

void ViewProviderMesh::segmentMesh(const MeshCore::MeshKernel& toolMesh, const Base::Vector3f& normal, SbBool clip_inner)
{
    // Get the attached mesh property
    Mesh::PropertyMeshKernel& meshProp = static_cast<Mesh::Feature*>(pcObject)->Mesh;
    const MeshCore::MeshKernel& meshPropKernel = meshProp.getValue().getKernel();

    // Get the facet indices inside the tool mesh
    std::vector<unsigned long> indices;
    MeshCore::MeshFacetGrid cGrid(meshPropKernel);
    MeshCore::MeshAlgorithm cAlg(meshPropKernel);
    cAlg.GetFacetsFromToolMesh(toolMesh, normal, cGrid, indices);
    if (!clip_inner) {
        // get the indices that are completely outside
        std::vector<unsigned long> complete(meshPropKernel.CountFacets());
        std::generate(complete.begin(), complete.end(), iotaGen<unsigned long>(0));
        std::sort(indices.begin(), indices.end());
        std::vector<unsigned long> complementary;
        std::back_insert_iterator<std::vector<unsigned long> > biit(complementary);
        std::set_difference(complete.begin(), complete.end(), indices.begin(), indices.end(), biit);
        indices = complementary;
    }

    meshProp.createSegment(indices);
    static_cast<Mesh::Feature*>(pcObject)->purgeTouched();
}

void ViewProviderMesh::faceInfoCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
        n->setHandled();
        // context-menu
        QMenu menu;
        QAction* cl = menu.addAction(QObject::tr("Leave info mode"));
        QAction* id = menu.exec(QCursor::pos());
        if (cl == id) {
            view->setEditing(false);
            view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
            view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), faceInfoCallback,ud);
        }
    }
    else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL) {
            Base::Console().Message("No facet picked.\n");
            return;
        }

        n->setHandled();

        // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
        // really from the mesh we render and not from any other geometry
        Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
        if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMesh::getClassTypeId()))
            return;
        ViewProviderMesh* that = static_cast<ViewProviderMesh*>(vp);
        const SoDetail* detail = point->getDetail(that->getShapeNode());
        if ( detail && detail->getTypeId() == SoFaceDetail::getClassTypeId() ) {
            // get the boundary to the picked facet
            unsigned long uFacet = ((SoFaceDetail*)detail)->getFaceIndex();
            that->faceInfo(uFacet);
            Gui::Flag* flag = new Gui::Flag;
            flag->setText(QObject::tr("Index: %1").arg(uFacet));
            flag->setOrigin(point->getPoint());
            view->addFlag(flag, Gui::FlagLayout::TopRight);
        }
    }
}

void ViewProviderMesh::fillHoleCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
        n->setHandled();
        // context-menu
        QMenu menu;
        QAction* cl = menu.addAction(QObject::tr("Leave hole-filling mode"));
        QAction* id = menu.exec(QCursor::pos());
        if (cl == id) {
            view->setEditing(false);
            view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
            view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), fillHoleCallback,ud);
        }
    }
    else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL) {
            Base::Console().Message("No facet picked.\n");
            return;
        }

        n->setHandled();

        // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
        // really from the mesh we render and not from any other geometry
        Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
        if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMesh::getClassTypeId()))
            return;
        ViewProviderMesh* that = static_cast<ViewProviderMesh*>(vp);
        const SoDetail* detail = point->getDetail(that->getShapeNode());
        if ( detail && detail->getTypeId() == SoFaceDetail::getClassTypeId() ) {
            // get the boundary to the picked facet
            unsigned long uFacet = ((SoFaceDetail*)detail)->getFaceIndex();
            that->fillHole(uFacet);
        }
    }
}

void ViewProviderMesh::markPartCallback(void * ud, SoEventCallback * n)
{
    // handle only mouse button events
    if (n->getEvent()->isOfType(SoMouseButtonEvent::getClassTypeId())) {
        const SoMouseButtonEvent * mbe = static_cast<const SoMouseButtonEvent*>(n->getEvent());
        Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

        // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
        n->getAction()->setHandled();
        if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
            n->setHandled();
            // context-menu
            QMenu menu;
            QAction* cl = menu.addAction(QObject::tr("Leave removal mode"));
            QAction* id = menu.exec(QCursor::pos());
            if (cl == id) {
                view->setEditing(false);
                view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), markPartCallback,ud);

                std::vector<ViewProvider*> views = view->getViewProvidersOfType(ViewProviderMesh::getClassTypeId());
                for (std::vector<ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
                    static_cast<ViewProviderMesh*>(*it)->unmarkParts();
                }
            }
        }
        else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
            const SoPickedPoint * point = n->getPickedPoint();
            if (point == NULL) {
                Base::Console().Message("No facet picked.\n");
                return;
            }

            n->setHandled();

            // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
            // really from the mesh we render and not from any other geometry
            Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
            if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMesh::getClassTypeId()))
                return;
            ViewProviderMesh* that = static_cast<ViewProviderMesh*>(vp);
            const SoDetail* detail = point->getDetail(that->getShapeNode());
            if ( detail && detail->getTypeId() == SoFaceDetail::getClassTypeId() ) {
                // get the boundary to the picked facet
                unsigned long uFacet = ((SoFaceDetail*)detail)->getFaceIndex();
                if (that->isMarked(uFacet)) {
                    Gui::Application::Instance->activeDocument()->openCommand("Remove");
                    that->removePart();
                    view->render();
                    Gui::Application::Instance->activeDocument()->commitCommand();
                }
                else {
                    that->markPart(uFacet);
                }
            }
        }
#if 0
        else if (mbe->getButton() == SoMouseButtonEvent::BUTTON3 && mbe->getState() == SoButtonEvent::DOWN) {
            Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
            std::vector<ViewProvider*> views = view->getViewProvidersOfType(ViewProviderMesh::getClassTypeId());

            // Check if facets are marked
            unsigned long count = 0;
            for (std::vector<ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
                count += static_cast<ViewProviderMesh*>(*it)->countMarkedFacets();
            }

            if (count > 0) {
                Gui::Application::Instance->activeDocument()->openCommand("Remove");
                for (std::vector<ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
                    unsigned long count = static_cast<ViewProviderMesh*>(*it)->countMarkedFacets();
                    static_cast<ViewProviderMesh*>(*it)->removePart();
                    if (count > 0)
                        view->render();
                }
                
                Gui::Application::Instance->activeDocument()->commitCommand();
            }
        }
#endif
    }
}

void ViewProviderMesh::faceInfo(unsigned long uFacet)
{
    Mesh::Feature* fea = reinterpret_cast<Mesh::Feature*>(this->getObject());
    const MeshCore::MeshKernel& rKernel = fea->Mesh.getValue().getKernel();
    const MeshCore::MeshFacetArray& facets = rKernel.GetFacets();
    if (uFacet < facets.size()) {
        MeshCore::MeshFacet face = facets[uFacet];
        MeshCore::MeshGeomFacet tria = rKernel.GetFacet(face);
        Base::Console().Message("Mesh: %s Facet %ld: Points: <%ld, %ld, %ld>, Neighbours: <%ld, %ld, %ld>\n"
            "Triangle: <[%.6f, %.6f, %.6f], [%.6f, %.6f, %.6f], [%.6f, %.6f, %.6f]>\n", fea->getNameInDocument(), uFacet, 
            face._aulPoints[0], face._aulPoints[1], face._aulPoints[2], 
            face._aulNeighbours[0], face._aulNeighbours[1], face._aulNeighbours[2],
            tria._aclPoints[0].x, tria._aclPoints[0].y, tria._aclPoints[0].z,
            tria._aclPoints[1].x, tria._aclPoints[1].y, tria._aclPoints[1].z,
            tria._aclPoints[2].x, tria._aclPoints[2].y, tria._aclPoints[2].z);
    }
}

void ViewProviderMesh::fillHole(unsigned long uFacet)
{
    // get parameter from user settings
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
    int level = (int)hGrp->GetInt("FillHoleLevel", 2);

    // get the boundary to the picked facet
    std::list<unsigned long> aBorder;
    Mesh::Feature* fea = reinterpret_cast<Mesh::Feature*>(this->getObject());
    const MeshCore::MeshKernel& rKernel = fea->Mesh.getValue().getKernel();
    MeshCore::MeshRefPointToFacets cPt2Fac(rKernel);
    MeshCore::MeshAlgorithm meshAlg(rKernel);
    meshAlg.GetMeshBorder(uFacet, aBorder);
    std::vector<unsigned long> boundary(aBorder.begin(), aBorder.end());
    std::list<std::vector<unsigned long> > boundaries;
    boundaries.push_back(boundary);
    meshAlg.SplitBoundaryLoops(boundaries);

    std::vector<MeshCore::MeshFacet> newFacets;
    std::vector<Base::Vector3f> newPoints;
    unsigned long numberOfOldPoints = rKernel.CountPoints();
    for (std::list<std::vector<unsigned long> >::iterator it = boundaries.begin(); it != boundaries.end(); ++it) {
        if (it->size() < 3/* || it->size() > 200*/)
            continue;
        boundary = *it;
        MeshCore::MeshFacetArray faces;
        MeshCore::MeshPointArray points;
        MeshCore::QuasiDelaunayTriangulator cTria/*(0.05f)*/;
        if (meshAlg.FillupHole(boundary, cTria, faces, points, level, &cPt2Fac)) {
            if (boundary.front() == boundary.back())
                boundary.pop_back();
            // the triangulation may produce additional points which we must take into account when appending to the mesh
            unsigned long countBoundaryPoints = boundary.size();
            unsigned long countDifference = points.size() - countBoundaryPoints;
            if (countDifference > 0) {
                MeshCore::MeshPointArray::_TIterator pt = points.begin() + countBoundaryPoints;
                for (unsigned long i=0; i<countDifference; i++, pt++) {
                    boundary.push_back(numberOfOldPoints++);
                    newPoints.push_back(*pt);
                 }
            }
            for (MeshCore::MeshFacetArray::_TIterator kt = faces.begin(); kt != faces.end(); ++kt ) {
                kt->_aulPoints[0] = boundary[kt->_aulPoints[0]];
                kt->_aulPoints[1] = boundary[kt->_aulPoints[1]];
                kt->_aulPoints[2] = boundary[kt->_aulPoints[2]];
                newFacets.push_back(*kt);
            }
        }
    }
 
    if (newFacets.empty())
        return; // nothing to do
 
    //add the facets to the mesh and open a transaction object for the undo/redo stuff
    Gui::Application::Instance->activeDocument()->openCommand("Fill hole");
    fea->Mesh.append(newFacets, newPoints);
    Gui::Application::Instance->activeDocument()->commitCommand();
}

void ViewProviderMesh::markPart(unsigned long uFacet)
{
    _markedFacets.push_back(uFacet);
    MeshCore::MeshTopFacetVisitor clVisitor(_markedFacets);
    const MeshCore::MeshKernel& rKernel = static_cast<Mesh::Feature*>(pcObject)->Mesh.getValue().getKernel();
    MeshCore::MeshAlgorithm(rKernel).ResetFacetFlag(MeshCore::MeshFacet::VISIT);
    rKernel.VisitNeighbourFacets(clVisitor, uFacet);

    pcMatBinding->value = SoMaterialBinding::PER_FACE;
    App::Color c = ShapeColor.getValue();
    unsigned long uCtFacets = rKernel.CountFacets();
    pcShapeMaterial->diffuseColor.setNum(uCtFacets);
    for (unsigned long i=0; i<uCtFacets; i++)
        pcShapeMaterial->diffuseColor.set1Value(i, c.r,c.g,c.b);
    for (std::vector<unsigned long>::iterator it = _markedFacets.begin(); it != _markedFacets.end(); ++it)
        pcShapeMaterial->diffuseColor.set1Value(*it, 1.0f,0.0f,0.0f);
}

bool ViewProviderMesh::isMarked(unsigned long facet) const
{
    for (std::vector<unsigned long>::const_iterator it = _markedFacets.begin(); it != _markedFacets.end(); ++it) {
        if (*it == facet)
            return true;
    }
    return false;
}

void ViewProviderMesh::unmarkParts()
{
    _markedFacets.clear();
    pcMatBinding->value = SoMaterialBinding::OVERALL;
    App::Color c = ShapeColor.getValue();
    pcShapeMaterial->diffuseColor.setNum(0);
    pcShapeMaterial->diffuseColor.setValue(c.r,c.g,c.b);
}

void ViewProviderMesh::removePart()
{
    if (!_markedFacets.empty()) {
        // Get the attached mesh property
        Mesh::PropertyMeshKernel& meshProp = ((Mesh::Feature*)pcObject)->Mesh;
        meshProp.deleteFacetIndices(_markedFacets);
        ((Mesh::Feature*)pcObject)->purgeTouched();
        
        // notify the mesh shape node
        const MeshCore::MeshKernel& rKernel = static_cast<Mesh::Feature*>(pcObject)->Mesh.getValue().getKernel();
        App::Color c = ShapeColor.getValue();
        unsigned long uCtFacets = rKernel.CountFacets();
        pcShapeMaterial->diffuseColor.setNum(uCtFacets);
        for (unsigned long i=0; i<uCtFacets; i++)
            pcShapeMaterial->diffuseColor.set1Value(i, c.r,c.g,c.b);
        _markedFacets.clear();
    }
}

unsigned long ViewProviderMesh::countMarkedFacets() const
{
    return _markedFacets.size();
}

// ------------------------------------------------------

PROPERTY_SOURCE(MeshGui::ViewProviderIndexedFaceSet, MeshGui::ViewProviderMesh)

ViewProviderIndexedFaceSet::ViewProviderIndexedFaceSet()
{
}

ViewProviderIndexedFaceSet::~ViewProviderIndexedFaceSet()
{
}

void ViewProviderIndexedFaceSet::createMesh(const MeshCore::MeshKernel& rcMesh)
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

/** 
 * Extracts the mesh data from the feature \a pcFeature and creates
 * an Inventor node \a SoNode with these data. 
 */
void ViewProviderIndexedFaceSet::attach(App::DocumentObject *pcFeat)
{
    ViewProviderMesh::attach(pcFeat);

    pcMeshCoord = new SoCoordinate3;
    pcHighlight->addChild(pcMeshCoord);

    pcMeshFaces = new SoFCIndexedFaceSet;
    pcHighlight->addChild(pcMeshFaces);

    // read the threshold from the preferences
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
    int size = hGrp->GetInt("RenderTriangleLimit", -1);
    if (size > 0) static_cast<SoFCIndexedFaceSet*>(pcMeshFaces)->renderTriangleLimit = (unsigned int)(pow(10.0f,size));
}

void ViewProviderIndexedFaceSet::updateData(const App::Property* prop)
{
    Gui::ViewProviderGeometryObject::updateData(prop);
    if (prop->getTypeId() == Mesh::PropertyMeshKernel::getClassTypeId()) {
        const Mesh::PropertyMeshKernel* mesh = static_cast<const Mesh::PropertyMeshKernel*>(prop);
        createMesh(mesh->getValue().getKernel());
        showOpenEdges(OpenEdges.getValue());
    }
}

void ViewProviderIndexedFaceSet::showOpenEdges(bool show)
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

SoShape* ViewProviderIndexedFaceSet::getShapeNode() const
{
    return this->pcMeshFaces;
}

// ------------------------------------------------------

PROPERTY_SOURCE(MeshGui::ViewProviderMeshObject, MeshGui::ViewProviderMesh)

ViewProviderMeshObject::ViewProviderMeshObject()
{
}

ViewProviderMeshObject::~ViewProviderMeshObject()
{
}

void ViewProviderMeshObject::attach(App::DocumentObject *pcFeat)
{
    ViewProviderMesh::attach(pcFeat);

    pcMeshNode = new SoFCMeshObjectNode;
    pcHighlight->addChild(pcMeshNode);

    pcMeshShape = new SoFCMeshObjectShape;
    pcHighlight->addChild(pcMeshShape);

    // read the threshold from the preferences
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
    int size = hGrp->GetInt("RenderTriangleLimit", -1);
    if (size > 0) pcMeshShape->renderTriangleLimit = (unsigned int)(pow(10.0f,size));
}

void ViewProviderMeshObject::updateData(const App::Property* prop)
{
    Gui::ViewProviderGeometryObject::updateData(prop);
    if (prop->getTypeId() == Mesh::PropertyMeshKernel::getClassTypeId()) {
        const Mesh::PropertyMeshKernel* mesh = static_cast<const Mesh::PropertyMeshKernel*>(prop);
        this->pcMeshNode->mesh.setValue(mesh->getValuePtr());
        // Needs to update internal bounding box caches
        this->pcMeshShape->touch();
    }
}

void ViewProviderMeshObject::showOpenEdges(bool show)
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

SoShape* ViewProviderMeshObject::getShapeNode() const
{
    return this->pcMeshShape;
}
