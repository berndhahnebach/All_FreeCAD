/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "SoFCDB.h"
#include "SoFCColorBar.h"
#include "SoFCColorLegend.h"
#include "SoFCColorGradient.h"
#include "SoFCSelection.h"
#include "SoFCBackgroundGradient.h"
#include "SoFCSelection.h"
#include "SoFCSelectionAction.h"
#include "SoFCInteractiveElement.h"

#include "propertyeditor/propertyeditordate.h"
#include "propertyeditor/propertyeditorfile.h"
#include "propertyeditor/propertyeditorfont.h"
#include "propertyeditor/propertyeditorinput.h"
#include "propertyeditor/propertyeditoritem.h"
#include "propertyeditor/propertyeditorlist.h"

using namespace Gui;
using namespace Gui::PropertyEditor;

void Gui::SoFCDB::init()
{
    SoFCColorBarBase                ::initClass();
    SoFCColorBar                    ::initClass();
    SoFCColorLegend                 ::initClass();
    SoFCColorGradient               ::initClass();
    SoFCBackgroundGradient          ::initClass();
    SoInteraction                   ::init();
    SoFCSelection                   ::initClass();
    SoFCSelectionAction             ::initClass();
    SoFCDocumentAction              ::initClass();
    SoFCInteractiveElement          ::initClass();
    SoFCEnableSelectionAction       ::initClass();
    SoFCEnableHighlightAction       ::initClass();
    SoFCSelectionColorAction        ::initClass();
    SoFCHighlightColorAction        ::initClass();
    SoFCDocumentObjectEvent         ::initClass();


    PropertyItem                    ::init();
    PropertyStringItem              ::init();
    PropertyIntegerItem             ::init();
    PropertyIntegerConstraintItem   ::init();
    PropertyFloatItem               ::init();
    PropertyFloatConstraintItem     ::init();
    PropertyBoolItem                ::init();
    PropertyEnumItem                ::init();
    PropertyStringListItem          ::init();
    PropertyColorItem               ::init();
    PropertyFileItem                ::init();
    PropertyPathItem                ::init();


    // deprecated ...
    EditableItem                    ::init();
    TimeEditorItem                  ::init();
    DateEditorItem                  ::init();
    DateTimeEditorItem              ::init();
    FileEditorItem                  ::init();
    PixmapEditorItem                ::init();
    ChildrenEditorItem              ::init();
    FontEditorItem                  ::init();
    ColorEditorItem                 ::init();
    TextEditorItem                  ::init();
    IntEditorItem                   ::init();
    IntConstraintEditorItem         ::init();
    FloatEditorItem                 ::init();
    FloatConstraintEditorItem       ::init();
    BoolEditorItem                  ::init();
    ListEditorItem                  ::init();
    ComboEditorItem                 ::init();
    CursorEditorItem                ::init();
}

void Gui::SoFCDB::finish()
{
    // Coin doesn't provide a mechanism to free static members of own data types. Hence, we need to define a static method e.g. 'finish()' for all new types 
    // to invoke the private member function 'atexit_cleanup()'.
    SoFCColorBarBase                ::finish();
    SoFCColorBar                    ::finish();
    SoFCColorLegend                 ::finish();
    SoFCColorGradient               ::finish();
    SoFCBackgroundGradient          ::finish();
    SoFCSelection                   ::finish();
    SoFCSelectionAction             ::finish();
    SoFCDocumentAction              ::finish();
    SoFCEnableSelectionAction       ::finish();
    SoFCEnableHighlightAction       ::finish();
    SoFCSelectionColorAction        ::finish();
    SoFCHighlightColorAction        ::finish();
}
