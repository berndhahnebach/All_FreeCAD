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

#include "ViewProvider.h"


using namespace MeshGui;
using Mesh::Feature;

using MeshCore::MeshAlgorithm;
using MeshCore::MeshKernel;
using MeshCore::MeshPointIterator;
using MeshCore::MeshFacetIterator;
using MeshCore::MeshGeomFacet;
using MeshCore::MeshFacet;
using MeshCore::MeshFacetGrid;
using MeshCore::MeshPolygonTriangulation;
using MeshCore::MeshEvalSolid;

PROPERTY_SOURCE(MeshGui::ViewProviderExport, Gui::ViewProviderDocumentObject)

ViewProviderExport::ViewProviderExport()
{
}

ViewProviderExport::~ViewProviderExport()
{
}

std::vector<std::string> ViewProviderExport::getModes(void)
{
  return std::vector<std::string>();
}

QPixmap ViewProviderExport::getIcon() const
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

// ======================================================================

PROPERTY_SOURCE(MeshGui::ViewProviderMesh, Gui::ViewProviderDocumentObject)



ViewProviderMesh::ViewProviderMesh() : _mouseModel(0), m_bEdit(false)
{
  ADD_PROPERTY(SolidMaterial,(0.8f,0.8f,0.8f));
  ADD_PROPERTY(LineWidth,(2.0f));
  ADD_PROPERTY(Display,(""));
  ADD_PROPERTY(Transparency,(0));
  ADD_PROPERTY(Visibility,(true));
  ADD_PROPERTY(OpenEdges,(false));

  Display.setSize(6);
  Display.set1Value(0,"Flat"/*"Shaded"*/); // active mode
  Display.set1Value(1,"Flat"/*"Shaded"*/);
  Display.set1Value(2,"Wire"/*"Wireframe"*/);
  Display.set1Value(3,"Point");
  Display.set1Value(4,"FlatWire"/*"Shaded&Wire"*/);
  Display.set1Value(5,"Hidden line");

  // create the mesh core nodes
  pcMeshCoord     = new SoCoordinate3();
  pcMeshCoord->ref();
//  pcMeshNormal    = new SoNormal();
//  pcMeshNormal->ref();
  pcMeshFaces     = new SoIndexedFaceSet();
  pcMeshFaces->ref();
}

ViewProviderMesh::~ViewProviderMesh()
{
  pcMeshCoord->unref();
//  pcMeshNormal->unref();
  pcMeshFaces->unref();
}

void ViewProviderMesh::onChanged(const App::Property* prop)
{
  if ( prop == &SolidMaterial ) {
    setColor(SolidMaterial.getValue());
  } else if ( prop == &LineWidth ) {
    pcLineStyle->lineWidth = LineWidth.getValue();
  } else if ( prop == &Transparency ) {
    setTransparency( Transparency.getValue()/100.0f );
  } else if ( prop == &Display ) {
    setMode( Display.getValues().front().c_str() );
  } else if ( prop == &Visibility ) {
    Visibility.getValue() ? show() : hide();
  } else if ( prop == &OpenEdges ) {
    showOpenEdges( OpenEdges.getValue() );
  }
}

void ViewProviderMesh::createMesh( const MeshCore::MeshKernel& rcMesh )
{
#if 1

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
      MeshFacet aFace = cFIt.GetIndicies();

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

#else /// @todo This doesn't seem to work as expected (save tmp. memory and time). Don't know why!?
  MeshKernel *cMesh = pcMesh->getKernel();
  pcMeshCoord->point.setNum( cMesh->CountPoints() );
  pcMeshFaces->coordIndex.setNum( 4*cMesh->CountFacets() );

  Base::SequencerLauncher seq( "Building View node...", cMesh->CountFacets() );

  // set the point coordinates
  MeshPointIterator cPIt(*cMesh);
  for ( cPIt.Init(); cPIt.More(); cPIt.Next() )
  {
    pcMeshCoord->point.set1Value( cPIt.Position(), cPIt->x, cPIt->y, cPIt->z );
  }

  // set the facets indices
  unsigned long j=0;
  MeshFacetIterator cFIt(*cMesh);
  for ( cFIt.Init(); cFIt.More(); cFIt.Next(), j++ )
  {
    MeshFacet aFace = cFIt.GetIndicies();

    for ( int i=0; i<3; i++ )
    {
      pcMeshFaces->coordIndex.set1Value(4*j+i, aFace._aulPoints[i]);
    }

    pcMeshFaces->coordIndex.set1Value(4*j+3, SO_END_FACE_INDEX);
    Base::Sequencer().next( false ); // don't allow to cancel
  }

#endif
}

void ViewProviderMesh::attach(App::DocumentObject *pcFeat)
{
  pcObject = pcFeat;

  // some helper Separators
  SoGroup* pcFlatRoot = new SoGroup();
//  SoGroup* pcFlatNormRoot = new SoGroup();
  SoGroup* pcWireRoot = new SoGroup();
  SoGroup* pcPointRoot = new SoGroup();
  SoGroup* pcFlatWireRoot = new SoGroup();
  SoGroup* pcHiddenLineRoot = new SoGroup();
//  SoGroup* pcColorShadedRoot = new SoGroup();

  // only one selection node for the mesh
  pcHighlight->objectName = getAsFeature()->name.getValue();
  pcHighlight->documentName = getAsFeature()->getDocument().getName();
  pcHighlight->subElementName = "Main";
  pcHighlight->addChild(pcMeshCoord);
  pcHighlight->addChild(pcMeshFaces);

  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
  pcBinding->value=SoNormalBinding::PER_FACE;
  
  // get and save the feature
  Feature* meshFea = dynamic_cast<Feature*>(pcFeat);
  MeshEvalSolid cEval(meshFea->getMesh());

  // if no solid then enable two-side rendering
  if ( !cEval.Evaluate() )
  {
    SoShapeHints * flathints = new SoShapeHints;
    flathints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
    flathints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    pcFlatRoot->addChild(flathints);
  }

  pcFlatRoot->addChild(pcFlatStyle);
  pcFlatRoot->addChild(pcSolidMaterial);
  pcFlatRoot->addChild(pcBinding);
  pcFlatRoot->addChild(pcHighlight);


/*  // flat shaded (per Vertex normales) ------------------------------------------
  pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcBinding = new SoNormalBinding();
  pcBinding->value=SoNormalBinding::PER_VERTEX;
  h = new SoLocateHighlight();
  h->color.setValue((float)0.2,(float)0.5,(float)0.2);
  h->addChild(pcBinding);
  h->addChild(pcMeshCoord);
  h->addChild(pcMeshNormal);
  h->addChild(pcMeshFaces);
  pcFlatNormRoot->addChild(pcFlatStyle);
  pcFlatNormRoot->addChild(pcSolidMaterial);
  pcFlatNormRoot->addChild(h); */

  // wire part ----------------------------------------------
  //SoDrawStyle *pcWireStyle = new SoDrawStyle();
  //pcWireStyle->style = SoDrawStyle::LINES;
  //pcWireStyle->lineWidth = fLineSize;
  SoLightModel *pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  pcWireRoot->addChild(pcLineStyle);
  pcWireRoot->addChild(pcLightModel);
  pcWireRoot->addChild(pcLineMaterial);
  pcWireRoot->addChild(pcHighlight);

  // points part ---------------------------------------------
  //SoDrawStyle *pcPointStyle = new SoDrawStyle();
  //pcPointStyle->style = SoDrawStyle::POINTS;
  //pcPointStyle->pointSize = fPointSize;
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
  pcPointRoot->addChild(pcHighlight);

  // wire shaded  ------------------------------------------
  pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcBinding = new SoNormalBinding();
  pcBinding->value=SoNormalBinding::PER_FACE;
  pcFlatWireRoot->addChild(pcBinding);
  pcFlatWireRoot->addChild(pcFlatStyle);
  pcFlatWireRoot->addChild(pcSolidMaterial);
  pcFlatWireRoot->addChild(pcHighlight);
  pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  pcFlatWireRoot->addChild(pcLineStyle);
  pcFlatWireRoot->addChild(pcLightModel);
  pcFlatWireRoot->addChild(pcLineMaterial);
  pcFlatWireRoot->addChild(pcMeshCoord);
  pcFlatWireRoot->addChild(pcMeshFaces);

 // Turns on backface culling
  SoShapeHints * wirehints = new SoShapeHints;
  wirehints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
  wirehints->shapeType = SoShapeHints::SOLID;
  wirehints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  pcHiddenLineRoot->addChild(wirehints);
  pcHiddenLineRoot->addChild(pcLineStyle);
  pcHiddenLineRoot->addChild(pcLightModel);
  pcHiddenLineRoot->addChild(pcLineMaterial);
  pcHiddenLineRoot->addChild(pcHighlight);

  // putting all together with a switch
  addDisplayMode(pcFlatRoot, "Flat");
//  addDisplayMode(pcFlatNormRoot, "Normal");
  addDisplayMode(pcWireRoot, "Wireframe");
  addDisplayMode(pcPointRoot, "Point");
  addDisplayMode(pcFlatWireRoot, "FlatWireframe");
  addDisplayMode(pcHiddenLineRoot, "HiddenLine");

  // call father (set material and feature pointer)
  ViewProviderDocumentObject::attach(pcFeat);

  // create the mesh core nodes
  updateData();
}

void ViewProviderMesh::updateData(void)
{
  Mesh::Feature* mesh = dynamic_cast<Mesh::Feature*>(pcObject);
  createMesh(mesh->getMesh());
}

QPixmap ViewProviderMesh::getIcon() const
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

void ViewProviderMesh::setMode(const char* ModeName)
{
  if ( strcmp("Flat",ModeName)==0 )
    setDisplayMode("Flat");
  else if ( strcmp("Wire",ModeName)==0 )
    setDisplayMode("Wireframe");
  else if ( strcmp("Point",ModeName)==0 )
    setDisplayMode("Point");
  else if ( strcmp("FlatWire",ModeName)==0 )
    setDisplayMode("FlatWireframe");
  else if ( strcmp("Hidden line",ModeName)==0 )
    setDisplayMode("HiddenLine");

  ViewProviderDocumentObject::setMode( ModeName );
}

std::vector<std::string> ViewProviderMesh::getModes(void)
{
  // get the modes of the father
  std::vector<std::string> StrList = ViewProviderDocumentObject::getModes();

  // add your own modes
  StrList.push_back("Flat");
  StrList.push_back("Wire");
  StrList.push_back("Point");
  StrList.push_back("FlatWire");
  StrList.push_back("Hidden line");

  return StrList;
}

void ViewProviderMesh::setEdit(void)
{
  if ( m_bEdit ) return;
//  ViewProviderFeature::setEdit();
  m_bEdit = true;
  _timer.start();
}

void ViewProviderMesh::unsetEdit(void)
{
//  ViewProviderFeature::unsetEdit();
  m_bEdit = false;
  //_mouseModel->releaseMouseModel();
  delete _mouseModel;
  _mouseModel = 0;
}

const char* ViewProviderMesh::getEditModeName(void)
{
  return "Polygon picking";
}

bool ViewProviderMesh::createToolMesh( const std::vector<SbVec2f>& rclPoly, const SbViewVolume& vol, const Base::Vector3f& rcNormal, std::vector<MeshCore::MeshGeomFacet>& aFaces) const
{
  float fX, fY, fZ;
  SbVec3f pt1, pt2, pt3, pt4;
  MeshGeomFacet face;
  std::vector<Base::Vector3f> top, bottom;

  for ( std::vector<SbVec2f>::const_iterator it = rclPoly.begin(); it != rclPoly.end(); ++it )
  {
    // the following element
    std::vector<SbVec2f>::const_iterator nt = it + 1;
    if ( nt == rclPoly.end() )
      nt = rclPoly.begin();
    else if ( *it == *nt )
      continue; // two adjacent verteces are equal

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
    }
  }

  bool ok=true;

  // now create the lids
  std::vector<MeshGeomFacet> aLid;
  MeshPolygonTriangulation cTria;
  cTria.setPolygon( top );
  ok &= cTria.compute();
  aLid = cTria.getFacets();

  // front lid
  for ( std::vector<MeshGeomFacet>::iterator itF1 = aLid.begin(); itF1 != aLid.end(); ++itF1 )
  {
    if ( itF1->GetNormal() * rcNormal < 0 )
    {
      std::swap( itF1->_aclPoints[1], itF1->_aclPoints[2]);
      itF1->CalcNormal();
    }
    aFaces.push_back( *itF1 );
  }

  cTria.setPolygon( bottom );
  ok &= cTria.compute();
  aLid = cTria.getFacets();

  // back lid
  for ( std::vector<MeshGeomFacet>::iterator itF2 = aLid.begin(); itF2 != aLid.end(); ++itF2 )
  {
    if ( itF2->GetNormal() * rcNormal > 0 )
    {
      std::swap( itF2->_aclPoints[1], itF2->_aclPoints[2]);
      itF2->CalcNormal();
    }
    aFaces.push_back( *itF2 );
  }

  return ok;
}

bool ViewProviderMesh::handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer)
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
      Gui::Command::doCommand(Gui::Command::Gui, "App.document().%s.solidMaterial.transparency = 0.7\n", fTool.c_str());
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
            , fName.c_str(),  meshFeature->name.getValue(), fTool.c_str(), 
              cPoint.x, cPoint.y, cPoint.z, cNormal.x, cNormal.y, cNormal.z );
      }

      pGDoc->commitCommand();
      pDoc->recompute();

#ifndef FC_DEBUG
      // make sure that toolmesh is still hidden
      Gui::Command::doCommand(Gui::Command::Gui, "Gui.hide(\"%s\")\n", fTool.c_str());
#endif

      unsetEdit();

      if ( !ok ) // note: the mouse grabbing needs to be released
        QMessageBox::warning(Viewer.getWidget(),"Invalid polygon","The picked polygon seems to have self-overlappings.\n\nThis could lead to strange rersults.");

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

void ViewProviderMesh::showOpenEdges(bool show)
{
  if ( show ) {
    SoGroup* pcLineRoot = new SoGroup();
    SoDrawStyle* lineStyle = new SoDrawStyle();
    lineStyle->lineWidth = 3;
    pcLineRoot->addChild(lineStyle);

    // Draw lines
    SoSeparator* linesep = new SoSeparator;
    SoBaseColor * basecol = new SoBaseColor;
    basecol->rgb.setValue( 1.0f, 1.0f, 0.0f );
    linesep->addChild(basecol);
    linesep->addChild(pcMeshCoord);
    SoIndexedLineSet* lines = new SoIndexedLineSet;
    linesep->addChild(lines);
    pcLineRoot->addChild(linesep);
    // add to the top of the node
    pcHighlight->addChild(pcLineRoot/*,0*/);

    // Build up the lines with indices to the list of vertices 'pcMeshCoord'
    int index=0;
    const MeshCore::MeshKernel& rMesh = dynamic_cast<Mesh::Feature*>(pcObject)->getMesh();
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
