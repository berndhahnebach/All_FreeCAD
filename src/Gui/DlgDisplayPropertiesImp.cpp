/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qcombobox.h>
# include <qiconview.h>
# include <qfiledialog.h>
# include <qlabel.h>
# include <qlineedit.h>
# include <qspinbox.h>
# include <qstringlist.h>
# include <qpushbutton.h>
#endif

#include "DlgDisplayPropertiesImp.h"
#include "DlgMaterialPropertiesImp.h"
#include "View3DInventorViewer.h"
#include "View3DInventor.h"
#include "Command.h"
#include "Application.h"
#include "Widgets.h"
#include "Selection.h"
#include "Document.h"
#include "ViewProvider.h"
#include "WaitCursor.h"
#include "SpinBox.h"

#include <App/Application.h>
#include <App/Feature.h>
#include <App/Material.h>

using namespace Gui::Dialog;
using namespace std;


/* TRANSLATOR Gui::Dialog::DlgDisplayPropertiesImp */

/**
 *  Constructs a DlgDisplayPropertiesImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgDisplayPropertiesImp::DlgDisplayPropertiesImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
  : DlgDisplayProperties( parent, name, modal, fl )
{
  QStringList commonModeList;
  QString activeMode;
  int transparency = -1;
  QColor shapeColor;
  float pointsize = -1.0f;
  float linewidth = -1.0f;
  float fPointSizeGranularity=0.125f;
  SbVec2f pointSizeRange(1.0f,1.0f);
  float fLineWidthGranularity=0.125f;
  SbVec2f lineWidthRange(1.0f,1.0f);
  App::Material::MaterialType cMatType = App::Material::DEFAULT;

  bool viewerLimits=false;
  bool bMaterial = false;
  bool bDisplay = true;
  bool bTransparency = true;
  bool bShapeColor = true;
  bool bPointSize = true;
  bool bLineWidth = true;
  bool bShapeMaterial = true;

  std::vector<SelectionSingleton::SelObj> obj = Gui::Selection().getCompleteSelection();
  for ( std::vector<SelectionSingleton::SelObj>::iterator so = obj.begin(); so != obj.end(); ++so )
  {
    Gui::Document* doc = Application::Instance->getDocument(so->pDoc);
    if ( viewerLimits == false ) {
      View3DInventor* activeView = dynamic_cast<View3DInventor*>(doc->getActiveView());
      if (activeView)
      {
        View3DInventorViewer* viewer = activeView->getViewer();
        viewer->getPointSizeLimits(pointSizeRange,fPointSizeGranularity);
        viewer->getLineWidthLimits(lineWidthRange,fLineWidthGranularity);
        viewerLimits = true;
      }
    }

    ViewProvider *pcProv = doc->getViewProvider(so->pObject);
    if ( pcProv )
    {
      Provider.push_back(pcProv);

      // 'Display' property
      if ( bDisplay )
      {
        App::Property* prop = pcProv->getPropertyByName("Display");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyStringList::getClassTypeId()))
        {
          App::PropertyStringList* Display = (App::PropertyStringList*)prop;
          const std::vector<std::string>& modes = Display->getValues();

          if ( commonModeList.isEmpty() ) {
            for ( std::vector<std::string>::const_iterator it = modes.begin(); it != modes.end(); ++it )
              commonModeList << it->c_str();
            // currently active mode is the first item and can be popped
            if ( !commonModeList.isEmpty() ) {
              activeMode = commonModeList.front();
              commonModeList.pop_front();
              if ( commonModeList.isEmpty() )
                bDisplay = false;
            } else {
              bDisplay = false;
            }
          } else {
            QStringList modeList;
            for ( std::vector<std::string>::const_iterator it = modes.begin(); it != modes.end(); ++it ) {
              if ( commonModeList.find(it->c_str()) != commonModeList.end() )
                modeList << it->c_str();
            }

            // intersection of both lists
            if ( !modeList.isEmpty() ) {
              modeList.pop_front();
              if ( modeList.isEmpty() )
                bDisplay = false;
              commonModeList = modeList;
            } else {
              bDisplay = false;
            }
          }
        }
        else
          bDisplay = false;
      }

      // 'Transparency' property
      if ( bTransparency )
      {
        App::Property* prop = pcProv->getPropertyByName("Transparency");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyInteger::getClassTypeId()))
        {
          App::PropertyInteger* Transparency = (App::PropertyInteger*)prop;
          transparency = Transparency->getValue();
        }
        else
          bTransparency = false;
      }

      // 'ShapeColor' property
      if ( bShapeColor )
      {
        App::Property* prop = pcProv->getPropertyByName("ShapeColor");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()))
        {
          App::PropertyColor* ShapeColor = (App::PropertyColor*)prop;
          App::Color c = ShapeColor->getValue();
          shapeColor.setRgb((int)(c.r*255.0f), (int)(c.g*255.0f),(int)(c.b*255.0f));
        }
        else
          bShapeColor = false;
      }

      // 'PointSize' property
      if ( bPointSize )
      {
        App::Property* prop = pcProv->getPropertyByName("PointSize");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()))
        {
          App::PropertyFloat* PointSize = (App::PropertyFloat*)prop;
          pointsize = PointSize->getValue();
        }
        else
          bPointSize = false;
      }

      // 'LineWidth' property
      if ( bLineWidth )
      {
        App::Property* prop = pcProv->getPropertyByName("LineWidth");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()))
        {
          App::PropertyFloat* LineWidth = (App::PropertyFloat*)prop;
          linewidth = LineWidth->getValue();
        }
        else
          bLineWidth = false;
      }

      // 'ShapeMaterial' property
      if ( bShapeMaterial )
      {
        App::Property* prop = pcProv->getPropertyByName("ShapeMaterial");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyMaterial::getClassTypeId()))
        {
          App::PropertyMaterial* ShapeMaterial = (App::PropertyMaterial*)prop;
          App::Material mat;
          if ( bMaterial == false ) {
            cMatType = mat.getType();
            bMaterial = true;
          } else if ( cMatType != mat.getType() ) {
            cMatType = App::Material::USER_DEFINED;
          }
        }
      }
    }
  }

  // set dialog stuff
  //
  if ( !bDisplay ) {
    ModeBox->setDisabled(true);
  } else {
    ModeBox->insertStringList(commonModeList);
    ModeBox->setCurrentText(activeMode);
  }

  if ( !bTransparency ) {
    TransBar->setDisabled(true);
    TransSpin->setDisabled(true);
    textLabel1_2->setDisabled(true);
  } else {
    TransBar->setValue(transparency);
    TransSpin->setValue(transparency);
  }

  if ( bShapeColor ) {
    ColorButton->setColor(shapeColor);
  } else {
    ColorButton->setDisabled(true);
  }

  if ( !bPointSize ) {
    pointSizeSpin->setDisabled(true);
    textLabel2->setDisabled(true);
  } else {
    pointSizeSpin->blockSignals(true);
    pointSizeSpin->setMaxValue(pointSizeRange[1]);
    pointSizeSpin->setMinValue(pointSizeRange[0]);
    pointSizeSpin->setLineStep(fPointSizeGranularity);
    pointSizeSpin->setValue(pointsize);
    pointSizeSpin->blockSignals(false);
  }

  if ( !bLineWidth ) {
    lineWidthSpin->setDisabled(true);
    textLabel3->setDisabled(true);
  } else {
    lineWidthSpin->blockSignals(true);
    lineWidthSpin->setMaxValue(lineWidthRange[1]);
    lineWidthSpin->setMinValue(lineWidthRange[0]);
    lineWidthSpin->setLineStep(fLineWidthGranularity);
    lineWidthSpin->setValue(linewidth);
    lineWidthSpin->blockSignals(false);
  }

  if ( bShapeMaterial )
  {
    Materials["Brass"]         = App::Material::BRASS;
    Materials["Bronze"]        = App::Material::BRONZE;
    Materials["Copper"]        = App::Material::COPPER;
    Materials["Gold"]          = App::Material::GOLD;
    Materials["Pewter"]        = App::Material::PEWTER;
    Materials["Plaster"]       = App::Material::PLASTER;
    Materials["Plastic"]       = App::Material::PLASTIC;
    Materials["Silver"]        = App::Material::SILVER;
    Materials["Steel"]         = App::Material::STEEL;
    Materials["Stone"]         = App::Material::STONE;
    Materials["Shiny plastic"] = App::Material::SHINY_PLASTIC;
    Materials["Satin"]         = App::Material::SATIN;
    Materials["Metalized"]     = App::Material::METALIZED;
    Materials["Neon GNC"]      = App::Material::NEON_GNC;
    Materials["Chrome"]        = App::Material::CHROME;
    Materials["Aluminium"]     = App::Material::ALUMINIUM;
    Materials["Obsidian"]      = App::Material::OBSIDIAN;
    Materials["Neon PHC"]      = App::Material::NEON_PHC;
    Materials["Jade"]          = App::Material::JADE;
    Materials["Ruby"]          = App::Material::RUBY;
    Materials["Emerald"]       = App::Material::EMERALD;

    QStringList material = Materials.keys();
    material.sort();
    MaterialCombo->insertItem("Default");
    MaterialCombo->insertStringList(material);
    Materials["Default"]       = App::Material::DEFAULT;
    for (QMap<QString, App::Material::MaterialType>::ConstIterator it = Materials.begin(); it != Materials.end(); ++it)
    {
      if (it.data() == cMatType) {
        MaterialCombo->setCurrentText(it.key());
        break;
      }
    }
  }
  else
  {
    MaterialCombo->setDisabled(true);
    pushButton2->setDisabled(true);
  }
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgDisplayPropertiesImp::~DlgDisplayPropertiesImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Opens a dialog that allows to modify the 'ShapeMaterial' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::onUserDefinedMaterial()
{
  DlgMaterialPropertiesImp dlg(this);
  dlg.setViewProviders(Provider);
  dlg.exec();

  ColorButton->setColor(dlg.diffuseColor->color());
}

/**
 * Sets the 'ShapeMaterial' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::onChangeMaterial(const QString& material)
{
  App::Material::MaterialType type = Materials[material];
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
  {
    App::Property* prop = (*It)->getPropertyByName("ShapeMaterial");
    if (prop && prop->getTypeId().isDerivedFrom(App::PropertyMaterial::getClassTypeId()))
    {
      App::PropertyMaterial* ShapeMaterial = (App::PropertyMaterial*)prop;
      App::Material mat;
      mat.setType(type);
      ShapeMaterial->setValue(mat);
      App::Color diffuseColor = mat.diffuseColor;
      ColorButton->setColor(QColor( (int)(diffuseColor.r*255.0f), (int)(diffuseColor.g*255.0f), (int)(diffuseColor.b*255.0f) ));
    }
  }
}

/**
 * Sets the 'Display' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::onChangeMode(const QString& s)
{
  Gui::WaitCursor wc;
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
  {
    App::Property* prop = (*It)->getPropertyByName("Display");
    if (prop && prop->getTypeId().isDerivedFrom(App::PropertyStringList::getClassTypeId()))
    {
      App::PropertyStringList* Display = (App::PropertyStringList*)prop;
      Display->set1Value(0,s.latin1());
      Display->touch();
    }
  }
}

void DlgDisplayPropertiesImp::onChangePlot(const QString&s)
{
  Base::Console().Log("Plot = %s\n",s.latin1());
}

/**
 * Sets the 'ShapeColor' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::onColorChange()
{
  QColor s = ColorButton->color();
  App::Color c(s.red()/255.0,s.green()/255.0,s.blue()/255.0);
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
  {
    App::Property* prop = (*It)->getPropertyByName("ShapeColor");
    if (prop && prop->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()))
    {
      App::PropertyColor* ShapeColor = (App::PropertyColor*)prop;
      ShapeColor->setValue(c);
    }
  }
}

/**
 * Sets the 'Transparency' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::onChangeTrans(int transparency)
{
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
  {
    App::Property* prop = (*It)->getPropertyByName("Transparency");
    if (prop && prop->getTypeId().isDerivedFrom(App::PropertyInteger::getClassTypeId()))
    {
      App::PropertyInteger* Transparency = (App::PropertyInteger*)prop;
      Transparency->setValue(transparency);
    }
  }
}

/**
 * Sets the 'PointSize' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::onChangePointSize(double pointsize)
{
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
  {
    App::Property* prop = (*It)->getPropertyByName("PointSize");
    if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()))
    {
      App::PropertyFloat* PointSize = (App::PropertyFloat*)prop;
      PointSize->setValue((float)pointsize);
    }
  }
}

/**
 * Sets the 'LineWidth' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::onChangeLineWidth(double linewidth)
{
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
  {
    App::Property* prop = (*It)->getPropertyByName("LineWidth");
    if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()))
    {
      App::PropertyFloat* LineWidth = (App::PropertyFloat*)prop;
      LineWidth->setValue((float)linewidth);
    }
  }
}

void DlgDisplayPropertiesImp::accept()
{
  DlgDisplayProperties::accept();
}

void DlgDisplayPropertiesImp::reject()
{
  DlgDisplayProperties::reject();
}


#include "DlgDisplayProperties.cpp"
#include "moc_DlgDisplayProperties.cpp"
#include "moc_DlgDisplayPropertiesImp.cpp"

