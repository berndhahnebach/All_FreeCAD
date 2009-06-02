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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "PropertyPythonObject.h"
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Console.h>
#include <Base/Interpreter.h>

using namespace App;


TYPESYSTEM_SOURCE(App::PropertyPythonObject , App::Property);

PropertyPythonObject::PropertyPythonObject()
{
}

PropertyPythonObject::~PropertyPythonObject()
{
    // this is needed because the release of the pickled object may need the
    // GIL. Thus, we grab the GIL and replace the pickled with an empty object
    Base::PyGILStateLocker lock;
    this->object = Py::Object();
}

void PropertyPythonObject::setValue(Py::Object o)
{
    aboutToSetValue();
    this->object = o;
    hasSetValue();
}

Py::Object PropertyPythonObject::getValue() const
{
    return object;
}

PyObject *PropertyPythonObject::getPyObject(void)
{
    return Py::new_reference_to(this->object);
}

void PropertyPythonObject::setPyObject(PyObject * obj)
{
    this->object = obj;
}

void PropertyPythonObject::Save (Base::Writer &writer) const
{
    if (writer.isForceXML()) {
    }
    else {
        writer.Stream() << writer.ind() << "<Python file=\"" << 
        writer.addFile("Callable.pyc", this) << "\"/>" << std::endl;
    }
}

void PropertyPythonObject::Restore(Base::XMLReader &reader)
{
    reader.readElement("Python");
    std::string file(reader.getAttribute("file"));
    if (!file.empty()) {
        reader.addFile(file.c_str(),this);
    }
}

void PropertyPythonObject::SaveDocFile (Base::Writer &writer) const
{
    Base::PyGILStateLocker lock;
    try {
        Py::Module pickle(PyImport_ImportModule("pickle"),true);
        Py::Callable method(pickle.getAttr(std::string("dumps")));
        Py::Tuple args(1);
        args.setItem(0, this->object);
        Py::Object res = method.apply(args);
        Py::String str(res);
        std::string buffer = str.as_std_string();
        for (std::string::iterator it = buffer.begin(); it != buffer.end(); ++it)
            writer.Stream().put(*it);
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Warning("PropertyPythonObject::SaveDocFile: %s\n", e.what());
    }
}

void PropertyPythonObject::RestoreDocFile(Base::Reader &reader)
{
    aboutToSetValue();
    Base::PyGILStateLocker lock;
    try {
        std::string buffer;
        char c;
        while (reader.get(c)) {
            buffer.push_back(c);
        }

        Py::Module pickle(PyImport_ImportModule("pickle"),true);
        Py::Callable method(pickle.getAttr(std::string("loads")));
        Py::Tuple args(1);
        args.setItem(0, Py::String(buffer));
        Py::Object res = method.apply(args);
        this->object = res;
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Warning("PropertyPythonObject::RestoreDocFile: %s\n", e.what());
    }
    hasSetValue();
}

unsigned int PropertyPythonObject::getMemSize (void) const
{
    return 0;
}

Property *PropertyPythonObject::Copy(void) const
{
    return 0;
}

void PropertyPythonObject::Paste(const Property &from)
{
}
