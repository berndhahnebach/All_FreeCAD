/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qmessagebox.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoIndexedLineSet.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
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
#include <Mod/Mesh/App/Core/Triangulation.h>
#include <Mod/Mesh/App/Core/Visitor.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include "ViewProvider.h"


using namespace MeshGui;

using Mesh::Feature;
using MeshCore::MeshKernel;
using MeshCore::MeshPointIterator;
using MeshCore::MeshFacetIterator;
using MeshCore::MeshGeomFacet;
using MeshCore::MeshFacet;
using MeshCore::MeshPolygonTriangulation;

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

// ======================================================================

App::PropertyFloatConstraint::Constraints ViewProviderMesh::floatRange = {0.0f,20.0f,0.1f};

PROPERTY_SOURCE(MeshGui::ViewProviderMesh, Gui::ViewProviderFeature)


ViewProviderMesh::ViewProviderMesh() : m_bEdit(false)
{
  ADD_PROPERTY(LineWidth,(2.0f));
  LineWidth.setConstraints(&floatRange);
  ADD_PROPERTY(PointSize,(2.0f));
  PointSize.setConstraints(&floatRange);
  ADD_PROPERTY(OpenEdges,(false));

  // create the mesh core nodes
  pcMeshCoord = new SoCoordinate3();
  pcMeshCoord->ref();
  pcMeshFaces = new SoIndexedFaceSet();
  pcMeshFaces->ref();
  pOpenEdges = new SoBaseColor();
  setOpenEdgeColorFrom(ShapeColor.getValue());
  pOpenEdges->ref();

  pcLineStyle = new SoDrawStyle();
  pcLineStyle->ref();
  pcLineStyle->style = SoDrawStyle::LINES;
  pcLineStyle->lineWidth = LineWidth.getValue();

  pcPointStyle = new SoDrawStyle();
  pcPointStyle->ref();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = PointSize.getValue();

  // read the correct shape color from the preferences
  Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
  App::Color color = ShapeColor.getValue();
  unsigned long current = color.getPackedValue();
  unsigned long setting = hGrp->GetUnsigned("MeshColor", current);
  if ( current != setting )
  {
    color.setPackedValue(setting);
    ShapeColor.setValue(color);
  }
}

ViewProviderMesh::~ViewProviderMesh()
{
  pcMeshCoord->unref();
  pcMeshFaces->unref();
  pOpenEdges->unref();
  pcLineStyle->unref();
  pcPointStyle->unref();
}

void ViewProviderMesh::onChanged(const App::Property* prop)
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

void ViewProviderMesh::setOpenEdgeColorFrom( const App::Color& c )
{
  float r=1.0f-c.r; r = r < 0.5f ? 0.0f : 1.0f;
  float g=1.0f-c.g; g = g < 0.5f ? 0.0f : 1.0f;
  float b=1.0f-c.b; b = b < 0.5f ? 0.0f : 1.0f;
  pOpenEdges->rgb.setValue(r, g, b);
}

void ViewProviderMesh::createMesh( const MeshCore::MeshKernel& rcMesh )
{
#if 0
  SbVec3f* vertices = 0;
  int* faces = 0;

  try {
    pcMeshCoord->point.deleteValues(0);
    pcMeshFaces->coordIndex.deleteValues(0);
    vertices = new SbVec3f[rcMesh.CountPoints()];
    faces = new int [4*rcMesh.CountFacets()];

    Base::SequencerLauncher seq( "Building View node...", rcMesh.CountFacets() );

    unsigned long j=0;
    MeshFacetIterator cFIt(rcMesh);
    for( cFIt.Init(); cFIt.More(); cFIt.Next(), j++ )
    {
      const MeshGeomFacet& rFace = *cFIt;
      MeshFacet aFace = cFIt.GetIndices();

      for ( int i=0; i<3; i++ )
      {
        vertices[aFace._aulPoints[i]].setValue(rFace._aclPoints[i].x,
                                               rFace._aclPoints[i].y,
                                               rFace._aclPoints[i].z);
        faces[4*j+i] = aFace._aulPoints[i];
      }

      faces[4*j+3] = SO_END_FACE_INDEX;
      Base::Sequencer().next( false ); // don't allow to cancel
    }

    pcMeshCoord->point.setValues(0,rcMesh.CountPoints(), vertices);
    delete [] vertices;
    pcMeshFaces->coordIndex.setValues(0,4*rcMesh.CountFacets(),(const int32_t*) faces);
    delete [] faces;
  } catch (const Base::MemoryException& e) {
    pcMeshCoord->point.deleteValues(0);
    pcMeshFaces->coordIndex.deleteValues(0);
    delete [] vertices;
    delete [] faces;
    throw e;
  }

#else
  // This is a faster way to build up the mesh structure and needs less temporary memory.
  // Therefore we must disable the notification of SoIndexedFaceSet whenn filling up the SoCoordinate3 and 
  // coordIndex fields. Once this has finished we enable it again.

  Base::SequencerLauncher seq( "Building display data...", rcMesh.CountFacets() );

  // disable the notification, otherwise whenever a point is inserted SoIndexedFacetSet gets notified
  pcMeshCoord->enableNotify(false);
  pcMeshCoord->point.setNum( rcMesh.CountPoints() );
  // set the point coordinates
  MeshPointIterator cPIt(rcMesh);
  for ( cPIt.Init(); cPIt.More(); cPIt.Next() )
  {
    pcMeshCoord->point.set1Value( cPIt.Position(), cPIt->x, cPIt->y, cPIt->z );
  }
  // enable notification again
  pcMeshCoord->enableNotify(true);

  // disable the notification, otherwise whenever a point is inserted SoIndexedFacetSet gets notified
  pcMeshFaces->coordIndex.enableNotify(false);
  pcMeshFaces->coordIndex.setNum( 4*rcMesh.CountFacets() );
  // set the facets indices
  unsigned long j=0;
  MeshFacetIterator cFIt(rcMesh);
  for ( cFIt.Init(); cFIt.More(); cFIt.Next(), j++ )
  {
    const MeshFacet& aFace = cFIt.GetReference();

    for ( int i=0; i<3; i++ )
    {
      pcMeshFaces->coordIndex.set1Value(4*j+i, aFace._aulPoints[i]);
    }

    pcMeshFaces->coordIndex.set1Value(4*j+3, SO_END_FACE_INDEX);
    Base::Sequencer().next( false ); // don't allow to cancel
  }
  // enable notification again
  pcMeshFaces->coordIndex.enableNotify(true);

  pcMeshCoord->touch();
  pcMeshFaces->coordIndex.touch();
#endif
}

void ViewProviderMesh::attach(App::DocumentObject *pcFeat)
{
  // Call parents attach method
  ViewProviderFeature::attach(pcFeat);

  // only one selection node for the mesh
  pcHighlight->addChild(pcMeshCoord);
  pcHighlight->addChild(pcMeshFaces);


  // flat shaded (Normal) ------------------------------------------
  SoGroup* pcFlatRoot = new SoGroup();

  // read the correct shape color from the preferences
  Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("Mod/Mesh");
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
  addDisplayMaskMode(pcFlatRoot, "Flat");

  // points part ---------------------------------------------
  SoGroup* pcPointRoot = new SoGroup();
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcFlatRoot);
  addDisplayMaskMode(pcPointRoot, "Point");

  // wire part ----------------------------------------------
  SoLightModel* pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  SoGroup* pcWireRoot = new SoGroup();
  pcWireRoot->addChild(pcLineStyle);
  pcWireRoot->addChild(pcLightModel);
  pcWireRoot->addChild(pcShapeMaterial);
  pcWireRoot->addChild(pcHighlight);
  addDisplayMaskMode(pcWireRoot, "Wireframe");

  // wire shaded  ------------------------------------------
  SoGroup* pcFlatWireRoot = new SoGroup();
  pcFlatWireRoot->addChild(pcFlatRoot);
  pcFlatWireRoot->addChild(pcWireRoot);
  addDisplayMaskMode(pcFlatWireRoot, "FlatWireframe");

  // Turns on backface culling
  SoShapeHints * wirehints = new SoShapeHints;
  wirehints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
  wirehints->shapeType = SoShapeHints::SOLID;
  wirehints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  SoGroup* pcHiddenLineRoot = new SoGroup();
  pcHiddenLineRoot->addChild(wirehints);
  pcHiddenLineRoot->addChild(pcWireRoot);
  addDisplayMaskMode(pcHiddenLineRoot, "HiddenLine");
}

void ViewProviderMesh::updateData(const App::Property* Prop)
{
    if (Prop->getTypeId() == Mesh::PropertyMeshKernel::getClassTypeId()) {
        const Mesh::PropertyMeshKernel* kernel = static_cast<const Mesh::PropertyMeshKernel*>(Prop);
        createMesh(kernel->getValue());
    }
}

QIcon ViewProviderMesh::getIcon() const
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

void ViewProviderMesh::setDisplayMode(const char* ModeName)
{
  if ( strcmp("Shaded",ModeName)==0 )
    setDisplayMaskMode("Flat");
  else if ( strcmp("Wireframe",ModeName)==0 )
    setDisplayMaskMode("Wireframe");
  else if ( strcmp("Points",ModeName)==0 )
    setDisplayMaskMode("Point");
  else if ( strcmp("Shaded+Wireframe",ModeName)==0 )
    setDisplayMaskMode("FlatWireframe");
  else if ( strcmp("Hidden line",ModeName)==0 )
    setDisplayMaskMode("HiddenLine");

  ViewProviderDocumentObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderMesh::getDisplayModes(void) const
{
  // get the modes of the father
  std::vector<std::string> StrList;

  // add your own modes
  StrList.push_back("Shaded");
  StrList.push_back("Wireframe");
  StrList.push_back("Shaded+Wireframe");
  StrList.push_back("Points");
  StrList.push_back("Hidden line");

  return StrList;
}

void ViewProviderMesh::setEdit(void)
{
  if ( m_bEdit ) return;
  m_bEdit = true;
}

void ViewProviderMesh::unsetEdit(void)
{
  m_bEdit = false;
}

const char* ViewProviderMesh::getEditModeName(void)
{
  return "Polygon picking";
}

bool ViewProviderMesh::createToolMesh( const std::vector<SbVec2f>& rclPoly, const SbViewVolume& vol, const Base::Vector3f& rcNormal, std::vector<MeshCore::MeshGeomFacet>& aFaces)
{
  float fX, fY, fZ;
  SbVec3f pt1, pt2, pt3, pt4;
  MeshGeomFacet face;
  std::vector<Base::Vector3f> top, bottom, polygon;

  for ( std::vector<SbVec2f>::const_iterator it = rclPoly.begin(); it != rclPoly.end(); ++it )
  {
    // the following element
    std::vector<SbVec2f>::const_iterator nt = it + 1;
    if ( nt == rclPoly.end() )
      nt = rclPoly.begin();
    else if ( *it == *nt )
      continue; // two adjacent vertices are equal

    vol.projectPointToLine( *it, pt1, pt2 );
    vol.projectPointToLine( *nt, pt3, pt4 );

    // 1st facet
    pt1.getValue(fX, fY, fZ);
    face._aclPoints[0].Set(fX, fY, fZ);
    pt4.getValue(fX, fY, fZ);
    face._aclPoints[1].Set(fX, fY, fZ);
    pt3.getValue(fX, fY, fZ);
    face._aclPoints[2].Set(fX, fY, fZ);
    if ( face.Area() > 0 )
      aFaces.push_back( face );

    // 2nd facet
    pt1.getValue(fX, fY, fZ);
    face._aclPoints[0].Set(fX, fY, fZ);
    pt2.getValue(fX, fY, fZ);
    face._aclPoints[1].Set(fX, fY, fZ);
    pt4.getValue(fX, fY, fZ);
    face._aclPoints[2].Set(fX, fY, fZ);
    if ( face.Area() > 0 )
      aFaces.push_back( face );

    if ( it+1 < rclPoly.end() )
    {
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
  MeshPolygonTriangulation cTria;
  cTria.SetPolygon( polygon );
  bool ok = cTria.Compute();
  
  std::vector<MeshFacet> faces = cTria.GetFacets();
  for ( std::vector<MeshFacet>::iterator itF = faces.begin(); itF != faces.end(); ++itF )
  {
    MeshGeomFacet topFacet;
    topFacet._aclPoints[0] = top[itF->_aulPoints[0]];
    topFacet._aclPoints[1] = top[itF->_aulPoints[1]];
    topFacet._aclPoints[2] = top[itF->_aulPoints[2]];
    if ( topFacet.GetNormal() * rcNormal < 0 )
    {
      std::swap( topFacet._aclPoints[1], topFacet._aclPoints[2]);
      topFacet.CalcNormal();
    }
    aFaces.push_back( topFacet );

    MeshGeomFacet botFacet;
    botFacet._aclPoints[0] = bottom[itF->_aulPoints[0]];
    botFacet._aclPoints[1] = bottom[itF->_aulPoints[1]];
    botFacet._aclPoints[2] = bottom[itF->_aulPoints[2]];
    if ( botFacet.GetNormal() * rcNormal > 0 )
    {
      std::swap( botFacet._aclPoints[1], botFacet._aclPoints[2]);
      botFacet.CalcNormal();
    }
    aFaces.push_back( botFacet );
  }

  return ok;
}

bool ViewProviderMesh::handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer)
{
  if ( m_bEdit )
  {
    unsetEdit();
    std::vector<SbVec2f> clPoly = Viewer.getPickedPolygon();
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
    std::vector<MeshGeomFacet> aFaces;
    bool ok = createToolMesh( clPoly, vol, cNormal, aFaces );

    Gui::Document* pGDoc = Gui::Application::Instance->activeDocument();
    App::Document* pDoc = pGDoc->getDocument();

    pGDoc->openCommand("Poly pick");
    Gui::Command::doCommand(Gui::Command::Doc, "import Mesh\n");
    Gui::Command::doCommand(Gui::Command::Gui, "import MeshGui\n");

    // create a mesh feature and append it to the document
    std::string fTool = pDoc->getUniqueObjectName("Toolmesh");
    Gui::Command::doCommand(Gui::Command::Doc, "App.document().addObject(\"Mesh::Feature\", \"%s\")\n", fTool.c_str());

    // replace the mesh from feature
    Gui::Command::doCommand(Gui::Command::Doc, "m=App.document().getObject(\"%s\").Mesh\n", fTool.c_str());
    for ( std::vector<MeshGeomFacet>::iterator itF = aFaces.begin(); itF != aFaces.end(); ++itF )
    {
      Gui::Command::doCommand(Gui::Command::Doc, "m.addFacet(%.6f,%.6f,%.6f, %.6f,%.6f,%.6f, %.6f,%.6f,%.6f)",
        itF->_aclPoints[0].x, itF->_aclPoints[0].y, itF->_aclPoints[0].z,
        itF->_aclPoints[1].x, itF->_aclPoints[1].y, itF->_aclPoints[1].z,
        itF->_aclPoints[2].x, itF->_aclPoints[2].y, itF->_aclPoints[2].z);
    }

    Gui::Command::doCommand(Gui::Command::Doc, "App.document().getObject(\"%s\").Mesh=m\n", fTool.c_str());
    Gui::Command::doCommand(Gui::Command::Doc, "App.document().recompute()\n");

#ifndef FC_DEBUG
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.hide(\"%s\")\n", fTool.c_str());
#endif

    // now intersect with each selected mesh feature
    std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());

    for ( std::vector<App::DocumentObject*>::iterator it = fea.begin(); it != fea.end(); ++it )
    {
      // check type
      std::string fName = pDoc->getUniqueObjectName("MeshSegment");
      Feature* meshFeature = dynamic_cast<Feature*>(*it);
      if ( !meshFeature ) continue; // no mesh

      Gui::Command::doCommand(Gui::Command::Doc,
          "f = App.document().addObject(\"Mesh::SegmentByMesh\",\"%s\")\n"
          "f.Source   = App.document().%s\n"
          "f.Tool     = App.document().%s\n"
          "f.Base     = (%.6f,%.6f,%.6f)\n"
          "f.Normal   = (%.6f,%.6f,%.6f)\n"
          , fName.c_str(),  meshFeature->getNameInDocument(), fTool.c_str(), 
            cPoint.x, cPoint.y, cPoint.z, cNormal.x, cNormal.y, cNormal.z );
    }

    pGDoc->commitCommand();
    pDoc->recompute();

#ifndef FC_DEBUG
    // make sure that toolmesh is still hidden
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.hide(\"%s\")\n", fTool.c_str());
#endif

    if ( !ok ) // note: the mouse grabbing needs to be released
      QMessageBox::warning(Viewer.getWidget(),"Invalid polygon","The picked polygon seems to have self-overlappings.\n\nThis could lead to strange rersults.");
  }

  return false;
}

void ViewProviderMesh::showOpenEdges(bool show)
{
  Mesh::Feature* meshFeature = dynamic_cast<Mesh::Feature*>(pcObject);
  if (!meshFeature)
      return;
  if ( show ) {
    SoGroup* pcLineRoot = new SoGroup();
    SoDrawStyle* lineStyle = new SoDrawStyle();
    lineStyle->lineWidth = 3;
    pcLineRoot->addChild(lineStyle);

    // Draw lines
    SoSeparator* linesep = new SoSeparator;
    linesep->addChild(pOpenEdges);
    linesep->addChild(pcMeshCoord);
    SoIndexedLineSet* lines = new SoIndexedLineSet;
    linesep->addChild(lines);
    pcLineRoot->addChild(linesep);
    // add to the top of the node
    pcHighlight->addChild(pcLineRoot/*,0*/);

    // Build up the lines with indices to the list of vertices 'pcMeshCoord'
    int index=0;
    const MeshCore::MeshKernel& rMesh = meshFeature->Mesh.getValue();
    const MeshCore::MeshFacetArray& rFaces = rMesh.GetFacets();
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it ) {
      for ( int i=0; i<3; i++ ) {
        if ( it->_aulNeighbours[i] == ULONG_MAX ) {
          lines->coordIndex.set1Value(index++,it->_aulPoints[i]);
          lines->coordIndex.set1Value(index++,it->_aulPoints[(i+1)%3]);
          lines->coordIndex.set1Value(index++,SO_END_LINE_INDEX);
        }
      }
    }
  } else {
    // remove the indexed line set node and its parent group
    int childs = pcHighlight->getNumChildren();
    if ( pcHighlight->findChild(pcMeshFaces)+1 < childs ) {
      SoNode* node = pcHighlight->getChild(childs-1);
      pcHighlight->removeChild(node);
    }
  }
}
