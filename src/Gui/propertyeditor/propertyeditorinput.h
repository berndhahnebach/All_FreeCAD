/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
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
    + use FreeCAD's export macro GuiExport instead of KEXICORE_EXPORT
    + use QLineEdit instead of KLineEdit
    + use QComboBox for PropertyEditorBool instead of a toggable QToolButton

   Modifications for FreeCAD from 10-19-2004
    + use namespace Gui::Kexi instead of prefix
*/

#ifndef PROPERTYEDITORINPUT_H
#define PROPERTYEDITORINPUT_H


#include "knuminput.h"

#include "kexipropertysubeditor.h"

class QLineEdit;
class QComboBox;
class QToolButton;
class QEvent;

namespace Gui {
namespace Kexi {

class Property;

class GuiExport PropertyEditorInput : public PropertySubEditor
{
  Q_OBJECT

public:
  PropertyEditorInput(QWidget *parent, Property *property, const char *name=0);
  ~PropertyEditorInput() {;}

  virtual QVariant  value();
  virtual void  setValue(const QVariant &value);

protected slots:
  void  slotTextChanged(const QString &text);

protected:
  QLineEdit *m_lineedit;
};

//INT

class GuiExport PropIntSpinBox : public KIntSpinBox
{
  Q_OBJECT

public:
  PropIntSpinBox(int lower, int upper, int step, int value, int base, QWidget *parent, const char *name);
  virtual bool eventFilter(QObject *o, QEvent *e);
  ~PropIntSpinBox() {;}
};

class GuiExport PropertyEditorSpin : public PropertySubEditor
{
  Q_OBJECT

public:
  PropertyEditorSpin(QWidget *parent, Property *property, const char *name=0);
  ~PropertyEditorSpin() {;}

  virtual QVariant  value();
  virtual void  setValue(const QVariant &value);

protected slots:
  void  valueChange(int);

protected:
  KIntSpinBox *m_spinBox;
};


//DOUBLE
class GuiExport PropDoubleSpinBox : public KDoubleSpinBox
{
  Q_OBJECT

public:
  PropDoubleSpinBox(QWidget *parent);
  virtual bool eventFilter(QObject *o, QEvent *e);
  ~PropDoubleSpinBox() {;}
};

class GuiExport PropertyEditorDblSpin : public PropertySubEditor
{
  Q_OBJECT

public:
  PropertyEditorDblSpin(QWidget *parent, Property *property, const char *name=0);
  ~PropertyEditorDblSpin() {;}

  virtual QVariant  value();
  virtual void  setValue(const QVariant &value);

protected slots:
  void  valueChange(int);

protected:
  KDoubleSpinBox  *m_spinBox;
};

//BOOL EDITOR

class GuiExport PropertyEditorBool : public PropertySubEditor
{
  Q_OBJECT

public:
  PropertyEditorBool(QWidget *parent, Property *property, const char *name=0);
  ~PropertyEditorBool() {;}

  virtual QVariant  value();
  virtual void  setValue(const QVariant &value);

protected slots:
  void  setState(int state);

protected:
  QComboBox *m_toggle;
};

} // namespace Kexi
} // namespace Gui

#endif
