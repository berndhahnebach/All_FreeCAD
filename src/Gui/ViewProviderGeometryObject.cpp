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
# include <qlistview.h>
# include <qpixmap.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCamera.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
# include <Inventor/nodes/SoDirectionalLight.h>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/actions/SoRayPickAction.h> 
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "ViewProviderGeometryObject.h"
#include "View3DInventorViewer.h"
#include "SoFCSelection.h"
#include "SoFCBoundingBox.h"
#include "Window.h"

#include <Base/Placement.h>
#include <App/PropertyGeo.h>


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderGeometryObject, Gui::ViewProviderDocumentObject)

const App::PropertyIntegerConstraint::Constraints intPercent = {0,100,1};
      
ViewProviderGeometryObject::ViewProviderGeometryObject() : pcBoundSwitch(0)
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
    bool disablePre = hGrp->GetBool("DisablePreselection", false);
    bool disableSel = hGrp->GetBool("DisableSelection", false);
    if (disablePre) {
        pcHighlight->highlightMode = Gui::SoFCSelection::OFF;
    }
    else {
        // Search for a user defined value with the current color as default
        SbColor highlightColor = pcHighlight->colorHighlight.getValue();
        unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
        int a = (highlight)&0xff;
        highlight = hGrp->GetUnsigned("HighlightColor", highlight);
        highlight += a;
        highlightColor.setPackedValue((uint32_t)highlight, transparency);
        pcHighlight->colorHighlight.setValue(highlightColor);
    }
    if (disableSel) {
        pcHighlight->selectionMode = Gui::SoFCSelection::SEL_OFF;
        pcHighlight->style = Gui::SoFCSelection::BOX;
    }
    else {
        // Do the same with the selection color
        SbColor selectionColor = pcHighlight->colorSelection.getValue();
        unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
        int a = (selection)&0xff;
        selection = hGrp->GetUnsigned("SelectionColor", selection);
        selection += a;
        selectionColor.setPackedValue((uint32_t)selection, transparency);
        pcHighlight->colorSelection.setValue(selectionColor);
    }

    pcShapeMaterial = new SoMaterial;
    pcShapeMaterial->ref();
    ShapeMaterial.touch();

    pcBoundingBox = new Gui::SoFCBoundingBox;
    pcBoundingBox->ref();
    sPixmap = "Feature";
}

ViewProviderGeometryObject::~ViewProviderGeometryObject()
{
    pcShapeMaterial->unref();
    pcHighlight->unref();
    pcBoundingBox->unref();
}

void ViewProviderGeometryObject::onChanged(const App::Property* prop)
{
    // Actually, the properties 'ShapeColor' and 'Transparency' are part of the property 'ShapeMaterial'.
    // Both redundant properties are kept due to more convenience for the user. But we must keep the values
    // consistent of all these properties.
    if ( prop == &ShapeColor ) {
        const App::Color& c = ShapeColor.getValue();
        pcShapeMaterial->diffuseColor.setValue(c.r,c.g,c.b);
        if (c != ShapeMaterial.getValue().diffuseColor)
        ShapeMaterial.setDiffuseColor(c);
    } 
    else if ( prop == &Transparency ) {
        const App::Material& Mat = ShapeMaterial.getValue();
        long value = (long)(100*Mat.transparency);
        if (value != Transparency.getValue()) {
            float trans = Transparency.getValue()/100.0f;
            pcShapeMaterial->transparency = trans;
            ShapeMaterial.setTransparency(trans);
        }
    } 
    else if ( prop == &ShapeMaterial ) {
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
    else if ( prop == &BoundingBox ) {
        showBoundingBox( BoundingBox.getValue() );
    }
    else {
        ViewProviderDocumentObject::onChanged(prop);
    }
}

void ViewProviderGeometryObject::attach(App::DocumentObject *pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);
    pcHighlight->objectName = pcObj->getNameInDocument();
    pcHighlight->documentName = pcObj->getDocument()->getName();
    pcHighlight->subElementName = "Main";
}

void ViewProviderGeometryObject::updateData(const App::Property* prop)
{
    if (prop->isDerivedFrom(App::PropertyComplexGeoData::getClassTypeId())) {
        Base::BoundBox3f box = static_cast<const App::PropertyComplexGeoData*>(prop)->getBoundingBox();
        pcBoundingBox->minBounds.setValue(box.MinX, box.MinY, box.MinZ);
        pcBoundingBox->maxBounds.setValue(box.MaxX, box.MaxY, box.MaxZ);
    }
    else if (prop->isDerivedFrom(App::PropertyPlacement::getClassTypeId())) {
        Base::Placement p = static_cast<const App::PropertyPlacement*>(prop)->getValue();
        float q0 = (float)p._q[0];
        float q1 = (float)p._q[1];
        float q2 = (float)p._q[2];
        float q3 = (float)p._q[3];
        float px = (float)p._pos.x;
        float py = (float)p._pos.y;
        float pz = (float)p._pos.z;
        pcTransform->rotation.setValue(q0,q1,q2,q3);
        pcTransform->translation.setValue(px,py,pz);
    }
}

SoPickedPointList ViewProviderGeometryObject::getPickedPoints(const SbVec2s& pos, const View3DInventorViewer& viewer,bool pickAll) const
{
    SoSeparator* root = new SoSeparator;
    root->ref();
    root->addChild(viewer.getHeadlight());
    root->addChild(viewer.getCamera());
    root->addChild(this->pcHighlight);

    SoRayPickAction rp(viewer.getViewportRegion());
    rp.setPickAll(pickAll);
    rp.setPoint(pos);
    rp.apply(root);
    root->unref();

    // returns a copy of the list
    return rp.getPickedPointList();
}

SoPickedPoint* ViewProviderGeometryObject::getPickedPoint(const SbVec2s& pos, const View3DInventorViewer& viewer) const
{
    SoSeparator* root = new SoSeparator;
    root->ref();
    root->addChild(viewer.getHeadlight());
    root->addChild(viewer.getCamera());
    root->addChild(this->pcHighlight);

    SoRayPickAction rp(viewer.getViewportRegion());
    rp.setPoint(pos);
    rp.apply(root);
    root->unref();

    // returns a copy of the point
    SoPickedPoint* pick = rp.getPickedPoint();
    //return (pick ? pick->copy() : 0); // needs the same instance of CRT under MS Windows
    return (pick ? new SoPickedPoint(*pick) : 0);
}

void ViewProviderGeometryObject::showBoundingBox(bool show)
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
