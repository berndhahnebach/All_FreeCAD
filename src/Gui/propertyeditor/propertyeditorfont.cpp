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
# include <qcolordialog.h>
# include <qfontdialog.h>
# include <qpainter.h>
#endif

#include "propertyeditorfont.h"

using namespace Gui::PropertyEditor;

FontEditorItem::FontEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  setText( 1, value.toFont().family() );
}

QWidget* FontEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QPushButton* editor = new QPushButton( parent, "FontEditorItem::edit" );
  editor->setText( overrideValue().toFont().family() );
  connect(editor, SIGNAL(clicked()), this, SLOT(onChangeFont()));
  return editor;
}

void FontEditorItem::stopEdit( QWidget* editor, int column )
{
  QPushButton* btn = dynamic_cast<QPushButton*>(_editor);

  QVariant var;
  var.asFont().setFamily( btn->text() );
  setOverrideValue( var );
  setText( column, overrideValue().toFont().family() );
}

void FontEditorItem::setDefaultValue()
{
  QPushButton* btn = dynamic_cast<QPushButton*>(_editor);
  btn->setText( value().toFont().family() );
}

void FontEditorItem::onChangeFont()
{
  QPushButton* btn = dynamic_cast<QPushButton*>(_editor);

  if ( btn )
  {
    bool ok;
    QFont font( btn->text() ); 
    font = QFontDialog::getFont( &ok, font, listView() );

    if ( ok ) 
    {
      onValueChanged();
      btn->setText( font.family() );
    }
  }
}

// ======================================================================

class ColorButton : public QPushButton
{
public:
  ColorButton( QWidget* parent = 0, const char* name = 0 );
  ~ColorButton();

  void setColor( const QColor& );
  QColor color() const;

protected:
  void drawButtonLabel( QPainter* );

private:
  QColor col;
};

ColorButton::ColorButton(QWidget* parent, const char* name)
  : QPushButton( parent, name )
{
}

ColorButton::~ColorButton()
{
}

void ColorButton::setColor( const QColor& c )
{
  col = c;
  update();
}

QColor ColorButton::color() const
{
  return col;
}

void ColorButton::drawButtonLabel( QPainter *paint )
{
  QColor pen = isEnabled() ? hasFocus() ? palette().active().buttonText() : palette().inactive().buttonText()
                   : palette().disabled().buttonText();
  paint->setPen( pen );
  paint->setBrush( QBrush( col ) );

  paint->drawRect( width()/4, height()/4, width()/2, height()/2 );
}

ColorEditorItem::ColorEditorItem( QListView* lv, const QString& text, const QVariant& value )
    :EditableItem( lv, value )
{
  setText( 0, text );
  _color = value.toColor();
}

QWidget* ColorEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  ColorButton* editor = new ColorButton( parent, "ColorEditorItem::edit" );

  editor->setColor( _color );

  connect(editor, SIGNAL(clicked()), this, SLOT(onChangeColor()));
  return editor;
}

void ColorEditorItem::stopEdit( QWidget* editor, int column )
{
  _color = dynamic_cast<ColorButton*>(_editor)->color();
  setOverrideValue( _color );
}

void ColorEditorItem::setDefaultValue()
{
  ColorButton* btn = dynamic_cast<ColorButton*>(_editor);
  btn->setColor( value().toColor() );
}

void ColorEditorItem::onChangeColor()
{
  ColorButton* btn = dynamic_cast<ColorButton*>(_editor);

  if ( btn )
  {
    QColor col = QColorDialog::getColor ( /*Qt::white, listView()*/ );

    if ( col.isValid() )
    {
      onValueChanged();
      btn->setColor( col );
    }
  }
}

void ColorEditorItem::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align)
{
  EditableItem::paintCell( p, cg, column, width, align );
 
  if ( column == 1 )
  {
    p->fillRect( 2, 2, width-5, height()-4, _color );
    p->save();
    p->setPen( QPen( QObject::black, 1 ) );
    p->drawRect( 2, 2, width-5, height()-4 );
    p->restore();
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();
  }
}

#include "moc_propertyeditorfont.cpp"

