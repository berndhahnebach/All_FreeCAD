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
# include <qpixmap.h>
#endif

#include "ViewProviderPythonFeature.h"
#include "Tree.h"
#include <Base/Console.h>
#include <Base/Reader.h>
#include <Base/Interpreter.h>


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderPythonFeature, Gui::ViewProviderDocumentObject)


ViewProviderPythonFeature::ViewProviderPythonFeature()
{
    ADD_PROPERTY(Proxy,(Py::Object()));
}

ViewProviderPythonFeature::~ViewProviderPythonFeature()
{
}

QIcon ViewProviderPythonFeature::getIcon() const
{
    const char * Python_Feature_xpm[] = {
        "14 15 7 1",
        "# c #000000",
        "b c #0000ff",
        "d c #00ff00",
        "a c #808000",
        ". c None",
        "e c #ff0000",
        "c c #ffff00",
        "...###........",
        "..#aaa#.......",
        "..#b#ba##.....",
        ".##c#c#aa#....",
        "#aa#a#aaaa#...",
        "#aaaaaadaa#...",
        "#dddddd#aa#...",
        "##de#d##da#...",
        ".#ee###da#..##",
        ".#e##ddaa#.#a#",
        "..##ddaa#..#a#",
        "..#ddaa####aa#",
        "..#ddaa#aaaa#.",
        "..#ddaaaaaad#.",
        "...#ddddddd#.."};
    QPixmap px(Python_Feature_xpm);
    return px;
}

void ViewProviderPythonFeature::onChanged(const App::Property* prop)
{
    if (prop == &Proxy) {
    }
    else {
        ViewProviderDocumentObject::onChanged(prop);
    }
}

void ViewProviderPythonFeature::attach(App::DocumentObject *obj)
{
    ViewProviderDocumentObject::attach(obj);
}

void ViewProviderPythonFeature::updateData(const App::Property* prop)
{
    // Run the updateData method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Py::Object vp = this->Proxy.getValue();
        if (vp.hasAttr(std::string("updateData"))) {
            Py::Callable method(vp.getAttr(std::string("updateData")));
            Py::Tuple args(2);
            args.setItem(0, Py::Object(this->getPyObject(), true));
            App::Property* that = const_cast<App::Property*>(prop);
            args.setItem(1, Py::Object(that->getPyObject(), true));
            method.apply(args);
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("ViewProviderPythonFeature::updateData: %s\n", e.what());
    }

    ViewProviderDocumentObject::updateData(prop);
}

void ViewProviderPythonFeature::setDisplayMode(char const *)
{
}

char const * ViewProviderPythonFeature::getDefaultDisplayMode(void) const
{
    return 0;
}

std::vector<std::string> ViewProviderPythonFeature::getDisplayModes(void)const
{
    return std::vector<std::string>();
}

bool ViewProviderPythonFeature::setEdit(int)
{
    return false;
}

void ViewProviderPythonFeature::unsetEdit(void)
{
}

void ViewProviderPythonFeature::Save (Base::Writer &writer) const 
{
    ViewProviderDocumentObject::Save(writer);
}

void ViewProviderPythonFeature::Restore(Base::XMLReader &reader)
{
    reader.readElement("Properties");
    int Cnt = reader.getAttributeAsInteger("Count");

    for (int i=0 ;i<Cnt ;i++) {
        reader.readElement("Property");
        const char* PropName = reader.getAttribute("name");
        const char* TypeName = reader.getAttribute("type");
        App::Property* prop = getPropertyByName(PropName);
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
            Base::Console().Warning("ViewProviderPythonFeature: Overread data for property %s of type %s,"
            " expected type is %s\n", prop->getName(), prop->getTypeId().getName(), TypeName);
        else
            Base::Console().Warning("ViewProviderPythonFeature: No property found with name %s and type %s\n",
                                    PropName, TypeName);
        reader.readEndElement("Property");
    }

    reader.readEndElement("Properties");
}

PyObject* ViewProviderPythonFeature::getPyObject()
{
    return ViewProviderDocumentObject::getPyObject();
}

void ViewProviderPythonFeature::getPropertyMap(std::map<std::string,App::Property*> &Map) const
{
    // get the properties of the base class first and insert the dynamic properties afterwards
    ViewProviderDocumentObject::getPropertyMap(Map);
    for (std::map<std::string,App::Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it)
        Map[it->first] = it->second;
}

App::Property *ViewProviderPythonFeature::getPropertyByName(const char* name) const
{
    std::map<std::string,App::Property*>::const_iterator it = objectProperties.find(name);
    if (it != objectProperties.end())
        return it->second;
    return ViewProviderDocumentObject::getPropertyByName(name);
}

const char* ViewProviderPythonFeature::getName(const App::Property* prop) const
{
    for ( std::map<std::string,App::Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it ) {
        if (it->second == prop)
            return it->first.c_str();
    }
    return ViewProviderDocumentObject::getName(prop);
}

App::Property* ViewProviderPythonFeature::addDynamicProperty(const char* type, const char* name)
{
    App::Property* pcProperty = (App::Property*) Base::Type::createInstanceByName(type,true);
    if (!pcProperty)
        return 0;
    if (!pcProperty->getTypeId().isDerivedFrom(App::Property::getClassTypeId())) {
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

std::string ViewProviderPythonFeature::getUniquePropertyName(const char *Name) const
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
    std::map<std::string,App::Property*> objectProps;
    getPropertyMap(objectProps);
    std::map<std::string,App::Property*>::const_iterator pos;
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
