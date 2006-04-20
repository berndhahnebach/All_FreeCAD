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


#ifndef PROPERTYEDITORINPUT_H
#define PROPERTYEDITORINPUT_H

#include "propertyeditoritem.h"

#include <qspinbox.h>

namespace Gui {
namespace PropertyEditor {

/**
 * Change a text property.
 * \author Werner Mayer
 */
class GuiExport TextEditorItem: public EditableItem
{
  TYPESYSTEM_HEADER();

  Q_OBJECT
public:
  TextEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();
  virtual void convertFromProperty(const std::vector<App::Property*>&);
  virtual void convertToProperty(const QVariant&);

private:
  TextEditorItem();
};

/**
 * Change a number.
 * \author Werner Mayer
 */
class GuiExport IntEditorItem: public EditableItem
{
  TYPESYSTEM_HEADER();

  Q_OBJECT
public:
  IntEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();
  virtual void convertFromProperty(const std::vector<App::Property*>&);
  virtual void convertToProperty(const QVariant&);

private:
  IntEditorItem();
};

/**
 * Change a floating point number.
 * \author Werner Mayer
 */
class GuiExport FloatEditorItem: public EditableItem
{
  TYPESYSTEM_HEADER();

  Q_OBJECT
public:
  FloatEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();
  virtual void convertFromProperty(const std::vector<App::Property*>&);
  virtual void convertToProperty(const QVariant&);

private:
  FloatEditorItem();
};

} // namespace PropertyEditor
} // namespace Gui

#endif
