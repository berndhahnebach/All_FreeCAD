/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "DocumentObjectGroup.h"
#include "DocumentObjectGroupPy.h"
#include "Document.h"
#define new DEBUG_CLIENTBLOCK
using namespace App;

PROPERTY_SOURCE(App::DocumentObjectGroup, App::AbstractFeature)


DocumentObjectGroup::DocumentObjectGroup() : pcGroupPy(0)
{
  ADD_PROPERTY(Group,(0));

  // make sure that the list is empty
  std::vector<DocumentObject*> grp;
  Group.enableNotify(false);
  Group.setValues(grp);
  Group.enableNotify(true);
}

DocumentObjectGroup::~DocumentObjectGroup()
{
}

void DocumentObjectGroup::onChanged(const Property* prop)
{
  AbstractFeature::onChanged(prop);
}

DocumentObject* DocumentObjectGroup::addObject(const char* sType, const char* pObjectName)
{
  DocumentObject* obj = getDocument().addObject(sType, pObjectName);
  if ( obj )
    addObject(obj);
  return obj;
}

void DocumentObjectGroup::addObject(DocumentObject* obj)
{
  std::vector<DocumentObject*> grp = Group.getValues();
  std::vector<DocumentObject*>::iterator it;
  for ( it = grp.begin(); it != grp.end(); ++it ) {
    if ( *it == obj ) {
      break;
    }
  }

  if ( it == grp.end() )
  {
    grp.push_back(obj);
    Group.enableNotify(false);
    Group.setValues(grp);
    Group.enableNotify(true);
  }
}

void DocumentObjectGroup::removeObject(const char* sName)
{
  DocumentObject* obj = getObject(sName);
  if ( obj )
  {
    removeObject(obj);
    getDocument().remObject(sName);
  }
}

void DocumentObjectGroup::removeObject(DocumentObject* obj)
{
  std::vector<DocumentObject*> grp = Group.getValues();
  for ( std::vector<DocumentObject*>::iterator it = grp.begin(); it != grp.end(); ++it ) {
    if ( *it == obj ) {
      grp.erase(it);
      Group.enableNotify(false);
      Group.setValues(grp);
      Group.enableNotify(true);
      break;
    }
  }
}

DocumentObject *DocumentObjectGroup::getObject(const char *Name) const
{
  DocumentObject* obj = getDocument().getObject( Name );
  if ( obj && hasObject(obj) )
    return obj;
  return 0;
}

bool DocumentObjectGroup::hasObject(DocumentObject* obj) const
{
  const std::vector<DocumentObject*>& grp = Group.getValues();
  for ( std::vector<DocumentObject*>::const_iterator it = grp.begin(); it != grp.end(); ++it ) {
    if ( *it == obj )
      return true;
  }

  return false;
}

std::vector<DocumentObject*> DocumentObjectGroup::getObjects() const
{
  return Group.getValues();
}

std::vector<DocumentObject*> DocumentObjectGroup::getObjectsOfType(const Base::Type& typeId) const
{
  std::vector<DocumentObject*> type;
  const std::vector<DocumentObject*>& grp = Group.getValues();
  for ( std::vector<DocumentObject*>::const_iterator it = grp.begin(); it != grp.end(); ++it ) {
    if ( (*it)->getTypeId().isDerivedFrom( typeId ) )
      type.push_back(*it);
  }

  return type;
}

int DocumentObjectGroup::countObjectsOfType(const Base::Type& typeId) const
{
  int ct=0;
  const std::vector<DocumentObject*>& grp = Group.getValues();
  for ( std::vector<DocumentObject*>::const_iterator it = grp.begin(); it != grp.end(); ++it ) {
    if ( (*it)->getTypeId().isDerivedFrom( typeId ) )
      ct++;
  }

  return ct;
}

DocumentObjectGroup* DocumentObjectGroup::getGroupOfObject(DocumentObject* obj)
{
  const Document& doc = obj->getDocument();
  std::vector<DocumentObject*> grps = doc.getObjectsOfType( DocumentObjectGroup::getClassTypeId() );
  for ( std::vector<DocumentObject*>::iterator it = grps.begin(); it != grps.end(); ++it ) {
    DocumentObjectGroup* grp = (DocumentObjectGroup*)(*it);
    if ( grp->hasObject( obj ) )
      return grp;
  }

  return 0;
}

int DocumentObjectGroup::execute()
{
  return 0;
}

Base::PyObjectBase *DocumentObjectGroup::GetPyObject()
{
  if(!pcGroupPy){
    // ref counter is set to 1
    pcGroupPy = new DocumentObjectGroupPy(this);
  }
  
  pcGroupPy->IncRef();
  return pcGroupPy;
}
