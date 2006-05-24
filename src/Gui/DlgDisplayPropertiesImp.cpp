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
# include <qlineedit.h>
# include <qspinbox.h>
# include <qstringlist.h>
#endif

#include "DlgDisplayPropertiesImp.h"
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
#include "ViewProviderDocumentObject.h"

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
DlgDisplayPropertiesImp::DlgDisplayPropertiesImp(  Gui::Command* pcCmd, QWidget* parent,  const char* name, bool modal, WFlags fl )
  : DlgDisplayProperties( parent, name, modal, fl ),_pcCmd(pcCmd),
    Sel(Gui::Selection().getObjectsOfType(App::DocumentObject::getClassTypeId()))
{

  ViewProvider *pcProv;
  set<string> ModeList;

  bool bSameMode= true;
  string sModeName;

  bool bSameTransp= true;
  float fTransp=0.0f;

  bool bSamePointSize= true;
  float fPointSize=0.0f;

  bool bSameLineWidth= true;
  float fLineWidth=0.0f;

  bool bSameColor= true;
  App::Color cColor;

  bool bViewerLimits=false;

  for(vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
  {
    pcProv = pcCmd->getActiveGuiDocument()->getViewProvider(*It);

    if ( pcProv )
    {
      vector<std::string> Modes = pcProv->getModes();

      for(vector<string>::iterator It2= Modes.begin();It2!=Modes.end();It2++)
        ModeList.insert(*It2);

      if(It==Sel.begin())
        sModeName = pcProv->getModeName();
      else
        if(sModeName != pcProv->getModeName())
          bSameMode = false;

      if(It==Sel.begin())
        fTransp = (*It)->getTransparency();
      else
        if(fTransp != (*It)->getTransparency())
          bSameMode = bSameTransp;

      if(It==Sel.begin())
        fPointSize = (*It)->getPointSize();
      else
        if(fPointSize != (*It)->getPointSize())
          bSameMode = bSamePointSize;

      if(It==Sel.begin())
        fLineWidth = (*It)->getLineSize();
      else
        if(fLineWidth != (*It)->getLineSize())
          bSameMode = bSameLineWidth;

      if(It==Sel.begin())
        cColor = (*It)->getColor();
      else
        if(cColor != (*It)->getColor())
          bSameColor = false;

      if ( !bViewerLimits ) {
        View3DInventor* activeView = dynamic_cast<View3DInventor*>(pcCmd->getActiveGuiDocument()->getActiveView());
        if (activeView)
        {
          View3DInventorViewer* viewer = activeView->getViewer();
          viewer->getPointSizeLimits(pointSizeRange,fPointSizeGranularity);
          viewer->getLineWidthLimits(lineWidthRange,fLineWidthGranularity);
          bViewerLimits = true;
        }
      }


      Provider.push_back(pcProv);
    }
  }


  if ( ModeList.size() > 0 ) {
    for(set<string>::iterator It3= ModeList.begin();It3!=ModeList.end();It3++)
      ModeBox->insertItem(It3->c_str()); 

    if(bSameMode){
      ModeBox->setCurrentText(sModeName.c_str());
    }else{
      ModeBox->insertItem("");
      ModeBox->setCurrentText("");
    }
  }
  else {
    // if the viewproviders of the selected features have no display mode
    ModeBox->setDisabled(true);
  }

  if(bSameTransp){
    TransBar->setValue((int) (fTransp * 100.0));
    TransSpin->setValue((int) (fTransp * 100.0));
  }

  if(bSamePointSize){
    pointSizeSpin->setMaxValue(pointSizeRange[1]);
    pointSizeSpin->setMinValue(pointSizeRange[0]);
    pointSizeSpin->setLineStep(fPointSizeGranularity);
    pointSizeSpin->setValue(fPointSize);
  }

  if(bSameLineWidth){
    lineWidthSpin->setMaxValue(lineWidthRange[1]);
    lineWidthSpin->setMinValue(lineWidthRange[0]);
    lineWidthSpin->setLineStep(fLineWidthGranularity);
    lineWidthSpin->setValue(fLineWidth);
  }

  if(bSameColor){
    ColorButton->setColor(QColor((int)(255.0f*cColor.r),(int)(255.0f*cColor.g),(int)(255.0f*cColor.b)));
  }else{
    ColorButton->setColor(QColor(127,127,127));
  }

  bModeChange = false;
  bTranspChange = false;
  bPointSizeChange = false;
  bLineWidthChange = false;
  bColorChange = false;

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

  
  QStringList material = Materials.keys();
  material.sort();
  MaterialCombo->insertStringList(material);
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgDisplayPropertiesImp::~DlgDisplayPropertiesImp()
{
  // no need to delete child widgets, Qt does it all for us
}

void DlgDisplayPropertiesImp::onChangeMaterial(const QString& material)
{
  App::Material::MaterialType type = Materials[material];
  App::Material mat(type);
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
  {
    if ( (*It)->getTypeId().isDerivedFrom( ViewProviderDocumentObject::getClassTypeId() ) ) {
      ViewProviderDocumentObject* vp = (ViewProviderDocumentObject*)(*It);
      vp->getObject()->setSolidMaterial(mat);
      vp->setMatFromObject();
    }
  }

  _pcCmd->getActiveGuiDocument()->onUpdate();
}

void DlgDisplayPropertiesImp::onChangeMode(const QString&s)
{
  Gui::WaitCursor wc;
  Base::Console().Log("Mode = %s\n",s.latin1());
//  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
//    (*It)->setMode(s.latin1()); 
  for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
  {
    ViewProvider* pcProv = _pcCmd->getActiveGuiDocument()->getViewProvider(*It);
    (*It)->setShowMode( s.latin1() );
    pcProv->setMode( (*It)->getShowMode() );
  }

  bModeChange = true;
  sModeChangeName = s.latin1();

  _pcCmd->getActiveGuiDocument()->onUpdate();
}

void DlgDisplayPropertiesImp::onChangePlot(const QString&s)
{
  Base::Console().Log("Plot = %s\n",s.latin1());
}

void DlgDisplayPropertiesImp::accept()
{

  if(bModeChange)
  {
    for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
    {
      if(_pcCmd->getActiveGuiDocument()->getViewProvider(*It))
      {
        _pcCmd->doCommand(Command::Doc,"App.document().%s.showMode = \"%s\"",(*It)->name.getValue(),sModeChangeName.c_str());
      }
    }
  }

  if(bTranspChange)
  {
    for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
    {
      if(_pcCmd->getActiveGuiDocument()->getViewProvider(*It))
      {
        _pcCmd->doCommand(Command::Doc,"App.document().%s.transparency = %f",(*It)->name.getValue(),fTranspChange);
      }
    }
  }

  if(bPointSizeChange)
  {
    for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
    {
      if(_pcCmd->getActiveGuiDocument()->getViewProvider(*It))
      {
        _pcCmd->doCommand(Command::Doc,"App.document().%s.pointSize = %f",(*It)->name.getValue(),fPointSizeChange);
      }
    }
  }

  if(bLineWidthChange)
  {
    for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
    {
      if(_pcCmd->getActiveGuiDocument()->getViewProvider(*It))
      {
        _pcCmd->doCommand(Command::Doc,"App.document().%s.lineSize = %f",(*It)->name.getValue(),fLineWidthChange);
      }
    }
  }

  if(bColorChange)
  {
    for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
    {
      if(_pcCmd->getActiveGuiDocument()->getViewProvider(*It))
      {
        _pcCmd->doCommand(Command::Doc,"App.document().%s.color = (%f,%f,%f)",(*It)->name.getValue(),cColorChange.r,cColorChange.g,cColorChange.b);
      }
    }
  }

  DlgDisplayProperties::accept();
}

void DlgDisplayPropertiesImp::onColorChange()
{
  QColor s = ColorButton->color();
  bColorChange = true;
  cColorChange = App::Color(s.red()/255.0,s.green()/255.0,s.blue()/255.0);

  for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
  {
    ViewProvider* pcProv = _pcCmd->getActiveGuiDocument()->getViewProvider(*It);
    pcProv->setColor(cColorChange);
  }


  _pcCmd->getActiveGuiDocument()->onUpdate();

}

void DlgDisplayPropertiesImp::reject()
{
  for(std::vector<App::DocumentObject*>::const_iterator It=Sel.begin();It!=Sel.end();It++)
  {
    ViewProvider* pcProv = _pcCmd->getActiveGuiDocument()->getViewProvider(*It);
    if (pcProv)
    {
      pcProv->setColor((*It)->getColor());
      (*It)->TouchView();
    }
  }

  DlgDisplayProperties::reject();
}

void DlgDisplayPropertiesImp::onChangeTrans(int i)
{
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
    (*It)->setTransparency( i/100.0); 

  bTranspChange = true;
  fTranspChange = i/100.0;

  _pcCmd->getActiveGuiDocument()->onUpdate();
}

void DlgDisplayPropertiesImp::onChangePointSize(double pointsize)
{
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
    (*It)->setPointSize( (float)pointsize ); 

  bPointSizeChange = true;
  fPointSizeChange = (float)pointsize;

  _pcCmd->getActiveGuiDocument()->onUpdate();
}

void DlgDisplayPropertiesImp::onChangeLineWidth(double linewidth)
{
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
    (*It)->setLineWidth( linewidth ); 

  bLineWidthChange = true;
  fLineWidthChange = (float)linewidth;

  _pcCmd->getActiveGuiDocument()->onUpdate();
}


#include "DlgDisplayProperties.cpp"
#include "moc_DlgDisplayProperties.cpp"
#include "moc_DlgDisplayPropertiesImp.cpp"

