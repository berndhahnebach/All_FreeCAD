/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QPixmap>
#endif

#include "ViewProviderPythonFeature.h"
#include "ViewProviderPythonFeaturePy.h"
#include "Tree.h"
#include "BitmapFactory.h"
#include <Base/Console.h>
#include <Base/Reader.h>
#include <Base/Interpreter.h>


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderPythonFeature, Gui::ViewProviderDocumentObject)


ViewProviderPythonFeature::ViewProviderPythonFeature() : docObject(0)
{
    ADD_PROPERTY(Proxy,(Py::Object()));
    props = new App::DynamicProperty(this);
}

ViewProviderPythonFeature::~ViewProviderPythonFeature()
{
    delete props;
}

QIcon ViewProviderPythonFeature::getIcon() const
{
    // default icon
    static QPixmap px = BitmapFactory().pixmap("Tree_Python");

    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Py::Object vp = this->Proxy.getValue();
        if (vp.hasAttr(std::string("getIcon"))) {
            Py::Callable method(vp.getAttr(std::string("getIcon")));
            Py::Tuple args(0);
            Py::String str(method.apply(args));
            std::string content = str.as_std_string();

            // test if in XPM format
            QByteArray ary;
            QPixmap icon;
            int strlen = (int)content.size();
            ary.resize(strlen);
            for (int j=0; j<strlen; j++)
                ary[j]=content[j];
            icon.loadFromData(ary, "XPM");
            if (!icon.isNull()) {
                return icon;
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("ViewProviderPythonFeature::getIcon: %s\n", e.what());
    }

    return px;
}

void ViewProviderPythonFeature::onChanged(const App::Property* prop)
{
    if (prop == &Proxy) {
        if (docObject && !Proxy.getValue().is(Py::_None())) {
            attach(docObject);
            updateView();
            docObject = 0;
        }
    }
    else {
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
            Base::Console().Error("ViewProviderPythonFeature::onChanged: %s\n", e.what());
        }

        ViewProviderDocumentObject::onChanged(prop);
    }
}

void ViewProviderPythonFeature::attach(App::DocumentObject *obj)
{
    if (docObject) {
        // Run the getDisplayModes method of the proxy object.
        Base::PyGILStateLocker lock;
        try {
            Py::Object vp = this->Proxy.getValue();
            Py::Callable method(vp.getAttr(std::string("attach")));
            Py::Tuple args(1);
            args.setItem(0, Py::Object(this->getPyObject(), true));
            method.apply(args);
        }
        catch (Py::Exception&) {
            Base::PyException e; // extract the Python error text
            Base::Console().Error("ViewProviderPythonFeature::attach: %s\n", e.what());
        }

        ViewProviderDocumentObject::attach(obj);
    }
    else {
        docObject = obj;
        pcObject = obj;
    }
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
            args.setItem(0, Py::Object(pcObject->getPyObject(), true));
            std::string prop_name = pcObject->getName(prop);
            args.setItem(1, Py::String(prop_name));
            method.apply(args);
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("ViewProviderPythonFeature::updateData: %s\n", e.what());
    }

    ViewProviderDocumentObject::updateData(prop);
}

void ViewProviderPythonFeature::setDisplayMode(char const * ModeName)
{
    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Py::Object vp = this->Proxy.getValue();
        if (vp.hasAttr(std::string("setDisplayMode"))) {
            Py::Callable method(vp.getAttr(std::string("setDisplayMode")));
            Py::Tuple args(1);
            args.setItem(0, Py::String(ModeName));
            Py::String str(method.apply(args));
            setDisplayMaskMode(str.as_std_string().c_str());
        }
        else
            setDisplayMaskMode(ModeName);
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("ViewProviderPythonFeature::setDisplayMode: %s\n", e.what());
    }

    ViewProviderDocumentObject::setDisplayMode(ModeName);
}

void ViewProviderPythonFeature::addDisplayMode(SoNode *node, const char* type)
{
    // call from the view provider base class
    addDisplayMaskMode(node, type);
}

char const * ViewProviderPythonFeature::getDefaultDisplayMode(void) const
{
    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    static std::string mode;
    try {
        Py::Object vp = this->Proxy.getValue();
        if (vp.hasAttr(std::string("getDefaultDisplayMode"))) {
            Py::Callable method(vp.getAttr(std::string("getDefaultDisplayMode")));
            Py::Tuple args(0);
            Py::String str(method.apply(args));
            mode = str.as_std_string();
            return mode.c_str();
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("ViewProviderPythonFeature::getDefaultDisplayMode: %s\n", e.what());
    }

    return 0;
}

std::vector<std::string> ViewProviderPythonFeature::getDisplayModes(void)const
{
    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    std::vector<std::string> modes;
    try {
        Py::Object vp = this->Proxy.getValue();
        if (vp.hasAttr(std::string("getDisplayModes"))) {
            Py::Callable method(vp.getAttr(std::string("getDisplayModes")));
            Py::Tuple args(1);
            ViewProviderPythonFeature* that = const_cast<ViewProviderPythonFeature*>(this);
            args.setItem(0, Py::Object(that->getPyObject(), true));
            Py::List list(method.apply(args));
            for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
                Py::String str(*it);
                modes.push_back(str.as_std_string());
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("ViewProviderPythonFeature::getDisplayModes: %s\n", e.what());
    }

    return modes;
}

bool ViewProviderPythonFeature::setEdit(int)
{
    return false;
}

void ViewProviderPythonFeature::unsetEdit(void)
{
}

PyObject* ViewProviderPythonFeature::getPyObject()
{
    if (!pyViewObject)
        pyViewObject = new ViewProviderPythonFeaturePy(this);
    pyViewObject->IncRef();
    return pyViewObject;
}

void ViewProviderPythonFeature::getPropertyMap(std::map<std::string,App::Property*> &Map) const
{
    props->getPropertyMap(Map);
}

App::Property *ViewProviderPythonFeature::getPropertyByName(const char* name) const
{
    return props->getPropertyByName(name);
}

const char* ViewProviderPythonFeature::getName(const App::Property* prop) const
{
    return props->getName(prop);
}

short ViewProviderPythonFeature::getPropertyType(const App::Property* prop) const
{
    return props->getPropertyType(prop);
}

short ViewProviderPythonFeature::getPropertyType(const char *name) const
{
    return props->getPropertyType(name);
}

const char* ViewProviderPythonFeature::getPropertyGroup(const App::Property* prop) const
{
    return props->getPropertyGroup(prop);
}

const char* ViewProviderPythonFeature::getPropertyGroup(const char *name) const
{
    return props->getPropertyGroup(name);
}

const char* ViewProviderPythonFeature::getPropertyDocumentation(const App::Property* prop) const
{
    return props->getPropertyDocumentation(prop);
}

const char* ViewProviderPythonFeature::getPropertyDocumentation(const char *name) const
{
    return props->getPropertyDocumentation(name);
}

bool ViewProviderPythonFeature::isReadOnly(const App::Property* prop) const
{
    return props->isReadOnly(prop);
}

bool ViewProviderPythonFeature::isReadOnly(const char *name) const
{
    return props->isReadOnly(name);
}

bool ViewProviderPythonFeature::isHidden(const App::Property* prop) const
{
    return props->isHidden(prop);
}

bool ViewProviderPythonFeature::isHidden(const char *name) const
{
    return props->isHidden(name);
}

void ViewProviderPythonFeature::Save (Base::Writer &writer) const 
{
    props->Save(writer);
}

void ViewProviderPythonFeature::Restore(Base::XMLReader &reader)
{
    props->Restore(reader);
}
