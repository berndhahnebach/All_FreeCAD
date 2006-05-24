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


#ifndef GUI_VIEWPROVIDER_DOCUMENTOBJECT_H
#define GUI_VIEWPROVIDER_DOCUMENTOBJECT_H

#include <Base/Factory.h>
#include <Base/TimeInfo.h>
#include "ViewProvider.h"

#include <App/DocumentObject.h>
#include <App/Feature.h>

class SoMaterial;
class SoDrawStyle;

namespace App
{
  class DocumentObject;
  class Material;
}


namespace Gui {

class ObjectItem;

class GuiExport ViewProviderDocumentObject:public ViewProvider
{
  PROPERTY_HEADER(Gui::ViewProviderDocumentObject);

public:
  /// constructor.
  ViewProviderDocumentObject();

  /// destructor.
  virtual ~ViewProviderDocumentObject();

  // Returns the tree label
  virtual QListViewItem* getTreeItem(QListViewItem* parent);

  virtual void attach(App::DocumentObject *pcObject);
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void);

  /// check if the Data has to be recalculated
  bool ifDataNewer(void);
  /// check if the matrial has to be recalculated
  bool ifMaterialNewer(void); 

  virtual void update(void);

  virtual void updateData(void){};

  inline App::AbstractFeature *getAsFeature(void){
    assert(pcObject && pcObject->getTypeId().isDerivedFrom(App::AbstractFeature::getClassTypeId()) );
    return dynamic_cast<App::AbstractFeature *>(pcObject);
  }
  

  /// helper to copy material from a feature to a SoMeterial Node
  void copy(const App::Material&, SoMaterial*);
  /// helper to set the materials
  void setMatFromObject(void);

  /// Set the transparency
  virtual void setTransparency(float);
  /// Set the color
  virtual void setColor(const App::Color &c);
  /// Set the point size
  virtual void setPointSize(float);
  /// Set the line width
  virtual void setLineWidth(float);

    /// freqently called by the framework to check status changes in the data
  virtual bool testStatus(void);
  virtual void hide(void);
  virtual void show(void);
  virtual bool isShow(void);



  App::DocumentObject *getObject(void){return pcObject;}

protected:
  SoMaterial  *pcSolidMaterial;
  SoMaterial  *pcLineMaterial;
  SoMaterial  *pcPointMaterial;
  SoDrawStyle *pcLineStyle;
  SoDrawStyle *pcPointStyle;

  App::DocumentObject *pcObject;

  ObjectItem *pcObjItem;

  int _cLastStatus;
  Base::TimeInfo calcMaterial,calcData;
};


} // namespace Gui

#endif // GUI_VIEWPROVIDER_DOCUMENTOBJECT_H

