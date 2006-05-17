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
#endif

#include "DlgDisplayPropertiesImp.h"
#include "Command.h"
#include "Application.h"
#include "Widgets.h"
#include "Selection.h"
#include "Document.h"
#include "ViewProvider.h"
#include "WaitCursor.h"

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

  bool bSameColor= true;
  App::Color cColor;

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
        cColor = (*It)->getColor();
      else
        if(cColor != (*It)->getColor())
          bSameColor = false;


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

  if(bSameColor){
    ColorButton->setColor(QColor((int)(255.0f*cColor.r),(int)(255.0f*cColor.g),(int)(255.0f*cColor.b)));
  }else{
    ColorButton->setColor(QColor(127,127,127));
  }

  bModeChange = false;
  bTranspChange = false;
  bColorChange = false;

}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgDisplayPropertiesImp::~DlgDisplayPropertiesImp()
{
  // no need to delete child widgets, Qt does it all for us
}

void DlgDisplayPropertiesImp::onChangeMaterial(const QString&s)
{
  Base::Console().Log("Material = %s\n",s.latin1());

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

void DlgDisplayPropertiesImp::onOK(void)
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

  accept();
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

void DlgDisplayPropertiesImp::onCancel()
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

  reject();
}

void DlgDisplayPropertiesImp::onChangeTrans(int i)
{
  for( std::vector<ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++)
    (*It)->setTransparency( i/100.0); 

  bTranspChange = true;
  fTranspChange = i/100.0;

  _pcCmd->getActiveGuiDocument()->onUpdate();
}


#include "DlgDisplayProperties.cpp"
#include "moc_DlgDisplayProperties.cpp"
#include "moc_DlgDisplayPropertiesImp.cpp"

