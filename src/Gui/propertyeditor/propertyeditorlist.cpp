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
    bool val = value.toBool();
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyBool::getClassTypeId()));
        QString cmd = pythonIdentifier(*it);
        if (!cmd.isEmpty()) {
            cmd += QString::fromAscii(" = %1").arg(val ? QLatin1String("True") : QLatin1String("False"));
            Gui::Application::Instance->runPythonCode((const char*)cmd.toAscii());
        }
        else {
            static_cast<App::PropertyBool*>(*it)->setValue(val);
        }
    }
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
    QVariant v;
    v.setValue<Base::Vector3f>(value);
    return v;
}

void PropertyVectorItem::setValue(const QVariant& value)
{
    if (!value.canConvert<Base::Vector3f>())
        return;
    const Base::Vector3f& val = value.value<Base::Vector3f>();
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyVector::getClassTypeId()));
        QString cmd = pythonIdentifier(*it);
        if (!cmd.isEmpty()) {
            cmd += QString::fromAscii(" = (%1, %2, %3)")
                .arg(val.x,0,'f',2)
                .arg(val.y,0,'f',2)
                .arg(val.z,0,'f',2);
            Gui::Application::Instance->runPythonCode((const char*)cmd.toAscii());
        }
        else {
            static_cast<App::PropertyVector*>(*it)->setValue(val);
        }
    }
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
    QStringList items = value.toStringList();
    if (!items.isEmpty()) {
        QString value = items.front();
        const std::vector<App::Property*>& items = getProperty();
        for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
            assert((*it)->getTypeId().isDerivedFrom(App::PropertyEnumeration::getClassTypeId()));
            QString cmd = pythonIdentifier(*it);
            if (!cmd.isEmpty()) {
                cmd += QString::fromAscii(" = \"%1\"").arg(value);
                Gui::Application::Instance->runPythonCode((const char*)cmd.toAscii());
            }
            else {
                static_cast<App::PropertyEnumeration*>(*it)->setValue((const char*)value.toUtf8());
            }
        }
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
    const std::vector<App::Property*>& items = getProperty();

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

QWidget* PropertyStringListItem::createEditor(QWidget* parent, QObject* /*receiver*/, const char* /*method*/) const
{
    QComboBox *cb = new QComboBox(parent);
    cb->setFrame(false);
    return cb;
}

void PropertyStringListItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    cb->insertItems(0, data.toStringList());
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
    QStringList values = value.toStringList();
    std::vector<std::string> list;
    for (QStringList::Iterator it = values.begin(); it != values.end(); ++it) {
        list.push_back((const char*)(*it).toAscii());
    }
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyStringList::getClassTypeId()));
        ((App::PropertyStringList*)*it)->setValues(list);
    }
}
