/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
    + comment out use of KDE class kDebug
*/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qiconset.h>
# include <qtoolbutton.h>
# include <qlineedit.h>
# include <qcombobox.h>
# include <limits.h>
#endif

//#include <kiconloader.h>
//#include <klineedit.h>
//#include <knuminput.h>
//#include <klocale.h>
//#include <kglobal.h>
//#include <kdebug.h>

#include "propertyeditorinput.h"
#include "kexiproperty.h"


using namespace Gui::Kexi;

PropertyEditorInput::PropertyEditorInput(QWidget *parent, Property *property, const char *name)
    : PropertySubEditor(parent, property, name)
{
  m_lineedit = new QLineEdit(this);
  m_lineedit->resize(width(), height());

  m_lineedit->setText(property->value().toString());
  m_lineedit->show();

  setWidget(m_lineedit);

  connect(m_lineedit, SIGNAL(textChanged(const QString &)), this, SLOT(slotTextChanged(const QString &)));
}

void
PropertyEditorInput::slotTextChanged(const QString &text)
{
  emit changed(this);
}

QVariant
PropertyEditorInput::value()
{
  return QVariant(m_lineedit->text());
}

void
PropertyEditorInput::setValue(const QVariant &value)
{
  m_lineedit->setText(value.toString());
}

//INT

PropIntSpinBox::PropIntSpinBox(int lower, int upper, int step, int value, int base=10, QWidget *parent=0, const char *name=0)
    : KIntSpinBox(lower, upper, step, value, base, parent, name)
{
  editor()->setAlignment(Qt::AlignLeft);
}

bool
PropIntSpinBox::eventFilter(QObject *o, QEvent *e)
{
  if(o == editor())
  {
    if(e->type() == QEvent::KeyPress)
    {
      QKeyEvent* ev = static_cast<QKeyEvent*>(e);
      if((ev->key()==Key_Up || ev->key()==Key_Down) && ev->state()!=ControlButton)
      {
        parentWidget()->eventFilter(o, e);
        return true;
      }
    }
  }

  return KIntSpinBox::eventFilter(o, e);
}


PropertyEditorSpin::PropertyEditorSpin(QWidget *parent, Property *property, const char *name)
    : PropertySubEditor(parent,property, name)
{
  //m_leaveTheSpaceForRevertButton = false;
  m_leaveTheSpaceForRevertButton = true;
  m_spinBox = new PropIntSpinBox(0,50000, 1, 0, 10, this);
  m_spinBox->resize(width(), height());
  m_spinBox->setValue(property->value().toInt());
  m_spinBox->show();

  setWidget(m_spinBox);
  connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

QVariant
PropertyEditorSpin::value()
{
  return QVariant(m_spinBox->value());
}

void
PropertyEditorSpin::setValue(const QVariant &value)
{
  m_spinBox->setValue(value.toInt());
}

void
PropertyEditorSpin::valueChange(int)
{
  emit changed(this);
}

//DOUBLE

PropDoubleSpinBox::PropDoubleSpinBox(QWidget *parent=0)
    : KDoubleSpinBox(parent)
{
  editor()->setAlignment(Qt::AlignLeft);
}

bool
PropDoubleSpinBox::eventFilter(QObject *o, QEvent *e)
{
  if(o == editor())
  {
    if(e->type() == QEvent::KeyPress)
    {
      QKeyEvent* ev = static_cast<QKeyEvent*>(e);
      if((ev->key()==Key_Up || ev->key()==Key_Down) && ev->state()!=ControlButton)
      {
        parentWidget()->eventFilter(o, e);
        return true;
      }
    }
  }

  return KDoubleSpinBox::eventFilter(o, e);
}

PropertyEditorDblSpin::PropertyEditorDblSpin(QWidget *parent, Property *property, const char *name)
    : PropertySubEditor(parent, property, name)
{
  //  m_leaveTheSpaceForRevertButton = false;
  m_leaveTheSpaceForRevertButton = true;
  m_spinBox = new PropDoubleSpinBox(this);
  m_spinBox->resize(width(), height());
  setValue( property->value() );
  //  m_spinBox->setValue(property->value().toDouble());
  m_spinBox->show();

  setWidget(m_spinBox);
  connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

QVariant
PropertyEditorDblSpin::value()
{
  return QVariant(m_spinBox->value());
}

void
PropertyEditorDblSpin::setValue(const QVariant &value)
{
  double v = value.toDouble();
  // FIXME: it should be possible to set range, precision. Just set maxValue to 100
  m_spinBox->setRange(0, 100);
  m_spinBox->setValue(v);
}

void
PropertyEditorDblSpin::valueChange(int)
{
  emit changed(this);
}


/*********************
 * BOOL-EDITOR       *
 *********************/

PropertyEditorBool::PropertyEditorBool(QWidget *parent, Property *property, const char *name)
    : PropertySubEditor(parent, property, name)
{
  m_toggle = new QComboBox(false, this);
  m_toggle->insertItem("False");
  m_toggle->insertItem("True");
  m_toggle->resize(width(), height());

  connect(m_toggle, SIGNAL(activated(int)), this, SLOT(setState(int)));
  if(property->value().toBool())
  {
    m_toggle->setCurrentItem(1);
  }
  else
  {
    m_toggle->setCurrentItem(0);
  }

  m_toggle->show();
  setWidget(m_toggle);
}

QVariant
PropertyEditorBool::value()
{
  return QVariant(m_toggle->currentItem() > 0, 3);
}

void
PropertyEditorBool::setValue(const QVariant &value)
{
  m_toggle->setCurrentItem(value.toBool() ? 1 : 0);
}

void
PropertyEditorBool::setState(int state)
{
  emit changed(this);
}

#include "propertyeditorinput.moc"
