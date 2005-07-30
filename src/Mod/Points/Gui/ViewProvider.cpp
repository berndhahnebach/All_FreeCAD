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
#include <Gui/Selection.h>

#include <Mod/Points/App/PointsFeature.h>

#include "ViewProvider.h"



using namespace PointsGui;
using namespace Points;

       
ViewProviderInventorPoints::ViewProviderInventorPoints()
{
  pcSwitch = new SoSwitch;
}

ViewProviderInventorPoints::~ViewProviderInventorPoints()
{
}

void ViewProviderInventorPoints::selected(Gui::View3DInventorViewer *, SoPath *)
{
  Base::Console().Log("Select viewprovider Points  %p\n",this);
  Gui::Selection().addFeature(pcFeature);

  pcHighlight->mode = SoLocateHighlight::ON;
  pcHighlight->color.setValue((float)0.0,(float)0.3,(float)0.0);
}

void ViewProviderInventorPoints::unselected(Gui::View3DInventorViewer *, SoPath *)
{
  Base::Console().Log("Unselect viewprovider Points  %p\n",this);
  Gui::Selection().removeFeature(pcFeature);

  pcHighlight->mode = SoLocateHighlight::AUTO;
  pcHighlight->color.setValue((float)0.1,(float)0.3,(float)0.7);
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
  std::vector<Points::PointsPropertyColor::fColor> color = pcProp->Color;
  for (unsigned long i=0; i<color.size();i++)
  {
    Points::PointsPropertyColor::fColor& col = color[i];
    pcColorMat->diffuseColor.set1Value(i, SbColor(col.r, col.g, col.b));
  }
}

void ViewProviderInventorPoints::attache(App::Feature* pcFeat)
{
  pcFeature = pcFeat;
  // get and save the feature
  PointsFeature* ptFea = dynamic_cast<PointsFeature*>(pcFeature);
  if ( !ptFea )
    throw "ViewProviderInventorPoints::attach(): wrong feature attached!";

  // copy the material properties of the feature
  setMatFromFeature();

  pcPointsCoord = new SoCoordinate3();
  pcPoints = new SoPointSet();
  createPoints( ptFea );

  SoSeparator* pcPointRoot = new SoSeparator();
  SoSeparator* pcColorShadedRoot = new SoSeparator();

  // Hilight for selection
  pcHighlight = new SoLocateHighlight();
  pcHighlight->color.setValue((float)0.1,(float)0.3,(float)0.7);
  pcHighlight->addChild(pcPointsCoord);
  pcHighlight->addChild(pcPoints);

  // points part ---------------------------------------------
  SoDrawStyle *pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = 1.0;
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
  pcHighlight->addChild(pcPointsCoord);
  pcHighlight->addChild(pcPoints);
  pcPointRoot->addChild(pcHighlight);

  // color shaded  ------------------------------------------
  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcColorMat = new SoMaterial;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
  pcColorShadedRoot->addChild(pcPointsCoord);
  pcColorShadedRoot->addChild(pcPoints);

  // putting all together with a switch
  pcSwitch->addChild(pcPointRoot);
  pcSwitch->addChild(pcColorShadedRoot);
  pcSwitch->whichChild = 0; 
  pcRoot->addChild(pcSwitch);

  pcFeat->setShowMode("Point");
  setMode(pcFeat->getShowMode());
}

void ViewProviderInventorPoints::setMode(const char* ModeName)
{
  if(stricmp("Point",ModeName)==0)
    pcSwitch->whichChild = 0; 
  else 
  {
    Points::PointsWithProperty &rcPoints = dynamic_cast<PointsFeature*>(pcFeature)->getPoints();
    App::PropertyBag *pcProp = 0;
    pcProp = rcPoints.Get(ModeName);
    if ( pcProp && stricmp("VertexColor",pcProp->GetType())==0 )
    {
      setVertexColorMode(dynamic_cast<Points::PointsPropertyColor*>(pcProp));
      pcSwitch->whichChild = 1;
    }else 
      Base::Console().Warning("Unknown mode '%s' in ViewProviderInventorPoints::setMode(), ignored\n", ModeName);
  }
}

std::vector<std::string> ViewProviderInventorPoints::getModes(void)
{
  std::vector<std::string> StrList;
  StrList.push_back("Point");

  Points::PointsWithProperty &rcPoints = dynamic_cast<PointsFeature*>(pcFeature)->getPoints();
  std::list<std::string> list = rcPoints.GetAllNamesOfType("VertexColor");

  for(std::list<std::string>::iterator It=list.begin();It!=list.end();It++)
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
