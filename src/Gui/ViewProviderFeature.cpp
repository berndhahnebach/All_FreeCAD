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
# include <Inventor/nodes/SoMaterial.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <App/Material.h>
#include <App/Feature.h>
#include "Selection.h"
#include "ViewProviderFeature.h"
#include "Tree.h"




using namespace Gui;


      
ViewProviderInventorFeature::ViewProviderInventorFeature()
:pcFeature(0)
{
  pcShadedMaterial = new SoMaterial;
  pcLineMaterial   = new SoMaterial;
  pcPointMaterial  = new SoMaterial;
  fLineSize        = 0.0;
  fPointSize       = 0.0;

  calcMaterial = 0;
  calcData=0;

}


ViewProviderInventorFeature::~ViewProviderInventorFeature()
{

}


void ViewProviderInventorFeature::attache(App::Feature *pcFeat)
{
  // save feature pointer
  pcFeature = pcFeat;

  // copy the material properties of the feature
  setMatFromFeature();

  // set viewing mode
  setMode(pcFeature->getShowMode());

}

bool ViewProviderInventorFeature::ifDataNewer(void)
{
  // first do attache
  assert(pcFeature);
  return pcFeature->getTouchTime() >= calcData; 
}

bool ViewProviderInventorFeature::ifMaterialNewer(void)
{
  // first do attache
  assert(pcFeature);
  return pcFeature->getTouchViewTime() >= calcMaterial; 

}


void ViewProviderInventorFeature::update(const ChangeType&)
{
  if(ifDataNewer())
    updateData();

  if(ifMaterialNewer())
  {
    setMatFromFeature();
    setMode(pcFeature->getShowMode());
  }

}


std::vector<std::string> ViewProviderInventorFeature::getModes(void)
{
  // empty
  return std::vector<std::string>();
}

void ViewProviderInventorFeature::selected(View3DInventorViewer *, SoPath *)
{
   Base::Console().Log("Select viewprovider Feature  %p\n",this);
   Gui::Selection().addFeature(pcFeature);
}
void ViewProviderInventorFeature::unselected(View3DInventorViewer *, SoPath *)
{
   Base::Console().Log("Unselect viewprovider Feature  %p\n",this);
   Gui::Selection().removeFeature(pcFeature);
}


void ViewProviderInventorFeature::copy(const App::Material &Mat, SoMaterial* pcSoMat)
{
  pcSoMat->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
  pcSoMat->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
  pcSoMat->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
  pcSoMat->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
  pcSoMat->shininess.setValue(Mat.shininess);
  pcSoMat->transparency.setValue(Mat.transparency);

}

void ViewProviderInventorFeature::setMatFromFeature(void)
{
  copy(pcFeature->getSolidMaterial(),pcShadedMaterial);
  copy(pcFeature->getLineMaterial(),pcLineMaterial);
  copy(pcFeature->getPointMaterial(),pcPointMaterial);
  fLineSize        = pcFeature->getLineSize();
  fPointSize       = pcFeature->getPointSize();

  // touch the material time
  time(&calcMaterial);
}

void ViewProviderInventorFeature::setTransparency(float trans)
{
  pcShadedMaterial->transparency = trans;
}

//===========================================================================
// FeatureFactorySingleton - Factory for Features
//===========================================================================



ViewProviderInventorFeatureFactorySingleton* ViewProviderInventorFeatureFactorySingleton::_pcSingleton = NULL;

ViewProviderInventorFeatureFactorySingleton& ViewProviderInventorFeatureFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new ViewProviderInventorFeatureFactorySingleton;
  return *_pcSingleton;
}

void ViewProviderInventorFeatureFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

ViewProviderInventorFeature* ViewProviderInventorFeatureFactorySingleton::Produce (const char* sName) const
{
	ViewProviderInventorFeature* w = (ViewProviderInventorFeature*)Factory::Produce(sName);

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

