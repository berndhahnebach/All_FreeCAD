/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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
#endif
# include <Inventor/nodes/SoAnnotation.h>

#include "ViewProviderAnnotation.h"
#include <App/PropertyGeo.h>
#include <App/PropertyStandard.h>

using namespace Gui;

const char* ViewProviderAnnotation::JustificationEnums[]= {"Left","Right","Center",NULL};

PROPERTY_SOURCE(Gui::ViewProviderAnnotation, Gui::ViewProviderDocumentObject)


ViewProviderAnnotation::ViewProviderAnnotation() 
{
    ADD_PROPERTY(TextColor,(1.0f,1.0f,1.0f));
    ADD_PROPERTY(Justification,((long)0));
    Justification.setEnums(JustificationEnums);
    ADD_PROPERTY(FontSize,(12));

    pFont = new SoFontStyle();
    pFont->ref();
    pLabel = new SoText2();
    pLabel->ref();
    pColor = new SoBaseColor();
    pColor->ref();
    pTranslation = new SoTranslation();
    pTranslation->ref();

    TextColor.touch();
    FontSize.touch();
}

ViewProviderAnnotation::~ViewProviderAnnotation()
{
    pFont->unref();
    pLabel->unref();
    pColor->unref();
    pTranslation->unref();
}

void ViewProviderAnnotation::onChanged(const App::Property* prop)
{
    if (prop == &TextColor) {
        const App::Color& c = TextColor.getValue();
        pColor->rgb.setValue(c.r,c.g,c.b);
    }
    else if (prop == &Justification) {
        if (Justification.getValue() == 0)
            pLabel->justification = SoText2::LEFT;
        else if (Justification.getValue() == 1)
            pLabel->justification = SoText2::RIGHT;
        else if (Justification.getValue() == 2)
            pLabel->justification = SoText2::CENTER;
    }
    else if (prop == &FontSize) {
        pFont->size = FontSize.getValue();
    }
    else {
        ViewProviderDocumentObject::onChanged(prop);
    }
}

std::vector<std::string> ViewProviderAnnotation::getDisplayModes(void) const
{
    // add modes
    std::vector<std::string> StrList;
    StrList.push_back("Base");
    return StrList;
}

void ViewProviderAnnotation::setDisplayMode(const char* ModeName)
{
    if (strcmp(ModeName, "Base") == 0)
        setDisplayMaskMode("Base");
    ViewProviderDocumentObject::setDisplayMode(ModeName);
}

void ViewProviderAnnotation::attach(App::DocumentObject* f)
{
    ViewProviderDocumentObject::attach(f);

    SoAnnotation* anno = new SoAnnotation();
    anno->addChild(pTranslation);
    anno->addChild(pColor);
    anno->addChild(pFont);
    anno->addChild(pLabel);
    addDisplayMaskMode(anno, "Base");
}

void ViewProviderAnnotation::updateData(const App::Property* prop)
{
    if (prop->getTypeId() == App::PropertyStringList::getClassTypeId() && 
        strcmp(prop->getName(),"LabelText") == 0) {
        const std::vector<std::string> lines = static_cast<const App::PropertyStringList*>(prop)->getValues();
        int index=0;
        pLabel->string.setNum((int)lines.size());
        for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
            pLabel->string.set1Value(index++, SbString(it->c_str()));
        }
    }
    else if (prop->getTypeId() == App::PropertyVector::getClassTypeId() &&
        strcmp(prop->getName(),"Position") == 0) {
        Base::Vector3f v = static_cast<const App::PropertyVector*>(prop)->getValue();
        pTranslation->translation.setValue(v.x,v.y,v.z);
    }

    ViewProviderDocumentObject::updateData(prop);
}
