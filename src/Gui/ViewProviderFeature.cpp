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
#include "ViewProviderFeature.h"
#include "Tree.h"




using namespace Gui;


      
ViewProviderInventorFeature::ViewProviderInventorFeature()
{
  pcShadedMaterial = new SoMaterial;
  pcLineMaterial   = new SoMaterial;
  pcPointMaterial  = new SoMaterial;
  fLineSize        = 0.0;
  fPointSize       = 0.0;

}


ViewProviderInventorFeature::~ViewProviderInventorFeature()
{

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
  copy(pcFeature->_solidMaterial,pcShadedMaterial);
  copy(pcFeature->_lineMaterial,pcLineMaterial);
  copy(pcFeature->_pointMaterial,pcPointMaterial);
  fLineSize        = pcFeature->_lineSize;
  fPointSize       = pcFeature->_pointSize;

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

