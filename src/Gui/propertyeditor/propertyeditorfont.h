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


namespace Gui {
namespace PropertyEditor {

/**
 * Change a color property.
 * \author Werner Mayer
 */
class GuiExport PropertyColorItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant decoration(const App::Property*) const;
    virtual QVariant toString(const App::Property*) const;
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyColorItem();
};




#if 0
/**
 * Change a font property.
 * \author Werner Mayer
 */
class GuiExport FontEditorItem: public EditableItem
{
  TYPESYSTEM_HEADER();

  Q_OBJECT 

public:
  FontEditorItem( Q3ListView* lv, const QString& text, const QVariant& value );

protected Q_SLOTS:
  void onChangeFont();

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( int column );
  virtual void setDefaultEditorValue( QWidget* editor );
  virtual QVariant currentEditorValue( QWidget* editor ) const;
  virtual QVariant convertFromProperty(const std::vector<App::Property*>&);
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
  ColorEditorItem( Q3ListView* lv, const QString& text, const QVariant& value );

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( int column );
  virtual void setDefaultEditorValue( QWidget* editor );
  virtual QVariant currentEditorValue( QWidget* editor ) const;
  /** Paints the current color with a frame outside. */
  void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align);
  virtual QVariant convertFromProperty(const std::vector<App::Property*>&);
  virtual void convertToProperty(const QVariant&);

private:
  ColorEditorItem();

private:
  QColor _color;
};
#endif
} //namespace PropertyEditor
} //namespace Gui

#endif
