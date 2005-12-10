/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


/*  Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
# include <assert.h>
# include <string>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Selection.h"
#include <Base/Exception.h>
#include <Base/Console.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/Feature.h>


using namespace Gui;
using namespace std;


vector<App::Feature*> SelectionSingelton::getSelectedFeatures(const char *TypeName, const char* pDocName) const
{
  vector<App::Feature*> temp;
  App::Document *pcDoc;
  string DocName;
  string typeName = TypeName ? TypeName : "";

  if(pDocName)
    pcDoc =  App::GetApplication().getDocument(pDocName);
  else
    pcDoc = App::GetApplication().getActiveDocument();

  if(!pcDoc)
    return temp;

  for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    if (It->pDoc == pcDoc && typeName.size() <= It->TypeName.size())
    {
      if ( It->pFeat && (typeName.empty() || It->TypeName.substr(0, typeName.size()).compare(typeName) == 0) )
        temp.push_back(It->pFeat);
    }
  }

  return temp;
}

vector<SelectionSingelton::SelObj> SelectionSingelton::getSelection(const char* pDocName)
{
  vector<SelObj> temp;
  SelObj tempSelObj;

  App::Document *pcDoc;
  string DocName;

  if(pDocName)
    pcDoc =  App::GetApplication().getDocument(pDocName);
  else
    pcDoc = App::GetApplication().getActiveDocument();

  if(!pcDoc)
    return temp;

  for( list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    if(It->pDoc == pcDoc)
    {
      tempSelObj.DocName  = It->DocName.c_str();
      tempSelObj.FeatName = It->FeatName.c_str();
      tempSelObj.SubName  = It->SubName.c_str();
      tempSelObj.TypeName = It->TypeName.c_str();
      tempSelObj.pFeat    = It->pFeat;
      tempSelObj.pDoc     = It->pDoc;
        
      temp.push_back(tempSelObj);
    }
  }

  return temp;
}

unsigned int SelectionSingelton::getNbrOfType(const char *TypeName, const char* pDocName)
{
  unsigned int iNbr=0;
  App::Document *pcDoc;
  string DocName;

  if(pDocName)
    pcDoc =  App::GetApplication().getDocument(pDocName);
  else
    pcDoc = App::GetApplication().getActiveDocument();

  if(!pcDoc)
    return 0;

  string typeName = TypeName ? TypeName : "";
  for( list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    // find_first_of() kann unmöglich richtig sein, da nur die jeweils ersten Buchstaben gleich sein müssen  (Werner)
//    if(It->pDoc == pcDoc && It->TypeName.find_first_of(TypeName) == 0)
    // 'it->TypeName' starts with 'typeName'
    if ( It->pDoc == pcDoc && typeName.size() <= It->TypeName.size() )
    {
      if ( It->TypeName.substr(0, typeName.size()).compare(typeName) == 0 )
      {
        iNbr++;
      }
    }
  }

  return iNbr;
}

bool SelectionSingelton::setPreselect(const char* pDocName, const char* pFeatName, const char* pSubName, float x, float y, float z)
{
  if(DocName != "")
    rmvPreselect();

  DocName = pDocName;
  FeatName= pFeatName;
  SubName = pSubName;
  hx = x;
  hy = y;
  hz = z;

  SelectionChanges Chng;

  Chng.pDocName  = DocName.c_str();
  Chng.pFeatName = FeatName.c_str();
  Chng.pSubName  = SubName.c_str();
  Chng.x = x;
  Chng.y = y;
  Chng.z = z;
  Chng.Type = SelectionChanges::SetPreselect;


  Notify(Chng);

  //Base::Console().Log("Sel : Add preselect %s \n",pFeatName);

  // allows the preselection
  return true;
}

void SelectionSingelton::rmvPreselect()
{
  if(DocName == "")
    return;


  SelectionChanges Chng;
  Chng.pDocName  = DocName.c_str();
  Chng.pFeatName = FeatName.c_str();
  Chng.pSubName  = SubName.c_str();
  Chng.Type = SelectionChanges::RmvPreselect;

  Notify(Chng);

  DocName = "";
  FeatName= "";
  SubName = "";
  hx = 0;
  hy = 0;
  hz = 0;
 
  //Base::Console().Log("Sel : Rmv preselect \n");

}



bool SelectionSingelton::addSelection(const char* pDocName, const char* pFeatName, const char* pSubName, float x, float y, float z)
{
  // already in ?
  if(isSelected(pDocName, pFeatName, pSubName))
    return true;

  _SelObj temp;

  if(pDocName)
    temp.pDoc =  App::GetApplication().getDocument(pDocName);
  else
    temp.pDoc = App::GetApplication().getActiveDocument();

  if(pFeatName)
    temp.pFeat = temp.pDoc->getFeature(pFeatName);
  else
    temp.pFeat = 0;


  temp.DocName  = pDocName;
  temp.FeatName = pFeatName?pFeatName:"";
  temp.SubName  = pSubName ?pSubName :"";
  temp.x        = x;
  temp.y        = y;
  temp.z        = z;

  if(temp.pFeat)
    temp.TypeName = temp.pFeat->type();

  _SelList.push_back(temp);

  SelectionChanges Chng;

  Chng.pDocName  = pDocName;
  Chng.pFeatName = pFeatName?pFeatName:"";
  Chng.pSubName  = pSubName ?pSubName :"";
  Chng.x         = x;
  Chng.y         = y;
  Chng.z         = z;
  Chng.Type      = SelectionChanges::AddSelection;


  Notify(Chng);

  Base::Console().Log("Sel : Add Selection \"%s.%s.%s(%f,%f,%f)\"\n",pDocName,pFeatName,pSubName,x,y,z);

  // allow selection
  return true;

}

void SelectionSingelton::rmvSelection(const char* pDocName, const char* pFeatName, const char* pSubName)
{
  vector<SelectionChanges> rmvList;

  for( list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();)
  {
    if( ( It->DocName == pDocName && !pFeatName ) ||
        ( It->DocName == pDocName && pFeatName && It->FeatName == pFeatName && !pSubName ) ||
        ( It->DocName == pDocName && pFeatName && It->FeatName == pFeatName && pSubName && It->SubName == pSubName ))
    {
      SelectionChanges Chng;

      Chng.pDocName  = pDocName;
      Chng.pFeatName = pFeatName?pFeatName:"";
      Chng.pSubName  = pSubName ?pSubName :"";
      Chng.Type      = SelectionChanges::RmvSelection;

      It = _SelList.erase(It);
      
      rmvList.push_back(Chng);
      Base::Console().Log("Sel : Rmv Selection \"%s.%s.%s\"\n",pDocName,pFeatName,pSubName);
    } else 
      ++It;
  }

  for(vector<SelectionChanges>::iterator It2=rmvList.begin();It2!=rmvList.end();++It2)
    Notify(*It2);


}

void SelectionSingelton::clearSelection(void)
{
  _SelList.clear();

  SelectionChanges Chng;
  Chng.Type = SelectionChanges::ClearSelection;


  Notify(Chng);

  Base::Console().Log("Sel : Clear selection\n");

}

bool SelectionSingelton::isSelected(const char* pDocName, const char* pFeatName, const char* pSubName)
{
  for( list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();++It)
    if(It->DocName == pDocName && It->FeatName == pFeatName && It->SubName == pSubName )
      return true;
  return false;
}


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
SelectionSingelton::SelectionSingelton()
{
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
SelectionSingelton::~SelectionSingelton()
{
}


SelectionSingelton* SelectionSingelton::_pcSingleton = NULL;

SelectionSingelton& SelectionSingelton::instance(void)
{
  if (_pcSingleton == NULL)
  {
    _pcSingleton = new SelectionSingelton;
  }

  return *_pcSingleton;
}

void SelectionSingelton::destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}
/*
void SelectionSingelton::addFeature(App::Feature *f)
{
  _FeatureSet.insert(f);

}

void SelectionSingelton::removeFeature(App::Feature *f)
{
  _FeatureSet.erase(f);


}
*/


//**************************************************************************
// separator for other implemetation aspects



