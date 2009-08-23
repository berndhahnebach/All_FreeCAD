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


#ifndef GUI_VIEWPROVIDERANNOTATION_H
#define GUI_VIEWPROVIDERANNOTATION_H

#include "ViewProviderDocumentObject.h"

class SoFontStyle;
class SoText2;
class SoAsciiText;
class SoBaseColor;
class SoTranslation;

namespace Gui
{

class GuiExport ViewProviderAnnotation : public ViewProviderDocumentObject
{
    PROPERTY_HEADER(Gui::ViewProviderAnnotation);

public:
    /// Constructor
    ViewProviderAnnotation(void);
    virtual ~ViewProviderAnnotation();

    // Display properties
    App::PropertyColor          TextColor;
    App::PropertyEnumeration    Justification;
    App::PropertyFloat        FontSize;

    void attach(App::DocumentObject *);
    void updateData(const App::Property*);
    std::vector<std::string> getDisplayModes(void) const;
    void setDisplayMode(const char* ModeName);

protected:
    void onChanged(const App::Property* prop);

private:
    SoFontStyle      * pFont;
    SoText2          * pLabel;
    SoAsciiText      * pLabel3d;
    SoBaseColor      * pColor;
    SoTranslation    * pTranslation;

    static const char* JustificationEnums[];
};

} //namespace Gui


#endif // GUI_VIEWPROVIDERANNOTATION_H
