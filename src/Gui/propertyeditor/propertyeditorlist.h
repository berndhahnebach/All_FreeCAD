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


#ifndef PROPERTYEDITORLIST_H
#define PROPERTYEDITORLIST_H

#include "propertyeditoritem.h"


namespace Gui {
namespace PropertyEditor {

/**
 * Edit properties of boolean type. 
 * \author Werner Mayer
 */
class GuiExport BoolEditorItem: public EditableItem
{
public:
  BoolEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();
};

/**
 * Select the value for a property in a list.. 
 * \author Werner Mayer
 */
class GuiExport ListEditorItem : public EditableItem
{
public:
  ListEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();
};

/**
 * Select a cursor.. 
 * \author Werner Mayer
 */
class GuiExport CursorEditorItem : public EditableItem
{
public:
  CursorEditorItem( QListView* lv, const QString& text, const QVariant& value );

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( QWidget* editor, int column );
  virtual void setDefaultValue();

private:
  QMap<int, QString> _lst;
};

} //namespace PropertyEditor
} //namespace Gui


#endif //PROPERTYEDITORLIST_H
