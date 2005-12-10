/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <OSD_Process.hxx>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <App/Material.h>
#include <App/Feature.h>
#include "Selection.h"
#include "ViewProviderFeature.h"
#include "Tree.h"




using namespace Gui;


      
ViewProviderFeature::ViewProviderFeature()
:pcFeature(0)
{
  pcSolidMaterial = new SoMaterial;
  pcSolidMaterial->ref();
  pcLineMaterial   = new SoMaterial;
  pcLineMaterial->ref();
  pcPointMaterial  = new SoMaterial;
  pcPointMaterial->ref();
  pcLineStyle = new SoDrawStyle();

  pcLineStyle->ref();
  pcLineStyle->style = SoDrawStyle::LINES;
  pcLineStyle->lineWidth = 1;

  pcPointStyle = new SoDrawStyle();
  pcPointStyle->style = SoDrawStyle::POINTS;
  pcPointStyle->pointSize = 1;

  sPixmap = "Feature";

}


ViewProviderFeature::~ViewProviderFeature()
{
  pcSolidMaterial->unref();
  pcLineMaterial->unref();
  pcPointMaterial->unref();
  pcLineStyle->unref();
}


void ViewProviderFeature::attach(App::Feature *pcFeat)
{
  // save feature pointer
  pcFeature = pcFeat;

  // copy the material properties of the feature
  setMatFromFeature();

  // set viewing mode
  setMode(pcFeature->getShowMode());


  calcMaterial = pcFeature->getTouchViewTime();
  calcData = pcFeature->getTouchTime();

}


QListViewItem* ViewProviderFeature::getTreeItem(QListViewItem* parent)
{
  pcFeatItem = new FeatItem(parent,pcFeature);
  pcFeatItem->setPixmap(0,ViewProvider::getIcon());
  return pcFeatItem;
}


bool ViewProviderFeature::ifDataNewer(void)
{
  // first do attach
  assert(pcFeature);
  return pcFeature->getTouchTime() > calcData || pcFeature->getTouchTime() == calcData; 
}

bool ViewProviderFeature::ifMaterialNewer(void)
{
  // first do attach
  assert(pcFeature);
  return pcFeature->getTouchViewTime() > calcMaterial || pcFeature->getTouchViewTime() == calcMaterial; 

}


void ViewProviderFeature::update(void)
{
  if(ifDataNewer())
  {
    updateData();
    OSD_Process pro;
    calcData = pro.SystemDate();
  }

  if(ifMaterialNewer())
  {
    setMatFromFeature();
    setMode(pcFeature->getShowMode());
    OSD_Process pro;
    calcMaterial = pro.SystemDate();
  }

}


std::vector<std::string> ViewProviderFeature::getModes(void)
{
  // empty
  return std::vector<std::string>();
}


void ViewProviderFeature::copy(const App::Material &Mat, SoMaterial* pcSoMat)
{
  pcSoMat->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
  pcSoMat->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
  pcSoMat->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
  pcSoMat->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
  pcSoMat->shininess.setValue(Mat.shininess);
  pcSoMat->transparency.setValue(Mat.transparency);

}

void ViewProviderFeature::setMatFromFeature(void)
{
  copy(pcFeature->getSolidMaterial(),pcSolidMaterial);
  copy(pcFeature->getLineMaterial(),pcLineMaterial);
  copy(pcFeature->getPointMaterial(),pcPointMaterial);
  pcLineStyle->lineWidth  = pcFeature->getLineSize();
  pcPointStyle->pointSize = pcFeature->getPointSize();

  // touch the material time
  OSD_Process pro;
  calcMaterial = pro.SystemDate ();
}

void ViewProviderFeature::setTransparency(float trans)
{
  pcSolidMaterial->transparency = trans;
}

void ViewProviderFeature::setColor(const App::Color &c)
{
  pcSolidMaterial->diffuseColor.setValue(c.r,c.g,c.b);
}

//===========================================================================
// FeatureFactorySingleton - Factory for Features
//===========================================================================



ViewProviderFeatureFactorySingleton* ViewProviderFeatureFactorySingleton::_pcSingleton = NULL;

ViewProviderFeatureFactorySingleton& ViewProviderFeatureFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new ViewProviderFeatureFactorySingleton;
  return *_pcSingleton;
}

void ViewProviderFeatureFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

ViewProviderFeature* ViewProviderFeatureFactorySingleton::Produce (const char* sName) const
{
	ViewProviderFeature* w = (ViewProviderFeature*)Factory::Produce(sName);

  // this Feature class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    Base::Console().Warning("\"%s\" ViewProvider is not registered\n", sName);
#else
    Base::Console().Log("Warn: %s ViewProvider is not registered\n", sName);
#endif
    return NULL;
  }

  return w;
}

