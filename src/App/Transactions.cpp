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


using namespace App;

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
TransactionObject::TransactionObject()
{
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
TransactionObject::~TransactionObject()
{
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

TYPESYSTEM_SOURCE(App::TransactionObjectNew, App::TransactionObject);

//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
TransactionObjectNew::TransactionObjectNew()
{
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
TransactionObjectNew::~TransactionObjectNew()
{
}

void TransactionObjectNew::Save (Writer &writer) const{
  assert(0);
} 

void TransactionObjectNew::Restore(XMLReader &reader){
  assert(0);
} 


//**************************************************************************
// separator for other implemetation aspects




