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
}

void Transaction::Save (Writer &writer) const{
  assert(0);
} 

void Transaction::Restore(XMLReader &reader){
  assert(0);
} 

int Transaction::getPos(void) const
{
  return iPos;
}


//**************************************************************************
// separator for other implemetation aspects


void Transaction::apply(Document &Doc)
{
  std::map<const DocumentObject*,TransactionObject*>::iterator It;

  for( It= _Objects.begin();It!=_Objects.end();++It)
  {
    It->second->apply(Doc,const_cast<DocumentObject*>(It->first));

  }


}

void Transaction::addObjectNew(const DocumentObject *Obj)
{
  TransactionObject *To = new TransactionObject(Obj);
  To->status = TransactionObject::New;
  _Objects[Obj] = To;
}

void Transaction::addObjectDel(const DocumentObject *Obj)
{
  map<const DocumentObject*,TransactionObject*>::iterator pos = _Objects.find(Obj);

  if(pos != _Objects.end())
    pos->second->status = TransactionObject::Del;
  else
  {
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
TransactionObject::TransactionObject(const DocumentObject *pcObj)
:status(New)
{
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
TransactionObject::~TransactionObject()
{
}

void TransactionObject::apply(Document &Doc, DocumentObject *pcObj)
{
  if(status == Del){
    // simply filling in the saved object
    Doc._remObject(pcObj);
  }else if(status == New){
    Doc._addObject(pcObj,pcObj->name.getValue());
  }else if(status == Chn){

  }

}



void TransactionObject::setProperty(const Property* pcProp)
{
  map<const Property*,Property*>::iterator pos = _PropChangeMap.find(pcProp);

  if(pos == _PropChangeMap.end())
    _PropChangeMap[pcProp] = pcProp->Copy();
  else
  {
    delete pos->second;
    pos->second = pcProp->Copy();
  }

}

void TransactionObject::Save (Writer &writer) const{
  assert(0);
} 

void TransactionObject::Restore(XMLReader &reader){
  assert(0);
} 


//**************************************************************************
// separator for other implemetation aspects




//**************************************************************************
//**************************************************************************
// TransactionObjectNew
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//TYPESYSTEM_SOURCE(App::TransactionObjectNew, App::TransactionObject);

//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
//TransactionObjectNew::TransactionObjectNew()
//{
//}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
//TransactionObjectNew::~TransactionObjectNew()
//{
//}

//void TransactionObjectNew::Save (Writer &writer) const{
 // assert(0);
//} 

//void TransactionObjectNew::Restore(XMLReader &reader){
//  assert(0);
//} 


//**************************************************************************
// separator for other implemetation aspects




