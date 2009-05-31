/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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


#ifndef APP_PROPERTYPYTHONOBJECT_H
#define APP_PROPERTYPYTHONOBJECT_H

#include <string>
#include <list>
#include <vector>
#include <CXX/Objects.hxx>

#include "Property.h"


namespace Base {
class Writer;
class XMLReader;
}

namespace App
{

class AppExport PropertyPythonObject : public Property
{
    TYPESYSTEM_HEADER();

public:
    PropertyPythonObject(void);
    virtual ~PropertyPythonObject();

    void setValue(Py::Object);
    Py::Object getValue() const;

    virtual PyObject *getPyObject(void);
    virtual void setPyObject(PyObject *);

    virtual void Save (Base::Writer &writer) const;
    virtual void Restore(Base::XMLReader &reader);
    virtual void SaveDocFile (Base::Writer &writer) const;
    virtual void RestoreDocFile(Base::Reader &reader);

    virtual unsigned int getMemSize (void) const;
    virtual Property *Copy(void) const;
    virtual void Paste(const Property &from);

private:
    Py::Object object;
};


} // namespace App

#endif // APP_PROPERTYPYTHONOBJECT_H
