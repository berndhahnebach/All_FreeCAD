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

#include "propertyeditorinput.h"

using namespace Gui::PropertyEditor;

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

// ======================================================================

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

// ======================================================================

FloatSpinBox::FloatSpinBox( int min, int max, float step, int digits, QWidget *parent, const char *name )
  : QSpinBox( (int) (min*pow(digits,10)), (int) (max*pow(digits,10)),
    (int) (step*pow(digits,10)), parent, name ), _digits( digits )
{
  setValue( (int) (min*pow(digits,10)) );
  delete validator();
  QDoubleValidator* validator = new QDoubleValidator( min,  max, _digits, this );
  setValidator( validator );
}

void FloatSpinBox::setValue( double value )
{
  QSpinBox::setValue((int)(value*pow(_digits,10)));
}

double FloatSpinBox::value() const
{
  QString txt = cleanText();
  return txt.toFloat();
}

QString FloatSpinBox::mapValueToText( int value ) 
{
  QString format = QString("%.%1f").arg( _digits );
  return QString().sprintf(format.latin1(), (value/(float)pow(_digits,10)) );
}

int FloatSpinBox::mapTextToValue( bool* ok ) 
{
  return int(cleanText().toFloat(ok)*pow(_digits,10));
}

void FloatSpinBox::onSetPrecision()
{
  bool ok;
  int dec = QInputDialog::getInteger(tr("Precision"), tr("Set the number of decimals."), 2, 1, 5, 1, &ok);

  if ( ok )
  {
    ((QDoubleValidator*)validator())->setDecimals( dec );
  }
}

void FloatSpinBox::contextMenuEvent ( QContextMenuEvent * e )
{
  QPopupMenu menu;
  menu.insertItem (tr( "Precision" ), this,  SLOT(onSetPrecision())); 
  menu.exec(QCursor::pos());
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

  FloatSpinBox* editor = new FloatSpinBox( 0 /*min*/, 12 /*max*/, 0.05f /*step*/, 3 /*digits*/,
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

#include "moc_propertyeditorinput.cpp"
