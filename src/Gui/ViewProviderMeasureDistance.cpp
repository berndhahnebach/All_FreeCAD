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
# include <sstream>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoFontStyle.h>
# include <Inventor/nodes/SoText2.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoIndexedLineSet.h>
# include <Inventor/nodes/SoPointSet.h>
# include <Inventor/nodes/SoDrawStyle.h>
#endif

#include "ViewProviderMeasureDistance.h"
#include <Gui/View3DInventorViewer.h>
#include <App/PropertyGeo.h>
#include <App/PropertyStandard.h>
#include <App/MeasureDistance.h>
#include <Base/Console.h>

using namespace Gui;

PROPERTY_SOURCE(Gui::ViewProviderMeasureDistance, Gui::ViewProviderDocumentObject)


ViewProviderMeasureDistance::ViewProviderMeasureDistance() 
{
    ADD_PROPERTY(TextColor,(1.0f,1.0f,1.0f));
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

    // indexes used to create the edges
    static const int32_t lines[9] =
    {
        0,2,-1,
        1,3,-1,
        2,3,-1
    };

    pCoords = new SoCoordinate3();
    pCoords->ref();
    pCoords->point.setNum(4);

    pLines  = new SoIndexedLineSet();
    pLines->ref();
    pLines->coordIndex.setNum(9);
    pLines->coordIndex.setValues(0, 9, lines);
}

ViewProviderMeasureDistance::~ViewProviderMeasureDistance()
{
    pFont->unref();
    pLabel->unref();
    pColor->unref();
    pTranslation->unref();
    pCoords->unref();
    pLines->unref();
}

void ViewProviderMeasureDistance::onChanged(const App::Property* prop)
{
    if (prop == &TextColor) {
        const App::Color& c = TextColor.getValue();
        pColor->rgb.setValue(c.r,c.g,c.b);
    }
    else if (prop == &FontSize) {
        pFont->size = FontSize.getValue();
    }
    else {
        ViewProviderDocumentObject::onChanged(prop);
    }
}

std::vector<std::string> ViewProviderMeasureDistance::getDisplayModes(void) const
{
    // add modes
    std::vector<std::string> StrList;
    StrList.push_back("Base");
    return StrList;
}

void ViewProviderMeasureDistance::setDisplayMode(const char* ModeName)
{
    if (strcmp(ModeName, "Base") == 0)
        setDisplayMaskMode("Base");
    ViewProviderDocumentObject::setDisplayMode(ModeName);
}

void ViewProviderMeasureDistance::attach(App::DocumentObject* f)
{
    ViewProviderDocumentObject::attach(f);

    SoSeparator *lineSep = new SoSeparator();
    lineSep->addChild(pCoords);
    lineSep->addChild(pLines);
    SoDrawStyle* style = new SoDrawStyle();
    style->pointSize = 4.0f;
    SoPointSet* points = new SoPointSet();
    points->numPoints=2;
    lineSep->addChild(style);
    lineSep->addChild(points);

    SoSeparator* textsep = new SoSeparator();
    textsep->addChild(pTranslation);
    textsep->addChild(pFont);
    textsep->addChild(pLabel);

    SoSeparator* sep = new SoSeparator();
    sep->addChild(pColor);
    sep->addChild(lineSep);
    sep->addChild(textsep);
    addDisplayMaskMode(sep, "Base");
}

void ViewProviderMeasureDistance::updateData(const App::Property* prop)
{
    if (prop->getTypeId() == App::PropertyVector::getClassTypeId()) {
        if (strcmp(prop->getName(),"P1") == 0) {
            Base::Vector3f v = static_cast<const App::PropertyVector*>(prop)->getValue();
            pCoords->point.set1Value(0, SbVec3f(v.x,v.y,v.z));
        }
        else if (strcmp(prop->getName(),"P2") == 0) {
            Base::Vector3f v = static_cast<const App::PropertyVector*>(prop)->getValue();
            pCoords->point.set1Value(1, SbVec3f(v.x,v.y,v.z));
        }

        float length = 10.0f;
        SbVec3f pt1 = pCoords->point[0];
        SbVec3f pt2 = pCoords->point[1];
        SbVec3f dif = pt1-pt2;
        if (dif.sqrLength() < 10.0e-6f) {
            pCoords->point.set1Value(2, pt1+SbVec3f(0.0f,0.0f,length));
            pCoords->point.set1Value(3, pt2+SbVec3f(0.0f,0.0f,length));
        }
        else {
            SbVec3f dir = dif.cross(SbVec3f(1.0f,0.0f,0.0f));
            if (dir.sqrLength() < 10.0e-6f)
                dir = dif.cross(SbVec3f(0.0f,1.0f,0.0f));
            if (dir.sqrLength() < 10.0e-6f)
                dir = dif.cross(SbVec3f(0.0f,0.0f,1.0f));
            dir.normalize();
            if (dir.dot(SbVec3f(0.0f,0.0f,1.0f)) < 0.0f)
                length = -length;
            pCoords->point.set1Value(2, pt1 + length*dir);
            pCoords->point.set1Value(3, pt2 + length*dir);
        }

        SbVec3f pos = (pCoords->point[2]+pCoords->point[3])/2.0f;
        pTranslation->translation.setValue(pos);

        std::stringstream s;
        s.precision(4);
        s.setf(std::ios::fixed | std::ios::showpoint);
        s << "Distance: " << dif.length();
        pLabel->string.setValue(s.str().c_str());
    }

    ViewProviderDocumentObject::updateData(prop);
}

SbBool ViewProviderMeasureDistance::firstPicked=TRUE;

void ViewProviderMeasureDistance::measureDistanceCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
        n->setHandled();
        view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
        view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), measureDistanceCallback, ud);
    }
    else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL) {
            Base::Console().Message("No point picked.\n");
            return;
        }

        n->setHandled();
        SbVec3f pt = point->getPoint();
        App::MeasureDistance* md  = reinterpret_cast<App::MeasureDistance*>(ud);
        if (firstPicked)
            md->P1.setValue(Base::Vector3f(pt[0],pt[1],pt[2]));
        else
            md->P2.setValue(Base::Vector3f(pt[0],pt[1],pt[2]));
        firstPicked = !firstPicked;
    }
}
