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
# include <qdatetimeedit.h>
#endif

#include "propertyeditordate.h"

using namespace Gui::PropertyEditor;

TYPESYSTEM_SOURCE(Gui::PropertyEditor::TimeEditorItem, Gui::PropertyEditor::EditableItem);

TimeEditorItem::TimeEditorItem()
{
}

TimeEditorItem::TimeEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, QString("%1").arg(value.toTime().toString()) );
}

QWidget* TimeEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QTimeEdit* editor = new QTimeEdit( parent, "TimeEditorItem::spin" );
  editor->setTime( overrideValue().toTime() );
  connect(editor, SIGNAL( valueChanged(const QTime&) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void TimeEditorItem::stopEdit( int column )
{
  setText( column, QString("%1").arg(overrideValue().toTime().toString()) );
}

void TimeEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QTimeEdit* edit = dynamic_cast<QTimeEdit*>(editor);
  edit->setTime( value().toTime() );
}

QVariant TimeEditorItem::currentEditorValue( QWidget* editor ) const
{
  QVariant var;
  var.asTime() = dynamic_cast<QTimeEdit*>(editor)->time();
  return var;
}

QVariant TimeEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  QVariant var;
  return var;
}

void TimeEditorItem::convertToProperty(const QVariant&)
{
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::DateEditorItem, Gui::PropertyEditor::EditableItem);

DateEditorItem::DateEditorItem()
{
}

DateEditorItem::DateEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, QString("%1").arg(value.toDate().toString()) );
}

QWidget* DateEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QDateEdit* editor = new QDateEdit( parent, "TimeEditorItem::spin" );
  editor->setDate( overrideValue().toDate() );
  connect(editor, SIGNAL( valueChanged(const QDate&) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void DateEditorItem::stopEdit( int column )
{
  setText( column, QString("%1").arg(overrideValue().toDate().toString()) );
}

void DateEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QDateEdit* edit = dynamic_cast<QDateEdit*>(editor);
  edit->setDate( value().toDate() );
}

QVariant DateEditorItem::currentEditorValue( QWidget* editor ) const
{
  QVariant var;
  var.asDate() = dynamic_cast<QDateEdit*>(editor)->date();
  return var;
}

QVariant DateEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  QVariant var;
  return var;
}

void DateEditorItem::convertToProperty(const QVariant&)
{
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::DateTimeEditorItem, Gui::PropertyEditor::EditableItem);

DateTimeEditorItem::DateTimeEditorItem()
{
}

DateTimeEditorItem::DateTimeEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, QString("%1").arg(value.toDateTime().toString()) );
}

QWidget* DateTimeEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QDateTimeEdit* editor = new QDateTimeEdit( parent, "TimeEditorItem::spin" );
  editor->setDateTime( overrideValue().toDateTime() );
  connect(editor, SIGNAL( valueChanged(const QDateTime&) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void DateTimeEditorItem::stopEdit( int column )
{
  setText( column, QString("%1").arg(overrideValue().toDateTime().toString()) );
}

void DateTimeEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QDateTimeEdit* edit = dynamic_cast<QDateTimeEdit*>(editor);
  edit->setDateTime( value().toDateTime() );
}

QVariant DateTimeEditorItem::currentEditorValue( QWidget* editor ) const
{
  QVariant var;
  var.asDateTime() = dynamic_cast<QDateTimeEdit*>(editor)->dateTime();
  return var;
}

QVariant DateTimeEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  QVariant var;
  return var;
}

void DateTimeEditorItem::convertToProperty(const QVariant&)
{
}
