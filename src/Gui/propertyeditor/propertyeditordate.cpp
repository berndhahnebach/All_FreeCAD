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

void TimeEditorItem::stopEdit( QWidget* editor, int column )
{
  setOverrideValue( dynamic_cast<QTimeEdit*>(editor)->time() );
  setText( column, QString("%1").arg(overrideValue().toTime().toString()) );
}

void TimeEditorItem::setDefaultValue()
{
  QTimeEdit* edit = dynamic_cast<QTimeEdit*>(_editor);
  edit->setTime( value().toTime() );
}

// ======================================================================

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

void DateEditorItem::stopEdit( QWidget* editor, int column )
{
  setOverrideValue( dynamic_cast<QDateEdit*>(editor)->date() );
  setText( column, QString("%1").arg(overrideValue().toDate().toString()) );
}

void DateEditorItem::setDefaultValue()
{
  QDateEdit* edit = dynamic_cast<QDateEdit*>(_editor);
  edit->setDate( value().toDate() );
}

// ======================================================================

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

void DateTimeEditorItem::stopEdit( QWidget* editor, int column )
{
  QVariant var = overrideValue();
  var.asDateTime() = dynamic_cast<QDateTimeEdit*>(editor)->dateTime();
  setOverrideValue( var );
  setText( column, QString("%1").arg(var.toDateTime().toString()) );
}

void DateTimeEditorItem::setDefaultValue()
{
  QDateTimeEdit* edit = dynamic_cast<QDateTimeEdit*>(_editor);
  edit->setDateTime( value().toDateTime() );
}

