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
            ViewProviderPythonFeature::attach(docObject);
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

// ----------------------------------------------------------------

PROPERTY_SOURCE(Gui::ViewProviderPythonGeometry, Gui::ViewProviderPythonFeature)

const App::PropertyIntegerConstraint::Constraints intPercent = {0,100,1};

ViewProviderPythonGeometry::ViewProviderPythonGeometry() : pcBoundSwitch(0)
{
    ADD_PROPERTY(ShapeColor,(0.8f,0.8f,0.8f));
    ADD_PROPERTY(Transparency,(0));
    Transparency.setConstraints(&intPercent);
    App::Material mat(App::Material::DEFAULT);
    ADD_PROPERTY(ShapeMaterial,(mat));
    ADD_PROPERTY(BoundingBox,(false));

    // Create the selection node
    pcHighlight = new SoFCSelection();
    pcHighlight->ref();

    float transparency;
    ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("View");

    // switch off preselection
    bool enablePre = hGrp->GetBool("EnablePreselection", false);
    bool enableSel = hGrp->GetBool("EnableSelection", false);
    if (!enablePre) {
        pcHighlight->highlightMode = Gui::SoFCSelection::OFF;
    }
    else {
        // Search for a user defined value with the current color as default
        SbColor highlightColor = pcHighlight->colorHighlight.getValue();
        unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
        highlight = hGrp->GetUnsigned("HighlightColor", highlight);
        highlightColor.setPackedValue((uint32_t)highlight, transparency);
        pcHighlight->colorHighlight.setValue(highlightColor);
    }
    if (!enableSel) {
        pcHighlight->selectionMode = Gui::SoFCSelection::SEL_OFF;
        pcHighlight->style = Gui::SoFCSelection::BOX;
    }
    else {
        // Do the same with the selection color
        SbColor selectionColor = pcHighlight->colorSelection.getValue();
        unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
        selection = hGrp->GetUnsigned("SelectionColor", selection);
        selectionColor.setPackedValue((uint32_t)selection, transparency);
        pcHighlight->colorSelection.setValue(selectionColor);
    }

    pcShapeMaterial = new SoMaterial;
    pcShapeMaterial->ref();
    ShapeMaterial.touch();

    pcBoundingBox = new Gui::SoFCBoundingBox;
    pcBoundingBox->ref();
}

ViewProviderPythonGeometry::~ViewProviderPythonGeometry()
{
    pcShapeMaterial->unref();
    pcHighlight->unref();
    pcBoundingBox->unref();
}

void ViewProviderPythonGeometry::onChanged(const App::Property* prop)
{
    // Actually, the properties 'ShapeColor' and 'Transparency' are part of the property 'ShapeMaterial'.
    // Both redundant properties are kept due to more convenience for the user. But we must keep the values
    // consistent of all these properties.
    if (prop == &ShapeColor) {
        const App::Color& c = ShapeColor.getValue();
        pcShapeMaterial->diffuseColor.setValue(c.r,c.g,c.b);
        if (c != ShapeMaterial.getValue().diffuseColor)
        ShapeMaterial.setDiffuseColor(c);
    } 
    else if (prop == &Transparency) {
        const App::Material& Mat = ShapeMaterial.getValue();
        long value = (long)(100*Mat.transparency);
        if (value != Transparency.getValue()) {
            float trans = Transparency.getValue()/100.0f;
            pcShapeMaterial->transparency = trans;
            ShapeMaterial.setTransparency(trans);
        }
    }
    else if (prop == &ShapeMaterial) {
        const App::Material& Mat = ShapeMaterial.getValue();
        long value = (long)(100*Mat.transparency);
        if (value != Transparency.getValue())
        Transparency.setValue(value);
        const App::Color& color = Mat.diffuseColor;
        if (color != ShapeColor.getValue())
        ShapeColor.setValue(Mat.diffuseColor);
        pcShapeMaterial->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
        pcShapeMaterial->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
        pcShapeMaterial->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
        pcShapeMaterial->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
        pcShapeMaterial->shininess.setValue(Mat.shininess);
        pcShapeMaterial->transparency.setValue(Mat.transparency);
    }
    else if (prop == &BoundingBox) {
        showBoundingBox(BoundingBox.getValue());
    }

    ViewProviderPythonFeature::onChanged(prop);
}

void ViewProviderPythonGeometry::attach(App::DocumentObject *pcObj)
{
    ViewProviderPythonFeature::attach(pcObj);
    pcHighlight->objectName = pcObj->getNameInDocument();
    pcHighlight->documentName = pcObj->getDocument()->getName();
    pcHighlight->subElementName = "Main";
}

void ViewProviderPythonGeometry::updateData(const App::Property* prop)
{
    if (prop->isDerivedFrom(App::PropertyComplexGeoData::getClassTypeId())) {
        Base::BoundBox3d box = static_cast<const App::PropertyComplexGeoData*>(prop)->getBoundingBox();
        pcBoundingBox->minBounds.setValue(box.MinX, box.MinY, box.MinZ);
        pcBoundingBox->maxBounds.setValue(box.MaxX, box.MaxY, box.MaxZ);
    }
    else if (prop->isDerivedFrom(App::PropertyPlacement::getClassTypeId())) {
        // Note: If R is the rotation, c the rotation center and t the translation
        // vector then Inventor applies the following transformation: R*(x-c)+c+t
        // In FreeCAD a placement only has a rotation and a translation part but
        // no rotation center. This means that the following equation must be ful-
        // filled: R * (x-c) + c + t = R * x + t
        //    <==> R * x + t - R * c + c = R * x + t
        //    <==> (I-R) * c = 0 ==> c = 0
        // This means that the center point must be the origin!
        Base::Placement p = static_cast<const App::PropertyPlacement*>(prop)->getValue();
        float q0 = (float)p.getRotation().getValue()[0];
        float q1 = (float)p.getRotation().getValue()[1];
        float q2 = (float)p.getRotation().getValue()[2];
        float q3 = (float)p.getRotation().getValue()[3];
        float px = (float)p.getPosition().x;
        float py = (float)p.getPosition().y;
        float pz = (float)p.getPosition().z;
        pcTransform->rotation.setValue(q0,q1,q2,q3);
        pcTransform->translation.setValue(px,py,pz);
        pcTransform->center.setValue(0.0f,0.0f,0.0f);
    }
    else {
        ViewProviderPythonFeature::updateData(prop);
    }
}

bool ViewProviderPythonGeometry::doubleClicked(void)
{
    Gui::Application::Instance->activeDocument()->setEdit(this);
    return true;
}

bool ViewProviderPythonGeometry::setEdit(int ModNum)
{
    SoSearchAction sa;
    sa.setInterest(SoSearchAction::FIRST);
    sa.setSearchingAll(FALSE);
    sa.setNode(this->pcTransform);
    sa.apply(pcRoot);
    SoPath * path = sa.getPath();
    if (path) {
        SoCenterballManip * manip = new SoCenterballManip;
        SoDragger* dragger = manip->getDragger();
        dragger->addStartCallback(dragStartCallback, this);
        dragger->addFinishCallback(dragFinishCallback, this);
        // Attach a sensor to the transform manipulator and set it as its user
        // data to delete it when the view provider leaves the edit mode
        SoNodeSensor* sensor = new SoNodeSensor(sensorCallback, this);
        //sensor->setPriority(0);
        sensor->attach(manip);
        manip->setUserData(sensor);
        return manip->replaceNode(path);
    }

    return false;
}

void ViewProviderPythonGeometry::unsetEdit(void)
{
    SoSearchAction sa;
    sa.setType(SoCenterballManip::getClassTypeId());
    sa.setInterest(SoSearchAction::FIRST);
    sa.apply(pcRoot);
    SoPath * path = sa.getPath();

    // No transform manipulator found.
    if (!path)
        return;

    // The manipulator has a sensor as user data and this sensor contains the view provider
    SoCenterballManip * manip = static_cast<SoCenterballManip*>(path->getTail());
    SoNodeSensor* sensor = reinterpret_cast<SoNodeSensor*>(manip->getUserData());

    // detach sensor
    sensor->detach();
    delete sensor;

    SoTransform* transform = this->pcTransform;
    manip->replaceManip(path, transform);

    if (this->pcObject->getTypeId().isDerivedFrom(App::GeoFeature::getClassTypeId())) {
        App::GeoFeature* geometry = static_cast<App::GeoFeature*>(this->pcObject);
        this->updateData(&geometry->Placement);
    }
}

void ViewProviderPythonGeometry::sensorCallback(void * data, SoSensor * s)
{
    SoNodeSensor* sensor = static_cast<SoNodeSensor*>(s);
    SoNode* node = sensor->getAttachedNode();

    if (node && node->getTypeId().isDerivedFrom(SoCenterballManip::getClassTypeId())) {
        // apply the transformation data to the placement
        SoCenterballManip* manip = static_cast<SoCenterballManip*>(node);
        float q0, q1, q2, q3;
        SbVec3f move = manip->translation.getValue();
        SbVec3f center = manip->center.getValue();
        manip->rotation.getValue().getValue(q0, q1, q2, q3);
    
        // get the placement
        ViewProviderPythonGeometry* view = reinterpret_cast<ViewProviderPythonGeometry*>(data);
        if (view && view->pcObject && view->pcObject->getTypeId().isDerivedFrom(App::GeoFeature::getClassTypeId())) {
            App::GeoFeature* geometry = static_cast<App::GeoFeature*>(view->pcObject);
            // Note: If R is the rotation, c the rotation center and t the translation
            // vector then Inventor applies the following transformation: R*(x-c)+c+t
            // In FreeCAD a placement only has a rotation and a translation part but
            // no rotation center. This means that we must divide the transformation
            // in a rotation and translation part.
            // R * (x-c) + c + t = R * x - R * c + c + t
            // The rotation part is R, the translation part t', however, is:
            // t' = t + c - R * c
            Base::Placement p;
            p.setRotation(Base::Rotation(q0,q1,q2,q3));
            Base::Vector3d t(move[0],move[1],move[2]);
            Base::Vector3d c(center[0],center[1],center[2]);
            t += c;
            p.getRotation().multVec(c,c);
            t -= c;
            p.setPosition(t);
            geometry->Placement.setValue(p);
        }
    }
}

void ViewProviderPythonGeometry::dragStartCallback(void *data, SoDragger *)
{
    // This is called when a manipulator is about to manipulating
    Gui::Application::Instance->activeDocument()->openCommand("Transform");
}

void ViewProviderPythonGeometry::dragFinishCallback(void *data, SoDragger *)
{
    // This is called when a manipulator has done manipulating
    Gui::Application::Instance->activeDocument()->commitCommand();
}

void ViewProviderPythonGeometry::showBoundingBox(bool show)
{
    if (!pcBoundSwitch && show) {
        pcBoundSwitch = new SoSwitch();
        SoSeparator* pBoundingSep = new SoSeparator();
        SoDrawStyle* lineStyle = new SoDrawStyle;
        lineStyle->lineWidth = 2.0f;
        pBoundingSep->addChild(lineStyle);
        SoBaseColor* color = new SoBaseColor();
        color->rgb.setValue(1.0f, 1.0f, 1.0f);
        pBoundingSep->addChild(color);

        pBoundingSep->addChild(pcBoundingBox);
        pcBoundingBox->coordsOn.setValue(false);
        pcBoundingBox->dimensionsOn.setValue(true);

        // add to the highlight node
        pcBoundSwitch->addChild(pBoundingSep);
        pcRoot->addChild(pcBoundSwitch);
    }

    if (pcBoundSwitch) {
        pcBoundSwitch->whichChild = (show ? 0 : -1);
    }
}

SoFCSelection* ViewProviderPythonGeometry::createFromSettings()
{
    SoFCSelection* sel = new SoFCSelection();

    float transparency;
    ParameterGrp::handle hGrp = Gui::WindowParameter::getDefaultParameter()->GetGroup("View");
    bool enablePre = hGrp->GetBool("EnablePreselection", false);
    bool enableSel = hGrp->GetBool("EnableSelection", false);
    if (!enablePre) {
        sel->highlightMode = Gui::SoFCSelection::OFF;
    }
    else {
        // Search for a user defined value with the current color as default
        SbColor highlightColor = sel->colorHighlight.getValue();
        unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
        highlight = hGrp->GetUnsigned("HighlightColor", highlight);
        highlightColor.setPackedValue((uint32_t)highlight, transparency);
        sel->colorHighlight.setValue(highlightColor);
    }
    if (!enableSel) {
        sel->selectionMode = Gui::SoFCSelection::SEL_OFF;
        sel->style = Gui::SoFCSelection::BOX;
    }
    else {
        // Do the same with the selection color
        SbColor selectionColor = sel->colorSelection.getValue();
        unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
        selection = hGrp->GetUnsigned("SelectionColor", selection);
        selectionColor.setPackedValue((uint32_t)selection, transparency);
        sel->colorSelection.setValue(selectionColor);
    }

    return sel;
}
