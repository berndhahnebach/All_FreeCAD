/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <Inventor/nodes/SoAnnotation.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoFont.h>
# include <Inventor/nodes/SoText2.h>
# include <Inventor/nodes/SoAsciiText.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoRotationXYZ.h>
#endif

#include "ViewProviderAnnotation.h"
#include <App/PropertyGeo.h>
#include <App/PropertyStandard.h>
#include "SoFCSelection.h"
#include "Application.h"
#include "Document.h"

using namespace Gui;

const char* ViewProviderAnnotation::JustificationEnums[]= {"Left","Right","Center",NULL};
const char* ViewProviderAnnotation::RotationAxisEnums[]= {"X","Y","Z",NULL};

PROPERTY_SOURCE(Gui::ViewProviderAnnotation, Gui::ViewProviderDocumentObject)


ViewProviderAnnotation::ViewProviderAnnotation() 
{
    ADD_PROPERTY(TextColor,(1.0f,1.0f,1.0f));
    ADD_PROPERTY(Justification,((long)0));
    Justification.setEnums(JustificationEnums);
    ADD_PROPERTY(FontSize,(12));
    ADD_PROPERTY(FontName,("Arial"));
    ADD_PROPERTY(Rotation,(0));
    ADD_PROPERTY(RotationAxis,((long)2));
    RotationAxis.setEnums(RotationAxisEnums);

    pFont = new SoFont();
    pFont->ref();
    pLabel = new SoText2();
    pLabel->ref();
    pLabel3d = new SoAsciiText();
    pLabel3d->ref();
    pColor = new SoBaseColor();
    pColor->ref();
    pTranslation = new SoTranslation();
    pTranslation->ref();
    pRotationXYZ = new SoRotationXYZ();
    pRotationXYZ->ref();

    RotationAxis.touch();
    TextColor.touch();
    FontSize.touch();
    FontName.touch();

    sPixmap = "Tree_Annotation";
}

ViewProviderAnnotation::~ViewProviderAnnotation()
{
    pFont->unref();
    pLabel->unref();
    pLabel3d->unref();
    pColor->unref();
    pTranslation->unref();
    pRotationXYZ->unref();
}

void ViewProviderAnnotation::onChanged(const App::Property* prop)
{
    if (prop == &TextColor) {
        const App::Color& c = TextColor.getValue();
        pColor->rgb.setValue(c.r,c.g,c.b);
    }
    else if (prop == &Justification) {
        if (Justification.getValue() == 0) {
            pLabel->justification = SoText2::LEFT;
            pLabel3d->justification = SoAsciiText::LEFT;
        }
        else if (Justification.getValue() == 1) {
            pLabel->justification = SoText2::RIGHT;
            pLabel3d->justification = SoAsciiText::RIGHT;
        }
        else if (Justification.getValue() == 2) {
            pLabel->justification = SoText2::CENTER;
            pLabel3d->justification = SoAsciiText::CENTER;
        }
    }
    else if (prop == &FontSize) {
        pFont->size = FontSize.getValue();
    }
    else if (prop == &FontName) {
        pFont->name = FontName.getValue();
    }
    else if (prop == &RotationAxis) {
        if (RotationAxis.getValue() == 0) {
	    pRotationXYZ->axis = SoRotationXYZ::X;
	}
	else if (RotationAxis.getValue() == 1) {
	    pRotationXYZ->axis = SoRotationXYZ::Y;
	}
	else if (RotationAxis.getValue() == 2) {
	    pRotationXYZ->axis = SoRotationXYZ::Z;
	}
    }
    else if (prop == &Rotation) {
        pRotationXYZ->angle = (Rotation.getValue()/360)*(2*M_PI);
    }
    else {
        ViewProviderDocumentObject::onChanged(prop);
    }
}

std::vector<std::string> ViewProviderAnnotation::getDisplayModes(void) const
{
    // add modes
    std::vector<std::string> StrList;
    StrList.push_back("Screen");
    StrList.push_back("World");
    return StrList;
}

void ViewProviderAnnotation::setDisplayMode(const char* ModeName)
{
    if (strcmp(ModeName, "Screen") == 0)
        setDisplayMaskMode("Screen");
    else if (strcmp(ModeName, "World")==0)
        setDisplayMaskMode("World");

    ViewProviderDocumentObject::setDisplayMode(ModeName);
}

void ViewProviderAnnotation::attach(App::DocumentObject* f)
{
    ViewProviderDocumentObject::attach(f);

    SoAnnotation* anno = new SoAnnotation();
    SoAnnotation* anno3d = new SoAnnotation();

    SoFCSelection* textsep = new SoFCSelection();
    textsep->objectName = pcObject->getNameInDocument();
    textsep->documentName = pcObject->getDocument()->getName();
    textsep->subElementName = "Main";
    textsep->addChild(pTranslation);
    textsep->addChild(pRotationXYZ);
    textsep->addChild(pColor);
    textsep->addChild(pFont);
    textsep->addChild(pLabel);

    SoFCSelection* textsep3d = new SoFCSelection();
    textsep3d->objectName = pcObject->getNameInDocument();
    textsep3d->documentName = pcObject->getDocument()->getName();
    textsep3d->subElementName = "Main";
    textsep3d->addChild(pTranslation);
    textsep3d->addChild(pRotationXYZ);
    textsep3d->addChild(pColor);
    textsep3d->addChild(pFont);
    textsep3d->addChild(pLabel3d);

    anno->addChild(textsep);
    anno3d->addChild(textsep3d);

    addDisplayMaskMode(anno, "Screen");
    addDisplayMaskMode(anno3d, "World");
}

void ViewProviderAnnotation::updateData(const App::Property* prop)
{
    if (prop->getTypeId() == App::PropertyStringList::getClassTypeId() && 
        strcmp(prop->getName(),"LabelText") == 0) {
        const std::vector<std::string> lines = static_cast<const App::PropertyStringList*>(prop)->getValues();
        int index=0;
        pLabel->string.setNum((int)lines.size());
        pLabel3d->string.setNum((int)lines.size());
        for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
            pLabel->string.set1Value(index++, SbString(it->c_str()));
            pLabel3d->string.set1Value(index++, SbString(it->c_str()));
        }
    }
    else if (prop->getTypeId() == App::PropertyVector::getClassTypeId() &&
        strcmp(prop->getName(),"Position") == 0) {
        Base::Vector3f v = static_cast<const App::PropertyVector*>(prop)->getValue();
        pTranslation->translation.setValue(v.x,v.y,v.z);
    }

    ViewProviderDocumentObject::updateData(prop);
}
