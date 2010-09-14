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
# include <sstream>
# include <QPixmap>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/actions/SoSearchAction.h>
# include <Inventor/draggers/SoDragger.h>
# include <Inventor/manips/SoCenterballManip.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCamera.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
# include <Inventor/nodes/SoDirectionalLight.h>
# include <Inventor/sensors/SoNodeSensor.h> 
# include <Inventor/SoPickedPoint.h>
# include <Inventor/actions/SoRayPickAction.h> 
#endif

#include "ViewProviderPythonFeature.h"
#include "ViewProviderPythonFeaturePy.h"
#include "SoFCSelection.h"
#include "SoFCBoundingBox.h"
#include "Tree.h"
#include "Window.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Document.h"
#include <App/GeoFeature.h>
#include <App/PropertyGeo.h>
#include <Base/Console.h>
#include <Base/Reader.h>
#include <Base/Interpreter.h>


using namespace Gui;


ViewProviderPythonFeatureImp::ViewProviderPythonFeatureImp(ViewProviderDocumentObject* vp)
  : object(vp)
{
}

ViewProviderPythonFeatureImp::~ViewProviderPythonFeatureImp()
{
}

QIcon ViewProviderPythonFeatureImp::getIcon() const
{
    // default icon
    static QPixmap px = BitmapFactory().pixmap("Tree_Python");

    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        App::Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
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
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("ViewProviderPythonFeature::getIcon: %s\n", e.what());
    }

    return px;
}

void ViewProviderPythonFeatureImp::attach(App::DocumentObject *pcObject)
{
    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        App::Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("attach"))) {
                Py::Callable method(vp.getAttr(std::string("attach")));
                Py::Tuple args(1);
                args.setItem(0, Py::Object(object->getPyObject(), true));
                method.apply(args);
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        const char* name = object->getObject()->Label.getValue();
        Base::Console().Error("ViewProviderPythonFeature::attach (%s): %s\n", name, e.what());
    }
}

void ViewProviderPythonFeatureImp::updateData(const App::Property* prop)
{
    // Run the updateData method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        App::Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("updateData"))) {
                Py::Callable method(vp.getAttr(std::string("updateData")));
                Py::Tuple args(2);
                args.setItem(0, Py::Object(object->getObject()->getPyObject(), true));
                const char* prop_name = object->getObject()->getName(prop);
                if (prop_name) {
                    args.setItem(1, Py::String(prop_name));
                    method.apply(args);
                }
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        const char* name = object->getObject()->Label.getValue();
        Base::Console().Error("ViewProviderPythonFeature::updateData (%s): %s\n", name, e.what());
    }
}

void ViewProviderPythonFeatureImp::onChanged(const App::Property* prop)
{
    // Run the execute method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        App::Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("onChanged"))) {
                Py::Callable method(vp.getAttr(std::string("onChanged")));
                Py::Tuple args(2);
                args.setItem(0, Py::Object(object->getPyObject(), true));
                std::string prop_name = object->getName(prop);
                args.setItem(1, Py::String(prop_name));
                method.apply(args);
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        const char* name = object->getObject()->Label.getValue();
        Base::Console().Error("ViewProviderPythonFeature::onChanged (%s): %s\n", name, e.what());
    }
}

const char* ViewProviderPythonFeatureImp::getDefaultDisplayMode() const
{
    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    static std::string mode;
    try {
        App::Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("getDefaultDisplayMode"))) {
                Py::Callable method(vp.getAttr(std::string("getDefaultDisplayMode")));
                Py::Tuple args(0);
                Py::String str(method.apply(args));
                mode = str.as_std_string();
                return mode.c_str();
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        const char* name = object->getObject()->Label.getValue();
        Base::Console().Error("ViewProviderPythonFeature::getDefaultDisplayMode (%s): %s\n", name, e.what());
    }

    return 0;
}

std::vector<std::string> ViewProviderPythonFeatureImp::getDisplayModes(void) const
{
    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    std::vector<std::string> modes;
    try {
        App::Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("getDisplayModes"))) {
                Py::Callable method(vp.getAttr(std::string("getDisplayModes")));
                Py::Tuple args(1);
                args.setItem(0, Py::Object(object->getPyObject(), true));
                Py::List list(method.apply(args));
                for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
                    Py::String str(*it);
                    modes.push_back(str.as_std_string());
                }
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        const char* name = object->getObject()->Label.getValue();
        Base::Console().Error("ViewProviderPythonFeature::getDisplayModes (%s): %s\n", name, e.what());
    }

    return modes;
}

std::string ViewProviderPythonFeatureImp::setDisplayMode(const char* ModeName)
{
    // Run the getDisplayModes method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        App::Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("setDisplayMode"))) {
                Py::Callable method(vp.getAttr(std::string("setDisplayMode")));
                Py::Tuple args(1);
                args.setItem(0, Py::String(ModeName));
                Py::String str(method.apply(args));
                return str.as_std_string();
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        const char* name = object->getObject()->Label.getValue();
        Base::Console().Error("ViewProviderPythonFeature::setDisplayMode (%s): %s\n", name, e.what());
    }

    return ModeName;
}

// ---------------------------------------------------------

namespace Gui {
PROPERTY_SOURCE_TEMPLATE(Gui::ViewProviderPythonFeature, Gui::ViewProviderDocumentObject)
// explicit template instantiation
template class GuiExport ViewProviderPythonFeatureT<ViewProviderDocumentObject>;
}

// ---------------------------------------------------------

namespace Gui {
PROPERTY_SOURCE_TEMPLATE(Gui::ViewProviderPythonGeometry, Gui::ViewProviderGeometryObject)
// explicit template instantiation
template class GuiExport ViewProviderPythonFeatureT<ViewProviderGeometryObject>;}


