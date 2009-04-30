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
#   include <assert.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......

#include <Base/Exception.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "DocumentObject.h"
#include "DocumentObjectPy.h"
#include "Document.h"

#include "PropertyLinks.h"

using namespace App;
using namespace Base;
using namespace std;




//**************************************************************************
//**************************************************************************
// PropertyLink
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyLink , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyLink::PropertyLink()
:_pcLink(0)
{

}


PropertyLink::~PropertyLink()
{

}

//**************************************************************************
// Base class implementer

void PropertyLink::setValue(App::DocumentObject * lValue)
{
    if (_pcLink != lValue) {
        aboutToSetValue();
        _pcLink = lValue;
        hasSetValue();
    }
}

App::DocumentObject * PropertyLink::getValue(void) const
{
    return _pcLink;
}

App::DocumentObject * PropertyLink::getValue(Base::Type t) const
{
    return (_pcLink && _pcLink->getTypeId().isDerivedFrom(t)) ? _pcLink : 0;
}

PyObject *PropertyLink::getPyObject(void)
{
    if(_pcLink)
        return _pcLink->getPyObject();
    else
        Py_Return;
}

void PropertyLink::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(DocumentObjectPy::Type))) {
        DocumentObjectPy  *pcObject = (DocumentObjectPy*)value;
        setValue(pcObject->getDocumentObjectPtr());
    }
    else if(Py_None == value) {
        setValue(0);
    }
    else {
        std::string error = std::string("type must be 'DocumentObject' or 'NoneType', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

void PropertyLink::Save (Writer &writer) const
{
    writer.Stream() << writer.ind() << "<Link value=\"" <<  (_pcLink?_pcLink->getNameInDocument():"") <<"\"/>" << std::endl;
}

void PropertyLink::Restore(Base::XMLReader &reader)
{
    // read my Element
    reader.readElement("Link");
    // get the value of my Attribute
    std::string name = reader.getAttribute("value");

    // Property not in a Feature!
    assert(getContainer()->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()) );

    if (name != ""){
        DocumentObject *pcObject = dynamic_cast<DocumentObject*>(getContainer())->getDocument()->getObject(name.c_str());
        assert(pcObject);
        setValue(pcObject);
    }
    else {
       setValue(0);
    }
}

Property *PropertyLink::Copy(void) const
{
    PropertyLink *p= new PropertyLink();
    p->_pcLink = _pcLink;
    return p;
}

void PropertyLink::Paste(const Property &from)
{
    setValue(dynamic_cast<const PropertyLink&>(from)._pcLink);
}

TYPESYSTEM_SOURCE(App::PropertyLinkList , App::PropertyLists);

//**************************************************************************
// Construction/Destruction


PropertyLinkList::PropertyLinkList()
{

}

PropertyLinkList::~PropertyLinkList()
{

}

void PropertyLinkList::setValue(DocumentObject* lValue)
{
    if (lValue){
        aboutToSetValue();
        _lValueList.resize(1);
        _lValueList[0]=lValue;
        hasSetValue();
    }
}

void PropertyLinkList::setValues(const std::vector<DocumentObject*>& lValue)
{
    aboutToSetValue();
    _lValueList=lValue;
    hasSetValue();
}

PyObject *PropertyLinkList::getPyObject(void)
{
    PyObject* list = PyList_New(	getSize() );
    for(int i = 0;i<getSize(); i++)
        PyList_SetItem( list, i, _lValueList[i]->getPyObject());
    return list;
}

void PropertyLinkList::setPyObject(PyObject *value)
{
    if (PyList_Check(value)) {
        Py_ssize_t nSize = PyList_Size(value);
        std::vector<DocumentObject*> values;
        values.resize(nSize);

        for (Py_ssize_t i=0; i<nSize;++i) {
            PyObject* item = PyList_GetItem(value, i);
            if (!PyObject_TypeCheck(item, &(DocumentObjectPy::Type))) {
                std::string error = std::string("type in list must be 'DocumentObject', not ");
                error += item->ob_type->tp_name;
                throw Py::TypeError(error);
            }

            values[i] = static_cast<DocumentObjectPy*>(item)->getDocumentObjectPtr();
        }

        setValues(values);
    }
    else if(PyObject_TypeCheck(value, &(DocumentObjectPy::Type))) {
        DocumentObjectPy  *pcObject = static_cast<DocumentObjectPy*>(value);
        setValue(pcObject->getDocumentObjectPtr());
    }
    else {
        std::string error = std::string("type must be 'DocumentObject' or list of 'DocumentObject', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

void PropertyLinkList::Save (Writer &writer) const
{
    writer.Stream() << writer.ind() << "<LinkList count=\"" <<  getSize() <<"\">" << endl;
    writer.incInd();
    for(int i = 0;i<getSize(); i++)
        writer.Stream() << writer.ind() << "<Link value=\"" <<  _lValueList[i]->getNameInDocument() <<"\"/>" << endl; ;
    writer.decInd();
    writer.Stream() << writer.ind() << "</LinkList>" << endl ;
}

void PropertyLinkList::Restore(Base::XMLReader &reader)
{
    // read my element
    reader.readElement("LinkList");
    // get the value of my Attribute
    int count = reader.getAttributeAsInteger("count");
    assert(getContainer()->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()) );

    std::vector<DocumentObject*> values(count);
    for (int i = 0; i < count; i++) {
        reader.readElement("Link");
        std::string name = reader.getAttribute("value");

        // Property not in an object!
        DocumentObject *pcObject = dynamic_cast<DocumentObject*>(getContainer())->getDocument()->getObject(name.c_str());
        assert(pcObject);
        values[i] = pcObject;
    }

    reader.readEndElement("LinkList");

    // assignment
    setValues(values);
}

Property *PropertyLinkList::Copy(void) const
{
    PropertyLinkList *p= new PropertyLinkList();
    p->_lValueList = _lValueList;
    return p;
}

void PropertyLinkList::Paste(const Property &from)
{
    aboutToSetValue();
    _lValueList = dynamic_cast<const PropertyLinkList&>(from)._lValueList;
    hasSetValue();
}
