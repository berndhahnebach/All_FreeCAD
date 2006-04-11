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
#include "ViewProviderFeature.h"
#include "Tree.h"
#include "Tools.h"




using namespace Gui;


PROPERTY_SOURCE_ABSTRACT(Gui::ViewProviderDocumentObject, Gui::ViewProvider)


      
ViewProviderDocumentObject::ViewProviderDocumentObject()
  : pcObject(0), _cLastStatus(-1)
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


ViewProviderDocumentObject::~ViewProviderDocumentObject()
{
  pcSolidMaterial->unref();
  pcLineMaterial->unref();
  pcPointMaterial->unref();
  pcLineStyle->unref();
}


void ViewProviderDocumentObject::attach(App::DocumentObject *pcFeat)
{
  // save Object pointer
  pcObject = pcFeat;

  // copy the material properties of the Object
  setMatFromObject();

  // set viewing mode
  setMode(pcObject->getShowMode());
  if ( !pcObject->visibility.getValue() )
    ViewProvider::hide();


  calcMaterial = pcObject->getTouchViewTime();
  calcData = pcObject->getTouchTime();

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

void ViewProviderDocumentObject::hide(void)
{
  ViewProvider::hide();
  if ( pcObject )
    pcObject->visibility.setValue(false);
}

void ViewProviderDocumentObject::show(void)
{
  ViewProvider::show();
  if ( pcObject )
    pcObject->visibility.setValue(true);
}

bool ViewProviderDocumentObject::isShow(void)
{
  return (pcObject ? pcObject->visibility.getValue() : false);
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
    setMatFromObject();
    setMode(pcObject->getShowMode());

    calcMaterial.setToActual();
  }

}


std::vector<std::string> ViewProviderDocumentObject::getModes(void)
{
  // empty
  return std::vector<std::string>();
}


void ViewProviderDocumentObject::copy(const App::Material &Mat, SoMaterial* pcSoMat)
{
  pcSoMat->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
  pcSoMat->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
  pcSoMat->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
  pcSoMat->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
  pcSoMat->shininess.setValue(Mat.shininess);
  pcSoMat->transparency.setValue(Mat.transparency);

}

void ViewProviderDocumentObject::setMatFromObject(void)
{
  copy(pcObject->getSolidMaterial(),pcSolidMaterial);
  copy(pcObject->getLineMaterial(),pcLineMaterial);
  copy(pcObject->getPointMaterial(),pcPointMaterial);
  pcLineStyle->lineWidth  = pcObject->getLineSize();
  pcPointStyle->pointSize = pcObject->getPointSize();

  // touch the material time
  calcMaterial.setToActual();
}

void ViewProviderDocumentObject::setTransparency(float trans)
{
  pcSolidMaterial->transparency = trans;
}

void ViewProviderDocumentObject::setColor(const App::Color &c)
{
  pcSolidMaterial->diffuseColor.setValue(c.r,c.g,c.b);
}

//===========================================================================
// FeatureFactorySingleton - Factory for Features
//===========================================================================

/*

ViewProviderDocumentObjectFactorySingleton* ViewProviderDocumentObjectFactorySingleton::_pcSingleton = NULL;

ViewProviderDocumentObjectFactorySingleton& ViewProviderDocumentObjectFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new ViewProviderDocumentObjectFactorySingleton;
  return *_pcSingleton;
}

void ViewProviderDocumentObjectFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

ViewProviderDocumentObject* ViewProviderDocumentObjectFactorySingleton::Produce (const char* sName) const
{
	ViewProviderDocumentObject* w = (ViewProviderDocumentObject*)Factory::Produce(sName);

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

*/ 
