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


#ifndef PROPERTYEDITORFONT_H
#define PROPERTYEDITORFONT_H

#include "propertyeditoritem.h"

#include <qpushbutton.h>


namespace Gui {
namespace PropertyEditor {

/**
 * Change a font property.
 * \author Werner Mayer
 */
class GuiExport FontEditorItem: public EditableItem
{
  TYPESYSTEM_HEADER();

  Q_OBJECT 

public:
  FontEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected slots:
  void onChangeFont();

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();
  virtual void convertFromProperty(const std::vector<App::Property*>&);
  virtual void convertToProperty(const QVariant&);

private:
  FontEditorItem();
};

/**
 * Change a color property.
 * \author Werner Mayer
 */
class GuiExport ColorEditorItem: public EditableItem
{
  TYPESYSTEM_HEADER();

  Q_OBJECT 

public:
  ColorEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected slots:
  void onChangeColor();

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();
  /** Paints the current color with a frame outside. */
  void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align);
  virtual void convertFromProperty(const std::vector<App::Property*>&);
  virtual void convertToProperty(const QVariant&);

private:
  ColorEditorItem();

private:
  QColor _color;
};

} //namespace PropertyEditor
} //namespace Gui

#endif
