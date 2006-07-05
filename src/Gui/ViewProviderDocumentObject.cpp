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
# include <qpixmap.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <App/Material.h>
#include <App/Feature.h>
#include "Selection.h"
#include "ViewProviderDocumentObject.h"
#include "Tree.h"
#include "Tools.h"




using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderDocumentObject, Gui::ViewProvider)


      
ViewProviderDocumentObject::ViewProviderDocumentObject()
  : pcObject(0), _cLastStatus(-1)
{
  ADD_PROPERTY(ShapeColor,(0.8f,0.8f,0.8f));
  ADD_PROPERTY(Display,("Flat"));
  ADD_PROPERTY(Transparency,(0));
  ADD_PROPERTY(Visibility,(true));
  App::Material mat(App::Material::DEFAULT);
  ADD_PROPERTY(ShapeMaterial,(mat));

  pcShapeMaterial = new SoMaterial;
  pcShapeMaterial->ref();
  ShapeMaterial.touch();
//  pcLineMaterial   = new SoMaterial;
//  pcLineMaterial->ref();
//  pcPointMaterial  = new SoMaterial;
//  pcPointMaterial->ref();

//  pcLineStyle = new SoDrawStyle();
//  pcLineStyle->ref();
//  pcLineStyle->style = SoDrawStyle::LINES;
//  pcLineStyle->lineWidth = 1;

//  pcPointStyle = new SoDrawStyle();
//  pcPointStyle->ref();
//  pcPointStyle->style = SoDrawStyle::POINTS;
//  pcPointStyle->pointSize = 1;

  sPixmap = "Feature";

}


ViewProviderDocumentObject::~ViewProviderDocumentObject()
{
  pcShapeMaterial->unref();
//  pcLineMaterial->unref();
//  pcPointMaterial->unref();
//  pcLineStyle->unref();
//  pcPointStyle->unref();
}

void ViewProviderDocumentObject::onChanged(const App::Property* prop)
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
    Transparency.setValue(100*Mat.transparency);
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
  } else if ( prop == &Display ) {
    setActiveMode();
  } else if ( prop == &Visibility ) {
    Visibility.getValue() ? show() : hide();
  }
}

void ViewProviderDocumentObject::attach(App::DocumentObject *pcObj)
{
  // save Object pointer
  pcObject = pcObj;

  // copy the material properties of the Object
//  setMatFromObject();

  // set viewing mode
  //setMode(pcObject->getShowMode());

  // Retrieve the supported display modes of the view provider
  std::list<std::string> modes = getModes();

  // In the 'Display' property the first item is duplicated and always specifies the active mode. 
  if ( modes.size() > 0 ) {
    // Has getModes() already defined the active mode?
    std::list<std::string>::iterator sp = modes.begin();
    std::list<std::string>::iterator it = std::find(++sp, modes.end(), modes.front());
    if ( it == modes.end() )
      modes.push_front(modes.front());
    Display.enableNotify(false);
    Display.setValue(modes);
    Display.enableNotify(true);
  } else {
    // default
    Display.enableNotify(false);
    Display.setValue(modes); // empty mode list
    Display.enableNotify(true);
  }


  calcMaterial = pcObject->getTouchViewTime();
  calcData = pcObject->getTouchTime();
}

void ViewProviderDocumentObject::setActiveMode()
{
  const std::vector<std::string>& modes = Display.getValues();
  if ( !modes.empty() )
    setMode(modes.front().c_str());
  if ( !Visibility.getValue() )
    ViewProvider::hide();
}

QListViewItem* ViewProviderDocumentObject::getTreeItem(QListViewItem* parent)
{
  pcObjItem = new ObjectItem(parent,this);
  pcObjItem->setPixmap(0,/*ViewProvider::*/getIcon());
  pcObjItem->setText(0,QString(pcObject->name.getValue()));
  return pcObjItem;
}

bool ViewProviderDocumentObject::testStatus(void)
{
  assert(pcObject);

  if(! pcObject->getTypeId().isDerivedFrom(App::AbstractFeature::getClassTypeId()) )
    return true;

  App::AbstractFeature *pcFeature = dynamic_cast<App::AbstractFeature *>(pcObject);

  // if status has changed then continue
  int newStatus = (((int)(pcFeature->getStatus())) << 2) | ((pcFeature->mustExecute()?1:0) << 1) | (isShow()?1:0);
  if ( _cLastStatus == newStatus )
    return false;
  _cLastStatus = newStatus;

  QPixmap *px=0;
  switch(pcFeature->getStatus())
  {
  case App::AbstractFeature::Valid:
    if(pcFeature->mustExecute())
    {
      const char *feature_warning_xpm[]={
        "7 7 3 1",
        ". c None",
        "a c #000000",
        "# c #c8c800",
        "...#...",
        "..#a...",
        "..#a#..",
        ".##a#..",
        ".##a##.",
        "######.",
        "###a###"};
      px = new QPixmap(feature_warning_xpm);
      pcObjItem->BaseColor =Qt::yellow.light();
      pcObjItem->TextColor =Qt::black;
    }else{
      pcObjItem->BaseColor =Qt::white;
      pcObjItem->TextColor =Qt::black;
    } break;
  case App::AbstractFeature::New:
    {
      pcObjItem->BaseColor =Qt::white;
      pcObjItem->TextColor =Qt::gray;
    } break;
  case App::AbstractFeature::Inactive:
    {
      pcObjItem->BaseColor =Qt::white;
      pcObjItem->TextColor =Qt::gray;
    } break;
  case App::AbstractFeature::Recompute:
    {
      const char *feature_recompute_xpm[]={
        "7 7 3 1",
        ". c None",
        "# c #ffffff",
        "a c #0000ff",
        "..#aa..",
        ".##aa#.",
        "###aa##",
        "###aa##",
        "#######",
        ".##aa#.",
        "..#aa.."};
      px = new QPixmap(feature_recompute_xpm);
      pcObjItem->BaseColor =Qt::white;
      pcObjItem->TextColor =Qt::black;
    } break;
  case App::AbstractFeature::Error:
    {
      const char *feature_error_xpm[]={
        "7 7 3 1",
        ". c None",
        "# c #ff0000",
        "a c #ffffff",
        "..#aa..",
        ".##aa#.",
        "###aa##",
        "###aa##",
        "#######",
        ".##aa#.",
        "..#aa.."};
      px = new QPixmap(feature_error_xpm);
      pcObjItem->BaseColor =Qt::white;
      pcObjItem->TextColor =Qt::red;
    } break;
  }

  if( !isShow() )
  {
    pcObjItem->BaseColor =Qt::white;
    pcObjItem->TextColor =Qt::gray;
    QPixmap hidden = Tools::disabled( getIcon() );
    if ( px ) {
      pcObjItem->setPixmap(0,Tools::merge(hidden,*px,false));
    } else {
      pcObjItem->setPixmap(0,hidden);
    }
  }
  else // visible
  {
    if ( px ) {
      pcObjItem->setPixmap(0,Tools::merge(getIcon(),*px,false));
    } else {
      pcObjItem->setPixmap(0,getIcon());
    }
  }

  if (px) delete px;

  return true;
}

bool ViewProviderDocumentObject::ifDataNewer(void)
{
  // first do attach
  assert(pcObject);
  return pcObject->getTouchTime() > calcData || pcObject->getTouchTime() == calcData; 
}

bool ViewProviderDocumentObject::ifMaterialNewer(void)
{
  // first do attach
  assert(pcObject);
  return pcObject->getTouchViewTime() > calcMaterial || pcObject->getTouchViewTime() == calcMaterial; 

}


void ViewProviderDocumentObject::update(void)
{
  if(ifDataNewer())
  {
    updateData();
    
    calcData.setToActual();
  }

  if(ifMaterialNewer())
  {
//    setMatFromObject();
    calcMaterial.setToActual();
  }

}


std::list<std::string> ViewProviderDocumentObject::getModes(void) const
{
  // empty
  return std::list<std::string>();
}


//void ViewProviderDocumentObject::copy(const App::Material &Mat, SoMaterial* pcSoMat)
//{
//  pcSoMat->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
//  pcSoMat->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
//  pcSoMat->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
//  pcSoMat->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
//  pcSoMat->shininess.setValue(Mat.shininess);
//  pcSoMat->transparency.setValue(Mat.transparency);
//
//}

//void ViewProviderDocumentObject::setMatFromObject(void)
//{
//  copy(pcObject->getSolidMaterial(),pcShapeMaterial);
//  copy(pcObject->getLineMaterial(),pcLineMaterial);
//  copy(pcObject->getPointMaterial(),pcPointMaterial);
//  pcLineStyle->lineWidth  = pcObject->getLineSize();
//  pcPointStyle->pointSize = pcObject->getPointSize();
//
//  // touch the material time
//  calcMaterial.setToActual();
//}

//void ViewProviderDocumentObject::setTransparency(float trans)
//{
//  pcShapeMaterial->transparency = trans;
//}

//void ViewProviderDocumentObject::setColor(const App::Color &c)
//{
//  pcShapeMaterial->diffuseColor.setValue(c.r,c.g,c.b);
//}

//void ViewProviderDocumentObject::setPointSize(float val)
//{
//  pcPointStyle->pointSize.setValue( val );
//}

//void ViewProviderDocumentObject::setLineWidth(float val)
//{
//  pcLineStyle->lineWidth.setValue( val );
//}
