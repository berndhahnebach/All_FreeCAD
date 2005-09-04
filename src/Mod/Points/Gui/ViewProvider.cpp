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

       
ViewProviderInventorPoints::ViewProviderInventorPoints()
{
//  pcSwitch = new SoSwitch;
}

ViewProviderInventorPoints::~ViewProviderInventorPoints()
{
}

void ViewProviderInventorPoints::createPoints(App::Feature *pcFeature)
{
  PointsFeature* PtFea = dynamic_cast<PointsFeature*>(pcFeature);
  if ( !PtFea ) return;

  // get all points
  const PointKernel& cPts = PtFea->getPoints().getKernel();
  int idx=0;
  for ( PointKernel::const_iterator it = cPts.begin(); it != cPts.end(); ++it, idx++ )
  {
    pcPointsCoord->point.set1Value(idx, it->x, it->y, it->z);
  }

  pcPoints->numPoints = cPts.size();
}

void ViewProviderInventorPoints::setVertexColorMode(Points::PointsPropertyColor* pcProp)
{
  const std::vector<Points::PointsPropertyColor::fColor>& color = pcProp->Color;
  unsigned long i=0;
  for ( std::vector<Points::PointsPropertyColor::fColor>::const_iterator it = color.begin(); it != color.end(); ++it )
  {
    pcColorMat->diffuseColor.set1Value(i++, SbColor(it->r, it->g, it->b));
  }
}

void ViewProviderInventorPoints::setVertexGreyvalueMode(Points::PointsPropertyGreyvalue* pcProp)
{
  std::vector<float> greyvalue = pcProp->aGreyvalue;
  for (unsigned long i = 0; i < greyvalue.size(); i++)
  {
    float& grey = greyvalue[i];
    pcColorMat->diffuseColor.set1Value(i, SbColor(grey, grey, grey));
  }
}

void ViewProviderInventorPoints::setVertexNormalMode(Points::PointsPropertyNormal* pcProp)
{
  if ( !pcProp->isValid() ) return; // no valid data
  std::vector<Vector3D> normal = pcProp->aVertexNormal;
  int i=0;
  for ( std::vector<Vector3D>::iterator it = normal.begin(); it != normal.end(); ++it, i++ )
  {
    pcPointsNormal->vector.set1Value(i, it->x, it->y, it->z);
  }
}

void ViewProviderInventorPoints::attache(App::Feature* pcFeat)
{
  // call father (set material and feature pointer)
  ViewProviderInventorFeature::attache(pcFeat);

  // get and save the feature
  PointsFeature* ptFea = dynamic_cast<PointsFeature*>(pcFeature);
  if ( !ptFea )
    throw "ViewProviderInventorPoints::attach(): wrong feature attached!";


  pcPointsCoord = new SoCoordinate3();
  pcPoints = new SoPointSet();
  pcPointsNormal = new SoNormal();  
  createPoints( ptFea );

  SoSeparator* pcPointRoot = new SoSeparator();
  SoSeparator* pcPointShadedRoot = new SoSeparator();
  SoSeparator* pcColorShadedRoot = new SoSeparator();

  // Hilight for selection
  pcHighlight = new Gui::SoFCSelection();
  pcHighlight->featureName = pcFeature->getName();
  pcHighlight->documentName = pcFeature->getDocument().getName();
  pcHighlight->subElementName = "Main";
  pcHighlight->addChild(pcPointsCoord);
  pcHighlight->addChild(pcPoints);

  // points part ---------------------------------------------
  SoDrawStyle *pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = fPointSize;
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
//  pcHighlight->addChild(pcPointsCoord);
//  pcHighlight->addChild(pcPoints);
  pcPointRoot->addChild(pcHighlight);

  // points shaded ---------------------------------------------
//  SoLightModel* pcLightModel = new SoLightModel();
//  pcLightModel->model = SoLightModel::PHONG;
  pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = 2*fPointSize;
  pcPointShadedRoot->addChild(pcPointStyle);
//  pcHighlight->addChild(pcLightModel);
  pcPointShadedRoot->addChild(pcPointMaterial);
//  pcPointShadedRoot->addChild(pcPointsCoord);
  pcPointShadedRoot->addChild(pcPointsNormal);
//  pcPointShadedRoot->addChild(pcPoints);
  pcPointShadedRoot->addChild(pcHighlight);

  // color shaded  ------------------------------------------
  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcColorMat = new SoMaterial;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
//  pcColorShadedRoot->addChild(pcPointsCoord);
//  pcColorShadedRoot->addChild(pcPoints);
  pcPointShadedRoot->addChild(pcHighlight);

  // putting all together with a switch
  pcModeSwitch->addChild(pcPointRoot);
  pcModeSwitch->addChild(pcColorShadedRoot);
  pcModeSwitch->addChild(pcPointShadedRoot);
  pcModeSwitch->whichChild = 0; 
//  pcRoot->addChild(pcSwitch);

  pcFeat->setShowMode("Point");
  setMode(pcFeat->getShowMode());
}

void ViewProviderInventorPoints::setMode(const char* ModeName)
{
  if(stricmp("Point",ModeName)==0)
    pcModeSwitch->whichChild = 0; 
  else 
  {
    Points::PointsWithProperty &rcPoints = dynamic_cast<PointsFeature*>(pcFeature)->getPoints();
    App::PropertyBag *pcProp = 0;
    pcProp = rcPoints.Get(ModeName);
    if ( pcProp && stricmp("VertexColor",pcProp->GetType())==0 )
    {
      setVertexColorMode(dynamic_cast<Points::PointsPropertyColor*>(pcProp));
      pcModeSwitch->whichChild = 1;
    }
    else if ( pcProp && stricmp("VertexGreyvalue",pcProp->GetType())==0 )
    {
      setVertexGreyvalueMode(dynamic_cast<Points::PointsPropertyGreyvalue*>(pcProp));
      pcModeSwitch->whichChild = 1;
    }
    else if ( pcProp && strcmp("VertexNormal",pcProp->GetType())==0 )
    {
      setVertexNormalMode(dynamic_cast<Points::PointsPropertyNormal*>(pcProp));
      pcModeSwitch->whichChild = 2;
    }
    else 
      Base::Console().Warning("Unknown mode '%s' in ViewProviderInventorPoints::setMode(), ignored\n", ModeName);
  }
}

std::vector<std::string> ViewProviderInventorPoints::getModes(void)
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

void ViewProviderInventorPoints::update(const ChangeType&)
{
  // set new view modes
  setMode(pcFeature->getShowMode());
  // copy the material properties of the feature
  setMatFromFeature();
}
