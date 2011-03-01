/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2010     *
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
#include <Base/Console.h>

#include "Geometry.h"
#include "GeometryPy.h"

#include "PropertyGeometryList.h"

using namespace App;
using namespace Base;
using namespace std;
using namespace Part;


//**************************************************************************
// PropertyGeometryList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(Part::PropertyGeometryList, App::PropertyLists);

//**************************************************************************
// Construction/Destruction


PropertyGeometryList::PropertyGeometryList()
{

}

PropertyGeometryList::~PropertyGeometryList()
{
    for (std::vector<Geometry*>::iterator it = _lValueList.begin(); it != _lValueList.end(); ++it)
        if (*it) delete *it;
}

void PropertyGeometryList::setSize(int newSize)
{
    _lValueList.resize(newSize);
}

int PropertyGeometryList::getSize(void) const
{
    return static_cast<int>(_lValueList.size());
}

void PropertyGeometryList::setValue(const Geometry* lValue)
{
    if (lValue) {
        aboutToSetValue();
        _lValueList.resize(1);
        _lValueList[0]=lValue->clone();
        hasSetValue();
    }
}

void PropertyGeometryList::setValues(const std::vector<Geometry*>& lValue)
{
    aboutToSetValue();
    _lValueList.resize(lValue.size());
    // copy all objects
    for (unsigned int i = 0; i < lValue.size(); i++)
        _lValueList[i] = lValue[i]->clone();

    hasSetValue();
}

PyObject *PropertyGeometryList::getPyObject(void)
{
    PyObject* list = PyList_New(getSize());
    for (int i = 0; i < getSize(); i++)
        PyList_SetItem( list, i, _lValueList[i]->getPyObject());
    return list;
}

void PropertyGeometryList::setPyObject(PyObject *value)
{
    if (PyList_Check(value)) {
        Py_ssize_t nSize = PyList_Size(value);
        std::vector<Geometry*> values;
        values.resize(nSize);

        for (Py_ssize_t i=0; i < nSize; ++i) {
            PyObject* item = PyList_GetItem(value, i);
            if (!PyObject_TypeCheck(item, &(GeometryPy::Type))) {
                std::string error = std::string("types in list must be 'Geometry', not ");
                error += item->ob_type->tp_name;
                throw Py::TypeError(error);
            }

            values[i] = static_cast<GeometryPy*>(item)->getGeometryPtr();
        }

        setValues(values);
    }
    else if (PyObject_TypeCheck(value, &(GeometryPy::Type))) {
        GeometryPy  *pcObject = static_cast<GeometryPy*>(value);
        setValue(pcObject->getGeometryPtr());
    }
    else {
        std::string error = std::string("type must be 'Geometry' or list of 'Geometry', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

void PropertyGeometryList::Save(Writer &writer) const
{
    writer.Stream() << writer.ind() << "<GeometryList count=\"" << getSize() <<"\">" << endl;
    writer.incInd();
    for (int i = 0; i < getSize(); i++) {
        writer.Stream() << writer.ind() << "<Geometry  type=\"" 
                        << _lValueList[i]->getTypeId().getName() << "\">" << endl;;
        writer.incInd();
        _lValueList[i]->Save(writer);
        writer.decInd();
        writer.Stream() << writer.ind() << "</Geometry>" << endl;
    }
    writer.decInd();
    writer.Stream() << writer.ind() << "</GeometryList>" << endl ;
}

void PropertyGeometryList::Restore(Base::XMLReader &reader)
{
    // read my element
    reader.readElement("GeometryList");
    // get the value of my attribute
    int count = reader.getAttributeAsInteger("count");

    std::vector<Geometry*> values;
    values.reserve(count);
    for (int i = 0; i < count; i++) {
        reader.readElement("Geometry");
        const char* TypeName = reader.getAttribute("type");
        Geometry *newG = (Geometry *)Base::Type::fromName(TypeName).createInstance();
        newG->Restore(reader);
        values.push_back(newG);
        reader.readEndElement("Geometry");
    }

    reader.readEndElement("GeometryList");

    // assignment
    setValues(values);
}

Property *PropertyGeometryList::Copy(void) const
{
    PropertyGeometryList *p = new PropertyGeometryList();

    p->_lValueList.resize(_lValueList.size());
    // copy all objects
    for(unsigned int i = 0;i<_lValueList.size(); i++)
        p->_lValueList[i]=_lValueList[i]->clone();

    return p;
}

void PropertyGeometryList::Paste(const Property &from)
{
    aboutToSetValue();
    const PropertyGeometryList& FromList = dynamic_cast<const PropertyGeometryList&>(from);
    _lValueList.resize(FromList._lValueList.size());
    // copy all objects
    for(unsigned int i = 0;i<FromList._lValueList.size(); i++)
        _lValueList[i]=FromList._lValueList[i]->clone();
    
    hasSetValue();
}

unsigned int PropertyGeometryList::getMemSize(void) const
{
    int size = sizeof(PropertyGeometryList);
    for (int i = 0; i < getSize(); i++)
        size += _lValueList[i]->getMemSize();
    return size;
}
