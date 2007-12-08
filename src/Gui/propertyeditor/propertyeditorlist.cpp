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

#include "propertyeditorlist.h"

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
        
        ((App::PropertyBool*)*it)->setValue(val);
    }
}

QWidget* PropertyBoolItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QComboBox *cb = new QComboBox(parent);
    cb->setFrame(false);
    cb->addItem("false");
    cb->addItem("true");
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
    if ( !items.isEmpty() ) {
        QString value = items.front();
        const std::vector<App::Property*>& items = getProperty();
        for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
            assert((*it)->getTypeId().isDerivedFrom(App::PropertyEnumeration::getClassTypeId()));
            ((App::PropertyEnumeration*)*it)->setValue((const char*)value.toUtf8());
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
                    commonModes << jt->c_str();
            }
            else {
                for (std::vector<std::string>::const_iterator jt = value.begin(); jt != value.end(); ++jt) {
                    if (commonModes.contains(jt->c_str()))
                        modes << jt->c_str();
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

QWidget* PropertyStringListItem::createEditor(QWidget* parent, QObject* receiver, const char* method) const
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
        list << QString(jt->c_str());
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
















#if 0
TYPESYSTEM_SOURCE(Gui::PropertyEditor::BoolEditorItem, Gui::PropertyEditor::EditableItem);

BoolEditorItem::BoolEditorItem()
{
}

BoolEditorItem::BoolEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
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

void BoolEditorItem::stopEdit( int column )
{
  if ( overrideValue().toBool() )
    setText( column, QObject::tr("True") );
  else
    setText( column, QObject::tr("False") );
}

void BoolEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);
  if ( value().toBool() )
    combo->setCurrentItem( 0 );
  else
    combo->setCurrentItem( 1 );
}

QVariant BoolEditorItem::currentEditorValue( QWidget* editor ) const
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);
  bool cur = combo->currentItem() == 0;
  QVariant val(cur, 0);
  return val;
}

QVariant BoolEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
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
  if ( !equal )
    setText( 1, QObject::tr("") );
  else if ( value )
    setText( 1, QObject::tr("True") );
  else
    setText( 1, QObject::tr("False") );

  return val;
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

ListEditorItem::ListEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
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

  if ( !items.isEmpty() )
  {
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
  }
  else
  {
    // all the selected objects don't share a common value
    editor->setDisabled(true);
  }

  return editor;
}

void ListEditorItem::stopEdit( int column )
{
  setText( column, overrideValue().toStringList().front() );
}

void ListEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);

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

QVariant ListEditorItem::currentEditorValue( QWidget* editor ) const
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);

  QVariant var = overrideValue();
  var.asStringList().front() = combo->currentText();
  return var;
}

QVariant ListEditorItem::convertFromProperty(const std::vector<App::Property*>& props)
{
  if ( props.size() > 0 )
  {
    // We want to get all common modes of the properties and also want to keep the order of them
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
    setText( 1, commonModeList.front() );
    return value;
  }
  else
  {
    QVariant value( "" );
    setText( 1, value.toString() );
    setReadOnly(true);
    return value;
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

TYPESYSTEM_SOURCE(Gui::PropertyEditor::ComboEditorItem, Gui::PropertyEditor::EditableItem);

ComboEditorItem::ComboEditorItem()
{
}

ComboEditorItem::ComboEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text);
  setText( 1, value.toStringList().front());
}

QWidget* ComboEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QStringList items = overrideValue().toStringList();
  QComboBox* editor = new QComboBox( parent, "ComboEditorItem::combo" );
  connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );

  if ( !items.isEmpty() )
  {
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
  }
  else
  {
    // all the selected objects don't share a common value
    editor->setDisabled(true);
  }

  return editor;
}

void ComboEditorItem::stopEdit( int column )
{
  QStringList items = overrideValue().toStringList();
  if ( !items.isEmpty() )
    setText( column, items.front() );
}

void ComboEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);

  QStringList items = value().toStringList();

  if ( !items.isEmpty() )
  {
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
}

QVariant ComboEditorItem::currentEditorValue( QWidget* editor ) const
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);

  QVariant var = overrideValue();
  QStringList& items = var.asStringList();
  if ( !items.isEmpty() )
    items.front() = combo->currentText();
  return var;
}

QVariant ComboEditorItem::convertFromProperty(const std::vector<App::Property*>& props)
{
  i18n2Native.clear();
  if ( props.size() > 0 )
  {
    // We want to get all common modes of the properties and also want to keep the order of them
    // So, we need a string list therefore (not a map)
    QStringList commonModeList;
    QString current;
    for ( std::vector<App::Property*>::const_iterator it = props.begin(); it != props.end(); ++it )
    {
      if ( it == props.begin() )
      {
        App::PropertyEnumeration* pPropEnum = (App::PropertyEnumeration*)*it;
        const std::vector<std::string>& modes = pPropEnum->getEnumVector();
        for ( std::vector<std::string>::const_iterator jt = modes.begin(); jt != modes.end(); ++jt ) {
          QString i18n = QObject::tr( jt->c_str() ); // i18n stuff
          commonModeList << i18n;
          i18n2Native[ i18n ] = jt->c_str();
        }
        
        current = pPropEnum->getValueAsString();
      }
      else
      {
        QStringList modeList;
        App::PropertyEnumeration* pPropEnum = (App::PropertyEnumeration*)*it;
        const std::vector<std::string>& modes = pPropEnum->getEnumVector();
        for ( std::vector<std::string>::const_iterator jt = modes.begin(); jt != modes.end(); ++jt ){
           // i18n stuff
          QString i18n = QObject::tr( jt->c_str() );
          if ( commonModeList.find( i18n ) != commonModeList.end() )
            modeList << i18n;
        }

        // intersection of both lists
        commonModeList = modeList;
      }
    }

    // As we must store the internals as a QVariant we need to define the first element
    // in the list is the current item.
    commonModeList.prepend(current);
    QVariant value( commonModeList );
    setText( 1, current );
    return value;
  }
  else
  {
    QVariant value( "" );
    setText( 1, value.toString() );
    setReadOnly(true);
    return value;
  }
}

void ComboEditorItem::convertToProperty(const QVariant& val)
{
  QStringList items = val.toStringList();
  if ( !items.isEmpty() ) {
    QString value = i18n2Native[ items.front() ];
    for (std::vector<App::Property*>::iterator it = _prop.begin(); it != _prop.end(); ++it)
    {
      App::PropertyEnumeration* pPropEnum = (App::PropertyEnumeration*)*it;
      // Set the current active item (last item)
      pPropEnum->setValue(value.ascii());
    }
  }
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::CursorEditorItem, Gui::PropertyEditor::EditableItem);

CursorEditorItem::CursorEditorItem()
{
  _lst[ Qt::ArrowCursor        ] = QString("Arrow");
  _lst[ Qt::UpArrowCursor      ] = QString("Up Arrow");
  _lst[ Qt::CrossCursor        ] = QString("Cross");
  _lst[ Qt::WaitCursor         ] = QString("Waiting");
  _lst[ Qt::IBeamCursor        ] = QString("iBeam");
  _lst[ Qt::SizeVerCursor      ] = QString("Size Vertical");
  _lst[ Qt::SizeHorCursor      ] = QString("Size Horizontal");
  _lst[ Qt::SizeBDiagCursor    ] = QString("Size Slash");
  _lst[ Qt::SizeFDiagCursor    ] = QString("Size Backslash");
  _lst[ Qt::SizeAllCursor      ] = QString("Size All");
  _lst[ Qt::BlankCursor        ] = QString("Blank");
  _lst[ Qt::SplitVCursor       ] = QString("Split Vertical");
  _lst[ Qt::SplitHCursor       ] = QString("Split Horizontal");
  _lst[ Qt::PointingHandCursor ] = QString("Pointing Hand");
  _lst[ Qt::ForbiddenCursor    ] = QString("Forbidden");
  _lst[ Qt::WhatsThisCursor    ] = QString("What's this");
}

CursorEditorItem::CursorEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  _lst[ Qt::ArrowCursor        ] = QString("Arrow");
  _lst[ Qt::UpArrowCursor      ] = QString("Up Arrow");
  _lst[ Qt::CrossCursor        ] = QString("Cross");
  _lst[ Qt::WaitCursor         ] = QString("Waiting");
  _lst[ Qt::IBeamCursor        ] = QString("iBeam");
  _lst[ Qt::SizeVerCursor      ] = QString("Size Vertical");
  _lst[ Qt::SizeHorCursor      ] = QString("Size Horizontal");
  _lst[ Qt::SizeBDiagCursor    ] = QString("Size Slash");
  _lst[ Qt::SizeFDiagCursor    ] = QString("Size Backslash");
  _lst[ Qt::SizeAllCursor      ] = QString("Size All");
  _lst[ Qt::BlankCursor        ] = QString("Blank");
  _lst[ Qt::SplitVCursor       ] = QString("Split Vertical");
  _lst[ Qt::SplitHCursor       ] = QString("Split Horizontal");
  _lst[ Qt::PointingHandCursor ] = QString("Pointing Hand");
  _lst[ Qt::ForbiddenCursor    ] = QString("Forbidden");
  _lst[ Qt::WhatsThisCursor    ] = QString("What's this");

  setText( 0, text);
  assert(value.canConvert<QCursor>());
  QCursor sh = value.value<QCursor>();
  setText( 1, _lst[sh.shape()]);
}

QWidget* CursorEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QComboBox* editor = new QComboBox( parent, "CursorEditorItem::combo" );
  editor->setEditable(false);
  connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );
  QStringList items;
  for ( QMap<Qt::CursorShape, QString>::ConstIterator it = _lst.begin(); it != _lst.end(); ++it )
    items << it.data();
  editor->insertItems(0, items);

  assert(overrideValue().canConvert<QCursor>());
  QCursor sh = overrideValue().value<QCursor>();
  editor->setCurrentItem(sh.shape());

  return editor;
}

void CursorEditorItem::stopEdit( int column )
{
  assert(overrideValue().canConvert<QCursor>());
  QCursor sh = overrideValue().value<QCursor>();
  setText( column, _lst[sh.shape()] );
}

void CursorEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);
  assert(value().canConvert<QCursor>());
  QCursor sh = value().value<QCursor>();
  combo->setCurrentItem(sh.shape());
}

QVariant CursorEditorItem::currentEditorValue( QWidget* editor ) const
{
  int pos = dynamic_cast<QComboBox*>(editor)->currentItem();
  QMap<Qt::CursorShape, QString>::ConstIterator it = _lst.begin();
  while (pos > 0) {
    pos--;
    it++;
  }
  
  QVariant var;
  QCursor sh;
  sh.setShape(it.key());
  var.setValue<QCursor>(sh);
  return var;
}

QVariant CursorEditorItem::convertFromProperty(const std::vector<App::Property*>&)
{
  QVariant var;
  QCursor sh(Qt::ArrowCursor);
  var.setValue<QCursor>(sh);
  return var;
}

void CursorEditorItem::convertToProperty(const QVariant&)
{
}

#endif

