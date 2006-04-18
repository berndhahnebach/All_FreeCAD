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
# include <qheader.h>
# include <qpainter.h>
# include <qpushbutton.h>
#endif

#include "propertyeditoritem.h"
 
using namespace Gui::PropertyEditor;
 
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

QListView* EditableItem::parentView = 0;

EditableItem::EditableItem()
    : QListViewItem( parentView ), _val(0), _newval(0), _modified(false), _editable(true)
{
  _reset = new QPushButton(listView()->viewport());
  _reset->setPixmap( resetproperty_xpm );
  _reset->hide();

  connect( _reset, SIGNAL( clicked() ), this, SLOT( restoreOverrideValue() ) );
}

EditableItem::EditableItem( QListView* lv, const QVariant& value )
    : QListViewItem( lv ), _val(value), _newval(value), _modified(false), _editable(true)
{
  _reset = new QPushButton(listView()->viewport());
  _reset->setPixmap( resetproperty_xpm );
  _reset->hide();

  connect( _reset, SIGNAL( clicked() ), this, SLOT( restoreOverrideValue() ) );
}

void EditableItem::setup()
{
  QListViewItem::setup();
  setHeight(20);
}

bool EditableItem::startEdit( int col )
{
  if ( !_editable ) 
    return false;

  _editor = createEditor( col, listView()->viewport() );
  if ( _editor ) 
  {
    _col = col;
    update();
    _reset->show();
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
    stopEdit( _editor, _col );
    _reset->hide();
    delete _editor;
    _editor = 0;
  }
}

void EditableItem::onValueChanged()
{
  setModified( true );
  QListViewItem::repaint();
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
  _reset->setGeometry( rect2 );
  _reset->setEnabled( isModified() );
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

void EditableItem::setOverrideValue( const QVariant& val )
{
  _newval = val;
  //FIXME: This is just for testing the modification of properties
  if ( isModified() )
    convertToProperty(_newval);
}

const QVariant& EditableItem::overrideValue() const
{
  return _newval;
}

void EditableItem::restoreOverrideValue()
{
  _newval = _val;
  setDefaultValue();
  setModified( false );
  QListViewItem::repaint();
}

void EditableItem::setProperty( const std::vector<App::Property*>& prop )
{
  _prop = prop;
  convertFromProperty(_prop);
}

void EditableItem::setModified( bool mod )
{
  _modified = mod;
  _reset->setEnabled( mod );
}

bool EditableItem::isModified() const
{
  return _modified;
}

void EditableItem::setEditable( bool edit )
{
  _editable = edit;
}

bool EditableItem::isEditable() const
{
  return _editable;
}

void EditableItem::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align)
{
  bool withColor = false;
  
  for( QListViewItemIterator it( listView() ); it.current(); ++it ) 
  {
    QListViewItem* father = it.current()->QListViewItem::parent();
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
  cgcopy.setColor( QColorGroup::HighlightedText, QObject::black);
  cgcopy.setColor( QColorGroup::Text, QObject::black );

  if ( withColor )               
    cgcopy.setColor( QColorGroup::Base, QColor(250, 248, 235) );
  else
    cgcopy.setColor( QColorGroup::Base, cg.base() );

  QListViewItem::paintCell(p, cgcopy, column, width, align);

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

