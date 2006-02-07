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
# include <Inventor/nodes/SoSwitch.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>
#include <Gui/SoFCColorBar.h>

#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/Core/Iterator.h>

#include "ViewProvider.h"
#include "ViewProviderCurvature.h"

using namespace Mesh;
using namespace MeshGui;
using namespace std;
using MeshCore::MeshKernel;
using MeshCore::MeshFacetIterator;
using Base::Vector3D;


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
}

ViewProviderMeshCurvature::~ViewProviderMeshCurvature()
{
  pcColorMat->unref();
  pcColorBar->Detach(this);
  pcColorBar->unref();
}

void ViewProviderMeshCurvature::init(App::Feature *pcFeat)
{
  MeshWithProperty& rMesh = dynamic_cast<Feature*>(pcFeat)->getMesh();
  MeshPropertyCurvature *prop = dynamic_cast<MeshPropertyCurvature*>(rMesh.Get("VertexCurvature") );
  if( prop && prop->isValid() )
  {
    std::vector<float> aMinValues = prop->getCurvature( MeshPropertyCurvature::MinCurvature );
    std::vector<float> aMaxValues = prop->getCurvature( MeshPropertyCurvature::MaxCurvature );
    if ( aMinValues.empty() || aMaxValues.empty() ) 
      return; // no values inside

    float fMin = *std::min_element( aMinValues.begin(), aMinValues.end() );
    float fMax = *std::max_element( aMinValues.begin(), aMinValues.end() );

    // histogram over all values
    std::map<int, int> aHistogram;
    for ( std::vector<float>::iterator it = aMinValues.begin(); it != aMinValues.end(); ++it )
    {
      int grp = (int)(10.0f*( *it - fMin )/( fMax - fMin ));
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
    for ( std::vector<float>::iterator it2 = aMaxValues.begin(); it2 != aMaxValues.end(); ++it2 )
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
  else if ( prop )
  {
    Base::Console().Warning("Invalid property 'VertexCurvature' found.\n");
  }
  else
  {
    Base::Console().Warning("Property 'VertexCurvature' not found.\n");
  }
}

void ViewProviderMeshCurvature::attach(App::Feature *pcFeat)
{
  init( pcFeat ); // init color bar

  SoGroup* pcColorShadedRoot = new SoGroup();

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

void ViewProviderMeshCurvature::setVertexCurvatureMode(MeshPropertyCurvature* pcProp, int mode)
{
  if ( !pcProp->isValid() ) return; // no valid data
  std::vector<float> fCurvature = pcProp->getCurvature(MeshPropertyCurvature::Mode(mode));

  unsigned long i=0;
  for ( std::vector<float>::const_iterator it = fCurvature.begin(); it != fCurvature.end(); ++it )
  {
    App::Color col = pcColorBar->getColor( *it );
    pcColorMat->diffuseColor.set1Value(i++, SbColor(col.r, col.g, col.b));
  }
}

void ViewProviderMeshCurvature::setVertexAbsCurvatureMode(MeshPropertyCurvature* pcProp)
{
  if ( !pcProp->isValid() ) return; // no valid data
  std::vector<float> fMaxCurvature = pcProp->getCurvature(MeshPropertyCurvature::MaxCurvature);
  std::vector<float> fMinCurvature = pcProp->getCurvature(MeshPropertyCurvature::MinCurvature);
  
  unsigned long i=0;
  for ( std::vector<float>::const_iterator it = fMaxCurvature.begin(),jt = fMinCurvature.begin(); it != fMaxCurvature.end(); ++it,++jt )
  {
    if ( fabs(*it) > fabs(*jt) )
    {
      App::Color col = pcColorBar->getColor( *it );
      pcColorMat->diffuseColor.set1Value(i++, SbColor(col.r, col.g, col.b));
    }
    else
    {
      App::Color col = pcColorBar->getColor( *jt );
      pcColorMat->diffuseColor.set1Value(i++, SbColor(col.r, col.g, col.b));
    }
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
  MeshWithProperty &rcMesh = dynamic_cast<Feature*>(pcFeature)->getMesh();
  App::PropertyBag *pcProp = 0;
  pcProp = rcMesh.GetFirstOfType("VertexCurvature");

  if ( pcProp && strcmp("Mean curvature",ModeName)==0 )
  {
    setVertexCurvatureMode(dynamic_cast<MeshPropertyCurvature*>(pcProp), MeshPropertyCurvature::MeanCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( pcProp && strcmp("Gaussian curvature",ModeName)==0  )
  {
    setVertexCurvatureMode(dynamic_cast<MeshPropertyCurvature*>(pcProp), MeshPropertyCurvature::GaussCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( pcProp && strcmp("Maximum curvature",ModeName)==0  )
  {
    setVertexCurvatureMode(dynamic_cast<MeshPropertyCurvature*>(pcProp), MeshPropertyCurvature::MaxCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( pcProp && strcmp("Minimum curvature",ModeName)==0  )
  {
    setVertexCurvatureMode(dynamic_cast<MeshPropertyCurvature*>(pcProp), MeshPropertyCurvature::MinCurvature);
    setDisplayMode("ColorShaded");
  }
  else if ( pcProp && strcmp("Absolute curvature",ModeName)==0  )
  {
    setVertexAbsCurvatureMode(dynamic_cast<MeshPropertyCurvature*>(pcProp));
    setDisplayMode("ColorShaded");
  }

  ViewProviderMesh::setMode(ModeName);
}

std::vector<std::string> ViewProviderMeshCurvature::getModes(void)
{
  std::vector<std::string> StrList = ViewProviderMesh::getModes();

  // add modes
  MeshWithProperty &rcMesh = dynamic_cast<Feature*>(pcFeature)->getMesh();
  if ( rcMesh.GetFirstOfType("VertexCurvature") )
  {
    StrList.push_back("Absolute curvature");
    StrList.push_back("Mean curvature");
    StrList.push_back("Gaussian curvature");
    StrList.push_back("Maximum curvature");
    StrList.push_back("Minimum curvature");
  }

  return StrList;
}

void ViewProviderMeshCurvature::OnChange(Base::Subject<int> &rCaller,int rcReason)
{
  setMode(this->getModeName().c_str());
}
