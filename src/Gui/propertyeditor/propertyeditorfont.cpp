/* This file is part of the KDE project
   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/* Modifications for FreeCAD from 06-13-2004
    + include FreeCAD's PreCompiled header stuff
    + use QFontDialog instead of KFontDialog
*/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qlabel.h>
# include <qstring.h>
# include <qevent.h>
# include <qfontdialog.h>
# include <qcolordialog.h>
# include <qpushbutton.h>
#endif

//#include <klocale.h>
//#include <kfontdialog.h>
//#include <kpushbutton.h>
//#include <kcolorbutton.h>
//#include <kdebug.h>

#include "propertyeditorfont.h"
#include "kexiproperty.h"

using namespace Gui::Kexi;

//FONT CHOOSER

PropertyEditorFont::PropertyEditorFont(QWidget *parent, Property *property, const char *name)
 : PropertySubEditor(parent, property, name)
{
//  m_font = property->value().toFont();

  m_label = new QLabel(this);
  m_label->setMargin( 2 );
  m_label->resize(width(), height()-1);

  m_button = new QPushButton(" ... ", this);
  m_button->resize(height(), height()-11);
  m_button->move(width() - m_button->width(), 0);

//  m_label->setText( Property::format( m_font ) );
  setValue(property->value());
  m_label->setBackgroundMode(Qt::PaletteBase);
  m_label->show();
  m_button->show();

  setWidget(m_label);

  connect(m_button, SIGNAL(clicked()), this, SLOT(selectFont()));
}

QVariant
PropertyEditorFont::value()
{
  return QVariant(m_font);
}

void
PropertyEditorFont::setValue(const QVariant &value)
{
  m_font = value.toFont();
  m_label->setText( Property::format( m_font ) );
  emit changed(this);
}

void
PropertyEditorFont::selectFont()
{
  bool ok;
  QFont font = QFontDialog::getFont(&ok, m_font, this);

  if(ok)
  {
    setValue(font);
//    m_label->setText(m_font.family() + " " + QString::number(m_font.pointSize()));
    emit changed(this);
  }
}

void
PropertyEditorFont::resizeEvent(QResizeEvent *ev)
{
  m_label->resize(ev->size().width(), ev->size().height()-1);
  m_button->move(ev->size().width() - m_button->width(), 0);
}

bool
PropertyEditorFont::eventFilter(QObject* watched, QEvent* e)
{
  if(e->type() == QEvent::KeyPress)
  {
    QKeyEvent* ev = static_cast<QKeyEvent*>(e);
    if((ev->key() == Key_Enter) | (ev->key()== Key_Space) || (ev->key() == Key_Return))
    {
      m_button->animateClick();
      return true;
    }
  }
  return PropertySubEditor::eventFilter(watched, e);
}

//COLOR CHOOSER

PropertyEditorColor::PropertyEditorColor(QWidget *parent, Property *property, const char *name)
 : PropertySubEditor(parent, property, name)
 {
  m_label = new QLabel(this);
  m_label->setMargin( 2 );
  m_label->resize(width(), height()-1);

  m_button = new QPushButton(" ... ", this);
  m_button->resize(height(), height()-11);
  m_button->move(width() - m_button->width(), 0);

  setValue(property->value());
  m_label->show();
  m_button->show();
  setWidget(m_button);

  connect(m_button, SIGNAL(clicked()), this, SLOT(selectColor()));
 }

 
 QVariant
PropertyEditorColor::value()
{
  return QVariant(m_color);
}

void
PropertyEditorColor::setValue(const QVariant &value)
{
  m_color = value.toColor();
  m_label->setPaletteBackgroundColor( m_color );
}

void
PropertyEditorColor::selectColor()
{
  QColor c = QColorDialog::getColor(m_color, this);

  if ( c.isValid() )
  {
    setValue(c);
    emit changed(this);
  }
}

void
PropertyEditorColor::resizeEvent(QResizeEvent *ev)
{
  m_label->resize(ev->size().width() - m_button->width()-5, ev->size().height()-6);
  m_label->move(3, 3);
  m_button->move(ev->size().width() - m_button->width(), 0);
}

bool
PropertyEditorColor::eventFilter(QObject* watched, QEvent* e)
{
  if(e->type() == QEvent::KeyPress)
  {
    QKeyEvent* ev = static_cast<QKeyEvent*>(e);
    if((ev->key() == Key_Enter) | (ev->key()== Key_Space) || (ev->key() == Key_Return))
    {
      m_button->animateClick();
      return true;
    }
  }
  return PropertySubEditor::eventFilter(watched, e);
}

#include "propertyeditorfont.moc"

