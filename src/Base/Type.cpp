/***************************************************************************
 *   Copyright (c) Riegel         <juergen.riegel@web.de>                  *
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
#include "Type.h"
#include "Exception.h"


using namespace Base;


struct Base::TypeData 
{
  TypeData(const char *theName,
           const Type type = Type::badType(),
           const Type theParent = Type::badType()
          ):name(theName),parent(theParent),type(type) { }

  std::string name;
  Type parent;
  Type type;
};

std::map<std::string,unsigned int> Type::typemap;
std::vector<TypeData*>             Type::typedata;

//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
Type::Type()
: index(0)
{
}


Type::Type(const Type& type)
:index(type.index)
{
}


/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
Type::~Type()
{
}


Type Type::badType(void)
{
  Type bad;
  bad.index = 0;
  return bad;
}


const Type Type::createType(const Type parent, const char *name)
{
  Type newType;
  newType.index = Type::typedata.size();
  TypeData * typeData = new TypeData(name, newType, parent);
  Type::typedata.push_back(typeData);

  // add to dictionary for fast lookup
  Type::typemap[name] = newType.getKey();

  return newType;
}


void Type::init(void)
{
  assert(Type::typedata.size() == 0);


  Type::typedata.push_back(new TypeData("BadType"));
  Type::typemap["BadType"] = 0;


}

Type Type::fromName(const char *name)
{
  std::map<std::string,unsigned int>::const_iterator pos;
  
  pos = typemap.find(name);
  if(pos != typemap.end())
    return typedata[pos->second]->type;
  else
    return Type::badType();
}

const char *Type::getName(void) const
{
  return typedata[index]->name.c_str();
}

const Type Type::getParent(void) const
{
  return typedata[index]->parent;
}

bool Type::isDerivedFrom(const Type type) const
{
  
  Type temp(*this);
  do {
    if(temp == type)
      return true;
    temp = temp.getParent();
  } while (temp != badType());

  return false;
}

int Type::getAllDerivedFrom(const Type type, std::vector<Type> & List)
{
  int cnt = 0;

  for(std::vector<TypeData*>::const_iterator it = typedata.begin();it!= typedata.end();++it)
  {
    if((*it)->parent == type)
    {
      List.push_back((*it)->type);
      cnt++;
    }
  }
  return cnt;
}

int Type::getNumTypes(void)
{
  return typedata.size();
}
