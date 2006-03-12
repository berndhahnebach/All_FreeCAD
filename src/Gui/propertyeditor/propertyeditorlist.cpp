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
# include <qstringlist.h>
# include <qcursor.h>
# include <qcombobox.h>
#endif

#include <App/PropertyStandard.h>

#include "propertyeditorlist.h"

using namespace Gui::PropertyEditor;


TYPESYSTEM_SOURCE(Gui::PropertyEditor::BoolEditorItem, Gui::PropertyEditor::EditableItem);

BoolEditorItem::BoolEditorItem()
{
}

BoolEditorItem::BoolEditorItem( QListView* lv, const QString& text, const QVariant& value )
  : EditableItem( lv, value )
{
  setText( 0, text);

  if ( value.toBool() )
    setText( 1, QObject::tr("True") );
  else
    setText( 1, QObject::tr("False") );
}

QWidget* BoolEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QComboBox* editor = new QComboBox( parent, "BoolEditorItem::combo" );
  editor->insertStringList( QStringList() << QObject::tr("True")
                              << QObject::tr("False") );
  connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );

  if ( overrideValue().toBool() )
    editor->setCurrentItem( 0 );
  else
    editor->setCurrentItem( 1 );
  return editor;
}

void BoolEditorItem::stopEdit( QWidget* editor, int column )
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);
  QVariant var;
  var.asBool() = combo->currentItem() == 0 ? true : false;
  setOverrideValue( var );
  setText( column, combo->currentText() );
}

void BoolEditorItem::setDefaultValue()
{
  QComboBox* combo = dynamic_cast<QComboBox*>(_editor);
  if ( value().toBool() )
    combo->setCurrentItem( 0 );
  else
    combo->setCurrentItem( 1 );
}

void BoolEditorItem::convertFromProperty(App::Property* prop)
{
  if ( prop && prop->getTypeId() == App::PropertyBool::getClassTypeId() )
  {
    App::PropertyBool* pPropBool = (App::PropertyBool*)prop;
    QVariant var( pPropBool->getValue(), 0 );
    setValue( var );
  }
}

void BoolEditorItem::convertToProperty(const QVariant&)
{
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::ListEditorItem, Gui::PropertyEditor::EditableItem);

ListEditorItem::ListEditorItem()
{
}

ListEditorItem::ListEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text);
  setText( 1, value.toStringList().last());
}

QWidget* ListEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QStringList items = overrideValue().toStringList();
  QComboBox* editor = new QComboBox( parent, "ListEditorItem::combo" );
  connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );

  QString txt = items.last();
  items.pop_back();
  editor->insertStringList( items );

  int cur = 0;
  for ( QStringList::Iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( txt == *it)
    {
      editor->setCurrentItem( cur );
      break;
    }

    cur++;
  }

  return editor;
}

void ListEditorItem::stopEdit( QWidget* editor, int column )
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);

  QVariant var = overrideValue();
  var.asStringList().last() = combo->currentText();
  setOverrideValue( var );
  setText( column, overrideValue().toStringList().last() );
}

void ListEditorItem::setDefaultValue()
{
  QComboBox* combo = dynamic_cast<QComboBox*>(_editor);

  QStringList items = value().toStringList();

  QString txt = items.last();
  items.pop_back();

  int cur = 0;
  for ( QStringList::Iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( txt == *it)
    {
      combo->setCurrentItem( cur );
      break;
    }

    cur++;
  }
}

void ListEditorItem::convertFromProperty(App::Property*)
{
}

void ListEditorItem::convertToProperty(const QVariant&)
{
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::CursorEditorItem, Gui::PropertyEditor::EditableItem);

CursorEditorItem::CursorEditorItem()
{
  _lst[ QObject::ArrowCursor        ] = QString("Arrow");
  _lst[ QObject::UpArrowCursor      ] = QString("Up Arrow");
  _lst[ QObject::CrossCursor        ] = QString("Cross");
  _lst[ QObject::WaitCursor         ] = QString("Waiting");
  _lst[ QObject::IbeamCursor        ] = QString("iBeam");
  _lst[ QObject::SizeVerCursor      ] = QString("Size Vertical");
  _lst[ QObject::SizeHorCursor      ] = QString("Size Horizontal");
  _lst[ QObject::SizeBDiagCursor    ] = QString("Size Slash");
  _lst[ QObject::SizeFDiagCursor    ] = QString("Size Backslash");
  _lst[ QObject::SizeAllCursor      ] = QString("Size All");
  _lst[ QObject::BlankCursor        ] = QString("Blank");
  _lst[ QObject::SplitVCursor       ] = QString("Split Vertical");
  _lst[ QObject::SplitHCursor       ] = QString("Split Horizontal");
  _lst[ QObject::PointingHandCursor ] = QString("Pointing Hand");
  _lst[ QObject::ForbiddenCursor    ] = QString("Forbidden");
  _lst[ QObject::WhatsThisCursor    ] = QString("What's this");
}

CursorEditorItem::CursorEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  _lst[ QObject::ArrowCursor        ] = QString("Arrow");
  _lst[ QObject::UpArrowCursor      ] = QString("Up Arrow");
  _lst[ QObject::CrossCursor        ] = QString("Cross");
  _lst[ QObject::WaitCursor         ] = QString("Waiting");
  _lst[ QObject::IbeamCursor        ] = QString("iBeam");
  _lst[ QObject::SizeVerCursor      ] = QString("Size Vertical");
  _lst[ QObject::SizeHorCursor      ] = QString("Size Horizontal");
  _lst[ QObject::SizeBDiagCursor    ] = QString("Size Slash");
  _lst[ QObject::SizeFDiagCursor    ] = QString("Size Backslash");
  _lst[ QObject::SizeAllCursor      ] = QString("Size All");
  _lst[ QObject::BlankCursor        ] = QString("Blank");
  _lst[ QObject::SplitVCursor       ] = QString("Split Vertical");
  _lst[ QObject::SplitHCursor       ] = QString("Split Horizontal");
  _lst[ QObject::PointingHandCursor ] = QString("Pointing Hand");
  _lst[ QObject::ForbiddenCursor    ] = QString("Forbidden");
  _lst[ QObject::WhatsThisCursor    ] = QString("What's this");

  setText( 0, text);
  setText( 1, _lst[value.toCursor().shape()]);
}

QWidget* CursorEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QComboBox* editor = new QComboBox( parent, "CursorEditorItem::combo" );
  editor->setEditable(false);
  connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );
  for ( QMapConstIterator<int, QString> it = _lst.begin(); it != _lst.end(); ++it )
  {
    editor->insertItem(it.data(), it.key());
  }

  editor->setCurrentItem(overrideValue().toCursor().shape());

  return editor;
}

void CursorEditorItem::stopEdit( QWidget* editor, int column )
{
  QVariant var;
  var.asCursor().setShape( dynamic_cast<QComboBox*>(editor)->currentItem() );
  setOverrideValue( var );
  setText( column, _lst[overrideValue().toCursor().shape()] );
}

void CursorEditorItem::setDefaultValue()
{
  QComboBox* combo = dynamic_cast<QComboBox*>(_editor);
  combo->setCurrentItem(value().toCursor().shape());
}

void CursorEditorItem::convertFromProperty(App::Property*)
{
}

void CursorEditorItem::convertToProperty(const QVariant&)
{
}

