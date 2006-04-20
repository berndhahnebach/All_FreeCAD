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
# include <qlistview.h>
# include <qpixmap.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoGroup.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoNormal.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoSwitch.h>
# include <algorithm>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Feature.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>
#include <Gui/SoFCColorBar.h>

#include <Mod/Mesh/App/Mesh.h>

#include "ViewProvider.h"
#include "ViewProviderCurvature.h"

using namespace Mesh;
using namespace MeshGui;
using namespace std;


PROPERTY_SOURCE(MeshGui::ViewProviderMeshCurvature, MeshGui::ViewProviderMesh)

ViewProviderMeshCurvature::ViewProviderMeshCurvature()
{
  pcColorMat = new SoMaterial;
  pcColorMat->ref();
  // simple color bar
  pcColorBar = new Gui::SoFCColorBar;
  pcColorBar->Attach(this);
  pcColorBar->ref();
  pcColorBar->setRange( -0.1f, 0.1f, 3 );

  // switch off preselection
  pcHighlight->highlightMode = Gui::SoFCSelection::OFF;
  pcHighlight->selectionMode = Gui::SoFCSelection::SEL_OFF;
  pcHighlight->style = Gui::SoFCSelection::BOX;
}

ViewProviderMeshCurvature::~ViewProviderMeshCurvature()
{
  pcColorMat->unref();
  pcColorBar->Detach(this);
  pcColorBar->unref();
}

void ViewProviderMeshCurvature::init(App::DocumentObject *pcFeat)
{
  Mesh::PropertyCurvatureList* pCurvInfo=0;
  std::map<std::string,App::Property*> Map;
  pcFeat->getPropertyMap(Map);
  for( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
  {
    Base::Type t = it->second->getTypeId();
    if ( t==Mesh::PropertyCurvatureList::getClassTypeId() )
    {
      pCurvInfo = (Mesh::PropertyCurvatureList*)it->second;
      break;
    }
  }

  if ( !pCurvInfo )
    return; // cannot display this feature type due to missing curvature property

  std::vector<float> aMinValues, aMaxValues;
  const std::vector<Mesh::CurvatureInfo>& fCurvInfo = pCurvInfo->getValues();
  aMinValues.reserve(fCurvInfo.size());
  aMaxValues.reserve(fCurvInfo.size());

  for ( std::vector<Mesh::CurvatureInfo>::const_iterator jt=fCurvInfo.begin();jt!=fCurvInfo.end(); ++jt )
  {
    aMinValues.push_back( jt->fMinCurvature );
    aMaxValues.push_back( jt->fMaxCurvature );
  }

  if ( aMinValues.empty() || aMaxValues.empty() ) 
    return; // no values inside

  float fMin = *std::min_element( aMinValues.begin(), aMinValues.end() );
  float fMax = *std::max_element( aMinValues.begin(), aMinValues.end() );

  // histogram over all values
  std::map<int, int> aHistogram;
  for ( std::vector<float>::const_iterator kt = aMinValues.begin(); kt != aMinValues.end(); ++kt )
  {
    int grp = (int)(10.0f*( *kt - fMin )/( fMax - fMin ));
    aHistogram[grp]++;
  }

  float fRMin=-1.0f;
  for ( std::map<int, int>::iterator mIt = aHistogram.begin(); mIt != aHistogram.end(); ++mIt )
  {
    if ( (float)mIt->second / (float)aMinValues.size() > 0.15f )
    {
      fRMin = mIt->first * ( fMax - fMin )/10.0f + fMin;
      break;
    }
  }

  fMin = *std::min_element( aMaxValues.begin(), aMaxValues.end() );
  fMax = *std::max_element( aMaxValues.begin(), aMaxValues.end() );

  // histogram over all values
  aHistogram.clear();
  for ( std::vector<float>::const_iterator it2 = aMaxValues.begin(); it2 != aMaxValues.end(); ++it2 )
  {
    int grp = (int)(10.0f*( *it2 - fMin )/( fMax - fMin ));
    aHistogram[grp]++;
  }

  float fRMax=1.0f;
  for ( std::map<int, int>::reverse_iterator rIt2 = aHistogram.rbegin(); rIt2 != aHistogram.rend(); ++rIt2 )
  {
    if ( (float)rIt2->second / (float)aMaxValues.size() > 0.15f )
    {
      fRMax = rIt2->first * ( fMax - fMin )/10.0f + fMin;
      break;
    }
  }

  float fAbs = std::max<float>(fabs(fRMin), fabs(fRMax));
  fRMin = -fAbs;
  fRMax =  fAbs;
  fMin = fRMin; fMax = fRMax;
  pcColorBar->setRange( fMin, fMax, 3 );
}

void ViewProviderMeshCurvature::attach(App::DocumentObject *pcFeat)
{
  init( pcFeat ); // init color bar

  SoShapeHints * flathints = new SoShapeHints;
  flathints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
  flathints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

  SoGroup* pcColorShadedRoot = new SoGroup();
  pcColorShadedRoot->addChild(flathints);

  // color shaded  ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcColorShadedRoot->addChild(pcFlatStyle);

  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
  pcColorShadedRoot->addChild(pcHighlight);

  addDisplayMode(pcColorShadedRoot, "ColorShaded");

  // creats the satandard viewing modes
  ViewProviderMesh::attach(pcFeat);
}

void ViewProviderMeshCurvature::updateData(void)
{
  ViewProviderMesh::updateData();
}

SoSeparator* ViewProviderMeshCurvature::getFrontRoot(void)
{
  return pcColorBar;
}

void ViewProviderMeshCurvature::setVertexCurvatureMode(int mode)
{
  Mesh::PropertyCurvatureList* pCurvInfo=0;
  std::map<std::string,App::Property*> Map;
  pcObject->getPropertyMap(Map);
  for( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
  {
    Base::Type t = it->second->getTypeId();
    if ( t==Mesh::PropertyCurvatureList::getClassTypeId() )
    {
      pCurvInfo = (Mesh::PropertyCurvatureList*)it->second;
      break;
    }
  }

  if ( !pCurvInfo )
    return; // cannot display this feature type due to missing curvature property

  // curvature values
  std::vector<float> fValues = pCurvInfo->getCurvature( mode ); 
  unsigned long j=0;
  for ( std::vector<float>::const_iterator jt = fValues.begin(); jt != fValues.end(); ++jt )
  {
    App::Color col = pcColorBar->getColor( *jt );
    pcColorMat->diffuseColor.set1Value(j++, SbColor(col.r, col.g, col.b));
  }
}

QPixmap ViewProviderMeshCurvature::getIcon() const
{
  const char * Mesh_Feature_xpm[] = {
    "16 16 7 1",
    ".	c None",
    "#	c #000000",
    "s	c #BEC2FC",
    "g	c #00FF00",
    "y	c #FFFF00",
    "b	c #0000FF",
    "r	c #FF0000",
    ".......##.......",
    "....#######.....",
    "..##ggg#yyy#....",
    "##ggggg#yyyy##..",
    "#b#ggg#yyyyyy##.",
    "#bb#gg#yyyy###s.",
    "#bb#gg#yy##yy#s.",
    "#bbb#####rrr#ss.",
    "#bbbb##rrrr#ss..",
    ".#b##b#rrrr#s...",
    ".##bbb#rrr#ss...",
    ".##bbb#r#ss....",
    "..s#####r#s.....",
    "....sss##ss.....",
    "........ss......",
    "................"};
  QPixmap px(Mesh_Feature_xpm);
  return px;
}

void ViewProviderMeshCurvature::setMode(const char* ModeName)
{
  if ( strcmp("Mean curvature",ModeName)==0 )
  {
    setVertexCurvatureMode(Mesh::PropertyCurvatureList::MeanCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( strcmp("Gaussian curvature",ModeName)==0  )
  {
    setVertexCurvatureMode(Mesh::PropertyCurvatureList::GaussCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( strcmp("Maximum curvature",ModeName)==0  )
  {
    setVertexCurvatureMode(Mesh::PropertyCurvatureList::MaxCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( strcmp("Minimum curvature",ModeName)==0  )
  {
    setVertexCurvatureMode(Mesh::PropertyCurvatureList::MinCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( strcmp("Absolute curvature",ModeName)==0  )
  {
    setVertexCurvatureMode(Mesh::PropertyCurvatureList::AbsCurvature);
    setDisplayMode("ColorShaded");
  }

  ViewProviderMesh::setMode(ModeName);
}

std::vector<std::string> ViewProviderMeshCurvature::getModes(void)
{
  std::vector<std::string> StrList = ViewProviderMesh::getModes();

  // add modes
  StrList.push_back("Absolute curvature");
  StrList.push_back("Mean curvature");
  StrList.push_back("Gaussian curvature");
  StrList.push_back("Maximum curvature");
  StrList.push_back("Minimum curvature");

  return StrList;
}

void ViewProviderMeshCurvature::OnChange(Base::Subject<int> &rCaller,int rcReason)
{
  setMode(this->getModeName().c_str());
}
