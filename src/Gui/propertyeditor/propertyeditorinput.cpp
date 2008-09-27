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
# include <cmath>
# include <climits>
# include <qlineedit.h>
# include <qvalidator.h>
# include <qinputdialog.h>
# include <qcursor.h>
#endif

#include <App/PropertyStandard.h>
#include <Gui/SpinBox.h>

#include "propertyeditorinput.h"

using namespace Gui::PropertyEditor;


TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyStringItem, Gui::PropertyEditor::PropertyItem);

PropertyStringItem::PropertyStringItem()
{
}

QVariant PropertyStringItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyString::getClassTypeId()));

    std::string value = ((App::PropertyString*)prop)->getValue();
    return QVariant(QString::fromUtf8(value.c_str()));
}

void PropertyStringItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::String))
        return;
    QString val = value.toString();
    QString data = QString::fromAscii("\"%1\"").arg(val);
    setPropertyValue(data);
}

QWidget* PropertyStringItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QLineEdit *le = new QLineEdit(parent);
    le->setFrame(false);
    QObject::connect(le, SIGNAL(textChanged(const QString&)), receiver, method);
    return le;
}

void PropertyStringItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QLineEdit *le = qobject_cast<QLineEdit*>(editor);
    le->setText(data.toString());
}

QVariant PropertyStringItem::editorData(QWidget *editor) const
{
    QLineEdit *le = qobject_cast<QLineEdit*>(editor);
    return QVariant(le->text());
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertySeparatorItem, Gui::PropertyEditor::PropertyItem);

QWidget* PropertySeparatorItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    return 0;
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyIntegerItem, Gui::PropertyEditor::PropertyItem);

PropertyIntegerItem::PropertyIntegerItem()
{
}

QVariant PropertyIntegerItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyInteger::getClassTypeId()));

    int value = (int)((App::PropertyInteger*)prop)->getValue();
    return QVariant(value);
}

void PropertyIntegerItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::Int))
        return;
    int val = value.toInt();
    QString data = QString::fromAscii("%1").arg(val);
    setPropertyValue(data);
}

QWidget* PropertyIntegerItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QSpinBox *sb = new QSpinBox(parent);
    sb->setFrame(false);
    QObject::connect(sb, SIGNAL(valueChanged(int)), receiver, method);
    return sb;
}

void PropertyIntegerItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    sb->setRange(INT_MIN, INT_MAX);
    sb->setValue(data.toInt());
}

QVariant PropertyIntegerItem::editorData(QWidget *editor) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    return QVariant(sb->value());
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyIntegerConstraintItem, Gui::PropertyEditor::PropertyItem);

PropertyIntegerConstraintItem::PropertyIntegerConstraintItem()
{
}

QVariant PropertyIntegerConstraintItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyIntegerConstraint::getClassTypeId()));

    int value = (int)((App::PropertyIntegerConstraint*)prop)->getValue();
    return QVariant(value);
}

void PropertyIntegerConstraintItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::Int))
        return;
    int val = value.toInt();
    QString data = QString::fromAscii("%1").arg(val);
    setPropertyValue(data);
}

QWidget* PropertyIntegerConstraintItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QSpinBox *sb = new QSpinBox(parent);
    sb->setFrame(false);
    QObject::connect(sb, SIGNAL(valueChanged(int)), receiver, method);
    return sb;
}

void PropertyIntegerConstraintItem::setEditorData(QWidget *editor, const QVariant& /*data*/) const
{
    const std::vector<App::Property*>& items = getPropertyData();
    App::PropertyIntegerConstraint* prop = (App::PropertyIntegerConstraint*)items[0];

    const App::PropertyIntegerConstraint::Constraints* c = 
        ((App::PropertyIntegerConstraint*)prop)->getConstraints();
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    if (c) {
        sb->setMinimum(c->LowerBound);
        sb->setMaximum(c->UpperBound);
        sb->setSingleStep(c->StepSize);
    }
    else {
        sb->setMinimum(INT_MIN);
        sb->setMaximum(INT_MAX);
    }
    sb->setValue(prop->getValue());
}

QVariant PropertyIntegerConstraintItem::editorData(QWidget *editor) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    return QVariant(sb->value());
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyFloatItem, Gui::PropertyEditor::PropertyItem);

PropertyFloatItem::PropertyFloatItem()
{
}

QVariant PropertyFloatItem::toString(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()));

    double value = ((App::PropertyFloat*)prop)->getValue();
    QString data = QString::fromAscii("%1").arg(value,0,'f',2);
    if (prop->getTypeId().isDerivedFrom(App::PropertyAngle::getClassTypeId()))
        data += QString::fromUtf8(" \xc2\xb0");
    else if (prop->getTypeId().isDerivedFrom(App::PropertyDistance::getClassTypeId()))
        data += QLatin1String(" mm");
    return QVariant(data);
}

QVariant PropertyFloatItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()));

    double value = static_cast<const App::PropertyFloat*>(prop)->getValue();
    return QVariant(value);
}

void PropertyFloatItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::Double))
        return;
    double val = value.toDouble();
    QString data = QString::fromAscii("%1").arg(val,0,'f',2);
    setPropertyValue(data);
}

QWidget* PropertyFloatItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
    sb->setFrame(false);
    QObject::connect(sb, SIGNAL(valueChanged(double)), receiver, method);
    return sb;
}

void PropertyFloatItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(editor);
    sb->setRange((double)INT_MIN, (double)INT_MAX);
    sb->setValue(data.toDouble());
    const std::vector<App::Property*>& prop = getPropertyData();
    if (prop.empty())
        return;
    else if (prop.front()->getTypeId().isDerivedFrom(App::PropertyAngle::getClassTypeId()))
        sb->setSuffix(QString::fromUtf8(" \xc2\xb0"));
    else if (prop.front()->getTypeId().isDerivedFrom(App::PropertyDistance::getClassTypeId()))
        sb->setSuffix(QLatin1String(" mm"));
}

QVariant PropertyFloatItem::editorData(QWidget *editor) const
{
    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(editor);
    return QVariant(sb->value());
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyFloatConstraintItem, Gui::PropertyEditor::PropertyItem);

PropertyFloatConstraintItem::PropertyFloatConstraintItem()
{
}

QVariant PropertyFloatConstraintItem::toString(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()));

    double value = ((App::PropertyFloat*)prop)->getValue();
    QString data = QString::fromAscii("%1").arg(value,0,'f',2);
    return QVariant(data);
}

QVariant PropertyFloatConstraintItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloatConstraint::getClassTypeId()));

    double value = ((App::PropertyFloatConstraint*)prop)->getValue();
    return QVariant(value);
}

void PropertyFloatConstraintItem::setValue(const QVariant& value)
{
    if (!value.canConvert(QVariant::Double))
        return;
    double val = value.toDouble();
    QString data = QString::fromAscii("%1").arg(val,0,'f',2);
    setPropertyValue(data);
}

QWidget* PropertyFloatConstraintItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
    sb->setFrame(false);
    QObject::connect(sb, SIGNAL(valueChanged(double)), receiver, method);
    return sb;
}

void PropertyFloatConstraintItem::setEditorData(QWidget *editor, const QVariant& /*data*/) const
{
    const std::vector<App::Property*>& items = getPropertyData();
    App::PropertyFloatConstraint* prop = (App::PropertyFloatConstraint*)items[0];

    const App::PropertyFloatConstraint::Constraints* c = ((App::PropertyFloatConstraint*)prop)->getConstraints();
    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(editor);
    if (c) {
        sb->setMinimum(c->LowerBound);
        sb->setMaximum(c->UpperBound);
        sb->setSingleStep(c->StepSize);
    }
    else {
        sb->setMinimum((double)INT_MIN);
        sb->setMaximum((double)INT_MAX);
        sb->setSingleStep(0.1);
    }
    sb->setValue(prop->getValue());
}

QVariant PropertyFloatConstraintItem::editorData(QWidget *editor) const
{
    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(editor);
    return QVariant(sb->value());
}
