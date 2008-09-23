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
#ifndef _PreComp_
# include <Inventor/VRMLnodes/SoVRMLParent.h>
#endif

#include "SoFCDB.h"
#include "SoFCColorBar.h"
#include "SoFCColorLegend.h"
#include "SoFCColorGradient.h"
#include "SoFCSelection.h"
#include "SoFCBackgroundGradient.h"
#include "SoFCBoundingBox.h"
#include "SoFCSelection.h"
#include "SoFCSelectionAction.h"
#include "SoFCInteractiveElement.h"
#include "SoFCVectorizeSVGAction.h"

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
    SoFCBoundingBox                 ::initClass();
    SoInteraction                   ::init();
    SoFCSelection                   ::initClass();
    SoFCSelectionAction             ::initClass();
    SoFCDocumentAction              ::initClass();
    SoFCInteractiveElement          ::initClass();
    SoFCEnableSelectionAction       ::initClass();
    SoFCEnableHighlightAction       ::initClass();
    SoFCSelectionColorAction        ::initClass();
    SoFCHighlightColorAction        ::initClass();
    SoFCDocumentObjectAction        ::initClass();
    SoFCVectorizeSVGAction          ::initClass();


    PropertyItem                    ::init();
    PropertyStringItem              ::init();
    PropertyIntegerItem             ::init();
    PropertyIntegerConstraintItem   ::init();
    PropertyFloatItem               ::init();
    PropertyFloatConstraintItem     ::init();
    PropertyBoolItem                ::init();
    PropertyVectorItem              ::init();
    PropertyEnumItem                ::init();
    PropertyStringListItem          ::init();
    PropertyColorItem               ::init();
    PropertyFileItem                ::init();
    PropertyPathItem                ::init();
}

void Gui::SoFCDB::finish()
{
    // Coin doesn't provide a mechanism to free static members of own data types.
    // Hence, we need to define a static method e.g. 'finish()' for all new types 
    // to invoke the private member function 'atexit_cleanup()'.
    SoFCColorBarBase                ::finish();
    SoFCColorBar                    ::finish();
    SoFCColorLegend                 ::finish();
    SoFCColorGradient               ::finish();
    SoFCBackgroundGradient          ::finish();
    SoFCBoundingBox                 ::finish();
    SoFCSelection                   ::finish();
    SoFCSelectionAction             ::finish();
    SoFCDocumentAction              ::finish();
    SoFCDocumentObjectAction        ::finish();
    SoFCEnableSelectionAction       ::finish();
    SoFCEnableHighlightAction       ::finish();
    SoFCSelectionColorAction        ::finish();
    SoFCHighlightColorAction        ::finish();
}

// buffer acrobatics for inventor ****************************************************
static char * buffer;
static size_t buffer_size = 0;
static std::string cReturnString;

static void *
buffer_realloc(void * bufptr, size_t size)
{
    buffer = (char *)realloc(bufptr, size);
    buffer_size = size;
    return buffer;
}

const std::string& Gui::SoFCDB::writeNodesToString(SoNode * root)
{
    SoOutput out;
    buffer = (char *)malloc(1024);
    buffer_size = 1024;
    out.setBuffer(buffer, buffer_size, buffer_realloc);
    if (root && root->getTypeId().isDerivedFrom(SoVRMLParent::getClassTypeId()))
        out.setHeaderString("#VRML V2.0 utf8");

    SoWriteAction wa(&out);
    wa.apply(root);

    cReturnString = buffer;
    free(buffer);
    return cReturnString;
}
