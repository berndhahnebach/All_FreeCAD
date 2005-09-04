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
#include <App/Application.h>
#include <App/Document.h>
#include <App/Feature.h>


using namespace Gui;
using namespace std;


vector<App::Feature*> SelectionSingelton::getSelectedFeatures(const char* pDocName)
{
  vector<App::Feature*> temp;
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
      if(It->pFeat)
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

  for( list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();++It)
    if(It->pDoc == pcDoc && It->TypeName.find_first_of(TypeName) == 0)
      iNbr++;

  return iNbr;
}


void SelectionSingelton::addSelection(const char* pDocName, const char* pFeatName, const char* pSubName)
{
  _SelObj temp;

  if(pDocName)
    temp.pDoc =  App::GetApplication().getDocument(pDocName);
  else
    temp.pDoc = App::GetApplication().getActiveDocument();

  temp.pFeat = temp.pDoc->getFeature(pFeatName);


  temp.DocName = pDocName;
  temp.FeatName = pFeatName;
  temp.SubName = pSubName;

  if(temp.pFeat)
    temp.TypeName = temp.pFeat->type();

  _SelList.push_back(temp);
}

void SelectionSingelton::rmvSelection(const char* pDocName, const char* pFeatName, const char* pSubName)
{
  for( list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    if(It->DocName == pDocName && It->FeatName == pFeatName && It->SubName == pSubName )
    {
      _SelList.erase(It);
      break;
    }
  }
}

void SelectionSingelton::clearSelection(void)
{
  _SelList.clear();
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



