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
# include <Inventor/SoPickedPoint.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <qmessagebox.h>
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
#include <Gui/Window.h>
#include <Gui/Flag.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include <Mod/Mesh/App/Core/Algorithm.h>
#include <Mod/Mesh/App/Core/Evaluation.h>
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/Core/MeshIO.h>
#include <Mod/Mesh/App/Core/Visitor.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/MeshProperties.h>
#include <Mod/Mesh/Gui/SoFCMeshNode.h>
#include <Mod/Mesh/Gui/SoFCMeshVertex.h>
#include <Mod/Mesh/Gui/SoFCMeshFaceSet.h>

#include "ViewProvider.h"
#include "ViewProviderMeshFaceSet.h"


using namespace MeshGui;


App::PropertyFloatConstraint::Constraints ViewProviderMeshFaceSet::floatRange = {1.0f,64.0f,1.0f};
const char* ViewProviderMeshFaceSet::LightingEnums[]= {"One side","Two side",NULL};

PROPERTY_SOURCE(MeshGui::ViewProviderMeshFaceSet, Gui::ViewProviderGeometryObject)

ViewProviderMeshFaceSet::ViewProviderMeshFaceSet() : pcOpenEdge(0), m_bEdit(false)
{
    ADD_PROPERTY(LineWidth,(2.0f));
    LineWidth.setConstraints(&floatRange);
    ADD_PROPERTY(PointSize,(2.0f));
    PointSize.setConstraints(&floatRange);
    ADD_PROPERTY(OpenEdges,(false));
    ADD_PROPERTY(Lighting,(1));
    Lighting.setEnums(LightingEnums);

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

    Lighting.touch();

    // read the correct shape color from the preferences
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
    App::Color color = ShapeColor.getValue();
    unsigned long current = color.getPackedValue();
    unsigned long setting = hGrp->GetUnsigned("MeshColor", current);
    if (current != setting) {
        color.setPackedValue(setting);
        ShapeColor.setValue(color);
    }

    pcComplexData = new SoTransform();
    pcComplexData->ref();
}

ViewProviderMeshFaceSet::~ViewProviderMeshFaceSet()
{
    pOpenColor->unref();
    pcLineStyle->unref();
    pcPointStyle->unref();
    pShapeHints->unref();
    pcMatBinding->unref();
    pcComplexData->unref();
}

void ViewProviderMeshFaceSet::onChanged(const App::Property* prop)
{
    if ( prop == &LineWidth ) {
        pcLineStyle->lineWidth = LineWidth.getValue();
    }
    else if ( prop == &PointSize ) {
        pcPointStyle->pointSize = PointSize.getValue();
    }
    else if ( prop == &OpenEdges ) {
        showOpenEdges( OpenEdges.getValue() );
    }
    else if ( prop == &Lighting ) {
        if ( Lighting.getValue() == 0 )
            pShapeHints->vertexOrdering = SoShapeHints::UNKNOWN_ORDERING;
        else
            pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    }
    else {
        // Set the inverse color for open edges
        if ( prop == &ShapeColor ) {
            setOpenEdgeColorFrom(ShapeColor.getValue());
        }
        else if ( prop == &ShapeMaterial ) {
            setOpenEdgeColorFrom(ShapeMaterial.getValue().diffuseColor);
        }
        ViewProviderGeometryObject::onChanged(prop);
    }
}

void ViewProviderMeshFaceSet::setOpenEdgeColorFrom( const App::Color& c )
{
    float r=1.0f-c.r; r = r < 0.5f ? 0.0f : 1.0f;
    float g=1.0f-c.g; g = g < 0.5f ? 0.0f : 1.0f;
    float b=1.0f-c.b; b = b < 0.5f ? 0.0f : 1.0f;
    pOpenColor->rgb.setValue(r, g, b);
}

void ViewProviderMeshFaceSet::attach(App::DocumentObject *pcFeat)
{
    ViewProviderGeometryObject::attach(pcFeat);

    // apply the transformation of the mesh object
    pcHighlight->addChild(pcComplexData);

    pcVertexNode = new SoFCMeshVertex;
    pcHighlight->addChild(pcVertexNode);

    pcFacetNode = new SoFCMeshFacet;
    pcHighlight->addChild(pcFacetNode);

    pcFaceSet = new SoFCMeshFaceSet;
    pcHighlight->addChild(pcFaceSet);

    // faces
    SoGroup* pcFlatRoot = new SoGroup();

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
    SoGroup* pcFlatWireRoot = new SoGroup();
    pcFlatWireRoot->addChild(pcFlatRoot);
    pcFlatWireRoot->addChild(pcWireRoot);
    addDisplayMaskMode(pcFlatWireRoot, "FlatWireframe");
}

void ViewProviderMeshFaceSet::updateData(const App::Property* prop)
{
    Gui::ViewProviderGeometryObject::updateData(prop);
    if (prop->getTypeId() == Mesh::PropertyMeshKernel::getClassTypeId()) {
        const Mesh::PropertyMeshKernel* mesh = static_cast<const Mesh::PropertyMeshKernel*>(prop);
        this->pcComplexData->setMatrix(convert(mesh->getValue().getMatrix()));
        const MeshCore::MeshPointArray& rPAry = mesh->getValue().getKernel().GetPoints();
        this->pcVertexNode->point.setValue(rPAry);
        const MeshCore::MeshFacetArray& rFAry = mesh->getValue().getKernel().GetFacets();
        this->pcFacetNode->coordIndex.setValue(rFAry);

        // Needs to update internal bounding box caches
        pcFaceSet->touch();
    }
}

QIcon ViewProviderMeshFaceSet::getIcon() const
{
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
}

void ViewProviderMeshFaceSet::setDisplayMode(const char* ModeName)
{
    if ( strcmp("Shaded",ModeName)==0 )
        setDisplayMaskMode("Flat");
    else if ( strcmp("Points",ModeName)==0 )
        setDisplayMaskMode("Point");
    else if ( strcmp("Flat Lines",ModeName)==0 )
        setDisplayMaskMode("FlatWireframe");
    else if ( strcmp("Wireframe",ModeName)==0 )
        setDisplayMaskMode("Wireframe");

    ViewProviderGeometryObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderMeshFaceSet::getDisplayModes(void) const
{
    std::vector<std::string> StrList;

    // add your own modes
    StrList.push_back("Shaded");
    StrList.push_back("Wireframe");
    StrList.push_back("Flat Lines");
    StrList.push_back("Points");

    return StrList;
}

void ViewProviderMeshFaceSet::setEdit(void)
{
    if ( m_bEdit ) return;
    m_bEdit = true;
}

void ViewProviderMeshFaceSet::unsetEdit(void)
{
    m_bEdit = false;
}

const char* ViewProviderMeshFaceSet::getEditModeName(void)
{
    return "Polygon picking";
}

bool ViewProviderMeshFaceSet::handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer)
{
    if ( m_bEdit ) {
        unsetEdit();
        std::vector<SbVec2f> clPoly = Viewer.getPickedPolygon();
        if ( clPoly.size() < 3 )
            return false;
        if ( clPoly.front() != clPoly.back() )
            clPoly.push_back(clPoly.front());

        cutMesh(clPoly, Viewer);
    }

    return false;
}

void ViewProviderMeshFaceSet::showOpenEdges(bool show)
{
    if (pcOpenEdge) {
        // remove the node and destroy the data
        pcRoot->removeChild(pcOpenEdge);
        pcOpenEdge = 0;
    }

    if ( show ) {
        pcOpenEdge = new SoSeparator();
        pcOpenEdge->addChild(pcLineStyle);
        pcOpenEdge->addChild(pOpenColor);

        pcOpenEdge->addChild(pcVertexNode);
        pcOpenEdge->addChild(pcFacetNode);
        pcOpenEdge->addChild(new SoFCMeshOpenEdgeSet);

        // add to the highlight node
        pcRoot->addChild(pcOpenEdge);
    }
}

void ViewProviderMeshFaceSet::cutMesh( const std::vector<SbVec2f>& picked, Gui::View3DInventorViewer &Viewer)
{
  // get the normal of the front clipping plane
  Base::Vector3f cPoint, cNormal;
  Viewer.getFrontClippingPlane(cPoint, cNormal);
  SoCamera* pCam = Viewer.getCamera();  
  SbViewVolume  vol = pCam->getViewVolume (); 

  // create a tool shape from these points
  std::vector<MeshCore::MeshGeomFacet> aFaces;
  bool ok = ViewProviderMesh::createToolMesh( picked, vol, cNormal, aFaces );

  // Get the attached mesh property
  Mesh::PropertyMeshKernel& meshProp = ((Mesh::Feature*)pcObject)->Mesh;

  // Get the facet indices inside the tool mesh
  std::vector<unsigned long> indices;
  MeshCore::MeshKernel cToolMesh;
  cToolMesh = aFaces;
  MeshCore::MeshFacetGrid cGrid(meshProp.getValue().getKernel());
  MeshCore::MeshAlgorithm cAlg(meshProp.getValue().getKernel());
  cAlg.GetFacetsFromToolMesh(cToolMesh, cNormal, cGrid, indices);

  //Remove the facets from the mesh and open a transaction object for the undo/redo stuff
  Gui::Application::Instance->activeDocument()->openCommand("Cut");
  meshProp.deleteFacetIndices( indices );
  Gui::Application::Instance->activeDocument()->commitCommand();
  ((Mesh::Feature*)pcObject)->purgeTouched();

  // notify the mesh shape node
  pcFaceSet->touch();
  Viewer.render();
  
  if ( !ok ) // note: the mouse grabbing needs to be released
//      QMessageBox::warning(Viewer.getWidget(),"Invalid polygon","The picked polygon seems to have self-overlappings.\n\nThis could lead to strange results.");
    Base::Console().Message("The picked polygon seems to have self-overlappings. This could lead to strange results.");
}

void ViewProviderMeshFaceSet::faceInfoCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
        n->setHandled();
        view->setEditing(false);
        view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
        view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), faceInfoCallback);
    } else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL) {
            Base::Console().Message("No facet picked.\n");
            return;
        }

        n->setHandled();

        // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
        // really from the mesh we render and not from any other geometry
        Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
        if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMeshFaceSet::getClassTypeId()))
            return;
        ViewProviderMeshFaceSet* that = static_cast<ViewProviderMeshFaceSet*>(vp);
        const SoDetail* detail = point->getDetail(that->pcFaceSet);
        if ( detail && detail->getTypeId() == SoFaceDetail::getClassTypeId() ) {
            // get the boundary to the picked facet
            unsigned long uFacet = ((SoFaceDetail*)detail)->getFaceIndex();
            that->faceInfo(uFacet);
            Gui::Flag* flag = new Gui::Flag;
            flag->setText(QString("Index: %1").arg(uFacet));
            flag->setOrigin(point->getPoint());
            view->addFlag(flag, Gui::FlagLayout::TopRight);
        }
    }
}

void ViewProviderMeshFaceSet::fillHoleCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
        n->setHandled();
        view->setEditing(false);
        view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
        view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), fillHoleCallback);
    } else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL) {
            Base::Console().Message("No facet picked.\n");
            return;
        }

        n->setHandled();

        // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
        // really from the mesh we render and not from any other geometry
        Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
        if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMeshFaceSet::getClassTypeId()))
            return;
        ViewProviderMeshFaceSet* that = static_cast<ViewProviderMeshFaceSet*>(vp);
        const SoDetail* detail = point->getDetail(that->pcFaceSet);
        if ( detail && detail->getTypeId() == SoFaceDetail::getClassTypeId() ) {
            // get the boundary to the picked facet
            unsigned long uFacet = ((SoFaceDetail*)detail)->getFaceIndex();
            that->fillHole(uFacet);
        }
    }
}

void ViewProviderMeshFaceSet::markPartCallback(void * ud, SoEventCallback * n)
{
    // handle only mouse button events
    if (n->getEvent()->isOfType(SoMouseButtonEvent::getClassTypeId())) {
        const SoMouseButtonEvent * mbe = static_cast<const SoMouseButtonEvent*>(n->getEvent());
        Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

        // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
        n->getAction()->setHandled();
        if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
            n->setHandled();
            view->setEditing(false);
            view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), markPartCallback);

            std::vector<ViewProvider*> views = view->getViewProvidersOfType(ViewProviderMeshFaceSet::getClassTypeId());
            for (std::vector<ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
                static_cast<ViewProviderMeshFaceSet*>(*it)->unmarkParts();
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
            if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMeshFaceSet::getClassTypeId()))
                return;
            ViewProviderMeshFaceSet* that = static_cast<ViewProviderMeshFaceSet*>(vp);
            const SoDetail* detail = point->getDetail(that->pcFaceSet);
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
            std::vector<ViewProvider*> views = view->getViewProvidersOfType(ViewProviderMeshFaceSet::getClassTypeId());

            // Check if facets are marked
            unsigned long count = 0;
            for (std::vector<ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
                count += static_cast<ViewProviderMeshFaceSet*>(*it)->countMarkedFacets();
            }

            if (count > 0) {
                Gui::Application::Instance->activeDocument()->openCommand("Remove");
                for (std::vector<ViewProvider*>::iterator it = views.begin(); it != views.end(); ++it) {
                    unsigned long count = static_cast<ViewProviderMeshFaceSet*>(*it)->countMarkedFacets();
                    static_cast<ViewProviderMeshFaceSet*>(*it)->removePart();
                    if (count > 0)
                        view->render();
                }
                
                Gui::Application::Instance->activeDocument()->commitCommand();
            }
        }
#endif
    }
}

void ViewProviderMeshFaceSet::faceInfo(unsigned long uFacet)
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

void ViewProviderMeshFaceSet::fillHole(unsigned long uFacet)
{
    // get the boundary to the picked facet
    std::list<unsigned long> aBorder;
    Mesh::Feature* fea = reinterpret_cast<Mesh::Feature*>(this->getObject());
    const MeshCore::MeshKernel& rKernel = fea->Mesh.getValue().getKernel();
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
        if (meshAlg.FillupHole(boundary, 0.05f, faces, points)) {
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

    // notify the mesh shape node
    this->pcFaceSet->touch();
    //fea->getDocument().update(fea);
}

void ViewProviderMeshFaceSet::markPart(unsigned long uFacet)
{
    _markedFacets.push_back(uFacet);
    MeshCore::MeshTopFacetVisitor clVisitor(_markedFacets);
    const MeshCore::MeshKernel& rKernel = static_cast<Mesh::Feature*>(pcObject)->Mesh.getValue().getKernel();
    MeshCore::MeshAlgorithm(rKernel).ResetFacetFlag(MeshCore::MeshFacet::VISIT);
    rKernel.VisitNeighbourFacets(clVisitor, uFacet);

    pcMatBinding->value = SoMaterialBinding::PER_FACE_INDEXED;
    App::Color c = ShapeColor.getValue();
    unsigned long uCtFacets = rKernel.CountFacets();
    pcShapeMaterial->diffuseColor.setNum(uCtFacets);
    for (unsigned long i=0; i<uCtFacets; i++)
        pcShapeMaterial->diffuseColor.set1Value(i, c.r,c.g,c.b);
    for (std::vector<unsigned long>::iterator it = _markedFacets.begin(); it != _markedFacets.end(); ++it)
        pcShapeMaterial->diffuseColor.set1Value(*it, 1.0f,0.0f,0.0f);
}

bool ViewProviderMeshFaceSet::isMarked(unsigned long facet) const
{
    for (std::vector<unsigned long>::const_iterator it = _markedFacets.begin(); it != _markedFacets.end(); ++it) {
        if (*it == facet)
            return true;
    }
    return false;
}

void ViewProviderMeshFaceSet::unmarkParts()
{
    _markedFacets.clear();
    pcMatBinding->value = SoMaterialBinding::OVERALL;
    App::Color c = ShapeColor.getValue();
    pcShapeMaterial->diffuseColor.setNum(0);
    pcShapeMaterial->diffuseColor.setValue(c.r,c.g,c.b);
}

void ViewProviderMeshFaceSet::removePart()
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
        pcFaceSet->touch();
        _markedFacets.clear();
    }
}

unsigned long ViewProviderMeshFaceSet::countMarkedFacets() const
{
    return _markedFacets.size();
}
