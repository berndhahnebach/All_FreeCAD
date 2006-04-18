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

void BoolEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  bool value = true;
  bool equal = true;
  for ( std::vector<App::Property*>::const_iterator it = prop.begin(); it != prop.end(); ++it )
  {
    App::PropertyBool* pPropBool = (App::PropertyBool*)*it;
    if ( it == prop.begin() )
      value = pPropBool->getValue();
    else 
      equal &= ( pPropBool->getValue() == value );
  }

  QVariant val( value, 0 );
  setValue( val );
  if ( !equal )
    setText( 1, QObject::tr("") );
  else if ( value )
    setText( 1, QObject::tr("True") );
  else
    setText( 1, QObject::tr("False") );
}

void BoolEditorItem::convertToProperty(const QVariant& val)
{
  bool value = val.toBool();
  for (std::vector<App::Property*>::iterator it = _prop.begin(); it != _prop.end(); ++it)
  {
    App::PropertyBool* pPropBool = (App::PropertyBool*)*it;
    pPropBool->setValue( value );
  }
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
  setText( 1, value.toStringList().front());
}

QWidget* ListEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QStringList items = overrideValue().toStringList();
  QComboBox* editor = new QComboBox( parent, "ListEditorItem::combo" );
  connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );

  QString txt = items.front();
  items.pop_front();
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
  var.asStringList().front() = combo->currentText();
  setOverrideValue( var );
  setText( column, overrideValue().toStringList().front() );
}

void ListEditorItem::setDefaultValue()
{
  QComboBox* combo = dynamic_cast<QComboBox*>(_editor);

  QStringList items = value().toStringList();

  QString txt = items.front();
  items.pop_front();

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

void ListEditorItem::convertFromProperty(const std::vector<App::Property*>& props)
{
  if ( props.size() > 0 )
  {
    // We want to get all common modes of the properties and also want to prevent the order of them
    // So, we need a string list therefore (not a map)
    QStringList commonModeList;
    for ( std::vector<App::Property*>::const_iterator it = props.begin(); it != props.end(); ++it )
    {
      if ( it == props.begin() )
      {
        App::PropertyStringList* pPropList = (App::PropertyStringList*)*it;
        const std::vector<std::string>& modes = pPropList->getValues();
        for ( std::vector<std::string>::const_iterator jt = modes.begin(); jt != modes.end(); ++jt )
          commonModeList << jt->c_str();
      }
      else
      {
        QStringList modeList;
        App::PropertyStringList* pPropList = (App::PropertyStringList*)*it;
        const std::vector<std::string>& modes = pPropList->getValues();
        for ( std::vector<std::string>::const_iterator jt = modes.begin(); jt != modes.end(); ++jt ){
          if ( commonModeList.find(jt->c_str()) != commonModeList.end() )
            modeList << jt->c_str();
        }

        // intersection of both lists
        commonModeList = modeList;
      }
    }

    // We don't know which item to use as default, just use the first and append
    // it to the end (which is reagarded as default)
    //commonModeList << commonModeList.front();
    QVariant value( commonModeList );
    setValue( value );
    setText( 1, commonModeList.front() );
  }
  else
  {
    QVariant value( "" );
    setValue( value );
    setText( 1, value.toString() );
    setEditable(false);
  }
}

void ListEditorItem::convertToProperty(const QVariant& val)
{
  QString value = val.toStringList().front();
  for (std::vector<App::Property*>::iterator it = _prop.begin(); it != _prop.end(); ++it)
  {
    App::PropertyStringList* pPropList = (App::PropertyStringList*)*it;
    // Set the current active item (last item)
    pPropList->setValue(value.ascii());
  }
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

void CursorEditorItem::convertFromProperty(const std::vector<App::Property*>&)
{
}

void CursorEditorItem::convertToProperty(const QVariant&)
{
}

