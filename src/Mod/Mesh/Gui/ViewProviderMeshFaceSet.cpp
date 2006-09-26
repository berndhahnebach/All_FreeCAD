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
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
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
#include <Mod/Mesh/Gui/SoFCMeshNode.h>
#include <Mod/Mesh/Gui/SoFCMeshVertex.h>
#include <Mod/Mesh/Gui/SoFCMeshFaceSet.h>

#include "ViewProvider.h"
#include "ViewProviderMeshFaceSet.h"


using namespace MeshGui;


PROPERTY_SOURCE(MeshGui::ViewProviderMeshFaceSet, Gui::ViewProviderFeature)

ViewProviderMeshFaceSet::ViewProviderMeshFaceSet() : pcOpenEdge(0), _mouseModel(0), m_bEdit(false)
{
  ADD_PROPERTY(LineWidth,(2.0f));
  ADD_PROPERTY(PointSize,(2.0f));
  ADD_PROPERTY(OpenEdges,(false));

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

  // read the correct shape color from the preferences
  FCHandle<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
  App::Color color = ShapeColor.getValue();
  unsigned long current = color.getPackedValue();
  unsigned long setting = hGrp->GetUnsigned("MeshColor", current);
  if ( current != setting )
  {
    color.setPackedValue(setting);
    ShapeColor.setValue(color);
  }
}

ViewProviderMeshFaceSet::~ViewProviderMeshFaceSet()
{
  pOpenColor->unref();
  pcLineStyle->unref();
  pcPointStyle->unref();
}

void ViewProviderMeshFaceSet::onChanged(const App::Property* prop)
{
  if ( prop == &LineWidth ) {
    pcLineStyle->lineWidth = LineWidth.getValue();
  } else if ( prop == &PointSize ) {
    pcPointStyle->pointSize = PointSize.getValue();
  } else if ( prop == &OpenEdges ) {
    showOpenEdges( OpenEdges.getValue() );
  } else {
    // Set the inverse color for open edges
    if ( prop == &ShapeColor ) {
      setOpenEdgeColorFrom(ShapeColor.getValue());
    } else if ( prop == &ShapeMaterial ) {
      setOpenEdgeColorFrom(ShapeMaterial.getValue().diffuseColor);
    }
    ViewProviderFeature::onChanged(prop);
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
  ViewProviderFeature::attach(pcFeat);

  // only one selection node for the mesh
  const Mesh::Feature* meshFeature = dynamic_cast<Mesh::Feature*>(pcFeat);

  SoFCMeshVertex* vertex = new SoFCMeshVertex;
  const MeshCore::MeshPointArray& rPAry = meshFeature->getMesh().GetPoints();
  vertex->point.setValue(rPAry);
  pcHighlight->addChild(vertex);

  SoFCMeshFacet* facet = new SoFCMeshFacet;
  const MeshCore::MeshFacetArray& rFAry = meshFeature->getMesh().GetFacets();
  facet->coordIndex.setValue(rFAry);
  pcHighlight->addChild(facet);

  pcFaceSet = new SoFCMeshFaceSet;
  pcHighlight->addChild(pcFaceSet);

  // faces
  SoGroup* pcFlatRoot = new SoGroup();

  // read the correct shape color from the preferences
  FCHandle<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
  bool twoSide = hGrp->GetBool("TwoSideRendering", true);
  if ( twoSide )
  {
    // enable two-side rendering
    SoShapeHints * flathints = new SoShapeHints;
    flathints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    flathints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    pcFlatRoot->addChild(flathints);
  }

  pcFlatRoot->addChild(pcShapeMaterial);
  pcFlatRoot->addChild(pcHighlight);
  addDisplayMode(pcFlatRoot, "Flat");

  // points
  SoGroup* pcPointRoot = new SoGroup();
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcFlatRoot);
  addDisplayMode(pcPointRoot, "Point");

  // wires
  SoLightModel* pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  SoGroup* pcWireRoot = new SoGroup();
  pcWireRoot->addChild(pcLineStyle);
  pcWireRoot->addChild(pcLightModel);
  pcWireRoot->addChild(pcShapeMaterial);
  pcWireRoot->addChild(pcHighlight);
  addDisplayMode(pcWireRoot, "Wireframe");

  // faces+wires
  SoGroup* pcFlatWireRoot = new SoGroup();
  pcFlatWireRoot->addChild(pcFlatRoot);
  pcFlatWireRoot->addChild(pcWireRoot);
  addDisplayMode(pcFlatWireRoot, "FlatWireframe");
}

void ViewProviderMeshFaceSet::updateData(void)
{
  const Mesh::Feature* meshFeature = dynamic_cast<Mesh::Feature*>(pcObject);
  if ( pcHighlight->getNumChildren() > 2 )
  {
    SoNode* ch1 = pcHighlight->getChild(0);
    if ( ch1->getTypeId() == SoFCMeshVertex::getClassTypeId() ) {
      const MeshCore::MeshPointArray& rPAry = meshFeature->getMesh().GetPoints();
      SoFCMeshVertex* vertex = (SoFCMeshVertex*)ch1;
      vertex->point.setValue(rPAry);
    }

    SoNode* ch2 = pcHighlight->getChild(1);
    if ( ch2->getTypeId() == SoFCMeshFacet::getClassTypeId() ) {
      const MeshCore::MeshFacetArray& rFAry = meshFeature->getMesh().GetFacets();
      SoFCMeshFacet* facet = (SoFCMeshFacet*)ch2;
      facet->coordIndex.setValue(rFAry);
    }

    // Needs to update internal bounding box caches
    pcFaceSet->touch();
  }
}

QPixmap ViewProviderMeshFaceSet::getIcon() const
{
  const char * Mesh_Feature_xpm[] = {
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

void ViewProviderMeshFaceSet::setMode(const char* ModeName)
{
  if ( strcmp("Shaded",ModeName)==0 )
    setDisplayMode("Flat");
  else if ( strcmp("Points",ModeName)==0 )
    setDisplayMode("Point");
  else if ( strcmp("Shaded+Wireframe",ModeName)==0 )
    setDisplayMode("FlatWireframe");
  else if ( strcmp("Wireframe",ModeName)==0 )
    setDisplayMode("Wireframe");

  ViewProviderDocumentObject::setMode( ModeName );
}

std::list<std::string> ViewProviderMeshFaceSet::getModes(void) const
{
  std::list<std::string> StrList;

  // add your own modes
  StrList.push_back("Shaded");
  StrList.push_back("Wireframe");
  StrList.push_back("Shaded+Wireframe");
  StrList.push_back("Points");

  return StrList;
}

void ViewProviderMeshFaceSet::setEdit(void)
{
  if ( m_bEdit ) return;
//  ViewProviderFeature::setEdit();
  m_bEdit = true;
  _timer.start();
}

void ViewProviderMeshFaceSet::unsetEdit(void)
{
//  ViewProviderFeature::unsetEdit();
  m_bEdit = false;
  //_mouseModel->releaseMouseModel();
  delete _mouseModel;
  _mouseModel = 0;
}

const char* ViewProviderMeshFaceSet::getEditModeName(void)
{
  return "Polygon picking";
}

bool ViewProviderMeshFaceSet::handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer)
{
  if ( m_bEdit && !_mouseModel )
  {
    _mouseModel = new Gui::PolyPickerMouseModel();
    _mouseModel->grabMouseModel(&Viewer);
  }

  if ( m_bEdit && _mouseModel )
  {
    // double click event
    int hd = _mouseModel->handleEvent(ev, Viewer.getViewportRegion());
    if ( hd == Gui::AbstractMouseModel::Finish )
    {
      std::vector<SbVec2f> clPoly = _mouseModel->getPolygon();
      if ( clPoly.size() < 3 )
        return true;
      if ( clPoly.front() != clPoly.back() )
        clPoly.push_back(clPoly.front());

      // get the normal of the front clipping plane
      Base::Vector3f cPoint, cNormal;
      Viewer.getFrontClippingPlane(cPoint, cNormal);
      SoCamera* pCam = Viewer.getCamera();  
      SbViewVolume  vol = pCam->getViewVolume (); 

      // create a tool shape from these points
      std::vector<MeshCore::MeshGeomFacet> aFaces;
      bool ok = ViewProviderMesh::createToolMesh( clPoly, vol, cNormal, aFaces );

      // Get the attached mesh property
      Mesh::PropertyMeshKernel& meshProp = ((Mesh::Feature*)pcObject)->Mesh;

      // Get the facet indices inside the tool mesh
      std::vector<unsigned long> indices;
      MeshCore::MeshKernel cToolMesh;
      cToolMesh = aFaces;
      MeshCore::MeshFacetGrid cGrid(meshProp.getValue());
      MeshCore::MeshAlgorithm cAlg(meshProp.getValue());
      cAlg.GetFacetsFromToolMesh(cToolMesh, cNormal, cGrid, indices);

      //Remove the facets from the mesh and open a transaction object for the undo/redo stuff
      Gui::Application::Instance->activeDocument()->openCommand("Cut");
      meshProp.deleteFacetIndices( indices );
      Gui::Application::Instance->activeDocument()->commitCommand();
      ((Mesh::Feature*)pcObject)->setModified(false);

      // notify the mesh shape node
      pcFaceSet->touch();

      Viewer.render();
      unsetEdit();
      if ( !ok ) // note: the mouse grabbing needs to be released
//        QMessageBox::warning(Viewer.getWidget(),"Invalid polygon","The picked polygon seems to have self-overlappings.\n\nThis could lead to strange results.");
        Base::Console().Message("The picked polygon seems to have self-overlappings. This could lead to strange results.");

      return true;
    }
    else if ( hd == Gui::AbstractMouseModel::Cancel )
    {
      unsetEdit();
    }

    return true;
  }

  return false;
}

void ViewProviderMeshFaceSet::showOpenEdges(bool show)
{
  if ( show ) {
    pcOpenEdge = new SoSeparator();
    pcOpenEdge->addChild(pcLineStyle);
    pcOpenEdge->addChild(pOpenColor);

    SoFCMeshOpenEdgeSet* openEdges = new SoFCMeshOpenEdgeSet;
    pcOpenEdge->addChild(openEdges);

    // add to the highlight node
    pcHighlight->addChild(pcOpenEdge);
  } else if (pcOpenEdge) {
    // remove the node and destroy the data
    pcHighlight->removeChild(pcOpenEdge);
    pcOpenEdge = 0;
  }
}
