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


#ifndef PROPERTYEDITORITEM_H
#define PROPERTYEDITORITEM_H

#ifndef _PreComp_
# include <qlistview.h>
# include <qvariant.h>
# include <qvector.h>
#endif

#include <Base/Type.h>
#include <App/Property.h>

class QPushButton;

namespace Gui {
namespace PropertyEditor {

/** Implementation of a property editor similar to this one of Qt designer.
 *
 * EditableItem is an abstract base class for all items with an editor inside.
 * The implementation is based on the sources of the book "Practical Qt".
 *
 * \author Werner Mayer
 */
class GuiExport EditableItem :public QObject, public QListViewItem, public Base::BaseClass
{
  TYPESYSTEM_HEADER();

  Q_OBJECT

public:
  /** Constructs an property item. */
  EditableItem( QListView* lv, const QVariant& value );

  /** Sets the default value for this item to \a value. */
  void setValue( const QVariant& value );

  /** Gets the default value of this item. */
  const QVariant& value() const;

  /** Sets the override value to \a value. */
  void setOverrideValue( const QVariant& value );

  /** Gets the override value. */
  const QVariant& overrideValue() const;

  /** Sets the current property object. */
  void setProperty( App::Property* );

  /** This is the class Id to distinguish from QListViewItem itself or 
   * from other QListViewItem-subclasses. 
   */
  int rtti () const { return 4000; }

  /** 
   * If \a edit is true, this item can be edited, 
   * otherwise it cannot be edited. 
   */
  void setEditable( bool edit );

  /** Returns true if the item is editable, otherwise returns false. */
  bool isEditable() const;

public slots:
  /** Undoes the all setOverrideValue(). */
  void restoreOverrideValue();

  /** 
   * If \a modify is true, this item is set to be modified, 
   * otherwise it is set to be not modified. 
   */
  void setModified( bool modify );

  /** Returns true if the item was modified, otherwise returns false. */
  bool isModified() const;

protected slots:
  /**
   * Sets the item to be modified and does a repaint of itself.
   * You should connect this slot with an appropriate signal of your editor widget.
   * \code
   * class MyPropertyItem : public EditableItem
   * {
   *   MyPropertyItem( QListView* lv, const QVariant& value )
   *    : EditableItem( lv, value )
   *   {
   *     QComboBox* editor = new QComboBox( parent, "MyPropertyItem::combo" );
   *
   *     connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );
   *     ...
   *   }
   *   
   *   ...
   * }
   * \endcode
   */
  void onValueChanged();

private: // Interface for EditableListView
  friend class EditableListView;
  bool startEdit( int column );
  void stopEdit();
  void update();
  void setup();

protected: // Interface for subclasses.
  /** Creates the appropriate editor for this item. */
  virtual QWidget* createEditor( int column, QWidget* parent ) = 0;
  /** Sets the new value for this item. Afterwards the editor is deleted automatically. */
  virtual void stopEdit( QWidget* editor, int column ) = 0;
  /** Sets the editor's default value. */
  virtual void setDefaultValue() = 0;

  /** @name Converter methods */
  //@{
  /** Converts from property to QVariant. */
  virtual void convertFromProperty(App::Property*) = 0;
  /** Converts from QVariant back to property. */
  virtual void convertToProperty(const QVariant&) = 0;
  //@}

  /** Does a repaint of the cell. */
  virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align);

  QWidget* _editor; /**< A pointer to the actual editor widget. */

protected:
  EditableItem();

public:
  static QListView* parentView;

private:
  App::Property* _prop;
  QVariant _val;
  QVariant _newval;
  bool _modified;
  bool _editable;
  int _col;
  QPushButton* _reset;
};

} // namespace PropertyEditor
} // namespace Gui

#endif // PROPERTYEDITORITEM_H
