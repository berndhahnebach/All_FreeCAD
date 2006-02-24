/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
#	include <assert.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Property.h"
#include "PropertyContainer.h"

using namespace App;


//**************************************************************************
//**************************************************************************
// Property
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::Property , Base::Persistance);

//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
Property::Property()
:father(0)
{

}

Property::~Property()
{

}

const char* Property::getName(void)
{
  return father->getName(this);
}


void Property::setContainer(PropertyContainer *Father)
{
  father = Father;
}

void Property::hasSetValue(void)
{
  if(father)
    father->onChanged(this);
}

void Property::aboutToSetValue(void)
{
  if(father)
    father->onBevorChange(this);
}


//**************************************************************************
//**************************************************************************
// Property
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE_ABSTRACT(App::PropertyLists , Base::Persistance);
