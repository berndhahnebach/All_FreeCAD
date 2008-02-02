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
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Writer.h>
using Base::Writer;
#include <Base/Reader.h>
using Base::XMLReader;
#include "Transactions.h"
#include "Property.h"
#include "Document.h"
#include "DocumentObject.h"

using namespace App;
using namespace std;

TYPESYSTEM_SOURCE(App::Transaction, Base::Persistance);

//**************************************************************************
// Construction/Destruction

Transaction::Transaction()
:iPos(0)
{
}

Transaction::Transaction(int pos)
:iPos(pos)
{
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
Transaction::~Transaction()
{
  std::map<const DocumentObject*,TransactionObject*>::iterator It;
  for( It= _Objects.begin();It!=_Objects.end();++It)
    delete It->second;
}

void Transaction::Save (Writer &/*writer*/) const{
  assert(0);
} 

void Transaction::Restore(XMLReader &/*reader*/){
  assert(0);
} 

int Transaction::getPos(void) const
{
  return iPos;
}


//**************************************************************************
// separator for other implemetation aspects


void Transaction::apply(Document &Doc/*, DocChanges &ChangeList*/)
{
  std::map<const DocumentObject*,TransactionObject*>::iterator It;
  //for( It= _Objects.begin();It!=_Objects.end();++It)
  //    It->second->apply(Doc,const_cast<DocumentObject*>(It->first));
  for( It= _Objects.begin();It!=_Objects.end();++It)
      It->second->applyDel(Doc,const_cast<DocumentObject*>(It->first));
  for( It= _Objects.begin();It!=_Objects.end();++It)
      It->second->applyNew(Doc,const_cast<DocumentObject*>(It->first));
  for( It= _Objects.begin();It!=_Objects.end();++It)
      It->second->applyChn(Doc,const_cast<DocumentObject*>(It->first));
}

void Transaction::addObjectNew(DocumentObject *Obj)
{
  map<const DocumentObject*,TransactionObject*>::iterator pos = _Objects.find(Obj);

  if(pos != _Objects.end()){
      if(pos->second->status == TransactionObject::Del){
            delete pos->second;
            delete pos->first;
            _Objects.erase(pos);

 
      }else{
        pos->second->status = TransactionObject::New;
        pos->second->_NameInDocument = Obj->getNameInDocument();
        Obj->pcNameInDocument = 0;
      }
  }else{
    TransactionObject *To = new TransactionObject(Obj,Obj->getNameInDocument());
    _Objects[Obj] = To;
    // set name cache false
    Obj->pcNameInDocument = 0;
    To->status = TransactionObject::New;
  }
}

void Transaction::addObjectDel(const DocumentObject *Obj)
{

    map<const DocumentObject*,TransactionObject*>::iterator pos = _Objects.find(Obj);

    // is it created in this transaction ?
    if(pos != _Objects.end() && pos->second->status == TransactionObject::New){
        // remove completely from transaction
        delete pos->second;
        _Objects.erase(pos);
    }else if (pos != _Objects.end() && pos->second->status == TransactionObject::Chn)
        pos->second->status = TransactionObject::Del;
    else {
        TransactionObject *To = new TransactionObject(Obj);
        _Objects[Obj] = To;
        To->status = TransactionObject::Del;
    }
}

void Transaction::addObjectChange(const DocumentObject *Obj,const Property *Prop)
{
  map<const DocumentObject*,TransactionObject*>::iterator pos = _Objects.find(Obj);
  TransactionObject *To;

  if(pos != _Objects.end())
    To = pos->second;
  else
  {
    To = new TransactionObject(Obj);
    _Objects[Obj] = To;
    To->status = TransactionObject::Chn;
  }

  To->setProperty(Prop);

}





//**************************************************************************
//**************************************************************************
// TransactionObject
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE_ABSTRACT(App::TransactionObject, Base::Persistance);

//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
TransactionObject::TransactionObject(const DocumentObject */*pcObj*/,const char *NameInDocument)
:status(New)
{
  if(NameInDocument)
    _NameInDocument=NameInDocument;
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
TransactionObject::~TransactionObject()
{
  std::map<const Property*,Property*>::const_iterator It;
  for(It=_PropChangeMap.begin();It!=_PropChangeMap.end();++It)
    delete It->second;

}


void TransactionObject::applyDel(Document &Doc, DocumentObject *pcObj)
{
  if(status == Del){
    // simply filling in the saved object
    Doc._remObject(pcObj);
  }
}
void TransactionObject::applyNew(Document &Doc, DocumentObject *pcObj)
{
  if(status == New){
    Doc._addObject(pcObj,_NameInDocument.c_str());
  }

}
void TransactionObject::applyChn(Document &/*Doc*/, DocumentObject */*pcObj*/)
{
  if(status == New || status == Chn){
    // apply changes if any
    std::map<const Property*,Property*>::const_iterator It;
    for(It=_PropChangeMap.begin();It!=_PropChangeMap.end();++It)
      const_cast<Property*>(It->first)->Paste(*(It->second));
  }
}



void TransactionObject::setProperty(const Property* pcProp)
{
  map<const Property*,Property*>::iterator pos = _PropChangeMap.find(pcProp);

  if(pos == _PropChangeMap.end())
    _PropChangeMap[pcProp] = pcProp->Copy();
}

void TransactionObject::Save (Writer &/*writer*/) const{
  assert(0);
} 

void TransactionObject::Restore(XMLReader &/*reader*/){
  assert(0);
} 


//**************************************************************************
// separator for other implemetation aspects







