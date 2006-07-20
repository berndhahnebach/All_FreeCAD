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
#include <App/DocumentObject.h>


using namespace Gui;
using namespace std;

std::vector<SelectionSingleton::SelObj> SelectionSingleton::getCompleteSelection() const
{
  vector<SelObj> temp;
  SelObj tempSelObj;

  for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    tempSelObj.DocName  = It->DocName.c_str();
    tempSelObj.FeatName = It->FeatName.c_str();
    tempSelObj.SubName  = It->SubName.c_str();
    tempSelObj.TypeName = It->TypeName.c_str();
    tempSelObj.pObject  = It->pObject;
    tempSelObj.pDoc     = It->pDoc;
    temp.push_back(tempSelObj);
  }

  return temp;
}

vector<SelectionSingleton::SelObj> SelectionSingleton::getSelection(const char* pDocName) const
{
  vector<SelObj> temp;
  SelObj tempSelObj;

  App::Document *pcDoc;
  string DocName;

  pcDoc = getDocument(pDocName);

  //FIXME: We should abandon all the 'active' document stuff because this make quite a lot of troubles.
  //Thus it should only be able to access (an existing) document stuff by its name. For Python there might
  //be an active document but not for internal stuff. (Werner 20060517)
  if(!pcDoc)
    return temp;

  for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    if(It->pDoc == pcDoc)
    {
      tempSelObj.DocName  = It->DocName.c_str();
      tempSelObj.FeatName = It->FeatName.c_str();
      tempSelObj.SubName  = It->SubName.c_str();
      tempSelObj.TypeName = It->TypeName.c_str();
      tempSelObj.pObject    = It->pObject;
      tempSelObj.pDoc     = It->pDoc;
        
      temp.push_back(tempSelObj);
    }
  }

  return temp;
}

vector<App::DocumentObject*> SelectionSingleton::getObjectsOfType(const Base::Type& typeId, const char* pDocName) const
{
  vector<App::DocumentObject*> temp;
  App::Document *pcDoc;

  pcDoc = getDocument(pDocName);

  //FIXME: We should abandon all the 'active' document stuff because this make quite a lot of troubles.
  //Thus it should only be able to access (an existing) document stuff by its name. For Python there might
  //be an active document but not for internal stuff. (Werner 20060517)
  if(!pcDoc)
    return temp;

  for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    if ( It->pDoc == pcDoc && It->pObject && It->pObject->getTypeId().isDerivedFrom( typeId ) )
    {
      temp.push_back(It->pObject);
    }
  }

  return temp;
}

std::vector<App::DocumentObject*> SelectionSingleton::getObjectsOfType(const char* typeName, const char* pDocName) const
{
  Base::Type typeId = Base::Type::fromName(typeName);
  if ( typeId == Base::Type::badType() )
    return std::vector<App::DocumentObject*>();
  return getObjectsOfType(typeId, pDocName);
}

unsigned int SelectionSingleton::countObjectsOfType(const Base::Type& typeId, const char* pDocName) const
{
  unsigned int iNbr=0;
  App::Document *pcDoc;

  pcDoc = getDocument(pDocName);

  //FIXME: We should abandon all the 'active' document stuff because this make quite a lot of troubles.
  //Thus it should only be able to access (an existing) document stuff by its name. For Python there might
  //be an active document but not for internal stuff. (Werner 20060517)
  if(!pcDoc)
    return 0;

  for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It)
  {
    if ( It->pDoc == pcDoc && It->pObject && It->pObject->getTypeId().isDerivedFrom( typeId ) )
    {
      iNbr++;
    }
  }

  return iNbr;
}

unsigned int SelectionSingleton::countObjectsOfType(const char* typeName, const char* pDocName) const
{
  Base::Type typeId = Base::Type::fromName(typeName);
  if ( typeId == Base::Type::badType() )
    return 0;
  return countObjectsOfType(typeId, pDocName);
}

bool SelectionSingleton::setPreselect(const char* pDocName, const char* pObjectName, const char* pSubName, float x, float y, float z)
{
  if(DocName != "")
    rmvPreselect();

  DocName = pDocName;
  FeatName= pObjectName;
  SubName = pSubName;
  hx = x;
  hy = y;
  hz = z;

  SelectionChanges Chng;

  Chng.pDocName  = DocName.c_str();
  Chng.pObjectName = FeatName.c_str();
  Chng.pSubName  = SubName.c_str();
  Chng.x = x;
  Chng.y = y;
  Chng.z = z;
  Chng.Type = SelectionChanges::SetPreselect;


  Notify(Chng);

  //Base::Console().Log("Sel : Add preselect %s \n",pObjectName);

  // allows the preselection
  return true;
}

void SelectionSingleton::rmvPreselect()
{
  if(DocName == "")
    return;


  SelectionChanges Chng;
  Chng.pDocName  = DocName.c_str();
  Chng.pObjectName = FeatName.c_str();
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

App::Document* SelectionSingleton::getDocument(const char* pDocName) const
{
  if(pDocName)
    return  App::GetApplication().getDocument(pDocName);
  else
    return App::GetApplication().getActiveDocument();
}

bool SelectionSingleton::addSelection(const char* pDocName, const char* pObjectName, const char* pSubName, float x, float y, float z)
{
  // already in ?
  if(isSelected(pDocName, pObjectName, pSubName))
    return true;

  _SelObj temp;

  temp.pDoc = getDocument(pDocName);

  if ( temp.pDoc )
  {
    if(pObjectName)
      temp.pObject = temp.pDoc->getObject(pObjectName);
    else
      temp.pObject = 0;


    temp.DocName  = pDocName;
    temp.FeatName = pObjectName?pObjectName:"";
    temp.SubName  = pSubName ?pSubName :"";
    temp.x        = x;
    temp.y        = y;
    temp.z        = z;

    if(temp.pObject)
      temp.TypeName = temp.pObject->getTypeId().getName();

    _SelList.push_back(temp);

    SelectionChanges Chng;

    Chng.pDocName  = pDocName;
    Chng.pObjectName = pObjectName?pObjectName:"";
    Chng.pSubName  = pSubName ?pSubName :"";
    Chng.x         = x;
    Chng.y         = y;
    Chng.z         = z;
    Chng.Type      = SelectionChanges::AddSelection;


    Notify(Chng);

    Base::Console().Log("Sel : Add Selection \"%s.%s.%s(%f,%f,%f)\"\n",pDocName,pObjectName,pSubName,x,y,z);

    // allow selection
    return true;
  }
  else // neither an existing nor active document available 
  {
    assert(0);
    return false;
  }
}

void SelectionSingleton::rmvSelection(const char* pDocName, const char* pObjectName, const char* pSubName)
{
  vector<SelectionChanges> rmvList;

  for( list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();)
  {
    if( ( It->DocName == pDocName && !pObjectName ) ||
        ( It->DocName == pDocName && pObjectName && It->FeatName == pObjectName && !pSubName ) ||
        ( It->DocName == pDocName && pObjectName && It->FeatName == pObjectName && pSubName && It->SubName == pSubName ))
    {
      // save in tmp. string vars
      std::string tmpDocName = It->DocName;
      std::string tmpFeaName = It->FeatName;
      std::string tmpSubName = It->SubName;

      // destroy the _SelObj item
      It = _SelList.erase(It);

      SelectionChanges Chng;
      Chng.pDocName  = tmpDocName.c_str();
      Chng.pObjectName = tmpFeaName.c_str();
      Chng.pSubName  = tmpSubName.c_str();
      Chng.Type      = SelectionChanges::RmvSelection;

      Notify(Chng);
      
      rmvList.push_back(Chng);
      Base::Console().Log("Sel : Rmv Selection \"%s.%s.%s\"\n",pDocName,pObjectName,pSubName);
    } else 
      ++It;
  }



}

void SelectionSingleton::clearSelection(const char* pDocName)
{
  App::Document* pDoc;
  pDoc = getDocument(pDocName);

  // the document 'pDocName' has already been removed
  if (!pDoc && !pDocName) {
    clearCompleteSelection();
  } else {
    std::string docName;
    if (pDocName)
      docName = pDocName;
    else
      docName = pDoc->getName(); // active document
    std::list<_SelObj> selList;
    for ( std::list<_SelObj>::iterator it = _SelList.begin(); it != _SelList.end(); ++it ) {
      if ( it->DocName != docName )
        selList.push_back(*it);
    }

    _SelList = selList;

    SelectionChanges Chng;
    Chng.Type = SelectionChanges::ClearSelection;
    Chng.pDocName = docName.c_str();

    Notify(Chng);

    Base::Console().Log("Sel : Clear selection\n");
  }
}

void SelectionSingleton::clearCompleteSelection()
{
  _SelList.clear();

  SelectionChanges Chng;
  Chng.Type = SelectionChanges::ClearSelection;
  Chng.pDocName = "";


  Notify(Chng);

  Base::Console().Log("Sel : Clear selection\n");
}

bool SelectionSingleton::isSelected(const char* pDocName, const char* pObjectName, const char* pSubName) const
{
  const char* tmpDocName = pDocName ? pDocName : "";
  const char* tmpFeaName = pObjectName ? pObjectName : "";
  const char* tmpSubName = pSubName ? pSubName : "";
  for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It)
    if(It->DocName == tmpDocName && It->FeatName == tmpFeaName && It->SubName == tmpSubName )
      return true;
  return false;
}

bool SelectionSingleton::isSelected(App::DocumentObject* obj) const
{
  if (!obj) return false;

  for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It) {
    if ( It->pObject == obj )
      return true;
  }

  return false;
}


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
SelectionSingleton::SelectionSingleton()
{
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
SelectionSingleton::~SelectionSingleton()
{
}


SelectionSingleton* SelectionSingleton::_pcSingleton = NULL;

SelectionSingleton& SelectionSingleton::instance(void)
{
  if (_pcSingleton == NULL)
  {
    _pcSingleton = new SelectionSingleton;
  }

  return *_pcSingleton;
}

void SelectionSingleton::destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}
/*
void SelectionSingleton::addObject(App::DocumentObject *f)
{
  _ObjectSet.insert(f);

}

void SelectionSingleton::removeObject(App::DocumentObject *f)
{
  _ObjectSet.erase(f);


}
*/


//**************************************************************************
// separator for other implemetation aspects



