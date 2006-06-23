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
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoPointSet.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoNormal.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <Base/Vector3D.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>

#include <Mod/Points/App/PointsFeature.h>

#include "ViewProvider.h"


using namespace PointsGui;
using namespace Points;


PROPERTY_SOURCE(PointsGui::ViewProviderPoints, Gui::ViewProviderDocumentObject)

       
ViewProviderPoints::ViewProviderPoints()
{
  pcPointsCoord = new SoCoordinate3();
  pcPointsCoord->ref();
  pcPoints = new SoPointSet();
  pcPoints->ref();
  pcPointsNormal = new SoNormal();  
  pcPointsNormal->ref();
  pcColorMat = new SoMaterial;
  pcColorMat->ref();
}

ViewProviderPoints::~ViewProviderPoints()
{
  pcPointsCoord->unref();
  pcPoints->unref();
  pcPointsNormal->unref();
  pcColorMat->unref();
}

void ViewProviderPoints::createPoints(Points::Feature *pFeature)
{
  if ( !pFeature ) return;
  const PointKernel& cPts = pFeature->getPoints().getKernel();

  // disable the notification, otherwise whenever a point is inserted SoPointSet gets notified
  pcPointsCoord->enableNotify(false);
  pcPointsCoord->point.deleteValues(0);
  pcPointsCoord->point.setNum(cPts.size());

  // get all points
  int idx=0;
  for ( PointKernel::const_iterator it = cPts.begin(); it != cPts.end(); ++it, idx++ )
  {
    pcPointsCoord->point.set1Value(idx, it->x, it->y, it->z);
  }

  pcPoints->numPoints = cPts.size();
  pcPointsCoord->enableNotify(true);
  pcPointsCoord->touch();
}

void ViewProviderPoints::setVertexColorMode(App::PropertyColorList* pcProperty)
{
  const std::vector<App::Color>& val = pcProperty->getValues();
  unsigned long i=0;

  pcColorMat->enableNotify(false);
  pcColorMat->diffuseColor.deleteValues(0);
  pcColorMat->diffuseColor.setNum(val.size());
  
  for ( std::vector<App::Color>::const_iterator it = val.begin(); it != val.end(); ++it )
  {
    pcColorMat->diffuseColor.set1Value(i++, SbColor(it->r, it->g, it->b));
  }

  pcColorMat->enableNotify(true);
  pcColorMat->touch();
}

void ViewProviderPoints::setVertexGreyvalueMode(Points::PropertyGreyValueList* pcProperty)
{
  const std::vector<float>& val = pcProperty->getValues();
  unsigned long i=0;

  pcColorMat->enableNotify(false);
  pcColorMat->diffuseColor.deleteValues(0);
  pcColorMat->diffuseColor.setNum(val.size());

  for ( std::vector<float>::const_iterator it = val.begin(); it != val.end(); ++it )
  {
    pcColorMat->diffuseColor.set1Value(i++, SbColor(*it, *it, *it));
  }

  pcColorMat->enableNotify(true);
  pcColorMat->touch();
}

void ViewProviderPoints::setVertexNormalMode(Points::PropertyNormalList* pcProperty)
{
  const std::vector<Base::Vector3f>& val = pcProperty->getValues();
  unsigned long i=0;

  pcPointsNormal->enableNotify(false);
  pcPointsNormal->vector.deleteValues(0);
  pcPointsNormal->vector.setNum(val.size());

  for ( std::vector<Base::Vector3f>::const_iterator it = val.begin(); it != val.end(); ++it )
  {
    pcPointsNormal->vector.set1Value(i++, it->x, it->y, it->z);
  }

  pcPointsNormal->enableNotify(true);
  pcPointsNormal->touch();
}

void ViewProviderPoints::attach(App::DocumentObject* pcObj)
{
  SoGroup* pcPointRoot = new SoGroup();
  SoGroup* pcPointShadedRoot = new SoGroup();
  SoGroup* pcColorShadedRoot = new SoGroup();

  // Hilight for selection
  pcHighlight->objectName = pcObj->name.getValue();
  pcHighlight->documentName = pcObj->getDocument().getName();
  pcHighlight->subElementName = "Main";
  pcHighlight->addChild(pcPointsCoord);
  pcHighlight->addChild(pcPoints);

  // points part ---------------------------------------------
  pcPointRoot->addChild(pcPointStyle);
  pcPointRoot->addChild(pcPointMaterial);
  pcPointRoot->addChild(pcHighlight);

  // points shaded ---------------------------------------------
  pcPointShadedRoot->addChild(pcPointStyle);
  pcPointShadedRoot->addChild(pcPointMaterial);
  pcPointShadedRoot->addChild(pcPointsNormal);
  pcPointShadedRoot->addChild(pcHighlight);

  // color shaded  ------------------------------------------
  pcColorShadedRoot->addChild(pcPointStyle);
  SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
  pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
  pcColorShadedRoot->addChild(pcColorMat);
  pcColorShadedRoot->addChild(pcMatBinding);
  pcColorShadedRoot->addChild(pcHighlight);

  // putting all together with a switch
  addDisplayMode(pcPointRoot, "Point");
  addDisplayMode(pcColorShadedRoot, "Color");
  addDisplayMode(pcPointShadedRoot, "Shaded");

  // get and save the feature
  Points::Feature* ptFea = dynamic_cast<Points::Feature*>(pcObj);
  createPoints( ptFea );

  // call father (set material and feature pointer)
  ViewProviderDocumentObject::attach(pcObj);
}

void ViewProviderPoints::setMode(const char* ModeName)
{
  modeString = ModeName;
  if ( strcmp("Color",ModeName)==0 )
  {
    std::map<std::string,App::Property*> Map;
    pcObject->getPropertyMap(Map);
    for( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
    {
      Base::Type t = it->second->getTypeId();
      if ( t==App::PropertyColorList::getClassTypeId() )
      {
        setVertexColorMode((App::PropertyColorList*)it->second);
        setDisplayMode("Color");
        break;
      }
    }
  }
  else if ( strcmp("Intensity",ModeName)==0 )
  {
    std::map<std::string,App::Property*> Map;
    pcObject->getPropertyMap(Map);
    for( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
    {
      Base::Type t = it->second->getTypeId();
      if ( t==Points::PropertyGreyValueList::getClassTypeId() )
      {
        setVertexGreyvalueMode((Points::PropertyGreyValueList*)it->second);
        setDisplayMode("Color");
        break;
      }
    }
  }
  else if ( strcmp("Shaded",ModeName)==0 )
  {
    std::map<std::string,App::Property*> Map;
    pcObject->getPropertyMap(Map);
    for( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
    {
      Base::Type t = it->second->getTypeId();
      if ( t==Points::PropertyNormalList::getClassTypeId() )
      {
        Points::PropertyNormalList* normals = (Points::PropertyNormalList*)it->second;
        if ( pcPointsCoord->point.getNum() != normals->getSize() ) {
          Base::Console().Error("No normals defined");
          return;
        }
        setVertexNormalMode(normals);
        setDisplayMode("Shaded");
        break;
      }
    }
  }
  else if ( strcmp("Point",ModeName)==0 )
  {
    setDisplayMode("Point");
  }

  ViewProviderDocumentObject::setMode(ModeName);
}

std::vector<std::string> ViewProviderPoints::getModes(void)
{
  std::vector<std::string> StrList;
  StrList.push_back("Point");

  if ( pcObject )
  {
    std::map<std::string,App::Property*> Map;
    pcObject->getPropertyMap(Map);

    for( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
    {
      Base::Type t = it->second->getTypeId();
      if ( t==Points::PropertyNormalList::getClassTypeId() )
        StrList.push_back("Shaded");
      else if ( t==Points::PropertyGreyValueList::getClassTypeId() )
        StrList.push_back("Intensity");
      else if ( t==App::PropertyColorList::getClassTypeId() )
        StrList.push_back("Color");
    }
  }

  return StrList;
}

void ViewProviderPoints::updateData()
{
  createPoints(dynamic_cast<Points::Feature*>(pcObject));
  setMode(modeString.c_str());
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
