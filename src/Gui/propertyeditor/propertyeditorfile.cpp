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
    + use QFileDialog instead of KFileDialog
    + comment out use of other KDE classes
*/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qlineedit.h>
# include <qpushbutton.h>
# include <qfiledialog.h>
# include <qstring.h>
# include <qpixmap.h>
# include <qvariant.h>
# include <qevent.h>
# include <qlabel.h>
# include <qcursor.h>
#endif

//#include <klocale.h>
//#include <kdebug.h>

#include "Tools.h"
#include "propertyeditorfile.h"
#include "kexiproperty.h"

using namespace Gui::Kexi;

PropertyEditorFile::PropertyEditorFile(QWidget *parent, Property *property, const char *name)
    : PropertySubEditor(parent, property, name)
{
  m_lineedit = new QLineEdit(this);
  m_lineedit->resize(width(), height()-2);

  m_button = new QPushButton(tr(" ... "), this);
  m_button->resize(height(), height()-11);
  m_button->move(width() - m_button->width() -1, 1);

  m_lineedit->setText(property->value().toString());
  m_lineedit->show();
  m_button->show();

  setWidget(m_lineedit);

  connect(m_button, SIGNAL(clicked()), this, SLOT(selectFile()));
}

QVariant
PropertyEditorFile::value()
{
  return QVariant(m_url.path());
}

void
PropertyEditorFile::setValue(const QVariant &value)
{
  m_url = value.toString();
  m_lineedit->setText(m_url.fileName());
}


void
PropertyEditorFile::selectFile()
{
  QString url = QFileDialog::getOpenFileName(QString::null, m_filter, this, tr("Choose a file"));
  if ( !url.isEmpty() )
  {
    m_url = url;
    m_lineedit->setText(m_url.fileName());
    emit changed(this);
  }
}

void
PropertyEditorFile::setFilter(QString filter, bool add)
{
  if(add)
    m_filter += filter;
  else
    m_filter = filter;
}

void
PropertyEditorFile::resizeEvent(QResizeEvent *ev)
{
  m_lineedit->resize(ev->size());
  m_button->move(ev->size().width() - m_button->width()-1, 1);
}

bool
PropertyEditorFile::eventFilter(QObject* watched, QEvent* e)
{
  if(e->type() == QEvent::KeyPress)
  {
    QKeyEvent* ev = static_cast<QKeyEvent*>(e);
    if((ev->key() == Key_Enter) || (ev->key()== Key_Space) || (ev->key() == Key_Return))
    {
      m_button->animateClick();
      return true;
    }
  }
  return PropertySubEditor::eventFilter(watched, e);
}

//PIXMAP

PropertyEditorPixmap::PropertyEditorPixmap(QWidget *parent, Property *property, const char *name)
    : PropertySubEditor(parent, property, name)
{
  m_label = new QLabel(this);
  m_label->setPixmap(property->value().toPixmap());
  m_label->setAlignment(Qt::AlignTop);
  m_label->resize(width(), height()-1);
  m_label->setBackgroundMode(Qt::PaletteBase);
  m_label->show();
  setWidget(m_label);

  m_button = new QPushButton(tr(" ... "), this);
  m_button->resize(height(), height()-11);
  m_button->move(width() - m_button->width() -1, 0);
  m_button->show();

  m_popup = new QLabel(0, 0, Qt::WStyle_NoBorder|Qt::WX11BypassWM|WStyle_StaysOnTop);
  m_popup->hide();

  connect(m_button, SIGNAL(clicked()), this, SLOT(selectFile()));
}

void
PropertyEditorPixmap::resizeEvent(QResizeEvent *ev)
{
  m_label->resize(ev->size().width(), ev->size().height()-1);
  m_button->move(ev->size().width() - m_button->width(), 0);
}

bool
PropertyEditorPixmap::eventFilter(QObject *o, QEvent *ev)
{
  if(o == m_label)
  {
    if(ev->type() == QEvent::MouseButtonPress)
    {
      if(m_label->pixmap()->size().height() < height()-2
          && m_label->pixmap()->size().width() < width()-20)
        return false;
      m_popup->setPixmap(*(m_label->pixmap()));
      m_popup->resize(m_label->pixmap()->size());
      m_popup->move(QCursor::pos());
      m_popup->show();
    }
    if(ev->type() == QEvent::MouseButtonRelease)
    {
      if(m_popup->isVisible())
        m_popup->hide();
    }
    if(ev->type() == QEvent::KeyPress)
    {
      QKeyEvent* e = static_cast<QKeyEvent*>(ev);
      if((e->key() == Key_Enter) || (e->key()== Key_Space) || (e->key() == Key_Return))
      {
        m_button->animateClick();
        return true;
      }
    }
  }
  return PropertySubEditor::eventFilter(o, ev);
}

QVariant
PropertyEditorPixmap::value()
{
  return *(m_label->pixmap());
}

void
PropertyEditorPixmap::setValue(const QVariant &value)
{
  m_label->setPixmap(value.toPixmap());
  emit changed(this);
}

void
PropertyEditorPixmap::selectFile()
{
  QString url = QFileDialog::getOpenFileName(QString::null, tr("*.png *.xpm *.bmp *.jpg Pixmap Files"),
                this, tr("Choose a file"));
  if ( !url.isEmpty() )
  {
    m_url = url;
    m_label->setPixmap(QPixmap(m_url.path()));
    emit changed(this);
  }
}

PropertyEditorPixmap::~PropertyEditorPixmap()
{
  delete m_popup;
}

#include "propertyeditorfile.moc"

