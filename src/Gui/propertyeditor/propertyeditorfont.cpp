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

#include <App/PropertyStandard.h>

#include "propertyeditorfont.h"
#include "Widgets.h"

using namespace Gui::PropertyEditor;


TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyColorItem, Gui::PropertyEditor::PropertyItem);

PropertyColorItem::PropertyColorItem()
{
}

QVariant PropertyColorItem::decoration(const App::Property* prop) const
{
    App::Color value = ((App::PropertyColor*)prop)->getValue();
    QColor color((int)(255.0f*value.r),(int)(255.0f*value.g),(int)(255.0f*value.b));

    int size = QApplication::style()->pixelMetric(QStyle::PM_ListViewIconSize);
    QPixmap p(size, size);
    p.fill(color);

    return QVariant(p);
}

QVariant PropertyColorItem::toString(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()));

    App::Color value = ((App::PropertyColor*)prop)->getValue();
    QString color = QString("[%1, %2, %3]").arg((int)(255.0f*value.r)).arg((int)(255.0f*value.g)).arg((int)(255.0f*value.b));
    return QVariant(color);
}

QVariant PropertyColorItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()));

    App::Color value = ((App::PropertyColor*)prop)->getValue();
    return QVariant(QColor((int)(255.0f*value.r),(int)(255.0f*value.g),(int)(255.0f*value.b)));
}

void PropertyColorItem::setValue(const QVariant& value)
{
    QColor col = value.value<QColor>();
    App::Color val;
    val.r = (float)col.red()/255.0f;
    val.g = (float)col.green()/255.0f;
    val.b = (float)col.blue()/255.0f;
    const std::vector<App::Property*>& items = getProperty();
    for (std::vector<App::Property*>::const_iterator it = items.begin(); it != items.end(); ++it) {
        assert((*it)->getTypeId().isDerivedFrom(App::PropertyColor::getClassTypeId()));
        ((App::PropertyColor*)*it)->setValue(val);
    }
}

QWidget* PropertyColorItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    Gui::ColorButton* cb = new Gui::ColorButton( parent );
    QObject::connect(cb, SIGNAL(changed()), receiver, method);
    return cb;
}

void PropertyColorItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    Gui::ColorButton *cb = qobject_cast<Gui::ColorButton*>(editor);
    QColor color = data.value<QColor>();
    cb->setColor(color);
}

QVariant PropertyColorItem::editorData(QWidget *editor) const
{
    Gui::ColorButton *cb = qobject_cast<Gui::ColorButton*>(editor);
    QVariant var;
    var.setValue(cb->color());
    return var;
}
















TYPESYSTEM_SOURCE(Gui::PropertyEditor::FontEditorItem, Gui::PropertyEditor::EditableItem);

FontEditorItem::FontEditorItem()
{
}

FontEditorItem::FontEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  assert(value.canConvert<QFont>());
  const QFont& ft = value.value<QFont>();
  setText( 1, ft.family() );
}

QWidget* FontEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QPushButton* editor = new QPushButton( parent, "FontEditorItem::edit" );
  assert(overrideValue().canConvert<QFont>());
  const QFont& ft = overrideValue().value<QFont>();
  editor->setText( ft.family() );
  connect(editor, SIGNAL(clicked()), this, SLOT(onChangeFont()));
  return editor;
}

void FontEditorItem::stopEdit( int column )
{
  assert(overrideValue().canConvert<QFont>());
  const QFont& ft = overrideValue().value<QFont>();
  setText( column, ft.family() );
}

void FontEditorItem::setDefaultEditorValue( QWidget* editor )
{
  QPushButton* btn = dynamic_cast<QPushButton*>(editor);
  assert(value().canConvert<QFont>());
  const QFont& ft = value().value<QFont>();
  btn->setText( ft.family() );
}

QVariant FontEditorItem::currentEditorValue( QWidget* editor ) const
{
  QPushButton* btn = dynamic_cast<QPushButton*>(editor);

  QVariant var;
  QFont ft( btn->text() );
  var.setValue<QFont>(ft);
  return var;
}

void FontEditorItem::onChangeFont()
{
  QPushButton* btn = (QPushButton*)sender();

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

QVariant FontEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  QVariant var;
  QFont ft("");
  var.setValue<QFont>(ft);
  return var;
}

void FontEditorItem::convertToProperty(const QVariant&)
{
}

// ======================================================================

TYPESYSTEM_SOURCE(Gui::PropertyEditor::ColorEditorItem, Gui::PropertyEditor::EditableItem);

ColorEditorItem::ColorEditorItem()
{
}

ColorEditorItem::ColorEditorItem( Q3ListView* lv, const QString& text, const QVariant& value )
    :EditableItem( lv, value )
{
  setText( 0, text );
  assert(value.canConvert<QColor>());
  _color = value.value<QColor>();
}

QWidget* ColorEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  Gui::ColorButton* editor = new Gui::ColorButton( parent );

  editor->setColor( _color );

  connect(editor, SIGNAL( changed() ), this, SLOT( onValueChanged() ));
  return editor;
}

void ColorEditorItem::stopEdit( int column )
{
  assert(overrideValue().canConvert<QColor>());
  _color = overrideValue().value<QColor>();
}

void ColorEditorItem::setDefaultEditorValue( QWidget* editor )
{
  Gui::ColorButton* btn = dynamic_cast<Gui::ColorButton*>(editor);
  assert(value().canConvert<QColor>());
  btn->setColor( value().value<QColor>() );
}

QVariant ColorEditorItem::currentEditorValue( QWidget* editor ) const
{
  Gui::ColorButton* btn = dynamic_cast<Gui::ColorButton*>(editor);

  QVariant var;
  var.setValue<QColor>( btn->color() );
  return var;
}

void ColorEditorItem::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align)
{
  EditableItem::paintCell( p, cg, column, width, align );
 
  if ( column == 1 )
  {
    p->fillRect( 2, 2, width-5, height()-4, _color );
    p->save();
    p->setPen( QPen( Qt::black, 1 ) );
    p->drawRect( 2, 2, width-5, height()-4 );
    p->restore();
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();
  }
}

QVariant ColorEditorItem::convertFromProperty(const std::vector<App::Property*>& prop)
{
  QVariant value;
  if ( prop.size() > 0 )
  {
    App::PropertyColor* pPropColor = (App::PropertyColor*)prop.front();
    App::Color col = pPropColor->getValue();
    _color.setRgb((int)(255.0f*col.r),(int)(255.0f*col.g),(int)(255.0f*col.b));
    value.setValue<QColor>( _color );
  }

  return value;
}

void ColorEditorItem::convertToProperty(const QVariant& val)
{
  assert(val.canConvert<QColor>());
  QColor col = val.value<QColor>();
  for (std::vector<App::Property*>::iterator it = _prop.begin(); it != _prop.end(); ++it)
  {
    App::PropertyColor* pPropColor = (App::PropertyColor*)*it;
    pPropColor->setValue( (float)col.red()/255.0f, (float)col.green()/255.0f, (float)col.blue()/255.0f );
  }
}

#include "moc_propertyeditorfont.cpp"


// --------------------------------------------------------------------
