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
#include <Gui/Document.h>
#include <Gui/SoFCSelection.h>
#include <Gui/MainWindow.h>
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
       
ViewProviderInventorMesh::ViewProviderInventorMesh() : _mouseModel(0), m_bEdit(false)
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

#ifdef _PICKTEST_
  _pickpoints = new SoCoordinate3;
  _pickpoints->ref();
  _pickpoints->point.deleteValues(0);
  _polylines = new SoLineSet;
  _polylines->ref();
#endif
}

ViewProviderInventorMesh::~ViewProviderInventorMesh()
{
  pcMeshCoord->unref();
//  pcMeshNormal->unref();
  pcMeshFaces->unref();
  pcHighlight->unref();
#ifdef _PICKTEST_
  _pickpoints->unref();
  _polylines->unref();
#endif
}

void ViewProviderInventorMesh::createMesh(Mesh::MeshWithProperty *pcMesh)
{
#if 1
  MeshKernel *cMesh = pcMesh->getKernel();
  SbVec3f* vertices = new SbVec3f[cMesh->CountPoints()];
  int* faces = new int [4*cMesh->CountFacets()];

  Base::Sequencer().start( "Building View node...", cMesh->CountFacets() );

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
	pcMeshFaces->coordIndex.setValues(0,4*cMesh->CountFacets(),(const int*) faces);

  delete [] vertices;
  delete [] faces;

  Base::Sequencer().stop();
#else /// @todo This doesn't seem to work as expected (save tmp. memory and time). Don't know why!?
  MeshKernel *cMesh = pcMesh->getKernel();
  pcMeshCoord->point.setNum( cMesh->CountPoints() );
  pcMeshFaces->coordIndex.setNum( 4*cMesh->CountFacets() );

  Base::Sequencer().start( "Building View node...", cMesh->CountFacets() );

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

  Base::Sequencer().stop();
#endif
}

void ViewProviderInventorMesh::attach(App::Feature *pcFeat)
{
  // standard viewing (flat) must be called before attach()
  pcModeSwitch->whichChild = 0;

  // call father (set material and feature pointer)
  ViewProviderInventorFeature::attach(pcFeat);

  // get and save the feature
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  if ( !meshFea )
    throw "ViewProviderInventorMesh::attach(): wrong feature attached!";

  // create the mesh core nodes
  createMesh(&(meshFea->getMesh()));


  // some helper Separators
  SoGroup* pcFlatRoot = new SoGroup();
//  SoGroup* pcFlatNormRoot = new SoGroup();
  SoGroup* pcWireRoot = new SoGroup();
  SoGroup* pcPointRoot = new SoGroup();
  SoGroup* pcFlatWireRoot = new SoGroup();
//  SoGroup* pcColorShadedRoot = new SoGroup();

  // only one selection node for the mesh
  pcHighlight->featureName = pcFeature->getName();
  pcHighlight->documentName = pcFeature->getDocument().getName();
  pcHighlight->subElementName = "Main";
  pcHighlight->addChild(pcMeshCoord);
  pcHighlight->addChild(pcMeshFaces);

  // flat shaded (Normal) ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  SoNormalBinding* pcBinding = new SoNormalBinding();
	pcBinding->value=SoNormalBinding::PER_FACE;

  pcFlatRoot->addChild(pcFlatStyle);
  pcFlatRoot->addChild(pcShadedMaterial);
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
  pcFlatNormRoot->addChild(pcShadedMaterial);
  pcFlatNormRoot->addChild(h); */

  // wire part ----------------------------------------------
  SoDrawStyle *pcWireStyle = new SoDrawStyle();
  pcWireStyle->style = SoDrawStyle::LINES;
  pcWireStyle->lineWidth = fLineSize;
  SoLightModel *pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  pcWireRoot->addChild(pcWireStyle);
  pcWireRoot->addChild(pcLightModel);
  pcWireRoot->addChild(pcLineMaterial);
  pcWireRoot->addChild(pcHighlight);

  // points part ---------------------------------------------
  SoDrawStyle *pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = fPointSize;
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
  pcFlatWireRoot->addChild(pcShadedMaterial);
  pcFlatWireRoot->addChild(pcHighlight);
  pcWireStyle = new SoDrawStyle();
  pcWireStyle->style = SoDrawStyle::LINES;
  pcWireStyle->lineWidth = 2.0;
  pcLightModel = new SoLightModel();
  pcLightModel->model = SoLightModel::BASE_COLOR;
  pcFlatWireRoot->addChild(pcWireStyle);
  pcFlatWireRoot->addChild(pcLightModel);
  pcFlatWireRoot->addChild(pcLineMaterial);
  pcFlatWireRoot->addChild(pcMeshCoord);
  pcFlatWireRoot->addChild(pcMeshFaces);


  // puting all togetern with a switch
  pcModeSwitch->addChild(pcFlatRoot);
//  pcSwitch->addChild(pcFlatNormRoot);
  pcModeSwitch->addChild(pcWireRoot);
  pcModeSwitch->addChild(pcPointRoot);
  pcModeSwitch->addChild(pcFlatWireRoot);

#ifdef _PICKTEST_
  // color shaded  ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;

  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;

  SoGroup* pcColorShadedRoot = new SoGroup();
  pcColorShadedRoot->addChild(pcFlatStyle);
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
  pcColorShadedRoot->addChild(pcHighlight);

#if 0
  SoSeparator * picksep = new SoSeparator;
  SoPickStyle * pickstyle = new SoPickStyle;
  pickstyle->style = SoPickStyle::UNPICKABLE;
  picksep->addChild(pickstyle);

  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = 3;
  drawstyle->lineWidth = 3;
  picksep->addChild(drawstyle);

  // container for picked points
  picksep->addChild(_pickpoints);

  SoMaterial* pMaterial = new SoMaterial();
//  pcSoMat->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
  pMaterial->diffuseColor.setValue(1.0f,1.0f,1.0f);
//  pcSoMat->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
//  pcSoMat->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
//  pcSoMat->shininess.setValue(Mat.shininess);
//  pcSoMat->transparency.setValue(Mat.transparency);
  picksep->addChild(pMaterial);
  picksep->addChild(new SoPointSet);
  picksep->addChild(_polylines);
  pcColorShadedRoot->addChild(picksep);

  SoEventCallback * ecb = new SoEventCallback;
  ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), eventCallback, this);
  pcColorShadedRoot->addChild(ecb);
#endif

  pcModeSwitch->addChild(pcColorShadedRoot);
#endif // _PICKTEST_
}

void ViewProviderInventorMesh::updateData(void)
{
  // get the mesh
  MeshFeature* meshFea = dynamic_cast<MeshFeature*>(pcFeature);
  createMesh(&(meshFea->getMesh()));
}


vector<string> ViewProviderInventorMesh::getModes(void)
{
  // get the modes of the father
  vector<string> StrList = ViewProviderInventorFeature::getModes();

  // add your own modes
  StrList.push_back("Flat");
  StrList.push_back("Wire");
  StrList.push_back("Point");
  StrList.push_back("FlatWire");
#ifdef _PICKTEST_
  StrList.push_back("Polygon picking");
#endif

  return StrList;
}

void ViewProviderInventorMesh::setEdit(void)
{
  if ( m_bEdit ) return;
//  ViewProviderInventorFeature::setEdit();
  m_bEdit = true;
  _timer.start();
}

void ViewProviderInventorMesh::unsetEdit(void)
{
//  ViewProviderInventorFeature::unsetEdit();
  m_bEdit = false;
  _mouseModel->releaseMouseModel();
  delete _mouseModel;
  _mouseModel = 0;
}

const char* ViewProviderInventorMesh::getEditModeName(void)
{
  return "Polygon picking";
}

bool ViewProviderInventorMesh::createToolMesh( const SbViewVolume& vol, const Base::Vector3D& rcNormal, std::vector<MeshCore::MeshGeomFacet>& aFaces) const
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

bool ViewProviderInventorMesh::handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer)
{
  if ( m_bEdit && !_mouseModel )
  {
    _mouseModel = new PolyPickerMouseModel();
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

          MeshKernel cToolMesh;
          cToolMesh = aFaces;

          if ( !ok )
            QMessageBox::warning(Viewer.getWidget(),"Invalid polygon","The picked polygon seems to have self-overlappings.\n\nThis could lead to strange rersults.");

#ifdef FC_DEBUG
          {
          // create a mesh feature and append it to the document
          Gui::Document* pGDoc = Gui::Application::Instance->activeDocument();
          App::Document* pDoc = pGDoc->getDocument();
          Mesh::MeshFeature* fea = dynamic_cast<Mesh::MeshFeature*>(pDoc->addFeature("Mesh", "Toolmesh"));
          if ( fea )
          {
            // replace the mesh from feature
            fea->setSolidTransparency( 0.5f );
            fea->addProperty("String", "Toolmesh");
            fea->getMesh().getKernel()->MeshKernel::operator =( aFaces );
            fea->TouchProperty("Toolmesh");
            pDoc->Recompute();
          }
          }
#endif
          std::vector<App::Feature*> fea = Gui::Selection().getSelectedFeatures("Mesh");
          if ( fea.size() == 1 && dynamic_cast<MeshFeature*>(fea.front()))
          {
            std::vector<unsigned long> faces;
            MeshKernel* rcMesh = dynamic_cast<MeshFeature*>(fea.front())->getMesh().getKernel();

            // get all facets inside the tool mesh
            MeshAlgorithm cAlgo(*rcMesh);
            cAlgo.GetFacetsFromToolMesh( cToolMesh, cNormal, faces );

            // now we have too many facets since we have (invisible) facets near to the back clipping plane, 
            // so we need the nearest facet to the front clipping plane
            //
            float fDist = FLOAT_MAX;
            unsigned long uIdx=ULONG_MAX;
            MeshFacetIterator cFIt(*rcMesh);

            // get the nearest facet to the user (front clipping plane)
            for ( std::vector<unsigned long>::iterator it = faces.begin(); it != faces.end(); ++it )
            {
              cFIt.Set(*it);
              float dist = fabs(cFIt->GetGravityPoint().DistanceToPlane( cPoint, cNormal ));
              if ( dist < fDist )
              {
                fDist = dist;
                uIdx = *it;
              }
            }

            // succeeded
            if ( uIdx != ULONG_MAX )
            {
              // set VISIT-Flag to all outer facets
              cAlgo.SetFacetFlag( MeshFacet::VISIT );
              cAlgo.ResetFacetsFlag(faces, MeshFacet::VISIT);

              faces.clear();
              MeshTopFacetVisitor clVisitor(faces);
              rcMesh->VisitNeighbourFacets(clVisitor, uIdx);
              
              // append also the start facet
              faces.push_back(uIdx);
   
              float fArea = 0.0f;
              // Calculate the area of the facets inside the picked polygon
              aFaces.clear();
              for ( std::vector<unsigned long>::iterator it = faces.begin(); it != faces.end(); ++it )
              {
                cFIt.Set(*it);
                fArea += cFIt->Area();
                aFaces.push_back( *cFIt );
              }

              // create a mesh feature and append it to the document
              Gui::Document* pGDoc = Gui::Application::Instance->activeDocument();
              App::Document* pDoc = pGDoc->getDocument();
              Mesh::MeshFeature* fea = dynamic_cast<Mesh::MeshFeature*>(pDoc->addFeature("Mesh", "Segment"));
              if ( fea )
              {
                // replace the mesh from feature
                fea->addProperty("String", "Segment");
                fea->getMesh().getKernel()->MeshKernel::operator =( aFaces );
                fea->TouchProperty("Segment");
                pDoc->Recompute();
              }

              Base::Console().Message("Area of surface: %.2fmm²", fArea);
            }
            else
            {
              QMessageBox::warning( Viewer.getWidget(), "No facets found", "Sorry, couldn't find any facets inside the picked polygon area." );
            }
          }

          unsetEdit();
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

#ifdef _PICKTEST_
// Calculates azimuth angle for the "directionvec" input argument. An
// azimuth angle is the angle of a vector versus the direction of
// compass north, where 0° means the input vector points straight
// north, 90° is east, 180° south and 270° is west.
float ViewProviderInventorMesh::calcAzimuthAngle(SbVec3f north, SbVec3f east, SbVec3f directionvec)
{
  // Make compass plane.
  SbPlane compassplane(SbVec3f(0, 0, 0), north, east);

  // Project input vector into the compass plane.
  float dist2plane = directionvec.dot(compassplane.getNormal()) - compassplane.getDistanceFromOrigin();
  directionvec = directionvec - (compassplane.getNormal() * dist2plane);

  // Find absolute angle in the [0, PI] range versus the north vector.
  directionvec.normalize();
  north.normalize();
  float angle = acos(north.dot(directionvec));

  // Check if the input argument vector is pointing to the "west side"
  // of the north-south axis, and if so, correct the absolute angle.
  if (directionvec.dot(east) < 0.0f) { angle = M_PI + (M_PI - angle); }

  return angle;
}

// Calculates elevation angle of the "directionvec" argument versus
// the given ground plane. The angle returned is in the range [0,PI/2]
// for a vector pointing "up" versus the plane, and in the range
// <0,-PI/2] for a vector pointing into the ground (ie in the opposite
// direction of the ground plane's normal vector).
float ViewProviderInventorMesh::calcElevationAngle(SbPlane groundplane, SbVec3f directionvec)
{
  // Project input vector into a mirrored vector in the ground plane.
  float dist2plane = directionvec.dot(groundplane.getNormal()) - groundplane.getDistanceFromOrigin();
  SbVec3f groundvec = directionvec - (groundplane.getNormal() * dist2plane);

  // Find absolute angle in the [0, PI/2] range versus the projected
  // groundplane vector.
  directionvec.normalize();
  groundvec.normalize();
  float angle = acos(groundvec.dot(directionvec));

  // Check if the input argument vector is pointing into the ground,
  // and if so, correct the absolute angle.
  SbVec3f n = groundplane.getNormal();
  n.normalize();
  if (directionvec.dot(n) < 0.0f) { angle = -angle; }

  return angle;
}

void ViewProviderInventorMesh::eventCallback(void * ud, SoEventCallback * n)
{
  const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();

  if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 &&
      mbe->getState() == SoButtonEvent::DOWN) {
    SoQtViewer* viewer = 0;
    Gui::View3DInventor* viewIv = dynamic_cast<Gui::View3DInventor*>(Gui::getMainWindow()->activeWindow());
    if ( viewIv )
      viewer = viewIv->getViewer();
    else
      return; // not a viewer

    ViewProviderInventorMesh* that = reinterpret_cast<ViewProviderInventorMesh*>(ud);

    SoRayPickAction rp(viewer->getViewportRegion());
    rp.setPoint(mbe->getPosition());
    rp.apply(viewer->getSceneManager()->getSceneGraph());

    SoPickedPoint * point = rp.getPickedPoint();
    if (point == NULL) {
//      Base::Console().Log("\n** miss! **\n\n");
      return;
    }

    n->setHandled();

    Base::Console().Message("\n");

    SbVec3f v = point->getPoint();
    SbVec3f nv = point->getNormal();

    // Adds a point marker to the picked point.
    const int idx = that->_pickpoints->point.getNum();
    that->_pickpoints->point.set1Value(idx, v);
    that->_polylines->numVertices.set1Value(0,idx+1);

    Base::Console().Message("point (%d)=<%f, %f, %f>, normvec=<%f, %f, %f>\n", idx, v[0], v[1], v[2], nv[0], nv[1], nv[2]);

    const SoCamera * camera = viewer->getCamera();
    SbVec3f camerapos = viewer->getCamera()->position.getValue();
    SbVec3f hitvec = v - camerapos;
    Base::Console().Message("hitvec=<%f, %f, %f>\n", hitvec[0], hitvec[1], hitvec[2]);

    // Find and print the azimuth angle of the vector from the camera
    // position to the point that was picked.
    const SbVec3f NORTH(0, 0, -1);
    const SbVec3f EAST(1, 0, 0);
    float azimuth = calcAzimuthAngle(NORTH, EAST, hitvec);
    Base::Console().Message("Azimuth angle: %f\n", azimuth * 180.0f / M_PI);

    // Find and print the elevation angle of the vector from the
    // camera position to the point that was picked.
    const SbPlane GROUNDPLANE(SbVec3f(0, 1, 0), 0);
    float elevation = calcElevationAngle(GROUNDPLANE, hitvec);
    Base::Console().Message("Elevation angle: %f\n", elevation * 180.0f / M_PI);
  }
}
#endif // _PICKTEST_

// ----------------------------------------------------------------------------

#ifndef _PreComp_
# include <assert.h>
# include <stack>
# include <vector>
# include <qapplication.h>
# include <qcursor.h>
# include <qevent.h>
# include <qpainter.h>
# include <qpixmap.h>
# include <qvbox.h>
#endif

#include <Base/Console.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

using namespace Gui; 

AbstractMouseModel::AbstractMouseModel() : _pcView3D(0)
{
}

void AbstractMouseModel::grabMouseModel(Gui::View3DInventorViewer* viewer)
{
  _pcView3D=viewer;
  m_cPrevCursor = _pcView3D->getWidget()->cursor();

  // do initialization of your mousemodel
  initialize();
}

void AbstractMouseModel::releaseMouseModel()
{
  // do termination of your mousemodel
  terminate();

  _pcView3D->getWidget()->setCursor(m_cPrevCursor);
  _pcView3D = 0;
}

void AbstractMouseModel::mouseMoveEvent (QMouseEvent *cEvent)
{
  // do all the drawing stuff for us
  QPoint clPoint = cEvent->pos();
  draw();
  m_iXnew = clPoint.x(); 
  m_iYnew = clPoint.y();
  draw();
}

void AbstractMouseModel::wheelMouseEvent (QWheelEvent *cEvent)
{
  // do all the drawing stuff for us
  //QPoint clPoint = cEvent->pos();
  draw();
  wheelEvent(cEvent);
  draw();
}

void AbstractMouseModel::mousePressEvent(QMouseEvent* cEvent)
{
  if ( cEvent->type() == QEvent::MouseButtonDblClick )
  {
    mouseDoubleClickEvent(cEvent);
    return;
  }

  switch (cEvent->button())
  {
    case Qt::LeftButton:
      mouseLeftPressEvent(cEvent);
      draw();
      break;
    case Qt::MidButton:
      mouseMiddlePressEvent(cEvent);
      break;
    case Qt::RightButton:
      mouseRightPressEvent(cEvent);
      break;
    default:
      break;
  };
}

void AbstractMouseModel::mouseReleaseEvent(QMouseEvent* cEvent)
{
  switch (cEvent->button())
  {
    case Qt::LeftButton:
      draw();
      mouseLeftReleaseEvent(cEvent);
      break;
    case Qt::MidButton:
      mouseMiddleReleaseEvent(cEvent);
      break;
    case Qt::RightButton:
      mouseRightReleaseEvent(cEvent);
      break;
    default:
      break;
  };
}

// **** BaseMouseModel ********************************************************

BaseMouseModel::BaseMouseModel()
  :AbstractMouseModel()
{
}

// -----------------------------------------------------------------------------------

PolyPickerMouseModel::PolyPickerMouseModel() 
{
  m_iRadius    = 2;
  m_iNodes     = 0;
  m_bWorking   = false;
  m_bDrawNodes = true;
}

void PolyPickerMouseModel::initialize()
{
  _pcView3D->getWidget()->setCursor(QCursor(QCursor::CrossCursor));
}

void PolyPickerMouseModel::terminate()
{
}

void PolyPickerMouseModel::draw ()
{
  if ( m_bWorking )
  {
    if (m_iNodes < int(_cNodeVector.size()))
    {
      m_iNodes = int(_cNodeVector.size());
      // drawing the point
      if (m_bDrawNodes == true)
      {
        _pcView3D->drawNode(m_iXnew-m_iRadius,m_iYnew-m_iRadius,2*m_iRadius,2*m_iRadius);
      }

      if ( _cNodeVector.size() > 2 )
      {
        QPoint start = _cNodeVector.front();
        _pcView3D->drawLine(m_iXnew,m_iYnew,start.x(), start.y() );
      }
    }
    else
    {
      _pcView3D->drawLine(m_iXnew,m_iYnew,m_iXold,m_iYold );
      if ( _cNodeVector.size() > 1 )
      {
        QPoint start = _cNodeVector.front();
        _pcView3D->drawLine(m_iXnew,m_iYnew,start.x(), start.y() );
      }
    }
  }
}

PolyPickerMouseModel::~PolyPickerMouseModel()
{
}

void PolyPickerMouseModel::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  // start working from now on
  if ( !m_bWorking )    
  {
    m_bWorking = true;
    // clear the old polygon
    _cNodeVector.clear();
    _pcView3D->getGLWidget()->update();
  }

  _cNodeVector.push_back(point);

  m_iXnew = point.x();  m_iYnew = point.y();
  m_iXold = point.x();  m_iYold = point.y();
}

void PolyPickerMouseModel::mouseMiddlePressEvent	 ( QMouseEvent *cEvent)
{
}

void PolyPickerMouseModel::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
}

void PolyPickerMouseModel::wheelEvent ( QWheelEvent * e)
{
  // do nothing
}

void PolyPickerMouseModel::keyPressEvent ( QKeyEvent * e)
{
  switch (e->key())
  {
    case Qt::Key_Escape:
//      _pcView3D->popMouseModel();
      break;
    default:
      BaseMouseModel::keyPressEvent(e);
      break;
  }
}

void PolyPickerMouseModel::mouseDoubleClickEvent	 ( QMouseEvent *cEvent)
{
  if( m_bWorking )
  {
    m_bWorking = false;
  }

//  _pcView3D->popMouseModel();
}

// -----------------------------------------------------------------------------------

SelectionMouseModel::SelectionMouseModel()
{
  m_bWorking = false;
}

SelectionMouseModel::~SelectionMouseModel()
{
}

void SelectionMouseModel::initialize()
{
}

void SelectionMouseModel::terminate()
{
}

void SelectionMouseModel::draw ()
{
  if (m_bWorking)
    _pcView3D->drawRect( m_iXold, m_iYold, m_iXnew, m_iYnew );
}

void SelectionMouseModel::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
  m_bWorking = true;
  QPoint p = cEvent->pos();
  m_iXold = m_iXnew = p.x(); 
  m_iYold = m_iYnew = p.y();
}

void SelectionMouseModel::mouseLeftReleaseEvent	 ( QMouseEvent *cEvent)
{
  m_bWorking = false;
}

// -----------------------------------------------------------------------------------

/* XPM */
static const char *xpm_cursor[]={
"32 32 2 1",
". c None",
"# c #ffffff",
"................................",
"................................",
"................................",
"................................",
".............######.............",
"..........###......###..........",
".........#............#.........",
".......##..............##.......",
"......#..................#......",
".....#....................#.....",
".....#....................#.....",
"....#......................#....",
"....#......................#....",
"....#......................#....",
"...#........................#...",
"...#...........##...........#...",
"...#...........##...........#...",
"...#........................#...",
"....#......................#....",
"....#......................#....",
"....#......................#....",
".....#....................#.....",
".....#....................#.....",
"......#..................#......",
".......##..............##.......",
".........#............#.........",
"..........###......###..........",
".............######.............",
"................................",
"................................",
"................................",
"................................"};

CirclePickerMouseModel::CirclePickerMouseModel()
: _nRadius(50)
{
  QPoint p = QCursor::pos();
  m_iXnew = p.x(); 
  m_iYnew = p.y();
}

CirclePickerMouseModel::~CirclePickerMouseModel()
{
}

void CirclePickerMouseModel::initialize()
{
  QPixmap p(xpm_cursor);
  QCursor cursor( p );
  _pcView3D->getWidget()->setCursor(cursor);
}

void CirclePickerMouseModel::terminate()
{
  draw();
}

void CirclePickerMouseModel::draw ()
{
  char szBuf[20];
  float fRad = 0.0f;//float(getView()->Convert(Standard_Integer(_nRadius)));

  sprintf(szBuf, "%.2f", fRad);
  _pcView3D->drawCircle(m_iXnew, m_iYnew, _nRadius);
  _pcView3D->drawText(m_iXnew+9, m_iYnew-9, szBuf);
}

void CirclePickerMouseModel::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
//  _pcView3D->popMouseModel();
}

void CirclePickerMouseModel::wheelEvent			    ( QWheelEvent  * cEvent )
{
  int delta = cEvent->delta();
  _nRadius = 5>(_nRadius + delta / 10)?5:(_nRadius + delta / 10);
}
