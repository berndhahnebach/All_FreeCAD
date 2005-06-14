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
#endif


#include "Mesh.h"
#include "Core/MeshKernel.h"

using namespace Mesh;


// ----------------------------------------------------------------------------

MeshPropertyNormal::MeshPropertyNormal(int size)
{
  Normales.resize(size);
}


void MeshPropertyNormal::resizePoints(void)
{
  setInvalid();
  Normales.clear();
}


void MeshPropertyNormal::transform(const Matrix4D &rclMat)
{
  // trivial implementation ;-)
  setInvalid();
  Normales.clear();

}



// ----------------------------------------------------------------------------

void DataWithPropertyBag::Add(PropertyBag* New, const char* Name)
{
  _Properties[Name] = New;
}

void DataWithPropertyBag::Remove(const char* Name)
{
  _Properties.erase(Name); 
}


void DataWithPropertyBag::RemoveType(const char* TypeName)
{
  std::vector<std::string> toErase;

  for(std::map<std::string,PropertyBag*>::iterator It= _Properties.begin();It!=_Properties.end();It++)
    if(strcmp(It->second->GetType(),TypeName) == 0)
      toErase.push_back(It->first);

  for(std::vector<std::string>::iterator It2 = toErase.begin();It2 != toErase.end();It2++)
    _Properties.erase(*It2);
}


PropertyBag* DataWithPropertyBag::Get(const char* Name)
{
  return _Properties[Name];
}


PropertyBag* DataWithPropertyBag::GetFirstOfType(const char* TypeName)
{
  for( std::map<std::string,PropertyBag*>::iterator It = _Properties.begin();It!=_Properties.end();It++)
    if( strcmp(It->second->GetType(),TypeName)==0 )
      return It->second;

  return 0;
}


std::list<PropertyBag*> DataWithPropertyBag::GetAllOfType(const char* TypeName)
{
  std::list<PropertyBag*> List;

  for( std::map<std::string,PropertyBag*>::iterator It = _Properties.begin();It!=_Properties.end();It++)
    if( strcmp(It->second->GetType(),TypeName)==0 )
      List.push_back( It->second);

    return List;
}


std::set<std::string> DataWithPropertyBag::GetAllTypes(void)
{
  std::set<std::string> Set;
  
  for( std::map<std::string,PropertyBag*>::iterator It = _Properties.begin();It!=_Properties.end();It++)
    Set.insert( It->second->GetType());

  return Set;
}


void DataWithPropertyBag::operator= ( const DataWithPropertyBag& New)
{
  for( std::map<std::string,PropertyBag*>::const_iterator It = New._Properties.begin();It!=New._Properties.end();It++)
    *(_Properties[It->first]) = *(It->second);
}

void DataWithPropertyBag::clear(void)
{
  _Properties.clear();
}

void DataWithPropertyBag::transform(const Matrix4D &rclMat)
{
  for( std::map<std::string,PropertyBag*>::const_iterator It = _Properties.begin();It!=_Properties.end();It++)
    It->second->transform(rclMat);
}
//*************************************************************************************************************

MeshWithProperty::MeshWithProperty(void)
{
  _Mesh = new MeshKernel();
}


MeshWithProperty::MeshWithProperty(const MeshWithProperty& New)
{
  _Mesh = new MeshKernel();

  *this = New; 
}

void MeshWithProperty::operator= ( const MeshWithProperty& New)
{
  _Mesh = New._Mesh;
  
  DataWithPropertyBag::operator= (New);
}

void MeshWithProperty::clear(void)
{
  DataWithPropertyBag::clear();
  _Mesh->Clear();
}

void MeshWithProperty::transform(const Matrix4D &rclMat)
{
  DataWithPropertyBag::transform(rclMat);
  *_Mesh *= rclMat;
}

