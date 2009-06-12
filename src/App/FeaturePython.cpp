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
# include <sstream>
#endif


#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/Reader.h>

#include "FeaturePython.h"
#include "FeaturePythonPy.h"

using namespace App;

PROPERTY_SOURCE(App::FeaturePython, App::DocumentObject)


FeaturePython::FeaturePython()
{
    ADD_PROPERTY(Proxy,(Py::Object()));
    props = new DynamicProperty(this);
}

FeaturePython::~FeaturePython()
{
    delete props;
}

short FeaturePython::mustExecute() const
{
    if (this->isTouched())
        return 1;
    return 0;
}

/** Invokes the execute method of the registered proxy object.
 * To register a proxy object in Python do it as follows:
 * \code
 *  # Create a document and add a FeaturePython object
 *  d=FreeCAD.newDocument()
 *  f=d.addObject("App::FeaturePython")
 *
 * class Feature:
 *      def execute(arg,obj):
 *          FreeCAD.PrintMessage("Hello, World!")
 *
 *  # Save the callback function
 *  f.Proxy = Feature()
 *
 *  # Performing a recomputation of the document invokes the proxy's execute method
 *  d.recompute()
 * \endcode
 *
 * \note You must always define two parameters, self and e.g. obj for the FreeCAD
 * document object.
 */
DocumentObjectExecReturn *FeaturePython::execute(void)
{
    // Run the execute method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Py::Object feature = this->Proxy.getValue();
        Py::Callable method(feature.getAttr(std::string("execute")));
        Py::Tuple args(1);
        args.setItem(0, this->PythonObject);
        method.apply(args);
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        return new App::DocumentObjectExecReturn(e.what());
    }

    return DocumentObject::StdReturn;
}

void FeaturePython::onChanged(const Property* prop)
{
    // Run the execute method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Py::Object vp = this->Proxy.getValue();
        if (vp.hasAttr(std::string("onChanged"))) {
            Py::Callable method(vp.getAttr(std::string("onChanged")));
            Py::Tuple args(2);
            args.setItem(0, Py::Object(this->getPyObject(), true));
            std::string prop_name = this->getName(prop);
            args.setItem(1, Py::String(prop_name));
            method.apply(args);
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("FeaturePython::onChanged: %s\n", e.what());
    }

    DocumentObject::onChanged(prop);
}

void FeaturePython::getPropertyMap(std::map<std::string,Property*> &Map) const
{
    props->getPropertyMap(Map);
}

Property *FeaturePython::getPropertyByName(const char* name) const
{
    return props->getPropertyByName(name);
}

const char* FeaturePython::getName(const Property* prop) const
{
    return props->getName(prop);
}

short FeaturePython::getPropertyType(const Property* prop) const
{
    return props->getPropertyType(prop);
}

short FeaturePython::getPropertyType(const char *name) const
{
    return props->getPropertyType(name);
}

const char* FeaturePython::getPropertyGroup(const Property* prop) const
{
    return props->getPropertyGroup(prop);
}

const char* FeaturePython::getPropertyGroup(const char *name) const
{
    return props->getPropertyGroup(name);
}

const char* FeaturePython::getPropertyDocumentation(const Property* prop) const
{
    return props->getPropertyDocumentation(prop);
}

const char* FeaturePython::getPropertyDocumentation(const char *name) const
{
    return props->getPropertyDocumentation(name);
}

bool FeaturePython::isReadOnly(const Property* prop) const
{
    return props->isReadOnly(prop);
}

bool FeaturePython::isReadOnly(const char *name) const
{
    return props->isReadOnly(name);
}

bool FeaturePython::isHidden(const Property* prop) const
{
    return props->isHidden(prop);
}

bool FeaturePython::isHidden(const char *name) const
{
    return props->isHidden(name);
}

void FeaturePython::Save (Base::Writer &writer) const 
{
    writer.ObjectName = this->getNameInDocument();
    props->Save(writer);
}

void FeaturePython::Restore(Base::XMLReader &reader)
{
    props->Restore(reader);
}

PyObject *FeaturePython::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new FeaturePythonPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}

void FeaturePython::setPyObject(PyObject *obj)
{
    if (obj)
        PythonObject = obj;
    else
        PythonObject = Py::None();
}
