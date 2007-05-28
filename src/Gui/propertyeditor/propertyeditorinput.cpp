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
# include <math.h>
# include <qlineedit.h>
# include <qvalidator.h>
# include <qinputdialog.h>
# include <q3popupmenu.h>
# include <qcursor.h>
#endif

#include <App/PropertyStandard.h>

#include "propertyeditorinput.h"
#include "../SpinBox.h"

using namespace Gui::PropertyEditor;


TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyStringItem, Gui::PropertyEditor::PropertyItem);

PropertyStringItem::PropertyStringItem()
{
}

QVariant PropertyStringItem::propertyData(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyString::getClassTypeId()));

    std::string value = ((App::PropertyString*)prop)->getValue();
    return QVariant(QString(value.c_str()));
}

void PropertyStringItem::setPropertyData(const QVariant& value)
{
    QString val = value.toString();
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyString::getClassTypeId()));
        ((App::PropertyString*)*it)->setValue(val.toAscii());
    }
}

QWidget* PropertyStringItem::createEditor(QWidget* parent) const
{
    QLineEdit *le = new QLineEdit(parent);
    le->setFrame(false);
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

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyIntegerItem, Gui::PropertyEditor::PropertyItem);

PropertyIntegerItem::PropertyIntegerItem()
{
}

QVariant PropertyIntegerItem::propertyData(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyInteger::getClassTypeId()));

    long value = ((App::PropertyInteger*)prop)->getValue();
    return QVariant(value);
}

void PropertyIntegerItem::setPropertyData(const QVariant& value)
{
    int val = value.toInt();
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyInteger::getClassTypeId()));
        ((App::PropertyInteger*)*it)->setValue(val);
    }
}

QWidget* PropertyIntegerItem::createEditor(QWidget* parent) const
{
    QSpinBox *sb = new QSpinBox(parent);
    sb->setFrame(false);
    return sb;
}

void PropertyIntegerItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
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

QVariant PropertyIntegerConstraintItem::propertyData(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyIntegerConstraint::getClassTypeId()));

    long value = ((App::PropertyIntegerConstraint*)prop)->getValue();
    return QVariant(value);
}

void PropertyIntegerConstraintItem::setPropertyData(const QVariant& value)
{
    int val = value.toInt();
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyIntegerConstraint::getClassTypeId()));
        ((App::PropertyIntegerConstraint*)*it)->setValue(val);
    }
}

QWidget* PropertyIntegerConstraintItem::createEditor(QWidget* parent) const
{
    QSpinBox *sb = new QSpinBox(parent);
    sb->setFrame(false);
    return sb;
}

void PropertyIntegerConstraintItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    const std::vector<App::Property*>& items = getProperty();
    App::PropertyIntegerConstraint* prop = (App::PropertyIntegerConstraint*)items[0];

    const App::PropertyIntegerConstraint::Constraints* c = ((App::PropertyIntegerConstraint*)prop)->getConstraints();
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    sb->setValue(prop->getValue());
    if (c) {
        sb->setMinimum(c->LowerBound);
        sb->setMaximum(c->UpperBound);
        sb->setSingleStep(c->StepSize);
    }
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

QVariant PropertyFloatItem::displayData(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()));

    double value = ((App::PropertyFloat*)prop)->getValue();
    QString data;
    data.sprintf("%.2f", value);
    return QVariant(data);
}

QVariant PropertyFloatItem::propertyData(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()));

    double value = ((App::PropertyFloat*)prop)->getValue();
    return QVariant(value);
}

void PropertyFloatItem::setPropertyData(const QVariant& value)
{
    double val = value.toDouble();
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()));
        ((App::PropertyFloat*)*it)->setValue(val);
    }
}

QWidget* PropertyFloatItem::createEditor(QWidget* parent) const
{
    QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
    sb->setFrame(false);
    return sb;
}

void PropertyFloatItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(editor);
    sb->setValue(data.toDouble());
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

QVariant PropertyFloatConstraintItem::displayData(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId()));

    double value = ((App::PropertyFloat*)prop)->getValue();
    QString data;
    data.sprintf("%.2f", value);
    return QVariant(data);
}

QVariant PropertyFloatConstraintItem::propertyData(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyFloatConstraint::getClassTypeId()));

    double value = ((App::PropertyFloatConstraint*)prop)->getValue();
    return QVariant(value);
}

void PropertyFloatConstraintItem::setPropertyData(const QVariant& value)
{
    double val = value.toDouble();
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyFloatConstraint::getClassTypeId()));
        ((App::PropertyFloatConstraint*)*it)->setValue(val);
    }
}

QWidget* PropertyFloatConstraintItem::createEditor(QWidget* parent) const
{
    QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
    sb->setFrame(false);
    return sb;
}

void PropertyFloatConstraintItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    const std::vector<App::Property*>& items = getProperty();
    App::PropertyFloatConstraint* prop = (App::PropertyFloatConstraint*)items[0];

    const App::PropertyFloatConstraint::Constraints* c = ((App::PropertyFloatConstraint*)prop)->getConstraints();
    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(editor);
    sb->setValue(prop->getValue());
    if (c) {
        sb->setMinimum(c->LowerBound);
        sb->setMaximum(c->UpperBound);
        sb->setSingleStep(c->StepSize);
    }
}

QVariant PropertyFloatConstraintItem::editorData(QWidget *editor) const
{
    QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(editor);
    return QVariant(sb->value());
}











TYPESYSTEM_SOURCE(Gui::PropertyEditor::TextEditorItem, Gui::PropertyEditor::EditableItem);

TextEditorItem::TextEditorItem()
{
}

TextEditorItem::TextEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, value.toString() );
}

QWidget* TextEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QLineEdit* editor = new QLineEdit( parent, "TextEditorItem::edit" );
  editor->setText( overrideValue().toString() );
  editor->setFocus();
  connect(editor, SIGNAL( textChanged(const QString&) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void TextEditorItem::stopEdit( int column )
{
  setText( column, overrideValue().toString() );
}

void TextEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QLineEdit* edit = dynamic_cast<QLineEdit*>(editor);
  edit->setText( value().toString() );
}

QVariant TextEditorItem::currentEditorValue( QWidget* editor ) const
{
  QVariant var;
  var.asString() = dynamic_cast<QLineEdit*>(editor)->text();
  return var;
}

QVariant TextEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  QString txt = "";
  bool equal = true;
  for ( std::vector<App::Property*>::const_iterator it = prop.begin(); it != prop.end(); ++it )
  {
    App::PropertyString* pPropChar = (App::PropertyString*)*it;
    if ( it == prop.begin() )
      txt = pPropChar->getValue();
    else 
      equal &= ( txt == pPropChar->getValue() );
  }

  // set empty text when the properties are different
  if ( !equal )
    txt = "";

  QVariant val( txt );
  setText( 1, txt );

  return val;
}

void TextEditorItem::convertToProperty(const QVariant& val)
{
  QString value = val.toString();
  for (std::vector<App::Property*>::iterator it = _prop.begin(); it != _prop.end(); ++it)
  {
    App::PropertyString* pPropChar = (App::PropertyString*)*it;
    pPropChar->setValue( value.ascii() );
  }
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::IntEditorItem, Gui::PropertyEditor::EditableItem);

IntEditorItem::IntEditorItem()
{
}

IntEditorItem::IntEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
  : EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, QString("%1").arg(value.toInt()) );
}

QWidget* IntEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  SpinBox* editor = new SpinBox( parent, "IntEditorItem::spin" );
  editor->setRange(INT_MIN, INT_MAX);
  editor->setValue( overrideValue().toInt() );
  connect(editor, SIGNAL( valueChanged(int) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void IntEditorItem::stopEdit( int column )
{
  QSpinBox* editor = (QSpinBox*)getEditor();
  QString txt = QString("%1").arg( overrideValue().toInt() );
  txt += editor->suffix();
  setText( column, txt );
}

void IntEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QSpinBox* spin = dynamic_cast<QSpinBox*>(editor);
  spin->setValue( value().toInt() );
}

QVariant IntEditorItem::currentEditorValue( QWidget* editor ) const
{
  QVariant var;
  var.asInt() = dynamic_cast<QSpinBox*>(editor)->value();
  return var;
}

QVariant IntEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  App::PropertyInteger* pPropInt = (App::PropertyInteger*)prop.front();
  QVariant value( (int)pPropInt->getValue() );
  QString txt = value.toString();
  if ( pPropInt->getTypeId().isDerivedFrom(App::PropertyPercent::getClassTypeId()) )
    txt += " %";
  setText( 1, txt );
  return value;
}

void IntEditorItem::convertToProperty(const QVariant& val)
{
  int value = val.toInt();
  for (std::vector<App::Property*>::iterator it = _prop.begin(); it != _prop.end(); ++it)
  {
    App::PropertyInteger* pPropInt = (App::PropertyInteger*)*it;
    pPropInt->setValue( value );
  }
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::IntConstraintEditorItem, Gui::PropertyEditor::IntEditorItem);

IntConstraintEditorItem::IntConstraintEditorItem()
{
}

QWidget* IntConstraintEditorItem::createEditor( int column, QWidget* parent )
{
  QSpinBox* editor = (QSpinBox*)IntEditorItem::createEditor( column, parent );
  if ( editor == 0 )
    return 0;

  App::PropertyIntegerConstraint* prop = (App::PropertyIntegerConstraint*)_prop.front();
  const App::PropertyIntegerConstraint::Constraints*  range = prop->getConstraints();
  if ( range ) {
    editor->setMaxValue( (int)range->UpperBound );
    editor->setMinValue( (int)range->LowerBound );
    editor->setLineStep( (int)range->StepSize );
  }
  if ( prop->getTypeId().isDerivedFrom(App::PropertyPercent::getClassTypeId()) )
    editor->setSuffix(" %");
  return editor;
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::FloatEditorItem, Gui::PropertyEditor::EditableItem);

FloatEditorItem::FloatEditorItem()
{
}

FloatEditorItem::FloatEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
  : EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, QString("%1").arg(value.toDouble()) );
}

QWidget* FloatEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  FloatSpinBox* editor = new FloatSpinBox( parent, "FloatEditorItem::spin" );
  editor->QSpinBox::setRange(INT_MIN/10, INT_MAX/10);
  editor->setRange(editor->minValue(), editor->maxValue(), 0.01, 2);
  editor->setValue( (float)overrideValue().toDouble() );
  connect(editor, SIGNAL( valueChanged(int) ), this, SLOT( onValueChanged() ) );

  App::PropertyFloat* prop = (App::PropertyFloat*)_prop.front();
  if ( prop->getTypeId().isDerivedFrom(App::PropertyDistance::getClassTypeId()) )
    editor->setSuffix(" mm");
  else if ( prop->getTypeId().isDerivedFrom(App::PropertyAngle::getClassTypeId()) )
    editor->setSuffix(" °");
  
  return editor;
}

void FloatEditorItem::stopEdit( int column )
{
  FloatSpinBox* editor = (FloatSpinBox*)getEditor();
  QString txt = QString("%1").arg( overrideValue().toDouble() );
  txt += editor->suffix();
  setText( column, txt );
}

void FloatEditorItem::setDefaultEditorValue( QWidget* editor )
{
  FloatSpinBox* spin = dynamic_cast<FloatSpinBox*>(editor);
  spin->setValue( (float)value().toDouble() );
}

QVariant FloatEditorItem::currentEditorValue( QWidget* editor ) const
{
  QVariant var;
  var.asDouble() = dynamic_cast<FloatSpinBox*>(editor)->value();
  return var;
}

QVariant FloatEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  App::PropertyFloat* pPropFloat = (App::PropertyFloat*)prop.front();
  QVariant value( (double)pPropFloat->getValue() );
  QString txt;
  txt.sprintf("%.2f", value.toDouble());
  if ( pPropFloat->getTypeId().isDerivedFrom(App::PropertyDistance::getClassTypeId()) )
    txt += " mm";
  else if ( pPropFloat->getTypeId().isDerivedFrom(App::PropertyAngle::getClassTypeId()) )
    txt += " °";
  setText( 1, txt );
  return value;
}

void FloatEditorItem::convertToProperty(const QVariant& val)
{
  float value = (float)val.toDouble();
  for (std::vector<App::Property*>::iterator it = _prop.begin(); it != _prop.end(); ++it)
  {
    App::PropertyFloat* pPropFloat = (App::PropertyFloat*)*it;
    pPropFloat->setValue( value );
  }
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::FloatConstraintEditorItem, Gui::PropertyEditor::FloatEditorItem);

FloatConstraintEditorItem::FloatConstraintEditorItem()
{
}

QWidget* FloatConstraintEditorItem::createEditor( int column, QWidget* parent )
{
  FloatSpinBox* editor = (FloatSpinBox*)FloatEditorItem::createEditor( column, parent );
  if ( editor == 0 )
    return 0;

  App::PropertyFloatConstraint* prop = (App::PropertyFloatConstraint*)_prop.front();
  const App::PropertyFloatConstraint::Constraints*  range = prop->getConstraints();
  if ( range ) {
    editor->setMaxValue( range->UpperBound );
    editor->setMinValue( range->LowerBound );
    editor->setLineStep( range->StepSize );
  }

  return editor;
}

#include "moc_propertyeditorinput.cpp"

// --------------------------------------------------------------------
