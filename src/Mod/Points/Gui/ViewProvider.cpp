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
# include <Inventor/nodes/SoPointSet.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoNormal.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>

#include <Mod/Points/App/PointsFeature.h>

#include "ViewProvider.h"



using namespace PointsGui;
using namespace Points;

       
ViewProviderPoints::ViewProviderPoints()
{
  pcPointsCoord = new SoCoordinate3();
  pcPointsCoord->ref();
  pcPoints = new SoPointSet();
  pcPoints->ref();
  pcPointsNormal = new SoNormal();  
  pcPointsNormal->ref();
  pcHighlight = new Gui::SoFCSelection();
  pcHighlight->ref();
  pcColorMat = new SoMaterial;
  pcColorMat->ref();
}

ViewProviderPoints::~ViewProviderPoints()
{
  pcPointsCoord->unref();
  pcPoints->unref();
  pcPointsNormal->unref();
  pcHighlight->unref();
  pcColorMat->unref();
}

void ViewProviderPoints::createPoints(PointsFeature *pFeature)
{
  if ( !pFeature ) return;

  pcPointsCoord->point.deleteValues(0);

  // get all points
  const PointKernel& cPts = pFeature->getPoints().getKernel();
  int idx=0;
  for ( PointKernel::const_iterator it = cPts.begin(); it != cPts.end(); ++it, idx++ )
  {
    pcPointsCoord->point.set1Value(idx, it->x, it->y, it->z);
  }

  pcPoints->numPoints = cPts.size();
}

void ViewProviderPoints::setVertexColorMode(Points::PointsPropertyColor* pcProp)
{
  const std::vector<Points::PointsPropertyColor::fColor>& color = pcProp->Color;
  unsigned long i=0;
  for ( std::vector<Points::PointsPropertyColor::fColor>::const_iterator it = color.begin(); it != color.end(); ++it )
  {
    pcColorMat->diffuseColor.set1Value(i++, SbColor(it->r, it->g, it->b));
  }
}

void ViewProviderPoints::setVertexGreyvalueMode(Points::PointsPropertyGreyvalue* pcProp)
{
  //std::vector<float> greyvalue& = pcProp->aGreyvalue;
  for (unsigned long i = 0; i < pcProp->aGreyvalue.size(); i++)
  {
    float& grey = pcProp->aGreyvalue[i];
    pcColorMat->diffuseColor.set1Value(i, SbColor(grey, grey, grey));
  }
}

void ViewProviderPoints::setVertexNormalMode(Points::PointsPropertyNormal* pcProp)
{
  if ( !pcProp->isValid() ) return; // no valid data
  //std::vector<Vector3D> normal& = pcProp->aVertexNormal;
  int i=0;
  for ( std::vector<Vector3D>::iterator it = pcProp->aVertexNormal.begin(); it != pcProp->aVertexNormal.end(); ++it, i++ )
  {
    pcPointsNormal->vector.set1Value(i, it->x, it->y, it->z);
  }
}

void ViewProviderPoints::attach(App::Feature* pcFeat)
{
  SoGroup* pcPointRoot = new SoGroup();
  SoGroup* pcPointShadedRoot = new SoGroup();
  SoGroup* pcColorShadedRoot = new SoGroup();

  // Hilight for selection
  pcHighlight->featureName = pcFeat->getName();
  pcHighlight->documentName = pcFeat->getDocument().getName();
  pcHighlight->subElementName = "Main";
  pcHighlight->addChild(pcPointsCoord);
  pcHighlight->addChild(pcPoints);

  // points part ---------------------------------------------
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
  pcPointRoot->addChild(pcHighlight);

  // points shaded ---------------------------------------------
  //SoLightModel* pcLightModel = new SoLightModel();
  //pcLightModel->model = SoLightModel::PHONG;
  SoDrawStyle* pcPointShadedStyle = new SoDrawStyle();
  pcPointShadedStyle->style = SoDrawStyle::POINTS;
  pcPointShadedStyle->pointSize = 2*pcPointStyle->pointSize.getValue();
  pcPointShadedRoot->addChild(pcPointShadedStyle);
//  pcHighlight->addChild(pcLightModel);
  pcPointShadedRoot->addChild(pcPointMaterial);
  pcPointShadedRoot->addChild(pcPointsNormal);
  pcPointShadedRoot->addChild(pcHighlight);

  // color shaded  ------------------------------------------
  SoDrawStyle *pcFlatStyle = new SoDrawStyle();
  pcFlatStyle->style = SoDrawStyle::FILLED;
  pcColorShadedRoot->addChild(pcFlatStyle);

  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
  pcColorShadedRoot->addChild(pcHighlight);

  // putting all together with a switch
  addDisplayMode(pcPointRoot, "Point");
  addDisplayMode(pcColorShadedRoot, "Color");
  addDisplayMode(pcPointShadedRoot, "Shaded");

  // call father (set material and feature pointer)
  ViewProviderFeature::attach(pcFeat);

  // get and save the feature
  PointsFeature* ptFea = dynamic_cast<PointsFeature*>(pcFeat);
  createPoints( ptFea );
}

void ViewProviderPoints::setMode(const char* ModeName)
{
  Points::PointsWithProperty &rcPoints = dynamic_cast<PointsFeature*>(pcFeature)->getPoints();
  App::PropertyBag *pcProp = 0;
  pcProp = rcPoints.Get(ModeName);

  if ( pcProp && stricmp("VertexColor",pcProp->GetType())==0 )
  {
    setVertexColorMode(dynamic_cast<Points::PointsPropertyColor*>(pcProp));
    setDisplayMode("Color");
  }
  else if ( pcProp && stricmp("VertexGreyvalue",pcProp->GetType())==0 )
  {
    setVertexGreyvalueMode(dynamic_cast<Points::PointsPropertyGreyvalue*>(pcProp));
    setDisplayMode("Color");
  }
  else if ( pcProp && strcmp("VertexNormal",pcProp->GetType())==0 )
  {
    setVertexNormalMode(dynamic_cast<Points::PointsPropertyNormal*>(pcProp));
    setDisplayMode("Shaded");
  }
  else if ( stricmp("Point",ModeName)==0 )
  {
    setDisplayMode("Point");
  }

  ViewProviderFeature::setMode(ModeName);
}

std::vector<std::string> ViewProviderPoints::getModes(void)
{
  std::vector<std::string> StrList;
  StrList.push_back("Point");

  Points::PointsWithProperty &rcPoints = dynamic_cast<PointsFeature*>(pcFeature)->getPoints();
  std::list<std::string>::iterator It;

  std::list<std::string> list = rcPoints.GetAllNamesOfType("VertexColor");
  for(It=list.begin();It!=list.end();It++)
    StrList.push_back(*It);

  list = rcPoints.GetAllNamesOfType("VertexNormal");
  for(It=list.begin();It!=list.end();It++)
    StrList.push_back(*It);

  list = rcPoints.GetAllNamesOfType("VertexGreyvalue");
  for(It=list.begin();It!=list.end();It++)
    StrList.push_back(*It);

  return StrList;
}

void ViewProviderPoints::updateData()
{
  createPoints(dynamic_cast<PointsFeature*>(pcFeature));
}

void ViewProviderPoints::setTransparency(float trans)
{
  pcPointMaterial->transparency = trans;
}

void ViewProviderPoints::setColor(const App::Color &c)
{
  pcPointMaterial->diffuseColor.setValue(c.r,c.g,c.b);
}

QPixmap ViewProviderPoints::getIcon() const
{
  const char * Points_Feature_xpm[] = {
    "16 16 4 1",
    ".	c none",
    "s	c #000000",
    "b	c #FFFF00",
    "r	c #FF0000",
    "ss.....ss.....bb",
    "ss..ss.ss.....bb",
    "....ss..........",
    "...........bb...",
    ".ss..ss....bb...",
    ".ss..ss.........",
    "........bb....bb",
    "ss......bb....bb",
    "ss..rr......bb..",
    "....rr......bb..",
    "........bb......",
    "..rr....bb..bb..",
    "..rr........bb..",
    ".....rr.........",
    "rr...rr..rr..rr.",
    "rr.......rr..rr."};
  QPixmap px(Points_Feature_xpm);
  return px;
}
