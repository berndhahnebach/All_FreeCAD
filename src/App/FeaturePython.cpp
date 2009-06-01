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
}

FeaturePython::~FeaturePython()
{
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
    catch (Py::Exception& e) {
        std::string err;
        Py::Object o = Py::type(e);
        if (o.isString()) {
            Py::String s(o);
            err = s.as_std_string();
        }
        else {
            Py::String s(o.repr());
            err = s.as_std_string();
        }
        e.clear();
        return new App::DocumentObjectExecReturn(err);
    }

    return DocumentObject::StdReturn;
}

void FeaturePython::getPropertyMap(std::map<std::string,Property*> &Map) const
{
    // get the properties of the base class first and insert the dynamic properties afterwards
    DocumentObject::getPropertyMap(Map);
    for ( std::map<std::string,Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it )
        Map[it->first] = it->second;
}

Property *FeaturePython::getPropertyByName(const char* name) const
{
    std::map<std::string,Property*>::const_iterator it = objectProperties.find( name );
    if ( it != objectProperties.end() )
        return it->second;
    return DocumentObject::getPropertyByName( name );
}

const char* FeaturePython::getName(const Property* prop) const
{
    for ( std::map<std::string,Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it ) {
        if (it->second == prop)
            return it->first.c_str();
    }
    return DocumentObject::getName(prop);
}

Property* FeaturePython::addDynamicProperty(const char* type, const char* name)
{
    Property* pcProperty = (Property*) Base::Type::createInstanceByName(type,true);
    if (!pcProperty)
        return 0;
    if (!pcProperty->getTypeId().isDerivedFrom(Property::getClassTypeId())) {
        delete pcProperty;
        std::stringstream str;
        str << "'" << type << "' is not a property type";
        throw Base::Exception(str.str());
    }

    // get unique name
    std::string ObjectName;
    if (name && *name != '\0')
        ObjectName = getUniquePropertyName(name);
    else
        ObjectName = getUniquePropertyName(type);

    pcProperty->setContainer(this);
    objectProperties[ObjectName] = pcProperty;

    return pcProperty;
}

std::string FeaturePython::getUniquePropertyName(const char *Name) const
{
    // check for first character whether it's a digit
    std::string CleanName = Name;
    if (!CleanName.empty() && CleanName[0] >= 48 && CleanName[0] <= 57)
        CleanName[0] = '_';
    // strip illegal chars
    for (std::string::iterator it = CleanName.begin(); it != CleanName.end(); ++it) {
        if (!((*it>=48 && *it<=57) ||  // number
             (*it>=65 && *it<=90)  ||  // uppercase letter
             (*it>=97 && *it<=122)))   // lowercase letter
             *it = '_'; // it's neither number nor letter
    }

    // name in use?
    std::map<std::string,Property*> objectProps;
    getPropertyMap(objectProps);
    std::map<std::string,Property*>::const_iterator pos;
    pos = objectProps.find(CleanName);

    if (pos == objectProps.end()) {
        // if not, name is OK
        return CleanName;
    }
    else {
        // find highest suffix
        int nSuff = 0;
        for (pos = objectProps.begin();pos != objectProps.end();++pos) {
            const std::string &PropName = pos->first;
            if (PropName.substr(0, CleanName.length()) == CleanName) { // same prefix
                std::string clSuffix(PropName.substr(CleanName.size()));
                if (clSuffix.size() > 0) {
                    std::string::size_type nPos = clSuffix.find_first_not_of("0123456789");
                    if (nPos==std::string::npos)
                        nSuff = std::max<int>(nSuff, std::atol(clSuffix.c_str()));
                }
            }
        }

        std::stringstream str;
        str << CleanName << ++nSuff;
        return str.str();
    }
}

void FeaturePython::onChanged(const Property* prop)
{
    //if (prop == &Proxy) {
    //    if (value == Py::None()) {
    //    }
    //    else {
    //        // Here we can check whether 'value' is a method or function but we cannot check the argument list
    //        // This will do Python for us in the execute method (and throws an exception if not empty).
    //        if (!value.isCallable()) {
    //            throw Py::TypeError("Value must be callable");
    //        }
    //    }
    //}

    DocumentObject::onChanged(prop);
}

void FeaturePython::Save (Base::Writer &writer) const 
{
    DocumentObject::Save(writer);
}

void FeaturePython::Restore(Base::XMLReader &reader)
{
    reader.readElement("Properties");
    int Cnt = reader.getAttributeAsInteger("Count");

    for (int i=0 ;i<Cnt ;i++) {
        reader.readElement("Property");
        const char* PropName = reader.getAttribute("name");
        const char* TypeName = reader.getAttribute("type");
        Property* prop = getPropertyByName(PropName);
        try {
            if (!prop) prop = addDynamicProperty(TypeName, PropName);
        }
        catch(const Base::Exception& e) {
            // only handle this exception type
            Base::Console().Warning(e.what());
        }

        //NOTE: We must also check the type of the current property because a subclass of
        //PropertyContainer might change the type of a property but not its name. In this
        //case we would force to read-in a wrong property type and the behaviour would be
        //undefined.
        if (prop && strcmp(prop->getTypeId().getName(), TypeName) == 0)
            prop->Restore(reader);
        else if (prop)
            Base::Console().Warning("%s: Overread data for property %s of type %s,"
            " expected type is %s\n", this->getNameInDocument(), prop->getName(),
                                    prop->getTypeId().getName(), TypeName);
        else
            Base::Console().Warning("%s: No property found with name %s and type %s\n",
                                    this->getNameInDocument(), PropName, TypeName);
        reader.readEndElement("Property");
    }

    reader.readEndElement("Properties");
}

PyObject *FeaturePython::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new FeaturePythonPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}
