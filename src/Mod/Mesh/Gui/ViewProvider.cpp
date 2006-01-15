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
# include <qapplication.h>
# include <qlistview.h>
# include <qmessagebox.h>
# include <qpainter.h>
# include <Inventor/events/SoEvent.h>
# include <Inventor/events/SoKeyboardEvent.h>
# include <Inventor/events/SoLocation2Event.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoNormal.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Sequencer.h>
#include <Base/Stream.h>
#include <Base/Tools2D.h>
#include <Base/ViewProj.h>

#include <App/Document.h>

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
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/Core/MeshIO.h>
#include <Mod/Mesh/App/Core/Visitor.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include "ViewProvider.h"


using namespace MeshGui;
using Mesh::MeshFeature;

using MeshCore::MeshAlgorithm;
using MeshCore::MeshKernel;
using MeshCore::MeshPointIterator;
using MeshCore::MeshFacetIterator;
using MeshCore::MeshGeomFacet;
using MeshCore::MeshFacet;
using MeshCore::MeshFacetGrid;
using MeshCore::MeshPolygonTriangulation;
using MeshCore::MeshSTL;
using MeshCore::MeshTopFacetVisitor;

using Base::Vector3D;

ViewProviderMesh::ViewProviderMesh() : _mouseModel(0), m_bEdit(false)
{
  // create the mesh core nodes
  pcMeshCoord     = new SoCoordinate3();
  pcMeshCoord->ref();
//  pcMeshNormal    = new SoNormal();
//  pcMeshNormal->ref();
  pcMeshFaces     = new SoIndexedFaceSet();
  pcMeshFaces->ref();
  pcHighlight = new Gui::SoFCSelection();
  pcHighlight->ref();
}

ViewProviderMesh::~ViewProviderMesh()
{
  pcMeshCoord->unref();
//  pcMeshNormal->unref();
  pcMeshFaces->unref();
  pcHighlight->unref();
}

void ViewProviderMesh::createMesh(Mesh::MeshWithProperty *pcMesh)
{
#if 1
  MeshKernel *cMesh = pcMesh->getKernel();
  SbVec3f* vertices = new SbVec3f[cMesh->CountPoints()];
  int* faces = new int [4*cMesh->CountFacets()];

  Base::SequencerLauncher seq( "Building View node...", cMesh->CountFacets() );

  unsigned long j=0;
  MeshFacetIterator cFIt(*cMesh);
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

	pcMeshCoord->point.setValues(0,cMesh->CountPoints(), vertices);
  delete [] vertices;
	pcMeshFaces->coordIndex.setValues(0,4*cMesh->CountFacets(),(const int*) faces);
  delete [] faces;

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

void ViewProviderMesh::attach(App::Feature *pcFeat)
{
  // some helper Separators
  SoGroup* pcFlatRoot = new SoGroup();
//  SoGroup* pcFlatNormRoot = new SoGroup();
  SoGroup* pcWireRoot = new SoGroup();
  SoGroup* pcPointRoot = new SoGroup();
  SoGroup* pcFlatWireRoot = new SoGroup();
//  SoGroup* pcColorShadedRoot = new SoGroup();

  // only one selection node for the mesh
  pcHighlight->featureName = pcFeat->getName();
  pcHighlight->documentName = pcFeat->getDocument().getName();
  pcHighlight->subElementName = "Main";
  pcHighlight->addChild(pcMeshCoord);
  pcHighlight->addChild(pcMeshFaces);

  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;

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

  // putting all together with a switch
  addDisplayMode(pcFlatRoot, "Flat");
//  addDisplayMode(pcFlatNormRoot, "Normal");
  addDisplayMode(pcWireRoot, "Wireframe");
  addDisplayMode(pcPointRoot, "Point");
  addDisplayMode(pcFlatWireRoot, "FlatWireframe");

  // call father (set material and feature pointer)
  ViewProviderFeature::attach(pcFeat);

  // get and save the feature
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  // create the mesh core nodes
  createMesh(&(meshFea->getMesh()));
}

void ViewProviderMesh::updateData(void)
{
  // get the mesh
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  createMesh(&(meshFea->getMesh()));
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

  ViewProviderFeature::setMode( ModeName );
}

std::vector<std::string> ViewProviderMesh::getModes(void)
{
  // get the modes of the father
  std::vector<std::string> StrList = ViewProviderFeature::getModes();

  // add your own modes
  StrList.push_back("Flat");
  StrList.push_back("Wire");
  StrList.push_back("Point");
  StrList.push_back("FlatWire");

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
  _mouseModel->releaseMouseModel();
  delete _mouseModel;
  _mouseModel = 0;
}

const char* ViewProviderMesh::getEditModeName(void)
{
  return "Polygon picking";
}

bool ViewProviderMesh::createToolMesh( const SbViewVolume& vol, const Base::Vector3D& rcNormal, std::vector<MeshCore::MeshGeomFacet>& aFaces) const
{
  float fX, fY, fZ;
  SbVec3f pt1, pt2, pt3, pt4;
  MeshGeomFacet face;
  std::vector<Vector3D> top, bottom;

  for ( std::vector<SbVec2f>::const_iterator it = _clPoly.begin(); it != _clPoly.end(); ++it )
  {
    // the following element
    std::vector<SbVec2f>::const_iterator nt = it + 1;
    if ( nt == _clPoly.end() )
      nt = _clPoly.begin();

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

    if ( it+1 < _clPoly.end() )
    {
      pt1.getValue(fX, fY, fZ);
      top.push_back( Vector3D(fX, fY, fZ) );
      pt2.getValue(fX, fY, fZ);
      bottom.push_back( Vector3D(fX, fY, fZ) );
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
    _clPoly.clear();
  }

  if ( m_bEdit && _mouseModel )
  {
    const SbViewportRegion& vp = Viewer.getViewportRegion();
    const SbVec2s& sz = vp.getWindowSize(); 
    short w,h; sz.getValue(w,h);

    SbVec2s loc = ev->getPosition();
    short x,y; loc.getValue(x,y);
    y = h-y; // the origin is at the left bottom corner (instead of left top corner)

    if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) 
    {
      const SoMouseButtonEvent * const event = (const SoMouseButtonEvent *) ev;
      const int button = event->getButton();
      const SbBool press = event->getState() == SoButtonEvent::DOWN ? TRUE : FALSE;

      Qt::ButtonState state = Qt::ButtonState((ev->wasShiftDown() ? Qt::ShiftButton : Qt::NoButton)|(ev->wasCtrlDown () ? 
                              Qt::ControlButton : Qt::NoButton)|(ev->wasAltDown() ? Qt::AltButton : Qt::NoButton));

      if ( press )
      {
        float fRatio = vp.getViewportAspectRatio();
        SbVec2f pos = ev->getNormalizedPosition(vp);
        float pX,pY; pos.getValue(pX,pY);

        SbVec2f org = vp.getViewportOrigin();
        float Ox, Oy; org.getValue( Ox, Oy );

        SbVec2f siz = vp.getViewportSize();
        float dX, dY; siz.getValue( dX, dY );

        // now calculate the real points respecting aspect ratio information
        //
        if ( fRatio > 1.0f )
        {
          pX = ( pX - 0.5f*dX ) * fRatio + 0.5f*dX;
          pos.setValue(pX,pY);
        }
        else if ( fRatio < 1.0f )
        {
          pY = ( pY - 0.5f*dY ) / fRatio + 0.5f*dY;
          pos.setValue(pX,pY);
        }

        _clPoly.push_back( pos );

        // double click event
        if ( _timer.restart() < QApplication::doubleClickInterval() )
        {
          QMouseEvent e(QEvent::MouseButtonDblClick, QPoint(x,y), Qt::LeftButton, state);
          _mouseModel->mousePressEvent(&e);

          // get the normal of the front clipping plane
          Vector3D cPoint, cNormal;
          Viewer.getFrontClippingPlane(cPoint, cNormal);
          SoCamera* pCam = Viewer.getCamera();  
          SbViewVolume  vol = pCam->getViewVolume (); 

          // create a tool shape from these points
          std::vector<MeshGeomFacet> aFaces;
          bool ok = createToolMesh( vol, cNormal, aFaces );

          Gui::Document* pGDoc = Gui::Application::Instance->activeDocument();
          App::Document* pDoc = pGDoc->getDocument();

          pGDoc->openCommand("Poly pick");
          Gui::Command::doCommand(Gui::Command::Doc, "import Mesh\n");
          Gui::Command::doCommand(Gui::Command::Gui, "import MeshGui\n");

          // create a mesh feature and append it to the document
          std::string fTool = pDoc->getUniqueFeatureName("Toolmesh");
          Gui::Command::doCommand(Gui::Command::Doc, "App.document().AddFeature(\"Mesh\", \"%s\")\n", fTool.c_str());

          // replace the mesh from feature
          Gui::Command::doCommand(Gui::Command::Gui, "App.document().%s.solidMaterial.transparency = 0.7\n", fTool.c_str());
          Gui::Command::doCommand(Gui::Command::Doc, "m=App.document().GetFeature(\"%s\").getMesh()\n", fTool.c_str());
          for ( std::vector<MeshGeomFacet>::iterator itF = aFaces.begin(); itF != aFaces.end(); ++itF )
          {
            Gui::Command::doCommand(Gui::Command::Doc, "m.addFacet(%.6f,%.6f,%.6f, %.6f,%.6f,%.6f, %.6f,%.6f,%.6f)",
              itF->_aclPoints[0].x, itF->_aclPoints[0].y, itF->_aclPoints[0].z,
              itF->_aclPoints[1].x, itF->_aclPoints[1].y, itF->_aclPoints[1].z,
              itF->_aclPoints[2].x, itF->_aclPoints[2].y, itF->_aclPoints[2].z);
          }

          Gui::Command::doCommand(Gui::Command::Doc, "App.document().Recompute()\n");

#ifndef FC_DEBUG
          Gui::Command::doCommand(Gui::Command::Gui, "Gui.hide(\"%s\")\n", fTool.c_str());
#endif

          // now intersect with each selected mesh feature
          std::vector<App::Feature*> fea = Gui::Selection().getSelectedFeatures("Mesh");

          for ( std::vector<App::Feature*>::iterator it = fea.begin(); it != fea.end(); ++it )
          {
            // check type
            std::string fName = pDoc->getUniqueFeatureName("MeshSegment");
            MeshFeature* meshFeature = dynamic_cast<MeshFeature*>(*it);
            if ( !meshFeature ) continue; // no mesh

            Gui::Command::doCommand(Gui::Command::Doc,
                "f = App.document().AddFeature(\"MeshSegmentByMesh\",\"%s\")\n"
                "f.Source   = App.document().%s\n"
                "f.Tool     = App.document().%s\n"
                "f.BaseX    = %.6f\n"
                "f.BaseY    = %.6f\n"
                "f.BaseZ    = %.6f\n"
                "f.NormalX  = %.6f\n"
                "f.NormalY  = %.6f\n"
                "f.NormalZ  = %.6f\n"
                , fName.c_str(),  meshFeature->getName(), fTool.c_str(), 
                  cPoint.x, cPoint.y, cPoint.z, cNormal.x, cNormal.y, cNormal.z );
          }

          pGDoc->commitCommand();
          pDoc->Recompute();

          unsetEdit();

          if ( !ok ) // note: the mouse grabbing needs to be released
            QMessageBox::warning(Viewer.getWidget(),"Invalid polygon","The picked polygon seems to have self-overlappings.\n\nThis could lead to strange rersults.");

          return true;
        }

        switch ( button )
        {
        case SoMouseButtonEvent::BUTTON1:
          {
            QMouseEvent e(QEvent::MouseButtonPress, QPoint(x,y), Qt::LeftButton, state);
            _mouseModel->mousePressEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON2:
          {
            QMouseEvent e(QEvent::MouseButtonPress, QPoint(x,y), Qt::RightButton, state);
            _mouseModel->mousePressEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON3:
          {
            QMouseEvent e(QEvent::MouseButtonPress, QPoint(x,y), Qt::MidButton, state);
            _mouseModel->mousePressEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON4:
          {
            QWheelEvent e(QPoint(x,y), QCursor::pos(), 120, state);
            _mouseModel->wheelMouseEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON5:
          {
            QWheelEvent e(QPoint(x,y), QCursor::pos(), -120, state);
            _mouseModel->wheelMouseEvent(&e);
          } break;
        default:
          {
          } break;
        }
      }
      else
      {
        QMouseEvent e(QEvent::MouseButtonRelease, QPoint(), button, 0);
        switch ( button )
        {
        case SoMouseButtonEvent::BUTTON1:
          {
            QMouseEvent e(QEvent::MouseButtonRelease, QPoint(x,y), Qt::LeftButton, Qt::LeftButton|state);
            _mouseModel->mouseReleaseEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON2:
          {
            QMouseEvent e(QEvent::MouseButtonRelease, QPoint(x,y), Qt::RightButton, Qt::RightButton|state);
            _mouseModel->mouseReleaseEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON3:
          {
            QMouseEvent e(QEvent::MouseButtonRelease, QPoint(x,y), Qt::MidButton, Qt::MidButton|state);
            _mouseModel->mouseReleaseEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON4:
          {
            QWheelEvent e(QPoint(x,y), QCursor::pos(), 120, state);
            _mouseModel->wheelMouseEvent(&e);
          } break;
        case SoMouseButtonEvent::BUTTON5:
          {
            QWheelEvent e(QPoint(x,y), QCursor::pos(), -120, state);
            _mouseModel->wheelMouseEvent(&e);
          } break;
        default:
          {
          } break;
        }
      }
    }
    else if (ev->getTypeId().isDerivedFrom(SoLocation2Event::getClassTypeId())) 
    {
      Qt::ButtonState state = Qt::ButtonState((ev->wasShiftDown() ? Qt::ShiftButton : Qt::NoButton)|(ev->wasCtrlDown () ? 
                              Qt::ControlButton : Qt::NoButton)|(ev->wasAltDown() ? Qt::AltButton : Qt::NoButton));
      
      QMouseEvent e( QEvent::MouseMove, QPoint(x,y), Qt::NoButton, state );
     _mouseModel->mouseMoveEvent(&e);
    }
    else if (ev->getTypeId().isDerivedFrom(SoKeyboardEvent::getClassTypeId())) 
    {
      SoKeyboardEvent * ke = (SoKeyboardEvent *)ev;
      switch (ke->getKey()) 
      {
      case SoKeyboardEvent::ESCAPE:
        unsetEdit();
        break;
      default:
        break;
      }
    }

    return true;
  }

  return false;
}
