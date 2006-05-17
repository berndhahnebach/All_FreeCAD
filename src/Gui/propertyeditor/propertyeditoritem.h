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

#include <qlistview.h>
#include <qvariant.h>
#include <vector>

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

  /** Gets the default value of this item. */
  const QVariant& value() const;

  /** Gets the overridden value. */
  const QVariant& overrideValue() const;

  /** Sets the current property object. */
  void setProperty( const std::vector<App::Property*>& );

  /** This is the class Id to distinguish from QListViewItem itself or 
   * from other QListViewItem-subclasses. 
   */
  int rtti () const { return 4000; }

  /** 
   * If \a read is true, this item cannot be edited, otherwise it can be edited. 
   */
  void setReadOnly( bool read );

  /** Returns true if the item is editable, otherwise returns false. */
  bool isReadOnly() const;

  /** Returns true if the item was modified, otherwise returns false. */
  bool isModified() const;

public slots:
  /** Undoes the changes performed by setOverrideValue(). */
  void restoreOverrideValue();
  /** Applies the changes performed by setOverrideValue(). */
  void applyOverrideValue();

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

protected: // Interface for subclasses.
  EditableItem();
  /** Creates the appropriate editor for this item and sets the editor to the value of overrideValue(). */
  virtual QWidget* createEditor( int column, QWidget* parent ) = 0;
  /** Sets the new value of overrideValue() for this item. Afterwards the editor is deleted automatically. */
  virtual void stopEdit( int column ) = 0;
  /** Sets the editor's default value of value(). */
  virtual void setDefaultEditorValue( QWidget* editor ) = 0;
  /** Gets the current overridden value. */
  virtual QVariant currentEditorValue( QWidget* editor ) const;

  /** @name Converter methods */
  //@{
  /** Converts from property to QVariant. */
  virtual QVariant convertFromProperty(const std::vector<App::Property*>&) = 0;
  /** Converts from QVariant back to property. */
  virtual void convertToProperty(const QVariant&) = 0;
  //@}

  /** Does a repaint of the cell. */
  virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align);

public:
  static QListView* parentView;

protected:
  std::vector<App::Property*> _prop;

private: // Interface for EditableListView
  friend class EditableListView;

  /** Sets the default value for this item to \a value. */
  void setValue( const QVariant& value );

  /** Sets the override value to \a value. */
  void setOverrideValue( const QVariant& value );

  /** 
   * If \a modify is true, this item is set to be modified, 
   * otherwise it is set to be not modified. 
   */
  void setModified( bool modify );

  bool startEdit( int column );
  void stopEdit();
  void update();
  void setup();

private:
//  std::vector<App::Property*> _prop;
  QVariant _val;
  QVariant _newval;
  bool _modified;
  bool _readonly;
  int _col;
  QWidget* _editor; /**< A pointer to the actual editor widget. */
  QPushButton* _apply;
};

} // namespace PropertyEditor
} // namespace Gui

#endif // PROPERTYEDITORITEM_H
