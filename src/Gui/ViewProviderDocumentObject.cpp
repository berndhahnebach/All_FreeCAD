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
# include <Inventor/nodes/SoSeparator.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <App/Material.h>
#include <App/Feature.h>
#include "Application.h"
#include "Document.h"
#include "Selection.h"
#include "ViewProviderDocumentObject.h"
#include "Tree.h"
#include "Tools.h"



#define new DEBUG_CLIENTBLOCK
using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderDocumentObject, Gui::ViewProvider)


      
ViewProviderDocumentObject::ViewProviderDocumentObject()
  : pcObject(0), pcObjItem(0), _cLastStatus(-1)
{
  ADD_PROPERTY(ShapeColor,(0.8f,0.8f,0.8f));
  ADD_PROPERTY(DisplayMode,((long)0));
  ADD_PROPERTY(Transparency,(0));
  ADD_PROPERTY(Visibility,(true));
  App::Material mat(App::Material::DEFAULT);
  ADD_PROPERTY(ShapeMaterial,(mat));

  pcShapeMaterial = new SoMaterial;
  pcShapeMaterial->ref();
  ShapeMaterial.touch();

  sPixmap = "Feature";

}


ViewProviderDocumentObject::~ViewProviderDocumentObject()
{
  pcShapeMaterial->unref();
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

void ViewProviderDocumentObject::attach(App::DocumentObject *pcObj)
{
  // save Object pointer
  pcObject = pcObj;

  // Retrieve the supported display modes of the view provider
  std::vector<std::string> modes = this->getDisplayModes();

  if( modes.empty() )
    modes.push_back("");

  // We must convert the array of strings into an array of const char*
  std::vector<const char*>* enums = new std::vector<const char*>;
  for ( std::vector<std::string>::iterator it = modes.begin(); it != modes.end(); ++it ) {
    // We must duplicate the string literals because the vector gets deleted later on all its content is lost
#if defined (_MSC_VER)
    const char* mode = _strdup( it->c_str() );
#else
    const char* mode = strdup( it->c_str() );
#endif
    enums->push_back( mode );
  }
  enums->push_back(0); // null termination

  DisplayMode.enableNotify(false);
  DisplayMode.setEnums(&((*enums)[0]));
  // set the active mode
  const char* defmode = this->getDefaultDisplayMode();
  if ( defmode )
    DisplayMode.setValue(defmode);
  DisplayMode.enableNotify(true);


  calcMaterial = pcObject->getTouchViewTime();
  calcData = pcObject->getTouchTime();
}

SoSeparator* ViewProviderDocumentObject::findFrontRootOfType( const SoType& type) const
{
  // first get the document this object is part of and get its GUI counterpart
  const App::Document& pAppDoc = pcObject->getDocument();
  Gui::Document* pGuiDoc = Gui::Application::Instance->getDocument(pAppDoc.getName());

  // search in all view providers for the node type
  std::vector<App::DocumentObject*> obj = pAppDoc.getObjects();
  for ( std::vector<App::DocumentObject*>::iterator it = obj.begin(); it != obj.end(); ++it )
  {
    const ViewProvider* vp = pGuiDoc->getViewProvider(*it);
    // Ignore 'this' view provider. It could also happen that vp is 0, e.g. when
    // several objects have been added to the App::Document before notifying the
    // Gui::Document
    if ( !vp || vp == this )
      continue;
    SoSeparator* front = vp->getFrontRoot();
    if ( front && front->getTypeId() == type ) 
      return front;
  }

  return 0;
}

void ViewProviderDocumentObject::setActiveMode()
{
  const char* mode = DisplayMode.getValueAsString();
  if ( mode )
    setDisplayMode(mode);
  if ( !Visibility.getValue() )
    ViewProvider::hide();
}

ObjectItem* ViewProviderDocumentObject::getTreeItem(QListViewItem* parent)
{
  if ( !pcObjItem )
    pcObjItem = createTreeItem(parent);
  return pcObjItem;
}

ObjectItem* ViewProviderDocumentObject::createTreeItem(QListViewItem* parent)
{
  ObjectItem* item = new ObjectItem(parent,this);
  item->setPixmap(0, this->getIcon());
  item->setText(0, QString(pcObject->name.getValue()));
  return item;
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

bool ViewProviderDocumentObject::ifDataNewer(void) const
{
  // first do attach
  assert(pcObject);
  return pcObject->getTouchTime() > calcData || pcObject->getTouchTime() == calcData; 
}

bool ViewProviderDocumentObject::ifMaterialNewer(void) const
{
  // first do attach
  assert(pcObject);
  return pcObject->getTouchViewTime() > calcMaterial || pcObject->getTouchViewTime() == calcMaterial; 

}


void ViewProviderDocumentObject::update(void)
{
  //FIXME: Leads to problems when using time stamps. It's basically the same problem as desribed in 
  //       AbstractFeature::mustExecute().
  updateData();
//  if(ifDataNewer())
//  {
//    updateData();
//    
//    calcData.setToActual();
//  }
//
//  if(ifMaterialNewer())
//  {
////    setMatFromObject();
//    calcMaterial.setToActual();
//  }
}

const char* ViewProviderDocumentObject::getDefaultDisplayMode() const
{
  // We use the first item then
  return 0;
}

std::vector<std::string> ViewProviderDocumentObject::getDisplayModes(void) const
{
  // empty
  return std::vector<std::string>();
}
