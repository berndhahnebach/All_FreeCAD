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
# include <qpopupmenu.h>
# include <qcursor.h>
#endif

#include <App/PropertyStandard.h>

#include "propertyeditorinput.h"
#include "../SpinBox.h"

using namespace Gui::PropertyEditor;

TYPESYSTEM_SOURCE(Gui::PropertyEditor::TextEditorItem, Gui::PropertyEditor::EditableItem);

TextEditorItem::TextEditorItem()
{
}

TextEditorItem::TextEditorItem( QListView* lv, const QString& text, const QVariant& value )
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

void TextEditorItem::stopEdit( QWidget* editor, int column )
{
  setOverrideValue( dynamic_cast<QLineEdit*>(editor)->text() );
  setText( column, overrideValue().toString() );
}

void TextEditorItem::setDefaultValue()
{
  QLineEdit* edit = dynamic_cast<QLineEdit*>(_editor);
  edit->setText( value().toString() );
}

void TextEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
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
  setValue( val );
  setText( 1, txt );
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

IntEditorItem::IntEditorItem( QListView* lv, const QString& text, const QVariant& value )
  : EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, QString("%1").arg(value.toInt()) );
}

QWidget* IntEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QSpinBox* editor = new QSpinBox( parent, "IntEditorItem::spin" );
  editor->setValue( overrideValue().toInt() );
  connect(editor, SIGNAL( valueChanged(int) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void IntEditorItem::stopEdit( QWidget* editor, int column )
{
  setOverrideValue( dynamic_cast<QSpinBox*>(editor)->value() );
  setText( column, QString("%1").arg( overrideValue().toInt() ) );
}

void IntEditorItem::setDefaultValue()
{
  QSpinBox* spin = dynamic_cast<QSpinBox*>(_editor);
  spin->setValue( value().toInt() );
}

void IntEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  App::PropertyInteger* pPropInt = (App::PropertyInteger*)prop.front();
  QVariant value( (int)pPropInt->getValue() );
  setValue( value );
  setText( 1, value.toString() );
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

TYPESYSTEM_SOURCE(Gui::PropertyEditor::FloatEditorItem, Gui::PropertyEditor::EditableItem);

FloatEditorItem::FloatEditorItem()
{
}

FloatEditorItem::FloatEditorItem( QListView* lv, const QString& text, const QVariant& value )
  : EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, QString("%1").arg(value.toDouble()) );
}

QWidget* FloatEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  FloatSpinBox* editor = new FloatSpinBox( 0 /*min*/, 12 /*max*/, 0.05 /*step*/, 5.5 /*value*/, 3 /*digits*/,
                                             parent, "FloatEditorItem::spin" );
  editor->setValue( (float)overrideValue().toDouble() );
  connect(editor, SIGNAL( valueChanged(int) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void FloatEditorItem::stopEdit( QWidget* editor, int column )
{
  setOverrideValue( dynamic_cast<FloatSpinBox*>(editor)->value() );
  setText( column, QString("%1").arg( overrideValue().toDouble() ) );
}

void FloatEditorItem::setDefaultValue()
{
  FloatSpinBox* spin = dynamic_cast<FloatSpinBox*>(_editor);
  spin->setValue( (float)value().toDouble() );
}

void FloatEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  App::PropertyFloat* pPropFloat = (App::PropertyFloat*)prop.front();
  QVariant value( (double)pPropFloat->getValue() );
  setValue( value );
  QString txt;
  setText( 1, txt.sprintf("%.3f", value.toDouble()) );
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

#include "moc_propertyeditorinput.cpp"
