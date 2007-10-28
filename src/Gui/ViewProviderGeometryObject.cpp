/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <Inventor/nodes/SoCamera.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoDirectionalLight.h>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/actions/SoRayPickAction.h> 
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "ViewProviderGeometryObject.h"
#include "View3DInventorViewer.h"



using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderGeometryObject, Gui::ViewProviderDocumentObject)

const App::PropertyIntegerConstraint::Constraints intPercent = {0,100,1};
      
ViewProviderGeometryObject::ViewProviderGeometryObject()
{
  ADD_PROPERTY(ShapeColor,(0.8f,0.8f,0.8f));
  ADD_PROPERTY(DisplayMode,((long)0));
  ADD_PROPERTY(Transparency,(0));
  Transparency.setConstraints(&intPercent);
  ADD_PROPERTY(Visibility,(true));
  App::Material mat(App::Material::DEFAULT);
  ADD_PROPERTY(ShapeMaterial,(mat));

  pcShapeMaterial = new SoMaterial;
  pcShapeMaterial->ref();
  ShapeMaterial.touch();

  sPixmap = "Feature";
}

ViewProviderGeometryObject::~ViewProviderGeometryObject()
{
  // Make sure that the property class does not destruct our string list
  DisplayMode.setEnums(0);
  pcShapeMaterial->unref();
}

void ViewProviderGeometryObject::onChanged(const App::Property* prop)
{
  // Actually, the properties 'ShapeColor' and 'Transparency' are part of the property 'ShapeMaterial'.
  // Both redundant properties are kept due to more convenience for the user. But we must keep the values
  // consistent of all these properties.
  if ( prop == &ShapeColor ) {
    const App::Color& c = ShapeColor.getValue();
    pcShapeMaterial->diffuseColor.setValue(c.r,c.g,c.b);
    ShapeMaterial.enableNotify(false);
    ShapeMaterial.setDiffuseColor(c);
    ShapeMaterial.enableNotify(true);
  } else if ( prop == &Transparency ) {
    float trans = Transparency.getValue()/100.0f;
    pcShapeMaterial->transparency = trans;
    ShapeMaterial.enableNotify(false);
    ShapeMaterial.setTransparency(trans);
    ShapeMaterial.enableNotify(true);
  } else if ( prop == &ShapeMaterial ) {
    const App::Material& Mat = ShapeMaterial.getValue();
    Transparency.enableNotify(false);
    Transparency.setValue((long)(100*Mat.transparency));
    Transparency.enableNotify(true);
    ShapeColor.enableNotify(false);
    ShapeColor.setValue(Mat.diffuseColor);
    ShapeColor.enableNotify(true);
    pcShapeMaterial->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
    pcShapeMaterial->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
    pcShapeMaterial->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
    pcShapeMaterial->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
    pcShapeMaterial->shininess.setValue(Mat.shininess);
    pcShapeMaterial->transparency.setValue(Mat.transparency);
  } else if ( prop == &DisplayMode ) {
    setActiveMode();
  } else if ( prop == &Visibility ) {
    Visibility.getValue() ? show() : hide();
  }
}

void ViewProviderGeometryObject::attach(App::DocumentObject *pcObj)
{
  ViewProviderDocumentObject::attach(pcObj);
}

SoPickedPointList ViewProviderGeometryObject::getPickedPoints(const SbVec2s& pos, const View3DInventorViewer& viewer,bool pickAll) const
{
  SoSeparator* root = new SoSeparator;
  root->ref();
  root->addChild(viewer.getHeadlight());
  root->addChild(viewer.getCamera());
//  root->addChild(this->pcHighlight);

  SoRayPickAction rp(viewer.getViewportRegion());
  rp.setPickAll(pickAll);
  rp.setPoint(pos);
  rp.apply(root);
  root->unref();

  // returns a copy of the list
  return rp.getPickedPointList();
}

SoPickedPoint* ViewProviderGeometryObject::getPickedPoint(const SbVec2s& pos, const View3DInventorViewer& viewer) const
{
  SoSeparator* root = new SoSeparator;
  root->ref();
  root->addChild(viewer.getHeadlight());
  root->addChild(viewer.getCamera());
//  root->addChild(this->pcHighlight);

  SoRayPickAction rp(viewer.getViewportRegion());
  rp.setPoint(pos);
  rp.apply(root);
  root->unref();

  // returns a copy of the point
  SoPickedPoint* pick = rp.getPickedPoint();
  return (pick ? pick->copy() : 0);
}
