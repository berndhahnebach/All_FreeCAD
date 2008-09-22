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
# include <qcolordialog.h>
# include <qfontdialog.h>
# include <qpainter.h>
#endif

#include <App/PropertyStandard.h>
#include <Gui/Application.h>

#include "propertyeditorfont.h"
#include "Widgets.h"

using namespace Gui::PropertyEditor;


TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyColorItem, Gui::PropertyEditor::PropertyItem);

PropertyColorItem::PropertyColorItem()
{
}

QVariant PropertyColorItem::decoration(const App::Property* prop) const
{
    App::Color value = ((App::PropertyColor*)prop)->getValue();
    QColor color((int)(255.0f*value.r),(int)(255.0f*value.g),(int)(255.0f*value.b));

    int size = QApplication::style()->pixelMetric(QStyle::PM_ListViewIconSize);
    QPixmap p(size, size);
    p.fill(color);

    return QVariant(p);
}

QVariant PropertyColorItem::toString(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()));

    App::Color value = ((App::PropertyColor*)prop)->getValue();
    QString color = QString::fromAscii("[%1, %2, %3]")
        .arg((int)(255.0f*value.r)).arg((int)(255.0f*value.g)).arg((int)(255.0f*value.b));
    return QVariant(color);
}

QVariant PropertyColorItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()));

    App::Color value = ((App::PropertyColor*)prop)->getValue();
    return QVariant(QColor((int)(255.0f*value.r),(int)(255.0f*value.g),(int)(255.0f*value.b)));
}

void PropertyColorItem::setValue(const QVariant& value)
{
    QColor col = value.value<QColor>();
    App::Color val;
    val.r = (float)col.red()/255.0f;
    val.g = (float)col.green()/255.0f;
    val.b = (float)col.blue()/255.0f;
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()));
        QString cmd = pythonIdentifier(*it);
        if (!cmd.isEmpty()) {
            cmd += QString::fromAscii(" = (%1,%2,%3)").arg(val.r,0,'f',2).arg(val.g,0,'f',2).arg(val.b,0,'f',2);
            Gui::Application::Instance->runPythonCode((const char*)cmd.toAscii());
        }
        else {
            static_cast<App::PropertyColor*>(*it)->setValue(val);
        }
    }
}

QWidget* PropertyColorItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    Gui::ColorButton* cb = new Gui::ColorButton( parent );
    QObject::connect(cb, SIGNAL(changed()), receiver, method);
    return cb;
}

void PropertyColorItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    Gui::ColorButton *cb = qobject_cast<Gui::ColorButton*>(editor);
    QColor color = data.value<QColor>();
    cb->setColor(color);
}

QVariant PropertyColorItem::editorData(QWidget *editor) const
{
    Gui::ColorButton *cb = qobject_cast<Gui::ColorButton*>(editor);
    QVariant var;
    var.setValue(cb->color());
    return var;
}
