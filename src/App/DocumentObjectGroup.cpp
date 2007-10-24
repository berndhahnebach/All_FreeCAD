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

using namespace App;

PROPERTY_SOURCE(App::DocumentObjectGroup, App::DocumentObject)


DocumentObjectGroup::DocumentObjectGroup() 
{
    ADD_PROPERTY_TYPE(Group,(""),"Base",(App::PropertyType)(Prop_ReadOnly|Prop_Output),"List of referenced objects");

    // make sure that the list is empty
    std::list<std::string> grp;
    Group.enableNotify(false);
    Group.setValues(grp);
    Group.enableNotify(true);
}

DocumentObjectGroup::~DocumentObjectGroup()
{
}

DocumentObject* DocumentObjectGroup::addObject(const char* sType, const char* pObjectName)
{
    DocumentObject* obj = getDocument().addObject(sType, pObjectName);
    if (obj) addObject(obj->getNameInDocument());
    return obj;
}

void DocumentObjectGroup::addObject(const char* Name)
{
    if (!hasObject(Name)) {
        std::vector<std::string> grp = Group.getValues();
        grp.push_back(Name);
        //Group.enableNotify(false);
        Group.setValues(grp);
        //Group.enableNotify(true);
    }
}

void DocumentObjectGroup::removeObject(const char* Name)
{
    std::vector<std::string> grp = Group.getValues();
    for (std::vector<std::string>::iterator it = grp.begin(); it != grp.end(); ++it) {
        if (*it == Name) {
            grp.erase(it);
            //Group.enableNotify(false);
            Group.setValues(grp);
            //Group.enableNotify(true);
            break;
        }
    }
}

DocumentObject *DocumentObjectGroup::getObject(const char *Name) const
{
    DocumentObject* obj = getDocument().getObject(Name);
    if (obj && hasObject(Name))
        return obj;
    return 0;
}

bool DocumentObjectGroup::hasObject(const char *Name) const
{
    const std::vector<std::string>& grp = Group.getValues();
    for (std::vector<std::string>::const_iterator it = grp.begin(); it != grp.end(); ++it) {
        if (*it == Name)
            return true;
    }

    return false;
}

std::vector<DocumentObject*> DocumentObjectGroup::getObjects() const
{
    std::vector<DocumentObject*> objs;
    const std::vector<std::string>& grp = Group.getValues();
    for (std::vector<std::string>::const_iterator it = grp.begin(); it != grp.end(); ++it) {
        DocumentObject* obj = this->getDocument().getObject(it->c_str());
        if (obj)
            objs.push_back(obj);
    }

    return objs;
}

std::vector<DocumentObject*> DocumentObjectGroup::getObjectsOfType(const Base::Type& typeId) const
{
    std::vector<DocumentObject*> type;
    const std::vector<std::string>& grp = Group.getValues();
    for (std::vector<std::string>::const_iterator it = grp.begin(); it != grp.end(); ++it) {
        DocumentObject* obj = this->getDocument().getObject(it->c_str());
        if (obj && obj->getTypeId().isDerivedFrom(typeId))
            type.push_back(obj);
    }

    return type;
}

int DocumentObjectGroup::countObjectsOfType(const Base::Type& typeId) const
{
    int ct=0;
    const std::vector<std::string>& grp = Group.getValues();
    for (std::vector<std::string>::const_iterator it = grp.begin(); it != grp.end(); ++it) {
        DocumentObject* obj = this->getDocument().getObject(it->c_str());
        if (obj && obj->getTypeId().isDerivedFrom(typeId))
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
        if (grp->hasObject(obj->getNameInDocument()))
            return grp;
    }

    return 0;
}

PyObject *DocumentObjectGroup::getPyObject()
{
    if (PythonObject.is(Py::_None())){
        // ref counter is set to 1
        PythonObject.set(new DocumentObjectGroupPy(this),true);
    }
    return Py::new_reference_to(PythonObject); 
}
