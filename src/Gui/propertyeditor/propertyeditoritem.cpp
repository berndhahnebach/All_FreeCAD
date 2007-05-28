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
# include <q3header.h>
# include <qpainter.h>
# include <qpushbutton.h>
#endif

#include "propertyeditoritem.h"

using namespace Gui::PropertyEditor;

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyItem, Base::BaseClass);

PropertyItem::PropertyItem() : parentItem(0)
{
}

PropertyItem::~PropertyItem()
{
    qDeleteAll(childItems);
}

void PropertyItem::reset()
{
    qDeleteAll(childItems);
    childItems.clear();
}

void PropertyItem::setProperty( const std::vector<App::Property*>& items)
{
    propertyItems = items;
}

const std::vector<App::Property*>& PropertyItem::getProperty() const
{
    return propertyItems;
}

void PropertyItem::setParent(PropertyItem* parent)
{
    parentItem = parent;
}

void PropertyItem::appendChild(PropertyItem *item)
{
    childItems.append(item);
}

PropertyItem *PropertyItem::child(int row)
{
    return childItems.value(row);
}

int PropertyItem::childCount() const
{
    return childItems.count();
}

int PropertyItem::columnCount() const
{
    return 2;
}

QVariant PropertyItem::pixmapData(const App::Property* prop) const
{
    return QVariant();
}

QVariant PropertyItem::displayData(const App::Property* prop) const
{
    return propertyData(prop);
}

QVariant PropertyItem::propertyData(const App::Property* prop) const
{
    return QVariant();
}

void PropertyItem::setPropertyData(const QVariant& value)
{
}

QWidget* PropertyItem::createEditor(QWidget* parent) const
{
    return 0;
}

void PropertyItem::setEditorData(QWidget *editor, const QVariant& data) const
{
}

QVariant PropertyItem::editorData(QWidget *editor) const
{
    return QVariant();
}

QVariant PropertyItem::data(int column, int role) const
{
    // no properties set
    if (propertyItems.empty())
        return QVariant();
    
    // property name
    if (column == 0) {
        if (role == Qt::DisplayRole)
            return QVariant(QString(propertyItems[0]->getName()));
        else
            return QVariant();
    } else {
        if (role == Qt::EditRole)
            return propertyData(propertyItems[0]);
        else if (role == Qt::DecorationRole)
            return pixmapData(propertyItems[0]);
        else if (role == Qt::DisplayRole)
            return displayData(propertyItems[0]);
        else
            return QVariant();
    }
}

bool PropertyItem::setData (const QVariant& value)
{
    if (propertyItems.empty())
        return false;
    setPropertyData(value);
    return true;
}

Qt::ItemFlags PropertyItem::flags(int column) const
{
    Qt::ItemFlags basicFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (column == 1)
        return basicFlags | Qt::ItemIsEditable;
    else
        return basicFlags;
}

PropertyItem *PropertyItem::parent()
{
    return parentItem;
}

int PropertyItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<PropertyItem*>(this));

    return 0;
}

// --------------------------------------------------------------------

PropertyItemEditorFactory::PropertyItemEditorFactory()
{
}

PropertyItemEditorFactory::~PropertyItemEditorFactory()
{
}

QWidget * PropertyItemEditorFactory::createEditor ( QVariant::Type type, QWidget * parent ) const
{
    // do not allow to create any editor widgets because we do that in subclasses of PropertyItem
    return 0;
}

QByteArray PropertyItemEditorFactory::valuePropertyName ( QVariant::Type type ) const
{
    // do not allow to set properties because we do that in subclasses of PropertyItem
    return "";
}

// --------------------------------------------------------------------

PropertyItemDelegate::PropertyItemDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

PropertyItemDelegate::~PropertyItemDelegate()
{
}

QWidget * PropertyItemDelegate::createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    if (!index.isValid())
        return 0;

    PropertyItem *childItem = static_cast<PropertyItem*>(index.internalPointer());
    return childItem->createEditor(parent);
}

void PropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::EditRole);
    PropertyItem *childItem = static_cast<PropertyItem*>(index.internalPointer());
    childItem->setEditorData(editor, data);
    return;
}

void PropertyItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    PropertyItem *childItem = static_cast<PropertyItem*>(index.internalPointer());
    QVariant data = childItem->editorData(editor);
    model->setData(index, data, Qt::EditRole);
}


















///* XPM */
//static const char * applyproperty_xpm[] = {
//"14 14 3 1",
//"# c #000000",
//"b c #00FF00",
//". c None",
//"..............",
//"..............",
//"..........#...",
//".........#b#..",
//"........#bbb#.",
//"...#...#bbb#..",
//"..#b#.#bbb#...",
//".#bbb#bbb#....",
//"..#bbbbb#.....",
//"...#bbb#......",
//"....#b#.......",
//".....#........",
//"..............",
//".............."};
 
/* XPM */
static const char * resetproperty_xpm[] = {
      "7 6 4 1",
      "   c None",
      ".  c #C00000",
      "+  c #FF0000",
      "@  c None",
      ".+@@@+.",
      "@.+@+.@",
      "@@.+.@@",
      "@@+.+@@",
      "@+.@.+@",
      "+.@@@.+"};

TYPESYSTEM_SOURCE_ABSTRACT(Gui::PropertyEditor::EditableItem, Base::BaseClass);

Q3ListView* EditableItem::parentView = 0;

EditableItem::EditableItem()
    : Q3ListViewItem( parentView, parentView->lastItem() ), _val(0), _newval(0), _modified(false), _readonly(false), _col(1), _editor(0)
{
  _apply = new QPushButton(listView()->viewport());
  _apply->setPixmap( resetproperty_xpm );
  _apply->hide();

  connect( _apply, SIGNAL( clicked() ), this, SLOT( restoreOverrideValue() ) );
}

EditableItem::EditableItem( Q3ListView* lv, const QVariant& value )
    : Q3ListViewItem( lv ), _val(value), _newval(value), _modified(false), _readonly(false), _col(1), _editor(0)
{
  _apply = new QPushButton(listView()->viewport());
  _apply->setPixmap( resetproperty_xpm );
  _apply->hide();

  connect( _apply, SIGNAL( clicked() ), this, SLOT( restoreOverrideValue() ) );
}

EditableItem::~EditableItem()
{
  delete _apply;
}

void EditableItem::setup()
{
  Q3ListViewItem::setup();
  setHeight(20);
}

bool EditableItem::startEdit( int col )
{
  if ( _readonly ) 
    return false;

  _editor = createEditor( col, listView()->viewport() );
  if ( _editor ) 
  {
    _col = col;
    update();
    _apply->show();
    _editor->show();
    return true;
  }
  else
    return false;
}

void EditableItem::stopEdit()
{
  if ( _editor )
  {
    QVariant var = currentEditorValue( _editor );
    setOverrideValue( var );
    stopEdit( _col );
    _apply->hide();
    delete _editor;
    _editor = 0;
  }
}

void EditableItem::onValueChanged()
{
  setModified( true );
  Q3ListViewItem::repaint();

  // do immediately
  QVariant var = currentEditorValue( _editor );
  convertToProperty(var);
}

void EditableItem::update()
{
  QRect rect = listView()->itemRect( this );
  rect.setLeft( listView()->header()->sectionPos(_col)
              - listView()->header()->offset());
  rect.setWidth( listView()->header()->sectionSize(_col)-rect.height() );
  if ( _editor )
    _editor->setGeometry( rect );

  QRect rect2 = rect;
  rect2.setLeft( listView()->header()->sectionPos(_col)
                - listView()->header()->offset() + rect.width());
  rect2.setWidth( rect.height() );
  _apply->setGeometry( rect2 );
  _apply->setEnabled( isModified() );
}

void EditableItem::setValue( const QVariant& val )
{
  _val = val;
  _newval = val;
}

const QVariant& EditableItem::value() const
{
  return _val;
}

QVariant EditableItem::currentEditorValue( QWidget* editor ) const
{
  return _newval;
}

void EditableItem::setOverrideValue( const QVariant& val )
{
  _newval = val;
}

const QVariant& EditableItem::overrideValue() const
{
  return _newval;
}

void EditableItem::restoreOverrideValue()
{
  _newval = _val;
  if ( _editor ) {
    setDefaultEditorValue( _editor );
  }
  setModified( false );
  Q3ListViewItem::repaint();

  // do immediately
  QVariant var = currentEditorValue( _editor );
  convertToProperty(var);
}

void EditableItem::applyOverrideValue()
{
  QVariant var = currentEditorValue( _editor );
  setValue( var );
  convertToProperty(var);
  setModified( false );
  Q3ListViewItem::repaint();
}

void EditableItem::setProperty( const std::vector<App::Property*>& prop )
{
  _prop = prop;
  QVariant var = convertFromProperty(_prop);
  setValue(var);
}

void EditableItem::setModified( bool mod )
{
  _modified = mod;
  _apply->setEnabled( mod );
}

bool EditableItem::isModified() const
{
  return _modified;
}

void EditableItem::setReadOnly( bool read )
{
  _readonly = read;
}

bool EditableItem::isReadOnly() const
{
  return _readonly;
}

void EditableItem::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align)
{
  bool withColor = false;
  
  for( Q3ListViewItemIterator it( listView() ); it.current(); ++it ) 
  {
    Q3ListViewItem* father = it.current()->Q3ListViewItem::parent();
    if ( father )
    {
      bool open = father->isOpen();
      while ( (father=father->parent()) != 0 && open )
        open = father->isOpen();
      if ( !open )
        continue;
    }

    if ( it.current() == this ) 
    {
      break;
    }
    
    withColor = !withColor;
  }

  if ( isModified() && column == 0 ) 
  {
    p->save();
    QFont f = p->font();
    f.setBold( TRUE );
    p->setFont( f );
  }

  QColorGroup cgcopy(cg);
  cgcopy.setColor( QColorGroup::Highlight, QColor(230, 230, 230));
  cgcopy.setColor( QColorGroup::HighlightedText, Qt::black);
  cgcopy.setColor( QColorGroup::Text, Qt::black );

  if ( withColor )               
    cgcopy.setColor( QColorGroup::Base, QColor(250, 248, 235) );
  else
    cgcopy.setColor( QColorGroup::Base, cg.base() );

  Q3ListViewItem::paintCell(p, cgcopy, column, width, align);

  if ( isModified() && column == 0 ) 
  {
    p->restore();
  }

  p->save();
  p->setPen( QPen( cg.dark(), 1 ) );
  p->drawLine( 0, height() - 1, width, height() - 1 );
  p->drawLine( width - 1, 0, width - 1, height() );
  p->restore();
}


#include "moc_propertyeditoritem.cpp"

