/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qpushbutton.h>
# include <q3filedialog.h>
# include <qcombobox.h>
# include <qpainter.h>
# include <q3header.h>
# include <qcheckbox.h>
# include <qlayout.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3CString>
#endif

#include <App/PropertyStandard.h>

#include "../FileDialog.h"

#include "propertyeditorfile.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"

using namespace Gui::PropertyEditor;


TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyFileItem, Gui::PropertyEditor::PropertyItem);

PropertyFileItem::PropertyFileItem()
{
}

QVariant PropertyFileItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFile::getClassTypeId()));

    std::string value = ((App::PropertyFile*)prop)->getValue();
    return QVariant(QString::fromUtf8(value.c_str()));
}

void PropertyFileItem::setValue(const QVariant& /*value*/)
{
}

QVariant PropertyFileItem::toolTip(const App::Property* prop) const
{
    return value(prop);
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyPathItem, Gui::PropertyEditor::PropertyItem);

PropertyPathItem::PropertyPathItem()
{
}

QVariant PropertyPathItem::value(const App::Property* /*prop*/) const
{
    return QVariant();
}

void PropertyPathItem::setValue(const QVariant& /*value*/)
{
}
