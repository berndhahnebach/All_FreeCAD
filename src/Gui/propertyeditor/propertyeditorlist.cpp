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
# include <algorithm>
# include <qstringlist.h>
# include <qcursor.h>
# include <qcombobox.h>
#endif

#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>
#include <Gui/Application.h>

#include "propertyeditorlist.h"
#include "propertyeditorinput.h"

using namespace Gui::PropertyEditor;


TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyBoolItem, Gui::PropertyEditor::PropertyItem);

PropertyBoolItem::PropertyBoolItem()
{
}

QVariant PropertyBoolItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyBool::getClassTypeId()));
    
    bool value = ((App::PropertyBool*)prop)->getValue();
    return QVariant(value);
}

void PropertyBoolItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::Bool))
        return;
    bool val = value.toBool();
    QString data = QString::fromAscii(" = %1")
                    .arg(val ? QLatin1String("True") : QLatin1String("False"));
    setPropertyValue(data);
}

QWidget* PropertyBoolItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QComboBox *cb = new QComboBox(parent);
    cb->setFrame(false);
    cb->addItem(QLatin1String("false"));
    cb->addItem(QLatin1String("true"));
    QObject::connect(cb, SIGNAL(activated(int)), receiver, method);
    return cb;
}

void PropertyBoolItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    cb->setCurrentIndex(cb->findText(data.toString()));
}

QVariant PropertyBoolItem::editorData(QWidget *editor) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    return QVariant(cb->currentText());
}

// ---------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyVectorItem, Gui::PropertyEditor::PropertyItem);

PropertyVectorItem::PropertyVectorItem()
{
    m_x = static_cast<PropertyFloatItem*>(PropertyFloatItem::create());
    m_x->setParent(this);
    m_x->setPropertyName(QLatin1String("x"));
    this->appendChild(m_x);
    m_y = static_cast<PropertyFloatItem*>(PropertyFloatItem::create());
    m_y->setParent(this);
    m_y->setPropertyName(QLatin1String("y"));
    this->appendChild(m_y);
    m_z = static_cast<PropertyFloatItem*>(PropertyFloatItem::create());
    m_z->setParent(this);
    m_z->setPropertyName(QLatin1String("z"));
    this->appendChild(m_z);
}

QVariant PropertyVectorItem::toString(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyVector::getClassTypeId()));

    const Base::Vector3f& value = static_cast<const App::PropertyVector*>(prop)->getValue();
    QString data = QString::fromAscii("[%1, %2, %3]")
        .arg(value.x,0,'f',2)
        .arg(value.y,0,'f',2)
        .arg(value.z,0,'f',2);
    return QVariant(data);
}

QVariant PropertyVectorItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyVector::getClassTypeId()));

    const Base::Vector3f& value = static_cast<const App::PropertyVector*>(prop)->getValue();
    return QVariant::fromValue<Base::Vector3f>(value);
}

void PropertyVectorItem::setValue(const QVariant& value)
{
    if (!value.canConvert<Base::Vector3f>())
        return;
    const Base::Vector3f& val = value.value<Base::Vector3f>();
    QString data = QString::fromAscii("(%1, %2, %3)")
                    .arg(val.x,0,'f',2)
                    .arg(val.y,0,'f',2)
                    .arg(val.z,0,'f',2);
    setPropertyValue(data);
}

QWidget* PropertyVectorItem::createEditor(QWidget* parent, const QObject* /*receiver*/, const char* /*method*/) const
{
    QLineEdit *le = new QLineEdit(parent);
    le->setFrame(false);
    le->setReadOnly(true);
    return le;
}

void PropertyVectorItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QLineEdit* le = qobject_cast<QLineEdit*>(editor);
    const Base::Vector3f& value = data.value<Base::Vector3f>();
    QString text = QString::fromAscii("[%1, %2, %3]")
        .arg(value.x,0,'f',2)
        .arg(value.y,0,'f',2)
        .arg(value.z,0,'f',2);
    le->setText(text);
}

QVariant PropertyVectorItem::editorData(QWidget *editor) const
{
    QLineEdit *le = qobject_cast<QLineEdit*>(editor);
    return QVariant(le->text());
}

float PropertyVectorItem::x() const
{
    return data(1,Qt::EditRole).value<Base::Vector3f>().x;
}

void PropertyVectorItem::setX(float x)
{
    setValue(QVariant::fromValue(Base::Vector3f(x, y(), z())));
}

float PropertyVectorItem::y() const
{
    return data(1,Qt::EditRole).value<Base::Vector3f>().y;
}

void PropertyVectorItem::setY(float y)
{
    setValue(QVariant::fromValue(Base::Vector3f(x(), y, z())));
}

float PropertyVectorItem::z() const
{
    return data(1,Qt::EditRole).value<Base::Vector3f>().z;
}

void PropertyVectorItem::setZ(float z)
{
    setValue(QVariant::fromValue(Base::Vector3f(x(), y(), z)));
}

// ---------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyEnumItem, Gui::PropertyEditor::PropertyItem);

PropertyEnumItem::PropertyEnumItem()
{
}

QVariant PropertyEnumItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyEnumeration::getClassTypeId()));

    const std::vector<std::string>& value = ((App::PropertyEnumeration*)prop)->getEnumVector();
    long currentItem = ((App::PropertyEnumeration*)prop)->getValue();
    return QVariant(QString::fromUtf8(value[currentItem].c_str()));
}

void PropertyEnumItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::StringList))
        return;
    QStringList items = value.toStringList();
    if (!items.isEmpty()) {
        QString val = items.front();
        QString data = QString::fromAscii("\"%1\"").arg(val);
        setPropertyValue(data);
    }
}

QWidget* PropertyEnumItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QComboBox *cb = new QComboBox(parent);
    cb->setFrame(false);
    QObject::connect(cb, SIGNAL(activated(int)), receiver, method);
    return cb;
}

void PropertyEnumItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    const std::vector<App::Property*>& items = getPropertyData();

    QStringList commonModes, modes;
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        if ((*it)->getTypeId() == App::PropertyEnumeration::getClassTypeId()) {
            App::PropertyEnumeration* prop = static_cast<App::PropertyEnumeration*>(*it);
            const std::vector<std::string>& value = prop->getEnumVector();
            if (it == items.begin()) {
                for (std::vector<std::string>::const_iterator jt = value.begin(); jt != value.end(); ++jt)
                    commonModes << QLatin1String(jt->c_str());
            }
            else {
                for (std::vector<std::string>::const_iterator jt = value.begin(); jt != value.end(); ++jt) {
                    if (commonModes.contains(QLatin1String(jt->c_str())))
                        modes << QLatin1String(jt->c_str());
                }

                commonModes = modes;
                modes.clear();
            }
        }
    }

    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    cb->addItems(commonModes);
    cb->setCurrentIndex(cb->findText(data.toString()));
}

QVariant PropertyEnumItem::editorData(QWidget *editor) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    return QVariant(cb->currentText());
}

// ---------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyStringListItem, Gui::PropertyEditor::PropertyItem);

PropertyStringListItem::PropertyStringListItem()
{
}

QWidget* PropertyStringListItem::createEditor(QWidget* parent, const QObject* /*receiver*/, const char* /*method*/) const
{
    QComboBox *cb = new QComboBox(parent);
    cb->setFrame(false);
    return cb;
}

void PropertyStringListItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    cb->setEditable(true);
    cb->insertItems(0, data.toStringList());
    cb->setCurrentIndex(0);
}

QVariant PropertyStringListItem::editorData(QWidget *editor) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    QStringList list;
    for (int i=0; i<cb->count(); i++)
        list << cb->itemText(i);
    return QVariant(list);
}

QVariant PropertyStringListItem::toString(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyStringList::getClassTypeId()));

    QStringList list;
    const std::vector<std::string>& value = ((App::PropertyStringList*)prop)->getValues();
    for ( std::vector<std::string>::const_iterator jt = value.begin(); jt != value.end(); ++jt ) {
        list << QString::fromAscii(jt->c_str());
    }

    QString text = QString::fromAscii("[%1]").arg(list.join(QLatin1String(",")));

    return QVariant(text);
}

QVariant PropertyStringListItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyStringList::getClassTypeId()));

    QStringList list;
    const std::vector<std::string>& value = ((App::PropertyStringList*)prop)->getValues();
    for ( std::vector<std::string>::const_iterator jt = value.begin(); jt != value.end(); ++jt ) {
        list << QString::fromAscii(jt->c_str());
    }

    return QVariant(list);
}

void PropertyStringListItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::StringList))
        return;
    QStringList values = value.toStringList();
    QString data = QString::fromAscii("[");
    for (QStringList::Iterator it = values.begin(); it != values.end(); ++it) {
        data += QString::fromAscii("\"%1\",").arg(*it);
    }
    data += QString::fromAscii("]");
    setPropertyValue(data);
}

#include "moc_propertyeditorlist.cpp"
